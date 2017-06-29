#include "efgloaderinterface.h"
#include "efgloaderwatcher.h"
#include <QDir>
#include <QFileInfo>
#include <QVariant>
#include <QThread>

#ifdef ENABLE_IPC_INTERFACE_DBUS
#include "dbusclient.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
#include "localsocketclient.h"

#define TRY_CONNECT(ipcc) \
  do { \
    try { \
      ipcc->connectToInterface(); \
      return; \
    } catch (std::exception &) { \
      delete ipcc; \
      ipcc = nullptr; \
    } \
  } while(false)

std::unique_ptr<EFGLoaderInterface> EFGLoaderInterface::s_me(nullptr);
const QString EFGLoaderInterface::LAST_FILE_PATHS_SETTINGS_TAG("LastFilePaths");

EFGLoaderInterface::EFGLoaderInterface(QObject *parent) :
  QObject(parent),
  m_watcher(nullptr)
{
  qRegisterMetaTypeStreamOperators<TagPathPack>("TagPathPack");
  qRegisterMetaTypeStreamOperators<TagPathPackVec>("TagPathPackVec");
  qRegisterMetaType<EFGDataSharedPtr>("EFGDataSharedPtr");

  m_myThread = new QThread();
  moveToThread(m_myThread);
  m_myThread->start();


#ifdef ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = new efg::DBusClient();
  if (m_ipcClient->isInterfaceAvailable())
    TRY_CONNECT(m_ipcClient);
  else
    delete m_ipcClient;
#endif // ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = new efg::LocalSocketClient();
  if (m_ipcClient->isInterfaceAvailable())
    TRY_CONNECT(m_ipcClient);
  else
    delete m_ipcClient;

  m_watcher = new efg::EFGLoaderWatcher();

#ifdef ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = new efg::DBusClient();
  TRY_CONNECT(m_ipcClient);
#endif // ENABLE_IPC_INTERFACE_DBUS

  m_ipcClient = new efg::LocalSocketClient();
  TRY_CONNECT(m_ipcClient);
}

EFGLoaderInterface::~EFGLoaderInterface()
{
  m_myThread->quit();
  if (!m_myThread->wait(5000))
    m_myThread->terminate();

  delete m_myThread;
  delete m_watcher;
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

void EFGLoaderInterface::initialize()
{
  s_me = std::unique_ptr<EFGLoaderInterface>(new EFGLoaderInterface());
}

EFGLoaderInterface & EFGLoaderInterface::instance()
{
  if (s_me == nullptr)
    throw std::runtime_error("EFGLoaderInterface not initialized");

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
