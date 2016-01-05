#include "evaluatedpeakswidgetconnector.h"
#include "gui/evaluatedpeakswidget.h"
#include "dataaccumulator.h"

void EvaluatedPeaksWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  EvaluatedPeaksWidget *widget = qobject_cast<EvaluatedPeaksWidget *>(anonWidget);
  EvaluationEngine *e;

  Q_ASSERT(dac != nullptr && widget != nullptr);
  e = dac->evaluationEngine();

  widget->setModel(e->evaluatedPeaksModel());

  QObject::connect(widget, &EvaluatedPeaksWidget::addPeakClicked, e, &EvaluationEngine::onAddPeak);
  QObject::connect(widget, &EvaluatedPeaksWidget::cancelSelection, e, &EvaluationEngine::onCancelEvaluatedPeakSelection);
  QObject::connect(widget, &EvaluatedPeaksWidget::deletePeakClicked, e, &EvaluationEngine::onDeletePeak);
  QObject::connect(widget, &EvaluatedPeaksWidget::peakSwitched, e, &EvaluationEngine::onPeakSwitched);
}
