#ifndef ABSTRACTMAPPERMODEL_H
#define ABSTRACTMAPPERMODEL_H

#include <QAbstractItemModel>

template<typename T, typename I>
class AbstractMapperModel : public QAbstractItemModel
{
public:
  AbstractMapperModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_firstIndex(createIndex(0, 0)),
    m_maxColumns(0)
  {
  }

  int columnCount(const QModelIndex &parent) const override
  {
    Q_UNUSED(parent);

    return m_maxColumns;
  }

  virtual QVariant data(const QModelIndex &index, int role) const override
  {
    if (!isParametersValid(index, role))
      return QVariant();

    if (m_data == nullptr)
      return QVariant();

    return m_data->at(index.column());
  }

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);

    return createIndex(row, column);
  }

  int indexFromItem(const I item) const
  {
    return static_cast<int>(item);
  }

  template<typename X>
  I itemFromIndex(const X x)
  {
    Q_STATIC_ASSERT_X((std::is_same<X, typename std::underlying_type<I>::type>::value), "Passed parameter has different type than the enumeration underlying type.");

    const typename std::underlying_type<I>::type idx = static_cast<typename std::underlying_type<I>::type>(x);
    Q_ASSERT(idx >= 0 && idx <= static_cast<typename std::underlying_type<I>::type>(I::LAST_INDEX));

    return static_cast<I>(idx);
  }

  void notifyAllDataChanged(const QVector<int> &roles = { Qt::DisplayRole })
  {
    emit this->dataChanged(m_firstIndex,
                           m_lastIndex,
                           roles);
  }

  void notifyDataChanged(const I &fromIndex, const I &toIndex, const QVector<int> &roles = { Qt::DisplayRole })
  {
    emit this->dataChanged(this->index(0, this->indexFromItem(fromIndex)),
                           this->index(0, this->indexFromItem(toIndex)),
                           roles);
  }

  void notifyDataChangedFromStart(const I &toIndex, const QVector<int> &roles = { Qt::DisplayRole })
  {
    emit this->dataChanged(m_firstIndex,
                           this->index(0, this->indexFromItem(toIndex)),
                           roles);
  }

  void notifyDataChangedToEnd(const I &fromIndex, const QVector<int> &roles = { Qt::DisplayRole })
  {
    emit this->dataChanged(this->index(0, this->indexFromItem(fromIndex)),
                           m_lastIndex,
                           roles);
  }

  QModelIndex parent(const QModelIndex &child) const override
  {
    Q_UNUSED(child);

    return QModelIndex();
  }

  int rowCount(const QModelIndex &parent) const override
  {
    Q_UNUSED(parent);

    return 1;
  }

  virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override = 0;

  void setUnderlyingData(QVector<T> *data)
  {
    m_data = data;
    m_maxColumns = m_data->length();
    m_lastIndex = createIndex(0, m_maxColumns - 1);
    emit dataChanged(m_firstIndex, m_lastIndex, { Qt::EditRole });
  }

protected:
  virtual bool isParametersValid(const QModelIndex &index, const int role, QVector<int> allowedRoles = { Qt::EditRole }) const
  {
    if (index.row() != 0)
      return false;
    if (index.column() < 0 || index.column() >= m_maxColumns)
      return false;
    if (!allowedRoles.contains(role))
      return false;

    return true;
  }

  QVector<T> *m_data;

private:
  const QModelIndex m_firstIndex;
  QModelIndex m_lastIndex;
  int m_maxColumns;
};

#endif // ABSTRACTMAPPERMODEL_H
