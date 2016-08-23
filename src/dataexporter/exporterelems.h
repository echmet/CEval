#ifndef EXPORTERELEMS_H
#define EXPORTERELEMS_H

#include "exporterglobals.h"
#include "backends/abstractexporterbackend.h"
#include <functional>
#include <QVariant>
#include <QPoint>

namespace DataExporter {

class ExportableExistsException : std::exception {
public:
  const char * what() const noexcept;

};

class InvalidExportableException : std::exception {
public:
  const char * what() const noexcept;

};

class UnknownExportableException : std::exception {
public:
  const char * what() const noexcept;

};

class IExportable {
public:
  virtual ~IExportable();
};

class ExportableRoot {
public:
  explicit ExportableRoot(const QString &name);
  virtual ~ExportableRoot();
  virtual QVariant value(const IExportable *exportee) const = 0;

  const QString name;

};

template<typename T = IExportable>
class Exportable : public ExportableRoot {
public:
  typedef std::function<QVariant (const T *)> Retriever;

  explicit Exportable() :
    ExportableRoot(""),
    m_retriever([](const T *) { return QVariant(); })
  {
  }
  explicit Exportable(const QString &name, const Retriever r) :
    ExportableRoot(name),
    m_retriever(r)
  {
  }

  virtual QVariant value(const IExportable *exportee) const override
  {
    const T *_exportee = dynamic_cast<const T *>(exportee);
    if (_exportee == nullptr)
      throw InvalidExportableException();

    return m_retriever(_exportee);
  }

private:
  Retriever m_retriever;

};

class SelectedExportable {
public:
  explicit SelectedExportable();
  explicit SelectedExportable(const ExportableRoot *exportable, const int position, const QString &displayName);

  const QString & name() const;

  const QString displayName;
  const int position;
  QVariant value(const IExportable *exportee) const;

private:
  const ExportableRoot *m_exportable;

};

typedef QMap<QString, ExportableRoot *> ExportablesMap;
typedef QMap<QString, SelectedExportable *> SelectedExportablesMap;

class SchemeBaseRoot {
public:
  explicit SchemeBaseRoot(const QString &name, const QString &description,
                          const ExportablesMap &exportables, const bool dontFree = false);
  virtual ~SchemeBaseRoot();

  virtual bool exportData(const IExportable *exportee, const SelectedExportablesMap &seMap, AbstractExporterBackend &backend) const = 0;

  const QString name;
  const QString description;
  const ExportablesMap exportables;

private:
  const bool m_dontFree;

};

template<typename T>
class SchemeBase : public SchemeBaseRoot {
public:
  typedef std::function<bool (const T *, const SelectedExportablesMap &, AbstractExporterBackend &)> Executor;

  explicit SchemeBase(const QString &name, const QString &description,
                      const ExportablesMap &exportables,
                      Executor executor = [](const T *exportee, const SelectedExportablesMap &seMap, AbstractExporterBackend &backend) {
                        for (const SelectedExportable *se : seMap) {
                          try {
                            backend.addCell(new AbstractExporterBackend::Cell(se->displayName, se->value(exportee)), 0, se->position);
                          } catch (std::bad_alloc &) {
                            return false;
                          }
                        }

                        return backend.exportData();
                      }, const bool dontFree = false) :
    SchemeBaseRoot(name, description, exportables, dontFree),
    m_executor(executor)
  {
  }

  virtual bool exportData(const IExportable *exportee, const SelectedExportablesMap &seMap, AbstractExporterBackend &backend) const override
  {
    const T *_exportee = dynamic_cast<const T *>(exportee);
    Q_ASSERT(_exportee != nullptr);

    return m_executor(_exportee, seMap, backend);
  }

private:
  Executor m_executor;

};

class Scheme {
public:
  explicit Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base, const Globals::DataArrangement arrangement);
  ~Scheme();

  QString baseName() const;
  bool exportData(const IExportable *exportee, AbstractExporterBackend &backend) const;

  const QString name;
  const SelectedExportablesMap selectedExportables;
  const Globals::DataArrangement arrangement;

private:
  const SchemeBaseRoot *m_base;

};

typedef QMap<QString, const SchemeBaseRoot *> SchemeBasesMap;
typedef QMap<QString, Scheme *> SchemesMap;

} // namespace DataExporter

#endif // EXPORTERELEMS_H
