#ifndef EXPORTERELEMS_H
#define EXPORTERELEMS_H

#include <functional>
#include <QVariant>
#include <QPoint>
#include <QDebug>

namespace DataExporter {

class SelectSchemeWidget;
class SchemeCreator;

enum class CaptionPosition {
  NONE,
  ABOVE,
  LEFT_OF
};

enum class SchemeTypes {
  NONE,
  SINGLE_ITEM,
  LIST
};

class ExportableExistsException : std::exception {
public:
  const char * what() const noexcept;

};

class InvalidExportableException : std::exception {
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
  explicit SelectedExportable(const ExportableRoot *exportable, const QPoint &position = QPoint(0, 0), const CaptionPosition captionPosition = CaptionPosition::NONE);

  const QString & name() const;
  QVariant value(const IExportable *exportee) const;

private:
  const ExportableRoot *m_exportable;
  QPoint m_position;
  CaptionPosition m_captionPosition;

};

typedef QMap<QString, ExportableRoot *> ExportablesMap;
typedef QMap<QString, SelectedExportable *> SelectedExportablesMap;

class SchemeBaseRoot {
public:
  explicit SchemeBaseRoot(const QString &name, const QString &description,
                          const ExportablesMap &exportables, const SchemeTypes type);
  virtual ~SchemeBaseRoot();

  virtual bool exportData(const IExportable *exportee, const SelectedExportablesMap &seMap) const = 0;

  const QString name;
  const QString description;
  const ExportablesMap exportables;
  const SchemeTypes type;

};

template<typename T>
class SchemeBase : public SchemeBaseRoot {
public:
  typedef std::function<bool (const T *, const SelectedExportablesMap &)> Executor;

  explicit SchemeBase(const QString &name, const QString &description,
                      const ExportablesMap &exportables, const SchemeTypes type,
                      Executor executor = [](const T *exportee, const SelectedExportablesMap &seMap) {
                        for (const SelectedExportable *se : seMap)
                          qDebug() << se->name() << se->value(exportee);

                        return true;
                      }) :
    SchemeBaseRoot(name, description, exportables, type),
    m_executor(executor)
  {
  }

  virtual bool exportData(const IExportable *exportee, const SelectedExportablesMap &seMap) const override
  {
    const T *_exportee = dynamic_cast<const T *>(exportee);
    Q_ASSERT(_exportee != nullptr);

    return m_executor(_exportee, seMap);
  }

private:
  Executor m_executor;

};

class Scheme {
public:
  explicit Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base);
  ~Scheme();

  bool exportData(const IExportable *exportee) const;

  const QString name;
  const SelectedExportablesMap selectedExportables;

private:
  const SchemeBaseRoot *m_base;

};

} // namespace DataExporter

#endif // EXPORTERELEMS_H
