#ifndef STRINGMAPPERMODEL_H
#define STRINGMAPPERMODEL_H

#include "abstractmappermodel.h"

template<typename I>
class StringMapperModel : public AbstractMapperModel<QString, I>
{
public:
  StringMapperModel(QObject *parent = nullptr) :
    AbstractMapperModel<QString, I>(parent)
  {
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role)
  {
    if (!this->isParametersValid(index, role))
      return false;

    if (this->m_data == nullptr)
      return false;

    (*(this->m_data))[index.column()] = value.toString();
    emit this->dataChanged(this->createIndex(0, index.column()), this->createIndex(0, index.column()), { role });
    return true;
  }

};

#endif // STRINGMAPPERMODEL_H
