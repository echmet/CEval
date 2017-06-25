#include "efgloaderinterface.h"
#include "efgloaderwatcher.h"
#include <QDir>
#include <QFileInfo>

#ifdef ENABLE_IPC_INTERFACE_DBUS
#include "dbusclient.h"
#endif // ENABLE_IPC_INTERFACE_DBUS
#include "localsocketclient.h"

#include <QDebug>

std::unique_ptr<EFGLoaderInterface> EFGLoaderInterface::s_me(nullptr);

EFGLoaderInterface::EFGLoaderInterface(QObject *parent) :
  QObject(parent)
{
  m_watcher = new efg::EFGLoaderWatcher(this);

#ifdef ENABLE_IPC_INTERFACE_DBUS
  try {
    m_ipcClient = new efg::DBusClient();
    return;
  } catch (std::exception &ex) {
    qWarning() << "Cannot connect to D-Bus(" << ex.what() << "), falling back to local socket";
    m_ipcClient = nullptr;
  }
#endif // ENABLE_IPC_INTERFACE_DBUS

  try {
    m_ipcClient = new efg::LocalSocketClient();
  } catch (std::exception &ex) {
    qWarning() << "Cannot connect to IPC socket(" << ex.what() << "). Electrophoregram loading will not be available";
    m_ipcClient = nullptr;
  }
}

EFGLoaderInterface::~EFGLoaderInterface()
{
}

void EFGLoaderInterface::loadData(const QString &formatTag)
{
  QString hintPath;
  efg::IPCClient::NativeDataVec ndVec;

  if (m_ipcClient == nullptr)
    return;

  if (m_lastPathsMap.contains(formatTag))
    hintPath = m_lastPathsMap[formatTag];
  else
    hintPath = "";

  if (!m_ipcClient->loadData(ndVec, formatTag, hintPath))
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

bool EFGLoaderInterface::supportedFileFormats(QVector<EFGSupportedFileFormat> &supportedFormats)
{
  if (m_ipcClient == nullptr)
    return false;

  return m_ipcClient->supportedFileFormats(supportedFormats);
}
