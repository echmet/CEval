#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "cevalcrashhandler.h"
#include "qwt_picker_machine.h"
#include <QMessageBox>
#include <QSettings>

const QString DataAccumulator::EVALUATIONENGINE_SETTINGS_TAG("EvaluationEngine");
const QString DataAccumulator::HYPERBOLAFITTINGENGINE_SETTINGS_TAG("HyperbolaFittingEngine");

DataAccumulator::DataAccumulator(QwtPlot *plot, QObject *parent) :
  QObject(parent),
  m_plot(plot)
{
  try {
    m_commonParamsEngine = new CommonParametersEngine(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate CommonParametersEngine"));
    throw;
  }

  try {
    m_evaluationEngine = new EvaluationEngine(m_commonParamsEngine, this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate EvaluationEngine"));
    throw;
  }

  try {
    m_hyperbolaFittingEngine = new HyperbolaFittingEngine(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate HyperbolaFittingEngine"));
    throw;
  } catch (HyperbolaFittingEngine::regressor_initialization_error &ex) {
    QMessageBox::critical(nullptr, tr("Initialization error"), QString(ex.what()));
    throw std::exception();
  }

  try {
    m_plotExporter = new PlotExporter(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate PlotExporter"));
    throw;
  }

  try {
    m_plotZoomer = new DoubleClickableQwtPlotZoomer(m_plot->canvas());
    m_plotZoomer->setTrackerMode(QwtPicker::AlwaysOn);
    m_plotZoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::LeftButton, Qt::ShiftModifier);

    m_plotPicker = new QwtPlotPicker(QwtPlot::Axis::xBottom, QwtPlot::Axis::yLeft,
                                     QwtPicker::NoRubberBand, QwtPicker::AlwaysOff,
                                     m_plot->canvas());
    m_plotPicker->setStateMachine(new QwtPickerClickPointMachine);
    m_plotPicker->setMousePattern(QwtEventPattern::MouseSelect1, Qt::RightButton);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate plot items"));
    throw;
  }

  /* Create contexts for all modes of operation */
  try {
    std::shared_ptr<PlotContext> evaluationContext = std::shared_ptr<PlotContext>(new PlotContext(m_plot, m_plotPicker, m_plotZoomer));
    m_evaluationEngine->assignContext(std::shared_ptr<PlotContextLimited>(new PlotContextLimited(evaluationContext)));
    m_plotCtxs.insert(PlotContextTypes::Types::EVALUATION, evaluationContext);

    std::shared_ptr<PlotContext> hyperbolaContext = std::shared_ptr<PlotContext>(new PlotContext(m_plot, m_plotPicker, m_plotZoomer));
    m_hyperbolaFittingEngine->assignContext(std::shared_ptr<PlotContextLimited>(new PlotContextLimited(hyperbolaContext)));
    m_plotCtxs.insert(PlotContextTypes::Types::HYPERBOLA_FIT, hyperbolaContext);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to initialize mode contexts"));
    throw;
  }

  for (std::shared_ptr<PlotContext> &ctx : m_plotCtxs) {
    PlotContext *tctx = ctx.get();
    DoubleToStringConvertor::notifyOnFormatChanged(tctx);
  }

  m_currentPlotCtx = nullptr;

  connect(m_evaluationEngine, &EvaluationEngine::registerMeasurement, m_hyperbolaFittingEngine, &HyperbolaFittingEngine::onRegisterMobility);

  /* Connect emergency mode to crash handler */
  CEvalCrashHandler::connectToEmergency<HyperbolaFittingEngine, &HyperbolaFittingEngine::onEmergencySave>(m_hyperbolaFittingEngine);
  /*if (CrashHandler::pointer() != nullptr) {
    QObject::connect(CrashHandler::pointer(), &CrashHandler::emergency, m_hyperbolaFittingEngine, &HyperbolaFittingEngine::onEmergencySave);
  }*/
}

void DataAccumulator::announceDefaultState()
{
  m_evaluationEngine->announceDefaultState();
}

void DataAccumulator::checkForCrashRecovery()
{
  m_hyperbolaFittingEngine->checkForCrashRecovery();
}

CommonParametersEngine *DataAccumulator::commonParametersEngine() const
{
  return m_commonParamsEngine;
}

EvaluationEngine *DataAccumulator::evaluationEngine() const
{
  return m_evaluationEngine;
}

HyperbolaFittingEngine *DataAccumulator::hyperbolaFittingEngine() const
{
  return m_hyperbolaFittingEngine;
}

void DataAccumulator::loadUserSettings(const QVariant &settings)
{
  if (!settings.isValid())
    return;
  if (!settings.canConvert<QMap<QString, QVariant>>())
    return;

  QMap<QString, QVariant> settingsMap = settings.value<QMap<QString, QVariant>>();

  if (settingsMap.contains(EVALUATIONENGINE_SETTINGS_TAG)) {
    QVariant eeSettings = settingsMap[EVALUATIONENGINE_SETTINGS_TAG];

    if (eeSettings.isValid())
      m_evaluationEngine->loadUserSettings(eeSettings);
  }

  if (settingsMap.contains(HYPERBOLAFITTINGENGINE_SETTINGS_TAG)) {
    QVariant hfSettings = settingsMap[HYPERBOLAFITTINGENGINE_SETTINGS_TAG];

    if (hfSettings.isValid())
      m_hyperbolaFittingEngine->loadUserSettings(hfSettings);
  }

}

void DataAccumulator::onAdjustPlotAppearance()
{
  m_currentPlotCtx->adjustAppearance();
}

void DataAccumulator::onExportAction(const DataAccumulatorMsgs::ExportAction action)
{
  switch (action) {
  case DataAccumulatorMsgs::ExportAction::EXPORT_DATATABLE_CSV:
    m_hyperbolaFittingEngine->exportToCsv();
    break;
  case DataAccumulatorMsgs::ExportAction::EXPORT_PLOT:
    m_plotExporter->exportPlot(m_plot, m_plotZoomer->zoomRect());
    break;
  case DataAccumulatorMsgs::ExportAction::WHOLE_PEAK_TO_CLIPBOARD:
    m_evaluationEngine->onCopyToClipboard(EvaluationEngineMsgs::CopyToClipboard::EVERYTHING);
    break;
  }
}

void DataAccumulator::onTabSwitched(const int idx)
{
  PlotContextTypes::Types mid = PlotContextTypes::fromID<PlotContextTypes::Types>(idx);
  DataAccumulatorMsgs::ProgramMode pmode;

  if (m_currentPlotCtx != nullptr)
    m_currentPlotCtx->deactivate();

  m_plotCtxs[mid]->activate();
  m_currentPlotCtx = m_plotCtxs[mid];

  switch (mid) {
  case PlotContextTypes::Types::EVALUATION:
    pmode = DataAccumulatorMsgs::ProgramMode::EVALUATION;
    break;
  case PlotContextTypes::Types::HYPERBOLA_FIT:
    m_hyperbolaFittingEngine->refreshModels();
    pmode = DataAccumulatorMsgs::ProgramMode::HYPERBOLA_FIT;
    break;
  default:
    return;
    break;
  }

  emit programModeChanged(pmode);
}

QVariant DataAccumulator::saveUserSettings() const
{
  QMap<QString, QVariant> map;

  map.insert(EVALUATIONENGINE_SETTINGS_TAG, m_evaluationEngine->saveUserSettings());
  map.insert(HYPERBOLAFITTINGENGINE_SETTINGS_TAG, m_hyperbolaFittingEngine->saveUserSettings());

  return map;
}
