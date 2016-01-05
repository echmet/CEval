#ifndef COMMONPARAMETERSWIDGETCONNECTOR_H
#define COMMONPARAMETERSWIDGETCONNECTOR_H

#include <QObject>

class CommonParametersWidgetConnector
{
public:
  CommonParametersWidgetConnector() = delete;

  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // COMMONPARAMETERSWIDGETCONNECTOR_H
