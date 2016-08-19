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

  SchemeSerializer() = delete;

  static RetCode deserializeScheme(Scheme **s, const QString &exporterId, const SchemeBasesMap &bases, const QString &path);
  static RetCode serializeScheme(const Scheme *s, const QString &exporterId, const QString &path);

private:
  typedef typename std::underlying_type<Globals::DataArrangement>::type ARType;

  class SerializedExportable {
    QString rootName;
  };

  typedef QMap<QString, QString> SerializedExportablesMap;
};

} //namespace DataExporter

#endif // SCHEMESERIALIZER_H
