/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2022  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QtWidgets/QMainWindow>

#include "Elfheader.h"
#include "Models/DataInspectorModel.h"
#include "ui_DataInspectorWindow.h"

class DataInspectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	using TreeNode = DataInspectorModel::TreeNode;

	explicit DataInspectorWindow(QWidget* parent = nullptr);
	
	void resetGlobals();
protected:
	
	std::unique_ptr<TreeNode> populateGlobals(bool groupBySection, bool groupByTranslationUnit);
	std::vector<std::unique_ptr<TreeNode>> populateInnerGlobals(u32 min_address, u32 max_address, bool groupByTU);

	std::vector<std::pair<ELF_SHR, std::string>> m_sectionHeaders;
	ccc::HighSymbolTable m_symbolTable;
	DataInspectorModel* m_globalModel;
	Ui::DataInspectorWindow m_ui;
	
	std::map<std::string, s32> m_typeNameToDeduplicatedTypeIndex;
};
