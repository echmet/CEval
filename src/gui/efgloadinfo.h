#ifndef EFGLOADINFO_H
#define EFGLOADINFO_H

#include <QMetaType>

class EFGLoadInfo {
public:
  explicit EFGLoadInfo() :
    formatTag(""),
    loadOption(0)
  {
  }
  EFGLoadInfo(const QString &formatTag, const int loadOption) :
    formatTag(formatTag),
    loadOption(loadOption)
  {
  }

  const QString formatTag;
  const int loadOption;
};
Q_DECLARE_METATYPE(EFGLoadInfo)

#endif // EFGLOADINFO_H
