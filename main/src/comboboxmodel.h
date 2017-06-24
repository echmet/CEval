#ifndef COMBOBOXMODEL_H
#define COMBOBOXMODEL_H

#include <QAbstractItemModel>

template<typename T>
struct ComboBoxItem {
  ComboBoxItem(const QString &description, const T &data) :
    description(description), data(data)  {}
  ComboBoxItem() :
    description(""),
    data(T())
  {
  }

  QString description;
  T data;
};

template<typename U>
class ComboBoxModel : public QAbstractItemModel
{
public:
  explicit ComboBoxModel(const QVector<ComboBoxItem<U>> &items, QObject *parent = nullptr) : QAbstractItemModel(parent),
    m_items(items)
  {
  }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override
  {
    Q_UNUSED(parent);

    return 1;
  }

  QVariant data(const QModelIndex &index, int role) const override
  {
    if (!isIndexValid(index))
      return QVariant();

    switch (role) {
    case Qt::DisplayRole:
      return m_items.at(index.row()).description;
      break;
    case Qt::UserRole + 1:
    {
      QVariant v;
      v.setValue(m_items.at(index.row()).data);
      return v;
      break;
    }
    default:
      return QVariant();
    }
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

    return m_items.length();
  }

protected:
  bool isIndexValid(const QModelIndex &index) const
  {
    return !(index.row() >= m_items.length() || index.row() < 0 || index.column() > 0 || index.column() < 0);
  }

  QVector<ComboBoxItem<U>> m_items;

};

#endif // COMBOBOXMODEL_H
