#include "dataexporter.h"

using namespace DataExporter;

const char * InvalidExportableException::what() const noexcept
{
  return "Unable to cast IExportable object to the expected type";
}

ExportableRoot::ExportableRoot(const QString &name) :
  name(name)
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

Scheme::Scheme(const QString &name, const SelectedExportablesMap &seMap, const SchemeBaseRoot *base) :
  name(name),
  selectedExportables(seMap),
  m_base(base)
{
}

bool Scheme::exportData(const IExportable *exportee) const
{
  return m_base->exportData(exportee, selectedExportables);
}
