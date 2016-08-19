#include "schemeserializer.h"
#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>

using namespace DataExporter;

QDataStream & operator<<(QDataStream &stream, const SchemeSerializer::SerializedExportable &e)
{
  stream << e.rootName << e.position;

  return stream;
}

QDataStream & operator>>(QDataStream &stream, SchemeSerializer::SerializedExportable &e)
{
  QString rootName;
  int position;

  stream >> rootName;
  stream >> position;

  const_cast<QString&>(e.rootName) = rootName;
  const_cast<int&>(e.position) = position;

  return stream;
}

SchemeSerializer::SerializedExportable::SerializedExportable() :
  rootName(""),
  position(-1)
{
}

SchemeSerializer::SerializedExportable::SerializedExportable(const QString &rootName, const int position) :
  rootName(rootName),
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

  /* Check the hash */
  {
    const quint64 pos = inFile.pos();

    QByteArray dataHash;
    stream >> dataHash;

    inFile.seek(0);
    QByteArray bytes = inFile.read(pos);

    QByteArray computedHash = QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);

    if (computedHash != dataHash)
      return RetCode::E_CORRUPTED_FILE;
  }

  const ExportablesMap &exportables = bases.value(baseName)->exportables;
  SerializedExportablesMap::ConstIterator cit = serMap.cbegin();
  while (cit != serMap.cend()) {
    const QString key = cit.key();
    const SerializedExportable serEx = cit.value();

    if (!exportables.contains(serEx.rootName))
      return RetCode::E_UNKNOWN_EXPORTABLE;

    selected.insert(key, new SelectedExportable(exportables.value(serEx.rootName), serEx.position));
    cit++;
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
    SerializedExportable serEx(cit.value()->name(), cit.value()->position);

    map.insert(name, serEx);
    cit++;
  }

  QFile outFile(path);
  if (!outFile.open(QIODevice::WriteOnly))
    return RetCode::E_CANT_OPEN;

  QByteArray outBuffer;
  QDataStream stream(&outBuffer, QIODevice::WriteOnly);
  stream << exporterId << s->baseName() << s->name << map << arr << s->delimiter;

  QByteArray hash = QCryptographicHash::hash(outBuffer, QCryptographicHash::Sha256);

  stream << hash;

  if (outFile.write(outBuffer) < outBuffer.size())
    return RetCode::E_CANT_WRITE;

  outFile.close();

  return RetCode::OK;
}

