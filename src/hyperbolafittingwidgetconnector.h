#ifndef HYPERBOLAFITTINGWIDGETCONNECTOR_H
#define HYPERBOLAFITTINGWIDGETCONNECTOR_H

#include <QObject>

class HyperbolaFittingWidgetConnector
{
public:
  HyperbolaFittingWidgetConnector() = delete;

  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // HYPERBOLAFITTINGWIDGETCONNECTOR_H
