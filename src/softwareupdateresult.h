#ifndef SOFTWAREUPDATERESULT_H
#define SOFTWAREUPDATERESULT_H

#include <QString>

class SoftwareUpdateResult {
public:
  enum class Severity {
    AVAILABLE,
    RECOMMENDED,
    REQUIRED
  };

  enum class State {
    OK,
    CHECK_ERROR,
    NETWORK_ERROR,
    NO_DATA,
    DISABLED
  };

  explicit SoftwareUpdateResult();
  explicit SoftwareUpdateResult(const State s, QString &&error);
  explicit SoftwareUpdateResult(const int verMajor, const int verMinor, QString &&revision,
                                const Severity severity,
                                QString &&link);

  const State state;
  const bool updateAvailable;

  const QString error;

  const int verMajor;
  const int verMinor;
  const QString revision;
  const Severity severity;
  const QString downloadLink;
};

#endif // SOFTWAREUPDATERESULT_H
