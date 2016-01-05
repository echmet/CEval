#ifndef BOOLEANMAPPERMODEL_H
#define BOOLEANMAPPERMODEL_H

#include "abstractmappermodel.h"

template<typename I>
class BooleanMapperModel : public AbstractMapperModel<bool, I>
{
public:
  BooleanMapperModel(QObject *parent = nullptr) :
    AbstractMapperModel<bool, I>(parent)
  {
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role)
  {
    if (!this->isParametersValid(index, role))
      return false;

    if (this->m_data == nullptr)
      return false;

    (*(this->m_data))[index.column()] = value.toBool();
    emit this->dataChanged(this->createIndex(0, index.column()), this->createIndex(0, index.column()), { role });
    return true;
  }

};

#endif // BOOLEANMAPPERMODEL_H
