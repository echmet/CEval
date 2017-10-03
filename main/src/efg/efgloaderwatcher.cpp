#include "efgloaderwatcher.h"
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>

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
  const QString execPath = curPath + "/" + s_EFGLoaderBinaryName;
  const qlonglong mainProcPid = QCoreApplication::applicationPid();

  m_efgLoader = new QProcess();
  m_efgLoader->setProgram(execPath);
  m_efgLoader->setArguments({ QString::number(mainProcPid) });

  connect(m_efgLoader, static_cast<void (QProcess:: *)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &EFGLoaderWatcher::onEFGLoaderFinished);
  connect(m_efgLoader, &QProcess::started, this, &EFGLoaderWatcher::onEFGLoaderStarted);

  m_efgLoader->start();
  if (!m_efgLoader->waitForStarted(5000))
    throw std::runtime_error(QString("Cannot start EFGLoader, error code %1").arg(m_efgLoader->error()).toUtf8().data());
}

EFGLoaderWatcher::~EFGLoaderWatcher()
{
  if (m_efgLoader->state() == QProcess::Running) {
    disconnect(m_efgLoader, static_cast<void (QProcess:: *)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &EFGLoaderWatcher::onEFGLoaderFinished);
    m_efgLoader->terminate();
    m_efgLoader->waitForFinished(1000);
  }

  delete m_efgLoader;
}

void EFGLoaderWatcher::onEFGLoaderFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode); Q_UNUSED(exitStatus);

  restartEFGLoader();
}

void EFGLoaderWatcher::onEFGLoaderStarted()
{
}

void EFGLoaderWatcher::restartEFGLoader()
{
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
