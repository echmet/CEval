#include "softwareupdateinfo.h"

SoftwareUpdateInfo::Version::Version(const int major, const int minor, const QString &revision) :
  major(major),
  minor(minor),
  revision(revision.toLower())
{
  for (const QChar &ch : revision) {
    if (!ch.isLetter())
      throw InvalidRevisionStringException();
  }
}

SoftwareUpdateInfo::Version::Version(const Version &other) :
  major(other.major),
  minor(other.minor),
  revision(other.revision)
{
}

bool SoftwareUpdateInfo::Version::operator>(const Version &other) const
{
  if (major > other.major)
    return true;
  else if (major < other.major)
    return false;

  if (minor > other.minor)
    return true;
  else if (minor < other.minor)
    return false;

  if (revision.length() > other.revision.length())
    return true;
  else if (revision.length() < other.revision.length())
    return false;

  for (int idx = 0; idx < revision.length(); idx++) {
    const QChar &ch = revision.at(idx);
    const QChar &otherCh = other.revision.at(idx);

    if (ch > otherCh)
      return true;
    else if (ch < otherCh)
      return false;
  }

  return false;
}

bool SoftwareUpdateInfo::Version::operator<(const Version &other) const
{
  return !(*this > other) && !(*this == other);
}

bool SoftwareUpdateInfo::Version::operator>=(const Version &other) const
{
  return (*this > other) || (*this == other);
}

bool SoftwareUpdateInfo::Version::operator<=(const Version &other) const
{
  return (*this < other) || (*this == other);
}

bool SoftwareUpdateInfo::Version::operator==(const Version &other) const
{
  if (major != other.major)
    return false;

  if (minor != other.minor)
    return false;

  if (revision.compare(other.revision, Qt::CaseInsensitive) != 0)
    return false;

  return true;
}

SoftwareUpdateInfo::Version & SoftwareUpdateInfo::Version::operator=(const Version &other)
{
  const_cast<int&>(major) = other.major;
  const_cast<int&>(minor) = other.minor;
  const_cast<QString&>(revision) = other.revision;

  return *this;
}

SoftwareUpdateInfo::SoftwareUpdateInfo() :
  name(""),
  version(Version(0, 0, "")),
  downloadLink("")
{
}

SoftwareUpdateInfo::SoftwareUpdateInfo(const QString &name, const Version &version, const QString &downloadLink) :
  name(name),
  version(version),
  downloadLink(downloadLink)
{
}

SoftwareUpdateInfo::SoftwareUpdateInfo(const SoftwareUpdateInfo &other) :
  name(other.name),
  version(other.version),
  downloadLink(other.downloadLink)
{
}

SoftwareUpdateInfo & SoftwareUpdateInfo::operator=(const SoftwareUpdateInfo &other)
{
  const_cast<QString&>(name) = other.name;
  const_cast<QString&>(downloadLink) = other.downloadLink;
  const_cast<Version&>(version) = version;

  return *this;
}
