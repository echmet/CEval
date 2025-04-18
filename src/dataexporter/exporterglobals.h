#ifndef EXPORTERGLOBALS_H
#define EXPORTERGLOBALS_H

#include <QObject>
#include <QVariant>

namespace DataExporter {

typedef QVector<QString> QStringVector;

class Globals : public QObject {
  Q_OBJECT
public:
  Globals() = delete;

enum class DataArrangement {
  HORIZONTAL,
  VERTICAL
};
Q_ENUM(DataArrangement)

};

} // namespace DataExporter

inline
QDataStream &operator>>(QDataStream &in, DataExporter::Globals::DataArrangement &a)
{
  typedef typename std::underlying_type<DataExporter::Globals::DataArrangement>::type ARType;

  QVariant v;

  in >> v;

  if (v.canConvert<ARType>())
    a = static_cast<DataExporter::Globals::DataArrangement>(v.value<ARType>());

  switch (a) {
  case DataExporter::Globals::DataArrangement::VERTICAL:
  case DataExporter::Globals::DataArrangement::HORIZONTAL:
    break;
  default:
    a = DataExporter::Globals::DataArrangement::VERTICAL;
    break;
  }

  return in;
}

inline
QDataStream &operator<<(QDataStream &out, const DataExporter::Globals::DataArrangement &a)
{
  typedef typename std::underlying_type<DataExporter::Globals::DataArrangement>::type ARType;

  out << QVariant::fromValue<ARType>(static_cast<ARType>(a));

  return out;
}

Q_DECLARE_METATYPE(DataExporter::Globals::DataArrangement);

#endif // EXPORTERGLOBALS_H
