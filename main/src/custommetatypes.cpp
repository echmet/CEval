#include "custommetatypes.h"
#include "dataexporter/exporterglobals.h"
#include "softwareupdater.h"
#include "datahash.h"

QDataStream &operator>>(QDataStream &in, EMT::SeriesSettingsMap &map)
{
  bool ok;
  int length;
  QVariant vLength;

  in >> vLength;

  length = vLength.toInt(&ok);
  if (!ok)
    return in;

  for (int idx = 0; idx < length; idx++) {
    int key;
    QMap<QString, QVariant> innerMap;
    QVariant vKey;
    QVariant vMap;

    in >> vKey;

    key = vKey.toInt(&ok);
    if (!ok)
      continue;

    in >> vMap;
    if (!vMap.canConvert<QMap<QString, QVariant>>())
      continue;

    innerMap = vMap.value<QMap<QString, QVariant>>();

    map.insert(key, innerMap);
  }

  return in;
}

QDataStream &operator<<(QDataStream &out, const EMT::SeriesSettingsMap &map)
{
  int length = map.size();

  out << QVariant(length);

  for (const int idx : map.keys()) {
    QMap<QString, QVariant> innerMap = map[idx];

    out << QVariant(idx) << innerMap;
  }

  return out;
}

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

QDataStream &operator<<(QDataStream &out, const DataExporter::Globals::DataArrangement &a)
{
  typedef typename std::underlying_type<DataExporter::Globals::DataArrangement>::type ARType;

  out << QVariant::fromValue<ARType>(static_cast<ARType>(a));

  return out;
}

void EMT::registerAll()
{
  qRegisterMetaTypeStreamOperators<DataExporter::Globals::DataArrangement>("DataArrangement");
  qRegisterMetaTypeStreamOperators<SeriesSettingsMap>("SeriesSettingsMap");
  qRegisterMetaType<UpdateCheckResults>("UpdateCheckResults");
  qRegisterMetaType<DataHash>("DataHash");
}
