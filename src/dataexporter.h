#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include "idataexportable.h"
#include <QDebug>

template<typename T>
class DataExporter {
private:
  typedef typename IDataExportable<T>::Retriever Retriever;

  class RegisteredExportable {
  public:
    explicit RegisteredExportable() :
      m_name(""),
      m_r([](const T *) { return QVariant(); })
    {}
    explicit RegisteredExportable(const QString &name, const Retriever &r) :
      m_name(name), m_r(r)
    {}
    explicit RegisteredExportable(const RegisteredExportable &other) :
      m_name(other.m_name), m_r(other.m_r)
    {}

    const QString & name() const
    {
      return m_name;
    }

    QVariant value(const T *exportee) const
    {
      return m_r(exportee);
    }

  private:
    const QString m_name;
    const Retriever m_r;

  };

public:
  bool exportAll(const T *exportee)
  {
    for (const RegisteredExportable &e : m_exportables) {
      qDebug() << e.name() << e.value(exportee);
    }

    return true;
  }

  void registerExportable(const QString &name, Retriever r)
  {
    m_exportables.push_back(RegisteredExportable(name, r));
  }

private:
  QVector<RegisteredExportable> m_exportables;

};

#endif // DATAEXPORTER_H
