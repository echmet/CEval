#ifndef FLOATINGMAPPERMODEL_H
#define FLOATINGMAPPERMODEL_H

#include "abstractmappermodel.h"

template<typename I>
class FloatingMapperModel : public AbstractMapperModel<double, I>
{
public:
  explicit FloatingMapperModel(QObject *parent = nullptr) :
    AbstractMapperModel<double, I>(parent)
  {
  }

  bool setData(const QModelIndex &index, const QVariant &value, int role)
  {
    bool ok;
    double dv;

    if (!this->isParametersValid(index, role))
      return false;

    if (this->m_data == nullptr)
      return false;

    dv = value.toDouble(&ok);
    if (!ok)
      return false;

    (*(this->m_data))[index.column()] = dv;
    emit this->dataChanged(this->createIndex(0, index.column()), this->createIndex(0, index.column()), { role });
    return true;
  }

};

#endif // FLOATINGMAPPERMODEL_H
