#include "dataexporter.h"

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
  m_exportable(new Exportable<>()),
  m_captionPosition(CaptionPosition::NONE)
{
}

SelectedExportable::SelectedExportable(const ExportableRoot *exportable, const QPoint &position, const CaptionPosition captionPosition) :
  m_exportable(exportable),
  m_position(position),
  m_captionPosition(captionPosition)
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

Scheme::Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base) :
  name(name),
  selectedExportables(seMap),
  m_base(base)
{
}

Scheme::~Scheme()
{
  for (const SelectedExportable *se : selectedExportables)
    delete se;
}

bool Scheme::exportData(const IExportable *exportee) const
{
  return m_base->exportData(exportee, selectedExportables);
}

Exporter::~Exporter()
{
  for (const Scheme *s : m_schemes)
    delete s;

  for (const SchemeBaseRoot *s : m_schemeBases)
    delete s;
}

bool Exporter::registerScheme(Scheme *scheme)
{
  if (m_schemes.contains(scheme->name))
    return false;

  m_schemes.insert(scheme->name, scheme);

  return true;
}

bool Exporter::registerSchemeBase(const SchemeBaseRoot *schemeBase)
{
  if (m_schemeBases.contains(schemeBase->name))
    return false;

  m_schemeBases.insert(schemeBase->name, schemeBase);

  return true;
}
