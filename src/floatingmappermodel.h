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

    const int rawIdx = index.column();

    dv = value.toDouble(&ok);
    if (!ok)
      return false;

    const double &current = (*(this->m_data)).at(rawIdx);
    if (current == dv)
      return true;

    (*(this->m_data))[rawIdx] = dv;
    emit this->dataChanged(this->createIndex(0, rawIdx), this->createIndex(0, rawIdx), { role });
    return true;
  }

};

#endif // FLOATINGMAPPERMODEL_H
