#ifndef MAINCONTROLSWIDGETCONNECTOR_H
#define MAINCONTROLSWIDGETCONNECTOR_H

#include <QObject>

class MainControlsWidgetConnector
{
public:
  MainControlsWidgetConnector() = delete;

  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // MAINCONTROLSWIDGETCONNECTOR_H
