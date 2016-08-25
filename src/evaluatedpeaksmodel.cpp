#include "evaluatedpeaksmodel.h"
#include "doubletostringconvertor.h"

EvaluatedPeaksModel::EvaluatedPeak::EvaluatedPeak() :
  name(""),
  time(0.0),
  area(0.0)
{
}

EvaluatedPeaksModel::EvaluatedPeak::EvaluatedPeak(const QString &name, const double time, const double area) :
  name(name),
  time(time),
  area(area)
{
}

EvaluatedPeaksModel::EvaluatedPeak &EvaluatedPeaksModel::EvaluatedPeak::operator=(const EvaluatedPeak &other)
{
  const_cast<QString&>(name) = other.name;
  const_cast<double&>(time) = other.time;
  const_cast<double&>(area) = other.area;

  return *this;
}

EvaluatedPeaksModel::EvaluatedPeaksModel(QObject *parent) :
  QAbstractItemModel(parent)
{
}

bool EvaluatedPeaksModel::appendEntry(const EvaluatedPeak &peak)
{
  bool ret;

  emit beginInsertRows(QModelIndex(), m_data.length(), m_data.length());
  try {
    m_data.push_back(peak);
    ret = true;
  } catch (std::bad_alloc&) {
    ret = false;
  }
  emit endInsertRows();

  return ret;
}

void EvaluatedPeaksModel::clearEntries()
{
  if (m_data.length() == 0)
    return;

  emit beginRemoveRows(QModelIndex(), 0, m_data.length() - 1);
  m_data.clear();
  emit endRemoveRows();
}

int EvaluatedPeaksModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return 3;
}

QVariant EvaluatedPeaksModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();
  if (index.row() < 0 || index.row() >= m_data.length())
    return QVariant();

  const int row = index.row();

  switch (role) {
  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return m_data.at(row).name;
    case 1:
      return DoubleToStringConvertor::convert(m_data.at(row).time);
      break;
    case 2:
      return DoubleToStringConvertor::convert(m_data.at(row).area);
      break;
    default:
      return QVariant();
      break;
    }
  default:
    return QVariant();
    break;
  }
}

QVariant EvaluatedPeaksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical)
    return QVariant();

  switch (section) {
  case 0:
    return QVariant("Analyte");
    break;
  case 1:
    return QVariant("Time");
    break;
  case 2:
    return QVariant("Area");
    break;
  default:
    return QVariant();
    break;
  }
}

QModelIndex EvaluatedPeaksModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return createIndex(row, column);
}

QModelIndex EvaluatedPeaksModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);

  return QModelIndex();
}

void EvaluatedPeaksModel::removeEntry(const int idx)
{
  if (idx < 0 || idx >= m_data.length())
    return;

  emit beginRemoveRows(QModelIndex(), idx, idx);
  m_data.remove(idx);
  emit endRemoveRows();
}

int EvaluatedPeaksModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);

  return m_data.length();
}

void EvaluatedPeaksModel::setEntries(const QVector<EvaluatedPeak> &entries)
{
  clearEntries();

  if (entries.length() == 0)
    return;

  emit beginInsertRows(QModelIndex(), 0, entries.length() - 1);
  m_data = entries;
  emit endInsertRows();
}

void EvaluatedPeaksModel::updateEntry(const int idx, const EvaluatedPeak &peak)
{
  if (idx < 0 || idx >= m_data.length())
    return;

  m_data[idx] = peak;
  emit dataChanged(createIndex(idx, 0), createIndex(idx, 2), { Qt::DisplayRole });
}
void EvaluatedPeaksModel::updateEntry(const int idx, const double time, const double area)
{
  if (idx < 0 || idx >= m_data.length())
    return;

  EvaluatedPeak evp(m_data.at(idx).name, time, area);
  updateEntry(idx, evp);
}

