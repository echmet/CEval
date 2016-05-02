#ifndef SOFTWAREUPDATER_H
#define SOFTWAREUPDATER_H

#include <QObject>
#include <QUrl>
#include "updatelistfetcher.h"

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
  void abortCheck();
  void checkForUpdate(const bool silent);

private:
  UpdateListFetcher m_fetcher;
  bool m_silent;

  static const QUrl UPDATE_LINK;

signals:
  void checkComplete(const UpdateCheckResults &results);

public slots:

private slots:
  void onListFetched(const UpdateListFetcher::RetCode tRet, const SoftwareUpdateInfoMap &map);

};

#endif // SOFTWAREUPDATER_H
