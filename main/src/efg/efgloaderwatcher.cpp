#include "efgloaderwatcher.h"
#include <QDir>
#include <QMessageBox>

#include <QDebug>

namespace efg {

#ifdef Q_OS_WIN
const QString EFGLoaderWatcher::s_EFGLoaderBinaryName("CEvalEFGLoader.exe");
#else
const QString EFGLoaderWatcher::s_EFGLoaderBinaryName("CEvalEFGLoader");
#endif // Q_OS_WIN

EFGLoaderWatcher::EFGLoaderWatcher(QObject *parent) :
  QObject(parent)
{
  const QString curPath = QDir::currentPath();

  m_efgLoader = new QProcess(this);
  m_efgLoader->setProgram(curPath + "/" + s_EFGLoaderBinaryName);

  connect(m_efgLoader, static_cast<void (QProcess:: *)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &EFGLoaderWatcher::onEFGLoaderFinished);
  connect(m_efgLoader, &QProcess::started, this, &EFGLoaderWatcher::onEFGLoaderStarted);

  m_efgLoader->start();
  if (!m_efgLoader->waitForStarted())
    throw std::runtime_error("Cannot start EFGLoader");
}

EFGLoaderWatcher::~EFGLoaderWatcher()
{
  if (m_efgLoader->state() == QProcess::Running) {
    disconnect(m_efgLoader, static_cast<void (QProcess:: *)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &EFGLoaderWatcher::onEFGLoaderFinished);
    m_efgLoader->terminate();
    m_efgLoader->waitForFinished();
  }
}

void EFGLoaderWatcher::onEFGLoaderFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode); Q_UNUSED(exitStatus);

  restartEFGLoader();
}

void EFGLoaderWatcher::onEFGLoaderStarted()
{
  qDebug() << "EFGLoader started";
}

void EFGLoaderWatcher::restartEFGLoader()
{
  qWarning() << "EFG loader has crashed, restarting";

  int ret = QMessageBox::question(nullptr, tr("EFGLoader"), tr("Aieee..., it looks like the process responsible for loading electophoregram data has crashed.\n"
                                                               "CEval can try to restart the process and resume operation. If the process is not restarted,"
                                                               "you will not be able to load any new electrophoregrams\n\n"
                                                               "Restart EFGLoader?"));

  if (ret == QMessageBox::Yes) {
    m_efgLoader->start();
    if (!m_efgLoader->waitForStarted())
      QMessageBox::critical(nullptr, tr("EFGLoader"), tr("CEval was unable to restart EFGLoader. You will not be able to load any new electropghoregrams."));
  }
}

} // namespace efg
