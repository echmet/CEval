#include "chemstationbatchloadmodel.h"

#define MAKE_LESS_THAN_CPR(member) \
  bool compare_lessThan_##member(const ChemStationBatchLoadModel::Entry &e1, const ChemStationBatchLoadModel::Entry &e2) { \
    return e1.member < e2.member; \
  }

#define MAKE_GREATER_THAN_CPR(member) \
  bool compare_greaterThan_##member(const ChemStationBatchLoadModel::Entry &e1, const ChemStationBatchLoadModel::Entry &e2) { \
    return e1.member > e2.member; \
  }

#define LESS_THAN_CPR(member) compare_lessThan_##member
#define GREATER_THAN_CPR(member) compare_greaterThan_##member

ChemStationBatchLoadModel::Entry::Entry(const QString &type, const QString &info, const ChemStationBatchLoader::Filter &filter) :
  type(type),
  info(info),
  filter(filter)
{
}

ChemStationBatchLoadModel::Entry::Entry(const Entry &other) :
  type(other.type),
  info(other.info),
  filter(other.filter)
{
}

ChemStationBatchLoadModel::Entry::Entry() :
  type(""),
  info(""),
  filter(ChemStationBatchLoader::Filter())
{
}

ChemStationBatchLoadModel::Entry & ChemStationBatchLoadModel::Entry::operator=(const Entry &other)
{
  const_cast<QString &>(type) = other.type;
  const_cast<QString &>(info) = other.info;
  const_cast<ChemStationBatchLoader::Filter&>(filter) = other.filter;

  return *this;
}

MAKE_LESS_THAN_CPR(type)
MAKE_LESS_THAN_CPR(info)
MAKE_GREATER_THAN_CPR(type)
MAKE_GREATER_THAN_CPR(info)

ChemStationBatchLoadModel::ChemStationBatchLoadModel(QObject *parent) :
  QAbstractItemModel(parent)
{
}

void ChemStationBatchLoadModel::clear()
{
  beginResetModel();
  m_entries.clear();
  endResetModel();
}

int ChemStationBatchLoadModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return 2;
}

QVariant ChemStationBatchLoadModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_entries.length() ||
      index.column() < 0 || index.column() > 1)
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return m_entries.at(index.row()).type;
      break;
    case 1:
      return m_entries.at(index.row()).info;
      break;
    default:
      return QVariant();
    }
  default:
    return QVariant();
  }
}

ChemStationBatchLoader::Filter ChemStationBatchLoadModel::filter(const QModelIndex &index) const
{
  if (index.row() < 0 || index.row() >= m_entries.size())
    return ChemStationBatchLoader::Filter();

  return m_entries.at(index.row()).filter;
}

QVariant ChemStationBatchLoadModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Vertical)
    return QVariant();

  switch (section) {
  case 0:
    return tr("Type");
    break;
  case 1:
    return tr("Additional info");
    break;
  default:
    return QVariant();
    break;
  }
}

QModelIndex ChemStationBatchLoadModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return createIndex(row, column);
}

QModelIndex ChemStationBatchLoadModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);

  return QModelIndex();
}

int ChemStationBatchLoadModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return m_entries.size();
}

void ChemStationBatchLoadModel::setNewData(const QVector<Entry> &entries)
{
  beginResetModel();
  m_entries = entries;
  endResetModel();
}

void ChemStationBatchLoadModel::sort(int column, Qt::SortOrder order)
{
  switch (column) {
  case 0:
    std::sort(m_entries.begin(), m_entries.end(), (order == Qt::AscendingOrder) ? LESS_THAN_CPR(type) : GREATER_THAN_CPR(type));
    break;
  case 1:
    std::sort(m_entries.begin(), m_entries.end(), (order == Qt::AscendingOrder) ? LESS_THAN_CPR(info) : GREATER_THAN_CPR(info));
    break;
  default:
    break;
  }
}
