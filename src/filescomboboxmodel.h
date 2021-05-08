#ifndef FILESCOMBOBOXMODEL_H
#define FILESCOMBOBOXMODEL_H

#include "datahash.h"
#include <QAbstractItemModel>
#include <vector>

class FilesComboBoxModel : public QAbstractItemModel
{
public:
  class Item {
  public:
    const QString path;
    const QString id;
    const DataHash hash;

    Item(const QString &path, const QString &id, const DataHash &hash) :
      path(path),
      id(id),
      hash(hash)
    {}

    Item(const Item &other) :
      path(other.path),
      id(other.id),
      hash(other.hash)
    {}

    Item(Item &&other) noexcept :
      path(std::move(other.path)),
      id(std::move(other.id)),
      hash(std::move(other.hash))
    {}

    Item & operator=(const Item &other)
    {
      const_cast<QString&>(path) = other.path;
      const_cast<QString&>(id) = other.id;
      const_cast<DataHash&>(hash) = other.hash;

      return *this;
    }

    Item & operator=(Item &&other) noexcept
    {
      const_cast<QString&>(path) = std::move(other.path);
      const_cast<QString&>(id) = std::move(other.id);
      const_cast<DataHash&>(hash) = std::move(other.hash);

      return *this;
    }

    bool operator==(const Item &other) const noexcept
    {
      return (path == other.path) &&
             (id == other.id) &&
             (hash == other.hash);
    }

    bool operator!=(const Item &other) const noexcept
    {
      return !(*this == other);
    }
  };

  explicit FilesComboBoxModel()
  {
  }

  void clear()
  {
      beginRemoveRows(QModelIndex(), 0, m_items.size());
      this->m_items.clear();
      endRemoveRows();
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);

    return 1;
  }

  bool appendEntry(const Item &item)
  {
    bool ret = true;

    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    try {
      this->m_items.emplace_back(item);
    } catch (std::bad_alloc&) {
      ret = false;
    }

    endInsertRows();
    return ret;
  }

  bool appendEntry(Item &&item)
  {
    bool ret = true;

    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    try {
      this->m_items.emplace_back(item);
    } catch (std::bad_alloc&) {
      ret = false;
    }

    endInsertRows();
    return ret;
  }

  void deleteByIdx(const int idx)
  {
    const size_t uidx = static_cast<size_t>(idx);
    if (idx < 0 || uidx >= m_items.size())
      return;

    beginRemoveRows(QModelIndex(), idx, m_items.size());
    m_items.erase(m_items.begin() + uidx);
    endRemoveRows();
  }

  int indexByHash(const DataHash &hash)
  {
    for (size_t idx = 0; idx < m_items.size(); idx++) {
      if (m_items.at(idx).hash == hash)
        return idx;
    }

    return -1;
  }

  int indexByItem(const Item &item)
  {
    for (size_t idx = 0; idx < m_items.size(); idx++) {
      if (m_items.at(idx) == item)
        return idx;
    }

    return -1;
  }

  QVariant data(const QModelIndex &index, int role) const override
  {
    if (!isIndexValid(index))
      return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
      if (item.id.length() > 0)
        return QString("%1 [%2]").arg(item.path).arg(item.id);
      return item.path;
    case Qt::UserRole + 1:
      return QVariant::fromValue<DataHash>(item.hash);
    }

    return {};
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);

    return createIndex(row, column);
  }

  QModelIndex parent(const QModelIndex &child) const override
  {
    Q_UNUSED(child);

    return QModelIndex();
  }

  int rowCount(const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);

    return m_items.size();
  }

private:
  bool isIndexValid(const QModelIndex &index) const
  {
    if (!index.isValid())
      return false;

    if (index.row() < 0)
      return false;

    return !(static_cast<size_t>(index.row()) >= m_items.size() || index.column() > 0 || index.column() < 0);
  }

  std::vector<Item> m_items;
};

#endif // FILESCOMBOBOXMODEL_H
