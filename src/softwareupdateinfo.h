#ifndef SOFTWAREUPDATEINFO_H
#define SOFTWAREUPDATEINFO_H

#include <QString>

/* Prevent conflict with gnu_dev_major and gnu_dev_minor */
#undef major
#undef minor

class SoftwareUpdateInfo
{
public:
  class Version {
  public:
    class InvalidRevisionStringException : public std::exception {
    public:
      const char * what() const noexcept
      {
        return "Revision string may contain only letters";
      }
    };

    Version(const int major, const int minor, const QString &revision);
    Version(const Version &other);
    bool operator>(const Version &other) const;
    bool operator<(const Version &other) const;
    bool operator>=(const Version &other) const;
    bool operator<=(const Version &other) const;
    bool operator==(const Version &other) const;
    Version & operator=(const Version &other);

    const int major;
    const int minor;
    const QString revision;

  };

  SoftwareUpdateInfo();
  SoftwareUpdateInfo(const QString &name, const Version &version, const QString &downloadLink);
  SoftwareUpdateInfo(const SoftwareUpdateInfo &other);
  SoftwareUpdateInfo & operator=(const SoftwareUpdateInfo &other);

  const QString name;
  const Version version;
  const QString downloadLink;

};

#endif // SOFTWAREUPDATEINFO_H
