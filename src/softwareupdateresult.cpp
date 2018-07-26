#include "softwareupdateresult.h"

SoftwareUpdateResult::SoftwareUpdateResult() :
  state(State::OK),
  updateAvailable(false),
  error(""),
  verMajor(0), verMinor(0), revision(""),
  severity(Severity::AVAILABLE),
  downloadLink("")
{}

SoftwareUpdateResult::SoftwareUpdateResult(const State s, QString &&error) :
  state(s),
  updateAvailable(false),
  error(std::move(error)),
  verMajor(0), verMinor(0), revision(""),
  severity(Severity::AVAILABLE),
  downloadLink("")
{}

SoftwareUpdateResult::SoftwareUpdateResult(const int verMajor, const int verMinor, QString &&revision,
                                           const Severity severity,
                                           QString &&link) :
  state(State::OK),
  updateAvailable(true),
  error(""),
  verMajor(verMajor), verMinor(verMinor), revision(std::move(revision)),
  severity(severity),
  downloadLink(std::move(link))
{}
