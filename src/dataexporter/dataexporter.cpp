#include "dataexporter.h"
#include "schemecreator.h"

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

Exporter::Exporter()
{
  m_schemeCreator = new SchemeCreator();
}

Exporter::~Exporter()
{
  for (const Scheme *s : m_schemes)
    delete s;

  for (const SchemeBaseRoot *s : m_schemeBases)
    delete s;
}

Scheme *Exporter::createScheme()
{
  const SchemeCreator::NewScheme ns = m_schemeCreator->interact();
  if (!ns.isValid)
    return nullptr;

  if (!m_schemeBases.contains(ns.baseName))
    return nullptr;

  const SchemeBaseRoot *sbr = m_schemeBases.value(ns.baseName);
  SelectedExportablesMap seMap;

  for (const QString &s : ns.exportables) {
    if (!sbr->exportables.contains(s))
      return nullptr;

    seMap.insert(s, new SelectedExportable(sbr->exportables.value(s)));
  }

  Scheme *s = new Scheme(ns.name, seMap, sbr);

  return s;
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

  QStringList exportables;
  for (const ExportableRoot *e : schemeBase->exportables)
    exportables << e->name;

  return m_schemeCreator->registerSchemeBase(SchemeCreator::SchemeBase(schemeBase->name, schemeBase->description, exportables));
}
