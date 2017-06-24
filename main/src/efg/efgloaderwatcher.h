#ifndef EFGLOADERWATCHER_H
#define EFGLOADERWATCHER_H

#include <QObject>
#include <QProcess>

namespace efg {

class EFGLoaderWatcher : public QObject
{
  Q_OBJECT
public:
  explicit EFGLoaderWatcher(QObject *parent = nullptr);
  ~EFGLoaderWatcher();

private:
  void restartEFGLoader();

  QProcess *m_efgLoader;

  static const QString s_EFGLoaderBinaryName;

private slots:
  void onEFGLoaderFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void onEFGLoaderStarted();
};

} // namespace efg

#endif // EFGLOADERWATCHER_H
