#include "efgloaderinterface.h"
#include "efgloaderwatcher.h"
#include <chrono>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QVariant>
#include <QThread>

#ifdef ENABLE_IPC_INTERFACE_DBUS
#include <edii_ipc_qtdbus.h>
#include "dbusclient.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
#include <edii_ipc_network.h>
#include "localsocketclient.h"

#define TRY_CONNECT(ipcc) \
  do { \
    try { \
      ipcc->connectToInterface(); \
      return true; \
    } catch (std::exception &) { \
      ipcc = nullptr; \
    } \
  } while(false)

std::unique_ptr<EFGLoaderInterface> EFGLoaderInterface::s_me(nullptr);
const QString EFGLoaderInterface::LAST_FILE_PATHS_SETTINGS_TAG("LastFilePaths");

EFGLoaderInterface::EFGLoaderInterface(const QString &ediiServicePath, QObject *parent) :
  QObject(parent),
  m_guiDisplayer(new efg::GUIDisplayer()),
  m_watcher(nullptr)
{
  m_randEngine.seed(std::chrono::system_clock::now().time_since_epoch().count());

  if (!bringUpIPCInterface(ediiServicePath))
    throw std::runtime_error("No interface to use to talk to EDII service is available.");

  m_myThread = new QThread();
  moveToThread(m_myThread);
  m_myThread->start();

  connect(m_ipcClient.get(), &efg::IPCClient::displayWarning, m_guiDisplayer.get(), &efg::GUIDisplayer::onDisplayWarning, Qt::QueuedConnection);
}

EFGLoaderInterface::~EFGLoaderInterface()
{
  m_myThread->quit();
  if (!m_myThread->wait(5000))
    m_myThread->terminate();

  delete m_myThread;
  delete m_watcher;
}

bool EFGLoaderInterface::bringUpIPCInterface(const QString &ediiServicePath)
{
#ifdef ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = std::unique_ptr<efg::DBusClient>(new efg::DBusClient());
  if (m_ipcClient->isInterfaceAvailable())
    TRY_CONNECT(m_ipcClient);
  else
    m_ipcClient = nullptr;
#endif // ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = std::unique_ptr<efg::LocalSocketClient>(new efg::LocalSocketClient());
  if (m_ipcClient->isInterfaceAvailable())
    return true; /* Socket will be created from the slot */
  else
    m_ipcClient = nullptr;

  m_watcher = new efg::EFGLoaderWatcher(ediiServicePath);

#ifdef ENABLE_IPC_INTERFACE_DBUS
  m_ipcClient = std::unique_ptr<efg::DBusClient>(new efg::DBusClient());
  TRY_CONNECT(m_ipcClient);
#endif // ENABLE_IPC_INTERFACE_DBUS

  m_ipcClient = std::unique_ptr<efg::LocalSocketClient>(new efg::LocalSocketClient());

  return true;
}

void EFGLoaderInterface::checkABICompatibility()
{
  if (!m_ipcClient->isABICompatible())
    throw std::runtime_error("EDII service ABI is not compatible");
}

QByteArray EFGLoaderInterface::computeDataHash(const efg::IPCClient::NativeData &nd)
{
  QCryptographicHash hash(QCryptographicHash::Sha1);

  const QByteArray pathBA = [this](const QString &path) {
    if (path.length() > 0)
      return path.toLocal8Bit();
    else {
      /* Generate random 32 bytes */
      const size_t SZ = sizeof(uint64_t);
      QByteArray bytes;
      bytes.resize(32);

      char *rawBytes = bytes.data();
      for (size_t ctr = 0; ctr < 32 / SZ; ctr++) {
        uint64_t rand = m_randEngine();
        memcpy(rawBytes + (ctr * SZ), reinterpret_cast<char *>(&rand), SZ);
      }

      return bytes;
    }
  }(nd.path);

  hash.addData(pathBA);
  hash.addData(nd.data->xType.toLocal8Bit());
  hash.addData(nd.data->yType.toLocal8Bit());
  hash.addData(nd.data->xUnit.toLocal8Bit());
  hash.addData(nd.data->yUnit.toLocal8Bit());

  for (const auto & dp : nd.data->data) {
    const qreal x = dp.x();
    const qreal y = dp.y();
    hash.addData(reinterpret_cast<const  char *>(&x), sizeof(qreal));
    hash.addData(reinterpret_cast<const  char *>(&y), sizeof(qreal));
  }

  /* NOTE: There is a corner case of a possibly legitimate collision:
   * If a file containing multiple signal traces contains exactly the same data
   * multiple times, all but the first occurence of such data will
   * be rejected by EvaluationEngine. I cannot think of a good way how to
   * handle this reliably.
   *
   * The likelihood of this happening on purpose is quite negligible. I am, however,
   * leaving this note here in case such data have to be handled as two distictiv
   * entities in the future.
   */

  return hash.result();
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

    for (auto &nd : ndVec) {
      DataHash hash(computeDataHash(nd));
      emit onDataLoaded(nd.data, hash, nd.path, nd.name);
    }
  }
}

void EFGLoaderInterface::initialize(const QString &ediiServicePath)
{
  s_me = std::unique_ptr<EFGLoaderInterface>(new EFGLoaderInterface(ediiServicePath));

  s_me->checkABICompatibility();
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

void EFGLoaderInterface::registerMetaTypes()
{
  qRegisterMetaType<TagPathPack>();
  qRegisterMetaType<TagPathPackVec>();
  qRegisterMetaTypeStreamOperators<TagPathPack>("TagPathPack");
  qRegisterMetaTypeStreamOperators<TagPathPackVec>("TagPathPackVec");
  qRegisterMetaType<EFGDataSharedPtr>("EFGDataSharedPtr");
  qRegisterMetaType<EFGSupportedFileFormatVec>("EFGSupportedFileFormatVec");
}

void EFGLoaderInterface::retrieveSupportedFileFormats()
{
  QMetaObject::invokeMethod(this, "retrieveSupportedFileFormatsInternal", Qt::QueuedConnection);
}

void EFGLoaderInterface::retrieveSupportedFileFormatsInternal()
{
  EFGSupportedFileFormatVec supportedFormats;
  if (m_ipcClient == nullptr)
    return;

  if (m_ipcClient->supportedFileFormats(supportedFormats))
    emit supportedFileFormatsRetrieved(supportedFormats);
}

QVariant EFGLoaderInterface::saveUserSettings()
{
  TagPathPackVec vec;

  for (auto it = m_lastPathsMap.begin(); it != m_lastPathsMap.end(); it++)
    vec.push_back(TagPathPack{it.key(), it.value()});

  QVariant v = QVariant::fromValue<TagPathPackVec>(vec);

  return v;
}

