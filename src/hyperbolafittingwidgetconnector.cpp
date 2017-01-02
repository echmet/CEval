#include "hyperbolafittingwidgetconnector.h"
#include "gui/hyperbolafitwidget.h"
#include "dataaccumulator.h"
#include "gui/evalmainwindow.h"
#include "helpers.h"
#include "hyperbolafittingengine.h"

void HyperbolaFittingWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  HyperbolaFitWidget *w = qobject_cast<HyperbolaFitWidget *>(anonWidget);
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  HyperbolaFittingEngine *e;
  const EvalMainWindow *mw;

  Q_ASSERT(w != nullptr && dac != nullptr);

  mw = Helpers::getRootWidget<EvalMainWindow>(w);
  if (mw == nullptr) {
    qCritical() << "Unable to get root widget";
    abort();
  }

  e = dac->hyperbolaFittingEngine();

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

  QObject::connect(w, &HyperbolaFitWidget::addAnalyte, e, &HyperbolaFittingEngine::onAddAnalyte);
  QObject::connect(w, &HyperbolaFitWidget::removeAnalyte, e, &HyperbolaFittingEngine::onRemoveAnalyte);
  QObject::connect(w, &HyperbolaFitWidget::analyteSwitched, e, &HyperbolaFittingEngine::onAnalyteSwitched);
  QObject::connect(w, &HyperbolaFitWidget::renameAnalyte, e, &HyperbolaFittingEngine::onRenameAnalyte);

  QObject::connect(w, &HyperbolaFitWidget::addConcentration, e, &HyperbolaFittingEngine::onAddConcentration);
  QObject::connect(w, &HyperbolaFitWidget::removeConcentration, e, &HyperbolaFittingEngine::onRemoveConcentration);
  QObject::connect(w, &HyperbolaFitWidget::editConcentration, e, &HyperbolaFittingEngine::onEditConcentration);
  QObject::connect(w, &HyperbolaFitWidget::concentrationSwitched, e, &HyperbolaFittingEngine::onConcentrationSwitched);

  QObject::connect(w, &HyperbolaFitWidget::addMobility, e, &HyperbolaFittingEngine::onAddMobility);
  QObject::connect(w, &HyperbolaFitWidget::removeMobility, e, &HyperbolaFittingEngine::onRemoveMobility);
  QObject::connect(w, &HyperbolaFitWidget::editMobility, e, &HyperbolaFittingEngine::onEditMobility);

  QObject::connect(w, &HyperbolaFitWidget::fitModeChanged, e, &HyperbolaFittingEngine::onFitModeChanged);
  QObject::connect(w, &HyperbolaFitWidget::statModeChanged, e, &HyperbolaFittingEngine::onStatModeChanged);
  QObject::connect(w, &HyperbolaFitWidget::statUnitsChanged, e, &HyperbolaFittingEngine::onStatUnitsChanged);
  QObject::connect(w, &HyperbolaFitWidget::statsForAnalyteChanged, e, &HyperbolaFittingEngine::onStatsForAnalyteChanged);
  QObject::connect(e, &HyperbolaFittingEngine::enableDoubleFit, w, &HyperbolaFitWidget::onEnableDoubleFit);

  QObject::connect(w, &HyperbolaFitWidget::doEstimate, e, &HyperbolaFittingEngine::onDoEstimate);
  QObject::connect(w, &HyperbolaFitWidget::doFit, e, &HyperbolaFittingEngine::onDoFit);
  QObject::connect(w, &HyperbolaFitWidget::doStats, e, &HyperbolaFittingEngine::onDoStats);
  QObject::connect(w, &HyperbolaFitWidget::chartMarkerValueChanged, e, &HyperbolaFittingEngine::onChartMarkerValueChanged);
  QObject::connect(w, &HyperbolaFitWidget::showChartMarker, e, &HyperbolaFittingEngine::onShowChartMarker);
  QObject::connect(w, &HyperbolaFitWidget::chartHorizontalMarkerIntersection, e, &HyperbolaFittingEngine::onChartHorizontalMarkerIntersection);
  QObject::connect(w, &HyperbolaFitWidget::chartVerticalMarkerIntersection, e, &HyperbolaFittingEngine::onChartVerticalMarkerIntersection);
  QObject::connect(e, &HyperbolaFittingEngine::chartHorizontalMarkerIntersectionSet, w, &HyperbolaFitWidget::onChartHorizontalMarkerIntersectionSet);
  QObject::connect(e, &HyperbolaFittingEngine::chartVerticalMarkerIntersectionSet, w, &HyperbolaFitWidget::onChartVerticalMarkerIntersectionSet);

  QObject::connect(w, &HyperbolaFitWidget::redrawDataSeries, e, &HyperbolaFittingEngine::onRedrawDataSeries);
  QObject::connect(w, &HyperbolaFitWidget::clearHyperbola, e, &HyperbolaFittingEngine::onClearHyperbola);
  QObject::connect(e, &HyperbolaFittingEngine::sortLists, w, &HyperbolaFitWidget::onSortLists);

  QObject::connect(mw, &EvalMainWindow::loadDataTable, e, &HyperbolaFittingEngine::onDeserialize);
  QObject::connect(mw, &EvalMainWindow::saveDataTable, e, &HyperbolaFittingEngine::onSerialize);
}


