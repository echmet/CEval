#include "schemeserializer.h"
#include <QDataStream>
#include <QFile>

using namespace DataExporter;

SchemeSerializer::RetCode SchemeSerializer::deserializeScheme(const Scheme **s, const QString &exporterId, const SchemeBasesMap &bases, const ExportablesMap &exportables, const QString &path)
{
  SelectedExportablesMap selected;

  QFile inFile(path);
  if (!inFile.open(QIODevice::ReadOnly))
    return RetCode::E_CANT_OPEN;

  QDataStream stream(&inFile);

  QString inExporterId;
  stream >> inExporterId;

  if (inExporterId != exporterId)
    return RetCode::E_INCORRECT_EXPORTER;

  QString baseName;
  stream >> baseName;

  if (!bases.contains(baseName))
    return RetCode::E_UNKNOWN_BASE;

  if (stream.atEnd())
    return RetCode::E_CORRUPTED_FILE;
  QString name;
  stream >> name;

  if (stream.atEnd())
    return RetCode::E_CORRUPTED_FILE;
  SerializedExportablesMap serMap;
  stream >> serMap;

  if (stream.atEnd())
    return RetCode::E_CORRUPTED_FILE;
  ARType arr;
  stream >> arr;

  if (stream.atEnd())
    return RetCode::E_CORRUPTED_FILE;
  QChar delimiter;
  stream >> delimiter;

  SerializedExportablesMap::ConstIterator cit = serMap.cbegin();
  while (cit != serMap.cend()) {
    const QString key = cit.key();
    const QString rootName = cit.value();

    if (!exportables.contains(rootName))
      return RetCode::E_UNKNOWN_EXPORTABLE;

    selected.insert(key, new SelectedExportable(exportables.value(rootName)));
  }

  try {
    *s = new Scheme(name, selected, bases.value(baseName), static_cast<Globals::DataArrangement>(arr), delimiter);
  } catch (std::bad_alloc &) {
    return RetCode::E_NO_MEMORY;
  }

  return RetCode::OK;
}

SchemeSerializer::RetCode SchemeSerializer::serializeScheme(const Scheme *s, const QString &exporterId, const QString &path)
{
  ARType arr = static_cast<ARType>(s->arrangement);

  SerializedExportablesMap map;

  SelectedExportablesMap::ConstIterator cit = s->selectedExportables.cbegin();
  while (cit != s->selectedExportables.cend()) {
    const QString name = cit.key();
    const QString rootName = cit.value()->name();

    map.insert(name, rootName);
    cit++;
  }

  QFile outFile(path);
  if (!outFile.open(QIODevice::WriteOnly))
    return RetCode::E_CANT_OPEN;

  QDataStream stream(&outFile);
  stream << exporterId << s->baseName() << s->name << map << arr << s->delimiter;

  outFile.close();

  return RetCode::OK;
}

