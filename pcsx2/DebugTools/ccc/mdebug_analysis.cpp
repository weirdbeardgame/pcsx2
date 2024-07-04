// This file is part of the Chaos Compiler Collection.
// SPDX-License-Identifier: MIT

#include "mdebug_analysis.h"

#include "stabs_to_ast.h"

namespace ccc::mdebug {

Result<void> LocalSymbolTableAnalyser::stab_magic(const char* magic)
{
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::source_file(const char* path, Address text_address)
{
	m_source_file.text_address = text_address;
	if(m_next_relative_path.empty()) {
		m_next_relative_path = m_source_file.command_line_path;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::data_type(const ParsedSymbol& symbol)
{
	Result<std::unique_ptr<ast::Node>> node = stabs_type_to_ast(
		*symbol.name_colon_type.type.get(), nullptr, m_stabs_to_ast_state, 0, false, false);
	CCC_RETURN_IF_ERROR(node);
	
	bool is_struct = (*node)->descriptor == ast::STRUCT_OR_UNION && (*node)->as<ast::StructOrUnion>().is_struct;
	bool force_typedef =
		((m_context.importer_flags & TYPEDEF_ALL_ENUMS) && (*node)->descriptor == ast::ENUM) ||
		((m_context.importer_flags & TYPEDEF_ALL_STRUCTS) && (*node)->descriptor == ast::STRUCT_OR_UNION && is_struct) ||
		((m_context.importer_flags & TYPEDEF_ALL_UNIONS) && (*node)->descriptor == ast::STRUCT_OR_UNION && !is_struct);
	
	(*node)->name = (symbol.name_colon_type.name == " ") ? "" : symbol.name_colon_type.name;
	if(symbol.is_typedef || force_typedef) {
		(*node)->storage_class = STORAGE_CLASS_TYPEDEF;
	}
	
	const char* name = (*node)->name.c_str();
	StabsTypeNumber number = symbol.name_colon_type.type->type_number;
	
	if(m_context.importer_flags & DONT_DEDUPLICATE_TYPES) {
		Result<DataType*> data_type = m_database.data_types.create_symbol(name, m_context.symbol_source);
		CCC_RETURN_IF_ERROR(data_type);
		
		m_source_file.stabs_type_number_to_handle[number] = (*data_type)->handle();
		(*data_type)->set_type(std::move(*node));
		
		(*data_type)->files = {m_source_file.handle()};
	} else {
		Result<ccc::DataType*> type = m_database.create_data_type_if_unique(
			std::move(*node), number, name, m_source_file, m_context.symbol_source);
		CCC_RETURN_IF_ERROR(type);
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::global_variable(
	const char* mangled_name, Address address, const StabsType& type, bool is_static, GlobalStorageLocation location)
{
	Result<GlobalVariable*> global = m_database.global_variables.create_symbol(
		mangled_name, m_context.symbol_source, address, m_context.importer_flags, m_context.demangler);
	CCC_RETURN_IF_ERROR(global);
	CCC_ASSERT(*global);
	
	m_global_variables.expand_to_include((*global)->handle());
	
	
	
	Result<std::unique_ptr<ast::Node>> node = stabs_type_to_ast(type, nullptr, m_stabs_to_ast_state, 0, true, false);
	CCC_RETURN_IF_ERROR(node);
	
	if(is_static) {
		(*global)->storage_class = STORAGE_CLASS_STATIC;
	}
	(*global)->set_type(std::move(*node));
	
	(*global)->storage.location = location;
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::sub_source_file(const char* path, Address text_address)
{
	if(m_current_function && m_state == IN_FUNCTION_BEGINNING) {
		Function::SubSourceFile& sub = m_current_function->sub_source_files.emplace_back();
		sub.address = text_address;
		sub.relative_path = path;
	} else {
		m_next_relative_path = path;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::procedure(const char* mangled_name, Address address, bool is_static)
{
	if(!m_current_function || strcmp(mangled_name, m_current_function->mangled_name().c_str()) != 0) {
		Result<void> result = create_function(mangled_name, address);
		CCC_RETURN_IF_ERROR(result);
	}
	
	if(is_static) {
		m_current_function->storage_class = STORAGE_CLASS_STATIC;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::label(const char* label, Address address, s32 line_number)
{
	if(address.valid() && m_current_function && label[0] == '$') {
		Function::LineNumberPair& pair = m_current_function->line_numbers.emplace_back();
		pair.address = address;
		pair.line_number = line_number;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::text_end(const char* name, s32 function_size)
{
	if(m_state == IN_FUNCTION_BEGINNING) {
		CCC_CHECK(m_current_function, "END TEXT symbol outside of function.");
		m_current_function->set_size(function_size);
		m_state = IN_FUNCTION_END;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::function(const char* mangled_name, const StabsType& return_type, Address address)
{
	if(!m_current_function || strcmp(mangled_name, m_current_function->mangled_name().c_str())) {
		Result<void> result = create_function(mangled_name, address);
		CCC_RETURN_IF_ERROR(result);
	}
	
	Result<std::unique_ptr<ast::Node>> node = stabs_type_to_ast(return_type, nullptr, m_stabs_to_ast_state, 0, true, true);
	CCC_RETURN_IF_ERROR(node);
	m_current_function->set_type(std::move(*node));
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::function_end()
{
	if(m_current_function) {
		m_current_function->set_parameter_variables(m_current_parameter_variables, DONT_DELETE_OLD_SYMBOLS, m_database);
		m_current_function->set_local_variables(m_current_local_variables, DONT_DELETE_OLD_SYMBOLS, m_database);
	}
	
	m_current_function = nullptr;
	m_current_parameter_variables.clear();
	m_current_local_variables.clear();
	
	m_blocks.clear();
	m_pending_local_variables.clear();
	
	m_state = NOT_IN_FUNCTION;
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::parameter(
	const char* name, const StabsType& type, bool is_stack, s32 value, bool is_by_reference)
{
	CCC_CHECK(m_current_function, "Parameter symbol before first func/proc symbol.");
	
	Result<ParameterVariable*> parameter_variable = m_database.parameter_variables.create_symbol(name, m_context.symbol_source);
	CCC_RETURN_IF_ERROR(parameter_variable);
	m_current_parameter_variables.expand_to_include((*parameter_variable)->handle());
	
	Result<std::unique_ptr<ast::Node>> node = stabs_type_to_ast(type, nullptr, m_stabs_to_ast_state, 0, true, true);
	CCC_RETURN_IF_ERROR(node);
	(*parameter_variable)->set_type(std::move(*node));
	
	if(is_stack) {
		StackStorage& stack_storage = (*parameter_variable)->storage.emplace<StackStorage>();
		stack_storage.stack_pointer_offset = value;
	} else {
		RegisterStorage& register_storage = (*parameter_variable)->storage.emplace<RegisterStorage>();
		register_storage.dbx_register_number = value;
		register_storage.is_by_reference = is_by_reference;
	}
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::local_variable(
	const char* name, const StabsType& type, u32 value, StabsSymbolDescriptor desc, SymbolClass sclass)
{
	if(!m_current_function) {
		return Result<void>();
	}
	
	Address address = (desc == StabsSymbolDescriptor::STATIC_LOCAL_VARIABLE) ? value : Address();
	Result<LocalVariable*> local_variable = m_database.local_variables.create_symbol(name, m_context.symbol_source, address);
	CCC_RETURN_IF_ERROR(local_variable);
	m_current_local_variables.expand_to_include((*local_variable)->handle());
	m_pending_local_variables.emplace_back((*local_variable)->handle());
	
	Result<std::unique_ptr<ast::Node>> node = stabs_type_to_ast(type, nullptr, m_stabs_to_ast_state, 0, true, false);
	CCC_RETURN_IF_ERROR(node);
	
	if(desc == StabsSymbolDescriptor::STATIC_LOCAL_VARIABLE) {
		GlobalStorage& global_storage = (*local_variable)->storage.emplace<GlobalStorage>();
		std::optional<GlobalStorageLocation> location_opt =
			symbol_class_to_global_variable_location(sclass);
		CCC_CHECK(location_opt.has_value(),
			"Invalid static local variable location %s.",
			symbol_class(sclass));
		global_storage.location = *location_opt;
		(*node)->storage_class = STORAGE_CLASS_STATIC;
	} else if(desc == StabsSymbolDescriptor::REGISTER_VARIABLE) {
		RegisterStorage& register_storage = (*local_variable)->storage.emplace<RegisterStorage>();
		register_storage.dbx_register_number = (s32) value;
	} else if(desc == StabsSymbolDescriptor::LOCAL_VARIABLE) {
		StackStorage& stack_storage = (*local_variable)->storage.emplace<StackStorage>();
		stack_storage.stack_pointer_offset = (s32) value;
	} else {
		return CCC_FAILURE("LocalSymbolTableAnalyser::local_variable() called with bad symbol descriptor.");
	}
	
	(*local_variable)->set_type(std::move(*node));
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::lbrac(s32 begin_offset)
{
	for(LocalVariableHandle local_variable_handle : m_pending_local_variables) {
		if(LocalVariable* local_variable = m_database.local_variables.symbol_from_handle(local_variable_handle)) {
			local_variable->live_range.low = m_source_file.text_address.value + begin_offset;
		}
	}
	
	m_blocks.emplace_back(std::move(m_pending_local_variables));
	m_pending_local_variables = {};
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::rbrac(s32 end_offset)
{
	CCC_CHECK(!m_blocks.empty(), "RBRAC symbol without a matching LBRAC symbol.");
	
	std::vector<LocalVariableHandle>& variables = m_blocks.back();
	for(LocalVariableHandle local_variable_handle : variables) {
		if(LocalVariable* local_variable = m_database.local_variables.symbol_from_handle(local_variable_handle)) {
			local_variable->live_range.high = m_source_file.text_address.value + end_offset;
		}
	}
	
	m_blocks.pop_back();
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::finish()
{
	CCC_CHECK(m_state != IN_FUNCTION_BEGINNING,
		"Unexpected end of symbol table for '%s'.", m_source_file.name().c_str());
	
	if(m_current_function) {
		Result<void> result = function_end();
		CCC_RETURN_IF_ERROR(result);
	}
	
	m_source_file.set_functions(m_functions, DONT_DELETE_OLD_SYMBOLS, m_database);
	m_source_file.set_globals_variables(m_global_variables, DONT_DELETE_OLD_SYMBOLS, m_database);
	
	return Result<void>();
}

Result<void> LocalSymbolTableAnalyser::create_function(const char* mangled_name, Address address)
{
	if(m_current_function) {
		Result<void> result = function_end();
		CCC_RETURN_IF_ERROR(result);
	}
	
	Result<Function*> function = m_database.functions.create_symbol(
		mangled_name, m_context.symbol_source, address, m_context.importer_flags, m_context.demangler);
	CCC_RETURN_IF_ERROR(function);
	CCC_ASSERT(*function);
	m_current_function = *function;
	
	m_functions.expand_to_include(m_current_function->handle());
	
	m_state = LocalSymbolTableAnalyser::IN_FUNCTION_BEGINNING;
	
	if(!m_next_relative_path.empty() && m_current_function->relative_path != m_source_file.command_line_path) {
		m_current_function->relative_path = m_next_relative_path;
	}
	
	return Result<void>();
}

std::optional<GlobalStorageLocation> symbol_class_to_global_variable_location(SymbolClass symbol_class)
{
	std::optional<GlobalStorageLocation> location;
	switch(symbol_class) {
		case SymbolClass::NIL: location = GlobalStorageLocation::NIL; break;
		case SymbolClass::DATA: location = GlobalStorageLocation::DATA; break;
		case SymbolClass::BSS: location = GlobalStorageLocation::BSS; break;
		case SymbolClass::ABS: location = GlobalStorageLocation::ABS; break;
		case SymbolClass::SDATA: location = GlobalStorageLocation::SDATA; break;
		case SymbolClass::SBSS: location = GlobalStorageLocation::SBSS; break;
		case SymbolClass::RDATA: location = GlobalStorageLocation::RDATA; break;
		case SymbolClass::COMMON: location = GlobalStorageLocation::COMMON; break;
		case SymbolClass::SCOMMON: location = GlobalStorageLocation::SCOMMON; break;
		case SymbolClass::SUNDEFINED: location = GlobalStorageLocation::SUNDEFINED; break;
		default: {}
	}
	return location;
}

}
