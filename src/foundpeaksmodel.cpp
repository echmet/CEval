#include "foundpeaksmodel.h"

FoundPeaksModel::Peak::Peak(const double time, const long index) :
  index(index),
  time(time)
{
}

FoundPeaksModel::Peak::Peak(const Peak &other) :
  index(other.index),
  time(other.time)
{
}

FoundPeaksModel::Peak::Peak() :
  index(0),
  time(0.0)
{
}

FoundPeaksModel::Peak &FoundPeaksModel::Peak::operator=(const Peak &other)
{
  const_cast<double&>(this->time) = other.time;
  const_cast<long&>(this->index) = other.index;

  return *this;
}

FoundPeaksModel::FoundPeaksModel(const QVector<Peak> &data, QObject *parent) :
  QAbstractItemModel(parent),
  m_data(data)
{
}

int FoundPeaksModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return 2;
}

QVariant FoundPeaksModel::data(const QModelIndex &index, int role) const
{
  if (index.row() < 0 || index.row() >= m_data.length())
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return index.row() + 1;
      break;
    case 1:
      return m_data.at(index.row()).time;
      break;
    default:
      return QVariant();
      break;
    }
  case Qt::UserRole + 1:
    switch (index.column()) {
    case 0:
    {
      QVariant var;
      var.setValue(m_data.at(index.row()).index);
      return var;
      break;
    }
    default:
      return QVariant();
      break;
    }
  default:
    return QVariant();
    break;
  }
}

QVariant FoundPeaksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation != Qt::Horizontal)
    return QVariant();

  switch (section) {
  case 0:
    return "Number";
    break;
  case 1:
    return "Time of maximum";
    break;
  default:
    return QVariant();
    break;
  }
}

QModelIndex FoundPeaksModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return createIndex(row, column);
}

QModelIndex FoundPeaksModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);

  return QModelIndex();
}

int FoundPeaksModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return m_data.length();
}





