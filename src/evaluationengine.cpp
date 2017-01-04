#include "evaluationengine.h"
#include "doubletostringconvertor.h"
#include "gui/appendoverwriteexportfilemessagebox.h"
#include "gui/registerinhyperbolafitdialog.h"
#include "gui/setaxistitlesdialog.h"
#include "gui/specifypeakboundariesdialog.h"
#include "gui/textexporterbackendconfigurationdialog.h"
#include "custommetatypes.h"
#include "helpers.h"
#include "hvlcalculator.h"
#include "manualpeakfinder.h"
#include "standardplotcontextsettingshandler.h"
#include "witchcraft.h"
#include "dataexporter/backends/textexporterbackend.h"
#include "dataexporter/backends/htmlexporterbackend.h"
#include "dataexporter/backends/textstreamexporterbackend.h"
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTextStream>

const QVector<bool> EvaluationEngine::s_defaultEvaluationAutoValues({true, /* SLOPE_WINDOW */
                                                                     true, /* NOISE */
                                                                     true, /* SLOPE_REF_POINT */
                                                                     true, /* SLOPE_THRESHOLD */
                                                                     true, /* FROM */
                                                                     true  /* TO */
                                                                    });

const QVector<bool> EvaluationEngine::s_defaultEvaluationBooleanValues({false, /* NOISE_CORRECTION */
                                                                        true   /* DISTURBANCE_DETECTION */
                                                                       });

/* Do not reorder these before reviewing the rest of the code! */
const QVector<double> EvaluationEngine::s_defaultEvaluationFloatingValues({0.5, /* NOISE_WINDOW */
                                                                          0.5, /* PEAK_WINDOW */
                                                                          1.0, /* SLOPE_WINDOW */
                                                                          1.0, /* NOISE */
                                                                          1.0, /* NOISE_LOD_COEFF */
                                                                          50.0, /* SLOPE_SENSITIVITY */
                                                                          1.0, /* SLOPE_THRESHOLD */
                                                                          0.0, /* NOISE_REF_POINT*/
                                                                          0.0, /* SLOPE_REF_POINT */
                                                                          0.0, /* FROM */
                                                                          0.0, /* TO */
                                                                          0.2 /* DISTURBANCE_DETECTION */
                                                                          });

const EvaluationParametersItems::ComboBaselineAlgorithm EvaluationEngine::s_defaultBaselineAlgorithm = EvaluationParametersItems::ComboBaselineAlgorithm::SLOPE;
const EvaluationParametersItems::ComboShowWindow EvaluationEngine::s_defaultShowWindow = EvaluationParametersItems::ComboShowWindow::NONE;
const EvaluationParametersItems::ComboWindowUnits EvaluationEngine::s_defaultWindowUnits = EvaluationParametersItems::ComboWindowUnits::MINUTES;

const QVector<ComboBoxItem<EvaluationParametersItems::ComboWindowUnits>>
EvaluationEngine::s_windowUnitsValues({ ComboBoxItem<EvaluationParametersItems::ComboWindowUnits>(tr("Minutes"), EvaluationParametersItems::ComboWindowUnits::MINUTES),
                                        ComboBoxItem<EvaluationParametersItems::ComboWindowUnits>(tr("Points"), EvaluationParametersItems::ComboWindowUnits::POINTS )});

const QVector<ComboBoxItem<EvaluationParametersItems::ComboBaselineAlgorithm>>
EvaluationEngine::s_baselineAlgorithmValues({ ComboBoxItem<EvaluationParametersItems::ComboBaselineAlgorithm>(tr("Slope"), EvaluationParametersItems::ComboBaselineAlgorithm::SLOPE),
                                              ComboBoxItem<EvaluationParametersItems::ComboBaselineAlgorithm>(tr("Noise"), EvaluationParametersItems::ComboBaselineAlgorithm::NOISE) });

const QVector<ComboBoxItem<EvaluationParametersItems::ComboShowWindow>>
EvaluationEngine::s_showWindowValues({ ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("None"), EvaluationParametersItems::ComboShowWindow::NONE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Noise"), EvaluationParametersItems::ComboShowWindow::NOISE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Slope threshold"), EvaluationParametersItems::ComboShowWindow::SLOPE_THRESHOLD),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Peak window"), EvaluationParametersItems::ComboShowWindow::PEAK_WINDOW),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Peak height"), EvaluationParametersItems::ComboShowWindow::PEAK_HEIGHT),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Peak half width"), EvaluationParametersItems::ComboShowWindow::PEAK_WIDTH_HALF),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Peak width 0.05"), EvaluationParametersItems::ComboShowWindow::NOISE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Baseline"), EvaluationParametersItems::ComboShowWindow::BASELINE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Smoothed"), EvaluationParametersItems::ComboShowWindow::SMOOTHED),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("1st derivative"), EvaluationParametersItems::ComboShowWindow::FIRST_DERIVATIVE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("2nd derivative"), EvaluationParametersItems::ComboShowWindow::SECOND_DERIVATIVE),
                                       ComboBoxItem<EvaluationParametersItems::ComboShowWindow>(tr("Both derivatives"), EvaluationParametersItems::ComboShowWindow::BOTH_DERIVATIVES)});

const QString EvaluationEngine::s_serieBaselineTitle = QString(tr("Peak baseline"));
const QString EvaluationEngine::s_serieEofTitle = QString(tr("EOF"));
const QString EvaluationEngine::s_seriePeakHeightTitle = QString(tr("Peak height"));
const QString EvaluationEngine::s_seriePeakTimeTitle = QString(tr("Peak maximum time"));
const QString EvaluationEngine::s_serieProvisionalPeakTitle = QString(tr("Provisional peak"));
const QString EvaluationEngine::s_serieSignalTitle = QString(tr("Signal"));
const QString EvaluationEngine::s_serieFinderSystemATitle = QString(tr("Finder A"));
const QString EvaluationEngine::s_serieFinderSystemBTitle = QString(tr("Finder A"));
const QString EvaluationEngine::s_serieHVLTitle = QString(tr("HVL"));
const QString EvaluationEngine::s_serieBaselineFromTitle = QString(tr("Baseline from"));
const QString EvaluationEngine::s_serieBaselineToTitle = QString(tr("Baseline from"));
const QString EvaluationEngine::s_serieProvisionalBaseline = QString(tr("Provisional baseline"));

const QString EvaluationEngine::s_emptyCtxKey = "";

const double EvaluationEngine::s_defaultHvlEpsilon = 1.0e-9;
const int EvaluationEngine::s_defaultHvlDigits = 50;
const int EvaluationEngine::s_defaultHvlIterations = 30;

const QString EvaluationEngine::DATAFILELOADER_SETTINGS_TAG("DataFileLoader");
const QString EvaluationEngine::HVLFITOPTIONS_DISABLE_AUTO_FIT_TAG("HVLFitOptions-DisableAutoFit");
const QString EvaluationEngine::HVLFITOPTIONS_SHOW_FIT_STATS_TAG("HVLFitOptions-ShowFitStats");
const QString EvaluationEngine::CLIPBOARDEXPORTER_DELIMTIER_TAG("ClipboardExporter-Delimiter");
const QString EvaluationEngine::CLIPBOARDEXPORTER_DATAARRANGEMENT_TAG("ClipboardExporter-DataArrangement");

EvaluationEngine::DataContext::DataContext(std::shared_ptr<DataFileLoader::Data> data, const QString &name,
                                           const CommonParametersEngine::Context &commonCtx, const EvaluationContext &evalCtx) :
  data(data),
  name(name),
  commonContext(commonCtx),
  evaluationContext(evalCtx)
{
}

EvaluationEngine::EvaluationContext::EvaluationContext(const QVector<StoredPeak> &peaks, const int lastIndex) :
  peaks(peaks),
  lastIndex(lastIndex)
{
}

EvaluationEngine::StoredPeak::StoredPeak() :
  name("")
{
}

EvaluationEngine::StoredPeak::StoredPeak(const QString &name, const PeakContext &peakCtx) :
  name(name),
  m_peakCtx(peakCtx)
{
}

EvaluationEngine::AssistedFinderContext::AssistedFinderContext() :
  afAutoValues(s_defaultEvaluationAutoValues),
  afBoolValues(s_defaultEvaluationBooleanValues),
  afFloatingValues(s_defaultEvaluationFloatingValues),
  baselineAlgorithm(s_defaultBaselineAlgorithm),
  showWindow(s_defaultShowWindow),
  windowUnits(s_defaultWindowUnits)
{
}

EvaluationEngine::AssistedFinderContext::AssistedFinderContext(const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &afAutoValues,
                                                               const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &afBoolValues,
                                                               const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &afFloatingValues,
                                                               const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                                                               const EvaluationParametersItems::ComboShowWindow showWindow,
                                                               const EvaluationParametersItems::ComboWindowUnits windowUnits) :
  afAutoValues(afAutoValues),
  afBoolValues(afBoolValues),
  afFloatingValues(afFloatingValues),
  baselineAlgorithm(baselineAlgorithm),
  showWindow(showWindow),
  windowUnits(windowUnits)
{
}

void EvaluationEngine::AssistedFinderContext::setProcessingData(const double noise,
                                                                 const double noiseRefPoint, const double slopeRefPoint,
                                                                 const double slopeThreshold, const double slopeWindow)
{
  afFloatingValues[EvaluationParametersItems::Floating::NOISE] = noise;
  afFloatingValues[EvaluationParametersItems::Floating::NOISE_REF_POINT] = noiseRefPoint;
  afFloatingValues[EvaluationParametersItems::Floating::SLOPE_REF_POINT] = slopeRefPoint;
  afFloatingValues[EvaluationParametersItems::Floating::SLOPE_THRESHOLD] = slopeThreshold;
  afFloatingValues[EvaluationParametersItems::Floating::SLOPE_WINDOW] = slopeWindow;
}

EvaluationEngine::AssistedFinderContext & EvaluationEngine::AssistedFinderContext::operator=(const AssistedFinderContext &other)
{
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Auto>&>(afAutoValues) = other.afAutoValues;
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Boolean>&>(afBoolValues) = other.afBoolValues;
  const_cast<MappedVectorWrapper<double, EvaluationParametersItems::Floating>&>(afFloatingValues) = other.afFloatingValues;
  const_cast<EvaluationParametersItems::ComboBaselineAlgorithm&>(baselineAlgorithm) = other.baselineAlgorithm;
  const_cast<EvaluationParametersItems::ComboShowWindow&>(showWindow) = other.showWindow;
  const_cast<EvaluationParametersItems::ComboWindowUnits&>(windowUnits) = other.windowUnits;

  return *this;
}

const EvaluationEngine::PeakContext &EvaluationEngine::StoredPeak::peak() const
{
  return m_peakCtx;
}

EvaluationEngine::PeakContextModels::PeakContextModels(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                                                       const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                                       const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                                       const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitFixedValues) :
  resultsValues(resultsValues),
  hvlValues(hvlValues),
  hvlFitIntValues(hvlFitIntValues),
  hvlFitFixedValues(hvlFitFixedValues)
{
}

void EvaluationEngine::StoredPeak::updatePeak(const EvaluationEngine::PeakContext &peakCtx)
{
  m_peakCtx = peakCtx;
}

EvaluationEngine::StoredPeak &EvaluationEngine::StoredPeak::operator=(const StoredPeak &other)
{
  const_cast<QString &>(name) = other.name;
  m_peakCtx = other.m_peakCtx;

  return *this;
}

EvaluationEngine::EvaluationContext &EvaluationEngine::EvaluationContext::operator=(const EvaluationContext &other)
{
  const_cast<QVector<StoredPeak>&>(peaks) = other.peaks;
  const_cast<int&>(lastIndex) = other.lastIndex;

  return *this;
}

EvaluationEngine::EvaluationEngine(CommonParametersEngine *commonParamsEngine, QObject *parent) : QObject(parent),
  m_userInteractionState(UserInteractionState::FINDING_PEAK),
  m_commonParamsEngine(commonParamsEngine),
  m_evaluationAutoValues(s_defaultEvaluationAutoValues),
  m_evaluationBooleanValues(s_defaultEvaluationBooleanValues),
  m_evaluationFloatingValues(s_defaultEvaluationFloatingValues),
  m_baselineAlgorithmModel(s_baselineAlgorithmValues, this),
  m_loadedFilesModel(QVector<ComboBoxItem<QString>>(), this),
  m_showWindowModel(s_showWindowValues, this),
  m_windowUnitsModel(s_windowUnitsValues, this),
  m_dataExporter("EVALUATION_ENGINE_DEXP")
{
  try {
    m_dataFileLoader = new DataFileLoader(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate DataFileLoader"));
    throw;
  }

  try {
    createContextMenus();
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate context menus"));
    throw;
  }

  try {
    /* Exporters to file */
    initDataExporter();
    m_dataExporterBackendsModel = new QStandardItemModel();

    QStandardItem *item = new QStandardItem(tr("Text"));
    item->setData(QVariant::fromValue<DataExporterBackends>(DataExporterBackends::TEXT), Qt::UserRole);
    m_dataExporterBackendsModel->appendRow(item);

    item = new QStandardItem(tr("HTML"));
    item->setData(QVariant::fromValue<DataExporterBackends>(DataExporterBackends::HTML), Qt::UserRole);
    m_dataExporterBackendsModel->appendRow(item);
    m_currentDataExporterBackend = DataExporterBackends::TEXT;

    m_dataExporterFileDlg = new QFileDialog();
    m_dataExporterFileDlg->setAcceptMode(QFileDialog::AcceptSave);
    m_dataExporterFileDlg->setOption(QFileDialog::DontConfirmOverwrite);
    m_dataExporterFileDlg->setWindowTitle(tr("Export data to file"));

    m_specifyPeakBoundsDlg = new SpecifyPeakBoundariesDialog();
    m_textDataExporterDelimiter = QChar(';');
    m_textDataExporterCfgDlg = new TextExporterBackendConfigurationDialog();

    m_exportOnFileLeftEnabled = false;

    /* Clipboard exporter */
    initClipboardExporter();
    m_ctcDataArrangementModel = new QStandardItemModel();
    item = new QStandardItem(tr("Vertical"));
    item->setData(QVariant::fromValue<DataExporter::Globals::DataArrangement>(DataExporter::Globals::DataArrangement::VERTICAL), Qt::UserRole);
    m_ctcDataArrangementModel->appendRow(item);
    item = new QStandardItem(tr("Horizontal"));
    item->setData(QVariant::fromValue<DataExporter::Globals::DataArrangement>(DataExporter::Globals::DataArrangement::HORIZONTAL), Qt::UserRole);
    m_ctcDataArrangementModel->appendRow(item);
    m_ctcDataArrangement = DataExporter::Globals::DataArrangement::VERTICAL;

    m_ctcDelimiter = ";";
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate data exporter"));
    throw;
  }

  m_evaluationAutoModel.setUnderlyingData(m_evaluationAutoValues.pointer());
  m_evaluationBooleanModel.setUnderlyingData(m_evaluationBooleanValues.pointer());
  m_evaluationFloatingModel.setUnderlyingData(m_evaluationFloatingValues.pointer());

  m_baselineAlgorithm = EvaluationParametersItems::ComboBaselineAlgorithm::SLOPE;
  m_showWindow = EvaluationParametersItems::ComboShowWindow::NONE;
  m_windowUnit = EvaluationParametersItems::ComboWindowUnits::MINUTES;

  m_resultsFloatingModel.setUnderlyingData(m_resultsNumericValues.pointer());

  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_EPSILON] = s_defaultHvlEpsilon;
  m_hvlFitModel.setUnderlyingData(m_hvlFitValues.pointer());

  m_hvlFixedModel.setUnderlyingData(m_hvlFitFixedValues.pointer());

  m_hvlFitIntValues[HVLFitParametersItems::Int::ITERATIONS] = s_defaultHvlIterations;
  m_hvlFitIntValues[HVLFitParametersItems::Int::DIGITS] = s_defaultHvlDigits;
  m_hvlFitIntModel.setUnderlyingData(m_hvlFitIntValues.pointer());

  m_hvlFitOptionsValues[HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT] = false;
  m_hvlFitOptionsValues[HVLFitOptionsItems::Boolean::SHOW_FIT_STATS] = false;
  m_hvlFitOptionsModel.setUnderlyingData(m_hvlFitOptionsValues.pointer());

  connect(m_dataFileLoader, &DataFileLoader::dataLoaded, this, &EvaluationEngine::onDataLoaded);

  connect(&m_hvlFitModel, &FloatingMapperModel<HVLFitResultsItems::Floating>::dataChanged, this, &EvaluationEngine::onHvlResultsModelChanged);
  connect(&m_hvlFitIntModel, &IntegerMapperModel<HVLFitParametersItems::Int>::dataChanged, this, &EvaluationEngine::onHvlParametersModelChanged);
  connect(&m_hvlFixedModel, &BooleanMapperModel<HVLFitParametersItems::Boolean>::dataChanged, this, &EvaluationEngine::onHvlParametersModelChanged);

  m_addPeakDlg = new AddPeakDialog();

  m_currentPeakIdx = 0;
  m_allPeaks.push_back(StoredPeak("", freshPeakContext()));
  m_currentDataContext = std::shared_ptr<DataContext>(new DataContext(nullptr, s_emptyCtxKey, m_commonParamsEngine->currentContext(),
                                                                      currentEvaluationContext()));
  m_currentDataContextKey = s_emptyCtxKey;

  connect(this, &EvaluationEngine::updateTEof, m_commonParamsEngine, &CommonParametersEngine::onUpdateTEof);
  connect(m_commonParamsEngine, &CommonParametersEngine::parametersUpdated, this, &EvaluationEngine::onCommonParametersChanged);
}

EvaluationEngine::~EvaluationEngine()
{
  delete m_addPeakDlg;
  delete m_findPeakMenu;
  delete m_manualIntegrationMenu;
  delete m_postProcessMenu;
  delete m_dataExporterBackendsModel;
  delete m_dataExporterFileDlg;
  delete m_specifyPeakBoundsDlg;
  delete m_textDataExporterCfgDlg;
  delete m_ctcEofScheme;
  delete m_ctcHvlScheme;
  delete m_ctcPeakScheme;
  delete m_ctcPeakDimsScheme;
  delete m_ctcEofSchemeBase;
  delete m_ctcHvlSchemeBase;
  delete m_ctcPeakSchemeBase;
  delete m_ctcPeakDimsSchemeBase;
  delete m_ctcDataArrangementModel;
}

void EvaluationEngine::activateCurrentDataContext()
{
  disconnect(m_commonParamsEngine, &CommonParametersEngine::parametersUpdated, this, &EvaluationEngine::onCommonParametersChanged);
  m_commonParamsEngine->setContext(m_currentDataContext->commonContext);
  setEvaluationContext(m_currentDataContext->evaluationContext);
  m_evaluatedPeaksModel.setEntries(makeEvaluatedPeaks());
  m_commonParamsEngine->revalidate();
  connect(m_commonParamsEngine, &CommonParametersEngine::parametersUpdated, this, &EvaluationEngine::onCommonParametersChanged);

  if (m_currentDataContext->data != nullptr) {
    if (m_currentDataContext->data->data.length() > 0)
      setPeakFinderParameters(m_currentDataContext->data->data.last().x());
  }

  drawEofMarker();
  m_plotCtx->scaleToFit();
}

void EvaluationEngine::addPeakToList(const PeakContext &ctx , const QString &name, const bool registerInHF, const RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom)
{
  /* Peak has no meaningful evaluation resutls,
   * do not add it */
  if (!ctx.finderResults->isValid()) {
    QMessageBox::information(nullptr, tr("Invalid peak"), tr("Peak has no meaningful evaluation results and will not be added"));
    return;
  }

  if (name.trimmed().length() < 1) {
    QMessageBox::information(nullptr, tr("Invalid peak"), tr("Name of the analyte must be specified"));
    return;
  }

  try {
    m_allPeaks.push_back(StoredPeak(name, ctx));
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add peak"));
    return;
  }

  const EvaluatedPeaksModel::EvaluatedPeak evpeak(name.trimmed(),
                                                  ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                                  ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  if (!m_evaluatedPeaksModel.appendEntry(evpeak)) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add peak to GUI list"));
    return;
  }

  if (registerInHF) {
    double mobility;

    switch (mobilityFrom) {
    case RegisterInHyperbolaFitWidget::MobilityFrom::HVL_A1:
      mobility = ctx.hvlValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1);
      break;
    case RegisterInHyperbolaFitWidget::MobilityFrom::PEAK_MAXIMUM:
      mobility = ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF);
      break;
    default:
      mobility = std::numeric_limits<double>::infinity();
      break;
    }

    emit registerMeasurement(name, m_commonParamsEngine->numValue(CommonParametersItems::Floating::SELECTOR),
                             mobility);
  }

  m_currentPeakIdx = m_allPeaks.length() - 1;
}

void EvaluationEngine::announceDefaultState()
{
  for (int idx = 0; idx < m_dataExporterBackendsModel->rowCount(); idx++) {
    const QModelIndex midx = m_dataExporterBackendsModel->index(idx, 0);
    const QVariant &v = m_dataExporterBackendsModel->data(midx, Qt::UserRole);

    if (m_currentDataExporterBackend == v.value<DataExporterBackends>())
      emit exporterBackendSet(midx);
  }

  for (int idx = 0; idx < m_ctcDataArrangementModel->rowCount(); idx++) {
    const QModelIndex midx = m_ctcDataArrangementModel->index(idx, 0);
    const QVariant &v = m_ctcDataArrangementModel->data(midx, Qt::UserRole);

    if (m_ctcDataArrangement == v.value<DataExporter::Globals::DataArrangement>())
      emit clipboardExporterDataArrangementSet(midx);
  }

  emit clipboardExporterDelimiterSet(m_ctcDelimiter);
}

void EvaluationEngine::assignContext(std::shared_ptr<PlotContextLimited> ctx)
{
  m_plotCtx = ctx;

  if (!m_plotCtx->addSerie(seriesIndex(Series::BASELINE), s_serieBaselineTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::red, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::PEAK_HEIGHT), s_seriePeakHeightTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::blue, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_seriePeakHeightTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::PEAK_TIME), s_seriePeakTimeTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::blue, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_seriePeakTimeTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::SIG), s_serieSignalTitle))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieSignalTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::EOF_MARK), s_serieEofTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::gray, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieEofTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::FINDER_SYSTEM_A), s_serieFinderSystemATitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::green, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieFinderSystemATitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::FINDER_SYSTEM_B), s_serieFinderSystemBTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(0,255,255), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieFinderSystemBTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::HVL), s_serieHVLTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(37,102,222), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieHVLTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::PROV_PEAK), s_serieProvisionalPeakTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::red, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieProvisionalPeakTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::BASELINE_FROM), s_serieBaselineFromTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(132, 172, 172), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineFromTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::BASELINE_TO), s_serieBaselineFromTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(132, 172, 172), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineToTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::PROV_BASELINE), s_serieProvisionalBaseline, SerieProperties::VisualStyle(QPen(QBrush(QColor(23, 73, 255), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieProvisionalBaseline));

  connect(m_plotCtx.get(), &PlotContextLimited::pointHovered, this, &EvaluationEngine::onPlotPointHovered);
  connect(m_plotCtx.get(), &PlotContextLimited::pointSelected, this, &EvaluationEngine::onPlotPointSelected);

  /* Default series titles */
  m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "");
  m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, "");

  /* Default axis fonts */
  m_plotCtx->setAxisFont(SerieProperties::Axis::X_BOTTOM, QFont());
  m_plotCtx->setAxisFont(SerieProperties::Axis::Y_LEFT, QFont());
}

AbstractMapperModel<bool, EvaluationParametersItems::Auto> *EvaluationEngine::autoValuesModel()
{
  return &m_evaluationAutoModel;
}

QAbstractItemModel *EvaluationEngine::baselineModel()
{
  return &m_baselineAlgorithmModel;
}

void EvaluationEngine::beginManualIntegration(const QPointF &from, const bool snap)
{
  if (!isContextValid())
    return;

  m_manualPeakSnapFrom = snap;

  if (snap) {
     try {
      m_manualPeakFrom = QPointF(from.x(), Helpers::yForX(from.x(), m_currentDataContext->data->data));
    } catch (std::out_of_range &) {
      return;
    }
  } else
    m_manualPeakFrom = from;

  m_userInteractionState = UserInteractionState::MANUAL_PEAK_INTEGRATION;
}

AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *EvaluationEngine::booleanValuesModel()
{
  return &m_evaluationBooleanModel;
}

double EvaluationEngine::calculateA1Mobility(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                             const CommonParametersEngine::Context &commonCtx)
{
  const bool noEof = commonCtx.boolData.at(CommonParametersItems::Boolean::NO_EOF);
  const double tP = hvlValues.at(HVLFitResultsItems::Floating::HVL_A1) * 60.0;
  const double tEOF = commonCtx.numData.at(CommonParametersItems::Floating::T_EOF) * 60.0;
  const double detector = commonCtx.numData.at(CommonParametersItems::Floating::DETECTOR) * 1.0e-2 ;
  const double E = commonCtx.numData.at(CommonParametersItems::Floating::FIELD) * 1.0e+3;

  double vP;
  double vP_Eff;
  double vEOF;

  if (Helpers::isSensible(tP))
    vP = detector / tP;
  else
    return std::numeric_limits<double>::infinity();

  if (noEof)
    vEOF = 0.0;
  else {
    if (Helpers::isSensible(tEOF))
      vEOF = detector / tEOF;
    else
      return std::numeric_limits<double>::infinity();
  }

  vP_Eff = vP - vEOF;

  if (Helpers::isSensible(std::abs(E)))
    return (vP_Eff / E) / 1.0e-9;
  else
    return std::numeric_limits<double>::infinity();
}

void EvaluationEngine::clearPeakPlots()
{
  m_plotCtx->clearSerieSamples(seriesIndex(Series::BASELINE));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::PEAK_HEIGHT));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::PEAK_TIME));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FINDER_SYSTEM_A));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FINDER_SYSTEM_B));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::HVL));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::BASELINE_FROM));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::BASELINE_TO));

  m_plotCtx->replot();
}

QAbstractItemModel *EvaluationEngine::clipboardDataArrangementModel()
{
  return m_ctcDataArrangementModel;
}

void EvaluationEngine::createContextMenus() noexcept(false)
{
  QAction *a;

  m_findPeakMenu = new QMenu;
  m_manualIntegrationMenu = new QMenu;
  m_postProcessMenu = new QMenu;

  /* Create Find peak menu */

  a = new QAction(tr("Peak from here"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::PEAK_FROM_HERE));
  m_findPeakMenu->addAction(a);

  a = new QAction(tr("Peak from here (snap to signal)"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::PEAK_FROM_HERE_SIGSNAP));
  m_findPeakMenu->addAction(a);

  m_findPeakMenu->addSeparator();

  a = new QAction(tr("Specify precise peak boundaries"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SPECIFY_PEAK_BOUNDARIES));
  m_findPeakMenu->addAction(a);

  m_findPeakMenu->addSeparator();

  a = new QAction(tr("Set noise reference point"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::NOISE_REF_POINT));
  m_findPeakMenu->addAction(a);

  a = new QAction(tr("Set slope reference point"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SLOPE_REF_POINT));
  m_findPeakMenu->addAction(a);

  m_findPeakMenu->addSeparator();

  a = new QAction(tr("Set EOF time"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SET_EOF_TIME));
  m_findPeakMenu->addAction(a);

  m_findPeakMenu->addSeparator();

  a = new QAction(tr("Set axis titles"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SET_AXIS_TITLES));
  m_findPeakMenu->addAction(a);

  a = new QAction(tr("Scale plot axes to fit"), m_postProcessMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SCALE_PLOT_TO_FIT));
  m_findPeakMenu->addAction(a);

  /* Create Manual integration menu */

  a = new QAction(tr("Peak to here"), m_manualIntegrationMenu);
  a->setData(QVariant::fromValue<ManualIntegrationMenuActions>(ManualIntegrationMenuActions::FINISH));
  m_manualIntegrationMenu->addAction(a);

  a = new QAction(tr("Peak to here (snap to signal)"), m_manualIntegrationMenu);
  a->setData(QVariant::fromValue<ManualIntegrationMenuActions>(ManualIntegrationMenuActions::FINISH_SIGSNAP));
  m_manualIntegrationMenu->addAction(a);

  m_manualIntegrationMenu->addSeparator();

  a = new QAction(tr("Cancel"), m_manualIntegrationMenu);
  a->setData(QVariant::fromValue<ManualIntegrationMenuActions>(ManualIntegrationMenuActions::CANCEL));
  m_manualIntegrationMenu->addAction(a);

  /* Create Post process menu */

  a = new QAction(tr("New peak from here"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::NEW_PEAK_FROM));
  m_postProcessMenu->addAction(a);

  a = new QAction(tr("New peak from here (snap to signal)"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::NEW_PEAK_FROM_SIGSNAP));
  m_postProcessMenu->addAction(a);

  m_postProcessMenu->addSeparator();

  a = new QAction(tr("New peak with specific boundaries"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::SPECIFY_PEAK_BOUNDARIES));
  m_postProcessMenu->addAction(a);

  m_postProcessMenu->addSeparator();

  a = new QAction(tr("Move peak beginning here"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::MOVE_PEAK_FROM));
  m_postProcessMenu->addAction(a);

  a = new QAction(tr("Move peak beginning here (snap to signal)"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::MOVE_PEAK_FROM_SIGSNAP));
  m_postProcessMenu->addAction(a);

  a = new QAction(tr("Move peak end here"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::MOVE_PEAK_TO));
  m_postProcessMenu->addAction(a);

  a = new QAction(tr("Move peak end here (snap to signal)"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::MOVE_PEAK_TO_SIGSNAP));
  m_postProcessMenu->addAction(a);

  m_postProcessMenu->addSeparator();

  a = new QAction(tr("Deselect peak"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::DESELECT_PEAK));
  m_postProcessMenu->addAction(a);

  m_postProcessMenu->addSeparator();

  a = new QAction(tr("Set EOF time"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::SET_EOF_TIME));
  m_postProcessMenu->addAction(a);

  m_postProcessMenu->addSeparator();

  a = new QAction(tr("Set axis titles"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::SET_AXIS_TITLES));
  m_postProcessMenu->addAction(a);

  a = new QAction(tr("Scale plot axes to fit"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::SCALE_PLOT_TO_FIT));
  m_postProcessMenu->addAction(a);
}

bool EvaluationEngine::createSignalPlot(std::shared_ptr<DataFileLoader::Data> data, const QString &name)
{
  m_plotCtx->setPlotTitle(name);

  if (data == nullptr) {
    m_plotCtx->clearAllSerieSamples();
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "");
    m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, "");
  } else{
    m_plotCtx->setSerieSamples(seriesIndex(Series::SIG), data->data);
    setAxisTitles();
  }

  m_plotCtx->scaleToFit();

  return true;
}

EvaluationEngine::EvaluationContext EvaluationEngine::currentEvaluationContext() const
{
  /* No peak has been added but the evaluation parameters might still have changed.
   * Make sure we store them all */
  QVector<StoredPeak> allPeaks(m_allPeaks);
  if (m_currentPeakIdx == 0) {
    try {
      allPeaks[0] = StoredPeak("", duplicatePeakContext());
    } catch (std::bad_alloc &) {
      QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Insufficient memory to execute EvaluationEngine::duplicatePeakContext(). Application may misbehave."));
    }
  }

  return EvaluationContext(allPeaks, m_currentPeakIdx);
}

QVector<bool> EvaluationEngine::defaultHvlFixedValues() const
{
  QVector<bool> def;
  def.resize(m_hvlFixedModel.indexFromItem(HVLFitParametersItems::Boolean::LAST_INDEX));

  for (int idx = 0; idx < m_hvlFixedModel.indexFromItem(HVLFitParametersItems::Boolean::LAST_INDEX); idx++)
    def[idx] = false;

  return def;
}

QVector<int> EvaluationEngine::defaultHvlIntValues() const
{
  QVector<int> def;
  def.resize(m_hvlFitIntModel.indexFromItem(HVLFitParametersItems::Int::LAST_INDEX));

  def[m_hvlFitIntModel.indexFromItem(HVLFitParametersItems::Int::DIGITS)] = s_defaultHvlDigits;
  def[m_hvlFitIntModel.indexFromItem(HVLFitParametersItems::Int::ITERATIONS)] = s_defaultHvlIterations;

  return def;
}

MappedVectorWrapper<double, HVLFitResultsItems::Floating> EvaluationEngine::doHvlFit(const std::shared_ptr<PeakFinderResults::Result> &finderResults,
                                                                                     const double estA0, const double estA1, const double estA2, const double estA3,
                                                                                     const bool fixA0, const bool fixA1, const bool fixA2, const bool fixA3,
                                                                                     const double epsilon,
                                                                                     const int iterations, const int digits,
                                                                                     const double tUsp,
                                                                                     bool *ok)
{
  MappedVectorWrapper<double, HVLFitResultsItems::Floating> results;

  results[HVLFitResultsItems::Floating::HVL_EPSILON] = s_defaultHvlEpsilon;

  if (!isContextValid()) {
    *ok = false;
    return results;
  }

  /* Peak has no meaningful evaluation results,
   * do not process it */
  if (!finderResults->isValid()) {
    *ok = false;
    return results;
  }


  HVLCalculator::HVLParameters p = HVLCalculator::fit(
    m_currentDataContext->data->data,
    finderResults->fromIndex, finderResults->toIndex,
    estA0,
    estA1,
    estA2,
    estA3,
    fixA0,
    fixA1,
    fixA2,
    fixA3,
    m_currentPeak.baselineIntercept,
    m_currentPeak.baselineSlope,
    epsilon,
    iterations,
    digits,
    m_hvlFitOptionsValues.at(HVLFitOptionsItems::Boolean::SHOW_FIT_STATS)
  );

  if (!p.isValid()) {
    *ok = false;
    return results;
  }

  results[HVLFitResultsItems::Floating::HVL_A0] = p.a0;
  results[HVLFitResultsItems::Floating::HVL_A1] = p.a1;
  results[HVLFitResultsItems::Floating::HVL_A2] = p.a2;
  results[HVLFitResultsItems::Floating::HVL_A3] = p.a3;
  results[HVLFitResultsItems::Floating::HVL_S] = p.s;
  results[HVLFitResultsItems::Floating::HVL_EPSILON] = epsilon;
  results[HVLFitResultsItems::Floating::HVL_TUSP] = tUsp;
  results[HVLFitResultsItems::Floating::HVL_U_EFF_A1] = calculateA1Mobility(results, m_commonParamsEngine->currentContext());


  *ok = true;
  return results;
}

void EvaluationEngine::drawEofMarker()
{
  if (!isContextValid()) {
    m_plotCtx->clearSerieSamples(seriesIndex(Series::EOF_MARK));

    m_plotCtx->replot();
    return;
  }

  /* Mark the EOF  */
  {
    if (m_commonParamsEngine->boolValue(CommonParametersItems::Boolean::NO_EOF)) {
      m_plotCtx->clearSerieSamples(seriesIndex(Series::EOF_MARK));
      m_plotCtx->replot();
      return;
    }

    double tEOF = m_commonParamsEngine->numValue(CommonParametersItems::Floating::T_EOF);

    if (tEOF > 0.0) {
      const double minY = Helpers::minYValue(m_currentDataContext->data->data);
      const double maxY = Helpers::maxYValue(m_currentDataContext->data->data);
      QVector<QPointF> vec;

      vec.push_back(QPointF(tEOF, minY));
      vec.push_back(QPointF(tEOF, maxY));
      m_plotCtx->setSerieSamples(seriesIndex(Series::EOF_MARK), vec);
    } else
      m_plotCtx->clearSerieSamples(seriesIndex(Series::EOF_MARK));

    m_plotCtx->replot();
  }

}

void EvaluationEngine::displayCurrentPeak()
{
  clearPeakPlots();

  plotEvaluatedPeak(m_currentPeak.finderResults,
                    m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                    m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT),
                    m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT),
                    m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT),
                    m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));
  setEvaluationResults(m_currentPeak);
  displayAssistedFinderData(m_currentPeak.afContext);
}

EvaluationEngine::PeakContext EvaluationEngine::duplicatePeakContext() const noexcept(false)
{
  /* Despite its name this function only copies the parameters of current
   * peak context, the results are created empty */
  return PeakContext(MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     m_hvlFitIntValues,
                     MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>(defaultHvlFixedValues()),
                     AssistedFinderContext(m_evaluationAutoValues, m_evaluationBooleanValues, m_evaluationFloatingValues,
                                           m_baselineAlgorithm, m_showWindow, m_windowUnit),
                     std::make_shared<PeakFinderResults::Result>(),
                     0.0, 0.0, QVector<QPointF>());
}

void EvaluationEngine::displayAssistedFinderData(const AssistedFinderContext &afContext)
{
  m_evaluationAutoValues = afContext.afAutoValues;
  m_evaluationBooleanValues = afContext.afBoolValues;
  m_evaluationFloatingValues = afContext.afFloatingValues;
  m_baselineAlgorithm = afContext.baselineAlgorithm;
  m_showWindow = afContext.showWindow;
  m_windowUnit = afContext.windowUnits;

  m_evaluationAutoModel.notifyAllDataChanged();
  m_evaluationBooleanModel.notifyAllDataChanged();
  m_evaluationFloatingModel.notifyAllDataChanged({ Qt::EditRole });
}

QVector<double> EvaluationEngine::emptyHvlValues() const
{
  QVector<double> empty;
  empty.reserve(m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX));

  for (int idx = 0; idx < m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX); idx++)
      empty.push_back(0.0);

  empty[m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_EPSILON)] = s_defaultHvlEpsilon;

  return empty;
}

QVector<double> EvaluationEngine::emptyResultsValues() const
{
  QVector<double> empty;
  empty.reserve(m_resultsFloatingModel.indexFromItem(EvaluationResultsItems::Floating::LAST_INDEX));

  for (int idx = 0; idx < m_resultsFloatingModel.indexFromItem(EvaluationResultsItems::Floating::LAST_INDEX); idx++)
    empty.push_back(0.0);

  return empty;
}

QAbstractItemModel *EvaluationEngine::evaluatedPeaksModel()
{
  return &m_evaluatedPeaksModel;
}

QAbstractItemModel *EvaluationEngine::exporterBackendsModel()
{
  return m_dataExporterBackendsModel;
}

QAbstractItemModel *EvaluationEngine::exporterSchemesModel()
{
  return m_dataExporter.schemesModel();
}

void EvaluationEngine::findPeakAssisted()
{
  SelectPeakDialog dialog;
  std::shared_ptr<PeakFinderResults> fr;

  if (!isContextValid())
    return;

  switch (m_userInteractionState) {
  case UserInteractionState::PEAK_POSTPROCESSING:
    onCancelEvaluatedPeakSelection();
    break;
  case UserInteractionState::MANUAL_PEAK_INTEGRATION:
    return;
  case UserInteractionState::FINDING_PEAK:
    break;
  }

  if (m_currentDataContext->data->data.length() == 0)
    return;

  AssistedPeakFinder::Parameters fp = makeFinderParameters();
  fp.selPeakDialog = &dialog;

  connect(&dialog, &SelectPeakDialog::listClicked, this, &EvaluationEngine::onProvisionalPeakSelected);
  connect(&dialog, &SelectPeakDialog::closedSignal, this, &EvaluationEngine::onUnhighlightProvisionalPeak);

  try {
    fr = AssistedPeakFinder::find(fp);
  } catch (std::bad_alloc &) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Not enough memory to find peaks"));
    return;
  }

  walkFoundPeaks(fr->results, AssistedFinderContext(m_evaluationAutoValues, m_evaluationBooleanValues, m_evaluationFloatingValues,
                                                    m_baselineAlgorithm, m_showWindow, m_windowUnit));
  displayCurrentPeak();
}

void EvaluationEngine::findPeakManually(const QPointF &from, const QPointF &to, const bool snapFrom, const bool snapTo, const bool updatePeak)
{
  std::shared_ptr<PeakFinderResults> fr;

  /* Erase the provisional baseline */
  m_plotCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), QVector<QPointF>());

  if (!isContextValid()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_plotCtx->replot();
    return;
  }

  if (m_currentDataContext->data->data.length() == 0) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_plotCtx->replot();
    return;
  }

  ManualPeakFinder::Parameters p(m_currentDataContext->data->data);
  p.fromX = from.x();
  if (snapFrom) {
    try {
      p.fromY = Helpers::yForX(from.x(), m_currentDataContext->data->data);
    } catch (std::out_of_range &) {
      QMessageBox::warning(nullptr,tr("Invalid value"), tr("Invalid value of \"from X\""));
      return;
    }
  } else
    p.fromY = from.y();

  p.toX = to.x();
  if (snapTo) {
    try {
      p.toY = Helpers::yForX(to.x(), m_currentDataContext->data->data);
    } catch (std::out_of_range &) {
      QMessageBox::warning(nullptr, tr("Invalid value"), tr("Invalid value of \"to X\""));
      return;
    }
  } else
    p.toY = to.y();

  try {
    fr = ManualPeakFinder::find(p);
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Not enough memory to evaluate peak"));
    goto err_out;
  }

  if (fr->results.size() == 0)
    goto err_out;

  walkFoundPeaks(fr->results, AssistedFinderContext(), updatePeak);
  displayCurrentPeak();
  return;

err_out:
  m_userInteractionState = UserInteractionState::FINDING_PEAK;
  m_plotCtx->replot();
}

void EvaluationEngine::findPeakMenuTriggered(const FindPeakMenuActions &action, const QPointF &point)
{
  QModelIndex autoFrom;
  QModelIndex autoTo;
  QModelIndex valueFrom;
  QModelIndex valueTo;

  switch (action) {
  case FindPeakMenuActions::PEAK_FROM_HERE:
    beginManualIntegration(point, false);
    break;
  case FindPeakMenuActions::PEAK_FROM_HERE_SIGSNAP:
    beginManualIntegration(point, true);
    break;
  case FindPeakMenuActions::SPECIFY_PEAK_BOUNDARIES:
    findPeakPreciseBoundaries();
    break;
  case FindPeakMenuActions::NOISE_REF_POINT:
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE_REF_POINT] = point.x();
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::NOISE_REF_POINT));
    valueTo = valueFrom;
    break;
  case FindPeakMenuActions::SLOPE_REF_POINT:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::SLOPE_REF_POINT] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_REF_POINT] = point.x();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_REF_POINT));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::SLOPE_REF_POINT));
    valueTo = valueFrom;
    break;
  case FindPeakMenuActions::SET_AXIS_TITLES:
    showSetAxisTitlesDialog();
    break;
  case FindPeakMenuActions::SET_EOF_TIME:
    setEofTime(point);
    break;
  case FindPeakMenuActions::SCALE_PLOT_TO_FIT:
    m_plotCtx->scaleToFit();
    return;
    break;
  }

  if (autoFrom.isValid())
    emit m_evaluationAutoModel.dataChanged(autoFrom, autoTo, { Qt::EditRole });
  if (valueFrom.isValid())
    emit m_evaluationFloatingModel.dataChanged(valueFrom, valueTo, { Qt::EditRole });
}

void EvaluationEngine::findPeakPreciseBoundaries()
{
  std::shared_ptr<PeakFinderResults> fr;

  if (!isContextValid())
    return;

  if (m_currentDataContext->data->data.length() == 0)
    return;

  if (m_userInteractionState != UserInteractionState::FINDING_PEAK)
    onCancelEvaluatedPeakSelection();

  while (m_specifyPeakBoundsDlg->exec() != QDialog::Rejected) {
    SpecifyPeakBoundariesDialog::Answer answer = m_specifyPeakBoundsDlg->answer();

    if (answer.fromX < 0.0 || answer.fromX > m_currentDataContext->data->data.back().x()) {
      QMessageBox::warning(nullptr, tr("Invalid input"), tr("Value of \"Peak from X\" is out of bounds"));
      continue;
    }
    if (answer.toX < 0.0 || answer.toX > m_currentDataContext->data->data.back().x()) {
      QMessageBox::warning(nullptr, tr("Invalid input"), tr("Value of \"Peak to X\" is out of bounds"));
      continue;
    }

    ManualPeakFinder::Parameters p(m_currentDataContext->data->data);

    if (answer.snapFrom)
      p.fromY = Helpers::yForX(answer.fromX, m_currentDataContext->data->data);
    else
      p.fromY = answer.fromY;

    if (answer.snapTo)
      p.toY = Helpers::yForX(answer.toX, m_currentDataContext->data->data);
    else
      p.toY = answer.toY;

    p.fromX = answer.fromX;
    p.toX = answer.toX;

    try {
      fr = ManualPeakFinder::find(p);
    } catch (std::bad_alloc &) {
      QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to find peak"));
      return;
    }

    if (fr->results.size() == 0)
      return;

    walkFoundPeaks(fr->results, AssistedFinderContext(), false);
    displayCurrentPeak();
    return;
  }
}

EvaluationEngine::EvaluationContext EvaluationEngine::freshEvaluationContext() const
{
  QVector<StoredPeak> fresh;

  try {
    fresh.push_back(StoredPeak("", duplicatePeakContext()));
  } catch (std::bad_alloc &) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Insufficient memory to execute EvaluationEngine::freshEvaluationContext(). Application cannon continue."));
    Helpers::execCFIT();
  }

  return EvaluationContext(fresh, 0);
}

EvaluationEngine::PeakContext EvaluationEngine::freshPeakContext() const noexcept(false)
{
  return PeakContext(MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     MappedVectorWrapper<int, HVLFitParametersItems::Int>(defaultHvlIntValues()),
                     MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>(defaultHvlFixedValues()),
                     AssistedFinderContext(),
                     std::make_shared<PeakFinderResults::Result>(),
                     0.0, 0.0, QVector<QPointF>());
}

void EvaluationEngine::fullViewUpdate()
{
  m_evaluationAutoModel.notifyAllDataChanged();
  m_evaluationBooleanModel.notifyAllDataChanged();
  m_evaluationFloatingModel.notifyAllDataChanged();
  m_resultsFloatingModel.notifyAllDataChanged();
  m_hvlFitModel.notifyAllDataChanged();
  m_hvlFixedModel.notifyAllDataChanged();
  m_hvlFitIntModel.notifyAllDataChanged();

  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::WINDOW_UNITS,
                                                                   EvaluationParametersItems::index(m_windowUnit)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::SHOW_WINDOW,
                                                                   EvaluationParametersItems::index(m_showWindow)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                                   EvaluationParametersItems::index(m_baselineAlgorithm)));
}

AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *EvaluationEngine::hvlFitFixedModel()
{
  return &m_hvlFixedModel;
}

AbstractMapperModel<int, HVLFitParametersItems::Int> *EvaluationEngine::hvlFitIntModel()
{
  return &m_hvlFitIntModel;
}

AbstractMapperModel<double, HVLFitResultsItems::Floating> *EvaluationEngine::hvlFitModel()
{
  return &m_hvlFitModel;
}

AbstractMapperModel<bool, HVLFitOptionsItems::Boolean> *EvaluationEngine::hvlFitOptionsModel()
{
  return &m_hvlFitOptionsModel;
}

bool EvaluationEngine::isContextValid() const
{
  return (m_currentDataContextKey.compare(s_emptyCtxKey) == 0) ? false : true;
}

QAbstractItemModel *EvaluationEngine::loadedFilesModel()
{
  return &m_loadedFilesModel;
}

void EvaluationEngine::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  StandardPlotContextSettingsHandler::loadUserSettings(settings, *m_plotCtx.get());

  EMT::StringVariantMap map = settings.value<EMT::StringVariantMap>();

  if (map.contains(DATAFILELOADER_SETTINGS_TAG)) {
    const QVariant &v = map[DATAFILELOADER_SETTINGS_TAG];

    m_dataFileLoader->loadUserSettings(v);
  }

  if (map.contains(HVLFITOPTIONS_DISABLE_AUTO_FIT_TAG)) {
    const QVariant &v = map[HVLFITOPTIONS_DISABLE_AUTO_FIT_TAG];

    if (v.canConvert<bool>()) {
      m_hvlFitOptionsValues[HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT] = v.toBool();

      m_hvlFitOptionsModel.notifyDataChanged(HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT, HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT, { Qt::EditRole });
    }
  }

  if (map.contains(HVLFITOPTIONS_SHOW_FIT_STATS_TAG)) {
    const QVariant &v = map[HVLFITOPTIONS_SHOW_FIT_STATS_TAG];

    if (v.canConvert<bool>()) {
      m_hvlFitOptionsValues[HVLFitOptionsItems::Boolean::SHOW_FIT_STATS] = v.toBool();

      m_hvlFitOptionsModel.notifyDataChanged(HVLFitOptionsItems::Boolean::SHOW_FIT_STATS, HVLFitOptionsItems::Boolean::SHOW_FIT_STATS, { Qt::EditRole });
   }
  }

  if (map.contains(CLIPBOARDEXPORTER_DATAARRANGEMENT_TAG)) {
    const QVariant &v = map[CLIPBOARDEXPORTER_DATAARRANGEMENT_TAG];

    if (v.canConvert<DataExporter::Globals::DataArrangement>()) {
      m_ctcDataArrangement = v.value<DataExporter::Globals::DataArrangement>();

      for (int idx = 0; idx < m_ctcDataArrangementModel->rowCount(); idx++) {
        const QModelIndex &midx = m_ctcDataArrangementModel->index(idx, 0);
        const QVariant &_v = m_ctcDataArrangementModel->data(midx, Qt::UserRole);

        if (_v.value<DataExporter::Globals::DataArrangement>() == m_ctcDataArrangement)
          emit clipboardExporterDataArrangementSet(midx);
      }
    }
  }

  if (map.contains(CLIPBOARDEXPORTER_DELIMTIER_TAG)) {
    const QVariant &v = map[CLIPBOARDEXPORTER_DELIMTIER_TAG];

    m_ctcDelimiter = v.toString();
    emit clipboardExporterDelimiterSet(m_ctcDelimiter);
  }
}

QVector<EvaluatedPeaksModel::EvaluatedPeak> EvaluationEngine::makeEvaluatedPeaks()
{
  QVector<EvaluatedPeaksModel::EvaluatedPeak> peaks;

  for (int idx = 1; idx < m_allPeaks.length(); idx++) {
    const PeakContext &ctx = m_allPeaks.at(idx).peak();
    peaks.push_back(EvaluatedPeaksModel::EvaluatedPeak(m_allPeaks.at(idx).name,
                                                       ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                                       ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA)));
  }

  return peaks;
}

PeakEvaluator::Parameters EvaluationEngine::makeEvaluatorParameters(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults::Result> &fr)
{
  PeakEvaluator::Parameters p(data);

  p.capillary = m_commonParamsEngine->numValue(CommonParametersItems::Floating::CAPILLARY);
  p.detector = m_commonParamsEngine->numValue(CommonParametersItems::Floating::DETECTOR);
  p.E = m_commonParamsEngine->numValue(CommonParametersItems::Floating::FIELD);
  p.fromIndex = fr->fromIndex;
  p.toIndex = fr->toIndex;
  p.tEOF = m_commonParamsEngine->numValue(CommonParametersItems::Floating::T_EOF);
  p.noEof = m_commonParamsEngine->boolValue(CommonParametersItems::Boolean::NO_EOF);
  p.fromX = fr->peakFromX;
  p.fromY = fr->peakFromY;
  p.toX = fr->peakToX;
  p.toY = fr->peakToY;
  p.voltage = m_commonParamsEngine->numValue(CommonParametersItems::Floating::VOLTAGE);

  return p;
}

AssistedPeakFinder::Parameters EvaluationEngine::makeFinderParameters()
{
  AssistedPeakFinder::Parameters p(m_currentDataContext->data->data);

  p.autoFrom = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::FROM);
  p.autoNoise = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::NOISE);
  p.autoSlopeRefPoint = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::SLOPE_REF_POINT);
  p.autoSlopeThreshold = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::SLOPE_THRESHOLD);
  p.autoSlopeWindow = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::SLOPE_WINDOW);
  p.autoTo = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::TO);

  p.baselineAlgorithm = m_baselineAlgorithm;
  p.from = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::FROM);
  p.noise = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::NOISE);
  p.noiseCoefficient = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::NOISE_LOD_COEFF);
  p.noiseCorrection = m_evaluationBooleanValues.at(EvaluationParametersItems::Boolean::NOISE_CORRECTION);
  p.noisePoint = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::NOISE_REF_POINT);
  p.noiseWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::NOISE_WINDOW);
  p.peakWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_WINDOW);
  p.showWindow = m_showWindow;
  p.slopeRefPoint = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_REF_POINT);
  p.slopeSensitivity = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_SENSITIVITY);
  p.slopeThreshold = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_THRESHOLD);
  p.slopeWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_WINDOW);
  p.to = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::TO);
  p.windowUnits = m_windowUnit;
  p.disturbanceDetection = m_evaluationBooleanValues.at(EvaluationParametersItems::Boolean::DISTURBANCE_DETECTION);
  p.disturbanceDetectionWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::DISTURBANCE_DETECTION);

  return p;
}

EvaluationEngine::PeakContext EvaluationEngine::makePeakContext(const PeakContextModels &models,
                                                                const AssistedFinderContext &afContext,
                                                                const std::shared_ptr<PeakFinderResults::Result> &fr,
                                                                const PeakEvaluator::Results &er,
                                                                const QVector<QPointF> &hvlPlot) const
{
  return PeakContext(models.resultsValues,
                     models.hvlValues, models.hvlFitIntValues, models.hvlFitFixedValues,
                     afContext,
                     fr,
                     er.baselineSlope, er.baselineIntercept,
                     hvlPlot);
}

EvaluationEngine::PeakContext EvaluationEngine::makePeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> resultsValues,
                                                                const MappedVectorWrapper<double, HVLFitResultsItems::Floating> hvlValues,
                                                                const MappedVectorWrapper<int, HVLFitParametersItems::Int> hvlFitIntValues,
                                                                const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> hvlFitFixedValues,
                                                                const PeakContext &oldPeak) const
{
  return PeakContext(resultsValues, hvlValues, hvlFitIntValues, hvlFitFixedValues,
                     oldPeak.afContext,
                     oldPeak.finderResults,
                     oldPeak.baselineSlope, oldPeak.baselineIntercept,
                     oldPeak.hvlPlot);
}

EvaluationEngine::PeakContextModels EvaluationEngine::makePeakContextModels(const std::shared_ptr<PeakFinderResults::Result> &fr, const PeakEvaluator::Results &er,
                                                                            const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlResults,
                                                                            const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                                                            const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitFixedValues) const
{
  MappedVectorWrapper<double, EvaluationResultsItems::Floating> resultsValues;

  resultsValues[EvaluationResultsItems::Floating::N_FULL] = er.nFull;
  resultsValues[EvaluationResultsItems::Floating::N_LEFT] = er.nLeft;
  resultsValues[EvaluationResultsItems::Floating::N_RIGHT] = er.nRight;
  resultsValues[EvaluationResultsItems::Floating::N_H_FULL] = er.nHFull;
  resultsValues[EvaluationResultsItems::Floating::N_H_LEFT] = er.nHLeft;
  resultsValues[EvaluationResultsItems::Floating::N_H_RIGHT] = er.nHRight;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MIN_FULL] = er.sigmaHalfFull;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MIN_LEFT] = er.sigmaHalfLeft;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MIN_RIGHT] = er.sigmaHalfRight;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MET_FULL] = er.sigmaHalfMFull;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MET_LEFT] = er.sigmaHalfMLeft;
  resultsValues[EvaluationResultsItems::Floating::SIGMA_MET_RIGHT] = er.sigmaHalfMRight;
  resultsValues[EvaluationResultsItems::Floating::EOF_MOBILITY] = er.uEOF;
  resultsValues[EvaluationResultsItems::Floating::PEAK_MOBILITY] = er.uP;
  resultsValues[EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF] = er.uP_Eff;
  resultsValues[EvaluationResultsItems::Floating::EOF_VELOCITY] = er.vEOF;
  resultsValues[EvaluationResultsItems::Floating::PEAK_VELOCITY] = er.vP;
  resultsValues[EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF] = er.vP_Eff;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_FULL] = er.widthHalfFull;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT] = er.widthHalfLeft;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT] = er.widthHalfRight;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_FULL] = er.widthHalfMFull;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_LEFT] = er.widthHalfMLeft;
  resultsValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_RIGHT] = er.widthHalfMRight;
  resultsValues[EvaluationResultsItems::Floating::PEAK_FROM_X] = fr->peakFromX;
  resultsValues[EvaluationResultsItems::Floating::PEAK_FROM_Y] = fr->peakFromY;
  resultsValues[EvaluationResultsItems::Floating::PEAK_TO_X] = fr->peakToX;
  resultsValues[EvaluationResultsItems::Floating::PEAK_TO_Y] = fr->peakToY;
  resultsValues[EvaluationResultsItems::Floating::PEAK_X] = er.peakX;
  resultsValues[EvaluationResultsItems::Floating::PEAK_HEIGHT] = er.peakHeight;
  resultsValues[EvaluationResultsItems::Floating::PEAK_HEIGHT_BL] = er.peakHeightBaseline;
  resultsValues[EvaluationResultsItems::Floating::PEAK_AREA] = er.peakArea;

  return PeakContextModels(resultsValues, hvlResults, hvlFitIntValues, hvlFitFixedValues);
}

void EvaluationEngine::manualIntegrationMenuTriggered(const ManualIntegrationMenuActions &action, const QPointF &point)
{
  switch (action) {
  case ManualIntegrationMenuActions::CANCEL:
    m_plotCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), QVector<QPointF>());
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_plotCtx->replot();
    break;
  case ManualIntegrationMenuActions::FINISH:
    findPeakManually(m_manualPeakFrom, point, m_manualPeakSnapFrom, false);
    break;
  case ManualIntegrationMenuActions::FINISH_SIGSNAP:
    findPeakManually(m_manualPeakFrom, point, m_manualPeakSnapFrom, true);
    break;
  }
}

AbstractMapperModel<double, EvaluationParametersItems::Floating> *EvaluationEngine::floatingValuesModel()
{
  return &m_evaluationFloatingModel;
}

void EvaluationEngine::onAddPeak()
{
  if (!isContextValid())
    return;

  m_addPeakDlg->setInformation(m_commonParamsEngine->numValue(CommonParametersItems::Floating::SELECTOR),
                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1),
                               m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  int dlgRet = m_addPeakDlg->exec();
  if (dlgRet != QDialog::Accepted)
    return;

  AddPeakDialog::Answer answer = m_addPeakDlg->answer();

  addPeakToList(m_currentPeak, answer.name, answer.registerInHF, answer.mobilityFrom);
}

void EvaluationEngine::onCancelEvaluatedPeakSelection()
{
  /* Store current peak */
  if (m_currentPeakIdx > 0)
    storeCurrentPeak();

  try {
    m_currentPeak = duplicatePeakContext();
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Insufficent memory"), tr("Insufficient memory to execute EvaluationEngine::duplicatePeakContext(). Application may misbehave."));
  }

  m_currentPeakIdx = 0;

  setPeakContext(m_currentPeak);

  m_userInteractionState = UserInteractionState::FINDING_PEAK;
}

void EvaluationEngine::onClipboardExporterDataArrangementChanged(const QModelIndex &idx)
{
  QVariant v = m_ctcDataArrangementModel->data(idx, Qt::UserRole);

  if (!v.canConvert<DataExporter::Globals::DataArrangement>())
    return;

  m_ctcDataArrangement = v.value<DataExporter::Globals::DataArrangement>();
}

void EvaluationEngine::onClipboardExporterDelimiterChanged(const QString &delimiter)
{
  m_ctcDelimiter = delimiter;
}

void EvaluationEngine::onCloseCurrentEvaluationFile(const int idx)
{
  int newIdx;
  const QString oldKey(m_currentDataContextKey);

  if (m_exportOnFileLeftEnabled)
    onExportScheme();

  if (m_allDataContexts.size() == 0)
    return;
  else if (m_allDataContexts.size() == 1) {
    m_loadedFilesModel.deleteByIdx(idx);
    m_currentDataContext = std::shared_ptr<DataContext>(new DataContext(nullptr, "", m_commonParamsEngine->currentContext(), freshEvaluationContext()));
    m_currentDataContextKey = "";
  } else {
    m_loadedFilesModel.deleteByIdx(idx);
    newIdx = (idx > 0) ? idx - 1 : 0;
    QString newKey = m_loadedFilesModel.data(m_loadedFilesModel.index(newIdx, 0), Qt::UserRole + 1).toString();

    if (!m_allDataContexts.contains(newKey)) {
      QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Data context for key %1 not found!")).arg(newKey));
      return;
    }

    m_currentDataContext = m_allDataContexts[newKey];
    m_currentDataContextKey = newKey;
  }

  m_allDataContexts.remove(oldKey);

  activateCurrentDataContext();
}

void EvaluationEngine::onComboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier notifier)
{
  switch (notifier.id) {
  case EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM:
    m_baselineAlgorithm = EvaluationParametersItems::fromIndex<EvaluationParametersItems::ComboBaselineAlgorithm>(notifier.value);
    break;
  case EvaluationEngineMsgs::ComboBox::SHOW_WINDOW:
    m_showWindow = EvaluationParametersItems::fromIndex<EvaluationParametersItems::ComboShowWindow>(notifier.value);
    break;
  case EvaluationEngineMsgs::ComboBox::WINDOW_UNITS:
    m_windowUnit = EvaluationParametersItems::fromIndex<EvaluationParametersItems::ComboWindowUnits>(notifier.value);
    switchWindowUnit(m_windowUnit);
    break;
  default:
    break;
  }
}

void EvaluationEngine::onCommonParametersChanged()
{
  if (!isContextValid())
    return;

  drawEofMarker();

  if (m_commonParamsEngine->numValue(CommonParametersItems::Floating::CAPILLARY) <= 0.0 ||
      m_commonParamsEngine->numValue(CommonParametersItems::Floating::DETECTOR) <= 0.0 ||
      m_commonParamsEngine->numValue(CommonParametersItems::Floating::VOLTAGE) == 0.0 ||
      (m_commonParamsEngine->numValue(CommonParametersItems::Floating::T_EOF) <= 0.0 && !m_commonParamsEngine->boolValue(CommonParametersItems::Boolean::NO_EOF)))
    return;

  /* Update all stored peaks */
  for (int idx = 1; idx < m_allPeaks.size(); idx++) {
    const PeakContext &ctx = m_allPeaks.at(idx).peak();
    PeakContext newCtx = processFoundPeak(m_currentDataContext->data->data, ctx.finderResults, ctx.afContext, true, false, ctx);

      m_allPeaks[idx].updatePeak(newCtx);
      m_evaluatedPeaksModel.updateEntry(idx - 1,
                                        newCtx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                        newCtx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  }

  if (m_currentPeak.finderResults->isValid()) {
    m_currentPeak = processFoundPeak(m_currentDataContext->data->data, m_currentPeak.finderResults, m_currentPeak.afContext, true, false, m_currentPeak);
    m_hvlFitValues[HVLFitResultsItems::Floating::HVL_U_EFF_A1] = calculateA1Mobility(m_currentPeak.hvlValues, m_commonParamsEngine->currentContext());

    m_resultsNumericValues = m_currentPeak.resultsValues;
    m_hvlFitModel.notifyDataChanged(HVLFitResultsItems::Floating::HVL_U_EFF_A1,
                                    HVLFitResultsItems::Floating::HVL_U_EFF_A1);
    m_resultsFloatingModel.notifyAllDataChanged();
  }
}

void EvaluationEngine::onConfigureExporterBackend()
{
  switch (m_currentDataExporterBackend) {
  case DataExporterBackends::HTML:
    return; /* This backend has no configuration options */
  case DataExporterBackends::TEXT:
  {
    bool canceled = false;

    while (true) {
      QString str = m_textDataExporterCfgDlg->interact(canceled, m_textDataExporterDelimiter);
      if (canceled)
        return;

      if (str == "\\t") {
        m_textDataExporterDelimiter = QChar('\t');
        return;
      } else {
        if (str.length() != 1) {
          QMessageBox::warning(nullptr, tr("Invalid input data"), tr("Delimiter must be a single character or '\\t' to represent TAB"));
          continue;
        }

        m_textDataExporterDelimiter = QChar(str.at(0));
        return;
      }
    }
  }
    return;
  }
}

void EvaluationEngine::onCopyToClipboard(const EvaluationEngineMsgs::CopyToClipboard ctc)
{
  QClipboard *clipboard = QApplication::clipboard();
  QString out;
  QTextStream toCopy(&out, QIODevice::WriteOnly);
  QChar delimiter;

  if (!isContextValid())
    return;

  if (m_ctcDelimiter == "\\t")
    delimiter = '\t';
  else if (m_ctcDelimiter.length() != 1) {
    QMessageBox::warning(nullptr, tr("Invalid exporter options"), tr("Delimiter must be a single character or '\\t' to represent TAB.\nPlease check clipboard exporter options on the Export tab."));
    return;
  } else
    delimiter = m_ctcDelimiter.at(0);

  DataExporter::TextStreamExporterBackend backend(&toCopy, delimiter, m_ctcDataArrangement);

  toCopy.setCodec("UTF-8");

  switch (ctc) {
  case EvaluationEngineMsgs::CopyToClipboard::EOFLOW:
    m_ctcEofScheme->exportData(this, backend);
    break;
  case EvaluationEngineMsgs::CopyToClipboard::HVL:
    m_ctcHvlScheme->exportData(this, backend);
    break;
  case EvaluationEngineMsgs::CopyToClipboard::PEAK:
    m_ctcPeakScheme->exportData(this, backend);
    break;
  case EvaluationEngineMsgs::CopyToClipboard::PEAK_DIMS:
    m_ctcPeakDimsScheme->exportData(this, backend);
    break;
  case EvaluationEngineMsgs::CopyToClipboard::EVERYTHING:
    toCopy << "EOF\n";
    m_ctcEofScheme->exportData(this, backend);
    backend.clear();

    toCopy << "\nPeak parameters\n";
    m_ctcPeakScheme->exportData(this, backend);
    backend.clear();

    toCopy << "\nHVL fit results\n";
    m_ctcHvlScheme->exportData(this, backend);
    backend.clear();

    toCopy << "\nPeak dimensions\n";
    m_ctcPeakDimsScheme->exportData(this, backend);
    break;
  }

  clipboard->setText(out);
}

void EvaluationEngine::onDataLoaded(std::shared_ptr<DataFileLoader::Data> data, QString fileID, QString fileName)
{
  if (m_exportOnFileLeftEnabled)
    onExportScheme();

  /* Empty file ID, file is coming from a temporary storage such as clipboard */
  if (fileID.compare("") == 0) {
    const QStringList &keys = m_allDataContexts.keys();
    int keyIdx;
    int num = 0;

    for (keyIdx = 0; keyIdx < keys.size(); keyIdx++) {
      const QString &key = keys.at(keyIdx);

      if (key.startsWith("temporary")) {
        int idx = key.lastIndexOf("_");

        if (idx < 0)
          continue;

        bool ok;
        QString numStr = key.mid(idx + 1);
        int inum = numStr.toInt(&ok);

        if (!ok)
          continue;

        if (inum > num)
          num = inum;
      }
    }

    fileID = QString("temporary_") + QString::number(num + 1);
    fileName = fileID;
  }

  if (m_allDataContexts.contains(fileID)) {

    QMessageBox::warning(nullptr, tr("Data exist"), tr("This file is already loaded"));
    int idx = m_loadedFilesModel.indexByItem(fileID);
    if (idx >= 0) {
      emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::DATA_FILES, idx));
      switchEvaluationContext(fileID);
    }
    return;
  }

  storeCurrentContext();

  std::shared_ptr<DataContext> ctx = std::shared_ptr<DataContext>(new DataContext(data, fileName, m_commonParamsEngine->currentContext(),
                                                                                  freshEvaluationContext()));

  try {
    m_allDataContexts.insert(fileID, ctx);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Data processing error"), tr("Unable to store data in database"));
    return;
  }

  if (!m_loadedFilesModel.appendEntry(ComboBoxItem<QString>(fileID, fileID)))
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Unable to add new entry to the list of loaded files"));
  else
    emit evaluationFileAdded(m_loadedFilesModel.rowCount() - 1);

  m_currentDataContext = ctx;
  m_currentDataContextKey = fileID;

  activateCurrentDataContext();
}

void EvaluationEngine::onDeletePeak(const QModelIndex &idx)
{
  Q_UNUSED(idx);

  if (m_currentPeakIdx == 0)
    return;

  m_allPeaks.remove(m_currentPeakIdx);
  m_evaluatedPeaksModel.removeEntry(m_currentPeakIdx - 1);

  m_currentPeakIdx--;
  m_currentPeak = m_allPeaks.at(m_currentPeakIdx).peak();

  setPeakContext(m_currentPeak);
}

void EvaluationEngine::onDoHvlFit()
{
  bool ok;

  if (!isContextValid())
    return;

  MappedVectorWrapper<double, HVLFitResultsItems::Floating> results = doHvlFit(m_currentPeak.finderResults,
                                                                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0),
                                                                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1),
                                                                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2),
                                                                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3),
                                                                               m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A0),
                                                                               m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A1),
                                                                               m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A2),
                                                                               m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A3),
                                                                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_EPSILON),
                                                                               m_hvlFitIntValues.at(HVLFitParametersItems::Int::ITERATIONS),
                                                                               m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS),
                                                                               m_currentPeak.hvlValues.at(HVLFitResultsItems::Floating::HVL_TUSP),
                                                                               &ok);
  if (ok) {
    m_hvlFitValues = results;
    m_hvlFitModel.notifyAllDataChanged();
    onReplotHvl();
  }
}

void EvaluationEngine::onEvaluationFileSwitched(const int idx)
{
  QVariant var;
  QString key;

  if (m_allDataContexts.size() == 0)
    return;

  var = m_loadedFilesModel.data(m_loadedFilesModel.index(idx, 0), Qt::UserRole + 1);
  if (!var.isValid())
    return;
  key = var.toString();

  switchEvaluationContext(key);
}

void EvaluationEngine::onExporterBackendChanged(const QModelIndex &idx)
{
  const QVariant v = m_dataExporterBackendsModel->data(idx, Qt::UserRole);

  if (!v.canConvert<DataExporterBackends>())
    return;

  m_currentDataExporterBackend = v.value<DataExporterBackends>();
}

void EvaluationEngine::onExporterSchemeChanged(const QModelIndex &idx)
{
  const QVariant v = m_dataExporter.schemesModel()->data(idx, Qt::UserRole);

  m_currentDataExporterSchemeId = v.toString();
}

void EvaluationEngine::onExportFileOnLeftToggled(const bool enabled)
{
  m_exportOnFileLeftEnabled = enabled;
}

void EvaluationEngine::onExportScheme()
{
  DataExporter::AbstractExporterBackend *backend = nullptr;
  uint32_t exportOpts = 0;

  if (!isContextValid())
    return;

  const DataExporter::Scheme *s = m_dataExporter.scheme(m_currentDataExporterSchemeId);
  if (s == nullptr) {
    QMessageBox::information(nullptr, tr("No scheme"), tr("No valid scheme is currently selected"));
    return;
  }

  if (m_dataExporterFileDlg->exec() != QDialog::Accepted)
    return;

  storeCurrentPeak();

  QString path = m_dataExporterFileDlg->selectedFiles().at(0);

  try {
    switch (m_currentDataExporterBackend) {
    case DataExporterBackends::HTML:
      if (QFileInfo::exists(path)) {
        int ret = QMessageBox::question(nullptr, tr("File exists"), tr("Selected file already exists. Do you wish to overwrite it?"), QMessageBox::Yes | QMessageBox::No);
        if (ret != QMessageBox::Yes)
          return;
      }
      backend = new DataExporter::HtmlExporterBackend(path, s->arrangement);
      break;
    case DataExporterBackends::TEXT:
    {
      /* Append .CSV if no suffix is present */
      {
        QFileInfo f(path);
        if (!f.fileName().contains('.'))
          path += ".csv";
      }

      bool append = false;
      if (QFileInfo::exists(path)) {
        AppendOverwriteExportFileMessageBox mbox;

        mbox.exec();

        switch (mbox.action()) {
        case AppendOverwriteExportFileMessageBox::Action::APPEND:
          exportOpts |= DataExporter::SchemeBaseRoot::EXCLUDE_HEADER;
          append = true;
          break;
        case AppendOverwriteExportFileMessageBox::Action::OVERWRITE:
          break;
        default:
          return;
        }
      }
      backend = new DataExporter::TextExporterBackend(path, m_textDataExporterDelimiter, s->arrangement, append);
    }
      break;
    default:
      break;
    }
  } catch (std::bad_alloc &) {
      QMessageBox::warning(nullptr, tr("Runtime error"), tr("Insufficient memory to export data to file"));
      return;
  }

  if (!s->exportData(this, *backend, exportOpts))
    QMessageBox::warning(nullptr, tr("Data export error"), tr("Unable to export data"));

  delete backend;
}

void EvaluationEngine::onFindPeaks()
{
  findPeakAssisted();
}


void EvaluationEngine::onHvlParametersModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  Q_UNUSED(topLeft); Q_UNUSED(bottomRight); Q_UNUSED(roles)

  m_currentPeak.updateHvlData(m_hvlFitValues, m_hvlFitIntValues, m_hvlFitFixedValues);
}

void EvaluationEngine::onHvlResultsModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  const int from = topLeft.column();
  const int to = bottomRight.column();

  if (!isContextValid())
    return;

  if (from <= m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A1) &&
      to >= m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A1)) {
    m_hvlFitValues[HVLFitResultsItems::Floating::HVL_U_EFF_A1] = calculateA1Mobility(m_hvlFitValues, m_commonParamsEngine->currentContext());

    m_hvlFitModel.notifyDataChanged(HVLFitResultsItems::Floating::HVL_U_EFF_A1, HVLFitResultsItems::Floating::HVL_U_EFF_A1, roles);
  }

  m_currentPeak.updateHvlData(m_hvlFitValues, m_hvlFitIntValues, m_hvlFitFixedValues);
}

void EvaluationEngine::onManageExporterScheme()
{
  m_dataExporter.manageSchemes();
}

void EvaluationEngine::onNoEofStateChanged(const bool noEof)
{
  Q_UNUSED(noEof);

  drawEofMarker();
}

void EvaluationEngine::onPeakSwitched(const QModelIndex &idx)
{
  int row;

  if (!idx.isValid()) {
    m_currentPeakIdx = 0;
    m_currentPeak = m_allPeaks.at(0).peak();
    setPeakContext(m_currentPeak);
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    return;
  }

  storeCurrentPeak();

  row = idx.row() + 1;
  if (row < 1 || row >= m_allPeaks.length())
    return;

  m_currentPeakIdx = row;
  m_currentPeak = m_allPeaks.at(row).peak();
  setPeakContext(m_currentPeak);

  m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;
}

void EvaluationEngine::onPlotPointHovered(const QPointF &point, const QPoint &cursor)
{
  Q_UNUSED(cursor)

  if (!isContextValid())
    return;

  if (m_userInteractionState == UserInteractionState::MANUAL_PEAK_INTEGRATION) {
    QVector<QPointF> line;
    double fx;
    double tx;
    double fy;
    double ty;
    if (m_manualPeakFrom.x() < point.x()) {
      fx = m_manualPeakFrom.x();
      tx = point.x();
      fy = m_manualPeakFrom.y();
      ty = point.y();
    } else {
      fx = point.x();
      tx = m_manualPeakFrom.x();
      fy = point.y();
      ty = m_manualPeakFrom.y();
    }

    line.push_back(QPointF(fx, fy));
    line.push_back(QPointF(tx, ty));

    m_plotCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), line);
    m_plotCtx->replot();
  }
}

void EvaluationEngine::onPlotPointSelected(const QPointF &point, const QPoint &cursor)
{
  QAction *trig;

  if (!isContextValid())
    return;

  switch (m_userInteractionState) {
  case UserInteractionState::FINDING_PEAK:
    trig = m_findPeakMenu->exec(cursor);
    if (trig == nullptr)
      return;

    findPeakMenuTriggered(trig->data().value<FindPeakMenuActions>(), point);
    break;
  case UserInteractionState::MANUAL_PEAK_INTEGRATION:
    trig = m_manualIntegrationMenu->exec(cursor);
    if (trig == nullptr)
      return;

    manualIntegrationMenuTriggered(trig->data().value<ManualIntegrationMenuActions>(), point);
    break;
  case UserInteractionState::PEAK_POSTPROCESSING:
    trig = m_postProcessMenu->exec(cursor);
    if (trig == nullptr)
      return;

    postProcessMenuTriggered(trig->data().value<PostProcessMenuActions>(), point);
    break;
  }
}

void EvaluationEngine::onProvisionalPeakSelected(const QModelIndex index, const QAbstractItemModel *model, const int peakWindow)
{
  bool ok;

  if (!isContextValid())
    return;

  if (!index.isValid())
    return;

  if (model == nullptr)
    return;

  const int row = index.row();
  if (row < 0 || row >= model->rowCount())
    return;

  int idx = model->data(model->index(row, 0), Qt::UserRole + 1).toInt(&ok);
  if (!ok)
    return;

  QVector<QPointF> data;
  const QVector<QPointF> &signalData = m_currentDataContext->data->data;
  int halfPWin = peakWindow / 2;
  int start = (idx - halfPWin) >= 0 ? idx - halfPWin : 0;
  int end = (idx + halfPWin) >= (signalData.length() - 1) ? signalData.length() - 1 : idx + halfPWin;
  for (int iidx = start; iidx < end; iidx++)
    data.push_back(m_currentDataContext->data->data.at(iidx));

  m_plotCtx->setSerieSamples(seriesIndex(Series::PROV_PEAK), data);
  m_plotCtx->replot();
}

void EvaluationEngine::onReadEof()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults->isValid())
    return;

  if (m_commonParamsEngine->boolValue(CommonParametersItems::Boolean::NO_EOF))
    return;

  double tEOF = m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X);
  emit updateTEof(tEOF);
}

void EvaluationEngine::onRegisterPeakInHyperbolaFit(const QModelIndex &idx)
{
  if (!isContextValid())
    return;

  if (!idx.isValid())
    return;

  if (!m_currentPeak.finderResults->isValid())
    return;

  int row = idx.row() + 1;
  if (row < 1 || row >= m_allPeaks.size())
    return;

  StoredPeak &p = m_allPeaks[row];
  RegisterInHyperbolaFitDialog regWidget;
  regWidget.setInformation(p.name,
                           m_commonParamsEngine->numValue(CommonParametersItems::Floating::SELECTOR),
                           p.peak().hvlValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1),
                           p.peak().resultsValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));

  if (regWidget.exec() != QDialog::Accepted)
    return;

  double mobility;

  switch (regWidget.mobilityFrom()) {
  case RegisterInHyperbolaFitWidget::MobilityFrom::HVL_A1:
    mobility = p.peak().hvlValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1);
    break;
  case RegisterInHyperbolaFitWidget::MobilityFrom::PEAK_MAXIMUM:
    mobility = p.peak().resultsValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF);
    break;
  default:
    mobility = std::numeric_limits<double>::infinity();
    break;
  }

  emit registerMeasurement(p.name, m_commonParamsEngine->numValue(CommonParametersItems::Floating::SELECTOR),
                           mobility);
}

void EvaluationEngine::onRenamePeak(const QModelIndex &idx)
{
  if (!isContextValid())
    return;

  if (!idx.isValid())
    return;

  const int row = idx.row() + 1;

  if (row >= m_allPeaks.size())
    return;

  QInputDialog dlg;
  dlg.setTextValue(m_allPeaks[row].name);

  if (dlg.exec() != QDialog::Accepted)
    return;

  const QString name = dlg.textValue().trimmed();

  m_allPeaks[row].name = name;
  m_evaluatedPeaksModel.updateName(row - 1, name);
}

void EvaluationEngine::onReplotHvl()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults->isValid())
    return;

  QVector<QPointF> vec = HVLCalculator::plot(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3),
                                             m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X),
                                             m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_TO_X),
                                             timeStep(m_currentPeak.finderResults->fromIndex, m_currentPeak.finderResults->toIndex),
                                             m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS));

  HVLCalculator::applyBaseline(vec, m_currentPeak.baselineSlope, m_currentPeak.baselineIntercept);
  m_currentPeak.updateHvlPlot(vec);

  m_plotCtx->setSerieSamples(seriesIndex(Series::HVL), vec);

  m_plotCtx->replot();
}

void EvaluationEngine::onSetDefault(EvaluationEngineMsgs::Default msg)
{
  switch (msg) {
  case EvaluationEngineMsgs::Default::FINDER_PARAMETERS:
    return setDefaultFinderParameters();
    break;
  }
}

void EvaluationEngine::onTraverseFiles(const EvaluationEngineMsgs::Traverse dir)
{
  typedef QMap<QString, std::shared_ptr<DataContext>> DataMap;

  if (!m_allDataContexts.contains(m_currentDataContextKey))
    return;

  DataMap::ConstIterator cit = m_allDataContexts.find(m_currentDataContextKey);

  switch (dir) {
  case EvaluationEngineMsgs::Traverse::PREVIOUS:
    if (cit == m_allDataContexts.cbegin())
      return;
    cit--;
    break;
  case EvaluationEngineMsgs::Traverse::NEXT:
    if (cit + 1 == m_allDataContexts.cend())
      return;
    cit++;
    break;
  }

  if (m_exportOnFileLeftEnabled)
    onExportScheme();

  switchEvaluationContext(cit.key());

  for (int idx = 0; idx < m_loadedFilesModel.rowCount(); idx++) {
    const QModelIndex midx = m_loadedFilesModel.index(idx, 0);
    const QString s = m_loadedFilesModel.data(midx, Qt::UserRole + 1).toString();

    if (s == cit.key())
      emit evaluationFileSwitched(idx);
  }
}

void EvaluationEngine::onUnhighlightProvisionalPeak()
{
  m_plotCtx->clearSerieSamples(seriesIndex(Series::PROV_PEAK));
  m_plotCtx->replot();
}

void EvaluationEngine::plotEvaluatedPeak(const std::shared_ptr<PeakFinderResults::Result> &fr, const double peakX,
                                         const double widthHalfLeft, const double widthHalfRight,
                                         const double peakHeight, const double peakHeightBaseline)
{
  /* Draw the baseline */
  {
    QVector<QPointF> blVec;

    blVec.push_back(QPointF(fr->peakFromX, fr->peakFromY));
    blVec.push_back(QPointF(fr->peakToX, fr->peakToY));

    m_plotCtx->setSerieSamples(seriesIndex(Series::BASELINE), blVec);
  }

  /* Mark the maximum of the peak */
  {
    QVector<QPointF> tpVec;

    tpVec.push_back(QPointF(peakX, peakHeight - peakHeightBaseline));
    tpVec.push_back(QPointF(peakX, peakHeight));
    m_plotCtx->setSerieSamples(seriesIndex(Series::PEAK_TIME), tpVec);
  }

  /* Mark the height of the peak */
  {
    QVector<QPointF> hpVec;

    hpVec.push_back(QPointF(peakX - widthHalfLeft, peakHeight));
    hpVec.push_back(QPointF(widthHalfRight + peakX, peakHeight));

    m_plotCtx->setSerieSamples(seriesIndex(Series::PEAK_HEIGHT), hpVec);
  }

  {
    const std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResult> afr = std::dynamic_pointer_cast<AssistedPeakFinder::AssistedPeakFinderResult>(fr);

    if (afr != nullptr) {
      if (m_currentPeak.afContext.showWindow != EvaluationParametersItems::ComboShowWindow::NONE) {
        if (afr->seriesA != nullptr)
          m_plotCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_A), *(afr->seriesA));
        if (afr->seriesB != nullptr)
          m_plotCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_B), *(afr->seriesB));
      }
    }
  }

  /* HVL estimate */
  m_plotCtx->setSerieSamples(seriesIndex(Series::HVL), m_currentPeak.hvlPlot);

  /* Mark the beginning and the end of the peak */
  {
    QVector<QPointF> blFrom;
    QVector<QPointF> blTo;

    blFrom.push_back(QPointF(fr->peakFromX, fr->peakFromY));
    blFrom.push_back(QPointF(fr->peakFromX, m_currentDataContext->data->data.at(fr->fromIndex).y()));

    blTo.push_back(QPointF(fr->peakToX, fr->peakToY));
    blTo.push_back(QPointF(fr->peakToX, m_currentDataContext->data->data.at(fr->toIndex).y()));

    m_plotCtx->setSerieSamples(seriesIndex(Series::BASELINE_FROM), blFrom);
    m_plotCtx->setSerieSamples(seriesIndex(Series::BASELINE_TO), blTo);
  }

  m_plotCtx->replot();
}

void EvaluationEngine::postProcessMenuTriggered(const PostProcessMenuActions &action, const QPointF &point)
{
  switch (action) {
  case PostProcessMenuActions::NEW_PEAK_FROM:
    onCancelEvaluatedPeakSelection();
    beginManualIntegration(point, false);
    break;
  case PostProcessMenuActions::NEW_PEAK_FROM_SIGSNAP:
    onCancelEvaluatedPeakSelection();
    beginManualIntegration(point, true);
    break;
  case PostProcessMenuActions::SPECIFY_PEAK_BOUNDARIES:
    onCancelEvaluatedPeakSelection();
    findPeakPreciseBoundaries();
    break;
  case PostProcessMenuActions::MOVE_PEAK_FROM:
    findPeakManually(point, QPointF(m_currentPeak.finderResults->peakToX, m_currentPeak.finderResults->peakToY), false, false, true);
    break;
  case PostProcessMenuActions::MOVE_PEAK_FROM_SIGSNAP:
    findPeakManually(point, QPointF(m_currentPeak.finderResults->peakToX, m_currentPeak.finderResults->peakToY), true, false, true);
    break;
  case PostProcessMenuActions::MOVE_PEAK_TO:
    findPeakManually(QPointF(m_currentPeak.finderResults->peakFromX, m_currentPeak.finderResults->peakFromY), point, false, false, true);
    break;
  case PostProcessMenuActions::MOVE_PEAK_TO_SIGSNAP:
    findPeakManually(QPointF(m_currentPeak.finderResults->peakFromX, m_currentPeak.finderResults->peakFromY), point, false, true, true);
    break;
  case PostProcessMenuActions::DESELECT_PEAK:
    onCancelEvaluatedPeakSelection();
    break;
  case PostProcessMenuActions::SET_AXIS_TITLES:
    showSetAxisTitlesDialog();
    break;
  case PostProcessMenuActions::SET_EOF_TIME:
    setEofTime(point);
    break;
  case PostProcessMenuActions::SCALE_PLOT_TO_FIT:
    m_plotCtx->scaleToFit();
    return;
    break;
  }
}

EvaluationEngine::PeakContext EvaluationEngine::processFoundPeak(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults::Result> &fr, const AssistedFinderContext &afContext,
                                                                 const bool updateCurrentPeak, const bool doHvlFitRq,
                                                                 const PeakContext &srcCtx)
{
  PeakEvaluator::Parameters ep = makeEvaluatorParameters(data, fr);
  PeakEvaluator::Results er = PeakEvaluator::evaluate(ep);
  double HVL_a0;
  double HVL_a1;
  double HVL_a2;
  double HVL_a3;
  bool hvlOk;
  double hvlEpsilon;
  int hvlIterations;
  int hvlDigits;
  double hvlTUsp;
  MappedVectorWrapper<double, HVLFitResultsItems::Floating> hvlResults;

  if (!er.isValid()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    clearPeakPlots();
    return PeakContext();
  }

  if (!doHvlFitRq && updateCurrentPeak) {
    HVL_a0 = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_A0);
    HVL_a1 = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_A1);
    HVL_a2 = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_A2);
    HVL_a3 = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_A3);
    er.peakArea = srcCtx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA);
    hvlEpsilon = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_EPSILON);
    hvlIterations = srcCtx.hvlFitIntValues.at(HVLFitParametersItems::Int::ITERATIONS);
    hvlDigits = srcCtx.hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS);
    hvlTUsp = srcCtx.hvlValues.at(HVLFitResultsItems::Floating::HVL_TUSP);
  } else {
    er = PeakEvaluator::estimateHvl(er, ep);
    HVL_a0 = er.peakArea;
    HVL_a1 = er.HVL_a1;
    HVL_a2 = er.HVL_a2;
    HVL_a3 = er.HVL_a3;
    hvlEpsilon = m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_EPSILON);
    hvlIterations = m_hvlFitIntValues.at(HVLFitParametersItems::Int::ITERATIONS);
    hvlDigits =  m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS);
    hvlTUsp = er.HVL_tUSP;
  }

  if (doHvlFitRq) {
    hvlResults = doHvlFit(fr,
                          HVL_a0,
                          HVL_a1,
                          HVL_a2,
                          HVL_a3,
                          srcCtx.hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A0),
                          srcCtx.hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A1),
                          srcCtx.hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A2),
                          srcCtx.hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A3),
                          hvlEpsilon, hvlIterations, hvlDigits, hvlTUsp,
                          &hvlOk);
    hvlResults[HVLFitResultsItems::Floating::HVL_TUSP] = er.HVL_tUSP;

    if (!hvlOk) {
      hvlResults[HVLFitResultsItems::Floating::HVL_A0] = HVL_a0;
      hvlResults[HVLFitResultsItems::Floating::HVL_A1] = HVL_a1;
      hvlResults[HVLFitResultsItems::Floating::HVL_A2] = HVL_a2;
      hvlResults[HVLFitResultsItems::Floating::HVL_A3] = HVL_a3;
      hvlResults[HVLFitResultsItems::Floating::HVL_EPSILON] = hvlEpsilon;
    }
  } else {
    if (updateCurrentPeak)
      hvlResults = srcCtx.hvlValues;
    else {
      hvlResults[HVLFitResultsItems::Floating::HVL_A0] = HVL_a0;
      hvlResults[HVLFitResultsItems::Floating::HVL_A1] = HVL_a1;
      hvlResults[HVLFitResultsItems::Floating::HVL_A2] = HVL_a2;
      hvlResults[HVLFitResultsItems::Floating::HVL_A3] = HVL_a3;
      hvlResults[HVLFitResultsItems::Floating::HVL_EPSILON] = hvlEpsilon;
    }
  }

  QVector<QPointF> hvlPlot;
  hvlPlot = HVLCalculator::plot(hvlResults.at(HVLFitResultsItems::Floating::HVL_A0),
                                hvlResults.at(HVLFitResultsItems::Floating::HVL_A1),
                                hvlResults.at(HVLFitResultsItems::Floating::HVL_A2),
                                hvlResults.at(HVLFitResultsItems::Floating::HVL_A3),
                                fr->peakFromX, fr->peakToX, timeStep(fr->fromIndex, fr->toIndex),
                                srcCtx.hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS));
  HVLCalculator::applyBaseline(hvlPlot, er.baselineSlope, er.baselineIntercept);

  const PeakContext ctx = makePeakContext(makePeakContextModels(fr, er,
                                                        hvlResults,
                                                        srcCtx.hvlFitIntValues, srcCtx.hvlFitFixedValues),
                                          afContext, fr, er, hvlPlot);

  m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;

  return ctx;
}

AbstractMapperModel<double, EvaluationResultsItems::Floating> *EvaluationEngine::resultsValuesModel()
{
  return &m_resultsFloatingModel;
}

QVariant EvaluationEngine::saveUserSettings() const
{
  EMT::StringVariantMap map = StandardPlotContextSettingsHandler::saveUserSettings(*m_plotCtx.get(), seriesIndex(Series::LAST_INDEX));
  map[DATAFILELOADER_SETTINGS_TAG] = m_dataFileLoader->saveUserSettings();
  map[HVLFITOPTIONS_DISABLE_AUTO_FIT_TAG] = m_hvlFitOptionsValues.at(HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT);
  map[HVLFITOPTIONS_SHOW_FIT_STATS_TAG] = m_hvlFitOptionsValues.at(HVLFitOptionsItems::Boolean::SHOW_FIT_STATS);
  map[CLIPBOARDEXPORTER_DELIMTIER_TAG] = m_ctcDelimiter;
  map[CLIPBOARDEXPORTER_DATAARRANGEMENT_TAG] = QVariant::fromValue<DataExporter::Globals::DataArrangement>(m_ctcDataArrangement);

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

int EvaluationEngine::seriesIndex(const Series iid)
{
  return static_cast<int>(iid);
}

void EvaluationEngine::setAxisTitles()
{
  QString xUnit;
  QString yUnit;

  if (m_currentDataContext == nullptr)
    return;

  if (m_currentDataContext->data->xUnit.length() > 0)
    xUnit = QString("(%1)").arg(m_currentDataContext->data->xUnit);
   else
    xUnit = "";

  if (m_currentDataContext->data->yUnit.length() > 0)
    yUnit = QString("(%1)").arg(m_currentDataContext->data->yUnit);
  else
    yUnit = "";

  m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, QString("%1 %2").arg(m_currentDataContext->data->xType).arg(xUnit));
  m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, QString("%1 %2").arg(m_currentDataContext->data->yType).arg(yUnit));
}

void EvaluationEngine::setDefaultFinderParameters()
{
  for (int idx = 0; idx < m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::LAST_INDEX); idx++)
      m_evaluationFloatingValues.setItemAt(idx, s_defaultEvaluationFloatingValues.at(idx));
  m_evaluationFloatingModel.notifyDataChangedFromStart(EvaluationParametersItems::Floating::FROM, { Qt::EditRole });

  for (int idx = 0; idx < m_evaluationBooleanModel.indexFromItem(EvaluationParametersItems::Boolean::LAST_INDEX); idx++)
    m_evaluationBooleanValues.setItemAt(idx, s_defaultEvaluationBooleanValues.at(idx));
  m_evaluationBooleanModel.notifyAllDataChanged({ Qt::EditRole });

  for (int idx = m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW);
       idx < m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::LAST_INDEX);
       idx++)
    m_evaluationAutoValues.setItemAt(idx, s_defaultEvaluationAutoValues.at(idx));
  m_evaluationAutoModel.notifyDataChangedToEnd(EvaluationParametersItems::Auto::SLOPE_WINDOW,  { Qt::EditRole });

  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboBaselineAlgorithm::SLOPE)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::SHOW_WINDOW,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboShowWindow::NONE)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::WINDOW_UNITS,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboWindowUnits::MINUTES)));
}

void EvaluationEngine::setEofTime(const QPointF &point)
{
  const double tEof = point.x();

  if (!isContextValid())
    return;

  if (tEof < 0 || tEof >= m_currentDataContext->data->data.back().x())
    return;

  emit updateTEof(tEof);
}

bool EvaluationEngine::setEvaluationContext(const EvaluationContext &ctx)
{
  m_allPeaks = ctx.peaks;
  m_currentPeakIdx = ctx.lastIndex;

  if (m_currentPeakIdx < 0 || m_currentPeakIdx >= m_allPeaks.length()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    return false;
  }

  m_currentPeak = m_allPeaks.at(m_currentPeakIdx).peak();

  m_plotCtx->clearAllSerieSamples();
  setPeakContext(m_currentPeak);

  if (m_currentPeakIdx == 0)
    m_userInteractionState = UserInteractionState::FINDING_PEAK; /* "Default" placeholder peak, enable finding */
  else
    m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;

  return createSignalPlot(m_currentDataContext->data, m_currentDataContext->name);
}

void EvaluationEngine::setEvaluationResults(const PeakContext &ctx)
{
  disconnect(&m_hvlFitModel, &FloatingMapperModel<HVLFitResultsItems::Floating>::dataChanged, this, &EvaluationEngine::onHvlResultsModelChanged);

  m_resultsNumericValues = ctx.resultsValues;
  m_hvlFitValues = ctx.hvlValues;

  m_resultsFloatingModel.notifyAllDataChanged();
  m_hvlFitModel.notifyAllDataChanged();

  connect(&m_hvlFitModel, &FloatingMapperModel<HVLFitResultsItems::Floating>::dataChanged, this, &EvaluationEngine::onHvlResultsModelChanged);
}

bool EvaluationEngine::setPeakContext(const PeakContext &ctx)
{
  m_resultsNumericValues = ctx.resultsValues;
  m_hvlFitValues = ctx.hvlValues;
  m_hvlFitFixedValues = ctx.hvlFitFixedValues;
  m_hvlFitIntValues = ctx.hvlFitIntValues;

  m_evaluationAutoValues = ctx.afContext.afAutoValues;
  m_evaluationBooleanValues = ctx.afContext.afBoolValues;
  m_evaluationFloatingValues = ctx.afContext.afFloatingValues;

  m_windowUnit = ctx.afContext.windowUnits;
  m_showWindow = ctx.afContext.showWindow;
  m_baselineAlgorithm = ctx.afContext.baselineAlgorithm;

  clearPeakPlots();
  fullViewUpdate();
  if (ctx.finderResults->isValid() && (m_currentDataContext->data != nullptr))
    plotEvaluatedPeak(ctx.finderResults,
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X),
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT),
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT),
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT),
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL));

  return true;
}

void EvaluationEngine::setPeakFinderParameters(const double maxX)
{
  /* Scale search windows accordingly to the X data range if the range is too small */

  if (m_windowUnit != EvaluationParametersItems::ComboWindowUnits::MINUTES)
    return;

  if (maxX / 2.0 <= s_defaultEvaluationFloatingValues.at(1))
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_WINDOW] = 0.10 * maxX;

  if (maxX / 2.0 <= s_defaultEvaluationFloatingValues.at(2))
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE_WINDOW] = 0.10 * maxX;

  if (maxX / 2.0 <= s_defaultEvaluationFloatingValues.at(3))
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_WINDOW] = 0.20 * maxX;

  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::NOISE_WINDOW, EvaluationParametersItems::Floating::NOISE_WINDOW, { Qt::EditRole });
  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::PEAK_WINDOW, EvaluationParametersItems::Floating::PEAK_WINDOW, { Qt::EditRole });
  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::SLOPE_WINDOW, EvaluationParametersItems::Floating::SLOPE_WINDOW, { Qt::EditRole });
}

void EvaluationEngine::showSetAxisTitlesDialog()
{
  if (m_currentDataContext == nullptr)
    return;

  SetAxisTitlesDialog dlg(m_currentDataContext->data->xType, m_currentDataContext->data->xUnit, m_currentDataContext->data->yType, m_currentDataContext->data->yUnit);

  if (dlg.exec() == QDialog::Accepted) {
    m_currentDataContext->data->xType = dlg.xType();
    m_currentDataContext->data->xUnit = dlg.xUnit();
    m_currentDataContext->data->yType = dlg.yType();
    m_currentDataContext->data->yUnit = dlg.yUnit();

    setAxisTitles();
  }
}

QAbstractItemModel *EvaluationEngine::showWindowModel()
{
  return &m_showWindowModel;
}

bool EvaluationEngine::storeCurrentContext()
{
  /* Do not store the default empty context */
  if (isContextValid()) {
    storeCurrentPeak();

    try {
      std::shared_ptr<DataContext> oldCtx = std::shared_ptr<DataContext>(new DataContext(m_currentDataContext->data, m_currentDataContext->name,
                                                                                         m_commonParamsEngine->currentContext(), currentEvaluationContext()));
      m_allDataContexts[m_currentDataContextKey] = oldCtx;
    } catch (std::bad_alloc&) {
      QMessageBox::warning(nullptr, tr("Rutime error"), tr("Cannot store current data context"));
      return false;
    }
    return true;
  }
  return true;
}

void EvaluationEngine::storeCurrentPeak()
{
  if (m_currentPeakIdx == 0)
    return;

  m_allPeaks[m_currentPeakIdx].updatePeak(makePeakContext(m_resultsNumericValues,
                                                          m_hvlFitValues,
                                                          m_hvlFitIntValues,
                                                          m_hvlFitFixedValues,
                                                          m_currentPeak));
}

void EvaluationEngine::switchEvaluationContext(const QString &key)
{
  storeCurrentContext();

  if (!m_allDataContexts.contains(key)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Data context for key %1 not found!")).arg(key));
    return;
  }

  m_currentDataContext = m_allDataContexts[key];
  m_currentDataContextKey = key;

  activateCurrentDataContext();
}

void EvaluationEngine::switchWindowUnit(const EvaluationParametersItems::ComboWindowUnits unit)
{
  int length;

  if (!isContextValid())
    return;

  if (m_currentDataContext->data == nullptr)
    return;

  length = m_currentDataContext->data->data.length();
  if (length == 0)
    return;

  double toTime = m_currentDataContext->data->data.last().x();
  double timeToPtCoeff = toTime / length;

  double nw = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::NOISE_WINDOW);
  double pw = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_WINDOW);
  double sw = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_WINDOW);

  switch (unit) {
  case EvaluationParametersItems::ComboWindowUnits::MINUTES:
    nw = nw * timeToPtCoeff;
    pw = pw * timeToPtCoeff;
    sw = sw * timeToPtCoeff;

    if (nw > toTime)
      nw = toTime;
    if (pw > toTime)
      pw = toTime;
    if (sw > toTime)
      sw = toTime;
    break;
  case EvaluationParametersItems::ComboWindowUnits::POINTS:
    nw = floor(nw / timeToPtCoeff);
    pw = floor(pw / timeToPtCoeff);
    sw = floor(sw / timeToPtCoeff);

    if (nw >= length)
      nw = length - 1;
    if (pw >= length)
      pw = length - 1;
    if (sw >= length)
      sw = length - 1;
    break;
  default:
    return;
    break;
  }

  m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE_WINDOW] = nw;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_WINDOW] = pw;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_WINDOW] = sw;

  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::NOISE_WINDOW, EvaluationParametersItems::Floating::NOISE_WINDOW,  { Qt::EditRole });
  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::PEAK_WINDOW, EvaluationParametersItems::Floating::PEAK_WINDOW, { Qt::EditRole });
  m_evaluationFloatingModel.notifyDataChanged(EvaluationParametersItems::Floating::SLOPE_WINDOW, EvaluationParametersItems::Floating::SLOPE_WINDOW, { Qt::EditRole });
}

double EvaluationEngine::timeStep(const int fromIdx, const int toIdx)
{
  if (!isContextValid())
    return 1.0;

  if (m_currentDataContext->data == nullptr)
    return 1.0;

  if (m_currentDataContext->data->data.length() == 0)
    return 1.0;

  if (toIdx <= fromIdx)
    return 1.0;

  double dt = (m_currentDataContext->data->data.at(toIdx).x() - m_currentDataContext->data->data.at(fromIdx).x()) / (toIdx - fromIdx);

  return dt;
}

void EvaluationEngine::walkFoundPeaks(const QVector<std::shared_ptr<PeakFinderResults::Result>> &results, const AssistedFinderContext &afContext,
                                      const bool updatePeak)
{
  const bool disableAutoFit = m_hvlFitOptionsValues.at(HVLFitOptionsItems::Boolean::DISABLE_AUTO_FIT);

  if (results.size() == 0) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    clearPeakPlots();
  } else if (results.size() == 1) {
    const std::shared_ptr<PeakFinderResults::Result> &r = results.at(0);

    AssistedFinderContext afcInner(afContext);

    const std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResult> afr = std::dynamic_pointer_cast<AssistedPeakFinder::AssistedPeakFinderResult>(r);
    if (afr != nullptr) {
      afcInner.setProcessingData(afr->noise,
                                 afr->noiseRefPoint, afr->slopeRefPoint,
                                 afr->slopeThreshold, afr->slopeWindow);
    }

    const PeakContext ctx = processFoundPeak(m_currentDataContext->data->data, r, afcInner, updatePeak, !disableAutoFit, m_currentPeak);
    m_currentPeak = ctx;

    if (updatePeak && m_currentPeakIdx > 0) {
      m_allPeaks[m_currentPeakIdx].updatePeak(m_currentPeak);
      m_evaluatedPeaksModel.updateEntry(m_currentPeakIdx - 1,
                                        ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                        ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
    }
  } else {
    int ctr = 1;
    for (const std::shared_ptr<PeakFinderResults::Result> &r : results) {

      AssistedFinderContext afcInner(afContext);

      const std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResult> afr = std::dynamic_pointer_cast<AssistedPeakFinder::AssistedPeakFinderResult>(r);
      if (afr != nullptr) {
        afcInner.setProcessingData(afr->noise,
                                   afr->noiseRefPoint, afr->slopeRefPoint,
                                   afr->slopeThreshold, afr->slopeWindow);
      }

      const PeakContext ctx = processFoundPeak(m_currentDataContext->data->data, r, afcInner, false, !disableAutoFit, m_currentPeak);
      addPeakToList(ctx, QString::number(ctr), false, RegisterInHyperbolaFitWidget::MobilityFrom::HVL_A1); /* 3rd parameter is useless since 2nd is set to false */
      ctr++;
    }

    m_currentPeak = m_allPeaks.back().peak();
  }
}

QAbstractItemModel *EvaluationEngine::windowUnitsModel()
{
  return &m_windowUnitsModel;
}
