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

const char * UnknownExportableException::what() const noexcept
{
  return "Unknown exportable ID";
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
  displayName(""),
  position(-1),
  m_exportable(new Exportable<>())
{
}

SelectedExportable::SelectedExportable(const ExportableRoot *exportable, const int position, const QString &displayName) :
  displayName(displayName),
  position(position),
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
                              const ExportablesMap &exportables, const bool dontFree) :
  name(name),
  description(description),
  exportables(exportables),
  m_dontFree(dontFree)
{
}

SchemeBaseRoot::~SchemeBaseRoot()
{
  if (m_dontFree)
    return;

  for (const ExportableRoot *e : exportables)
    delete e;
}

/*
const SchemeBase::Executor SchemeBase::DEFAULT_EXECUTOR = [](const T *exportee, const SelectedExportablesMap &seMap, AbstractExporterBackend &backend, const uint32_t opts) {
    for (const SelectedExportable *se : seMap) {
      (void)opts;
      try {
        backend.addCell(new AbstractExporterBackend::Cell(se->displayName, se->value(exportee)), 0, se->position);
      } catch (std::bad_alloc &) {
        return false;
      }
    }

    return backend.exportData();
  };
  */

Scheme::Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base, const Globals::DataArrangement arrangement) :
  name(name),
  selectedExportables(seMap),
  arrangement(arrangement),
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

bool Scheme::exportData(const IExportable *exportee, AbstractExporterBackend &backend, const uint32_t opts) const
{
  return m_base->exportData(exportee, selectedExportables, backend, opts);
}
