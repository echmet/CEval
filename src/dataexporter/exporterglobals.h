#ifndef EXPORTERGLOBALS_H
#define EXPORTERGLOBALS_H

#include <QObject>

namespace DataExporter {

class Globals : public QObject {
  Q_OBJECT
public:
  Globals() = delete;

enum class DataArrangement {
  HORIZONTAL,
  VERTICAL
};
Q_ENUM(DataArrangement)

};

} // namespace DataExporter

#endif // EXPORTERGLOBALS_H
