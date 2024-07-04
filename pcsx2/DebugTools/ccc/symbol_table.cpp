// SPDX-FileCopyrightText: 2002-2023 PCSX2 Dev Team
// SPDX-License-Identifier: LGPL-3.0+

#include "symbol_table.h"

#include "elf.h"
#include "elf_symtab.h"
#include "mdebug_importer.h"
#include "mdebug_section.h"
#include "sndll.h"

namespace ccc {

const std::vector<SymbolTableFormatInfo> SYMBOL_TABLE_FORMATS = {
	{MDEBUG, "mdebug", ".mdebug"}, // The infamous Third Eye symbol table.
	{SYMTAB, "symtab", ".symtab"}, // The standard ELF symbol table.
	{SNDLL, "sndll", ".sndata"}    // The SNDLL symbol table.
};

const SymbolTableFormatInfo* symbol_table_format_from_enum(SymbolTableFormat format)
{
	for(size_t i = 0; i < SYMBOL_TABLE_FORMATS.size(); i++) {
		if(SYMBOL_TABLE_FORMATS[i].format == format) {
			return &SYMBOL_TABLE_FORMATS[i];
		}
	}
	return nullptr;
}

const SymbolTableFormatInfo* symbol_table_format_from_name(const char* format_name)
{
	for(size_t i = 0; i < SYMBOL_TABLE_FORMATS.size(); i++) {
		if(strcmp(SYMBOL_TABLE_FORMATS[i].format_name, format_name) == 0) {
			return &SYMBOL_TABLE_FORMATS[i];
		}
	}
	return nullptr;
}

const SymbolTableFormatInfo* symbol_table_format_from_section(const char* section_name)
{
	for(size_t i = 0; i < SYMBOL_TABLE_FORMATS.size(); i++) {
		if(strcmp(SYMBOL_TABLE_FORMATS[i].section_name, section_name) == 0) {
			return &SYMBOL_TABLE_FORMATS[i];
		}
	}
	return nullptr;
}

// *****************************************************************************

Result<std::unique_ptr<SymbolTable>> create_elf_symbol_table(
	const ElfSection& section, const ElfFile& elf, SymbolTableFormat format)
{
	std::unique_ptr<SymbolTable> symbol_table;
	switch(format) {
		case MDEBUG: {
			symbol_table = std::make_unique<MdebugSymbolTable>(elf.image, (s32) section.offset);
			break;
		}
		case SYMTAB: {
			CCC_CHECK(section.offset + section.size <= elf.image.size(),
				"Section '%s' out of range.", section.name.c_str());
			std::span<const u8> data = std::span(elf.image).subspan(section.offset, section.size);
			
			CCC_CHECK(section.link != 0, "Section '%s' has no linked string table.", section.name.c_str());
			CCC_CHECK(section.link < elf.sections.size(),
				"Section '%s' has out of range link field.", section.name.c_str());
			const ElfSection& linked_section = elf.sections[section.link];
			
			CCC_CHECK(linked_section.offset + linked_section.size <= elf.image.size(),
				"Linked section '%s' out of range.", linked_section.name.c_str());
			std::span<const u8> linked_data = std::span(elf.image).subspan(linked_section.offset, linked_section.size);
			
			symbol_table = std::make_unique<SymtabSymbolTable>(data, linked_data);
			break;
		}
		case SNDLL: {
			CCC_CHECK(section.offset + section.size <= elf.image.size(),
				"Section '%s' out of range.", section.name.c_str());
			std::span<const u8> data = std::span(elf.image).subspan(section.offset, section.size);
			
			if(data.size() >= 4 && data[0] != '\0') {
				Result<SNDLLFile> file = parse_sndll_file(data, section.address);
				CCC_RETURN_IF_ERROR(file);
				
				symbol_table = std::make_unique<SNDLLSymbolTable>(std::make_shared<SNDLLFile>(std::move(*file)));
			} else {
				CCC_WARN("Invalid SNDLL section.");
			}
			
			break;
		}
	}
	
	return symbol_table;
}

Result<ModuleHandle> import_symbol_tables(
	SymbolDatabase& database,
	std::string module_name,
	const std::vector<std::unique_ptr<SymbolTable>>& symbol_tables,
	u32 importer_flags,
	DemanglerFunctions demangler)
{
	Result<SymbolSourceHandle> module_source = database.get_symbol_source("Symbol Table Importer");
	CCC_RETURN_IF_ERROR(module_source);
	
	Result<Module*> module_symbol = database.modules.create_symbol(std::move(module_name), *module_source, nullptr);
	CCC_RETURN_IF_ERROR(module_symbol);
	
	for(const std::unique_ptr<SymbolTable>& symbol_table : symbol_tables) {
		// Find a symbol source object with the right name, or create one if one
		// doesn't already exist.
		Result<SymbolSourceHandle> source = database.get_symbol_source(symbol_table->name());
		if(!source.success()) {
			database.destroy_symbols_from_modules((*module_symbol)->handle());
			return source;
		}
		
		// Import the symbol table.
		Result<void> result = symbol_table->import(database, *source, *module_symbol, importer_flags, demangler);
		if(!result.success()) {
			database.destroy_symbols_from_modules((*module_symbol)->handle());
			return result;
		}
	}
	
	return (*module_symbol)->handle();
}

// *****************************************************************************

MdebugSymbolTable::MdebugSymbolTable(std::span<const u8> image, s32 section_offset)
	: m_image(image), m_section_offset(section_offset) {}

const char* MdebugSymbolTable::name() const
{
	return "MIPS Debug Symbol Table";
}

Result<void> MdebugSymbolTable::import(
	SymbolDatabase& database,
	SymbolSourceHandle source,
	const Module* module_symbol,
	u32 importer_flags,
	DemanglerFunctions demangler) const
{
	return mdebug::import_symbol_table(
		database, m_image, m_section_offset, source, module_symbol, importer_flags | DONT_DEDUPLICATE_SYMBOLS, demangler);
}

Result<void> MdebugSymbolTable::print_headers(FILE* out) const
{
	mdebug::SymbolTableReader reader;
	
	Result<void> reader_result = reader.init(m_image, m_section_offset);
	CCC_RETURN_IF_ERROR(reader_result);
	
	reader.print_header(out);
	
	return Result<void>();
}

Result<void> MdebugSymbolTable::print_symbols(FILE* out, bool print_locals, bool print_externals) const
{
	mdebug::SymbolTableReader reader;
	Result<void> reader_result = reader.init(m_image, m_section_offset);
	CCC_RETURN_IF_ERROR(reader_result);
	
	Result<void> print_result = reader.print_symbols(out, print_locals, print_externals);
	CCC_RETURN_IF_ERROR(print_result);
	
	return Result<void>();
}

// *****************************************************************************

SymtabSymbolTable::SymtabSymbolTable(std::span<const u8> symtab, std::span<const u8> strtab)
	: m_symtab(symtab), m_strtab(strtab) {}

const char* SymtabSymbolTable::name() const
{
	return "ELF Symbol Table";
}

Result<void> SymtabSymbolTable::import(
	SymbolDatabase& database,
	SymbolSourceHandle source,
	const Module* module_symbol,
	u32 importer_flags,
	DemanglerFunctions demangler) const
{
	return elf::import_symbols(database, source, module_symbol, m_symtab, m_strtab, importer_flags, demangler);
}

Result<void> SymtabSymbolTable::print_headers(FILE* out) const
{
	return Result<void>();
}

Result<void> SymtabSymbolTable::print_symbols(FILE* out, bool print_locals, bool print_externals) const
{
	Result<void> symbtab_result = elf::print_symbol_table(out, m_symtab, m_strtab);
	CCC_RETURN_IF_ERROR(symbtab_result);
	
	return Result<void>();
}

// *****************************************************************************

SNDLLSymbolTable::SNDLLSymbolTable(std::shared_ptr<SNDLLFile> sndll)
	: m_sndll(std::move(sndll)) {}

const char* SNDLLSymbolTable::name() const
{
	return "SNDLL Symbol Table";
}

Result<void> SNDLLSymbolTable::import(
	SymbolDatabase& database,
	SymbolSourceHandle source,
	const Module* module_symbol,
	u32 importer_flags,
	DemanglerFunctions demangler) const
{
	return import_sndll_symbols(database, *m_sndll, source, module_symbol, importer_flags, demangler);
}

Result<void> SNDLLSymbolTable::print_headers(FILE* out) const
{
	return Result<void>();
}

Result<void> SNDLLSymbolTable::print_symbols(FILE* out, bool print_locals, bool print_externals) const
{
	print_sndll_symbols(out, *m_sndll);
	
	return Result<void>();
}

// *****************************************************************************

ElfSectionHeadersSymbolTable::ElfSectionHeadersSymbolTable(const ElfFile& elf)
	: m_elf(elf) {}

const char* ElfSectionHeadersSymbolTable::name() const
{
	return "ELF Section Headers";
}

Result<void> ElfSectionHeadersSymbolTable::import(
	SymbolDatabase& database,
	SymbolSourceHandle source,
	const Module* module_symbol,
	u32 importer_flags,
	DemanglerFunctions demangler) const
{
	return import_elf_section_headers(database, m_elf, source, module_symbol);
}

Result<void> ElfSectionHeadersSymbolTable::print_headers(FILE* out) const
{
	return Result<void>();
}

Result<void> ElfSectionHeadersSymbolTable::print_symbols(FILE* out, bool print_locals, bool print_externals) const
{
	return Result<void>();
}

}
