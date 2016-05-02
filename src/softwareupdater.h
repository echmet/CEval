#ifndef SOFTWAREUPDATER_H
#define SOFTWAREUPDATER_H

#include <QObject>
#include <QUrl>
#include "updatelistfetcher.h"

class AutoUpdateCheckDialog;
class UpdateListFetcher;

class UpdateCheckResults {
public:
  enum class Status {
    INVALID,
    FAILED,
    UP_TO_DATE,
    UPDATE_AVAILABLE
  };

  UpdateCheckResults();
  UpdateCheckResults(const Status status, const QString downloadLink, const QString versionTag, const QString errorMessage);

  const Status status;
  const QString downloadLink;
  const QString errorMessage;
  const QString versionTag;
};
Q_DECLARE_METATYPE(UpdateCheckResults)

class SoftwareUpdater : public QObject
{
  Q_OBJECT
public:
  explicit SoftwareUpdater(QObject *parent = nullptr);
  ~SoftwareUpdater();
  bool automaticCheckEnabled() const;
  void checkAutomatically();
  void checkForUpdate(const bool automatic);
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings() const;

private:
  UpdateListFetcher m_fetcher;
  bool m_automatic;

  AutoUpdateCheckDialog *m_autoDlg;

  bool m_checkAutomatically;

  static const QUrl UPDATE_LINK;
  static const QString CHECK_AUTOMATICALLY_SETTINGS_TAG;

signals:
  void automaticCheckComplete(const UpdateCheckResults &results);
  void autoUpdateChanged(const bool enabled);
  void checkComplete(const UpdateCheckResults &results);

public slots:
  void abortCheck();
  void onCheckForUpdate();
  void onSetAutoUpdate(const bool enabled);

private slots:
  void onAutomaticCheckComplete(const UpdateCheckResults &results);
  void onListFetched(const UpdateListFetcher::RetCode tRet, const SoftwareUpdateInfoMap &map);

};

#endif // SOFTWAREUPDATER_H
