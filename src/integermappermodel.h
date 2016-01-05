#ifndef INTEGERMAPPERMODEL_H
#define INTEGERMAPPERMODEL_H

#include "abstractmappermodel.h"

template<typename I>
class IntegerMapperModel : public AbstractMapperModel<int, I>
{
public:
  IntegerMapperModel(QObject *parent = nullptr) :
    AbstractMapperModel<int, I>(parent)
  {
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role)
  {
    bool ok;
    int iv;

    if (!this->isParametersValid(index, role))
      return false;

    if (this->m_data == nullptr)
      return false;

    iv = value.toInt(&ok);
    if (!ok)
      return false;

    (*(this->m_data))[index.column()] = iv;
    emit this->dataChanged(this->createIndex(0, index.column()), this->createIndex(0, index.column()), { role });
    return true;
  }

};

#endif // INTEGERMAPPERMODEL_H
