#ifndef CUSTOMMETATYPES_H
#define CUSTOMMETATYPES_H

#include <QMap>
#include <QVariant>

class EMT {
public:
  typedef QMap<QString, QVariant> StringVariantMap;
  typedef QMap<int, QMap<QString, QVariant>> SeriesSettingsMap;

  static void registerAll();
};

Q_DECLARE_METATYPE(EMT::StringVariantMap)
Q_DECLARE_METATYPE(EMT::SeriesSettingsMap)


#endif // CUSTOMMETATYPES_H

