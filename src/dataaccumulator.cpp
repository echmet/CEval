#include "dataaccumulator.h"
#include "doubletostringconvertor.h"
#include "crashhandler.h"
#include "qwt_picker_machine.h"
#include <QMessageBox>
#include <QSettings>

const QString DataAccumulator::EVALUATIONENGINE_SETTINGS_TAG("EvaluationEngine");
const QString DataAccumulator::HYPERBOLEFITTINGENGINE_SETTINGS_TAG("HyperboleFittingEngine");

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
    m_hyperboleFittingEngine = new HyperboleFittingEngine(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate HyperboleFittingEngine"));
    throw;
  } catch (HyperboleFittingEngine::regressor_initialization_error &ex) {
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
    m_plotZoomer = new QwtPlotZoomer(m_plot->canvas());
    m_plotZoomer->setTrackerMode(QwtPicker::AlwaysOn);
    m_plotZoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::LeftButton, Qt::ShiftModifier);

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
    std::shared_ptr<ModeContext> evaluationContext = std::shared_ptr<ModeContext>(new ModeContext(m_plot, m_plotPicker, m_plotZoomer));
    m_evaluationEngine->assignContext(std::shared_ptr<ModeContextLimited>(new ModeContextLimited(evaluationContext)));
    m_modeCtxs.insert(ModeContextTypes::Types::EVALUATION, evaluationContext);

    std::shared_ptr<ModeContext> hyperboleContext = std::shared_ptr<ModeContext>(new ModeContext(m_plot, m_plotPicker, m_plotZoomer));
    m_hyperboleFittingEngine->assignContext(std::shared_ptr<ModeContextLimited>(new ModeContextLimited(hyperboleContext)));
    m_modeCtxs.insert(ModeContextTypes::Types::HYPERBOLE_FIT, hyperboleContext);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to initialize mode contexts"));
    throw;
  }

  for (std::shared_ptr<ModeContext> &ctx : m_modeCtxs) {
    ModeContext *tctx = ctx.get();
    DoubleToStringConvertor::notifyOnFormatChanged(tctx);
  }

  m_currentModeCtx = nullptr;

  connect(m_evaluationEngine, &EvaluationEngine::registerMeasurement, m_hyperboleFittingEngine, &HyperboleFittingEngine::onRegisterMobility);

  /* Connect emergency mode to crash handler */
  if (CrashHandler::pointer() != nullptr) {
    QObject::connect(CrashHandler::pointer(), &CrashHandler::emergency, m_hyperboleFittingEngine, &HyperboleFittingEngine::onEmergencySave);
  }
}

void DataAccumulator::checkForCrashRecovery()
{
  m_hyperboleFittingEngine->checkForCrashRecovery();
}

CommonParametersEngine *DataAccumulator::commonParametersEngine() const
{
  return m_commonParamsEngine;
}

EvaluationEngine *DataAccumulator::evaluationEngine() const
{
  return m_evaluationEngine;
}

HyperboleFittingEngine *DataAccumulator::hyperboleFittingEngine() const
{
  return m_hyperboleFittingEngine;
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

  if (settingsMap.contains(HYPERBOLEFITTINGENGINE_SETTINGS_TAG)) {
    QVariant hfSettings = settingsMap[HYPERBOLEFITTINGENGINE_SETTINGS_TAG];

    if (hfSettings.isValid())
      m_hyperboleFittingEngine->loadUserSettings(hfSettings);
  }

}

void DataAccumulator::onAdjustPlotAppearance()
{
  m_currentModeCtx->adjustAppearance();
}

void DataAccumulator::onExportAction(const DataAccumulatorMsgs::ExportAction action)
{
  switch (action) {
  case DataAccumulatorMsgs::ExportAction::EXPORT_DATATABLE_CSV:
    m_hyperboleFittingEngine->exportToCsv();
    break;
  case DataAccumulatorMsgs::ExportAction::EXPORT_PLOT:
    m_plotExporter->exportPlot(m_plot);
    break;
  }
}

void DataAccumulator::onTabSwitched(const int idx)
{
  ModeContextTypes::Types mid = ModeContextTypes::fromID<ModeContextTypes::Types>(idx);
  DataAccumulatorMsgs::ProgramMode pmode;

  if (m_currentModeCtx != nullptr)
    m_currentModeCtx->deactivate();

  m_modeCtxs[mid]->activate();
  m_currentModeCtx = m_modeCtxs[mid];

  switch (mid) {
  case ModeContextTypes::Types::EVALUATION:
    pmode = DataAccumulatorMsgs::ProgramMode::EVALUATION;
    break;
  case ModeContextTypes::Types::HYPERBOLE_FIT:
    m_hyperboleFittingEngine->refreshModels();
    pmode = DataAccumulatorMsgs::ProgramMode::HYPERBOLE_FIT;
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
  map.insert(HYPERBOLEFITTINGENGINE_SETTINGS_TAG, m_hyperboleFittingEngine->saveUserSettings());

  return map;
}
