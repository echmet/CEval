#include "efgloaderwatcher.h"
#include "../globals.h"

#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>

namespace efg {

const QString EFGLoaderWatcher::s_EFGLoaderPathPrefix("bin");

#ifdef Q_OS_WIN
const QString EFGLoaderWatcher::s_EFGLoaderBinaryName("EDIICore.exe");
#else
const QString EFGLoaderWatcher::s_EFGLoaderBinaryName("EDIICore");
#endif // Q_OS_WIN

#ifdef CEVAL_FLATPAK_BUILD
QString EFGLoaderWatcher::stripPathIfNeeded(const QString &path)
{
  if (path.endsWith("/" + s_EFGLoaderPathPrefix + "/" + s_EFGLoaderBinaryName)) {
    int idx = path.lastIndexOf("/");
    idx = path.lastIndexOf("/", idx - 1);
    if (idx < 1)
	    return "/";
    return path.mid(0, idx);
  }

  return path;
}
#endif // CEVAL_FLATPAK_BUILD

EFGLoaderWatcher::EFGLoaderWatcher(const QString &inEdiiServicePath, QObject *parent) :
  QObject(parent)
{
#ifdef CEVAL_FLATPAK_BUILD
  const QString ediiServicePath = stripPathIfNeeded(inEdiiServicePath);
#else
  const QString ediiServicePath = inEdiiServicePath;
#endif // CEVAL_FLATPAK_BUILD

  const QString execPath = appendPath(ediiServicePath);
  const qlonglong mainProcPid = QCoreApplication::applicationPid();

  m_efgLoader = new QProcess();
  m_efgLoader->setProgram(execPath);
  m_efgLoader->setWorkingDirectory(ediiServicePath + "/" + s_EFGLoaderPathPrefix + "/");
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

QString EFGLoaderWatcher::appendPath(const QString &path)
{
  return path + "/" + s_EFGLoaderPathPrefix + "/" + s_EFGLoaderBinaryName;
}

bool EFGLoaderWatcher::isServicePathValid(const QString &inPath)
{
#ifdef CEVAL_FLATPAK_BUILD
  const QString path = stripPathIfNeeded(inPath);
#else
  const QString path = inPath;
#endif // CEVAL_FLATPAK_BUILD

  QFile f(appendPath(path));

  if (!f.exists())
    return false;

  return f.permissions() & QFile::ExeUser;
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
  int ret = QMessageBox::question(nullptr, tr("ECHMET Data Import Infrastructure"),
                                  QString(tr("Aieee..., it looks like the process responsible for loading electophoregram data has crashed.\n"
                                             "%1 can try to restart the process and resume operation. If the process is not restarted,"
                                             "you will not be able to load any new electrophoregrams\n\n"
                                             "Restart EDII service?")).arg(Globals::SOFTWARE_NAME));

  if (ret == QMessageBox::Yes) {
    m_efgLoader->start();
    if (!m_efgLoader->waitForStarted())
      QMessageBox::critical(nullptr, tr("ECHMET Data Import Infrastructure"),
                            QString(tr("%1 was unable to restart EDII service. "
                                       "You will not be able to load any new electropghoregrams.")).arg(Globals::SOFTWARE_NAME));
  }
}

} // namespace efg
