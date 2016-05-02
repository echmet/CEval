#include "updatecheckresults.h"

UpdateCheckResults::UpdateCheckResults() :
  status(Status::INVALID),
  downloadLink(""),
  errorMessage(""),
  versionTag("")
{
}

UpdateCheckResults::UpdateCheckResults(const Status status, const QString downloadLink, const QString versionTag, const QString errorMessage) :
  status(status),
  downloadLink(downloadLink),
  errorMessage(errorMessage),
  versionTag(versionTag)
{
}
