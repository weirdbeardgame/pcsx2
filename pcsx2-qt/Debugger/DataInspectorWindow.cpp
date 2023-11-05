#include "DataInspectorWindow.h"

#include "../QtHost.h"
#include "common/Error.h"
#include "CDVD/CDVD.h"
#include "DebugTools/ccc/analysis.h"
#include "Debugger/Delegates/DataInspectorValueColumnDelegate.h"
#include "MainWindow.h"

DataInspectorWindow::DataInspectorWindow(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	m_ui.statusBar->showMessage("Loading...");

	loadMdebugSymbolTableAsync(
		[](ccc::HighSymbolTable& symbolTable, std::vector<std::pair<ELF_SHR, std::string>>& elfSections) {
			DataInspectorWindow* window = g_main_window->getDataInspectorWindow();
			if (window)
			{
				window->m_symbolTable = std::move(symbolTable);
				window->m_elfSections = std::move(elfSections);
				window->createGUI();
			}
		},
		[](QString errorMessage, const char* sourceFile, int sourceLine) {
			DataInspectorWindow* window = g_main_window->getDataInspectorWindow();
			if (window)
			{
				window->m_ui.statusBar->showMessage(errorMessage);
			}
		});
}

void DataInspectorWindow::loadMdebugSymbolTableAsync(
	std::function<void(ccc::HighSymbolTable& symbolTable, std::vector<std::pair<ELF_SHR, std::string>>& elfSections)> successCallback,
	std::function<void(QString errorMessage, const char* sourceFile, int sourceLine)> failureCallback)
{
	Host::RunOnCPUThread([successCallback, failureCallback]() {
		std::string elfPath = VMManager::GetELFPath();
		if (elfPath.empty())
		{
			QtHost::RunOnUIThread([failureCallback]() {
				failureCallback("No ELF file loaded", nullptr, -1);
			});
			return;
		}

		ElfObject elf;
		Error error;
		if (!cdvdLoadElf(&elf, elfPath, false, &error))
		{
			QString message = QString("Failed to load ELF file %1 (%2)").arg(elfPath.c_str()).arg(error.GetDescription().c_str());
			QtHost::RunOnUIThread([failureCallback, message]() {
				failureCallback(message, nullptr, -1);
			});
			return;
		}

		std::vector<std::pair<ELF_SHR, std::string>> elfSections = elf.GetSectionHeaders();

		// Checking the section name is better than checking the section type
		// since both STABS and DWARF symbols have a type of MIPS_DEBUG.
		ELF_SHR* mdebugSectionHeader = nullptr;
		for (auto& [sectionHeader, sectionName] : elfSections)
			if (sectionName == ".mdebug")
				mdebugSectionHeader = &sectionHeader;

		if (mdebugSectionHeader == nullptr)
		{
			failureCallback("No .mdebug symbol table section present", nullptr, -1);
			return;
		}

		ccc::Result<ccc::mdebug::SymbolTable> mdebugSymbolTable =
			ccc::mdebug::parse_symbol_table(elf.GetData(), mdebugSectionHeader->sh_offset);
		if (!mdebugSymbolTable.success())
		{
			QString message = QString::fromStdString(mdebugSymbolTable.error().message);
			QtHost::RunOnUIThread([failureCallback, message]() {
				failureCallback(message, nullptr, -1);
			});
			return;
		}

		ccc::Result<ccc::HighSymbolTable> symbolTable =
			ccc::analyse(*mdebugSymbolTable, ccc::DEDUPLICATE_TYPES);
		if (!symbolTable.success())
		{
			QString message = QString::fromStdString(symbolTable.error().message);
			const char* sourceFile = symbolTable.error().source_file;
			s32 sourceLine = symbolTable.error().source_line;
			QtHost::RunOnUIThread([failureCallback, message, sourceFile, sourceLine]() {
				failureCallback(message, sourceFile, sourceLine);
			});
			return;
		}

		QtHost::RunOnUIThread([successCallback, &symbolTable, &elfSections]() {
			successCallback(*symbolTable, elfSections);
		},
			true);
	});
}

void DataInspectorWindow::createGUI()
{
	m_typeNameToDeduplicatedTypeIndex = ccc::build_type_name_to_deduplicated_type_index_map(m_symbolTable);

	bool groupBySection = m_ui.globalsGroupBySection->isChecked();
	bool groupByTranslationUnit = m_ui.globalsGroupByTranslationUnit->isChecked();
	QString filter = m_ui.globalsFilter->text().toLower();

	auto initialGlobalRoot = populateGlobalSections(groupBySection, groupByTranslationUnit, filter);
	m_globalModel = new DataInspectorModel(std::move(initialGlobalRoot), m_symbolTable, m_typeNameToDeduplicatedTypeIndex, this);
	m_ui.globalsTreeView->setModel(m_globalModel);

	connect(m_ui.globalsFilter, &QLineEdit::textEdited, this, &DataInspectorWindow::resetGlobals);
	connect(m_ui.globalsGroupBySection, &QCheckBox::toggled, this, &DataInspectorWindow::resetGlobals);
	connect(m_ui.globalsGroupByTranslationUnit, &QCheckBox::toggled, this, &DataInspectorWindow::resetGlobals);

	for (QTreeView* view : {m_ui.watchTreeView, m_ui.globalsTreeView, m_ui.stackTreeView})
	{
		auto delegate = new DataInspectorValueColumnDelegate(m_symbolTable, m_typeNameToDeduplicatedTypeIndex, this);
		view->setItemDelegateForColumn(DataInspectorModel::VALUE, delegate);
		view->setAlternatingRowColors(true);
	}

	m_ui.statusBar->showMessage(QString("Loaded %1 data types").arg(m_symbolTable.deduplicated_types.size()));
}

void DataInspectorWindow::resetGlobals()
{
	bool groupBySection = m_ui.globalsGroupBySection->isChecked();
	bool groupByTranslationUnit = m_ui.globalsGroupByTranslationUnit->isChecked();
	QString filter = m_ui.globalsFilter->text().toLower();

	m_globalModel->reset(populateGlobalSections(groupBySection, groupByTranslationUnit, filter));
}

std::unique_ptr<DataInspectorNode> DataInspectorWindow::populateGlobalSections(
	bool groupBySection, bool groupByTranslationUnit, const QString& filter)
{
	std::unique_ptr<DataInspectorNode> root = std::make_unique<DataInspectorNode>();
	root->childrenFetched = true;

	if (groupBySection)
	{
		for (auto& [sectionHeader, sectionName] : m_elfSections)
		{
			if (sectionHeader.sh_addr > 0)
			{
				u32 minAddress = sectionHeader.sh_addr;
				u32 maxAddress = sectionHeader.sh_addr + sectionHeader.sh_size;
				auto sectionChildren = populateGlobalTranslationUnits(minAddress, maxAddress, groupByTranslationUnit, filter);
				if (!sectionChildren.empty())
				{
					std::unique_ptr<DataInspectorNode> node = std::make_unique<DataInspectorNode>();
					node->name = QString::fromStdString(sectionName);
					node->children = std::move(sectionChildren);

					for (std::unique_ptr<DataInspectorNode>& child : node->children)
						child->parent = node.get();

					root->children.emplace_back(std::move(node));
				}
			}
		}
	}
	else
	{
		auto rootChildren = populateGlobalTranslationUnits(0, UINT32_MAX, groupByTranslationUnit, filter);
		root->children.insert(root->children.end(),
			std::make_move_iterator(rootChildren.begin()),
			std::make_move_iterator(rootChildren.end()));
	}

	for (std::unique_ptr<DataInspectorNode>& child : root->children)
		child->parent = root.get();

	return root;
}

std::vector<std::unique_ptr<DataInspectorNode>> DataInspectorWindow::populateGlobalTranslationUnits(
	u32 minAddress, u32 maxAddress, bool groupByTranlationUnit, const QString& filter)
{
	std::vector<std::unique_ptr<DataInspectorNode>> children;

	if (groupByTranlationUnit)
	{
		for (const std::unique_ptr<ccc::ast::SourceFile>& sourceFile : m_symbolTable.source_files)
		{
			std::unique_ptr<DataInspectorNode> node = std::make_unique<DataInspectorNode>();
			if (!sourceFile->relative_path.empty())
				node->name = QString::fromStdString(sourceFile->relative_path);
			else
				node->name = QString::fromStdString(sourceFile->full_path);
			node->type = sourceFile.get();
			node->children = populateGlobalVariables(*sourceFile, minAddress, maxAddress, filter);

			if (!node->children.empty())
			{
				for (std::unique_ptr<DataInspectorNode>& child : node->children)
					child->parent = node.get();
				children.emplace_back(std::move(node));
			}
		}
	}
	else
	{
		for (const std::unique_ptr<ccc::ast::SourceFile>& sourceFile : m_symbolTable.source_files)
		{
			std::vector<std::unique_ptr<DataInspectorNode>> variables = populateGlobalVariables(*sourceFile, minAddress, maxAddress, filter);
			children.insert(children.end(),
				std::make_move_iterator(variables.begin()),
				std::make_move_iterator(variables.end()));
		}
	}

	return children;
}

std::vector<std::unique_ptr<DataInspectorNode>> DataInspectorWindow::populateGlobalVariables(
	const ccc::ast::SourceFile& sourceFile, u32 minAddress, u32 maxAddress, const QString& filter)
{
	std::vector<std::unique_ptr<DataInspectorNode>> variables;

	for (const std::unique_ptr<ccc::ast::Node>& global : sourceFile.globals)
	{
		const ccc::ast::Variable& variable = global->as<ccc::ast::Variable>();
		if (variable.storage.global_address != (u32)-1)
		{
			std::unique_ptr<DataInspectorNode> node = std::make_unique<DataInspectorNode>();
			node->name = QString::fromStdString(global->name);
			node->type = global.get();
			node->address = variable.storage.global_address;
			bool addressInRange = (u32)variable.storage.global_address >= minAddress && (u32)variable.storage.global_address < maxAddress;
			bool containsFilterString = filter.isEmpty() || node->name.toLower().contains(filter);
			if (addressInRange && containsFilterString)
			{
				variables.emplace_back(std::move(node));
			}
		}
	}

	return variables;
}
