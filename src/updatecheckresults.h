#ifndef UPDATECHECKRESULTS_H
#define UPDATECHECKRESULTS_H

#include <QObject>
#include <QString>

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

#endif // UPDATECHECKRESULTS_H
