#ifndef EVALUATIONWIDGETCONNECTOR_H
#define EVALUATIONWIDGETCONNECTOR_H

#include <QObject>

class EvaluationWidgetConnector
{
public:
  EvaluationWidgetConnector() = delete;
  static void connectAll(QObject *anonWidget, QObject *anonDac);

};

#endif // EVALUATIONWIDGETCONNECTOR_H
