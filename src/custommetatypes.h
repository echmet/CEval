#ifndef CUSTOMMETATYPES_H
#define CUSTOMMETATYPES_H

#include <QMap>
#include <QVariant>

class EMT {
public:
  using StringVariantMap = QMap<QString, QVariant>;
  using SeriesSettingsMap = QMap<int, QVariantMap>;

  static void registerAll();
};

inline
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
    QVariantMap innerMap;
    QVariant vKey;
    QVariant vMap;

    in >> vKey;
    key = vKey.toInt(&ok);
    if (!ok) {
      qWarning() << "Invalid index of SerieSettingsMap data for serie" << vMap << ", returning what I read so far but rest of the config will be broken";
      return in;
    }

    in >> vMap;
    if (!vMap.canConvert<QVariantMap>()) {
      qWarning() << "Invalid SerieSettingsMap data for serie" << vMap << ", returning what I read so far but rest of the config will be broken";
      return in;
    }

    map.insert(key, vMap.value<QVariantMap>());
  }

  return in;
}

inline
QDataStream &operator<<(QDataStream &out, const EMT::SeriesSettingsMap &map)
{
  int length = map.size();

  out << QVariant(length);

  for (const int idx : map.keys()) {
    out << QVariant(idx) << QVariant(map[idx]);
  }

  return out;
}

Q_DECLARE_METATYPE(EMT::SeriesSettingsMap)

#endif // CUSTOMMETATYPES_H

