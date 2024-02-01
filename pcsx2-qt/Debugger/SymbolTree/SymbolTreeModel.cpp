// SPDX-FileCopyrightText: 2002-2024 PCSX2 Dev Team
// SPDX-License-Identifier: LGPL-3.0+

#include "SymbolTreeModel.h"

#include "common/Assertions.h"

SymbolTreeModel::SymbolTreeModel(DebugInterface& cpu, QObject* parent)
	: QAbstractItemModel(parent)
	, m_cpu(cpu)
	, m_guardian(cpu.GetSymbolGuardian())
{
}

QModelIndex SymbolTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	SymbolTreeNode* parent_node;
	if (parent.isValid())
		parent_node = static_cast<SymbolTreeNode*>(parent.internalPointer());
	else
		parent_node = m_root.get();

	const SymbolTreeNode* child_node = parent_node->children().at(row).get();
	if (!child_node)
		return QModelIndex();

	return createIndex(row, column, child_node);
}

QModelIndex SymbolTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	SymbolTreeNode* child_node = static_cast<SymbolTreeNode*>(index.internalPointer());
	const SymbolTreeNode* parent_node = child_node->parent();
	if (!parent_node)
		return QModelIndex();

	return indexFromNode(*parent_node);
}

int SymbolTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0)
		return 0;

	SymbolTreeNode* node;
	if (parent.isValid())
		node = static_cast<SymbolTreeNode*>(parent.internalPointer());
	else
		node = m_root.get();

	return (int)node->children().size();
}

int SymbolTreeModel::columnCount(const QModelIndex& parent) const
{
	return COLUMN_COUNT;
}

bool SymbolTreeModel::hasChildren(const QModelIndex& parent) const
{
	bool result = true;

	if (!parent.isValid())
		return result;

	SymbolTreeNode* parent_node = static_cast<SymbolTreeNode*>(parent.internalPointer());
	if (!parent_node->type.valid())
		return result;

	m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
		const ccc::ast::Node* type = parent_node->type.lookup_node(database);
		if (!type)
			return;

		result = nodeHasChildren(*type, database);
	});

	return result;
}

QVariant SymbolTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::UserRole))
		return QVariant();

	SymbolTreeNode* node = static_cast<SymbolTreeNode*>(index.internalPointer());

	switch (index.column())
	{
		case NAME:
		{
			return node->name;
		}
		case LOCATION:
		{
			return node->location.name();
		}
		case TYPE:
		{
			QVariant result;
			switch (node->state)
			{
				case SymbolTreeNodeState::NORMAL:
				case SymbolTreeNodeState::ARRAY:
				{
					m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
						const ccc::ast::Node* type = node->type.lookup_node(database);
						if (!type)
							return;

						result = typeToString(type, database);
					});
					break;
				}
				case SymbolTreeNodeState::STRING:
				{
					result = "string";
					break;
				}
			}
			return result;
		}
		case LIVENESS:
		{
			if (node->live_range.low.valid() && node->live_range.high.valid())
			{
				u32 pc = m_cpu.getPC();
				bool alive = pc >= node->live_range.low && pc < node->live_range.high;
				return alive ? "Alive" : "Dead";
			}
			return QVariant();
		}
		case VALUE:
		{
			if (!node->type.valid())
				return QVariant();

			QVariant result;
			m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
				const ccc::ast::Node* logical_type = node->type.lookup_node(database);
				if (!logical_type)
					return;

				const ccc::ast::Node& type = *resolvePhysicalType(logical_type, database).first;

				switch (role)
				{
					case Qt::DisplayRole:
						result = node->toString(type);
						break;
					case Qt::UserRole:
						result = node->toVariant(type);
						break;
				}
			});

			return result;
		}
	}

	return QVariant();
}

bool SymbolTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	bool result = false;

	if (!index.isValid() || role != Qt::UserRole)
		return result;

	SymbolTreeNode* node = static_cast<SymbolTreeNode*>(index.internalPointer());
	if (!node->type.valid())
		return result;

	m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
		const ccc::ast::Node* logical_type = node->type.lookup_node(database);
		if (!logical_type)
			return;

		const ccc::ast::Node& type = *resolvePhysicalType(logical_type, database).first;
		result = node->fromVariant(value, type);
	});

	if (result)
		emit dataChanged(index, index);

	return result;
}

void SymbolTreeModel::fetchMore(const QModelIndex& parent)
{
	if (!parent.isValid())
		return;

	SymbolTreeNode* parent_node = static_cast<SymbolTreeNode*>(parent.internalPointer());
	if (!parent_node->type.valid())
		return;

	std::vector<std::unique_ptr<SymbolTreeNode>> children;
	m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
		const ccc::ast::Node* logical_parent_type = parent_node->type.lookup_node(database);
		if (!logical_parent_type)
			return;

		children = populateChildren(parent_node->location, *logical_parent_type, parent_node->type, database);
	});

	if (!children.empty())
		beginInsertRows(parent, 0, children.size() - 1);
	parent_node->setChildren(std::move(children));
	if (!children.empty())
		endInsertRows();
}

bool SymbolTreeModel::canFetchMore(const QModelIndex& parent) const
{
	bool result = false;

	if (!parent.isValid())
		return result;

	SymbolTreeNode* parent_node = static_cast<SymbolTreeNode*>(parent.internalPointer());
	if (!parent_node->type.valid())
		return result;

	m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
		const ccc::ast::Node* parent_type = parent_node->type.lookup_node(database);
		if (!parent_type)
			return;

		result = nodeHasChildren(*parent_type, database) && !parent_node->childrenFetched();
	});

	return result;
}

Qt::ItemFlags SymbolTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flags = QAbstractItemModel::flags(index);

	if (index.column() == VALUE)
		flags |= Qt::ItemIsEditable;

	return flags;
}

QVariant SymbolTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	switch (section)
	{
		case NAME:
			return tr("Name");
		case LOCATION:
			return tr("Location");
		case TYPE:
			return tr("Type");
		case LIVENESS:
			return tr("Liveness");
		case VALUE:
			return tr("Value");
	}

	return QVariant();
}

void SymbolTreeModel::reset(std::unique_ptr<SymbolTreeNode> new_root)
{
	beginResetModel();
	m_root = std::move(new_root);
	endResetModel();
}

QString SymbolTreeModel::changeTypeTemporarily(QModelIndex index, std::string_view type_string)
{
	pxAssertRel(index.isValid(), "Invalid model index.");

	SymbolTreeNode* node = static_cast<SymbolTreeNode*>(index.internalPointer());

	// Remove any existing children.
	bool remove_rows = !node->children().empty();
	if (remove_rows)
		beginRemoveRows(index, 0, node->children().size() - 1);
	node->clearChildren();
	if (remove_rows)
		endRemoveRows();

	QString error_message;
	m_guardian.ShortReadWrite([&](ccc::SymbolDatabase& database) -> void {
		// Parse the input string.
		std::unique_ptr<ccc::ast::Node> type = stringToType(type_string, database, error_message);
		if (!error_message.isEmpty() || !type)
		{
			return;
		}

		// Update the model.
		node->temporary_type = std::move(type);
		node->type = ccc::NodeHandle(node->temporary_type.get());
	});

	return error_message;
}

QString SymbolTreeModel::typeToString(QModelIndex index)
{
	pxAssertRel(index.isValid(), "Invalid model index.");

	QString result;
	m_guardian.Read([&](const ccc::SymbolDatabase& database) -> void {
		SymbolTreeNode* node = static_cast<SymbolTreeNode*>(index.internalPointer());
		const ccc::ast::Node* type = node->type.lookup_node(database);
		if (!type)
			return;

		result = typeToString(type, database);
	});

	return result;
}

QString SymbolTreeModel::typeToString(const ccc::ast::Node* type, const ccc::SymbolDatabase& database)
{
	QString suffix;

	// Traverse through arrays, pointers and references, and build a string
	// to be appended to the end of the type name.
	bool done_finding_arrays_pointers = false;
	bool found_pointer = false;
	while (!done_finding_arrays_pointers)
	{
		switch (type->descriptor)
		{
			case ccc::ast::ARRAY:
			{
				// If we see a pointer to an array we can't print that properly
				// with C-like syntax so we just print the node type instead.
				if (found_pointer)
				{
					done_finding_arrays_pointers = true;
					break;
				}

				const ccc::ast::Array& array = type->as<ccc::ast::Array>();
				suffix.append(QString("[%1]").arg(array.element_count));
				type = array.element_type.get();
				break;
			}
			case ccc::ast::POINTER_OR_REFERENCE:
			{
				const ccc::ast::PointerOrReference& pointer_or_reference = type->as<ccc::ast::PointerOrReference>();
				suffix.prepend(pointer_or_reference.is_pointer ? '*' : '&');
				type = pointer_or_reference.value_type.get();
				found_pointer = true;
				break;
			}
			default:
			{
				done_finding_arrays_pointers = true;
				break;
			}
		}
	}

	// Determine the actual type name, or at the very least the node type.
	QString name;
	switch (type->descriptor)
	{
		case ccc::ast::BUILTIN:
		{
			const ccc::ast::BuiltIn& built_in = type->as<ccc::ast::BuiltIn>();
			name = ccc::ast::builtin_class_to_string(built_in.bclass);
			break;
		}
		case ccc::ast::TYPE_NAME:
		{
			const ccc::ast::TypeName& type_name = type->as<ccc::ast::TypeName>();
			const ccc::DataType* data_type = database.data_types.symbol_from_handle(type_name.data_type_handle);
			if (data_type)
			{
				name = QString::fromStdString(data_type->name());
			}
			break;
		}
		default:
		{
			name = ccc::ast::node_type_to_string(*type);
		}
	}

	return name + suffix;
}

std::unique_ptr<ccc::ast::Node> SymbolTreeModel::stringToType(std::string_view string, const ccc::SymbolDatabase& database, QString& error_out)
{
	if (string.empty())
	{
		error_out = tr("No type name provided.");
		return nullptr;
	}

	size_t i = string.size();

	// Parse array subscripts e.g. 'float[4][4]'.
	std::vector<s32> array_subscripts;
	for (; i > 0; i--)
	{
		if (string[i - 1] != ']' || i < 2)
			break;

		size_t j = i - 1;
		for (; string[j - 1] >= '0' && string[j - 1] <= '9' && j >= 0; j--)
			;

		if (string[j - 1] != '[')
			break;

		s32 element_count = atoi(&string[j]);
		if (element_count < 0)
		{
			error_out = tr("Array subscripts cannot be negative.");
			return nullptr;
		}
		array_subscripts.emplace_back(element_count);

		i = j;
	}

	// Parse pointer characters e.g. 'char*&'.
	std::vector<char> pointer_characters;
	for (; i > 0 && string[i - 1] == '*' || string[i - 1] == '&'; i--)
	{
		pointer_characters.emplace_back(string[i - 1]);
	}

	if (i > 0 && string[i - 1] == ']')
	{
		error_out = tr("To create a pointer to an array, you must change the type of the pointee instead of the pointer itself.");
		return nullptr;
	}

	// Lookup the type.
	std::string type_name_string(string.data(), string.data() + i);
	ccc::DataTypeHandle handle = database.data_types.first_handle_from_name(type_name_string);
	const ccc::DataType* data_type = database.data_types.symbol_from_handle(handle);
	if (!data_type || !data_type->type())
	{
		error_out = tr("Type '%1' not found.").arg(QString::fromStdString(type_name_string));
		return nullptr;
	}

	std::unique_ptr<ccc::ast::Node> result;

	// Create the AST.
	std::unique_ptr<ccc::ast::TypeName> type_name = std::make_unique<ccc::ast::TypeName>();
	type_name->computed_size_bytes = data_type->type()->computed_size_bytes;
	type_name->data_type_handle = data_type->handle();
	type_name->source = ccc::ast::TypeNameSource::REFERENCE;
	result = std::move(type_name);

	for (i = pointer_characters.size(); i > 0; i--)
	{
		std::unique_ptr<ccc::ast::PointerOrReference> pointer_or_reference = std::make_unique<ccc::ast::PointerOrReference>();
		pointer_or_reference->computed_size_bytes = 4;
		pointer_or_reference->is_pointer = pointer_characters[i - 1] == '*';
		pointer_or_reference->value_type = std::move(result);
		result = std::move(pointer_or_reference);
	}

	for (s32 element_count : array_subscripts)
	{
		std::unique_ptr<ccc::ast::Array> array = std::make_unique<ccc::ast::Array>();
		array->computed_size_bytes = element_count * result->computed_size_bytes;
		array->element_type = std::move(result);
		array->element_count = element_count;
		result = std::move(array);
	}

	return result;
}

std::vector<std::unique_ptr<SymbolTreeNode>> SymbolTreeModel::populateChildren(
	SymbolTreeLocation location,
	const ccc::ast::Node& logical_type,
	ccc::NodeHandle parent_handle,
	const ccc::SymbolDatabase& database)
{
	auto [type, symbol] = resolvePhysicalType(&logical_type, database);

	// If we went through a type name, we need to make the node handles for the
	// children point to the new symbol instead of the original one.
	if (symbol)
		parent_handle = ccc::NodeHandle(*symbol, nullptr);

	std::vector<std::unique_ptr<SymbolTreeNode>> children;

	switch (type->descriptor)
	{
		case ccc::ast::ARRAY:
		{
			const ccc::ast::Array& array = type->as<ccc::ast::Array>();
			for (s32 i = 0; i < array.element_count; i++)
			{
				std::unique_ptr<SymbolTreeNode> element = std::make_unique<SymbolTreeNode>();
				element->name = QString("[%1]").arg(i);
				element->type = parent_handle.handle_for_child(array.element_type.get());
				element->location = location.addOffset(i * array.element_type->computed_size_bytes);
				if (element->location.type != SymbolTreeLocation::NONE)
					children.emplace_back(std::move(element));
			}
			break;
		}
		case ccc::ast::POINTER_OR_REFERENCE:
		{
			u32 address = location.read32();
			if (location.cpu().isValidAddress(address))
			{
				const ccc::ast::PointerOrReference& pointer_or_reference = type->as<ccc::ast::PointerOrReference>();
				std::unique_ptr<SymbolTreeNode> element = std::make_unique<SymbolTreeNode>();
				element->name = QString("*%1").arg(QString::number(address, 16));
				element->type = parent_handle.handle_for_child(pointer_or_reference.value_type.get());
				element->location = location.createAddress(address);
				children.emplace_back(std::move(element));
			}
			break;
		}
		case ccc::ast::STRUCT_OR_UNION:
		{
			const ccc::ast::StructOrUnion& struct_or_union = type->as<ccc::ast::StructOrUnion>();
			for (const std::unique_ptr<ccc::ast::Node>& base_class : struct_or_union.base_classes)
			{
				SymbolTreeLocation base_class_location = location.addOffset(base_class->offset_bytes);
				if (base_class_location.type != SymbolTreeLocation::NONE)
				{
					std::vector<std::unique_ptr<SymbolTreeNode>> fields = populateChildren(
						base_class_location, *base_class.get(), parent_handle, database);
					children.insert(children.end(),
						std::make_move_iterator(fields.begin()),
						std::make_move_iterator(fields.end()));
				}
			}
			for (const std::unique_ptr<ccc::ast::Node>& field : struct_or_union.fields)
			{
				std::unique_ptr<SymbolTreeNode> child_node = std::make_unique<SymbolTreeNode>();
				child_node->name = QString::fromStdString(field->name);
				child_node->type = parent_handle.handle_for_child(field.get());
				child_node->location = location.addOffset(field->offset_bytes);
				if (child_node->location.type != SymbolTreeLocation::NONE)
					children.emplace_back(std::move(child_node));
			}
			break;
		}
		default:
		{
		}
	}

	return children;
}

bool SymbolTreeModel::nodeHasChildren(const ccc::ast::Node& logical_type, const ccc::SymbolDatabase& database)
{
	const ccc::ast::Node& type = *resolvePhysicalType(&logical_type, database).first;

	bool result = false;
	switch (type.descriptor)
	{
		case ccc::ast::ARRAY:
		{
			const ccc::ast::Array& array = type.as<ccc::ast::Array>();
			result = array.element_count > 0;
			break;
		}
		case ccc::ast::POINTER_OR_REFERENCE:
		{
			result = true;
			break;
		}
		case ccc::ast::STRUCT_OR_UNION:
		{
			const ccc::ast::StructOrUnion& struct_or_union = type.as<ccc::ast::StructOrUnion>();
			result = !struct_or_union.base_classes.empty() || !struct_or_union.fields.empty();
			break;
		}
		default:
		{
		}
	}

	return result;
}

QModelIndex SymbolTreeModel::indexFromNode(const SymbolTreeNode& node) const
{
	int row = 0;
	if (node.parent())
	{
		for (int i = 0; i < (int)node.parent()->children().size(); i++)
			if (node.parent()->children()[i].get() == &node)
				row = i;
	}
	else
		row = 0;

	return createIndex(row, 0, &node);
}

std::pair<const ccc::ast::Node*, const ccc::DataType*> resolvePhysicalType(const ccc::ast::Node* type, const ccc::SymbolDatabase& database)
{
	const ccc::DataType* symbol = nullptr;
	for (s32 i = 0; i < 10 && type->descriptor == ccc::ast::TYPE_NAME; i++)
	{
		const ccc::DataType* data_type = database.data_types.symbol_from_handle(type->as<ccc::ast::TypeName>().data_type_handle);
		if (!data_type || !data_type->type())
			break;
		type = data_type->type();
		symbol = data_type;
	}
	return std::pair(type, symbol);
}
