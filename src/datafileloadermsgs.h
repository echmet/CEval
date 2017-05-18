#ifndef DATAFILELOADERMSGS_H
#define DATAFILELOADERMSGS_H

#include <QObject>

class DataFileLoaderMsgs : public QObject {
  Q_OBJECT
public:
  enum class LoadableFileTypes : int {
    CHEMSTATION,
    COMMA_SEPARATED_CLIPBOARD,
    COMMA_SEPARATED_FILE,
    NETCDF_FILE
  };
  Q_ENUM(LoadableFileTypes)
};

#endif // DATAFILELOADERMSGS_H

