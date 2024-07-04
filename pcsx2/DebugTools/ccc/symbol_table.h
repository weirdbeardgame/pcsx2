// This file is part of the Chaos Compiler Collection.
// SPDX-License-Identifier: MIT

#pragma once

#include "symbol_database.h"

namespace ccc {

// Determine which symbol tables are present in a given file.

enum SymbolTableFormat {
	MDEBUG = 0, // The infamous Third Eye symbol table
	SYMTAB = 1, // Standard ELF symbol table
	SNDLL  = 2  // SNDLL section
};

struct SymbolTableFormatInfo {
	SymbolTableFormat format;
	const char* format_name;
	const char* section_name;
};

// All the supported symbol table formats, sorted from best to worst.
extern const std::vector<SymbolTableFormatInfo> SYMBOL_TABLE_FORMATS;

const SymbolTableFormatInfo* symbol_table_format_from_enum(SymbolTableFormat format);
const SymbolTableFormatInfo* symbol_table_format_from_name(const char* format_name);
const SymbolTableFormatInfo* symbol_table_format_from_section(const char* section_name);

class SymbolTable {
public:
	virtual ~SymbolTable() {}
	
	virtual std::string name() const = 0;
	
	// Imports this symbol table into the passed database.
	virtual Result<void> import(
		SymbolDatabase& database,
		SymbolSourceHandle source,
		u32 importer_flags,
		DemanglerFunctions demangler) const = 0;
	
	// Print out all the field in the header structure if one exists.
	virtual Result<void> print_headers(FILE* out) const = 0;
	
	// Print out all the symbols in the symbol table. For .mdebug symbol tables
	// the symbols are split between those that are local to a specific
	// translation unit and those that are external, which is what the
	// print_locals and print_externals parameters control.
	virtual Result<void> print_symbols(FILE* out, bool print_locals, bool print_externals) const = 0;
};

struct ElfSection;
struct ElfFile;

// Create a symbol table from an ELF section. The return value may be null.
Result<std::unique_ptr<SymbolTable>> create_elf_symbol_table(
	const ElfSection& section, const ElfFile& elf, SymbolTableFormat format);

// Utility function to call import_symbol_table on all the passed symbol tables.
Result<SymbolSourceRange> import_symbol_tables(
	SymbolDatabase& database,
	const std::vector<std::unique_ptr<SymbolTable>>& symbol_tables,
	u32 importer_flags,
	DemanglerFunctions demangler);

class MdebugSymbolTable : public SymbolTable {
public:
	MdebugSymbolTable(std::span<const u8> image, s32 section_offset, std::string section_name);
	
	std::string name() const override;
	
	Result<void> import(
		SymbolDatabase& database,
		SymbolSourceHandle source,
		u32 importer_flags,
		DemanglerFunctions demangler) const override;
	Result<void> print_headers(FILE* out) const override;
	Result<void> print_symbols(FILE* out, bool print_locals, bool print_externals) const override;
	
protected:
	std::span<const u8> m_image;
	s32 m_section_offset;
	std::string m_section_name;
};

class SymtabSymbolTable : public SymbolTable {
public:
	SymtabSymbolTable(std::span<const u8> symtab, std::span<const u8> strtab, std::string section_name);
	
	std::string name() const override;
	
	Result<void> import(
		SymbolDatabase& database,
		SymbolSourceHandle source,
		u32 importer_flags,
		DemanglerFunctions demangler) const override;
	
	Result<void> print_headers(FILE* out) const override;
	Result<void> print_symbols(FILE* out, bool print_locals, bool print_externals) const override;
	
protected:
	std::span<const u8> m_symtab;
	std::span<const u8> m_strtab;
	std::string m_section_name;
};

struct SNDLLFile;

class SNDLLSymbolTable : public SymbolTable {
public:
	SNDLLSymbolTable(std::shared_ptr<SNDLLFile> sndll, std::string fallback_name);
	
	std::string name() const override;
	
	Result<void> import(
		SymbolDatabase& database,
		SymbolSourceHandle source,
		u32 importer_flags,
		DemanglerFunctions demangler) const override;
	
	Result<void> print_headers(FILE* out) const override;
	Result<void> print_symbols(FILE* out, bool print_locals, bool print_externals) const override;
	
protected:
	std::shared_ptr<SNDLLFile> m_sndll;
	std::string m_fallback_name;
};

class ElfSectionHeadersSymbolTable : public SymbolTable {
public:
	ElfSectionHeadersSymbolTable(const ElfFile& elf);
	
	std::string name() const override;
	
	Result<void> import(
		SymbolDatabase& database,
		SymbolSourceHandle source,
		u32 importer_flags,
		DemanglerFunctions demangler) const override;
	
	Result<void> print_headers(FILE* out) const override;
	Result<void> print_symbols(FILE* out, bool print_locals, bool print_externals) const override;
protected:
	const ElfFile& m_elf;
};

}
