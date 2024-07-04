// SPDX-FileCopyrightText: 2002-2023 PCSX2 Dev Team
// SPDX-License-Identifier: LGPL-3.0+

#pragma once

#include <vector>
#include <map>
#include <string>
#include <mutex>

#include <functional>
#include <shared_mutex>

#include "common/Pcsx2Types.h"
#include "DebugTools/ccc/symbol_database.h"

struct SymbolGuardian
{
public:
	SymbolGuardian();
	SymbolGuardian(const SymbolGuardian& rhs) = delete;
	SymbolGuardian(SymbolGuardian&& rhs) = delete;
	SymbolGuardian& operator=(const SymbolGuardian& rhs) = delete;
	SymbolGuardian& operator=(SymbolGuardian&& rhs) = delete;

	bool Read(std::function<void(const ccc::SymbolDatabase&)> callback) const;
	void ReadWrite(std::function<void(ccc::SymbolDatabase&)> callback);

	void LoadSymbolTables(std::vector<u8> elf, std::string file_name);

	// Delete all symbols, including user-defined symbols. It is important to do
	// it this way rather than assigning a new SymbolDatabase object so we
	// don't get dangling symbol handles.
	void Clear();
	void ClearIrxModules();

	std::string FunctionNameFromAddress(u32 address) const;

protected:
	ccc::SymbolDatabase m_database;
	ccc::SymbolSourceHandle m_user_defined;
	ccc::ModuleHandle m_main_elf;
	mutable std::shared_mutex m_big_symbol_lock;
};

extern SymbolGuardian R5900SymbolGuardian;
extern SymbolGuardian R3000SymbolGuardian;

enum SymbolType
{
	ST_NONE = 0,
	ST_FUNCTION = 1,
	ST_DATA = 2,
	ST_ALL = 3,
};

struct SymbolInfo
{
	SymbolType type;
	u32 address;
	u32 size;
};

struct SymbolEntry
{
	std::string name;
	u32 address;
	u32 size;
};

enum DataType
{
	DATATYPE_NONE,
	DATATYPE_BYTE,
	DATATYPE_HALFWORD,
	DATATYPE_WORD,
	DATATYPE_ASCII
};

class SymbolMap
{
public:
	SymbolMap() {}
	void Clear();
	void SortSymbols();

	bool LoadNocashSym(const std::string& filename);

	SymbolType GetSymbolType(u32 address) const;
	bool GetSymbolInfo(SymbolInfo* info, u32 address, SymbolType symmask = ST_FUNCTION) const;
	u32 GetNextSymbolAddress(u32 address, SymbolType symmask);
	std::string GetDescription(unsigned int address) const;
	std::vector<SymbolEntry> GetAllSymbols(SymbolType symmask) const;

	void AddFunction(const std::string& name, u32 address, u32 size, bool noReturn = false);
	u32 GetFunctionStart(u32 address) const;
	int GetFunctionNum(u32 address) const;
	bool GetFunctionNoReturn(u32 address) const;
	u32 GetFunctionSize(u32 startAddress) const;
	bool SetFunctionSize(u32 startAddress, u32 newSize);
	bool RemoveFunction(u32 startAddress);

	void AddLabel(const std::string& name, u32 address);
	std::string GetLabelName(u32 address) const;
	void SetLabelName(const std::string& name, u32 address);
	bool GetLabelValue(const std::string& name, u32& dest);

	void AddData(u32 address, u32 size, DataType type, int moduleIndex = -1);
	u32 GetDataStart(u32 address) const;
	u32 GetDataSize(u32 startAddress) const;
	DataType GetDataType(u32 startAddress) const;

	// Module functions for IOP symbols

	static const u32 INVALID_ADDRESS = (u32)-1;

	bool IsEmpty() const { return functions.empty() && labels.empty() && data.empty(); };

private:
	void AssignFunctionIndices();

	struct FunctionEntry
	{
		u32 start;
		u32 size;
		int index;
		std::string name;
		bool noReturn;
	};

	struct LabelEntry
	{
		u32 addr;
		std::string name;
	};

	struct DataEntry
	{
		DataType type;
		u32 start;
		u32 size;
	};

	std::map<u32, FunctionEntry> functions;
	std::map<u32, LabelEntry> labels;
	std::map<u32, DataEntry> data;

	mutable std::recursive_mutex m_lock;
};

extern SymbolMap R5900SymbolMap;
extern SymbolMap R3000SymbolMap;
