#ifndef SCHEMESERIALIZER_H
#define SCHEMESERIALIZER_H

#include "exporterelems.h"
#include <QMap>

namespace DataExporter {

class SchemeSerializer
{
public:
  enum class RetCode {
    OK,
    E_CANT_OPEN,
    E_CANT_WRITE,
    E_CORRUPTED_FILE,
    E_INCORRECT_EXPORTER,
    E_NO_MEMORY,
    E_UNKNOWN_BASE,
    E_UNKNOWN_EXPORTABLE
  };

  class SerializedExportable {
  public:
    SerializedExportable();
    SerializedExportable(const QString &rootName, const int position);

    SerializedExportable & operator=(const SerializedExportable &other);

    const QString rootName;
    const int position;
  };

  SchemeSerializer() = delete;

  static RetCode deserializeScheme(Scheme **s, const QString &exporterId, const SchemeBasesMap &bases, const QString &path);
  static RetCode serializeScheme(const Scheme *s, const QString &exporterId, const QString &path);

private:
  typedef typename std::underlying_type<Globals::DataArrangement>::type ARType;

  typedef QMap<QString, SerializedExportable> SerializedExportablesMap;
};

} //namespace DataExporter

#endif // SCHEMESERIALIZER_H
