#include "custommetatypes.h"
#include "softwareupdater.h"

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

void EMT::registerAll()
{
  qRegisterMetaTypeStreamOperators<SeriesSettingsMap>("SeriesSettingsMap");
  qRegisterMetaType<UpdateCheckResults>();
}
