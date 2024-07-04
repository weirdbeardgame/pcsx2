// SPDX-FileCopyrightText: 2002-2023 PCSX2 Dev Team
// SPDX-License-Identifier: LGPL-3.0+

#include "SymbolMap.h"

#include "common/Console.h"
#include "common/FileSystem.h"
#include "common/StringUtil.h"

#include "demangle.h"
#include "ccc/ast.h"
#include "ccc/elf.h"
#include "ccc/importer_flags.h"
#include "ccc/symbol_file.h"

SymbolGuardian R5900SymbolGuardian;
SymbolGuardian R3000SymbolGuardian;

static void error_callback(const ccc::Error& error, ccc::ErrorSeverity severity)
{
	Console.Error("Error while importing symbol table: %s", error.message.c_str());
}

SymbolGuardian::SymbolGuardian()
{
	ccc::set_custom_error_callback(error_callback);
	Clear();
}

bool SymbolGuardian::Read(std::function<void(const ccc::SymbolDatabase&)> callback) const noexcept
{
	if (m_busy)
		return false;
	m_big_symbol_lock.lock_shared();
	callback(m_database);
	m_big_symbol_lock.unlock_shared();
	return true;
}

void SymbolGuardian::ShortReadWrite(std::function<void(ccc::SymbolDatabase&)> callback) noexcept
{
	m_big_symbol_lock.lock();
	callback(m_database);
	m_big_symbol_lock.unlock();
}

void SymbolGuardian::LongReadWrite(std::function<void(ccc::SymbolDatabase&)> callback) noexcept
{
	m_busy = true;
	m_big_symbol_lock.lock();
	callback(m_database);
	m_big_symbol_lock.unlock();
	m_busy = false;
}

void SymbolGuardian::ImportElfSymbolTablesAsync(std::vector<u8> elf, std::string file_name)
{
	ccc::Result<ccc::ElfFile> parsed_elf = ccc::parse_elf_file(std::move(elf));
	if (!parsed_elf.success())
	{
		ccc::report_error(parsed_elf.error());
		return;
	}

	std::unique_ptr<ccc::SymbolFile> symbol_file = std::make_unique<ccc::ElfSymbolFile>(std::move(*parsed_elf), std::move(file_name));
	ImportSymbolTablesAsync(std::move(symbol_file));
}

void SymbolGuardian::ImportSymbolTablesAsync(std::unique_ptr<ccc::SymbolFile> symbol_file)
{
	InterruptImportThread();

	m_import_thread = std::thread([this, file = std::move(symbol_file)]() {
		LongReadWrite([&](ccc::SymbolDatabase& database) {
			ccc::Result<std::vector<std::unique_ptr<ccc::SymbolTable>>> symbol_tables = file->get_all_symbol_tables();
			if (!symbol_tables.success())
			{
				ccc::report_error(symbol_tables.error());
				return;
			}

			ccc::DemanglerFunctions demangler;
			demangler.cplus_demangle = cplus_demangle;
			demangler.cplus_demangle_opname = cplus_demangle_opname;

			u32 importer_flags = ccc::DEMANGLE_PARAMETERS | ccc::DEMANGLE_RETURN_TYPE;

			ccc::Result<ccc::ModuleHandle> module_handle = ccc::import_symbol_tables(
				database, file->name(), *symbol_tables, importer_flags, demangler, &m_interrupt_import_thread);
			if (!module_handle.success())
			{
				ccc::report_error(module_handle.error());
				return;
			}

			Console.WriteLn("Imported %d symbols.", database.symbol_count());
		});
	});
}

bool SymbolGuardian::ImportNocashSymbols(const std::string& filename)
{
	bool success = false;

	FILE* f = FileSystem::OpenCFile(filename.c_str(), "r");
	if (!f)
		return success;
	
	LongReadWrite([&](ccc::SymbolDatabase& database) {
		ccc::Result<ccc::SymbolSourceHandle> source = database.get_symbol_source("Nocash Symbols");
		if (!source.success())
			return;

		while (!feof(f))
		{
			char line[256], value[256] = {0};
			char* p = fgets(line, 256, f);
			if (p == NULL)
				break;

			u32 address;
			if (sscanf(line, "%08X %s", &address, value) != 2)
				continue;
			if (address == 0 && strcmp(value, "0") == 0)
				continue;

			if (value[0] == '.')
			{
				// data directives
				char* s = strchr(value, ':');
				if (s != NULL)
				{
					*s = 0;

					u32 size = 0;
					if (sscanf(s + 1, "%04X", &size) != 1)
						continue;

					std::unique_ptr<ccc::ast::BuiltIn> scalar_type = std::make_unique<ccc::ast::BuiltIn>();
					if (StringUtil::Strcasecmp(value, ".byt") == 0)
					{
						scalar_type->computed_size_bytes = 1;
						scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_8;
					}
					else if (StringUtil::Strcasecmp(value, ".wrd") == 0)
					{
						scalar_type->computed_size_bytes = 2;
						scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_16;
					}
					else if (StringUtil::Strcasecmp(value, ".dbl") == 0)
					{
						scalar_type->computed_size_bytes = 4;
						scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_32;
					}
					else if (StringUtil::Strcasecmp(value, ".asc") == 0)
					{
						scalar_type->computed_size_bytes = 1;
						scalar_type->bclass = ccc::ast::BuiltInClass::UNQUALIFIED_8;
					}
					else
					{
						continue;
					}

					ccc::Result<ccc::GlobalVariable*> global_variable = database.global_variables.create_symbol(
						line, *source, nullptr, address);
					if (!global_variable.success())
						return;

					if (scalar_type->computed_size_bytes == (s32)size)
					{
						(*global_variable)->set_type(std::move(scalar_type));
					}
					else
					{
						std::unique_ptr<ccc::ast::Array> array = std::make_unique<ccc::ast::Array>();
						array->computed_size_bytes = (s32)size;
						array->element_type = std::move(scalar_type);
						array->element_count = size / array->element_type->computed_size_bytes;
						(*global_variable)->set_type(std::move(array));
					}
				}
			}
			else
			{ // labels
				int size = 1;
				char* seperator = strchr(value, ',');
				if (seperator != NULL)
				{
					*seperator = 0;
					sscanf(seperator + 1, "%08X", &size);
				}

				if (size != 1)
				{
					ccc::Result<ccc::Function*> function = database.functions.create_symbol(value, *source, nullptr, address);
					if (!function.success())
						return;
				}
				else
				{
					ccc::Result<ccc::Label*> label = database.labels.create_symbol(value, *source, nullptr, address);
					if (!label.success())
						return;
				}
			}
		}

		success = true;
	});

	fclose(f);
	return success;
}

void SymbolGuardian::InterruptImportThread()
{
	m_interrupt_import_thread = true;
	if (m_import_thread.joinable())
		m_import_thread.join();
	m_interrupt_import_thread = false;
}

void SymbolGuardian::Clear()
{
	LongReadWrite([&](ccc::SymbolDatabase& database) {
		database.clear();
		m_main_elf = ccc::ModuleHandle();

		ccc::Result<ccc::SymbolSource*> source = database.symbol_sources.create_symbol("User Defined", ccc::SymbolSourceHandle());
		CCC_EXIT_IF_ERROR(source);
		m_user_defined = (*source)->handle();
	});
}

void SymbolGuardian::ClearIrxModules()
{
	LongReadWrite([&](ccc::SymbolDatabase& database) {
		std::vector<ccc::ModuleHandle> irx_modules;
		for (const ccc::Module& module : m_database.modules)
			if (module.is_irx)
				irx_modules.emplace_back(module.handle());

		for (ccc::ModuleHandle module : irx_modules)
			m_database.destroy_symbols_from_modules(module);
	});
}

bool SymbolGuardian::FunctionExistsWithStartingAddress(u32 address) const
{
	bool exists = false;
	Read([&](const ccc::SymbolDatabase& database) {
		ccc::FunctionHandle handle = database.functions.first_handle_from_starting_address(address);
		exists = handle.valid();
	});
	return exists;
}

bool SymbolGuardian::FunctionExistsThatOverlapsAddress(u32 address) const
{
	bool exists = false;
	Read([&](const ccc::SymbolDatabase& database) {
		const ccc::Function* function = database.functions.symbol_overlapping_address(address);
		exists = function != nullptr;
	});
	return exists;
}

FunctionStat SymbolGuardian::StatFunctionStartingAtAddress(u32 address) const
{
	FunctionStat stat;
	Read([&](const ccc::SymbolDatabase& database) {
		ccc::FunctionHandle handle = database.functions.first_handle_from_starting_address(address);
		const ccc::Function* function = database.functions.symbol_from_handle(handle);
		if (function)
		{
			stat.handle = function->handle();
			stat.name = function->name();
			stat.address = function->address();
			stat.size = function->size();
		}
	});
	return stat;
}

FunctionStat SymbolGuardian::StatFunctionOverlappingAddress(u32 address) const
{
	FunctionStat stat;
	Read([&](const ccc::SymbolDatabase& database) {
		const ccc::Function* function = database.functions.symbol_overlapping_address(address);
		if (function)
		{
			stat.handle = function->handle();
			stat.name = function->name();
			stat.address = function->address();
			stat.size = function->size();
		}
	});
	return stat;
}

SymbolMap R5900SymbolMap;
SymbolMap R3000SymbolMap;

void SymbolMap::SortSymbols()
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	AssignFunctionIndices();
}

void SymbolMap::Clear()
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	functions.clear();
	labels.clear();
	data.clear();
}

SymbolType SymbolMap::GetSymbolType(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	if (functions.find(address) != functions.end())
		return ST_FUNCTION;
	if (data.find(address) != data.end())
		return ST_DATA;
	return ST_NONE;
}

bool SymbolMap::GetSymbolInfo(SymbolInfo* info, u32 address, SymbolType symmask) const
{
	u32 functionAddress = INVALID_ADDRESS;
	u32 dataAddress = INVALID_ADDRESS;

	if (symmask & ST_FUNCTION)
		functionAddress = GetFunctionStart(address);

	if (symmask & ST_DATA)
		dataAddress = GetDataStart(address);

	if (functionAddress == INVALID_ADDRESS || dataAddress == INVALID_ADDRESS)
	{
		if (functionAddress != INVALID_ADDRESS)
		{
			if (info != NULL)
			{
				info->type = ST_FUNCTION;
				info->address = functionAddress;
				info->size = GetFunctionSize(functionAddress);
			}

			return true;
		}

		if (dataAddress != INVALID_ADDRESS)
		{
			if (info != NULL)
			{
				info->type = ST_DATA;
				info->address = dataAddress;
				info->size = GetDataSize(dataAddress);
			}

			return true;
		}

		return false;
	}

	// if both exist, return the function
	if (info != NULL)
	{
		info->type = ST_FUNCTION;
		info->address = functionAddress;
		info->size = GetFunctionSize(functionAddress);
	}

	return true;
}

u32 SymbolMap::GetNextSymbolAddress(u32 address, SymbolType symmask)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	const auto functionEntry = symmask & ST_FUNCTION ? functions.upper_bound(address) : functions.end();
	const auto dataEntry = symmask & ST_DATA ? data.upper_bound(address) : data.end();

	if (functionEntry == functions.end() && dataEntry == data.end())
		return INVALID_ADDRESS;

	u32 funcAddress = (functionEntry != functions.end()) ? functionEntry->first : 0xFFFFFFFF;
	u32 dataAddress = (dataEntry != data.end()) ? dataEntry->first : 0xFFFFFFFF;

	if (funcAddress <= dataAddress)
		return funcAddress;
	else
		return dataAddress;
}

std::string SymbolMap::GetDescription(unsigned int address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	std::string labelName;

	u32 funcStart = GetFunctionStart(address);
	if (funcStart != INVALID_ADDRESS)
	{
		labelName = GetLabelName(funcStart);
	}
	else
	{
		u32 dataStart = GetDataStart(address);
		if (dataStart != INVALID_ADDRESS)
			labelName = GetLabelName(dataStart);
	}

	if (!labelName.empty())
		return labelName;

	char descriptionTemp[256];
	std::snprintf(descriptionTemp, std::size(descriptionTemp), "(%08x)", address);
	return descriptionTemp;
}

std::vector<SymbolEntry> SymbolMap::GetAllSymbols(SymbolType symmask) const
{
	std::vector<SymbolEntry> result;

	if (symmask & ST_FUNCTION)
	{
		std::lock_guard<std::recursive_mutex> guard(m_lock);
		for (auto it = functions.begin(); it != functions.end(); it++)
		{
			SymbolEntry entry;
			entry.address = it->first;
			entry.size = GetFunctionSize(entry.address);
			entry.name = GetLabelName(entry.address);

			result.push_back(entry);
		}
	}

	if (symmask & ST_DATA)
	{
		std::lock_guard<std::recursive_mutex> guard(m_lock);
		for (auto it = data.begin(); it != data.end(); it++)
		{
			SymbolEntry entry;
			entry.address = it->first;
			entry.size = GetDataSize(entry.address);
			entry.name = GetLabelName(entry.address);

			result.push_back(entry);
		}
	}

	return result;
}

void SymbolMap::AddFunction(const std::string& name, u32 address, u32 size, bool noReturn)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);

	auto existing = functions.find(address);

	if (existing != functions.end())
	{
		existing->second.size = size;
	}
	else
	{
		FunctionEntry func;
		func.start = address;
		func.size = size;
		func.index = (int)functions.size();
		func.name = name;
		func.noReturn = noReturn;
		functions[address] = func;

		functions.insert(std::make_pair(address, func));
	}

	AddLabel(name, address);
}

u32 SymbolMap::GetFunctionStart(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = functions.upper_bound(address);
	if (it == functions.end())
	{
		// check last element
		auto rit = functions.rbegin();
		if (rit != functions.rend())
		{
			u32 start = rit->first;
			u32 size = rit->second.size;
			if (start <= address && start + size > address)
				return start;
		}
		// otherwise there's no function that contains this address
		return INVALID_ADDRESS;
	}

	if (it != functions.begin())
	{
		it--;
		u32 start = it->first;
		u32 size = it->second.size;
		if (start <= address && start + size > address)
			return start;
	}

	return INVALID_ADDRESS;
}

u32 SymbolMap::GetFunctionSize(u32 startAddress) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = functions.find(startAddress);
	if (it == functions.end())
		return INVALID_ADDRESS;

	return it->second.size;
}

int SymbolMap::GetFunctionNum(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	u32 start = GetFunctionStart(address);
	if (start == INVALID_ADDRESS)
		return INVALID_ADDRESS;

	auto it = functions.find(start);
	if (it == functions.end())
		return INVALID_ADDRESS;

	return it->second.index;
}

bool SymbolMap::GetFunctionNoReturn(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = functions.find(address);
	if (it == functions.end())
		return false;

	return it->second.noReturn;
}

void SymbolMap::AssignFunctionIndices()
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	int index = 0;
	auto begin = functions.lower_bound(0);
	auto end = functions.upper_bound(0xFFFFFFFF);
	for (auto it = begin; it != end; ++it)
	{
		it->second.index = index++;
	}
}

bool SymbolMap::SetFunctionSize(u32 startAddress, u32 newSize)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);

	auto funcInfo = functions.find(startAddress);
	if (funcInfo != functions.end())
	{
		funcInfo->second.size = newSize;
	}

	// TODO: check for overlaps
	return true;
}

bool SymbolMap::RemoveFunction(u32 startAddress)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);

	auto it = functions.find(startAddress);
	if (it == functions.end())
		return false;

	functions.erase(it);

	labels.erase(startAddress);

	return true;
}

void SymbolMap::AddLabel(const std::string& name, u32 address)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);

	auto existing = labels.find(address);

	if (existing != labels.end())
	{
		// Adding a function will automatically call this.
		// We don't want to overwrite the name if it's already set because
		// label names are added before our functions
	}
	else
	{
		LabelEntry label;
		label.addr = address;
		label.name = name;

		labels[address] = label;
	}
}

void SymbolMap::SetLabelName(const std::string& name, u32 address)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto labelInfo = labels.find(address);
	if (labelInfo == labels.end())
	{
		AddLabel(name, address);
	}
	else
	{
		labelInfo->second.name = name;
	}
}

std::string SymbolMap::GetLabelName(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = labels.find(address);
	if (it == labels.end())
		return "";

	return it->second.name;
}

bool SymbolMap::GetLabelValue(const std::string& name, u32& dest)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	for (auto it = labels.begin(); it != labels.end(); it++)
	{
		if (name == it->second.name)
		{
			dest = it->first;
			return true;
		}
	}

	return false;
}

void SymbolMap::AddData(u32 address, u32 size, DataType type, int moduleIndex)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);

	auto existing = data.find(address);

	if (existing != data.end())
	{
		existing->second.size = size;
		existing->second.type = type;
	}
	else
	{
		DataEntry entry;
		entry.start = address;
		entry.size = size;
		entry.type = type;

		data[address] = entry;
	}
}

u32 SymbolMap::GetDataStart(u32 address) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = data.upper_bound(address);
	if (it == data.end())
	{
		// check last element
		auto rit = data.rbegin();

		if (rit != data.rend())
		{
			u32 start = rit->first;
			u32 size = rit->second.size;
			if (start <= address && start + size > address)
				return start;
		}
		// otherwise there's no data that contains this address
		return INVALID_ADDRESS;
	}

	if (it != data.begin())
	{
		it--;
		u32 start = it->first;
		u32 size = it->second.size;
		if (start <= address && start + size > address)
			return start;
	}

	return INVALID_ADDRESS;
}

u32 SymbolMap::GetDataSize(u32 startAddress) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = data.find(startAddress);
	if (it == data.end())
		return INVALID_ADDRESS;
	return it->second.size;
}

DataType SymbolMap::GetDataType(u32 startAddress) const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = data.find(startAddress);
	if (it == data.end())
		return DATATYPE_NONE;
	return it->second.type;
}

bool SymbolMap::AddModule(const std::string& name, ModuleVersion version)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	auto it = modules.find(name);
	if (it != modules.end())
	{
		for (auto [itr, end] = modules.equal_range(name); itr != end; ++itr)
		{
			// Different major versions, we treat this one as a different module
			if (itr->second.version.major != version.major)
				continue;

			// RegisterLibraryEntries will fail if the new minor ver is <= the old minor ver
			// and the major version is the same
			if (itr->second.version.minor >= version.minor)
				return false;

			// Remove the old module and its export table
			RemoveModule(name, itr->second.version);
			break;
		}
	}

	modules.insert(std::make_pair(name, ModuleEntry{name, version, {}}));
	return true;
}

void SymbolMap::AddModuleExport(const std::string& module, ModuleVersion version, const std::string& name, u32 address, u32 size)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	for (auto [itr, end] = modules.equal_range(module); itr != end; ++itr)
	{
		if (itr->second.version != version)
			continue;

		AddFunction(name, address, size);
		AddLabel(name, address);
		itr->second.exports.push_back({address, size, 0, name});
	}
}

std::vector<ModuleInfo> SymbolMap::GetModules() const
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	std::vector<ModuleInfo> result;
	for (auto& module : modules)
	{
		std::vector<SymbolEntry> exports;
		for (auto& fun : module.second.exports)
		{
			exports.push_back({fun.name, fun.start, fun.size});
		}
		result.push_back({module.second.name, module.second.version, exports});
	}
	return result;
}

void SymbolMap::RemoveModule(const std::string& name, ModuleVersion version)
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	for (auto [itr, end] = modules.equal_range(name); itr != end; ++itr)
	{
		if (itr->second.version != version)
			continue;

		for (auto& exportEntry : itr->second.exports)
		{
			RemoveFunction(exportEntry.start);
		}

		modules.erase(itr);
		break;
	}
}

void SymbolMap::ClearModules()
{
	std::lock_guard<std::recursive_mutex> guard(m_lock);
	for (auto& module : modules)
	{
		for (auto& exportEntry : module.second.exports)
		{
			RemoveFunction(exportEntry.start);
		}
	}
	modules.clear();
}
