#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include "idataexportable.h"

template<typename T>
class DataExporter {
private:
  typedef typename IDataExportable<T>::Retriever Retriever;

  class RegisteredExportable {
  public:
    explicit RegisteredExportable() :
      name(""),
      r([](T *) { return QVariant(); })
    {}
    explicit RegisteredExportable(const QString &name, const Retriever &r) :
      name(name), r(r)
    {}
    explicit RegisteredExportable(const RegisteredExportable &other) :
      name(other.name), r(other.r)
    {}

    const QString name;
    const Retriever r;

  };

public:
  void registerExportable(const QString &name, Retriever r)
  {
    m_exportables.push_back(RegisteredExportable(name, r));
  }

private:
  QVector<RegisteredExportable> m_exportables;

};

#endif // DATAEXPORTER_H
