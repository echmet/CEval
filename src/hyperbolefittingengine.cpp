#include "setup.h"

#include "hyperbolefittingengine.h"
#include <QFileDialog>
#include <QMessageBox>
#include "custommetatypes.h"
#include "globals.h"
#include "hyperbolefititems.h"
#include "math/regressor/profiler.h"
#include "doubletostringconvertor.h"
#include "standardmodecontextsettingshandler.h"
#include "gui/exportdatatabletocsvdialog.h"
#include "math/regressor/regress.h"

using namespace echmet;
using namespace regressCore;

const QString HyperboleFittingEngine::s_dataPointsATitle = tr("Points A");
const QString HyperboleFittingEngine::s_dataPointsBTitle = tr("Points B");
const QString HyperboleFittingEngine::s_dataPointsAAvgTitle = tr("Average of points A");
const QString HyperboleFittingEngine::s_dataPointsBAvgTitle = tr("Average of points B");
const QString HyperboleFittingEngine::s_fitCurveATitle = tr("Fit A");
const QString HyperboleFittingEngine::s_fitCurveBTitle = tr("Fit B");
const QString HyperboleFittingEngine::s_fitCurveStatsTitle = tr("Statistics");
const QString HyperboleFittingEngine::s_horizontalMarkerTitle = tr("Horizontal marker");
const QString HyperboleFittingEngine::s_verticalAMarkerTitle = tr("Vertical A marker");
const QString HyperboleFittingEngine::s_verticalBMarkerTitle = tr("Vertical B marker");

const QString HyperboleFittingEngine::s_uACaption = tr("Free mobility (\u03BC (A))");
const QString HyperboleFittingEngine::s_uCSCaption = tr("Mobility of complex (\u03BC (AS))");
const QString HyperboleFittingEngine::s_KCSCaption = tr("Complexation constant (K (AS))");
const QString HyperboleFittingEngine::s_tauCaption = tr("Tau (\u03C4)");
const QString HyperboleFittingEngine::s_confidenceCaption = tr("Confidence");
const QString HyperboleFittingEngine::s_pValueCaption = tr("P-value");


const double HyperboleFittingEngine::s_defaultViscositySlope = 0.0;
const double HyperboleFittingEngine::s_defaultEpsilon = 1.0e-9;
const int HyperboleFittingEngine::s_defaultMaxIterations = 50;

const QString HyperboleFittingEngine::CSV_FILE_SUFFIX("csv");
const QString HyperboleFittingEngine::DATA_TABLE_FILE_SUFFIX("evd");
const QString HyperboleFittingEngine::EMERG_SAVE_FILE("recovery." + DATA_TABLE_FILE_SUFFIX);
const double HyperboleFittingEngine::INVAL_CONC_KEY(-1.0);
const QString HyperboleFittingEngine::INVAL_ANALYTE_KEY("");

const QString HyperboleFittingEngine::LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG("LastExportToCsvPath");
const QString HyperboleFittingEngine::LAST_LOADSAVE_PATH_SETTINGS_TAG("LastLoadSavePath");

HyperboleFittingEngine::Analyte::Analyte(const QString &name) :
  name(name)
{
}

HyperboleFittingEngine::Analyte::Analyte(const QString &name, std::shared_ptr<const Analyte> other) :
  concentrations(other->concentrations),
  name(name)
{
}

EvalSerializable::RetCode HyperboleFittingEngine::SerializableConcentration::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;
  ret = Manipulator.Operate(c);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(mobilities);
}

std::string HyperboleFittingEngine::SerializableConcentration::TypeName() const
{
  return "SerializableConcentration";
}

EvalSerializable::RetCode HyperboleFittingEngine::SerializableAnalyte::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;
  ret = Manipulator.Operate(name);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(concentrations);
}

std::string HyperboleFittingEngine::SerializableAnalyte::TypeName() const
{
  return "SerializableAnalyte";
}

EvalSerializable::RetCode HyperboleFittingEngine::SerializableDataTable::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;

  ret = Manipulator.Operate(viscositySlope);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(analytes);
}

std::string HyperboleFittingEngine::SerializableDataTable::TypeName() const
{
  return "SerializableDataTable";
}

HyperboleFittingEngine::Concentration::Concentration(const double concentration) :
  concentration(concentration),
  m_avgMobility(0.0)
{
}

void HyperboleFittingEngine::Concentration::addMobility(const double mobility)
{
  m_mobilities.push_back(mobility);

  recalculateAverage();
}

double HyperboleFittingEngine::Concentration::avgMobility() const
{
  return m_avgMobility;
}

const HyperboleFittingEngine::Concentration::Mobilities &HyperboleFittingEngine::Concentration::mobilities() const
{
  return m_mobilities;
}

void HyperboleFittingEngine::Concentration::removeMobility(const int idx)
{
  if (idx < 0 || idx >= m_mobilities.length())
    return;

  m_mobilities.remove(idx);

  recalculateAverage();
}

void HyperboleFittingEngine::Concentration::recalculateAverage()
{
  double sd = 0.0;

  if (m_mobilities.length() < 1)
    return;

  for (const double d : m_mobilities)
    sd += d;

  m_avgMobility = sd / m_mobilities.length();
}

HyperboleFittingEngine::HypResults::HypResults(const double u0, const double uCS, const double KCS, const double maxX, const double sigma, const int iterations) :
  u0_A(u0), uCS_A(uCS), KCS_A(KCS), maxX_A(maxX),
  sigma(sigma), iterations(iterations),
  m_valid(true)
{
}

HyperboleFittingEngine::HypResults::HypResults() :
  u0_A(0.0), uCS_A(0.0), KCS_A(0.0), maxX_A(0.0), sigma(0.0), iterations(0),
  m_valid(false)
{
}

bool HyperboleFittingEngine::HypResults::isValid() const
{
  return m_valid;
}

HyperboleFittingEngine::HypResults &HyperboleFittingEngine::HypResults::operator=(const HypResults &other)
{
  const_cast<double&>(u0_A) = other.u0_A;
  const_cast<double&>(uCS_A) = other.uCS_A;
  const_cast<double&>(KCS_A) = other.KCS_A;
  const_cast<double&>(maxX_A) = other.maxX_A;
  const_cast<double&>(sigma) = other.sigma;
  const_cast<int&>(iterations) = other.iterations;
  const_cast<bool&>(m_valid) = other.m_valid;

  return *this;
}

HyperboleFittingEngine::DoubleHypResults::DoubleHypResults(const double u0_A, const double uCS_A, const double KCS_A, const double maxX_A,
                                                           const double sigma, const int iterations,
                                                           const double u0_B, const double uCS_B, const double KCS_B) :
  HypResults(u0_A, uCS_A, KCS_A, maxX_A, sigma, iterations),
  u0_B(u0_B), uCS_B(uCS_B), KCS_B(KCS_B)
{
}

HyperboleFittingEngine::DoubleHypResults::DoubleHypResults() :
  HypResults(),
  u0_B(0.0), uCS_B(0.0), KCS_B(0.0)
{
}

HyperboleFittingEngine::DoubleHypResults &HyperboleFittingEngine::DoubleHypResults::operator=(const DoubleHypResults &other)
{
  const_cast<double&>(u0_A) = other.u0_A;
  const_cast<double&>(uCS_A) = other.uCS_A;
  const_cast<double&>(KCS_A) = other.KCS_A;
  const_cast<double&>(maxX_A) = other.maxX_A;
  const_cast<double&>(sigma) = other.sigma;
  const_cast<int&>(iterations) = other.iterations;
  const_cast<bool&>(m_valid) = other.m_valid;

  const_cast<double&>(u0_B) = other.u0_B;
  const_cast<double&>(uCS_B) = other.uCS_B;
  const_cast<double&>(KCS_B) = other.KCS_B;

  return *this;
}

HyperboleFittingEngine::HyperboleFittingEngine(QObject *parent) :
  QObject(parent),
  m_currentAnalyte(nullptr),
  m_secondAnalyte(nullptr),
  m_currentConcentration(nullptr),
  m_currentConcentrationKey(INVAL_CONC_KEY),
  m_statsForAnalyte(AnalyteId::ANALYTE_A),
  m_showHorizontalMarker(false),
  m_showVerticalAMarker(false),
  m_showVerticalBMarker(false),
  m_horizontalMarkerPosition(0.0),
  m_verticalAMarkerPosition(0.0),
  m_verticalBMarkerPosition(0.0),
  m_lastDataTablePath(QDir::homePath()),
  m_lastExportToCsvPath(QDir::homePath()),
  m_dataTablesNameFilter(QStringList() << Globals::SOFTWARE_NAME + " Data table (*." + HyperboleFittingEngine::DATA_TABLE_FILE_SUFFIX + ")" << "Any file (*.*)"),
  m_exportToCsvNameFilter(QStringList() << "Comma separated values (*." + HyperboleFittingEngine::CSV_FILE_SUFFIX + ")")
{
  initFitModeModel();
  m_currentFitMode = FitMode::SINGLE;
  initStatUnitsModel();
  m_currentStatUnits = StatUnits::TAU;

  setSingleFitStats();
  m_currentStatMode = StatMode::MOBILITY_A;

  m_fitFloatValues[HyperboleFitParameters::Floating::VISCOSITY_SLOPE] = s_defaultViscositySlope;
  m_fitFloatValues[HyperboleFitParameters::Floating::EPSILON] = s_defaultEpsilon;
  m_fitIntValues[HyperboleFitParameters::Int::MAX_ITERATIONS] = s_defaultMaxIterations;

  m_fitFixedModel.setUnderlyingData(m_fitFixedValues.pointer());
  m_fitFloatModel.setUnderlyingData(m_fitFloatValues.pointer());
  m_fitIntModel.setUnderlyingData(m_fitIntValues.pointer());
  m_fitResultsModel.setUnderlyingData(m_fitResultsValues.pointer());
  m_analyteNamesModel.setUnderlyingData(m_analyteNamesValues.pointer());

  m_singleFitRegressor = new echmet::regressCore::RectangularHyperbole<double, double>();
  m_doubleFitRegressor = new echmet::regressCore::RectangularHyperbole2<double, double>();

  m_exportDTToCsvDlg = new ExportDatatableToCsvDialog(m_exportToCsvNameFilter, m_lastExportToCsvPath);
  if (m_exportDTToCsvDlg == nullptr)
    throw std::bad_alloc();

  connect(&m_fitFixedModel, &BooleanMapperModel<HyperboleFitParameters::Boolean>::dataChanged, this, &HyperboleFittingEngine::onSecondAnalyteSameChanged);

  typedef Mat<double> MatrixD;

  /* Preinitialize regressors */
  {
    vector<double> mat_x(1, 1);
    MatrixD        mat_y(1, 1);
    bool ret;

    mat_x[0] = 0;
    mat_y(0,0) = 0;

    ret = m_singleFitRegressor->Initialize(mat_x, mat_y,
                                           m_fitFloatValues.at(HyperboleFitParameters::Floating::EPSILON),
                                           m_fitIntValues.at(HyperboleFitParameters::Int::MAX_ITERATIONS),
                                           true, 0.0,
                                           m_fitFloatValues.at(HyperboleFitParameters::Floating::VISCOSITY_SLOPE));
    if (!ret)
        throw regressor_initialization_error("Failed to preinitialize single fit regressor");
  }
  {
    vector<hyp2x_type> mat_x(1, 1);
    MatrixD            mat_y(1, 1);
    bool ret;

    mat_x[0] = 0;
    mat_y(0,0) = 0;

    ret = m_doubleFitRegressor->Initialize(mat_x, mat_y,
                                           m_fitFloatValues.at(HyperboleFitParameters::Floating::EPSILON),
                                           m_fitIntValues.at(HyperboleFitParameters::Int::MAX_ITERATIONS),
                                           true, 0.0, 0.0,
                                           m_fitFloatValues.at(HyperboleFitParameters::Floating::VISCOSITY_SLOPE));

    if (!ret)
      throw regressor_initialization_error("Failed to preinitialize double fit regressor");
  }

  DoubleToStringConvertor::notifyOnFormatChanged(this);
}

HyperboleFittingEngine::~HyperboleFittingEngine()
{
  delete m_singleFitRegressor;
  delete m_doubleFitRegressor;
  delete m_exportDTToCsvDlg;
}

AbstractMapperModel<QString, HyperboleFitParameters::String> *HyperboleFittingEngine::analyteNamesModel()
{
  return &m_analyteNamesModel;
}

QAbstractItemModel *HyperboleFittingEngine::analytesModel()
{
  return &m_analytesModel;
}

void HyperboleFittingEngine::assignContext(std::shared_ptr<ModeContextLimited> ctx)
{
  m_modeCtx = ctx;

  if (!m_modeCtx->addSerie(seriesIndex(Series::POINTS_A), s_dataPointsATitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(POINT_TYPE, POINT_SIZE, Qt::black),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsATitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::POINTS_B), s_dataPointsBTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(QwtSymbol::Style::XCross, POINT_SIZE, QColor(142, 87, 172)),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsBTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::POINTS_A_AVG), s_dataPointsAAvgTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(CENTRAL_POINT_TYPE, CENTRAL_POINT_SIZE, Qt::red),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsAAvgTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::POINTS_B_AVG), s_dataPointsBAvgTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(QwtSymbol::Style::XCross, CENTRAL_POINT_SIZE, QColor(77, 119, 183)),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsBAvgTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::FIT_A_CURVE), s_fitCurveATitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveATitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::FIT_B_CURVE), s_fitCurveBTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(77, 119, 183), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveBTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::STATS), s_fitCurveStatsTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(130, 190, 73), Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveStatsTitle));

  if (!m_modeCtx->addSerie(seriesIndex(Series::HORIZONTAL_MARKER), s_horizontalMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_horizontalMarkerTitle));
  if (!m_modeCtx->addSerie(seriesIndex(Series::VERTICAL_A_MARKER), s_verticalAMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_verticalAMarkerTitle));
  if (!m_modeCtx->addSerie(seriesIndex(Series::VERTICAL_B_MARKER), s_verticalAMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_verticalBMarkerTitle));


  /* Default axis fonts*/
  m_modeCtx->setAxisFont(SerieProperties::Axis::X_BOTTOM, QFont());
  m_modeCtx->setAxisFont(SerieProperties::Axis::Y_LEFT, QFont());

  showDataSeries();
}

void HyperboleFittingEngine::checkForCrashRecovery()
{
  QString path = QDir::currentPath() + "/" + EMERG_SAVE_FILE;
  QFile recovery(path);

  if (!recovery.exists())
    return;

  int ret =  QMessageBox::question(nullptr, tr("Crash recovery"), QString(tr("A file with data tables saved right before %1 crashed was found.\n"
                                                                             "Would you like to load this file?").arg(Globals::SOFTWARE_NAME)),
                                   QMessageBox::Yes | QMessageBox::No);

  if (ret == QMessageBox::Yes) {
    bool bret = doDeserialize(path);

    if (!bret)
      QMessageBox::warning(nullptr, tr("Crash recovery failed"), QString(tr("%1 was unable to recover data tables from the emergency file.")));
    else
      QMessageBox::information(nullptr, tr("Crash recovery successful"), tr("Data tables from emergency file were sucessfully recovered."));

    ret = QMessageBox::question(nullptr,  tr("Crash recovery"), tr("Delete the emergency file?"), QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes)
      recovery.remove();
  }
}

void HyperboleFittingEngine::clearAllModels()
{
  m_mobilitiesModel.clear();
  m_concentrationsModel.clear();
}

void HyperboleFittingEngine::clearAnalyteASeries()
{
  m_modeCtx->clearSerieSamples(seriesIndex(Series::POINTS_A));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::POINTS_A_AVG));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::FIT_A_CURVE));
}

void HyperboleFittingEngine::clearAnalyteBSeries()
{
  m_modeCtx->clearSerieSamples(seriesIndex(Series::POINTS_B));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::POINTS_B_AVG));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::FIT_B_CURVE));
}

QAbstractItemModel *HyperboleFittingEngine::concentrationsModel()
{
  return &m_concentrationsModel;
}

bool HyperboleFittingEngine::doDeserialize(const QString &path)
{
  SerializableDataTable table;

  EvalSerializable::RetCode r = EvalSerializable::ReadFromFile(path.toLocal8Bit(), table);
  if (r != EvalSerializable::RetCode::SUCCESS) {
    QString errMsg = QString::fromStdString(EvalSerializable::RetCodeToString(r));
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Error while loading data table:\n%1").arg(errMsg)));
    return false;
  }

  m_analytes.clear();

  try {
    for (const SerializableAnalyte &sa : table.analytes) {
      QString qName = QString::fromStdString(sa.name);
      std::shared_ptr<Analyte> a = std::shared_ptr<Analyte>(new Analyte(qName));

      for (const SerializableConcentration sc : sa.concentrations) {
        std::shared_ptr<Concentration> c = std::shared_ptr<Concentration>(new Concentration(sc.c));

       for (const double u : sc.mobilities) {
         c->addMobility(u);
        }

        a->concentrations.insert(sc.c, c);
      }

      m_analytes.insert(qName, a);
    }
  } catch (std::bad_alloc& ) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to load data table"));
    return false;
  }

  m_fitFloatValues[HyperboleFitParameters::Floating::VISCOSITY_SLOPE] = table.viscositySlope;
  m_fitFloatModel.notifyDataChanged(HyperboleFitParameters::Floating::VISCOSITY_SLOPE, HyperboleFitParameters::Floating::VISCOSITY_SLOPE, { Qt::EditRole });

  refreshModels();
  return true;
}

void HyperboleFittingEngine::displayHypResults(const HypResults *r)
{
  if (!r->isValid())
    return;

  m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_A] = r->u0_A;
  m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_CS_A] = r->uCS_A;
  m_fitResultsValues[HyperboleFitResults::Floating::K_CS_A] = r->KCS_A;
  m_fitResultsValues[HyperboleFitResults::Floating::SIGMA] = r->sigma;
  m_fitResultsValues[HyperboleFitResults::Floating::ITERATIONS] = r->iterations;

  m_fitResultsModel.notifyDataChanged(HyperboleFitResults::Floating::SIGMA, HyperboleFitResults::Floating::K_CS_A, { Qt::EditRole });
  m_fitResultsModel.notifyDataChanged(HyperboleFitResults::Floating::ITERATIONS, HyperboleFitResults::Floating::ITERATIONS, { Qt::EditRole });

  showDataSeries();

  switch (m_currentFitMode) {
  case FitMode::SINGLE:
    plotSingleCurve(*r);
    break;
  case FitMode::DOUBLE:
  {
    const DoubleHypResults *dr = static_cast<const DoubleHypResults *>(r);

    m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_B] = dr->u0_B;
    m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_CS_B] = dr->uCS_B;
    m_fitResultsValues[HyperboleFitResults::Floating::K_CS_B] = dr->KCS_B;

    m_fitResultsModel.notifyDataChanged(HyperboleFitResults::Floating::MOBILITY_B, HyperboleFitResults::Floating::K_CS_B, { Qt::EditRole });

    plotDoubleCurve(*dr);
  }
  }
}

HyperboleFittingEngine::DoubleHypResults HyperboleFittingEngine::doDoubleEstimate(const bool usedForStats = false)
{
  double maxX = std::numeric_limits<double>::min();
  /* Prevent any ptr vs. var mishaps */
  echmet::regressCore::RectangularHyperbole2<double, double> &dfrRef = *m_doubleFitRegressor;

  typedef Mat<double> MatrixD;

  vector<hyp2x_type> mat_x;
  MatrixD            mat_y;

  if (m_currentAnalyte == nullptr || m_secondAnalyte == nullptr)
    return DoubleHypResults();

  if (m_currentAnalyte->concentrations.size() < 1 ||
      m_secondAnalyte->concentrations.size() < 1)
    return DoubleHypResults();

  if (!m_currentAnalyte->concentrations.contains(0.0)) {
    double u0 = m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_A);
    int ret = QMessageBox::question(nullptr, tr("No zero concentration"), QString(tr("List of entered concentrations does not contain any data for zero concentration of the selector.\n"
                                                                                     "Do you want to use the current value of \u03BC0 = %1 as the estimate?")).arg(u0),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
      return DoubleHypResults();
  }

  /* Sry :-( */
  const Analyte::ConcentrationMap &cs1 = (!usedForStats) ? m_currentAnalyte->concentrations : ( (m_statsForAnalyte == AnalyteId::ANALYTE_A) ? m_currentAnalyte->concentrations : m_secondAnalyte->concentrations );
  const Analyte::ConcentrationMap &cs2 = (!usedForStats) ? m_secondAnalyte->concentrations : ( (m_statsForAnalyte == AnalyteId::ANALYTE_A) ? m_secondAnalyte->concentrations : m_currentAnalyte->concentrations );

  mat_x = vector<hyp2x_type> (cs1.size() + cs2.size());
  mat_y = MatrixD            (cs1.size() + cs2.size(), 1);

  // reading 1st
  int idx = 0;
  for (std::shared_ptr<const Concentration> c : cs1) {
    if (c->concentration > maxX)
      maxX = c->concentration;

    mat_x[idx].index = 0;
    mat_x[idx].value = c->concentration;
    if (c->mobilities().length() < 1) {
      QMessageBox::warning(nullptr, tr("Invalid data"), QString(tr("There are no mobilities assigned to concentration \"%1\"")).arg(c->concentration));
      return DoubleHypResults();
    }

    mat_y(idx,0) = c->avgMobility();

    ++idx;
  }

  // reading 2nd
  for (std::shared_ptr<const Concentration> c : cs2) {
    if (c->concentration > maxX)
      maxX = c->concentration;

    mat_x[idx].index = 1;
    mat_x[idx].value = c->concentration;
    if (c->mobilities().length() < 1) {
      QMessageBox::warning(nullptr, tr("Invalid data"), QString(tr("There are no mobilities assigned to concentration \"%1\"")).arg(c->concentration));
      return DoubleHypResults();
    }

    mat_y(idx,0) = c->avgMobility();

    ++idx;
  }

  double u0A =  m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_A);
  double uCSA = m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_CS_A);
  double KCSA = m_fitResultsValues.at(HyperboleFitResults::Floating::K_CS_A);
  double u0B =  m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_B);
  double uCSB;
  double KCSB;

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::u0, u0A);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::u0);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::uS, uCSA);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::uS);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_K_CS_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::KS, KCSA);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::KS);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_B)) {
    u0B = u0A;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::du0, u0B - u0A);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::du0);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_B)) {
    uCSB = uCSA;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::duS, uCSB - uCSA);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::duS);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_K_CS_B)) {
    KCSB = KCSA;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbole2Params::dKS, KCSB - KCSA);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbole2Params::dKS);


  bool ret = dfrRef.Initialize(mat_x, mat_y,
                               m_fitFloatValues.at(HyperboleFitParameters::Floating::EPSILON),
                               m_fitIntValues.at(HyperboleFitParameters::Int::MAX_ITERATIONS),
                               true, u0A, u0B,
                               m_fitFloatValues.at(HyperboleFitParameters::Floating::VISCOSITY_SLOPE));

  if (!ret) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Double estimation failed."));
    return DoubleHypResults();
  }

  u0A  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::u0);
  uCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::uS);
  KCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::KS);
  u0B  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::du0);
  uCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::duS);
  KCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::dKS);
  double sigma = dfrRef.GetS();

  return DoubleHypResults(u0A, uCSA, KCSA, maxX, sigma, 0, u0A + u0B, uCSA + uCSB, KCSA + KCSB);
}

HyperboleFittingEngine::DoubleHypResults HyperboleFittingEngine::doDoubleFit(const DoubleHypResults &dr)
{
  echmet::regressCore::RectangularHyperbole2<double, double> &dfrRef = *m_doubleFitRegressor;
  if (!dfrRef.Regress())
    return DoubleHypResults();

  double u0A = dfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::u0);
  double uCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::uS);
  double KCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::KS);
  double u0B  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::du0);
  double uCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::duS);
  double KCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbole2Params::dKS);
  int iters = dfrRef.GetIterationCounter();
  double sigma = dfrRef.GetS();

  return DoubleHypResults(u0A, uCSA, KCSA, dr.maxX_A, sigma, iters, u0A + u0B, uCSA + uCSB, KCSA + KCSB);
}

void HyperboleFittingEngine::doSerialize(const QString &path)
{
  SerializableDataTable table;
  table.viscositySlope = m_fitFloatValues.at(HyperboleFitParameters::Floating::VISCOSITY_SLOPE);

  try {
    for (std::shared_ptr<const Analyte> a : m_analytes) {
      SerializableAnalyte sa;
      sa.name = a->name.toStdString();

      for (std::shared_ptr<const Concentration> c : a->concentrations) {
        SerializableConcentration sc;
        sc.c = c->concentration;

        for (const double m : c->mobilities()) {
          sc.mobilities.push_back(m);
        }

        sa.concentrations.push_back(sc);
      }

      table.analytes.push_back(sa);
    }
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to serialize data table"));
    return;
  }

  EvalSerializable::RetCode r =  EvalSerializable::WriteToFile(path.toLocal8Bit(), table);
  if (r != EvalSerializable::RetCode::SUCCESS) {
    QString errMsg = QString::fromStdString(EvalSerializable::RetCodeToString(r));

    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Error while saving data table:\n%1")).arg(errMsg));
  }
}

HyperboleFittingEngine::HypResults HyperboleFittingEngine::doSingleEstimate()
{
  double maxX = std::numeric_limits<double>::min();
  /* Prevent any ptr vs. var mishaps */
  echmet::regressCore::RectangularHyperbole<double, double> &sfrRef = *m_singleFitRegressor;

  typedef Mat<double> MatrixD;

  vector<double> mat_x;
  MatrixD        mat_y;

  if (m_currentAnalyte == nullptr)
    return HypResults();

  if (m_currentAnalyte->concentrations.size() < 1)
    return HypResults();

  if (!m_currentAnalyte->concentrations.contains(0.0)) {
    double u0 = m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_A);
    int ret = QMessageBox::question(nullptr, tr("No zero concentration"), QString(tr("List of entered concentrations does not contain any data for zero concentration of the selector.\n"
                                                                                     "Do you want to use the current value of \u03BC0 = %1 as the estimate?")).arg(u0),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
      return HypResults();
  }

  const Analyte::ConcentrationMap &cs = m_currentAnalyte->concentrations;

  mat_x = vector<double>(cs.size());
  mat_y = MatrixD(cs.size(), 1);
  int idx = 0;
  for (std::shared_ptr<const Concentration> c : cs) {
    if (c->concentration > maxX)
      maxX = c->concentration;

    mat_x[idx] = c->concentration;
    if (c->mobilities().length() < 1) {
      QMessageBox::warning(nullptr, tr("Invalid data"), QString(tr("There are no mobilities assigned to concentration \"%1\"")).arg(c->concentration));
      return HypResults();
    }

    mat_y(idx,0) = c->avgMobility();

    ++idx;
  }

  double u0 = m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_A);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperboleParams::u0,
        m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperboleParams::u0);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperboleParams::uS,
        m_fitResultsValues.at(HyperboleFitResults::Floating::MOBILITY_CS_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperboleParams::uS);

  if (m_fitFixedValues.at(HyperboleFitParameters::Boolean::FIXED_K_CS_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperboleParams::KS,
        m_fitResultsValues.at(HyperboleFitResults::Floating::K_CS_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperboleParams::KS);

  bool ret = sfrRef.Initialize(mat_x, mat_y,
                               m_fitFloatValues.at(HyperboleFitParameters::Floating::EPSILON),
                               m_fitIntValues.at(HyperboleFitParameters::Int::MAX_ITERATIONS),
                               true, u0,
                               m_fitFloatValues.at(HyperboleFitParameters::Floating::VISCOSITY_SLOPE));

  if (!ret) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Single estimation failed."));
    return HypResults();
  }

  u0 = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::u0);
  double uCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::uS);
  double KCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::KS);
  double sigma = sfrRef.GetS();

  return HypResults(u0, uCS, KCS, maxX, sigma, 0);
}

HyperboleFittingEngine::HypResults HyperboleFittingEngine::doSingleFit(const HypResults &r)
{
  echmet::regressCore::RectangularHyperbole<double, double> &sfrRef = *m_singleFitRegressor;
  sfrRef.Regress();

  double u0 = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::u0);
  double uCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::uS);
  double KCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperboleParams::KS);
  double sigma = sfrRef.GetS();

  return HypResults(u0, uCS, KCS, r.maxX_A, sigma, sfrRef.GetIterationCounter());
}

void HyperboleFittingEngine::exportToCsv()
{
  if (m_analytes.size() < 1) {
    QMessageBox::information(nullptr, QObject::tr("Nothing to export"), QObject::tr("The data table is empty"));
    return;
  }

  while (m_exportDTToCsvDlg->exec() == QDialog::Accepted) {
    QChar delimiter;
    QString path;
    ExportDatatableToCsvDialog::Parameters p(m_exportDTToCsvDlg->parameters());

    if (p.decimalSeparator == QChar('\0'))
      return;

    if (p.path.length() < 1) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid path specified"));
      continue;
    }
    if (p.precision < 1) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Nonsensical value of numeric precision"));
      continue;
    }
    if (p.delimiter.length() != 1) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Delimiter must be a single character"));
      continue;
    }
    delimiter = p.delimiter.at(0);

    path = p.path;
    if (!path.endsWith("." + CSV_FILE_SUFFIX))
      path.append("." + CSV_FILE_SUFFIX);

    bool ret;
    switch (p.exMode) {
    case ExportDatatableToCsvDialog::ExportMode::SINGLE_FILE:
      ret = exportToCsvSingleFile(path, delimiter, p.decimalSeparator, p.precision);
      break;
    }

    if (ret == true) {
      m_lastExportToCsvPath = path;
      return;
    }
  }
}

bool HyperboleFittingEngine::exportToCsvSingleFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const int precision)
{
  QFile file(path);
  QTextStream stream(&file);
  QLocale loc(QLocale::C);

  if (file.exists()) {
    if (QMessageBox::question(nullptr, QObject::tr("File exists"), QObject::tr("Selected file already exists, overwrite?")) == QMessageBox::No)
      return false;
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(nullptr, QObject::tr("I/O error"), QObject::tr("Cannot open output file"));
    return false;
  }

  stream.setCodec("UTF-8");
  stream.setGenerateByteOrderMark(false);

  for (const std::shared_ptr<Analyte> analyte : m_analytes) {
    stream << analyte->name << delimiter << "\n";

    stream << QObject::tr("Concentration") << delimiter << QObject::tr("Mobilities") << "\n";
    for (const std::shared_ptr<Concentration> concentration : analyte->concentrations) {
      QString c = loc.toString(concentration->concentration, 'g', precision);
      c.replace(loc.decimalPoint(), decimalSeparator);

      stream << c << delimiter;
      for (const double mobility : concentration->mobilities()) {
        QString u = loc.toString(mobility, 'g', precision);
        u.replace(loc.decimalPoint(), decimalSeparator);

        stream << u << delimiter;
      }
      stream << "\n";
    }
    stream << "\n";
  }

  file.close();

  return true;
}

AbstractMapperModel<bool, HyperboleFitParameters::Boolean> *HyperboleFittingEngine::fitFixedModel()
{
  return &m_fitFixedModel;
}

AbstractMapperModel<double, HyperboleFitParameters::Floating> *HyperboleFittingEngine::fitFloatModel()
{
  return &m_fitFloatModel;
}

AbstractMapperModel<int, HyperboleFitParameters::Int> *HyperboleFittingEngine::fitIntModel()
{
  return &m_fitIntModel;
}

AbstractMapperModel<double, HyperboleFitResults::Floating> *HyperboleFittingEngine::fitResultsModel()
{
  return &m_fitResultsModel;
}

QAbstractItemModel *HyperboleFittingEngine::fitModeModel()
{
  return &m_fitModeModel;
}

void HyperboleFittingEngine::hideDataSeries()
{
  m_modeCtx->hideSerie(seriesIndex(Series::FIT_A_CURVE));
  m_modeCtx->hideSerie(seriesIndex(Series::FIT_B_CURVE));
  m_modeCtx->hideSerie(seriesIndex(Series::POINTS_A));
  m_modeCtx->hideSerie(seriesIndex(Series::POINTS_B));
  m_modeCtx->hideSerie(seriesIndex(Series::POINTS_A_AVG));
  m_modeCtx->hideSerie(seriesIndex(Series::POINTS_B_AVG));
}

void HyperboleFittingEngine::initFitModeModel()
{
  try {
    QStandardItem *singleFit = new QStandardItem(tr("Single fit"));
    singleFit->setData(QVariant::fromValue(FitMode::SINGLE), Qt::UserRole + 1);
    m_fitModeModel.appendRow(singleFit);

    QStandardItem *doubleFit = new QStandardItem(tr("Double fit"));
    doubleFit->setData(QVariant::fromValue(FitMode::DOUBLE), Qt::UserRole + 1);
    m_fitModeModel.appendRow(doubleFit);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to initialize fitModeModel"));
    throw;
  }
}

void HyperboleFittingEngine::initStatUnitsModel()
{
  try {
    QStandardItem *tau = new QStandardItem(s_tauCaption);
    tau->setData(QVariant::fromValue(StatUnits::TAU), Qt::UserRole + 1);
    m_statUnitsModel.appendRow(tau);

    QStandardItem *significance = new QStandardItem(s_confidenceCaption);
    significance->setData(QVariant::fromValue(StatUnits::CONFIDENCE), Qt::UserRole + 1);
    m_statUnitsModel.appendRow(significance);

    QStandardItem *pVal = new QStandardItem(s_pValueCaption);
    pVal->setData(QVariant::fromValue(StatUnits::P_VALUE), Qt::UserRole + 1);
    m_statUnitsModel.appendRow(pVal);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to initialize statUnitsModel"));
    throw;
  }
}

void HyperboleFittingEngine::invalidateAll()
{
  invalidateCurrentConcentration();
  m_currentAnalyte = nullptr;
}

double HyperboleFittingEngine::interpolateVerticalMarkerPosition(const QPointF &a, const QPointF &b, const double y) const
{
  const double k = (b.y() - a.y()) / (b.x() - a.x());

  return ((y - a.y()) / k) + a.x();
}

void HyperboleFittingEngine::invalidateAnalyteB()
{
  m_secondAnalyte = nullptr;

  m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_B] = "";
  m_analyteNamesModel.notifyDataChanged(HyperboleFitParameters::String::ANALYTE_B, HyperboleFitParameters::String::ANALYTE_B);

  m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_B] = 0.0;
  m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_CS_B] = 0.0;
  m_fitResultsValues[HyperboleFitResults::Floating::K_CS_B] = 0.0;
  m_fitResultsModel.notifyDataChanged(HyperboleFitResults::Floating::MOBILITY_B, HyperboleFitResults::Floating::K_CS_B, { Qt::EditRole });
}

void HyperboleFittingEngine::invalidateCurrentConcentration()
{
  m_currentConcentration = nullptr;
  m_currentConcentrationKey = INVAL_CONC_KEY;
}

bool HyperboleFittingEngine::isEditable() const
{
  if (m_currentFitMode == FitMode::SINGLE)
    return true;

  QMessageBox::warning(nullptr, tr("Input error"), tr("Data tables cannot be edited in double fit mode"));
  return false;
}

void HyperboleFittingEngine::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  StandardModeContextSettingsHandler::loadUserSettings(settings, *m_modeCtx.get());

  EMT::StringVariantMap map = settings.value<EMT::StringVariantMap>();
  if (map.contains(LAST_LOADSAVE_PATH_SETTINGS_TAG)) {
    const QVariant &v = map[LAST_LOADSAVE_PATH_SETTINGS_TAG];

    m_lastDataTablePath = v.toString();
  }

  if (map.contains(LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG)) {
    const QVariant &v = map[LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG];

    m_lastExportToCsvPath = v.toString();
    m_exportDTToCsvDlg->setLastPath(m_lastExportToCsvPath);
  }
}

QList<QStandardItem *> HyperboleFittingEngine::makeConcentrationsList(const Analyte::ConcentrationMap &data)
{
  QList<QStandardItem *> list;

  for (std::shared_ptr<Concentration> c : data) {
    QStandardItem *item = new QStandardItem(DoubleToStringConvertor::convert(c->concentration));
    item->setData(c->concentration, Qt::UserRole + 1);

    list.push_back(item);
  }

  return list;
}

QList<QStandardItem *> HyperboleFittingEngine::makeMobilitiesList(const Concentration::Mobilities &data)
{
  QList<QStandardItem *> list;

  for (const double d : data) {
    QStandardItem *item = new QStandardItem(DoubleToStringConvertor::convert(d));
    item->setData(d, Qt::UserRole + 1);

    list.push_back(item);
  }

  return list;
}

QAbstractItemModel *HyperboleFittingEngine::mobilitiesModel()
{
  return &m_mobilitiesModel;
}

void HyperboleFittingEngine::onAddAnalyte(const QString &name)
{
  QStandardItem *item;
  std::shared_ptr<Analyte> analyte;

  if (!isEditable())
    return;

  if (m_analytes.contains(name)) {
    QMessageBox::warning(nullptr, tr("Input error"), QString(tr("Analyte \"%1\" has already been added.")).arg(name));
    return;
  }

  try {
    analyte = std::shared_ptr<Analyte>(new Analyte(name));
    m_analytes.insert(name, analyte);
    item = new QStandardItem(name);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add analyte"));
    return;
  }

  item->setData(name, Qt::UserRole + 1);

  clearAllModels();

  m_analytesModel.appendRow(item);

  m_currentAnalyte = analyte;
  m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_A] = name;
  m_analyteNamesModel.notifyDataChanged(HyperboleFitParameters::String::ANALYTE_A, HyperboleFitParameters::String::ANALYTE_A);

  clearAnalyteASeries();

  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperboleFittingEngine::onAddConcentration(const double num)
{
  QStandardItem* item;
  std::shared_ptr<Concentration> c;

  if (!isEditable())
    return;

  if (m_currentAnalyte == nullptr) {
    QMessageBox::information(nullptr, tr("No analyte"), tr("No analyte has been selected. Select the analyte for which you wish to add the concentration first."));
    return;
  }

  if (num < 0.0) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Entered concentration value is nonsensical."));
    return;
  }

  if (m_currentAnalyte->concentrations.contains(num)) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("This concentration has already been added."));
    return;
  }

  try {
    c = std::shared_ptr<Concentration>(new Concentration(num));
    m_currentAnalyte->concentrations.insert(num, c);
    item = new QStandardItem(DoubleToStringConvertor::convert(num));
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add concentration"));
    return;
  }

  item->setData(num, Qt::UserRole + 1);

  m_mobilitiesModel.clear();

  m_concentrationsModel.appendRow(item);
  m_currentConcentration = c;
  m_currentConcentrationKey = num;
}

void HyperboleFittingEngine::onAddMobility(const double u)
{
  QStandardItem* item;

  if (!isEditable())
    return;


  if (m_currentConcentration == nullptr) {
    QMessageBox::information(nullptr, tr("No analyte"), tr("No concentration has been selected. Select the concentration for which you wish to add the migration time first."));
    return;
  }

  try {
    m_currentConcentration->addMobility(u);

    item = new QStandardItem(DoubleToStringConvertor::convert(u));
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add migration time"));
    return;
  }

  item->setData(u, Qt::UserRole + 1);

  m_mobilitiesModel.appendRow(item);

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperboleFittingEngine::onAnalyteSwitched(const QModelIndexList &inList)
{
  QStandardItem *item;
  QString name;
  QModelIndexList list = inList;

  if (list.isEmpty())
    return;

  item = m_analytesModel.item(list.first().row());
  if (item == nullptr)
    return;

  name = item->data().toString();

  if (!m_analytes.contains(name)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No analyte with name \"%1\" has been added.")).arg(name));
    return;
  }

  m_currentAnalyte = m_analytes[name];

  if (m_currentFitMode == FitMode::DOUBLE) {
    clearAnalyteBSeries();

    if (list.size() == 1) {
      invalidateAnalyteB();
    } else if (list.size() == 2) {
      item = m_analytesModel.item(list.at(1).row());
      if (item != nullptr) {
        QString name2 = item->data().toString();

        if (!m_analytes.contains(name2)) {
          QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No analyte with name \"%1\" has been added.")).arg(name2));
          return;
        }

        m_secondAnalyte = m_analytes[name2];
        m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_B] = name2;
        plotPoints(Series::POINTS_B, m_secondAnalyte);

        m_analyteNamesModel.notifyDataChanged(HyperboleFitParameters::String::ANALYTE_B, HyperboleFitParameters::String::ANALYTE_B);
      }
    } else if (list.size() > 2) {
        QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No more that two analytes can be selected at once.")).arg(name));
        return;
    }
  }
  m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_A] = name;
  m_analyteNamesModel.notifyDataChanged(HyperboleFitParameters::String::ANALYTE_A, HyperboleFitParameters::String::ANALYTE_A);

  clearAnalyteASeries();


  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);

  clearAllModels();
  invalidateCurrentConcentration();

  setConcentrationsList(makeConcentrationsList(m_currentAnalyte->concentrations));

  emit sortLists();
}

void HyperboleFittingEngine::onChartHorizontalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker)
{
  auto intersection = [](const QPointF &a, const QPointF &b, const double x) {
    const double k = (b.y() - a.y()) / (b.x() - a.x());

    return k * (x - a.x()) + a.y();
  };

  switch (marker) {
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    for (int idx = 1; idx < m_statData.size(); idx++) {
      if (m_statData.at(idx).x() >= m_verticalAMarkerPosition) {
        m_horizontalMarkerPosition = intersection(m_statData.at(idx - 1), m_statData.at(idx), m_verticalAMarkerPosition);
        break;
      }
    }
    break;
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    for (int idx = m_statData.size() - 2; idx >= 0; idx--) {
      if (m_statData.at(idx).x() <= m_verticalBMarkerPosition) {
        m_horizontalMarkerPosition = intersection(m_statData.at(idx), m_statData.at(idx + 1), m_verticalBMarkerPosition);
        break;
      }
    }
    break;
  default:
    return;
  }

  setMarkerPosition(HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER);
  emit chartHorizontalMarkerIntersectionSet(m_horizontalMarkerPosition);
}

void HyperboleFittingEngine::onChartVerticalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker)
{
  std::function<bool (const double, const double)> comparator;
  if (m_currentStatUnits == StatUnits::P_VALUE) {
    comparator = [](const double current, const double threshold) {
      return current >= threshold;
    };
  } else {
    comparator = [](const double current, const double threshold) {
      return current <= threshold;
    };
  }

  switch (marker) {
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    for (int idx = 1; idx < m_statData.size(); idx++) {
      if (comparator(m_statData.at(idx).y(), m_horizontalMarkerPosition)) {
        m_verticalAMarkerPosition = interpolateVerticalMarkerPosition(m_statData.at(idx - 1), m_statData.at(idx), m_horizontalMarkerPosition);

        setMarkerPosition(marker);
        emit chartVerticalMarkerIntersectionSet(marker, m_verticalAMarkerPosition);
        return;
      }
    }
    break;
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    for (int idx = m_statData.size() - 2; idx >= 0; idx--) {
      if (comparator(m_statData.at(idx).y(), m_horizontalMarkerPosition)) {
        m_verticalBMarkerPosition = interpolateVerticalMarkerPosition(m_statData.at(idx + 1), m_statData.at(idx), m_horizontalMarkerPosition);

        setMarkerPosition(marker);
        emit chartVerticalMarkerIntersectionSet(marker, m_verticalBMarkerPosition);
        return;
      }
    }
    break;
  default:
    break;
  }
}

void HyperboleFittingEngine::onChartMarkerValueChanged(const HyperboleFittingEngineMsgs::MarkerType marker, const double d)
{
  switch (marker) {
    case HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
      m_horizontalMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showHorizontalMarker) {
        setMarkerPosition(marker);
        m_modeCtx->replot();
      }
      break;
    case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
      m_verticalAMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showVerticalAMarker) {
        setMarkerPosition(marker);
        m_modeCtx->replot();
      }
      break;
    case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
      m_verticalBMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showVerticalBMarker) {
        setMarkerPosition(marker);
        m_modeCtx->replot();
      }
      break;
    default:
      break;
  }
}

void HyperboleFittingEngine::onConcentrationSwitched(const QModelIndex &idx)
{
  QStandardItem *item;
  double num;
  bool ok;

  if (m_currentAnalyte == nullptr)
    return;

  if (!idx.isValid())
    return;

  item = m_concentrationsModel.item(idx.row());
  if (item == nullptr)
    return;

  num = item->data(Qt::UserRole + 1).toDouble(&ok);
  if (!ok) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Invalid concentration key"));
    return;
  }

  if (!m_currentAnalyte->concentrations.contains(num)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("No such concentration has been added."));
    return;
  }

  if (num == m_currentConcentrationKey)
    return;

  m_mobilitiesModel.clear();

  m_currentConcentration = m_currentAnalyte->concentrations[num];
  m_currentConcentrationKey = num;

  setMobilitiesList(makeMobilitiesList(m_currentConcentration->mobilities()));
}

void HyperboleFittingEngine::onDeserialize()
{
  QFileDialog openDlg(nullptr, tr("Pick a data table to load"), m_lastDataTablePath);
  openDlg.setAcceptMode(QFileDialog::AcceptOpen);
  openDlg.setFileMode(QFileDialog::ExistingFile);
  openDlg.setNameFilters(m_dataTablesNameFilter);

  if (openDlg.exec() != QDialog::Accepted)
    return;

  QStringList files = openDlg.selectedFiles();
  if (files.length() < 1)
    return;

  QString filePath = files.at(0);

  doDeserialize(filePath);

  QDir d(filePath);
  m_lastDataTablePath = d.absolutePath();

  QFileInfo finfo(d.absolutePath());
  m_modeCtx->setPlotTitle(finfo.fileName());
}

void HyperboleFittingEngine::onDoEstimate()
{

  switch (m_currentFitMode) {
  case FitMode::SINGLE:
  {
    HypResults r;
    r = doSingleEstimate();
    displayHypResults(&r);
    break;
  }
  case FitMode::DOUBLE:
  {
    DoubleHypResults r;
    r = doDoubleEstimate();
    displayHypResults(&r);
    break;
  }
  default:
    return;
    break;
  }

}

void HyperboleFittingEngine::onDoFit()
{
  switch (m_currentFitMode) {
  case FitMode::SINGLE:
  {
    HypResults r = doSingleEstimate();
    if (!r.isValid())
      return;
    r = doSingleFit(r);
    if (!r.isValid()) {
      QMessageBox::warning(nullptr, tr("Regressor failure"), tr("Single fit regressor failed to converge. Try to increase the number of iterations and try again."));
      return;
    }

    displayHypResults(&r);
    break;
  }
  case FitMode::DOUBLE:
  {
    DoubleHypResults dr = doDoubleEstimate();
    if (!dr.isValid())
      return;
    dr = doDoubleFit(dr);
    if (!dr.isValid()) {
      QMessageBox::warning(nullptr, tr("Regressor failure"), tr("Double fit regressor failed to converge. Try to increase the number of iterations and try again."));
      return;
    }

    displayHypResults(&dr);
    break;
  }
  default:
    break;
  }
}

void HyperboleFittingEngine::onDoStats(const HyperboleStats::Intervals intr)
{
  QVector<QPointF> data;
  std::vector<std::pair<double, double> > out;
  double tau;
  double twoSided = false;

  switch (intr) {
  case HyperboleStats::Intervals::LEFT:
    tau = -TAU_MAX;
    break;
  case HyperboleStats::Intervals::BOTH:
    twoSided = true;
  case HyperboleStats::Intervals::RIGHT:
    tau = TAU_MAX;
    break;
  default:
    return;
  }

  switch (m_currentFitMode) {
  case FitMode::SINGLE:
  {
    echmet::regressCore::Profiler<double, double> MProfiler;

    MProfiler.nmax = TAU_MAXCOUNT;
    MProfiler.toTau = tau;
    MProfiler.twoSided = twoSided;

    msize_t paramId;

    switch (m_currentStatUnits) {
    case StatUnits::P_VALUE:
      MProfiler.converter = MProfiler.tau_to_pvalueT;
      break;
    case StatUnits::CONFIDENCE:
      MProfiler.converter = MProfiler.tau_to_confidenceT;
      break;
    case StatUnits::TAU:
      MProfiler.converter = MProfiler.tau_to_abs;
      break;
    default:
      return;
    }

    switch (m_currentStatMode) {
    case StatMode::MOBILITY_A:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperboleParams::u0);
      break;
    case StatMode::MOBILITY_CS_A:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperboleParams::uS);
      break;
    case StatMode::K_CS_A:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperboleParams::KS);
      break;
    default:
      return;
      break;
    }

    try {
      if (!MProfiler(out, *m_singleFitRegressor, paramId))
          QMessageBox::warning(nullptr, tr("Runtime error"), tr("Profiling failed."));
    } catch (...) {
        /* Nasty! */
        QMessageBox::critical(nullptr, tr("Runtime error"), tr("Unhandled error triggered by the profiler. Ingoring and attempting to recover!"));
    }

    break;
  }

  case FitMode::DOUBLE:
  {
    echmet::regressCore::Profiler<echmet::regressCore::RectangularHyperbole2XType<double>, double> MProfiler;

    MProfiler.nmax = TAU_MAXCOUNT;
    MProfiler.toTau = tau;
    MProfiler.twoSided = twoSided;

    msize_t paramId;

    switch (m_currentStatUnits) {
    case StatUnits::P_VALUE:
      MProfiler.converter = MProfiler.tau_to_pvalueT;
      break;
    case StatUnits::CONFIDENCE:
      MProfiler.converter = MProfiler.tau_to_confidenceT;
      break;
    case StatUnits::TAU:
      MProfiler.converter = MProfiler.tau_to_abs;
      break;
    default:
      return;
    }

    switch (m_currentStatMode) {
    case StatMode::MOBILITY_B:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::u0);
      break;
    case StatMode::MOBILITY_CS_B:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::uS);
      break;
    case StatMode::K_CS_B:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::KS);
      break;
    case StatMode::D_MOBILITY:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::du0);
      break;
    case StatMode::D_MOBILITY_CS:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::duS);
      break;
    case StatMode::D_K_CS:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbole2Params::dKS);
      break;
    default:
      return;
      break;
    }

    /* Reinitialize regressor */
    {
      DoubleHypResults r = doDoubleEstimate(true);
      doDoubleFit(r);
    }
    if (!MProfiler(out, *m_doubleFitRegressor, paramId))
      QMessageBox::warning(nullptr, tr("Runtime error"), tr("Profiling failed."));

    break;
  }
  default:
    break;
  }

  for (const auto& p : out)
    data.push_back(QPointF(p.first, p.second));

  {
    auto comparator = [](const QPointF &first, const QPointF &second) {
      return first.x() < second.x();
    };
    std::sort(data.begin(), data.end(), comparator);
  }

  hideDataSeries();
  showStatsSeries(m_currentStatUnits, m_currentStatMode);
  m_modeCtx->setSerieSamples(seriesIndex(Series::STATS), data);
  m_modeCtx->clearSerieSamples(seriesIndex(Series::HORIZONTAL_MARKER));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::VERTICAL_A_MARKER));
  m_modeCtx->clearSerieSamples(seriesIndex(Series::VERTICAL_B_MARKER));

  m_statData = data;
  if (m_statData.size() > 0) {
    const int halfSize = m_statData.size() / 2;

    if (m_showHorizontalMarker) {
      m_horizontalMarkerPosition = m_statData.at(halfSize).y();
      setMarkerPosition(HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER);
      emit chartHorizontalMarkerIntersectionSet(m_horizontalMarkerPosition);
    }
    if (m_showVerticalAMarker) {
      m_verticalAMarkerPosition = m_statData.at(halfSize).x();
      setMarkerPosition(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
      emit chartVerticalMarkerIntersectionSet(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, m_verticalAMarkerPosition);
    }
    if (m_showVerticalBMarker) {
      m_verticalBMarkerPosition = m_statData.at(halfSize).x();
      setMarkerPosition(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
      emit chartVerticalMarkerIntersectionSet(HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, m_verticalBMarkerPosition);
    }
  }

  m_modeCtx->replot();
}

void HyperboleFittingEngine::onEmergencySave()
{
  QString path = QDir::currentPath();

  if (m_analytes.size() > 0)
    doSerialize(path + "/" + EMERG_SAVE_FILE);
}

void HyperboleFittingEngine::onFitModeChanged(const QVariant &v)
{
  if (!v.canConvert<FitMode>())
    return;

  m_currentFitMode = v.value<FitMode>();

  switch (m_currentFitMode) {
  case FitMode::SINGLE:
    setSingleFitStats();
    clearAnalyteBSeries();
    invalidateAnalyteB();
    clearAnalyteASeries();
    plotPoints(Series::POINTS_A, m_currentAnalyte);

    m_statsForAnalyte = AnalyteId::ANALYTE_A;
    emit enableDoubleFit(false);
    break;
  case FitMode::DOUBLE:
    setDoubleFitStats();
    emit enableDoubleFit(true);
    break;
  }
}

void HyperboleFittingEngine::onNumberFormatChanged(const QLocale *oldLocale)
{
  Q_UNUSED(oldLocale);

  if (m_currentAnalyte == nullptr || m_currentConcentration == nullptr)
    return;

  m_concentrationsModel.clear();
  Analyte::ConcentrationMap::const_iterator cit = m_currentAnalyte->concentrations.cbegin();
  for (; cit != m_currentAnalyte->concentrations.cend(); cit++) {
    QStandardItem *item = new QStandardItem(DoubleToStringConvertor::convert(cit.key()));
    item->setData(cit.key(), Qt::UserRole + 1);
    m_concentrationsModel.appendRow(item);
  }

  m_mobilitiesModel.clear();
  setMobilitiesList(makeMobilitiesList(m_currentConcentration->mobilities()));
}

void HyperboleFittingEngine::onRedrawDataSeries()
{
  m_modeCtx->hideSerie(seriesIndex(Series::STATS));
  showDataSeries();
  m_modeCtx->replot();
}

void HyperboleFittingEngine::onRegisterMobility(const QString &name, const double selConcentration, const double mobility)
{
  std::shared_ptr<Analyte> analyte;
  std::shared_ptr<Concentration> c;

  if (name.length() < 1) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Name of the analyte cannot be empty."));
    return;
  }

  if (selConcentration < 0) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Concentration of the selector cannot be negative."));
    return;
  }

  if (!std::isfinite(mobility)) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Nonsensical value of mobility."));
    return;
  }

  if (!m_analytes.contains(name)) {
    try {
      analyte = std::shared_ptr<Analyte>(new Analyte(name));
      m_analytes.insert(name, analyte);
    } catch (std::bad_alloc&) {
      QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add analyte"));
      return;
    }
  } else
    analyte = m_analytes[name];

  if (!analyte->concentrations.contains(selConcentration)) {
    try {
      c = std::shared_ptr<Concentration>(new Concentration(selConcentration));
      analyte->concentrations.insert(selConcentration, c);
    } catch (std::bad_alloc&) {
      QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add concentration"));
      return;
    }
  } else
    c = analyte->concentrations[selConcentration];

  try {
    c->addMobility(mobility);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to add mobility"));
    return;
  }

  return;
}

void HyperboleFittingEngine::onRemoveAnalyte(const QModelIndex &idx)
{
  QStandardItem *item;
  QString name;

  if (!isEditable())
    return;

  if (!idx.isValid())
    return;

  item = m_analytesModel.item(idx.row());
  if (item == nullptr)
    return;

  name = item->data().toString();

  if (!m_analytes.contains(name)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No analyte with name \"%1\" has been added.")).arg(name));
    return;
  }

  clearAllModels();
  invalidateAll();

  m_analytes.remove(name);
  m_analytesModel.removeRow(idx.row());

  clearAnalyteASeries();
}

void HyperboleFittingEngine::onRemoveConcentration(const QModelIndex &idx)
{
  QStandardItem *item;
  double num;
  bool ok;

  if (!isEditable())
    return;

  if (m_currentAnalyte == nullptr)
    return;

  if (!idx.isValid())
    return;

  item = m_concentrationsModel.item(idx.row());
  if (item == nullptr)
    return;

  num = item->data().toDouble(&ok);
  if (!ok) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Invalid concentration key."));
    return;
  }

  if (!m_currentAnalyte->concentrations.contains(num)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Concentration \"%1\" has not been added.")).arg(num));
    return;
  }

  m_mobilitiesModel.clear();
  invalidateCurrentConcentration();

  m_currentAnalyte->concentrations.remove(num);
  m_concentrationsModel.removeRow(idx.row());

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperboleFittingEngine::onRemoveMobility(const QModelIndex &idx)
{
  if (!isEditable())
    return;

  if (!idx.isValid())
    return;

  if (m_currentConcentration == nullptr)
    return;

  m_currentConcentration->removeMobility(idx.row());
  m_mobilitiesModel.removeRow(idx.row());

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperboleFittingEngine::onRenameAnalyte(const QVariant &internalId, const QString &newName, const int idx)
{
  if (!isEditable())
    return;

  QString idString = internalId.toString();

  if (!m_analytes.contains(idString))
    return;

  if (m_analytes.contains(newName)) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Analyte of the name \"%1\" has already been added."));
    return;
  }

  std::shared_ptr<Analyte> newA;
  std::shared_ptr<Analyte> oldA = m_analytes[idString];

  try {
    newA = std::shared_ptr<Analyte>(new Analyte(newName, oldA));
    m_analytes.insert(newName, newA);
  } catch (std::bad_alloc& ) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to rename analyte."));
    return;
  }

  m_analytes.remove(idString);
  m_analytesModel.removeRow(idx);

  QStandardItem *item = new QStandardItem(newName);
  item->setData(newName);

  m_analytesModel.insertRow(idx, item);

  m_currentAnalyte = newA;
}

void HyperboleFittingEngine::onSecondAnalyteSameChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
  if (!roles.contains(Qt::EditRole))
    return;

  for (int idx = topLeft.column(); idx <= bottomRight.column(); idx++) {
    HyperboleFitParameters::Boolean i = m_fitFixedModel.itemFromIndex(idx);
    HyperboleFitResults::Floating fi = HyperboleFitResults::Floating::LAST_INDEX;

    bool same = m_fitFixedValues.at(i);

    if (same) {
      switch (i) {
      case HyperboleFitParameters::Boolean::FIXED_K_CS_B:
        m_fitResultsValues[HyperboleFitResults::Floating::K_CS_B] = m_fitResultsValues[HyperboleFitResults::Floating::K_CS_A];
        fi = HyperboleFitResults::Floating::K_CS_B;
        break;
      case HyperboleFitParameters::Boolean::FIXED_MOBILITY_B:
        m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_B] =  m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_A];
        fi = HyperboleFitResults::Floating::MOBILITY_B;
        break;
      case HyperboleFitParameters::Boolean::FIXED_MOBILITY_CS_B:
        m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_CS_B] = m_fitResultsValues[HyperboleFitResults::Floating::MOBILITY_CS_A];
        fi = HyperboleFitResults::Floating::MOBILITY_CS_B;
        break;
      default:
        return;
        break;
      }
    }

    emit m_fitResultsModel.dataChanged(m_fitResultsModel.index(0, m_fitResultsModel.indexFromItem(fi)),
                                       m_fitResultsModel.index(0, m_fitResultsModel.indexFromItem(fi)),
                                       { Qt::DisplayRole });
  }
}

void HyperboleFittingEngine::onSerialize()
{
  QFileDialog saveDlg(nullptr, tr("Pick a data table to save"), m_lastDataTablePath);
  saveDlg.setAcceptMode(QFileDialog::AcceptSave);
  saveDlg.setNameFilters(m_dataTablesNameFilter);

  if (saveDlg.exec() != QDialog::Accepted)
    return;

  QStringList files = saveDlg.selectedFiles();
  if (files.length() < 1)
    return;

  QString filePath = files.at(0);
  if (!filePath.endsWith("." + DATA_TABLE_FILE_SUFFIX))
    filePath.append("." + DATA_TABLE_FILE_SUFFIX);

  doSerialize(filePath);

  QDir d(filePath);
  m_lastDataTablePath = d.absolutePath();
}

void HyperboleFittingEngine::onShowChartMarker(const HyperboleFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value)
{
  bool ok;

  value.toDouble(&ok);
  if (ok) {
    const double d = value.toDouble();

    switch (marker) {
    case HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
      m_horizontalMarkerPosition = d;
      break;
    case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
      m_verticalAMarkerPosition = d;
      break;
    case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
      m_verticalBMarkerPosition = d;
      break;
    default:
      break;
    }
  }

  switch (marker) {
  case HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
    m_showHorizontalMarker = visible;
    if (m_showHorizontalMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_modeCtx->showSerie(seriesIndex(Series::HORIZONTAL_MARKER));
        m_modeCtx->replot();
      }
    } else {
      m_modeCtx->hideSerie(seriesIndex(Series::HORIZONTAL_MARKER));
      m_modeCtx->replot();
    }
    break;
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    m_showVerticalAMarker = visible;
    if (m_showVerticalAMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_modeCtx->showSerie(seriesIndex(Series::VERTICAL_A_MARKER));
        m_modeCtx->replot();
      }
    } else {
      m_modeCtx->hideSerie(seriesIndex(Series::VERTICAL_A_MARKER));
      m_modeCtx->replot();
    }
    break;
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    m_showVerticalBMarker = visible;
    if (m_showVerticalBMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_modeCtx->showSerie(seriesIndex(Series::VERTICAL_B_MARKER));
        m_modeCtx->replot();
      }
    } else {
      m_modeCtx->hideSerie(seriesIndex(Series::VERTICAL_B_MARKER));
      m_modeCtx->replot();
    }
    break;
  default:
    break;
  }
}

void HyperboleFittingEngine::onStatModeChanged(const QVariant &v)
{
  if (!v.canConvert<StatMode>())
    return;

  m_currentStatMode = v.value<StatMode>();
}

void HyperboleFittingEngine::onStatsForAnalyteChanged(const HyperboleFittingEngineMsgs::AnalyteId aId)
{
  if (m_currentFitMode != FitMode::DOUBLE)
    return;

  switch (aId) {
  case HyperboleFittingEngineMsgs::AnalyteId::ANALYTE_A:
    m_statsForAnalyte = AnalyteId::ANALYTE_A;
    break;
  case HyperboleFittingEngineMsgs::AnalyteId::ANALYTE_B:
    m_statsForAnalyte = AnalyteId::ANALYTE_B;
    break;
  default:
    break;
  }
}

void HyperboleFittingEngine::onStatUnitsChanged(const QVariant &v)
{
  if (!v.canConvert<StatUnits>())
    return;

  m_currentStatUnits = v.value<StatUnits>();
}

void HyperboleFittingEngine::plotCurve(const Series s, const QVector<QPointF> &data)
{
  m_modeCtx->setSerieSamples(seriesIndex(s), data);

  m_modeCtx->replot();
}

void HyperboleFittingEngine::plotDoubleCurve(const DoubleHypResults &dr)
{
    QVector<QPointF> curveA;
    for (double x = 0.0; x <= dr.maxX_A; x += (dr.maxX_A - 0.0) / 100.0){
        echmet::regressCore::RectangularHyperbole2XType<double> mx(0, x);
        curveA.push_back(QPointF(x, (*m_doubleFitRegressor)(mx)));
    }

    QVector<QPointF> curveB;
    for (double x = 0.0; x <= dr.maxX_A; x += (dr.maxX_A - 0.0) / 100.0){
        echmet::regressCore::RectangularHyperbole2XType<double> mx(1, x);
        curveB.push_back(QPointF(x, (*m_doubleFitRegressor)(mx)));
    }

    m_modeCtx->setSerieSamples(seriesIndex(Series::FIT_A_CURVE), curveA);
    m_modeCtx->setSerieSamples(seriesIndex(Series::FIT_B_CURVE), curveB);

    m_modeCtx->replot();
}

void HyperboleFittingEngine::plotPoints(const Series s, std::shared_ptr<const Analyte> a)
{
  QVector<QPointF> points;
  QVector<QPointF> average;

  if (a != nullptr) {
    for (std::shared_ptr<const Concentration> c : a->concentrations) {
      const double x = c->concentration;

      if (c->mobilities().length() < 1)
        continue;

      for (const double t : c->mobilities())
        points.push_back(QPointF(x, t));

      average.push_back(QPointF(x, c->avgMobility()));
    }
  }

  m_modeCtx->setSerieSamples(seriesIndex(s), points);
  m_modeCtx->setSerieSamples(seriesIndex(s) + 1, average);
  m_modeCtx->replot();
}

void HyperboleFittingEngine::plotSingleCurve(const HypResults &r)
{
  QVector<QPointF> curve;
  for (double x = 0.0; x <= r.maxX_A; x += (r.maxX_A - 0.0) / 100.0)
    curve.push_back(QPointF(x, (*m_singleFitRegressor)(x)));

  m_modeCtx->setSerieSamples(seriesIndex(Series::FIT_A_CURVE), curve);
  m_modeCtx->replot();
}

void HyperboleFittingEngine::refreshModels()
{
  m_analytesModel.clear();

  for (auto &a : m_analytes) {
    QStandardItem *item = new QStandardItem(a->name);
    item->setData(a->name);
    m_analytesModel.appendRow(item);
  }

  m_concentrationsModel.clear();
  m_mobilitiesModel.clear();
  m_modeCtx->clearAllSerieSamples();
  m_modeCtx->replot();

  m_currentAnalyte = nullptr;
  m_secondAnalyte = nullptr;

  m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_A] = "";
  m_analyteNamesValues[HyperboleFitParameters::String::ANALYTE_B] = "";

  m_analyteNamesModel.notifyDataChanged(HyperboleFitParameters::String::ANALYTE_A, HyperboleFitParameters::String::ANALYTE_B);
}

QVariant HyperboleFittingEngine::saveUserSettings() const
{
  EMT::StringVariantMap map = StandardModeContextSettingsHandler::saveUserSettings(*m_modeCtx.get(), seriesIndex(Series::LAST_INDEX));

  map[LAST_LOADSAVE_PATH_SETTINGS_TAG] = m_lastDataTablePath;
  map[LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG] = m_lastExportToCsvPath;

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

int constexpr HyperboleFittingEngine::seriesIndex(const Series s)
{
  return static_cast<int>(s);
}

void HyperboleFittingEngine::setConcentrationsList(const QList<QStandardItem *> &list)
{
  for (QStandardItem *const item : list)
    m_concentrationsModel.appendRow(item);
}

void HyperboleFittingEngine::setDoubleFitStats()
{
  m_statModeModel.clear();
  try {
    {
    QStandardItem *freeMobB = new QStandardItem(s_uACaption);
    freeMobB->setData(QVariant::fromValue(StatMode::MOBILITY_B), Qt::UserRole + 1);
    m_statModeModel.appendRow(freeMobB);

    QStandardItem *compMobB = new QStandardItem(s_uCSCaption);
    compMobB->setData(QVariant::fromValue(StatMode::MOBILITY_CS_B), Qt::UserRole + 1);
    m_statModeModel.appendRow(compMobB);

    QStandardItem *kcsB = new QStandardItem(s_KCSCaption);
    kcsB->setData(QVariant::fromValue(StatMode::K_CS_B), Qt::UserRole + 1);
    m_statModeModel.appendRow(kcsB);
    }

    {
    QStandardItem *freeMobD = new QStandardItem(s_uACaption +  tr(", difference"));
    freeMobD->setData(QVariant::fromValue(StatMode::D_MOBILITY), Qt::UserRole + 1);
    m_statModeModel.appendRow(freeMobD);

    QStandardItem *compMobD = new QStandardItem(s_uCSCaption + tr(", difference"));
    compMobD->setData(QVariant::fromValue(StatMode::D_MOBILITY_CS), Qt::UserRole + 1);
    m_statModeModel.appendRow(compMobD);

    QStandardItem *kcsB = new QStandardItem(s_KCSCaption + tr(", difference"));
    kcsB->setData(QVariant::fromValue(StatMode::D_K_CS), Qt::UserRole + 1);
    m_statModeModel.appendRow(kcsB);
    }
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to initialize statUnitsModel"));
    throw;
  }

  m_currentStatMode = StatMode::MOBILITY_B;
}

void HyperboleFittingEngine::setMarkerPosition(const HyperboleFittingEngineMsgs::MarkerType marker)
{
  const QRectF r = m_modeCtx->range();

  switch (marker) {
  case HyperboleFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(r.topLeft().x(), m_horizontalMarkerPosition));
    points.push_back(QPointF(r.topRight().x(), m_horizontalMarkerPosition));

    m_modeCtx->setSerieSamples(seriesIndex(Series::HORIZONTAL_MARKER), points);
    break;
  }
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(m_verticalAMarkerPosition, r.topLeft().y()));
    points.push_back(QPointF(m_verticalAMarkerPosition, r.bottomLeft().y()));

    m_modeCtx->setSerieSamples(seriesIndex(Series::VERTICAL_A_MARKER), points);
    break;
  }
  case HyperboleFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(m_verticalBMarkerPosition, r.topLeft().y()));
    points.push_back(QPointF(m_verticalBMarkerPosition, r.bottomLeft().y()));

    m_modeCtx->setSerieSamples(seriesIndex(Series::VERTICAL_B_MARKER), points);
    break;
  }
  default:
    return;
  }
}

void HyperboleFittingEngine::setMobilitiesList(const QList<QStandardItem *> &list)
{
  for (QStandardItem *const item : list)
    m_mobilitiesModel.appendRow(item);
}

void HyperboleFittingEngine::setSingleFitStats()
{
  m_statModeModel.clear();
  try {
    QStandardItem *freeMobA = new QStandardItem(s_uACaption);
    freeMobA->setData(QVariant::fromValue(StatMode::MOBILITY_A), Qt::UserRole + 1);
    m_statModeModel.appendRow(freeMobA);

    QStandardItem *compMobA = new QStandardItem(s_uCSCaption);
    compMobA->setData(QVariant::fromValue(StatMode::MOBILITY_CS_A), Qt::UserRole + 1);
    m_statModeModel.appendRow(compMobA);

    QStandardItem *kcsA = new QStandardItem(s_KCSCaption);
    kcsA->setData(QVariant::fromValue(StatMode::K_CS_A), Qt::UserRole + 1);
    m_statModeModel.appendRow(kcsA);
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to initialize statUnitsModel"));
    throw;
  }

  m_currentStatMode = StatMode::MOBILITY_A;
}

void HyperboleFittingEngine::showDataSeries()
{
  m_modeCtx->hideSerie(seriesIndex(Series::STATS));
  m_modeCtx->hideSerie(seriesIndex(Series::HORIZONTAL_MARKER));
  m_modeCtx->hideSerie(seriesIndex(Series::VERTICAL_A_MARKER));
  m_modeCtx->hideSerie(seriesIndex(Series::VERTICAL_B_MARKER));

  m_modeCtx->showSerie(seriesIndex(Series::FIT_A_CURVE));
  m_modeCtx->showSerie(seriesIndex(Series::FIT_B_CURVE));
  m_modeCtx->showSerie(seriesIndex(Series::POINTS_A));
  m_modeCtx->showSerie(seriesIndex(Series::POINTS_B));
  m_modeCtx->showSerie(seriesIndex(Series::POINTS_A_AVG));
  m_modeCtx->showSerie(seriesIndex(Series::POINTS_B_AVG));

  m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, tr("Mobility"));
  m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, tr("Selector concentration"));

  m_viewMode = ViewMode::DATA;
}

void HyperboleFittingEngine::showStatsSeries(const StatUnits units, const StatMode mode)
{
  switch (units) {
  case StatUnits::P_VALUE:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_pValueCaption);
    break;
  case StatUnits::CONFIDENCE:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_confidenceCaption);
    break;
  case StatUnits::TAU:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_tauCaption);
    break;
  default:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, tr("---"));
    break;
  }

  switch (mode) {
  case StatMode::MOBILITY_A:
  case StatMode::MOBILITY_B:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uACaption);
    break;
  case StatMode::D_MOBILITY:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uACaption + tr(", difference"));
    break;
  case StatMode::MOBILITY_CS_A:
  case StatMode::MOBILITY_CS_B:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uCSCaption);
    break;
  case StatMode::D_MOBILITY_CS:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uCSCaption + tr(", difference"));
    break;
  case StatMode::K_CS_A:
  case StatMode::K_CS_B:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_KCSCaption);
    break;
  case StatMode::D_K_CS:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_KCSCaption + tr(", difference"));
    break;
  default:
    m_modeCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "---");
    break;
  }

  m_modeCtx->showSerie(seriesIndex(Series::STATS));

  if (m_showHorizontalMarker)
    m_modeCtx->showSerie(seriesIndex(Series::HORIZONTAL_MARKER));
  if (m_showVerticalAMarker)
    m_modeCtx->showSerie(seriesIndex(Series::VERTICAL_A_MARKER));
  if (m_showVerticalBMarker)
    m_modeCtx->showSerie(seriesIndex(Series::VERTICAL_B_MARKER));

  m_viewMode = ViewMode::STATS;
}

QAbstractItemModel *HyperboleFittingEngine::statModeModel()
{
  return &m_statModeModel;
}

QAbstractItemModel *HyperboleFittingEngine::statUnitsModel()
{
  return &m_statUnitsModel;
}
