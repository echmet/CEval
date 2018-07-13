#ifndef EVALUATIONUPPERWIDGETCONNECTOR_H
#define EVALUATIONUPPERWIDGETCONNECTOR_H

#include <QObject>

class EvaluationUpperWidgetConnector
{
public:
  EvaluationUpperWidgetConnector() = delete;

  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // EVALUATIONUPPERWIDGETCONNECTOR_H
