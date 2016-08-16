#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include "idataexportable.h"
#include <QPoint>
#include <QDebug>

class ExportableBase {};
class SelectedExportableBase {
public:
  virtual const QString & name() const;
  QVariant value(const void *) const
  {
    return QVariant();
  }
};

  enum class CaptionPosition {
    NONE,
    ABOVE,
    LEFT_OF
  };

  template<typename T>
  class Exportable : public ExportableBase {
  //typedef typename IDataExportable<T>::Retriever Retriever;
  typedef std::function<QVariant (const T *)> Retriever;

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

    QVariant valueInternal(const T *exportee) const
    {
      return m_r(exportee);
    }

  private:
    const QString m_name;
    const Retriever m_r;

  };

  template<typename T>
  class SelectedExportable {
  public:
    explicit SelectedExportable() :
      m_exportable(Exportable<T>()),
      m_captionPosition(CaptionPosition::NONE)
    {}
    explicit SelectedExportable(const Exportable<T> &exportable, const QPoint &position, const CaptionPosition captionPosition) :
      m_exportable(exportable),
      m_position(position),
      m_captionPosition(captionPosition)
    {}

    const QString & name() const override
    {
      return m_exportable.name();
    }

    QVariant value(const T *exportee) const
    {
      return m_exportable.value(exportee);
    }

  private:
    const Exportable<T> *m_exportable;
    QPoint m_position;
    CaptionPosition m_captionPosition;
  };


class DataExporter {
private:

public:
  typedef QMap<QString, ExportableBase *> ExportablesMap;
  typedef QMap<QString, SelectedExportableBase *> SelectedExportablesMap;

  enum class SchemeTypes {
    NONE,
    SINGLE_ITEM,
    LIST
  };

  class SchemeBaseRoot {
  public:
    explicit SchemeBaseRoot(const QString &name, const QString &description) :
      name(name), description(description)
    {}

    const QString name;
    const QString description;

  };
  class SchemeRoot {
  public:
    explicit SchemeRoot(const QString &name) :
      name(name)
    {}

    const QString name;

  };

  template<typename T>
  class SchemeBase : public SchemeBaseRoot {
  public:
    typedef std::function<bool (const T *, const SelectedExportablesMap &)> Executor;

    explicit SchemeBase() :
      SchemeBaseRoot("", ""),
      exportables(ExportablesMap()), type(SchemeTypes::NONE),
      m_executor([](const T *) { return false; })
    {}
    explicit SchemeBase(const QString &name, const QString &description,
                        const ExportablesMap &exportables, const SchemeTypes type,
                        Executor executor = [](const T *exportee, const SelectedExportablesMap &seMap)
    {
      for (const SelectedExportableBase *se : seMap)
        qDebug() << se->name() << se->value(exportee);

      return true;
    }) :
      SchemeBaseRoot(name, description),
      exportables(exportables),
      type(type),
      m_executor(executor)
    {}

    const ExportablesMap exportables;
    const SchemeTypes type;

    bool operator()(const T *exportee, const SelectedExportablesMap &seMap) const
    {
      return m_executor(exportee, seMap);
    }

  private:
    const Executor m_executor;

  };

  template<typename T>
  class Scheme : public SchemeRoot {
  public:
    explicit Scheme() :
      SchemeRoot(""),
      m_base(new SchemeBase<T>())
    {}

    explicit Scheme(const QString &name, const SchemeBase<T> *base) :
      SchemeRoot(name),
      m_base(base)
    {}

    bool exportData(const T *exportee) const
    {
      return m_base(exportee, m_selectedExportables);
    }


  private:
    const SchemeBase<T> *m_base;
    SelectedExportablesMap m_selectedExportables;

  };

  typedef QMap<QString, SchemeBaseRoot *> SchemeBasesMap;
  typedef QMap<QString, SchemeRoot *> SchemesMap;

  void registerScheme(SchemeRoot *scheme)
  {
    if (m_schemes.contains(scheme->name))
      return;

    m_schemes.insert(scheme->name, scheme);
  }

  void registerSchemeBase(SchemeBaseRoot *base)
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
