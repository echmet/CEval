#ifndef COMMONPARAMETERSITEMS_H
#define COMMONPARAMETERSITEMS_H

#include <QObject>

class CommonParametersItems : public QObject {
  Q_OBJECT
public:
  CommonParametersItems() = delete;
  CommonParametersItems(QObject *parent) = delete;

  enum class Floating : int {
    T_EOF,
    CAPILLARY,
    DETECTOR,
    VOLTAGE,
    FIELD,
    SELECTOR,
    LAST_INDEX
  };
  Q_ENUM(Floating)

};

#endif // COMMONPARAMETERSITEMS_H

