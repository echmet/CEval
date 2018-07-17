#include "cevalconfig.h"
#include "globals.h"

#include <QDir>
#include <QSettings>

std::unique_ptr<CEvalConfig> CEvalConfig::s_me(nullptr);
const QString CEvalConfig::DAC_SETTINGS_TAG("DataAccumulator");
const QString CEvalConfig::EFG_LOADER_INTERFACE_SETTINGS_TAG("EFGLoaderInterface");
const QString CEvalConfig::NUM_FORMAT_SETTINGS_TAG("NumFormat");
const QString CEvalConfig::SOFTWARE_UPDATER_SETTINGS_TAG("SoftwareUpdater");
const QString CEvalConfig::EDII_SERVICE_PATH_TAG("EDIIServicePath");

const QString CEvalConfig::ROOT_SETTINGS_TAG("Root");

#ifdef Q_OS_UNIX
const QString CEvalConfig::s_pathPrefixTemplate(QString("%2/.local/share/ECHMET/%1/").arg(Globals::SOFTWARE_NAME));
#endif // Q_OS_UNIX

CEvalConfig::CEvalConfig()
{
  QSettings s(configFilePath(), QSettings::IniFormat);

  QVariant root = s.value(ROOT_SETTINGS_TAG);
  if (root.canConvert<EMT::StringVariantMap>())
    m_cfg = root.value<EMT::StringVariantMap>();
}

QString CEvalConfig::configFilePath()
{
#ifdef Q_OS_UNIX
  const QString prefix = s_pathPrefixTemplate.arg(QDir::homePath());
#else
  const QString prefix = "";
#endif // Q_OS_UNIX

  return QString("%1%2.conf").arg(prefix).arg(Globals::SOFTWARE_NAME);
}

bool CEvalConfig::initialize()
{
  try {
    s_me = std::unique_ptr<CEvalConfig>(new CEvalConfig());
  } catch (const std::bad_alloc &) {
    return false;
  } catch (const std::runtime_error &) {
    return false;
  }

  return true;
}

QVariant CEvalConfig::value(const QString &key)
{
  if (s_me == nullptr)
    return {};

  if (!s_me->m_cfg.contains(key))
    return {};

  return s_me->m_cfg.value(key);
}

void CEvalConfig::save()
{
  if (s_me == nullptr)
    return;

  s_me->saveInternal();
}

void CEvalConfig::saveInternal() const
{
  QSettings s(configFilePath(), QSettings::IniFormat);

  s.setValue(ROOT_SETTINGS_TAG, m_cfg);
}

void CEvalConfig::setValue(const QString &key, const QVariant &value)
{
  if (s_me == nullptr)
    return;

  s_me->m_cfg.insert(key, value);
}


