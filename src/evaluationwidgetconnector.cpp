#include "evaluationwidgetconnector.h"
#include "gui/evalmainwindow.h"
#include "dataaccumulator.h"
#include "helpers.h"

void EvaluationWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  EvaluationWidget *w = qobject_cast<EvaluationWidget *>(anonWidget);
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  const EvalMainWindow *mw;
  EvaluationEngine *e;

  Q_ASSERT(w != nullptr && dac != nullptr);

  mw = Helpers::getRootWidget<EvalMainWindow>(w);
  if (mw == nullptr) {
    qCritical() << "Unable to get root widget";
    abort();
  }

  e = dac->evaluationEngine();

  /* Bind models */
  w->setEvaluationBaselineModel(e->baselineModel());
  w->setEvaluationHvlFixedModel(e->hvlFitFixedModel());
  w->setEvaluationHvlFitIntModel(e->hvlFitIntModel());
  w->setEvaluationHvlFitModel(e->hvlFitModel());
  w->setEvaluationParametersAutoModel(e->autoValuesModel());
  w->setEvaluationParametersBooleanModel(e->booleanValuesModel());
  w->setEvaluationParametersFloatingModel(e->floatingValuesModel());
  w->setEvaluationResultsModel(e->resultsValuesModel());
  w->setEvaluationShowWindowModel(e->showWindowModel());
  w->setEvaluationWindowUnitsModel(e->windowUnitsModel());

  QObject::connect(w, &EvaluationWidget::comboBoxChanged, e, &EvaluationEngine::onComboBoxChanged);
  QObject::connect(w, &EvaluationWidget::evaluationSetDefault, e, &EvaluationEngine::onSetDefault);
  QObject::connect(e, &EvaluationEngine::comboBoxIndexChanged, w, &EvaluationWidget::onComboBoxChangedExt);
  QObject::connect(w, &EvaluationWidget::findPeaks, e, &EvaluationEngine::onFindPeaks);
  QObject::connect(mw, &EvalMainWindow::loadDataFile, e->dataFileLoader(), &DataFileLoader::onLoadDataFile);
  QObject::connect(w, &EvaluationWidget::doHvlFit, e, &EvaluationEngine::onDoHvlFit);
  QObject::connect(w, &EvaluationWidget::replotHvl, e, &EvaluationEngine::onReplotHvl);
  QObject::connect(w, &EvaluationWidget::showHvlFitStats, e, &EvaluationEngine::onShowHvlFitStatsChanged);
  QObject::connect(w, &EvaluationWidget::copyToClipboard, e, &EvaluationEngine::onCopyToClipboard);
}
