#ifndef CEVALCONFIG_H
#define CEVALCONFIG_H

#include "custommetatypes.h"

#include <memory>

class QString;

class CEvalConfig
{
public:
  static bool initialize();
  static QVariant value(const QString &key);
  static void save();
  static void setValue(const QString &key, const QVariant &value);

  static const QString DAC_SETTINGS_TAG;
  static const QString EFG_LOADER_INTERFACE_SETTINGS_TAG;
  static const QString NUM_FORMAT_SETTINGS_TAG;
  static const QString SOFTWARE_UPDATER_SETTINGS_TAG;
  static const QString EDII_SERVICE_PATH_TAG;

private:
  explicit CEvalConfig();
  void saveInternal() const;

  EMT::StringVariantMap m_cfg;

  static QString configFilePath();

  static std::unique_ptr<CEvalConfig> s_me;

  static const QString ROOT_SETTINGS_TAG;
#ifdef Q_OS_UNIX
  static const QString s_pathPrefixTemplate;
#endif // Q_OS_UNIX
};

#endif // CEVALCONFIG_H
