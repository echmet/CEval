#ifndef DYNAMICCOMBOBOXMODEL_H
#define DYNAMICCOMBOBOXMODEL_H

#include "comboboxmodel.h"

template<typename U>
class DynamicComboBoxModel : public ComboBoxModel<U>
{
public:
  explicit DynamicComboBoxModel(const QVector<ComboBoxItem<U>> &items, QObject *parent = nullptr) :
    ComboBoxModel<U>(items, parent)
  {
  }

  bool appendEntry(const ComboBoxItem<U> &item)
  {
    bool ret = true;

    this->beginInsertRows(QModelIndex(), this->m_items.length(), this->m_items.length());
    try {
      this->m_items.push_back(item);
    } catch (std::bad_alloc&) {
      ret = false;
    }

    this->endInsertRows();
    return ret;
  }

  void deleteByIdx(const int idx)
  {
    if (idx < 0 || idx >= ComboBoxModel<U>::m_items.length())
      return;

    this->beginRemoveRows(QModelIndex(), idx, ComboBoxModel<U>::m_items.length());
    this->m_items.remove(idx);
    this->endRemoveRows();
  }

  int indexByItem(const U &item)
  {
    for (int idx = 0; idx < this->m_items.count(); idx++) {
      if (this->m_items.at(idx).data == item)
        return idx;
    }

    return -1;
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role) override
  {
    if (!this->isIndexValid(index))
      return false;

    switch (role) {
    case Qt::DisplayRole:
      if (!value.canConvert<QString>())
        return false;

      this->m_items[index.row()].description = value.toString();
      return true;
      break;
    case Qt::UserRole + 1:
      if (!value.canConvert<U>())
        return false;

      this->m_items[index.row()].data = value.value<U>();
      return true;
      break;
    default:
      return false;
      break;
    }
  }

};

#endif // DYNAMICCOMBOBOXMODEL_H
