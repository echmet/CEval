#include "setup.h"

#include "evaluationengine.h"
#include "doubletostringconvertor.h"
#include "gui/addpeakdialog.h"
#include "gui/setaxistitlesdialog.h"
#include "custommetatypes.h"
#include "helpers.h"
#include "manualpeakfinder.h"
#include "standardmodecontextsettingshandler.h"
#include <QApplication>
#include <QClipboard>
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

const QVector<bool> EvaluationEngine::s_defaultEvaluationBooleanValues({false /* NOISE_CORRECTION */});

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
                                                                          0.0  /* TO */
                                                                          });

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

EvaluationEngine::EvaluationContext::EvaluationContext(const QVector<PeakContext> &peaks, const int lastIndex,
                                                       const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &afAutoValues,
                                                       const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &afBoolValues,
                                                       const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &afFloatingValues) :
  peaks(peaks),
  lastIndex(lastIndex),
  afAutoValues(afAutoValues),
  afBoolValues(afBoolValues),
  afFloatingValues(afFloatingValues)
{
}

EvaluationEngine::EvaluationContext &EvaluationEngine::EvaluationContext::operator=(const EvaluationContext &other)
{
  const_cast<QVector<PeakContext>&>(peaks) = other.peaks;
  const_cast<int&>(lastIndex) = other.lastIndex;

  return *this;
}

EvaluationEngine::EvaluationEngine(CommonParametersEngine *commonParamsEngine, QObject *parent) : QObject(parent),
  m_userInteractionState(UserInteractionState::FINDING_PEAK),
  m_showHvlFitStats(false),
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
    createContextMenus();
  } catch (std::bad_alloc&) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Unable to allocate context menus"));
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
  connect(&m_hvlFitIntModel, &IntegerMapperModel<HVLFitParametersItems::Int>::dataChanged, this, &EvaluationEngine::onHvlParametersModelChanged);
  connect(&m_hvlFixedModel, &BooleanMapperModel<HVLFitParametersItems::Boolean>::dataChanged, this, &EvaluationEngine::onHvlParametersModelChanged);

  m_addPeakDlg = new AddPeakDialog();

  m_currentPeakIdx = 0;
  m_allPeaks.push_back(freshPeakContext());
  m_currentDataContext = std::shared_ptr<DataContext>(new DataContext(nullptr, s_emptyCtxKey, m_commonParamsEngine->currentContext(),
                                                                      currentEvaluationContext()));
  m_currentDataContextKey = s_emptyCtxKey;

  connect(this, &EvaluationEngine::updateTEof, m_commonParamsEngine, &CommonParametersEngine::onUpdateTEof);
  connect(m_commonParamsEngine, &CommonParametersEngine::tEofUpdated, this, &EvaluationEngine::onUpdateCurrentPeak);
}

EvaluationEngine::~EvaluationEngine()
{
  delete m_addPeakDlg;
  delete m_findPeakMenu;
  delete m_manualIntegrationMenu;
  delete m_postProcessMenu;
}

void EvaluationEngine::assignContext(std::shared_ptr<ModeContextLimited> ctx)
{
  m_modeCtx = ctx;

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

  if (!m_modeCtx->addSerie(seriesIndex(Series::PROV_BASELINE), s_serieProvisionalBaseline, SerieProperties::VisualStyle(QPen(QBrush(QColor(23, 73, 255), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_serieProvisionalBaseline));

  connect(m_modeCtx.get(), &ModeContextLimited::pointHovered, this, &EvaluationEngine::onPlotPointHovered);
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

void EvaluationEngine::createContextMenus() throw(std::bad_alloc)
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

  a = new QAction(tr("Set noise reference point"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::NOISE_REF_POINT));
  m_findPeakMenu->addAction(a);

  a = new QAction(tr("Set slope reference point"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SLOPE_REF_POINT));
  m_findPeakMenu->addAction(a);

  m_findPeakMenu->addSeparator();

  a = new QAction(tr("Set axis titles"), m_findPeakMenu);
  a->setData(QVariant::fromValue<FindPeakMenuActions>(FindPeakMenuActions::SET_AXIS_TITLES));
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

  a = new QAction(tr("Set axis titles"), m_postProcessMenu);
  a->setData(QVariant::fromValue<PostProcessMenuActions>(PostProcessMenuActions::SET_AXIS_TITLES));
  m_postProcessMenu->addAction(a);
}

bool EvaluationEngine::createSignalPlot(std::shared_ptr<DataFileLoader::Data> data, const QString &name)
{
  m_modeCtx->setPlotTitle(name);

  if (data == nullptr) {
    m_modeCtx->clearAllSerieSamples();
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "");
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, "");
  } else{
    m_modeCtx->setSerieSamples(seriesIndex(Series::SIG), data->data);
    setAxisTitles();
  }

  m_modeCtx->replot();

  return true;
}

EvaluationEngine::EvaluationContext EvaluationEngine::currentEvaluationContext() const
{
  /* No peak has been added but the evaluation parameters might still have changed.
   * Make sure we store them all */
  QVector<PeakContext> allPeaks(m_allPeaks);
  if (m_currentPeakIdx == 0) {
    try {
      allPeaks[0] = duplicatePeakContext();
    } catch (std::bad_alloc &) {
      QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Insufficient memory to execute EvaluationEngine::duplicatePeakContext(). Application may misbehave."));
    }
  }

  return EvaluationContext(allPeaks, m_currentPeakIdx,
                           m_evaluationAutoValues,
                           m_evaluationBooleanValues,
                           m_evaluationFloatingValues);
}

EvaluationEngine::PeakContext EvaluationEngine::currentPeakContext(const std::shared_ptr<PeakFinderResults> &finderResults,
                                                                   const int peakIndex, const double baselineSlope, const double baselineIntercept,
                                                                   const QVector<QPointF> &hvlPlot) const
{
  return PeakContext(m_resultsNumericValues,
                     m_hvlFitValues, m_hvlFitIntValues, m_hvlFitFixedValues,
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     finderResults, peakIndex, baselineSlope, baselineIntercept, hvlPlot);
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

EvaluationEngine::PeakContext EvaluationEngine::duplicatePeakContext() const throw(std::bad_alloc)
{
  /* Despite its name this function only copies the parameters of current
   * peak context, the results are created empty */
  return PeakContext(MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     m_hvlFitIntValues,
                     MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>(defaultHvlFixedValues()),
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     std::make_shared<PeakFinderResults>(),
                     -1, 0.0, 0.0, QVector<QPointF>());
}

void EvaluationEngine::displayAutomatedResults(const std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResults> &fr)
{
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::NOISE] = fr->noise;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_WINDOW] = fr->slopeWindow;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_THRESHOLD] = fr->slopeThreshold;
  m_evaluationFloatingValues[EvaluationParametersItems::Floating::SLOPE_REF_POINT] = fr->slopeRefPoint;

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

void EvaluationEngine::findPeakAssisted()
{
  SelectPeakDialog dialog;
  std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResults> fr;

  if (!isContextValid())
    return;

  if (m_userInteractionState != UserInteractionState::FINDING_PEAK) {
    QMessageBox::information(nullptr, tr("Peak already selected"), tr("A peak is already selected. Please cancel current selection before finding a new peak."));
    return;
  }

  if (m_currentDataContext->data->data.length() == 0)
    return;

  AssistedPeakFinder::Parameters fp = makeFinderParameters();
  fp.selPeakDialog = &dialog;

  connect(&dialog, &SelectPeakDialog::listClicked, this, &EvaluationEngine::onProvisionalPeakSelected);
  connect(&dialog, &SelectPeakDialog::closedSignal, this, &EvaluationEngine::onUnhighlightProvisionalPeak);

  try {
    fr = std::static_pointer_cast<AssistedPeakFinder::AssistedPeakFinderResults>(AssistedPeakFinder::find(fp));
  } catch (std::bad_alloc &) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Not enough memory to find peaks"));
    return;
  }

  if (!fr->isValid()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    clearPeakPlots();
    return;
  }

  displayAutomatedResults(fr);

  processFoundPeak(m_currentDataContext->data->data, fr, false);
}

void EvaluationEngine::findPeakManually(const QPointF &from, const QPointF &to, const bool snapFrom, const bool snapTo)
{
  std::shared_ptr<PeakFinderResults> fr;

  /* Erase the provisional baseline */
  m_modeCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), QVector<QPointF>());

  if (!isContextValid()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_modeCtx->replot();
    return;
  }

  if (m_currentDataContext->data->data.length() == 0) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_modeCtx->replot();
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

  if (!fr->isValid())
    goto err_out;

  processFoundPeak(m_currentDataContext->data->data, fr, (m_userInteractionState == UserInteractionState::PEAK_POSTPROCESSING ? true : false));
  return;

err_out:
  m_userInteractionState = UserInteractionState::FINDING_PEAK;
  m_modeCtx->replot();
}

void EvaluationEngine::findPeakMenuTriggered(const FindPeakMenuActions &action, const QPointF &point)
{
  QModelIndex autoFrom;
  QModelIndex autoTo;
  QModelIndex valueFrom;
  QModelIndex valueTo;

  switch (action) {
  case FindPeakMenuActions::PEAK_FROM_HERE:
    m_manualPeakSnapFrom = false;
    m_manualPeakFrom = point;
    m_userInteractionState = UserInteractionState::MANUAL_PEAK_INTEGRATION;
    break;
  case FindPeakMenuActions::PEAK_FROM_HERE_SIGSNAP:
    try {
      m_manualPeakFrom = QPointF(point.x(), Helpers::yForX(point.x(), m_currentDataContext->data->data));
    } catch (std::out_of_range &) {
      return;
    }
    m_manualPeakSnapFrom = true;
    m_userInteractionState = UserInteractionState::MANUAL_PEAK_INTEGRATION;
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
  }

  if (autoFrom.isValid())
    emit m_evaluationAutoModel.dataChanged(autoFrom, autoTo, { Qt::EditRole });
  if (valueFrom.isValid())
    emit m_evaluationFloatingModel.dataChanged(valueFrom, valueTo, { Qt::EditRole });
}

EvaluationEngine::EvaluationContext EvaluationEngine::freshEvaluationContext() const
{
  QVector<PeakContext> fresh;

  try {
    fresh.push_back(duplicatePeakContext());
  } catch (std::bad_alloc &) {
    QMessageBox::critical(nullptr, tr("Insufficient memory"), tr("Insufficient memory to execute EvaluationEngine::freshEvaluationContext(). Application cannon continue."));
    Helpers::execCFIT();
  }

  return EvaluationContext(fresh, 0,
                           MappedVectorWrapper<bool, EvaluationParametersItems::Auto>(s_defaultEvaluationAutoValues),
                           MappedVectorWrapper<bool, EvaluationParametersItems::Boolean>(s_defaultEvaluationBooleanValues),
                           MappedVectorWrapper<double, EvaluationParametersItems::Floating>(s_defaultEvaluationFloatingValues));
}

EvaluationEngine::PeakContext EvaluationEngine::freshPeakContext() const throw(std::bad_alloc)
{
  return PeakContext(MappedVectorWrapper<double, EvaluationResultsItems::Floating>(emptyResultsValues()),
                     MappedVectorWrapper<double, HVLFitResultsItems::Floating>(emptyHvlValues()),
                     MappedVectorWrapper<int, HVLFitParametersItems::Int>(defaultHvlIntValues()),
                     MappedVectorWrapper<bool, HVLFitParametersItems::Boolean>(defaultHvlFixedValues()),
                     m_windowUnit, m_showWindow, m_baselineAlgorithm,
                     std::make_shared<PeakFinderResults>(),
                     -1, 0.0, 0.0, QVector<QPointF>());
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

  emit m_hvlFitModel.dataChanged(m_hvlFitModel.index(0, 0),
                                 m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::LAST_INDEX)));

  emit m_hvlFixedModel.dataChanged(m_hvlFixedModel.index(0, 0),
                                   m_hvlFixedModel.index(0, m_hvlFixedModel.indexFromItem(HVLFitParametersItems::Boolean::LAST_INDEX)));

  emit m_hvlFitIntModel.dataChanged(m_hvlFitIntModel.index(0, 0),
                                    m_hvlFitIntModel.index(0, m_hvlFitIntModel.indexFromItem(HVLFitParametersItems::Int::LAST_INDEX)));

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

PeakEvaluator::Parameters EvaluationEngine::makeEvaluatorParameters(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults> &fr)
{
  PeakEvaluator::Parameters p(data);

  p.capillary = m_commonParamsEngine->value(CommonParametersItems::Floating::CAPILLARY);
  p.detector = m_commonParamsEngine->value(CommonParametersItems::Floating::DETECTOR);
  p.E = m_commonParamsEngine->value(CommonParametersItems::Floating::FIELD);
  p.fromIndex = fr->fromIndex;
  p.toIndex = fr->toIndex;
  p.tEOF = m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF);
  p.fromX = fr->peakFromX;
  p.fromY = fr->peakFromY;
  p.toX = fr->peakToX;
  p.toY = fr->peakToY;
  p.voltage = m_commonParamsEngine->value(CommonParametersItems::Floating::VOLTAGE);

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
  p.tEOF = m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF);
  p.to = m_evaluationFloatingValues.at(EvaluationParametersItems::Floating::TO);
  p.windowUnits = m_windowUnit;

  return p;
}

void EvaluationEngine::manualIntegrationMenuTriggered(const ManualIntegrationMenuActions &action, const QPointF &point)
{
  switch (action) {
  case ManualIntegrationMenuActions::CANCEL:
    m_modeCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), QVector<QPointF>());
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    m_modeCtx->replot();
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

  /* Peak has no meaningful evaluation resutls,
   * do not add it */
  if (!m_currentPeak.finderResults->isValid())
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
  try {
    m_currentPeak = duplicatePeakContext();
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Insufficent memory"), tr("Insufficient memory to execute EvaluationEngine::duplicatePeakContext(). Application may misbehave."));
  }

  m_currentPeakIdx = 0;

  setPeakContext(m_currentPeak);

  m_userInteractionState = UserInteractionState::FINDING_PEAK;
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

void EvaluationEngine::onCopyToClipboard(const EvaluationEngineMsgs::CopyToClipboard ctc)
{
  QClipboard *clipboard = QApplication::clipboard();
  QString out;
  QTextStream toCopy(&out, QIODevice::WriteOnly);

  if (!isContextValid())
    return;

  toCopy.setCodec("UTF-8");

  switch (ctc) {
  case EvaluationEngineMsgs::CopyToClipboard::EOFLOW:
    toCopy << "v (1e-3 m/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY)) << "\n";
    toCopy << "v! (1e-3 m/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_VELOCITY_EFF)) << "\n";
    toCopy << "u (1e-9 m.m/V/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::EOF_MOBILITY)) << "\n";
    break;
  case EvaluationEngineMsgs::CopyToClipboard::HVL:
    toCopy << "a0;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0)) << "\n";
    toCopy << "a1;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1)) << "\n";
    toCopy << "a2;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2)) << "\n";
    toCopy << "a3;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3)) << "\n";
    toCopy << "S;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_S)) << "\n";
    toCopy << "a1 u! (1e-9 m.m/V/s);" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_U_EFF_A1)) << "\n";
    break;
  case EvaluationEngineMsgs::CopyToClipboard::PEAK:
    toCopy << "Peak from X;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_X)) << "\n";
    toCopy << "Peak from Y;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_FROM_Y)) << "\n";
    toCopy << "Peak to X;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_X)) << "\n";
    toCopy << "Peak to Y;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_TO_Y)) << "\n";
    toCopy << "Peak max at X;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X)) << "\n";
    toCopy << "Peak height;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_HEIGHT_BL)) << "\n";
    toCopy << "v (1e-3 m/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY)) << "\n";
    toCopy << "v! (1e-3 m/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_VELOCITY_EFF)) << "\n";
    toCopy << "u! (1e-9 m.m/V/s);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_MOBILITY_EFF)) << "\n";
    toCopy << "Area (Units.min);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_AREA)) << "\n";
    toCopy << "t USP;" << DoubleToStringConvertor::convert(m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_TUSP)) << "\n";
    break;
  case EvaluationEngineMsgs::CopyToClipboard::PEAK_DIMS:
    toCopy << ";Left;Right;Full\n";
    toCopy << "Width 1/2 (min);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_LEFT)) << ";"
                                 << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_RIGHT)) << ";"
                                 << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MIN_FULL)) << "\n";
    toCopy << "Sigma (min);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MIN_LEFT)) << ";"
                            << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MIN_RIGHT)) << ";"
                            << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MIN_FULL)) << "\n";
    toCopy << "Width 1/2 (m);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MET_LEFT)) << ";"
                              << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MET_RIGHT)) << ";"
                              << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::WIDTH_HALF_MET_FULL)) << "\n";
    toCopy << "Sigma (m);" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MET_LEFT)) << ";"
                           << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MET_RIGHT)) << ";"
                           << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::SIGMA_MET_FULL)) << "\n";
    toCopy << "N;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_LEFT)) << ";"
                   << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_RIGHT)) << ";"
                   << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_FULL)) << "\n";
    toCopy << "HETP;" << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_H_LEFT)) << ";"
                      << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_H_RIGHT)) << ";"
                      << DoubleToStringConvertor::convert(m_resultsNumericValues.at(EvaluationResultsItems::Floating::N_H_FULL)) << "\n";
    break;
  }

  clipboard->setText(out);
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

  if (ctx->data->data.length() > 0)
    setPeakFinderParameters(ctx->data->data.last().x());
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
  if (!m_currentPeak.finderResults->isValid())
    return;

  if (!HVLCalculator::available())
    return;

  HVLCalculator::HVLParameters p = HVLCalculator::fit(
    m_currentDataContext->data->data,
    m_currentPeak.finderResults->fromIndex, m_currentPeak.finderResults->toIndex,
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A0),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A1),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A2),
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_A3),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A0),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A1),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A2),
    m_hvlFitFixedValues.at(HVLFitParametersItems::Boolean::HVL_A3),
    m_currentPeak.baselineIntercept,
    m_currentPeak.baselineSlope,
    m_hvlFitValues.at(HVLFitResultsItems::Floating::HVL_EPSILON),
    m_hvlFitIntValues.at(HVLFitParametersItems::Int::ITERATIONS),
    m_hvlFitIntValues.at(HVLFitParametersItems::Int::DIGITS),
    m_showHvlFitStats
  );

  if (!p.isValid())
    return;

  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A0] = p.a0;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A1] = p.a1;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A2] = p.a2;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_A3] = p.a3;
  m_hvlFitValues[HVLFitResultsItems::Floating::HVL_S] = p.s;

  emit m_hvlFitModel.dataChanged(
    m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_A0)),
    m_hvlFitModel.index(0, m_hvlFitModel.indexFromItem(HVLFitResultsItems::Floating::HVL_S))
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
  findPeakAssisted();
}


void EvaluationEngine::onHvlParametersModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles)
{
  Q_UNUSED(topLeft); Q_UNUSED(bottomRight); Q_UNUSED(roles)

  m_currentPeak.updateHvlData(m_hvlFitValues, m_hvlFitIntValues, m_hvlFitFixedValues);
  if (m_currentPeakIdx > 0)
    m_allPeaks[m_currentPeakIdx] = m_currentPeak;
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

  m_currentPeak.updateHvlData(m_hvlFitValues, m_hvlFitIntValues, m_hvlFitFixedValues);
  if (m_currentPeakIdx > 0)
    m_allPeaks[m_currentPeakIdx] = m_currentPeak;
}

void EvaluationEngine::onPeakSwitched(const QModelIndex &idx)
{
  int row;
  //disconnectPeakUpdate();

  if (!idx.isValid()) {
    m_currentPeakIdx = 0;
    m_currentPeak = m_allPeaks.at(0);
    setPeakContext(m_currentPeak);
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    return;
  }

  row = idx.row() + 1;
  if (row < 1 || row >= m_allPeaks.length())
    return;

  m_currentPeakIdx = row;
  m_currentPeak = m_allPeaks.at(row);
  setPeakContext(m_currentPeak);

  m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;

//out:
  //return;
  //connectPeakUpdate();
}

void EvaluationEngine::onPlotPointHovered(const QPointF &point, const QPoint &cursor)
{
  Q_UNUSED(cursor)

  if (!isContextValid())
    return;

  if (m_currentDataContext->data->data.size() < 2)
    return;

  if (m_userInteractionState == UserInteractionState::MANUAL_PEAK_INTEGRATION) {
    QVector<QPointF> line;
    double x;
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
    const double k = (ty - fy) / (tx - fx);
    const double step = (m_currentDataContext->data->data.at(1).x() - m_currentDataContext->data->data.at(0).x()) / 10.0;

    x = fx;
    while (x <= tx) {
      const double vf = k * (x - fx) + fy;

      line.push_back(QPointF(x, vf));
      x += step;
    }

    m_modeCtx->setSerieSamples(seriesIndex(Series::PROV_BASELINE), line);
    m_modeCtx->replot();
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

  if (!m_currentPeak.finderResults->isValid())
    return;

  double tEOF = m_currentPeak.resultsValues.at(EvaluationResultsItems::Floating::PEAK_X);
  emit updateTEof(tEOF);
}

void EvaluationEngine::onReplotHvl()
{
  if (!isContextValid())
    return;

  if (!m_currentPeak.finderResults->isValid())
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

  HVLCalculator::applyBaseline(vec, m_currentPeak.baselineSlope, m_currentPeak.baselineIntercept);
  m_currentPeak.updateHvlPlot(vec);

  m_modeCtx->setSerieSamples(seriesIndex(Series::HVL), vec);

  m_modeCtx->replot(false);
}

void EvaluationEngine::onSetDefault(EvaluationEngineMsgs::Default msg)
{
  switch (msg) {
  case EvaluationEngineMsgs::Default::FINDER_PARAMETERS:
    return setDefaultFinderParameters();
    break;
  }
}

void EvaluationEngine::onShowHvlFitStatsChanged(const bool show)
{
  m_showHvlFitStats = show;
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

  if (!m_currentPeak.finderResults->isValid())
    return;

  findPeakManually(QPointF(m_currentPeak.finderResults->peakFromX, m_currentPeak.finderResults->peakFromY),
                   QPointF(m_currentPeak.finderResults->peakToX, m_currentPeak.finderResults->peakToY),
                   false, false);
}

void EvaluationEngine::plotEvaluatedPeak(const std::shared_ptr<PeakFinderResults> fr, const double peakX,
                                         const double minY, const double maxY,
                                         const double widthHalfLeft, const double widthHalfRight,
                                         const double peakHeight, const double peakHeightBaseline)
{
  /* Mark the EOF  */
  {
    double tEOF = m_commonParamsEngine->value(CommonParametersItems::Floating::T_EOF);

    if (tEOF > 0.0) {
      QVector<QPointF> vec;

      vec.push_back(QPointF(tEOF, minY));
      vec.push_back(QPointF(tEOF, maxY));
      m_modeCtx->setSerieSamples(seriesIndex(Series::EOF_MARK), vec);
    }
  }

  /* Draw the baseline */
  {
    QVector<QPointF> blVec;

    blVec.push_back(QPointF(fr->peakFromX, fr->peakFromY));
    blVec.push_back(QPointF(fr->peakToX, fr->peakToY));

    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE), blVec);
  }

  /* Mark the maximum of the peak */
  {
    QVector<QPointF> tpVec;

    tpVec.push_back(QPointF(peakX, peakHeight - peakHeightBaseline));
    tpVec.push_back(QPointF(peakX, peakHeight));
    m_modeCtx->setSerieSamples(seriesIndex(Series::PEAK_TIME), tpVec);
  }

  /* Mark the height of the peak */
  {
    QVector<QPointF> hpVec;

    hpVec.push_back(QPointF(peakX - widthHalfLeft, peakHeight));
    hpVec.push_back(QPointF(widthHalfRight + peakX, peakHeight));

    m_modeCtx->setSerieSamples(seriesIndex(Series::PEAK_HEIGHT), hpVec);
  }

  {
    const std::shared_ptr<AssistedPeakFinder::AssistedPeakFinderResults> afr = std::dynamic_pointer_cast<AssistedPeakFinder::AssistedPeakFinderResults>(fr);

    if (afr != nullptr) {
      if (m_currentPeak.showWindow != EvaluationParametersItems::ComboShowWindow::NONE) {
        m_modeCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_A), *afr->seriesA);
        m_modeCtx->setSerieSamples(seriesIndex(Series::FINDER_SYSTEM_B), *afr->seriesB);
      }
    }
  }

  /* HVL estimate */
  m_modeCtx->setSerieSamples(seriesIndex(Series::HVL), m_currentPeak.hvlPlot);

  /* Mark the beginning and the end of the peak */
  {
    QVector<QPointF> blFrom;
    QVector<QPointF> blTo;

    blFrom.push_back(QPointF(fr->peakFromX, fr->peakFromY));
    blFrom.push_back(QPointF(fr->peakFromX, m_currentDataContext->data->data.at(fr->fromIndex).y()));

    blTo.push_back(QPointF(fr->peakToX, fr->peakToY));
    blTo.push_back(QPointF(fr->peakToX, m_currentDataContext->data->data.at(fr->toIndex).y()));

    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE_FROM), blFrom);
    m_modeCtx->setSerieSamples(seriesIndex(Series::BASELINE_TO), blTo);
  }

  m_modeCtx->replot(false);
}

void EvaluationEngine::postProcessMenuTriggered(const PostProcessMenuActions &action, const QPointF &point)
{
  QModelIndex autoFrom;
  QModelIndex autoTo;
  QModelIndex valueFrom;
  QModelIndex valueTo;

  switch (action) {
  case PostProcessMenuActions::MOVE_PEAK_FROM:
    findPeakManually(point, QPointF(m_currentPeak.finderResults->peakToX, m_currentPeak.finderResults->peakToY), false, false);
    break;
  case PostProcessMenuActions::MOVE_PEAK_FROM_SIGSNAP:
    findPeakManually(point, QPointF(m_currentPeak.finderResults->peakToX, m_currentPeak.finderResults->peakToY), true, false);
    break;
  case PostProcessMenuActions::MOVE_PEAK_TO:
    findPeakManually(QPointF(m_currentPeak.finderResults->peakFromX, m_currentPeak.finderResults->peakFromY), point, false, false);
    break;
  case PostProcessMenuActions::MOVE_PEAK_TO_SIGSNAP:
    findPeakManually(QPointF(m_currentPeak.finderResults->peakFromX, m_currentPeak.finderResults->peakFromY), point, false, true);
    break;
  case PostProcessMenuActions::DESELECT_PEAK:
    onCancelEvaluatedPeakSelection();
    break;
  case PostProcessMenuActions::SET_AXIS_TITLES:
    showSetAxisTitlesDialog();
    break;
  default:
    return;
    break;
  }
}

void EvaluationEngine::processFoundPeak(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults> &fr, const bool updateCurrentPeak)
{
  PeakEvaluator::Parameters ep = makeEvaluatorParameters(data, fr);
  PeakEvaluator::Results er = PeakEvaluator::evaluate(ep);

  if (!er.isValid()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    clearPeakPlots();
    return;
  }

  setEvaluationResults(fr, er);

  QVector<QPointF> hvlPlot;
  if (HVLCalculator::available()) {
    hvlPlot = HVLCalculator::plot(er.peakArea, er.HVL_a1, er.HVL_a2, er.HVL_a3, fr->peakFromX, fr->peakToX, timeStep(), 50);
    HVLCalculator::applyBaseline(hvlPlot, er.baselineSlope, er.baselineIntercept);
  }

  m_currentPeak = currentPeakContext(fr, er.peakIndex, er.baselineSlope, er.baselineIntercept, hvlPlot);
  onDoHvlFit();

  clearPeakPlots();
  plotEvaluatedPeak(fr, er.peakX, er.minY, er.maxY, er.widthHalfLeft, er.widthHalfRight, er.peakHeight, er.peakHeightBaseline);

  if (m_currentPeakIdx > 0 && updateCurrentPeak) {
    m_allPeaks[m_currentPeakIdx] = m_currentPeak;
    m_evaluatedPeaksModel.updateEntry(m_currentPeakIdx - 1, er.peakX, er.peakArea);
  }

  m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;
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

  m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, QString("%1 %2").arg(m_currentDataContext->data->xType).arg(xUnit));
  m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, QString("%1 %2").arg(m_currentDataContext->data->yType).arg(yUnit));
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

bool EvaluationEngine::setEvaluationContext(const EvaluationContext &ctx)
{
  m_allPeaks = ctx.peaks;
  m_currentPeakIdx = ctx.lastIndex;

  if (m_currentPeakIdx < 0 || m_currentPeakIdx >= m_allPeaks.length()) {
    m_userInteractionState = UserInteractionState::FINDING_PEAK;
    return false;
  }

  m_currentPeak = m_allPeaks.at(m_currentPeakIdx);

  m_modeCtx->clearAllSerieSamples();
  setPeakContext(m_currentPeak);

  if (m_currentPeakIdx == 0)
    m_userInteractionState = UserInteractionState::FINDING_PEAK; /* "Default" placeholder peak, enable finding */
  else
    m_userInteractionState = UserInteractionState::PEAK_POSTPROCESSING;

  m_evaluationAutoValues = ctx.afAutoValues;
  m_evaluationBooleanValues = ctx.afBoolValues;
  m_evaluationFloatingValues = ctx.afFloatingValues;

  emit m_evaluationAutoModel.dataChanged(m_evaluationAutoModel.index(0, 0),
                                         m_evaluationAutoModel.index(0, m_evaluationAutoModel.indexFromItem(EvaluationParametersItems::Auto::LAST_INDEX)),
                                         { Qt::DisplayRole });
  emit m_evaluationBooleanModel.dataChanged(m_evaluationBooleanModel.index(0, 0),
                                            m_evaluationBooleanModel.index(0, m_evaluationBooleanModel.indexFromItem(EvaluationParametersItems::Boolean::LAST_INDEX)),
                                            { Qt::DisplayRole });
  emit m_evaluationFloatingModel.dataChanged(m_evaluationFloatingModel.index(0, 0),
                                             m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::LAST_INDEX)),
                                             { Qt::EditRole });


  return createSignalPlot(m_currentDataContext->data, m_currentDataContext->name);
}

void EvaluationEngine::setEvaluationResults(const std::shared_ptr<PeakFinderResults> &fr, const PeakEvaluator::Results &er)
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
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_FROM_X] = fr->peakFromX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_FROM_Y] = fr->peakFromY;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_TO_X] = fr->peakToX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_TO_Y] = fr->peakToY;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_X] = er.peakX;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_HEIGHT] = er.peakHeight;
  m_resultsNumericValues[EvaluationResultsItems::Floating::PEAK_HEIGHT_BL] = er.peakHeightBaseline;

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
  m_resultsNumericValues = ctx.resultsValues;
  m_hvlFitValues = ctx.hvlValues;
  m_hvlFitFixedValues = ctx.hvlFitFixedValues;
  m_hvlFitIntValues = ctx.hvlFitIntValues;
  m_windowUnit = ctx.windowUnit;
  m_showWindow = ctx.showWindow;
  m_baselineAlgorithm = ctx.baselineAlgorithm;

  clearPeakPlots();
  fullViewUpdate();
  if (ctx.finderResults->isValid() && (m_currentDataContext->data != nullptr))
    plotEvaluatedPeak(ctx.finderResults,
                      m_resultsNumericValues.at(EvaluationResultsItems::Floating::PEAK_X),
                      Helpers::minYValue(m_currentDataContext->data->data), Helpers::maxYValue(m_currentDataContext->data->data),
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

  QModelIndex nwIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::NOISE_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(nwIdx, nwIdx, { Qt::EditRole });

  QModelIndex pwIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::PEAK_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(pwIdx, pwIdx, { Qt::EditRole });

  QModelIndex swIdx = m_evaluationFloatingModel.index(0, m_evaluationFloatingModel.indexFromItem(EvaluationParametersItems::Floating::SLOPE_WINDOW));
  emit m_evaluationFloatingModel.dataChanged(swIdx, swIdx, { Qt::EditRole });
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

  disconnect(m_commonParamsEngine, &CommonParametersEngine::tEofUpdated, this, &EvaluationEngine::onUpdateCurrentPeak);
  m_commonParamsEngine->setContext(m_currentDataContext->commonContext);
  setEvaluationContext(m_currentDataContext->evaluationContext);
  m_evaluatedPeaksModel.setEntries(makeEvaluatedPeaks());
  connect(m_commonParamsEngine, &CommonParametersEngine::tEofUpdated, this, &EvaluationEngine::onUpdateCurrentPeak);
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
