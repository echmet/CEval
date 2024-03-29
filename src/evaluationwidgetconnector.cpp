#include "evaluationwidgetconnector.h"
#include <QDebug>
#include "gui/evalmainwindow.h"
#include "dataaccumulator.h"
#include "helpers.h"
#include "efg/efgloaderinterface.h"

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
  w->setEvaluationHvlBooleanModel(e->hvlFitBooleanModel());
  w->setEvaluationHvlExtrapolationModels(e->hvlExtrapolatorModels());
  w->setEvaluationHvlFitIntModel(e->hvlFitIntModel());
  w->setEvaluationHvlFitModel(e->hvlFitModel());
  w->setEvaluationHvlFitOptionsModel(e->hvlFitOptionsModel());
  w->setEvaluationParametersAutoModel(e->autoValuesModel());
  w->setEvaluationParametersBooleanModel(e->booleanValuesModel());
  w->setEvaluationParametersFloatingModel(e->floatingValuesModel());
  w->setEvaluationResultsModel(e->resultsValuesModel());
  w->setSNRModel(e->snrModel());
  w->setEvaluationShowWindowModel(e->showWindowModel());
  w->setEvaluationWindowUnitsModel(e->windowUnitsModel());
  w->setExporterBackendsModel(e->exporterBackendsModel());
  w->setExporterSchemesModel(e->exporterSchemesModel());
  w->setClipboardDataArrangementModel(e->clipboardDataArrangementModel());

  QObject::connect(w, &EvaluationWidget::comboBoxChanged, e, &EvaluationEngine::onComboBoxChanged);
  QObject::connect(w, &EvaluationWidget::evaluationSetDefault, e, &EvaluationEngine::onSetDefault);
  QObject::connect(e, &EvaluationEngine::comboBoxIndexChanged, w, &EvaluationWidget::onComboBoxChangedExt);
  QObject::connect(w, &EvaluationWidget::findPeaks, e, &EvaluationEngine::onFindPeaks);
  QObject::connect(mw, &EvalMainWindow::loadElectrophoregram , &EFGLoaderInterface::instance(), &EFGLoaderInterface::loadData);
  QObject::connect(w, &EvaluationWidget::doHvlFit, e, &EvaluationEngine::onDoHvlFit);
  QObject::connect(w, &EvaluationWidget::hvlExtrapolationToggled, e, &EvaluationEngine::onHvlExtrapolationToggled);
  QObject::connect(w, &EvaluationWidget::replotHvl, e, &EvaluationEngine::onReplotHvl);
  QObject::connect(w, &EvaluationWidget::copyToClipboard, e, &EvaluationEngine::onCopyToClipboard);
  QObject::connect(w, &EvaluationWidget::configureExporterBackend, e, &EvaluationEngine::onConfigureExporterBackend);
  QObject::connect(w, &EvaluationWidget::exportScheme, e, &EvaluationEngine::onExportScheme);
  QObject::connect(w, &EvaluationWidget::manageExporterSchemes, e, &EvaluationEngine::onManageExporterScheme);
  QObject::connect(w, &EvaluationWidget::exporterBackendChanged, e, &EvaluationEngine::onExporterBackendChanged);
  QObject::connect(w, &EvaluationWidget::exporterSchemeChanged, e, &EvaluationEngine::onExporterSchemeChanged);
  QObject::connect(w, &EvaluationWidget::clipboardExporterArrangementChanged, e, &EvaluationEngine::onClipboardExporterDataArrangementChanged);
  QObject::connect(w, &EvaluationWidget::clipboardExporterDelimiterChanged, e, &EvaluationEngine::onClipboardExporterDelimiterChanged);
  QObject::connect(e, &EvaluationEngine::clipboardExporterDataArrangementSet, w, &EvaluationWidget::onClipboardExporterDataArrangementSet);
  QObject::connect(e, &EvaluationEngine::clipboardExporterDelimiterSet, w, &EvaluationWidget::onClipboardExporterDelimiterSet);
  QObject::connect(w, &EvaluationWidget::exportOnFileLeftToggled, e, &EvaluationEngine::onExportFileOnLeftToggled);
  QObject::connect(e, &EvaluationEngine::exporterBackendSet, w, &EvaluationWidget::onExporterBackendSet);
}
