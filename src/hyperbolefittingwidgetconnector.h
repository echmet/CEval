#ifndef HYPERBOLEFITTINGWIDGETCONNECTOR_H
#define HYPERBOLEFITTINGWIDGETCONNECTOR_H

#include <QObject>

class HyperboleFittingWidgetConnector
{
public:
  HyperboleFittingWidgetConnector() = delete;

  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // HYPERBOLEFITTINGWIDGETCONNECTOR_H
