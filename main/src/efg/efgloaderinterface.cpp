#include "efgloaderinterface.h"
#include "efgloaderwatcher.h"
#include <QDir>
#include <QFileInfo>

#ifdef ENABLE_IPC_INTERFACE_DBUS
#include "dbusclient.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
#include "localsocketclient.h"

std::unique_ptr<EFGLoaderInterface> EFGLoaderInterface::s_me(nullptr);
const QString EFGLoaderInterface::LAST_FILE_PATHS_SETTINGS_TAG("LastFilePaths");

EFGLoaderInterface::EFGLoaderInterface(QObject *parent) :
  QObject(parent)
{
  qRegisterMetaTypeStreamOperators<TagPathPack>("TagPathPack");
  qRegisterMetaTypeStreamOperators<TagPathPackVec>("TagPathPackVec");

  m_watcher = new efg::EFGLoaderWatcher(this);

#ifdef ENABLE_IPC_INTERFACE_DBUS
  try {
    m_ipcClient = new efg::DBusClient();
    return;
  } catch (std::exception &ex) {
    m_ipcClient = nullptr;
  }
#endif // ENABLE_IPC_INTERFACE_DBUS

  try {
    m_ipcClient = new efg::LocalSocketClient();
  } catch (std::exception &ex) {
    m_ipcClient = nullptr;
  }
}

EFGLoaderInterface::~EFGLoaderInterface()
{
}

void EFGLoaderInterface::loadData(const QString &formatTag, const int loadOption)
{
  QString hintPath;
  efg::IPCClient::NativeDataVec ndVec;

  if (m_ipcClient == nullptr)
    return;

  if (m_lastPathsMap.contains(formatTag))
    hintPath = m_lastPathsMap[formatTag];
  else
    hintPath = "";

  if (!m_ipcClient->loadData(ndVec, formatTag, hintPath, loadOption))
    return;

  if (ndVec.size() > 0) {
    m_lastPathsMap[formatTag] = [](const QString &path) {
      return QFileInfo(path).dir().absolutePath();
    }(ndVec.at(0).path);

    for (auto &nd : ndVec)
      emit onDataLoaded(nd.data, nd.path, nd.name);
  }
}

EFGLoaderInterface & EFGLoaderInterface::instance()
{
  if (s_me == nullptr)
    s_me = std::unique_ptr<EFGLoaderInterface>(new EFGLoaderInterface());

  return *s_me;
}

void EFGLoaderInterface::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<TagPathPackVec>())
    return;

  for (const auto &p : settings.value<TagPathPackVec>())
    m_lastPathsMap[p.tag] = p.path;
}

QVariant EFGLoaderInterface::saveUserSettings()
{
  TagPathPackVec vec;

  for (auto it = m_lastPathsMap.begin(); it != m_lastPathsMap.end(); it++)
    vec.push_back(TagPathPack{it.key(), it.value()});

  QVariant v = QVariant::fromValue<TagPathPackVec>(vec);

  return v;
}

bool EFGLoaderInterface::supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats)
{
  if (m_ipcClient == nullptr)
    return false;

  return m_ipcClient->supportedFileFormats(supportedFormats);
}
