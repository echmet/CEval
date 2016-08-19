#include "exporterelems.h"

using namespace DataExporter;

const char * ExportableExistsException::what() const noexcept
{
  return "Exportable already exists in the map";
}

const char * InvalidExportableException::what() const noexcept
{
  return "Unable to cast IExportable object to the expected type";
}

IExportable::~IExportable()
{
}

ExportableRoot::ExportableRoot(const QString &name) :
  name(name)
{
}

ExportableRoot::~ExportableRoot()
{
}

SelectedExportable::SelectedExportable() :
  m_exportable(new Exportable<>())
{
}

SelectedExportable::SelectedExportable(const ExportableRoot *exportable) :
  m_exportable(exportable)
{
}

const QString & SelectedExportable::name() const
{
  return m_exportable->name;
}

QVariant SelectedExportable::value(const IExportable *exportee) const
{
  return m_exportable->value(exportee);
}

SchemeBaseRoot::SchemeBaseRoot(const QString &name, const QString &description,
                              const ExportablesMap &exportables, const SchemeTypes type) :
  name(name),
  description(description),
  exportables(exportables),
  type(type)
{
}

SchemeBaseRoot::~SchemeBaseRoot()
{
  for (const ExportableRoot *e : exportables)
    delete e;
}

Scheme::Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base, const Globals::DataArrangement arrangement,
               const QChar &delimiter) :
  name(name),
  selectedExportables(seMap),
  arrangement(arrangement),
  delimiter(delimiter),
  m_base(base)
{
}

Scheme::~Scheme()
{
  for (const SelectedExportable *se : selectedExportables)
    delete se;
}

QString Scheme::baseName() const
{
  return m_base->name;
}

bool Scheme::exportData(const IExportable *exportee) const
{
  return m_base->exportData(exportee, selectedExportables);
}
