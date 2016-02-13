#include "setup.h"

#include "evaluationengine.h"
#include "gui/addpeakdialog.h"
#include "custommetatypes.h"
#include "helpers.h"
#include "standardmodecontextsettingshandler.h"
#include <QMenu>
#include <QMessageBox>

const QVector<bool> EvaluationEngine::s_defaultEvaluationAutoValues({true, /* PEAK_FROM_X */
                                                                     true, /* PEAK_FROM_Y */
                                                                     true, /* PEAK_TO_X */
                                                                     true, /* PEAK_TO_Y */
                                                                     true, /* SLOPE_WINDOW */
                                                                     true, /* NOISE */
                                                                     true, /* SLOPE_REF_POINT */
                                                                     true, /* SLOPE_THRESHOLD */
                                                                     true, /* FROM */
                                                                     true, /* TO */
                                                                     true, /* PEAK_X */
                                                                     true, /* PEAK_HEIGHT */
                                                                     true  /* PEAK WIDTH */});

const QVector<bool> EvaluationEngine::s_defaultEvaluationBooleanValues({false /* NOISE_CORRECTION */});

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
                                                                          0.0, /*PEAK_FROM_X */
                                                                          0.0, /* PEAK_FROM_Y */
                                                                          0.0, /* PEAK_TO_X */
                                                                          0.0, /* PEAK_TO_Y */
                                                                          0.0, /* PEAK_X */
                                                                          0.0, /* PEAK_HEIGHT */
                                                                          0.0  /* PEAK_WIDTH */});

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

const QString EvaluationEngine::s_emptyCtxKey = "";

const double EvaluationEngine::s_defaultHvlEpsilon = 1.0e-9;
const int EvaluationEngine::s_defaultHvlDigits = 50;
const int EvaluationEngine::s_defaultHvlIterations = 10;

const QString EvaluationEngine::DATAFILELOADER_SETTINGS_TAG("DataFileLoader");

EvaluationEngine::DataContext::DataContext(std::shared_ptr<DataFileLoader::Data> data, const QString &name,
                                           const CommonParametersEngine::Context &commonCtx, const EvaluationContext &evalCtx) :
  data(data),
  name(name),
  commonContext(commonCtx),
  evaluationContext(evalCtx)
{
}

EvaluationEngine::EvaluationContext::EvaluationContext(const QVector<PeakContext> &peaks, const int lastIndex) :
  peaks(peaks),
  lastIndex(lastIndex)
{
}

EvaluationEngine::EvaluationContext &EvaluationEngine::EvaluationContext::operator=(const EvaluationContext &other)
{
  const_cast<QVector<PeakContext>&>(peaks) = other.peaks;
  const_cast<int&>(lastIndex) = other.lastIndex;

  return *this;
}

EvaluationEngine::PeakContext::PeakContext(const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &autoValues,
                                           const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &boolValues,
                                           const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &floatingValues,
                                           const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues, const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                           const EvaluationParametersItems::ComboWindowUnits windowUnit, const EvaluationParametersItems::ComboShowWindow showWindow,
                                           const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                                           const PeakFinder::Results finderResults, const QVector<QPointF> &hvlPlot) :
  autoValues(autoValues), boolValues(boolValues), floatingValues(floatingValues),
  resultsValues(resultsValues), hvlValues(hvlValues),
  windowUnit(windowUnit), showWindow(showWindow), baselineAlgorithm(baselineAlgorithm),
  finderResults(finderResults), hvlPlot(hvlPlot), peakName("")
{
}

EvaluationEngine::PeakContext::PeakContext() :
  windowUnit(EvaluationParametersItems::ComboWindowUnits::LAST_INDEX),
  showWindow(EvaluationParametersItems::ComboShowWindow::LAST_INDEX),
  baselineAlgorithm(EvaluationParametersItems::ComboBaselineAlgorithm::LAST_INDEX),
  finderResults(PeakFinder::Results()),
  peakName("")
{
}

void EvaluationEngine::PeakContext::setPeakName(const QString &name)
{
  const_cast<QString&>(peakName) = name;
}


EvaluationEngine::PeakContext &EvaluationEngine::PeakContext::operator=(const PeakContext &other)
{
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Auto>&>(autoValues) = other.autoValues;
  const_cast<MappedVectorWrapper<bool, EvaluationParametersItems::Boolean>&>(boolValues) = other.boolValues;
  const_cast<MappedVectorWrapper<double, EvaluationParametersItems::Floating>&>(floatingValues) = other.floatingValues;
  const_cast<MappedVectorWrapper<double, EvaluationResultsItems::Floating>&>(resultsValues) = other.resultsValues;
  const_cast<MappedVectorWrapper<double, HVLFitResultsItems::Floating>&>(hvlValues) = other.hvlValues;
  const_cast<EvaluationParametersItems::ComboWindowUnits&>(windowUnit) = other.windowUnit;
  const_cast<EvaluationParametersItems::ComboShowWindow&>(showWindow) = other.showWindow;
  const_cast<EvaluationParametersItems::ComboBaselineAlgorithm&>(baselineAlgorithm) = other.baselineAlgorithm;
  const_cast<PeakFinder::Results&>(finderResults) = other.finderResults;
  const_cast<QVector<QPointF>&>(hvlPlot) = other.hvlPlot;

  return *this;
}

EvaluationEngine::EvaluationEngine(CommonParametersEngine *commonParamsEngine, QObject *parent) : QObject(parent),
  m_commonParamsEngine(commonParamsEngine),
  m_evaluationAutoValues(s_defaultEvaluationAutoValues),
  m_evaluationBooleanValues(s_defaultEvaluationBooleanValues),
  m_evaluationFloatingValues(s_defaultEvaluationFloatingValues),
  m_baselineAlgorithmModel(s_baselineAlgorithmValues, this),
  m_loadedFilesModel(QVector<ComboBoxItem<QString>>(), this),
  m_showWindowModel(s_showWindowValues, this),
  m_windowUnitsModel(s_windowUnitsValues, this)
{
  try {
    m_dataFileLoader = new DataFileLoader(this);
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate DataFileLoader"));
    throw;
  }

  try {
    m_contextMenu = createContextMenu();
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate ContextMenu"));
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

  connect(m_dataFileLoader, &DataFileLoader::dataLoaded, this, &EvaluationEngine::onDataLoaded);
  connect(&m_hvlFitModel, &FloatingMapperModel<HVLFitResultsItems::Floating>::dataChanged, this, &EvaluationEngine::onHvlResultsModelChanged);
  connectPeakUpdate();

  m_addPeakDlg = new AddPeakDialog();

  m_currentPeakIdx = 0;
  m_allPeaks.push_back(freshPeakContext());
  m_currentDataContext = std::shared_ptr<DataContext>(new DataContext(nullptr, s_emptyCtxKey, m_commonParamsEngine->currentContext(),
                                                                      currentEvaluationContext()));
  m_currentDataContextKey = s_emptyCtxKey;

  connect(this, &EvaluationEngine::updateTEof, m_commonParamsEngine, &CommonParametersEngine::onUpdateTEof);
}

EvaluationEngine::~EvaluationEngine()
{
  delete m_addPeakDlg;
  delete m_contextMenu;
}

void EvaluationEngine::assignContext(std::shared_ptr<ModeContextLimited> ctx)
{
  m_modeCtx = ctx;


  if (!m_modeCtx->addSerie(seriesIndex(Series::BASELINE), s_serieBaselineTitle, SerieProperties::VisualStyle(QPen(Qt::red))))
  if (!m_modeCtx->addSerie(seriesIndex(Series::BASELINE), s_serieBaselineTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::red, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::PEAK_HEIGHT), s_seriePeakHeightTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::blue, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_seriePeakHeightTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::PEAK_TIME), s_seriePeakTimeTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::blue, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_seriePeakTimeTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::SIG), s_serieSignalTitle))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieSignalTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::EOF_MARK), s_serieEofTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::gray, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieEofTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::FINDER_SYSTEM_A), s_serieFinderSystemATitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::green, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieFinderSystemATitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::FINDER_SYSTEM_B), s_serieFinderSystemBTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(0,255,255), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieFinderSystemBTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::HVL), s_serieHVLTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(37,102,222), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieHVLTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::PROV_PEAK), s_serieProvisionalPeakTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::red, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieProvisionalPeakTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::BASELINE_FROM), s_serieBaselineFromTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(132, 172, 172), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineFromTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::BASELINE_TO), s_serieBaselineFromTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(132, 172, 172), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieBaselineToTitle));

  connect(m_modeCtx.get(), &ModeContextLimited::pointSelected, this, &EvaluationEngine::onPlotPointSelected);

  /* Default series titles */
  m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "");
  m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, "");

  /* Default axis fonts */
  m_modeCtx->setAxisFont(SerieProperties::Axis::X_BOTTOM, QFont());
  m_modeCtx->setAxisFont(SerieProperties::Axis::Y_LEFT, QFont());
}

AbstractMapperModel<bool, EvaluationParametersItems::Auto> *EvaluationEngine::autoValuesModel()
{
  return &m_evaluationAutoModel;
}

QAbstractItemModel *EvaluationEngine::baselineModel()
{
  return &m_baselineAlgorithmModel;
}

AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *EvaluationEngine::booleanValuesModel()
{
  return &m_evaluationBooleanModel;
}

double EvaluationEngine::calculateA1Mobility(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                                             const MappedVectorWrapper<double, CommonParametersItems::Floating> commonData)
{
  const double tP = hvlValues.at(HVLFitResultsItems::Floating::HVL_A1) * 60.0;
  const double tEOF = commonData.at(CommonParametersItems::Floating::T_EOF) * 60.0;
  const double detector = commonData.at(CommonParametersItems::Floating::DETECTOR) * 1.0e-2 ;
  const double E = commonData.at(CommonParametersItems::Floating::FIELD) * 1.0e+3;

  double vP;
  double vP_Eff;
  double vEOF;

  if (Helpers::isSensible(tP))
    vP = detector / tP;
  else
    return std::numeric_limits<double>::infinity();

  if (Helpers::isSensible(tEOF))
    vEOF = detector / tEOF;
  else
    return std::numeric_limits<double>::infinity();

  vP_Eff = vP - vEOF;

  if (Helpers::isSensible(E))
    return (vP_Eff / E) / 1.0e-9;
  else
    return std::numeric_limits<double>::infinity();
}

void EvaluationEngine::connectPeakUpdate()
{
  connect(&m_evaluationAutoModel, &BooleanMapperModel<EvaluationParametersItems::Auto>::dataChanged,
          this, &EvaluationEngine::onUpdateCurrentPeak);
  connect(&m_evaluationFloatingModel, &FloatingMapperModel<EvaluationParametersItems::Floating>::dataChanged,
          this, &EvaluationEngine::onUpdateCurrentPeak);
  connect(&m_evaluationBooleanModel, &BooleanMapperModel<EvaluationParametersItems::Boolean>::dataChanged,
          this, &EvaluationEngine::onUpdateCurrentPeak);
  connect(m_commonParamsEngine, &CommonParametersEngine::tEofUpdated, this, &EvaluationEngine::onUpdateCurrentPeak);
}

void EvaluationEngine::contextMenuTriggered(const ContextMenuActions &action, const QPointF &point)
{
  QModelIndex autoFrom;
  QModelIndex autoTo;
  QModelIndex valueFrom;
  QModelIndex valueTo;

  switch (action) {
  case ContextMenuActions::PEAK_FROM_THIS_X:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_FROM_X] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_X] = point.x();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_X));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_X));
    valueTo = valueFrom;
    break;
  case ContextMenuActions::PEAK_FROM_THIS_Y:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_FROM_Y] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_Y] = point.y();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_Y));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_Y));
    valueTo = valueFrom;
    break;
  case ContextMenuActions::PEAK_FROM_THIS_XY:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_FROM_X] = false;
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_FROM_Y] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_X] = point.x();
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_Y] = point.y();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_X));
    autoTo = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_FROM_Y));
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_X));
    valueTo = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_FROM_Y));
    break;
  case ContextMenuActions::PEAK_TO_THIS_X:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_TO_X] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_X] = point.x();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_X));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_X));
    valueTo = valueFrom;
    break;
  case ContextMenuActions::PEAK_TO_THIS_Y:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_TO_Y] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_Y] = point.y();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_Y));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_Y));
    valueTo = valueFrom;
    break;
  case ContextMenuActions::PEAK_TO_THIS_XY:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_TO_X] = false;
    m_evaluationAutoValues[EvaluationParametersItems::Auto::PEAK_TO_Y] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_X] = point.x();
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_Y] = point.y();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_X));
    autoTo = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_Y));
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_X));
    valueTo = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_TO_Y));
    break;
  case ContextMenuActions::NOISE_REF_POINT:
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE_REF_POINT] = point.x();
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::NOISE_REF_POINT));
    valueTo = valueFrom;
    break;
  case ContextMenuActions::SLOPE_REF_POINT:
    m_evaluationAutoValues[EvaluationParametersItems::Auto::SLOPE_REF_POINT] = false;
    m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_REF_POINT] = point.x();
    autoFrom = m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_REF_POINT));
    autoTo = autoFrom;
    valueFrom = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::SLOPE_REF_POINT));
    valueTo = valueFrom;
    break;
  default:
    return;
    break;
  }

  if (autoFrom.isValid())
    emit m_evaluationAutoModel.dataChanged(autoFrom, autoTo, { Qt::EditRole });
  if (valueFrom.isValid())
    emit m_evaluationFloatingModel.dataChanged(valueFrom, valueTo, { Qt::EditRole });
}

void EvaluationEngine::clearPeakPlots()
{
  m_modeCtx->clearSerieSamples(seriesIndex(Series::BASELINE));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::EOF_MARK));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::PEAK_HEIGHT));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::PEAK_TIME));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::FINDER_SYSTEM_A));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::FINDER_SYSTEM_B));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::HVL));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::BASELINE_FROM));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::BASELINE_TO));

  m_modeCtx->replot();
}

QMenu *EvaluationEngine::createContextMenu()
{
  QAction *a;
  QMenu *m = new QMenu;

  a = new QAction(tr("Peak from this X"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_FROM_THIS_X));
  m->addAction(a);

  a = new QAction(tr("Peak from this Y"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_FROM_THIS_Y));
  m->addAction(a);

  a = new QAction(tr("Peak from this X,Y"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_FROM_THIS_XY));
  m->addAction(a);

  m->addSeparator();

  a = new QAction(tr("Peak to this X"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_TO_THIS_X));
  m->addAction(a);

  a = new QAction(tr("Peak to this Y"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_TO_THIS_Y));
  m->addAction(a);

  a = new QAction(tr("Peak to this X,Y"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::PEAK_TO_THIS_XY));
  m->addAction(a);

  m->addSeparator();

  a = new QAction(tr("Set noise reference point"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::NOISE_REF_POINT));
  m->addAction(a);

  a = new QAction(tr("Set slope reference point"), m);
  a->setData(QVariant::fromValue<ContextMenuActions>(ContextMenuActions::SLOPE_REF_POINT));
  m->addAction(a);

  return m;
}

bool EvaluationEngine::createSignalPlot(std::shared_ptr<DataFileLoader::Data> data, const QString &name)
{
  m_modeCtx->setPlotTitle(name);

  if (data == nullptr) {
    m_modeCtx->clearAllSerieSamples();
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "");
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, "");
  } else {
    m_modeCtx->setSerieSamples(seriesIndex(Series::SIG), data->data);
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, QString("%1 (%2)").arg(data->xType).arg(data->xUnit));
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, QString("%1 (%2)").arg(data->yType).arg(data->yUnit));
  }

  m_modeCtx->replot();

  return true;
}

EvaluationEngine::EvaluationContext EvaluationEngine::currentEvaluationContext() const
{
  return EvaluationContext(m_allPeaks, m_currentPeakIdx);
}

EvaluationEngine::PeakContext EvaluationEngine::currentPeakContext(const PeakFinder::Results finderResults, const QVector<QPointF> &hvlPlot) const
{
  return PeakContext(m_evaluationAutoValues, m_evaluationBooleanValues,
                     m_evaluationFloatingValues, m_resultsNumericValues,
                     m_hvlFitValues,
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     finderResults, hvlPlot);
}

void EvaluationEngine::disconnectPeakUpdate()
{
  disconnect(&m_evaluationAutoModel, &BooleanMapperModel<EvaluationParametersItems::Auto>::dataChanged,
             this, &EvaluationEngine::onUpdateCurrentPeak);
  disconnect(&m_evaluationFloatingModel, &FloatingMapperModel<EvaluationParametersItems::Floating>::dataChanged,
             this, &EvaluationEngine::onUpdateCurrentPeak);
  disconnect(&m_evaluationBooleanModel, &BooleanMapperModel<EvaluationParametersItems::Boolean>::dataChanged,
             this, &EvaluationEngine::onUpdateCurrentPeak);
  disconnect(m_commonParamsEngine, &CommonParametersEngine::tEofUpdated, this, &EvaluationEngine::onUpdateCurrentPeak);
}

EvaluationEngine::PeakContext EvaluationEngine::duplicatePeakContext() const
{
  /* Despite its name this function only copies the parameters of current
   * peak context, the results are created empty */
  return PeakContext(m_evaluationAutoValues, m_evaluationBooleanValues,
                     m_evaluationFloatingValues,
                     MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     PeakFinder::Results(), QVector<QPointF>());
}

void EvaluationEngine::displayAutomatedResults(const PeakFinder::Results &fr, const PeakEvaluator::Results &er)
{
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_X] = fr.peakFromX;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_FROM_Y] = fr.peakFromY;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_X] = fr.peakToX;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_TO_Y] = fr.peakToY;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_HEIGHT] = fr.peakHeight;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_X] = fr.peakX;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::PEAK_WIDTH] = er.widthHalfLeft;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE] = fr.noise;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_WINDOW] = fr.slopeWindow;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_THRESHOLD] = fr.slopeThreshold;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_REF_POINT] = fr.slopeRefPoint;

  emit m_evaluationFloatingModel.dataChanged(m_evaluationFloatingModel.index(0, 0),
                                             m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::LAST_INDEX)),
                                             { Qt::EditRole });
}

QVector<double> EvaluationEngine::emptyHvlValues() const
{
  QVector<double> empty;
  empty.reserve(m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX));

  for (int idx = 0; idx < m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX); idx++)
      empty.push_back(0.0);

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

QAbstractItemModel *EvaluationEngine::loadedFilesModel()
{
  return &m_loadedFilesModel;
}

void EvaluationEngine::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  StandardModeContextSettingsHandler::loadUserSettings(settings, *m_modeCtx.get());

  EMT::StringVariantMap map = settings.value<EMT::StringVariantMap>();

  if (map.contains(DATAFILELOADER_SETTINGS_TAG)) {
    QVariant v = map[DATAFILELOADER_SETTINGS_TAG];

    m_dataFileLoader->loadUserSettings(v);
  }
}

void EvaluationEngine::findPeak(const bool useCurrentPeak)
{
  SelectPeakDialog dialog;
  PeakFinder::Results fr;

  if (!isContextValid())
    return;

  if (m_currentDataContext->data->data.length() == 0)
    return;

  PeakFinder::Parameters fp = makeFinderParameters(!useCurrentPeak);

  connect(&dialog, &SelectPeakDialog::listClicked, this, &EvaluationEngine::onProvisionalPeakSelected);
  connect(&dialog, &SelectPeakDialog::closedSignal, this, &EvaluationEngine::onUnhighlightProvisionalPeak);

  if (useCurrentPeak)
    fr = PeakFinder::find(fp, &dialog, m_currentPeak.finderResults.tPiCoarse);
  else
    fr = PeakFinder::find(fp, &dialog, -1.0);

  if (!fr.isValid())
    return;

  PeakEvaluator::Parameters ep = makeEvaluatorParameters(fp, fr);
  PeakEvaluator::Results er = PeakEvaluator::evaluate(ep);

  /* Prevent infinite signal-slot loop by disconnecting signals that
   * recalculate the peak every time the evaluation parameters change */
  disconnectPeakUpdate();

  if (!useCurrentPeak)
    setDefaultPeakProperties();

  displayAutomatedResults(fr, er);
  setEvaluationResults(fr, er);

  QVector<QPointF> hvlPlot;
  if (HVLCalculator::available()) {
    hvlPlot = HVLCalculator::plot(er.peakArea, er.HVL_a1, er.HVL_a2, er.HVL_a3, fr.peakFromX, fr.peakToX, timeStep(), 50);
    HVLCalculator::applyBaseline(hvlPlot, fr.baselineSlope, fr.baselineIntercept);
  }

  m_currentPeak = currentPeakContext(fr, hvlPlot);

  clearPeakPlots();
  plotEvaluatedPeak(fr);

  if (m_currentPeakIdx > 0 && useCurrentPeak) {
    m_allPeaks[m_currentPeakIdx] = m_currentPeak;
    m_evaluatedPeaksModel.updateEntry(m_currentPeakIdx - 1, fr.peakX, er.peakArea);
  }

  connectPeakUpdate();
}

EvaluationEngine::EvaluationContext EvaluationEngine::freshEvaluationContext() const
{
  QVector<PeakContext> fresh;

  fresh.push_back(duplicatePeakContext());
  return EvaluationContext(fresh, 0);
}

EvaluationEngine::PeakContext EvaluationEngine::freshPeakContext() const
{
  return PeakContext(MappedVectorWrapper<bool, EvaluationParametersItems::Auto>(s_defaultEvaluationAutoValues),
                     MappedVectorWrapper<bool, EvaluationParametersItems::Boolean>(s_defaultEvaluationBooleanValues),
                     MappedVectorWrapper<double, EvaluationParametersItems::Floating>(s_defaultEvaluationFloatingValues),
                     MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     PeakFinder::Results(), QVector<QPointF>());
}

void EvaluationEngine::fullViewUpdate()
{
  emit m_evaluationAutoModel.dataChanged(m_evaluationAutoModel.index(0, 0),
                                         m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::LAST_INDEX) - 1),
                                         { Qt::EditRole });
  emit m_evaluationBooleanModel.dataChanged(m_evaluationBooleanModel.index(0, 0),
                                            m_evaluationBooleanModel.index(0, m_evaluationBooleanModel.indexFromItem(EvaluationParametersItems::Boolean::LAST_INDEX) - 1),
                                            { Qt::EditRole });
  emit m_evaluationFloatingModel.dataChanged(m_evaluationFloatingModel.index(0, 0),
                                             m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::LAST_INDEX) - 1),
                                            { Qt::EditRole });
  emit m_resultsFloatingModel.dataChanged(m_resultsFloatingModel.index(0, 0),
                                          m_resultsFloatingModel.index(0, m_resultsFloatingModel.indexFromItem(EvaluationResultsItems::Floating::LAST_INDEX)));

  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::WINDOW_UNITS,
                                                                   EvaluationParametersItems::index(m_windowUnit)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::SHOW_WINDOW,
                                                                   EvaluationParametersItems::index(m_showWindow)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                                   EvaluationParametersItems::index(m_baselineAlgorithm)));
}

bool EvaluationEngine::isContextValid() const
{
  return (m_currentDataContextKey.compare(s_emptyCtxKey) == 0) ? false : true;
}

QVector<EvaluatedPeaksModel::EvaluatedPeak> EvaluationEngine::makeEvaluatedPeaks()
{
  QVector<EvaluatedPeaksModel::EvaluatedPeak> peaks;

  for (int idx = 1; idx < m_allPeaks.length(); idx++) {
    const PeakContext &ctx = m_allPeaks.at(idx);
    peaks.push_back(EvaluatedPeaksModel::EvaluatedPeak(ctx.peakName,
                                                       ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                                       ctx.resultsValues.at(EvaluationResultsItems::Floating::PEAK_AREA)));
  }

  return peaks;
}

PeakEvaluator::Parameters EvaluationEngine::makeEvaluatorParameters(const PeakFinder::Parameters &fp, const PeakFinder::Results &fr)
{
  PeakEvaluator::Parameters p(fp.data);

  p.BSLIntercept = fr.baselineIntercept;
  p.BSLSlope = fr.baselineSlope;
  p.capillary = m_commonParamsEngine->value(CommonParametersItems::Floating::CAPILLARY);
  p.detector = m_commonParamsEngine->value(CommonParametersItems::Floating::DETECTOR);
  p.E = m_commonParamsEngine->value(CommonParametersItems::Floating::FIELD);
  p.HP_BSL = fr.peakHeightBaseline;
  p.tAi = fr.fromPeakIndex;
  p.tBi = fr.toPeakIndex;
  p.tEOF = fp.tEOF;
  p.tP = fr.peakX;
  p.tWPLeft = fr.twPLeft;
  p.tWPRight = fr.twPRight;
  p.voltage = m_commonParamsEngine->value(CommonParametersItems::Floating::VOLTAGE);
  p.autoWidthHalfLeft = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_WIDTH);
  p.widthHalfLeft = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_WIDTH);

  return p;
}

PeakFinder::Parameters EvaluationEngine::makeFinderParameters(bool autoPeakProps)
{
  PeakFinder::Parameters p(m_currentDataContext->data->data);

  p.autoFrom = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::FROM);
  p.autoNoise = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::NOISE);
  if (autoPeakProps) {
    p.autoPeakFromX = true;
    p.autoPeakFromY = true;
    p.autoPeakToX = true;
    p.autoPeakToY = true;
    p.autoPeakX = true;
    p.autoPeakHeight = true;
    p.autoPeakWidth = true;
  } else {
    p.autoPeakFromX = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_FROM_X);
    p.autoPeakFromY = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_FROM_Y);
    p.autoPeakHeight = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_HEIGHT);
    p.autoPeakToX = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_TO_X);
    p.autoPeakToY = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_TO_Y);
    p.autoPeakX = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_X);
    p.autoPeakWidth = m_evaluationAutoValues.at(EvaluationParametersItems::Auto::PEAK_WIDTH);
  }
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
  p.peakFromX = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_FROM_X);
  p.peakFromY = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_FROM_Y);
  p.peakHeight = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_HEIGHT);
  p.peakToX = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_TO_X);
  p.peakToY = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_TO_Y);
  p.peakWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_WINDOW);
  p.peakX = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::PEAK_X);
  p.showWindow = m_showWindow;
  p.slopeRefPoint = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_REF_POINT);
  p.slopeSensitivity = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_SENSITIVITY);
  p.slopeThreshold = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_THRESHOLD);
  p.slopeWindow = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::SLOPE_WINDOW);
  p.tEOF = m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF);
  p.to = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::TO);
  p.windowUnits = m_windowUnit;

  return p;
}

AbstractMapperModel<double, EvaluationParametersItems::Floating> *EvaluationEngine::floatingValuesModel()
{
  return &m_evaluationFloatingModel;
}

void EvaluationEngine::onAddPeak()
{
  if (!isContextValid())
    return;

  /* Peak has no meaningful evaluation resutls,
   * do not add it */
  if (!m_currentPeak.finderResults.isValid())
    return;

  m_addPeakDlg->setInformation(m_commonParamsEngine->value(CommonParametersItems::Floating::SELECTOR),
                               m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1),
                               m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF));
  int dlgRet = m_addPeakDlg->exec();
  if (dlgRet != QDialog::Accepted)
    return;

  AddPeakDialog::Answer answer = m_addPeakDlg->answer();
  m_currentPeak.setPeakName(answer.name);

  try {
    m_allPeaks.push_back(m_currentPeak);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add peak"));
    return;
  }

  const EvaluatedPeaksModel::EvaluatedPeak evpeak(answer.name,
                                                  m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X),
                                                  m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA));
  if (!m_evaluatedPeaksModel.appendEntry(evpeak)) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add peak to GUI list"));
    return;
  }

  if (answer.registerInHF) {
    double mobility;

    switch (answer.mobilityFrom) {
    case AddPeakDialog::MobilityFrom::HVL_A1:
      mobility = m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1);
      break;
    case AddPeakDialog::MobilityFrom::PEAK_MAXIMUM:
      mobility = m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF);
      break;
    default:
      mobility = std::numeric_limits<double>::infinity();
      break;
    }

    emit registerMeasurement(answer.name, m_commonParamsEngine->value(CommonParametersItems::Floating::SELECTOR),
                             mobility);
  }

  m_currentPeakIdx = m_allPeaks.length() - 1;
}

void EvaluationEngine::onCancelEvaluatedPeakSelection()
{
  m_currentPeak = duplicatePeakContext();
  m_currentPeakIdx = 0;

  setPeakContext(m_currentPeak);
}

void EvaluationEngine::onCloseCurrentEvaluationFile(const int idx)
{
  int newIdx;
  QString oldKey(m_currentDataContextKey);

  if (m_allDataContexts.size() == 0)
    return;
  else if (m_allDataContexts.size() == 1) {
    m_loadedFilesModel.deleteByIdx(idx);
    newIdx = 0;
    m_currentDataContext = std::shared_ptr<DataContext>(new DataContext(nullptr, "", m_commonParamsEngine->currentContext(), currentEvaluationContext()));
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

  m_commonParamsEngine->setContext(m_currentDataContext->commonContext);
  setEvaluationContext(m_currentDataContext->evaluationContext);

  m_allDataContexts.remove(oldKey);
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

void EvaluationEngine::onDataLoaded(std::shared_ptr<DataFileLoader::Data> data, const QString &fullPath, const QString &fileName)
{
  if (m_allDataContexts.contains(fullPath)) {
    QMessageBox::warning(nullptr, tr("Data exist"), tr("This file is already loaded"));
    int idx = m_loadedFilesModel.indexByItem(fullPath);
    if (idx >= 0) {
      emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::DATA_FILES, idx));
      switchEvaluationContext(fullPath);
    }
    return;
  }

  storeCurrentContext();

  std::shared_ptr<DataContext> ctx = std::shared_ptr<DataContext>(new DataContext(data, fileName, m_commonParamsEngine->currentContext(),
                                                                                  freshEvaluationContext()));

  try {
    m_allDataContexts.insert(fullPath, ctx);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Data processing error"), tr("Unable to store data in database"));
    return;
  }

  if (!m_loadedFilesModel.appendEntry(ComboBoxItem<QString>(fullPath, fullPath)))
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Unable to add new entry to the list of loaded files"));
  else
    emit evaluationFileAdded(m_loadedFilesModel.rowCount() - 1);

  m_currentDataContext = ctx;
  m_currentDataContextKey = fullPath;
  setEvaluationContext(ctx->evaluationContext);
  m_commonParamsEngine->revalidate();
  m_evaluatedPeaksModel.clearEntries();
}

void EvaluationEngine::onDeletePeak(const QModelIndex &idx)
{
  Q_UNUSED(idx);

  if (m_currentPeakIdx == 0)
    return;

  m_allPeaks.remove(m_currentPeakIdx);
  m_evaluatedPeaksModel.removeEntry(m_currentPeakIdx - 1);

  m_currentPeakIdx--;
  m_currentPeak = m_allPeaks.at(m_currentPeakIdx);

  setPeakContext(m_currentPeak);
}

void EvaluationEngine::onDoHvlFit()
{
  if (!isContextValid())
    return;

  /* Peak has no meaningful evaluation resutls,
   * do not add it */
  if (!m_currentPeak.finderResults.isValid())
    return;

  if (!HVLCalculator::available())
    return;

  HVLCalculator::HVLParameters p = HVLCalculator::fit(
    m_currentDataContext->data->data,
    m_currentPeak.finderResults.fromPeakIndex, m_currentPeak.finderResults.toPeakIndex,
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A0),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A1),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A2),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A3),
    m_currentPeak.finderResults.baselineIntercept,
    m_currentPeak.finderResults.baselineSlope,
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_EPSILON),
    m_hvlFitIntValues.at(HVLFitParametersItems::Int::ITERATIONS),
    m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS)
  );

  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A0] = p.a0;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A1] = p.a1;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A2] = p.a2;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A3] = p.a3;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_CHI_SQUARED] = p.chiSquared;

  emit m_hvlFitModel.dataChanged(
    m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A0)),
    m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_CHI_SQUARED))
   );

  onReplotHvl();
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

void EvaluationEngine::onFindPeaks()
{
  findPeak(false);
}

void EvaluationEngine::onHvlResultsModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  const int from = topLeft.column();
  const int to = bottomRight.column();

  if (!isContextValid())
    return;

  if (from <= m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A1) &&
      to >= m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A1)) {
    m_hvlFitValues[HVLFitResultsItems::Floating::HVL_U_EFF_A1] = calculateA1Mobility(m_hvlFitValues, m_commonParamsEngine->currentContext().data);

    emit m_hvlFitModel.dataChanged(m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_U_EFF_A1)),
                                   m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_U_EFF_A1)),
                                   roles);
  }
}

void EvaluationEngine::onPeakSwitched(const QModelIndex &idx)
{
  int row;
  disconnectPeakUpdate();

  if (!idx.isValid()) {
    m_currentPeakIdx = 0;
    m_currentPeak = m_allPeaks.at(0);
    setPeakContext(m_currentPeak);
    goto out;
  }

  row = idx.row() + 1;
  if (row < 1 || row >= m_allPeaks.length())
    goto out;

  m_currentPeakIdx = row;
  m_currentPeak = m_allPeaks.at(row);
  setPeakContext(m_currentPeak);

out:
  connectPeakUpdate();
}

void EvaluationEngine::onPlotPointSelected(const QPointF &point, const QPoint &cursor)
{
  if (!isContextValid())
    return;

  QAction *trig = m_contextMenu->exec(cursor);
  if (trig == nullptr)
    return;

  contextMenuTriggered(trig->data().value<ContextMenuActions>(), point);
}

void EvaluationEngine::onProvisionalPeakSelected(const QModelIndex index, const QAbstractItemModel *model, const long peakWindow)
{
  bool ok;

  if (!isContextValid())
    return;

  if (!index.isValid())
    return;

  if (model == nullptr)
    return;

  int idx = model->data(model->index(index.row(), 0), Qt::UserRole + 1).toInt(&ok);
  if (!ok)
    return;

  QVector<QPointF> data;
  const QVector<QPointF> &signalData = m_currentDataContext->data->data;
  int halfPWin = peakWindow / 2;
  int start = (idx - halfPWin) >= 0 ? idx - halfPWin : 0;
  int end = (idx + halfPWin) >= (signalData.length() - 1) ? signalData.length() - 1 : idx + halfPWin;
  for (int idx = start; idx < end; idx++)
    data.push_back(m_currentDataContext->data->data.at(idx));

  m_modeCtx->setSerieSamples(seriesIndex(Series::PROV_PEAK), data);
  m_modeCtx->replot();
}

void EvaluationEngine::onReadEof()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults.isValid())
    return;

  double tEOF = m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X);
  emit updateTEof(tEOF);
}

void EvaluationEngine::onReplotHvl()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults.isValid())
    return;

  if (!HVLCalculator::available())
    return;

  QVector<QPointF> vec = HVLCalculator::plot(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2),
                                             m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3),
                                             m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X),
                                             m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_TO_X),
                                             timeStep(),
                                             m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS));
  HVLCalculator::applyBaseline(vec, m_currentPeak.finderResults.baselineSlope, m_currentPeak.finderResults.baselineIntercept);
  m_modeCtx->setSerieSamples(seriesIndex(Series::HVL), vec);

  m_modeCtx->replot();
}

void EvaluationEngine::onSetDefault(EvaluationEngineMsgs::Default msg)
{
  switch (msg) {
  case EvaluationEngineMsgs::Default::FINDER_PARAMETERS:
    return setDefaultFinderParameters();
    break;
  case EvaluationEngineMsgs::Default::PEAK_PROPERTIES:
    return setDefaultPeakProperties();
    break;
  }
}

void EvaluationEngine::onUnhighlightProvisionalPeak()
{
  m_modeCtx->clearSerieSamples(seriesIndex(Series::PROV_PEAK));
  m_modeCtx->replot();
}

void EvaluationEngine::onUpdateCurrentPeak()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults.isValid())
    return;

  findPeak(true);
}

void EvaluationEngine::plotEvaluatedPeak(const PeakFinder::Results &fr)
{
  /* Mark the EOF  */
  {
    double tEOF = m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF);

    if (tEOF > 0.0) {
      QVector<QPointF> vec;

      vec.push_back(QPointF(tEOF, fr.minY));
      vec.push_back(QPointF(tEOF, fr.maxY));
      m_modeCtx->setSerieSamples(seriesIndex(Series::EOF_MARK), vec);
    }
  }

  /* Draw the baseline */
  {
    QVector<QPointF> blVec;

    blVec.push_back(QPointF(fr.peakFromX, fr.peakFromY));
    blVec.push_back(QPointF(fr.peakToX, fr.peakToY));

    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE), blVec);
  }

  /* Mark the maximum of the peak */
  {
    QVector<QPointF> tpVec;
    QPointF b;

    tpVec.push_back(QPointF(fr.peakX, fr.minY));
    if (fr.indexAtMax < m_currentDataContext->data->data.length())
      b = QPointF(fr.peakX, m_currentDataContext->data->data.at(fr.indexAtMax).y());
    else
      b = QPointF(fr.peakX, fr.maxY);

    tpVec.push_back(b);
    m_modeCtx->setSerieSamples(seriesIndex(Series::PEAK_TIME), tpVec);
  }

  /* Mark the height of the peak */
  {
    QVector<QPointF> hpVec;

    hpVec.push_back(QPointF(fr.twPLeft, fr.peakHeight));
    hpVec.push_back(QPointF(fr.twPRight, fr.peakHeight));

    m_modeCtx->setSerieSamples(seriesIndex(Series::PEAK_HEIGHT), hpVec);
  }

  if (m_currentPeak.showWindow != EvaluationParametersItems::ComboShowWindow::NONE) {
    m_modeCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_A), *fr.seriesA);
    m_modeCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_B), *fr.seriesB);
  }

  /* HVL estimate */
  m_modeCtx->setSerieSamples(seriesIndex(Series::HVL), m_currentPeak.hvlPlot);

  /* Mark the beginning and the end of the peak */
  {
    QVector<QPointF> blFrom;
    QVector<QPointF> blTo;

    blFrom.push_back(QPointF(fr.peakFromX, fr.peakFromY));
    blFrom.push_back(QPointF(fr.peakFromX, m_currentDataContext->data->data.at(fr.fromPeakIndex).y()));

    blTo.push_back(QPointF(fr.peakToX, fr.peakToY));
    blTo.push_back(QPointF(fr.peakToX, m_currentDataContext->data->data.at(fr.toPeakIndex).y()));

    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE_FROM), blFrom);
    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE_TO), blTo);
  }

  m_modeCtx->replot();
}

AbstractMapperModel<double, EvaluationResultsItems::Floating> *EvaluationEngine::resultsValuesModel()
{
  return &m_resultsFloatingModel;
}

QVariant EvaluationEngine::saveUserSettings() const
{
  EMT::StringVariantMap map = StandardModeContextSettingsHandler::saveUserSettings(*m_modeCtx.get(), seriesIndex(Series::LAST_INDEX));
  map[DATAFILELOADER_SETTINGS_TAG] = m_dataFileLoader->saveUserSettings();

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

int EvaluationEngine::seriesIndex(const Series iid)
{
  return static_cast<int>(iid);
}

void EvaluationEngine::setDefaultFinderParameters()
{
  for (int idx = 0; idx < m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::LAST_INDEX); idx++)
      m_evaluationFloatingValues.setItemAt(idx, s_defaultEvaluationFloatingValues.at(idx));
  m_evaluationFloatingModel.dataChanged(m_evaluationFloatingModel.index(0,0),
                                        m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::FROM)),
                                        { Qt::EditRole });

  for (int idx = 0; idx < m_evaluationBooleanModel.indexFromItem(EvaluationParametersItems::Boolean::LAST_INDEX); idx++)
    m_evaluationBooleanValues.setItemAt(idx, s_defaultEvaluationBooleanValues.at(idx));
  m_evaluationBooleanModel.dataChanged(m_evaluationBooleanModel.index(0, 0),
                                       m_evaluationBooleanModel.index(0, m_evaluationBooleanModel.indexFromItem(EvaluationParametersItems::Boolean::LAST_INDEX) - 1),
                                       { Qt::EditRole });

  for (int idx = m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW);
       idx < m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::LAST_INDEX);
       idx++)
    m_evaluationAutoValues.setItemAt(idx, s_defaultEvaluationAutoValues.at(idx));
  m_evaluationAutoModel.dataChanged(m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW)),
                                    m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::LAST_INDEX) - 1),
                                    { Qt::EditRole });

  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::BASELINE_ALGORITHM,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboBaselineAlgorithm::SLOPE)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::SHOW_WINDOW,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboShowWindow::NONE)));
  emit comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier(EvaluationEngineMsgs::ComboBox::WINDOW_UNITS,
                                                                   EvaluationParametersItems::index(EvaluationParametersItems::ComboWindowUnits::MINUTES)));
}

void EvaluationEngine::setDefaultPeakProperties()
{
  for (int idx = 0; idx < m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::SLOPE_WINDOW); idx++)
    m_evaluationAutoValues.setItemAt(idx, s_defaultEvaluationAutoValues.at(idx));
  m_evaluationAutoModel.dataChanged(m_evaluationAutoModel.index(0, 0),
                                    m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::PEAK_TO_Y)),
                                    { Qt::EditRole });

}

bool EvaluationEngine::setEvaluationContext(const EvaluationContext &ctx)
{
  m_allPeaks = ctx.peaks;
  m_currentPeakIdx = ctx.lastIndex;

  if (m_currentPeakIdx < 0 || m_currentPeakIdx >= m_allPeaks.length())
    return false;

  m_currentPeak = m_allPeaks.at(m_currentPeakIdx);

  m_modeCtx->clearAllSerieSamples();
  setPeakContext(m_currentPeak);

  return createSignalPlot(m_currentDataContext->data, m_currentDataContext->name);
}

void EvaluationEngine::setEvaluationResults(PeakFinder::Results fr, PeakEvaluator::Results er)
{
  /* Results from peak evaluator */
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_FULL] = er.nFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_LEFT] = er.nLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_RIGHT] = er.nRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_H_FULL] = er.nHFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_H_LEFT] = er.nHLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::N_H_RIGHT] = er.nHRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_AREA] = er.peakArea;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MIN_FULL] = er.sigmaHalfFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MIN_LEFT] = er.sigmaHalfLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MIN_RIGHT] = er.sigmaHalfRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MET_FULL] = er.sigmaHalfMFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MET_LEFT] = er.sigmaHalfMLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::SIGMA_MET_RIGHT] = er.sigmaHalfMRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::EOF_MOBILITY] = er.uEOF;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_MOBILITY] = er.uP;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF] = er.uP_Eff;
  m_resultsNumericValues[EvaluationResultsItems::Floating::EOF_VELOCITY] = er.vEOF;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_VELOCITY] = er.vP;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF] = er.vP_Eff;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_FULL] = er.widthHalfFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT] = er.widthHalfLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT] = er.widthHalfRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_FULL] = er.widthHalfMFull;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_LEFT] = er.widthHalfMLeft;
  m_resultsNumericValues[EvaluationResultsItems::Floating::WIDTH_HALF_MET_RIGHT] = er.widthHalfMRight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_FROM_X] = fr.peakFromX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_FROM_Y] = fr.peakFromY;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_TO_X] = fr.peakToX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_TO_Y] = fr.peakToY;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_X] = fr.peakX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_HEIGHT] = fr.peakHeight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_HEIGHT_BL] = fr.peakHeightBaseline;

  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A0] = er.peakArea;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A1] = er.HVL_a1;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A2] = er.HVL_a2;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A3] = er.HVL_a3;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_TUSP] = er.HVL_tUSP;


  emit m_resultsFloatingModel.dataChanged(m_resultsFloatingModel.index(0, 0),
                                          m_resultsFloatingModel.index(0, m_resultsFloatingModel.indexFromItem(EvaluationResultsItems::Floating::LAST_INDEX)));
  emit m_hvlFitModel.dataChanged(m_hvlFitModel.index(0, 0),
                                 m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX)));
}

bool EvaluationEngine::setPeakContext(const PeakContext &ctx)
{
  m_evaluationAutoValues = ctx.autoValues;
  m_evaluationBooleanValues = ctx.boolValues;
  m_evaluationFloatingValues = ctx.floatingValues;
  m_resultsNumericValues = ctx.resultsValues;
  m_windowUnit = ctx.windowUnit;
  m_showWindow = ctx.showWindow;
  m_baselineAlgorithm = ctx.baselineAlgorithm;

  clearPeakPlots();
  fullViewUpdate();
  if (ctx.finderResults.isValid() && (m_currentDataContext->data != nullptr))
    plotEvaluatedPeak(ctx.finderResults);

  return true;
}

QAbstractItemModel *EvaluationEngine::showWindowModel()
{
  return &m_showWindowModel;
}

bool EvaluationEngine::storeCurrentContext()
{
  /* Do not store the default empty context */
  if (isContextValid()) {
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

void EvaluationEngine::switchEvaluationContext(const QString &key)
{
  storeCurrentContext();

  if (!m_allDataContexts.contains(key)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Data context for key %1 not found!")).arg(key));
    return;
  }

  m_currentDataContext = m_allDataContexts[key];
  m_currentDataContextKey = key;

  disconnectPeakUpdate();
  m_commonParamsEngine->setContext(m_currentDataContext->commonContext);
  setEvaluationContext(m_currentDataContext->evaluationContext);
  m_evaluatedPeaksModel.setEntries(makeEvaluatedPeaks());
  connectPeakUpdate();
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

  QModelIndex nwIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::NOISE_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(nwIdx, nwIdx, { Qt::EditRole });

  QModelIndex pwIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(pwIdx, pwIdx, { Qt::EditRole });

  QModelIndex swIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::SLOPE_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(swIdx, swIdx, { Qt::EditRole });

}

double EvaluationEngine::timeStep()
{
  if (!isContextValid())
    return 1.0;

  if (m_currentDataContext->data == nullptr)
    return 1.0;

  if (m_currentDataContext->data->data.length() == 0)
    return 1.0;

  double dt = m_currentDataContext->data->data.at(1).x() - m_currentDataContext->data->data.at(0).x();

  return dt;
}

QAbstractItemModel *EvaluationEngine::windowUnitsModel()
{
  return &m_windowUnitsModel;
}
