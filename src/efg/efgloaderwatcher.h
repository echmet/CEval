#ifndef EFGLOADERWATCHER_H
#define EFGLOADERWATCHER_H

#include <QObject>
#include <QProcess>

namespace efg {

class EFGLoaderWatcher : public QObject
{
  Q_OBJECT
public:
  explicit EFGLoaderWatcher(const QString &ediiServicePath, QObject *parent = nullptr);
  ~EFGLoaderWatcher();

  static bool isServicePathValid(const QString &path);

  static const QString s_EFGLoaderPathPrefix;
  static const QString s_EFGLoaderBinaryName;

private:
  void restartEFGLoader();

  QProcess *m_efgLoader;


private slots:
  void onEFGLoaderFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void onEFGLoaderStarted();
};

} // namespace efg

#endif // EFGLOADERWATCHER_H
