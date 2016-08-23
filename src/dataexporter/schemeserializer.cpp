#include "schemeserializer.h"
#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>

using namespace DataExporter;

QDataStream & operator<<(QDataStream &stream, const SchemeSerializer::SerializedExportable &e)
{
  stream << e.rootName << e.displayName << e.position;

  return stream;
}

QDataStream & operator>>(QDataStream &stream, SchemeSerializer::SerializedExportable &e)
{
  QString rootName;
  QString displayName;
  int position;

  stream >> rootName;
  stream >> displayName;
  stream >> position;

  const_cast<QString&>(e.rootName) = rootName;
  const_cast<QString&>(e.displayName) = displayName;
  const_cast<int&>(e.position) = position;

  return stream;
}

SchemeSerializer::SerializedExportable::SerializedExportable() :
  rootName(""),
  displayName(""),
  position(-1)
{
}

SchemeSerializer::SerializedExportable::SerializedExportable(const QString &rootName, const QString &displayName, const int position) :
  rootName(rootName),
  displayName(displayName),
  position(position)
{
}

SchemeSerializer::SerializedExportable & SchemeSerializer::SerializedExportable::operator=(const SerializedExportable &other)
{
  const_cast<QString&>(rootName) = other.rootName;
  const_cast<int&>(position) = other.position;

  return *this;
}

SchemeSerializer::RetCode SchemeSerializer::deserializeScheme(Scheme **s, const QString &exporterId, const SchemeBasesMap &bases, const QString &path)
{
  SelectedExportablesMap selected;

  QFile inFile(path);
  if (!inFile.open(QIODevice::ReadOnly))
    return RetCode::E_CANT_OPEN;

  /* Check the hash */
  {
    QByteArray hash = inFile.read(32);
    QByteArray bytes = inFile.readAll();

    QByteArray computedHash = QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);

    if (computedHash != hash)
      return RetCode::E_CORRUPTED_FILE;

    inFile.seek(32);
  }

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

  const ExportablesMap &exportables = bases.value(baseName)->exportables;
  SerializedExportablesMap::ConstIterator cit = serMap.cbegin();
  while (cit != serMap.cend()) {
    const QString key = cit.key();
    const SerializedExportable serEx = cit.value();

    if (!exportables.contains(serEx.rootName))
      return RetCode::E_UNKNOWN_EXPORTABLE;

    selected.insert(key, new SelectedExportable(exportables.value(serEx.rootName), serEx.position, serEx.displayName));
    cit++;
  }

  try {
    *s = new Scheme(name, selected, bases.value(baseName), static_cast<Globals::DataArrangement>(arr));
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
    SerializedExportable serEx(cit.value()->name(), cit.value()->displayName, cit.value()->position);

    map.insert(name, serEx);
    cit++;
  }

  QFile outFile(path);
  if (!outFile.open(QIODevice::WriteOnly))
    return RetCode::E_CANT_OPEN;

  QByteArray outBuffer;
  QDataStream stream(&outBuffer, QIODevice::WriteOnly);
  stream << exporterId << s->baseName() << s->name << map << arr;

  QByteArray hash = QCryptographicHash::hash(outBuffer, QCryptographicHash::Sha256);

  if (outFile.write(hash) < hash.size())
    return RetCode::E_CANT_WRITE;

  if (outFile.write(outBuffer) < outBuffer.size())
    return RetCode::E_CANT_WRITE;

  outFile.close();

  return RetCode::OK;
}

