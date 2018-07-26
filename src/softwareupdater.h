#ifndef SOFTWAREUPDATER_H
#define SOFTWAREUPDATER_H

#include "softwareupdateresult.h"

#include <QObject>
#include <mutex>

class AutoUpdateCheckDialog;

class SoftwareUpdateWorker : public QObject {
  Q_OBJECT
public:
  SoftwareUpdateWorker(const QStringList &links, const bool automatic) :
    m_automatic(automatic),
    m_links(std::move(links))
  {}

public slots:
  void process();

signals:
  void checkComplete(bool automatic, const SoftwareUpdateResult &result);
  void finished();

private:
  const bool m_automatic;
  const QStringList m_links;
};


class SoftwareUpdater : public QObject
{
  Q_OBJECT
public:
  explicit SoftwareUpdater(QObject *parent = nullptr);
  bool automaticCheckEnabled() const;
  void checkAutomatically();
  void checkForUpdate(const bool automatic);
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings() const;

private:
  void automaticCheckComplete(const SoftwareUpdateResult &result);

  AutoUpdateCheckDialog *m_autoDlg;

  bool m_checkAutomatically;
  std::mutex m_checkInProgressLock;
  bool m_checkInProgress;

  static const QStringList UPDATE_LINKS;
  static const QString CHECK_AUTOMATICALLY_SETTINGS_TAG;

signals:
  void autoUpdateChanged(const bool enabled);
  void checkComplete(const SoftwareUpdateResult &result);

public slots:
  void onCheckForUpdate();
  void onSetAutoUpdate(const bool enabled);

private slots:
  void onUpdateCheckComplete(const bool automatic, const SoftwareUpdateResult &result);
};

#endif // SOFTWAREUPDATER_H
