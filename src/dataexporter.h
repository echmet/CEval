#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include "idataexportable.h"
#include <QPoint>
#include <QDebug>

template<typename T>
class DataExporter {
private:
  typedef typename IDataExportable<T>::Retriever Retriever;

public:
  enum class CaptionPosition {
    NONE,
    ABOVE,
    LEFT_OF
  };

  class Exportable {
  public:
    explicit Exportable() :
      m_name(""),
      m_r([](const T *) { return QVariant(); })
    {}
    explicit Exportable(const QString &name, const Retriever &r) :
      m_name(name), m_r(r)
    {}
    explicit Exportable(const Exportable &other) :
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

  class SelectedExportable {
  public:
    explicit SelectedExportable() :
      m_exportable(Exportable()),
      m_captionPosition(CaptionPosition::NONE)
    {}
    explicit SelectedExportable(const Exportable &exportable, const QPoint &position, const CaptionPosition captionPosition) :
      m_exportable(exportable),
      m_position(position),
      m_captionPosition(captionPosition)
    {}

    QString name() const
    {
      return m_exportable.name();
    }

    QVariant value(const T *exportee) const
    {
      return m_exportable.value(exportee);
    }

  private:
    const Exportable *m_exportable;
    QPoint m_position;
    CaptionPosition m_captionPosition;
  };

  typedef QMap<QString, Exportable *> ExportablesMap;
  typedef QMap<QString, SelectedExportable *> SelectedExportablesMap;
  typedef std::function<bool (const T *, const SelectedExportablesMap &)> Executor;

  enum class SchemeTypes {
    NONE,
    SINGLE_ITEM,
    LIST
  };

  class SchemeBase {
  public:
    explicit SchemeBase() :
      name(""), description(""), exportables(ExportablesMap()), type(SchemeTypes::NONE),
      m_executor([](const T *) { return false; })
    {}
    explicit SchemeBase(const QString &name, const QString &description,
                        const QMap<QString, Exportable> exportables, const SchemeTypes type,
                        Executor executor = [](const T *exportee, const SelectedExportablesMap &seMap)
    {
      for (const SelectedExportable &se : seMap)
        qDebug() << se.name() << se.value(exportee);

      return true;
    }) :
      name(name),
      description(description),
      exportables(exportables),
      type(type),
      m_executor(executor)
    {}

    const QString name;
    const QString description;
    const ExportablesMap exportables;
    const SchemeTypes type;

    bool operator()(const T *exportee, const SelectedExportablesMap &seMap) const
    {
      return m_executor(exportee, seMap);
    }

  private:
    const Executor m_executor;

  };

  class Scheme {
  public:
    explicit Scheme() :
      name(""),
      m_base(new SchemeBase())
    {}

    explicit Scheme(const QString &name, const SchemeBase *base) :
      name(name),
      m_base(base)
    {}

    bool exportData(const T *exportee) const
    {
      return m_base(exportee, m_selectedExportables);
    }

    const QString name;

  private:
    const SchemeBase *m_base;
    SelectedExportablesMap m_selectedExportables;

  };

  typedef QMap<QString, SchemeBase *> SchemeBasesMap;
  typedef QMap<QString, Scheme *> SchemesMap;

  void registerScheme(Scheme *scheme)
  {
    if (m_schemes.contains(scheme->name))
      return;

    m_schemes.insert(scheme->name, scheme);
  }

  void registerSchemeBase(SchemeBase *base)
  {
    if (m_schemeBases.contains(base->name))
      return;

    m_schemeBases.insert(base->name, base);
  }

private:
  SchemesMap m_schemes;
  SchemeBasesMap m_schemeBases;

};

#endif // DATAEXPORTER_H
