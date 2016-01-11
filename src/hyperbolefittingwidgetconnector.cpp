#include "hyperbolefittingwidgetconnector.h"
#include "gui/hyperbolefitwidget.h"
#include "dataaccumulator.h"
#include "gui/evalmainwindow.h"
#include "helpers.h"
#include "hyperbolefittingengine.h"

void HyperboleFittingWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  HyperboleFitWidget *w = qobject_cast<HyperboleFitWidget *>(anonWidget);
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  HyperboleFittingEngine *e;
  const EvalMainWindow *mw;

  Q_ASSERT(w != nullptr && dac != nullptr);

  mw = Helpers::getRootWidget<EvalMainWindow>(w);
  if (mw == nullptr) {
    qCritical() << "Unable to get root widget";
    abort();
  }

  e = dac->hyperboleFittingEngine();

  w->setAnalyteNamesModel(e->analyteNamesModel());
  w->setAnalytesModel(e->analytesModel());
  w->setConcentrationsModel(e->concentrationsModel());
  w->setFitFixedModel(e->fitFixedModel());
  w->setFitFloatModel(e->fitFloatModel());
  w->setFitIntModel(e->fitIntModel());
  w->setFitResultsModel(e->fitResultsModel());
  w->setFitModeModel(e->fitModeModel());
  w->setMobilitiesModel(e->mobilitiesModel());
  w->setStatModeModel(e->statModeModel());
  w->setStatUnitsModel(e->statUnitsModel());

  QObject::connect(w, &HyperboleFitWidget::addAnalyte, e, &HyperboleFittingEngine::onAddAnalyte);
  QObject::connect(w, &HyperboleFitWidget::removeAnalyte, e, &HyperboleFittingEngine::onRemoveAnalyte);
  QObject::connect(w, &HyperboleFitWidget::analyteSwitched, e, &HyperboleFittingEngine::onAnalyteSwitched);
  QObject::connect(w, &HyperboleFitWidget::renameAnalyte, e, &HyperboleFittingEngine::onRenameAnalyte);

  QObject::connect(w, &HyperboleFitWidget::addConcentration, e, &HyperboleFittingEngine::onAddConcentration);
  QObject::connect(w, &HyperboleFitWidget::removeConcentration, e, &HyperboleFittingEngine::onRemoveConcentration);
  QObject::connect(w, &HyperboleFitWidget::concentrationSwitched, e, &HyperboleFittingEngine::onConcentrationSwitched);

  QObject::connect(w, &HyperboleFitWidget::addMobility, e, &HyperboleFittingEngine::onAddMobility);
  QObject::connect(w, &HyperboleFitWidget::removeMobility, e, &HyperboleFittingEngine::onRemoveMobility);

  QObject::connect(w, &HyperboleFitWidget::fitModeChanged, e, &HyperboleFittingEngine::onFitModeChanged);
  QObject::connect(w, &HyperboleFitWidget::statModeChanged, e, &HyperboleFittingEngine::onStatModeChanged);
  QObject::connect(w, &HyperboleFitWidget::statUnitsChanged, e, &HyperboleFittingEngine::onStatUnitsChanged);
  QObject::connect(w, &HyperboleFitWidget::swapAnalytesChanged, e, &HyperboleFittingEngine::onSwapAnalytesChanged);
  QObject::connect(e, &HyperboleFittingEngine::enableDoubleFit, w, &HyperboleFitWidget::onEnableDoubleFit);
  QObject::connect(e, &HyperboleFittingEngine::swapAnalyteNamesModel, w, &HyperboleFitWidget::onSwapAnalyteNamesModel);

  QObject::connect(w, &HyperboleFitWidget::doEstimate, e, &HyperboleFittingEngine::onDoEstimate);
  QObject::connect(w, &HyperboleFitWidget::doFit, e, &HyperboleFittingEngine::onDoFit);
  QObject::connect(w, &HyperboleFitWidget::doStats, e, &HyperboleFittingEngine::onDoStats);

  QObject::connect(w, &HyperboleFitWidget::redrawDataSeries, e, &HyperboleFittingEngine::onRedrawDataSeries);

  QObject::connect(mw, &EvalMainWindow::loadDataTable, e, &HyperboleFittingEngine::onDeserialize);
  QObject::connect(mw, &EvalMainWindow::saveDataTable, e, &HyperboleFittingEngine::onSerialize);
}


