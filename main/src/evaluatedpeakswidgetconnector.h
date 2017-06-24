#ifndef EVALUATEDPEAKSWIDGETCONNECTOR_H
#define EVALUATEDPEAKSWIDGETCONNECTOR_H

#include <QObject>

class EvaluatedPeaksWidgetConnector
{
public:
  EvaluatedPeaksWidgetConnector() = delete;
  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // EVALUATEDPEAKSWIDGETCONNECTOR_H
