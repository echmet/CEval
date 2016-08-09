#include "chemstationfileinfomodel.h"
#include <QPalette>

#define LESS_THAN_CPR(member) \
   [](const Entry &e1, const Entry &e2) -> bool { \
    return e1.member < e2.member; \
  }
#define GREATER_THAN_CPR(member) \
   [](const Entry &e1, const Entry &e2) -> bool { \
    return e1.member > e2.member; \
  }

ChemStationFileInfoModel::Entry::Entry(const QString &name, const QString &type, const QString &info, const bool isChFile) :
  name(name),
  type(type),
  info(info),
  isChFile(isChFile)
{
}

ChemStationFileInfoModel::Entry::Entry(const Entry &other) :
  name(other.name),
  type(other.type),
  info(other.info),
  isChFile(other.isChFile)
{
}

ChemStationFileInfoModel::Entry::Entry() :
  name(""),
  type(""),
  info(""),
  isChFile(false)
{
}

ChemStationFileInfoModel::Entry &ChemStationFileInfoModel::Entry::operator=(const Entry &other)
{
  const_cast<QString &>(name) = other.name;
  const_cast<QString &>(type) = other.type;
  const_cast<QString &>(info) = other.info;
  const_cast<bool &>(isChFile) = other.isChFile;

  return *this;
}

ChemStationFileInfoModel::ChemStationFileInfoModel(QObject *parent) :
  QAbstractItemModel(parent)
{
}

void ChemStationFileInfoModel::clear()
{
  beginResetModel();
  m_entries.clear();
  endResetModel();
}

int ChemStationFileInfoModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return 3;
}

QVariant ChemStationFileInfoModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_entries.length() ||
      index.column() < 0 || index.column() > 2)
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return m_entries.at(index.row()).name;
      break;
    case 1:
      return m_entries.at(index.row()).type;
      break;
    case 2:
      return m_entries.at(index.row()).info;
      break;
    default:
      return QVariant();
    }
    break;
  case Qt::TextColorRole:
    {
      QPalette palette;
      if (m_entries.at(index.row()).isChFile)
        return palette.color(QPalette::Active, QPalette::WindowText);
      else
        return palette.color(QPalette::Disabled, QPalette::WindowText);
    }
    break;
  default:
    return QVariant();
  }
}

QVariant ChemStationFileInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();
  if (orientation == Qt::Vertical)
    return QVariant();

  switch (section) {
  case 0:
    return QVariant(tr("File name"));
    break;
  case 1:
    return QVariant(tr("File type"));
    break;
  case 2:
    return QVariant(tr("Additional info"));
    break;
  default:
    return QVariant();
    break;
  }
}

QModelIndex ChemStationFileInfoModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return createIndex(row, column);
}

QModelIndex ChemStationFileInfoModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);

  return QModelIndex();
}

int ChemStationFileInfoModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)

  return m_entries.length();
}

void ChemStationFileInfoModel::setNewData(const QVector<Entry> &entries)
{
  beginResetModel();
  m_entries = entries;
  endResetModel();
}

void ChemStationFileInfoModel::sort(int column, Qt::SortOrder order)
{
  switch (column) {
  case 0:
    qSort(m_entries.begin(), m_entries.end(), (order == Qt::AscendingOrder) ? LESS_THAN_CPR(name) : GREATER_THAN_CPR(name));
    break;
  case 1:
    qSort(m_entries.begin(), m_entries.end(), (order == Qt::AscendingOrder) ? LESS_THAN_CPR(type) : GREATER_THAN_CPR(type));
    break;
  case 2:
    qSort(m_entries.begin(), m_entries.end(), (order == Qt::AscendingOrder) ? LESS_THAN_CPR(info) : GREATER_THAN_CPR(info));
    break;
  default:
    break;
  }
}
