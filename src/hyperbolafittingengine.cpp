#include "hyperbolafittingengine.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QTextStream>
#include <functional>
#include "custommetatypes.h"
#include "globals.h"
#include "hyperbolafititems.h"
#include "math/regressor/profiler.h"
#include "doubletostringconvertor.h"
#include "standardplotcontextsettingshandler.h"
#include "gui/exportdatatabletocsvdialog.h"
#include "math/regressor/hyperbola.h"
#include "math/regressor/hyperbola2.h"

#define TAU_MAX      5.0
#define TAU_MAXCOUNT 600

using namespace echmet;
using namespace regressCore;

typedef RectangularHyperbola2<double, double>::x_type hyp2x_type;

const QString HyperbolaFittingEngine::s_dataPointsATitle = QObject::tr("Points A");
const QString HyperbolaFittingEngine::s_dataPointsBTitle = QObject::tr("Points B");
const QString HyperbolaFittingEngine::s_dataPointsAAvgTitle = QObject::tr("Average of points A");
const QString HyperbolaFittingEngine::s_dataPointsBAvgTitle = QObject::tr("Average of points B");
const QString HyperbolaFittingEngine::s_fitCurveATitle = QObject::tr("Fit A");
const QString HyperbolaFittingEngine::s_fitCurveBTitle = QObject::tr("Fit B");
const QString HyperbolaFittingEngine::s_fitCurveStatsTitle = QObject::tr("Statistics");
const QString HyperbolaFittingEngine::s_horizontalMarkerTitle = QObject::tr("Horizontal marker");
const QString HyperbolaFittingEngine::s_verticalAMarkerTitle = QObject::tr("Vertical A marker");
const QString HyperbolaFittingEngine::s_verticalBMarkerTitle = QObject::tr("Vertical B marker");

const QString HyperbolaFittingEngine::s_uACaption = QObject::tr("Free mobility (\xCE\xBC (A))");
const QString HyperbolaFittingEngine::s_uCSCaption = QObject::tr("Mobility of complex (\xCE\xBC (AS))");
const QString HyperbolaFittingEngine::s_KCSCaption = QObject::tr("Complexation constant (K (AS))");
const QString HyperbolaFittingEngine::s_tauCaption = QObject::tr("Tau (\xCF\x84)");
const QString HyperbolaFittingEngine::s_confidenceCaption = QObject::tr("Confidence");
const QString HyperbolaFittingEngine::s_pValueCaption = QObject::tr("P-value");


const double HyperbolaFittingEngine::s_defaultViscositySlope = 0.0;
const double HyperbolaFittingEngine::s_defaultEpsilon = 1.0e-9;
const int HyperbolaFittingEngine::s_defaultMaxIterations = 50;

const QString HyperbolaFittingEngine::CSV_FILE_SUFFIX("csv");
const QString HyperbolaFittingEngine::DATA_TABLE_FILE_SUFFIX("evd");
const QString HyperbolaFittingEngine::EMERG_SAVE_FILE("recovery." + DATA_TABLE_FILE_SUFFIX);
const double HyperbolaFittingEngine::INVAL_CONC_KEY(-1.0);
const QString HyperbolaFittingEngine::INVAL_ANALYTE_KEY("");

const QString HyperbolaFittingEngine::LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG("LastExportToCsvPath");
const QString HyperbolaFittingEngine::LAST_LOADSAVE_PATH_SETTINGS_TAG("LastLoadSavePath");

const char * HyperbolaFittingEngine::regressor_initialization_error::what() const noexcept
{
  if (m_what != nullptr)
    return m_what;

  return "Unable to initialize regressor";
}

HyperbolaFittingEngine::Analyte::Analyte(const QString &name) :
  name(name)
{
}

HyperbolaFittingEngine::Analyte::Analyte(const QString &name, std::shared_ptr<const Analyte> other) :
  concentrations(other->concentrations),
  name(name)
{
}

EvalSerializable::RetCode HyperbolaFittingEngine::SerializableConcentration::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;
  ret = Manipulator.Operate(c);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(mobilities);
}

std::string HyperbolaFittingEngine::SerializableConcentration::TypeName() const
{
  return "SerializableConcentration";
}

EvalSerializable::RetCode HyperbolaFittingEngine::SerializableAnalyte::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;
  ret = Manipulator.Operate(name);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(concentrations);
}

std::string HyperbolaFittingEngine::SerializableAnalyte::TypeName() const
{
  return "SerializableAnalyte";
}

EvalSerializable::RetCode HyperbolaFittingEngine::SerializableDataTable::Operate(DataManipulator &Manipulator)
{
  EvalSerializable::RetCode ret;

  ret = Manipulator.Header();
  if (ret != EvalSerializable::SUCCESS) return ret;

  ret = Manipulator.Operate(viscositySlope);
  if (ret != EvalSerializable::SUCCESS) return ret;

  return Manipulator.Operate(analytes);
}

std::string HyperbolaFittingEngine::SerializableDataTable::TypeName() const
{
  return "SerializableDataTable";
}

HyperbolaFittingEngine::Concentration::Concentration(const double concentration) :
  concentration(concentration),
  m_avgMobility(0.0)
{
}

HyperbolaFittingEngine::Concentration::Concentration(const double concentration, const Concentration &other) :
  concentration(concentration),
  m_avgMobility(other.m_avgMobility),
  m_mobilities(other.m_mobilities)
{
}

void HyperbolaFittingEngine::Concentration::addMobility(const double mobility)
{
  m_mobilities.push_back(mobility);

  recalculateAverage();
}

double HyperbolaFittingEngine::Concentration::avgMobility() const
{
  return m_avgMobility;
}

const HyperbolaFittingEngine::Concentration::Mobilities &HyperbolaFittingEngine::Concentration::mobilities() const
{
  return m_mobilities;
}

void HyperbolaFittingEngine::Concentration::removeMobility(const int idx)
{
  if (idx < 0 || idx >= m_mobilities.length())
    return;

  m_mobilities.remove(idx);

  recalculateAverage();
}

bool HyperbolaFittingEngine::Concentration::updateMobility(const double u, const int idx)
{
  if (idx < 0 || idx >= m_mobilities.length())
    return false;

  m_mobilities[idx] = u;
  recalculateAverage();

  return true;
}

void HyperbolaFittingEngine::Concentration::recalculateAverage()
{
  double sd = 0.0;

  if (m_mobilities.length() < 1)
    return;

  for (const double d : m_mobilities)
    sd += d;

  m_avgMobility = sd / m_mobilities.length();
}

HyperbolaFittingEngine::HypResults::HypResults(const double u0, const double uCS, const double KCS, const double maxX, const double sigma, const int iterations) :
  u0_A(u0), uCS_A(uCS), KCS_A(KCS), maxX_A(maxX),
  sigma(sigma), iterations(iterations),
  m_valid(true)
{
}

HyperbolaFittingEngine::HypResults::HypResults() :
  u0_A(0.0), uCS_A(0.0), KCS_A(0.0), maxX_A(0.0), sigma(0.0), iterations(0),
  m_valid(false)
{
}

HyperbolaFittingEngine::HypResults::HypResults(const HypResults &other) :
  u0_A(other.u0_A),
  uCS_A(other.uCS_A),
  KCS_A(other.KCS_A),
  maxX_A(other.maxX_A),
  sigma(other.sigma),
  iterations(other.iterations),
  m_valid(other.m_valid)
{
}

bool HyperbolaFittingEngine::HypResults::isValid() const
{
  return m_valid;
}

HyperbolaFittingEngine::HypResults &HyperbolaFittingEngine::HypResults::operator=(const HypResults &other)
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

HyperbolaFittingEngine::DoubleHypResults::DoubleHypResults(const double u0_A, const double uCS_A, const double KCS_A, const double maxX_A,
                                                           const double sigma, const int iterations,
                                                           const double u0_B, const double uCS_B, const double KCS_B) :
  HypResults(u0_A, uCS_A, KCS_A, maxX_A, sigma, iterations),
  u0_B(u0_B), uCS_B(uCS_B), KCS_B(KCS_B)
{
}

HyperbolaFittingEngine::DoubleHypResults::DoubleHypResults() :
  HypResults(),
  u0_B(0.0), uCS_B(0.0), KCS_B(0.0)
{
}

HyperbolaFittingEngine::DoubleHypResults::DoubleHypResults(const DoubleHypResults &other) :
  HypResults(other),
  u0_B(other.u0_B),
  uCS_B(other.uCS_B),
  KCS_B(other.KCS_B)
{
}

HyperbolaFittingEngine::DoubleHypResults &HyperbolaFittingEngine::DoubleHypResults::operator=(const DoubleHypResults &other)
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

HyperbolaFittingEngine::HyperbolaFittingEngine(QObject *parent) :
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
  m_dataTablesNameFilter(QStringList() << Globals::SOFTWARE_NAME + " Data table (*." + HyperbolaFittingEngine::DATA_TABLE_FILE_SUFFIX + ")" << "Any file (*.*)"),
  m_exportToCsvNameFilter(QStringList() << "Comma separated values (*." + HyperbolaFittingEngine::CSV_FILE_SUFFIX + ")")
{
  initFitModeModel();
  m_currentFitMode = FitMode::SINGLE;
  initStatUnitsModel();
  m_currentStatUnits = StatUnits::TAU;

  setSingleFitStats();
  m_currentStatMode = StatMode::MOBILITY_A;

  m_fitFloatValues[HyperbolaFitParameters::Floating::VISCOSITY_SLOPE] = s_defaultViscositySlope;
  m_fitFloatValues[HyperbolaFitParameters::Floating::EPSILON] = s_defaultEpsilon;
  m_fitIntValues[HyperbolaFitParameters::Int::MAX_ITERATIONS] = s_defaultMaxIterations;

  m_fitFixedModel.setUnderlyingData(m_fitFixedValues.pointer());
  m_fitFloatModel.setUnderlyingData(m_fitFloatValues.pointer());
  m_fitIntModel.setUnderlyingData(m_fitIntValues.pointer());
  m_fitResultsModel.setUnderlyingData(m_fitResultsValues.pointer());
  m_analyteNamesModel.setUnderlyingData(m_analyteNamesValues.pointer());

  createContextMenu();

  m_singleFitRegressor = new echmet::regressCore::RectangularHyperbola<double, double>();
  m_doubleFitRegressor = new echmet::regressCore::RectangularHyperbola2<double, double>();

  m_exportDTToCsvDlg = new ExportDatatableToCsvDialog(m_exportToCsvNameFilter, m_lastExportToCsvPath);
  if (m_exportDTToCsvDlg == nullptr)
    throw std::bad_alloc();

  connect(&m_fitFixedModel, &BooleanMapperModel<HyperbolaFitParameters::Boolean>::dataChanged, this, &HyperbolaFittingEngine::onSecondAnalyteSameChanged);

  typedef Eigen::MatrixXd MatrixD;

  /* Preinitialize regressors */
  {
    Eigen::VectorXd mat_x(1, 1);
    MatrixD         mat_y(1, 1);
    bool ret;

    mat_x[0] = 0;
    mat_y(0,0) = 0;

    ret = m_singleFitRegressor->Initialize(mat_x, mat_y,
                                           m_fitFloatValues.at(HyperbolaFitParameters::Floating::EPSILON),
                                           m_fitIntValues.at(HyperbolaFitParameters::Int::MAX_ITERATIONS),
                                           true, 0.0,
                                           m_fitFloatValues.at(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE));
    if (!ret)
        throw regressor_initialization_error("Failed to preinitialize single fit regressor");
  }
  {
    Vector<hyp2x_type> mat_x(1, 1);
    MatrixD            mat_y(1, 1);
    bool ret;

    mat_x[0] = 0;
    mat_y(0,0) = 0;

    ret = m_doubleFitRegressor->Initialize(mat_x, mat_y,
                                           m_fitFloatValues.at(HyperbolaFitParameters::Floating::EPSILON),
                                           m_fitIntValues.at(HyperbolaFitParameters::Int::MAX_ITERATIONS),
                                           true, 0.0, 0.0,
                                           m_fitFloatValues.at(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE));

    if (!ret)
      throw regressor_initialization_error("Failed to preinitialize double fit regressor");
  }

  DoubleToStringConvertor::notifyOnFormatChanged(this);
}

HyperbolaFittingEngine::~HyperbolaFittingEngine()
{
  delete m_singleFitRegressor;
  delete m_doubleFitRegressor;
  delete m_exportDTToCsvDlg;
  delete m_plotCtxMenu;
}

AbstractMapperModel<QString, HyperbolaFitParameters::String> *HyperbolaFittingEngine::analyteNamesModel()
{
  return &m_analyteNamesModel;
}

QAbstractItemModel *HyperbolaFittingEngine::analytesModel()
{
  return &m_analytesModel;
}

void HyperbolaFittingEngine::assignContext(std::shared_ptr<PlotContextLimited> ctx)
{
  m_plotCtx = ctx;

  if (!m_plotCtx->addSerie(seriesIndex(Series::POINTS_A), s_dataPointsATitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(PlotContextLimited::DEFAULT_POINT_TYPE, PlotContextLimited::DEFAULT_POINT_SIZE, Qt::black),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsATitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::POINTS_B), s_dataPointsBTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(QwtSymbol::Style::XCross, PlotContextLimited::DEFAULT_POINT_SIZE, QColor(142, 87, 172)),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsBTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::POINTS_A_AVG), s_dataPointsAAvgTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(PlotContextLimited::DEFAULT_CENTRAL_POINT_TYPE, PlotContextLimited::DEFAULT_CENTRAL_POINT_SIZE, Qt::red),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsAAvgTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::POINTS_B_AVG), s_dataPointsBAvgTitle,
                           SerieProperties::VisualStyle(QPen(Qt::black),
                                                      SerieProperties::SQwtSymbol(QwtSymbol::Style::XCross, PlotContextLimited::DEFAULT_CENTRAL_POINT_SIZE, QColor(77, 119, 183)),
                                                      QwtPlotCurve::CurveStyle::NoCurve)))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_dataPointsBAvgTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::FIT_A_CURVE), s_fitCurveATitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveATitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::FIT_B_CURVE), s_fitCurveBTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(77, 119, 183), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveBTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::STATS), s_fitCurveStatsTitle, SerieProperties::VisualStyle(QPen(QBrush(QColor(130, 190, 73), Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_fitCurveStatsTitle));

  if (!m_plotCtx->addSerie(seriesIndex(Series::HORIZONTAL_MARKER), s_horizontalMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_horizontalMarkerTitle));
  if (!m_plotCtx->addSerie(seriesIndex(Series::VERTICAL_A_MARKER), s_verticalAMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_verticalAMarkerTitle));
  if (!m_plotCtx->addSerie(seriesIndex(Series::VERTICAL_B_MARKER), s_verticalAMarkerTitle, SerieProperties::VisualStyle(QPen(QBrush(Qt::black, Qt::SolidPattern), PlotContextLimited::DEFAULT_SERIES_WIDTH))))
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Cannot create serie for %1 plot. The serie will not be displayed.")).arg(s_verticalBMarkerTitle));


  /* Default axis fonts*/
  m_plotCtx->setAxisFont(SerieProperties::Axis::X_BOTTOM, QFont());
  m_plotCtx->setAxisFont(SerieProperties::Axis::Y_LEFT, QFont());

  connect(m_plotCtx.get(), &PlotContextLimited::pointSelected, this, &HyperbolaFittingEngine::onPlotPointSelected);

  showDataSeries();
}

void HyperbolaFittingEngine::checkForCrashRecovery()
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

void HyperbolaFittingEngine::clearAllModels()
{
  m_mobilitiesModel.clear();
  m_concentrationsModel.clear();
}

void HyperbolaFittingEngine::clearAnalyteASeries()
{
  m_plotCtx->clearSerieSamples(seriesIndex(Series::POINTS_A));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::POINTS_A_AVG));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FIT_A_CURVE));
}

void HyperbolaFittingEngine::clearAnalyteBSeries()
{
  m_plotCtx->clearSerieSamples(seriesIndex(Series::POINTS_B));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::POINTS_B_AVG));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FIT_B_CURVE));
}

QAbstractItemModel *HyperbolaFittingEngine::concentrationsModel()
{
  return &m_concentrationsModel;
}

void HyperbolaFittingEngine::createContextMenu() noexcept(false)
{
  m_plotCtxMenu = new QMenu();

  QAction *a;

  a = new QAction(tr("Scale plot axes to fit"), m_plotCtxMenu);
  a->setData(QVariant::fromValue<PlotCtxMenuActions>(PlotCtxMenuActions::SCALE_PLOT_TO_FIT));
  m_plotCtxMenu->addAction(a);
}

bool HyperbolaFittingEngine::doDeserialize(const QString &path)
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

      for (const SerializableConcentration &sc : sa.concentrations) {
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

  m_fitFloatValues[HyperbolaFitParameters::Floating::VISCOSITY_SLOPE] = table.viscositySlope;
  m_fitFloatModel.notifyDataChanged(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE, HyperbolaFitParameters::Floating::VISCOSITY_SLOPE, { Qt::EditRole });

  refreshModels();
  return true;
}

void HyperbolaFittingEngine::displayHypResults(const HypResults *r)
{
  if (!r->isValid())
    return;

  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_A] = r->u0_A;
  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_A] = r->uCS_A;
  m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_A] = r->KCS_A;
  m_fitResultsValues[HyperbolaFitResults::Floating::SIGMA] = r->sigma;
  m_fitResultsValues[HyperbolaFitResults::Floating::ITERATIONS] = r->iterations;

  m_fitResultsModel.notifyDataChanged(HyperbolaFitResults::Floating::SIGMA, HyperbolaFitResults::Floating::K_CS_A, { Qt::EditRole });
  m_fitResultsModel.notifyDataChanged(HyperbolaFitResults::Floating::ITERATIONS, HyperbolaFitResults::Floating::ITERATIONS, { Qt::EditRole });

  showDataSeries();

  switch (m_currentFitMode) {
  case FitMode::SINGLE:
    plotSingleCurve(*r);
    break;
  case FitMode::DOUBLE:
  {
    const DoubleHypResults *dr = static_cast<const DoubleHypResults *>(r);

    m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_B] = dr->u0_B;
    m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_B] = dr->uCS_B;
    m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_B] = dr->KCS_B;

    m_fitResultsModel.notifyDataChanged(HyperbolaFitResults::Floating::MOBILITY_B, HyperbolaFitResults::Floating::K_CS_B, { Qt::EditRole });

    plotDoubleCurve(*dr);
  }
  }
}

HyperbolaFittingEngine::DoubleHypResults HyperbolaFittingEngine::doDoubleEstimate(const bool usedForStats = false)
{
  double maxX = std::numeric_limits<double>::min();
  /* Prevent any ptr vs. var mishaps */
  echmet::regressCore::RectangularHyperbola2<double, double> &dfrRef = *m_doubleFitRegressor;

  typedef Eigen::VectorXd MatrixD;

  Vector<hyp2x_type> mat_x;
  MatrixD            mat_y;

  if (m_currentAnalyte == nullptr || m_secondAnalyte == nullptr)
    return DoubleHypResults();

  if (m_currentAnalyte->concentrations.size() < 1 ||
      m_secondAnalyte->concentrations.size() < 1)
    return DoubleHypResults();

  if (!m_currentAnalyte->concentrations.contains(0.0)) {
    double u0 = m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_A);
    int ret = QMessageBox::question(nullptr, tr("No zero concentration"), QString(tr("List of entered concentrations does not contain any data for zero concentration of the selector.\n"
                                                                                     "Do you want to use the current value of \xCE\xBC = %1 as the estimate?")).arg(u0),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
      return DoubleHypResults();
  }

  /* Sry :-( */
  const Analyte::ConcentrationMap &cs1 = (!usedForStats) ? m_currentAnalyte->concentrations : ( (m_statsForAnalyte == AnalyteId::ANALYTE_A) ? m_currentAnalyte->concentrations : m_secondAnalyte->concentrations );
  const Analyte::ConcentrationMap &cs2 = (!usedForStats) ? m_secondAnalyte->concentrations : ( (m_statsForAnalyte == AnalyteId::ANALYTE_A) ? m_secondAnalyte->concentrations : m_currentAnalyte->concentrations );

  mat_x = Vector<hyp2x_type> (cs1.size() + cs2.size());
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

  double u0A =  m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_A);
  double uCSA = m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_CS_A);
  double KCSA = m_fitResultsValues.at(HyperbolaFitResults::Floating::K_CS_A);
  double u0B =  m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_B);
  double uCSB;
  double KCSB;

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::u0, u0A);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::u0);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::uS, uCSA);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::uS);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_K_CS_A))
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::KS, KCSA);
  else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::KS);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_B)) {
    u0B = u0A;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::du0, u0B - u0A);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::du0);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_B)) {
    uCSB = uCSA;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::duS, uCSB - uCSA);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::duS);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_K_CS_B)) {
    KCSB = KCSA;
    dfrRef.FixParameter(echmet::regressCore::RectangularHyperbola2Params::dKS, KCSB - KCSA);
  } else
    dfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbola2Params::dKS);


  bool ret = dfrRef.Initialize(mat_x, mat_y,
                               m_fitFloatValues.at(HyperbolaFitParameters::Floating::EPSILON),
                               m_fitIntValues.at(HyperbolaFitParameters::Int::MAX_ITERATIONS),
                               true, u0A, u0B,
                               m_fitFloatValues.at(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE));

  if (!ret) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Double estimation failed."));
    return DoubleHypResults();
  }

  u0A  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::u0);
  uCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::uS);
  KCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::KS);
  u0B  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::du0);
  uCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::duS);
  KCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::dKS);
  double sigma = dfrRef.GetS();

  return DoubleHypResults(u0A, uCSA, KCSA, maxX, sigma, 0, u0A + u0B, uCSA + uCSB, KCSA + KCSB);
}

HyperbolaFittingEngine::DoubleHypResults HyperbolaFittingEngine::doDoubleFit(const DoubleHypResults &dr)
{
  echmet::regressCore::RectangularHyperbola2<double, double> &dfrRef = *m_doubleFitRegressor;
  if (!dfrRef.Regress())
    return DoubleHypResults();

  double u0A = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::u0);
  double uCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::uS);
  double KCSA = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::KS);
  double u0B  = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::du0);
  double uCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::duS);
  double KCSB = dfrRef.GetParameter(echmet::regressCore::RectangularHyperbola2Params::dKS);
  int iters = dfrRef.GetIterationCounter();
  double sigma = dfrRef.GetS();

  return DoubleHypResults(u0A, uCSA, KCSA, dr.maxX_A, sigma, iters, u0A + u0B, uCSA + uCSB, KCSA + KCSB);
}

void HyperbolaFittingEngine::doSerialize(const QString &path)
{
  SerializableDataTable table;
  table.viscositySlope = m_fitFloatValues.at(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE);

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

HyperbolaFittingEngine::HypResults HyperbolaFittingEngine::doSingleEstimate()
{
  double maxX = std::numeric_limits<double>::min();
  /* Prevent any ptr vs. var mishaps */
  echmet::regressCore::RectangularHyperbola<double, double> &sfrRef = *m_singleFitRegressor;

  typedef Eigen::MatrixXd MatrixD;

  Vector<double> mat_x;
  MatrixD        mat_y;

  if (m_currentAnalyte == nullptr)
    return HypResults();

  if (m_currentAnalyte->concentrations.size() < 1)
    return HypResults();

  if (!m_currentAnalyte->concentrations.contains(0.0)) {
    double u0 = m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_A);
    int ret = QMessageBox::question(nullptr, tr("No zero concentration"), QString(tr("List of entered concentrations does not contain any data for zero concentration of the selector.\n"
                                                                                     "Do you want to use the current value of \xCE\xBC = %1 as the estimate?")).arg(u0),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes)
      return HypResults();
  }

  const Analyte::ConcentrationMap &cs = m_currentAnalyte->concentrations;

  mat_x = Vector<double>(cs.size());
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

  double u0 = m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_A);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperbolaParams::u0,
        m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbolaParams::u0);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperbolaParams::uS,
        m_fitResultsValues.at(HyperbolaFitResults::Floating::MOBILITY_CS_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbolaParams::uS);

  if (m_fitFixedValues.at(HyperbolaFitParameters::Boolean::FIXED_K_CS_A))
    sfrRef.FixParameter(echmet::regressCore::RectangularHyperbolaParams::KS,
        m_fitResultsValues.at(HyperbolaFitResults::Floating::K_CS_A)
    );
  else
    sfrRef.ReleaseParameter(echmet::regressCore::RectangularHyperbolaParams::KS);

  bool ret = sfrRef.Initialize(mat_x, mat_y,
                               m_fitFloatValues.at(HyperbolaFitParameters::Floating::EPSILON),
                               m_fitIntValues.at(HyperbolaFitParameters::Int::MAX_ITERATIONS),
                               true, u0,
                               m_fitFloatValues.at(HyperbolaFitParameters::Floating::VISCOSITY_SLOPE));

  if (!ret) {
    QMessageBox::warning(nullptr, tr("Runtime error"), tr("Single estimation failed."));
    return HypResults();
  }

  u0 = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::u0);
  double uCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::uS);
  double KCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::KS);
  double sigma = sfrRef.GetS();

  return HypResults(u0, uCS, KCS, maxX, sigma, 0);
}

HyperbolaFittingEngine::HypResults HyperbolaFittingEngine::doSingleFit(const HypResults &r)
{
  echmet::regressCore::RectangularHyperbola<double, double> &sfrRef = *m_singleFitRegressor;
  if (!sfrRef.Regress())
    return HypResults();

  double u0 = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::u0);
  double uCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::uS);
  double KCS = sfrRef.GetParameter(echmet::regressCore::RectangularHyperbolaParams::KS);
  double sigma = sfrRef.GetS();

  return HypResults(u0, uCS, KCS, r.maxX_A, sigma, sfrRef.GetIterationCounter());
}

void HyperbolaFittingEngine::exportToCsv()
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

bool HyperbolaFittingEngine::exportToCsvSingleFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const int precision)
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

  stream.setEncoding(QStringConverter::Utf8);
  stream.setGenerateByteOrderMark(false);

  for (const std::shared_ptr<Analyte> &analyte : m_analytes) {
    stream << analyte->name << delimiter << "\n";

    stream << QObject::tr("Concentration") << delimiter << QObject::tr("Mobilities") << "\n";
    for (const std::shared_ptr<Concentration> &concentration : analyte->concentrations) {
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

AbstractMapperModel<bool, HyperbolaFitParameters::Boolean> *HyperbolaFittingEngine::fitFixedModel()
{
  return &m_fitFixedModel;
}

AbstractMapperModel<double, HyperbolaFitParameters::Floating> *HyperbolaFittingEngine::fitFloatModel()
{
  return &m_fitFloatModel;
}

AbstractMapperModel<int, HyperbolaFitParameters::Int> *HyperbolaFittingEngine::fitIntModel()
{
  return &m_fitIntModel;
}

AbstractMapperModel<double, HyperbolaFitResults::Floating> *HyperbolaFittingEngine::fitResultsModel()
{
  return &m_fitResultsModel;
}

QAbstractItemModel *HyperbolaFittingEngine::fitModeModel()
{
  return &m_fitModeModel;
}

void HyperbolaFittingEngine::hideDataSeries()
{
  m_plotCtx->hideSerie(seriesIndex(Series::FIT_A_CURVE));
  m_plotCtx->hideSerie(seriesIndex(Series::FIT_B_CURVE));
  m_plotCtx->hideSerie(seriesIndex(Series::POINTS_A));
  m_plotCtx->hideSerie(seriesIndex(Series::POINTS_B));
  m_plotCtx->hideSerie(seriesIndex(Series::POINTS_A_AVG));
  m_plotCtx->hideSerie(seriesIndex(Series::POINTS_B_AVG));
}

void HyperbolaFittingEngine::initFitModeModel()
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

void HyperbolaFittingEngine::initStatUnitsModel()
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

void HyperbolaFittingEngine::invalidateAll()
{
  invalidateCurrentConcentration();
  m_currentAnalyte = nullptr;
}

double HyperbolaFittingEngine::interpolateVerticalMarkerPosition(const QPointF &a, const QPointF &b, const double y) const
{
  const double k = (b.y() - a.y()) / (b.x() - a.x());

  return ((y - a.y()) / k) + a.x();
}

void HyperbolaFittingEngine::invalidateAnalyteA()
{
  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_A] = 0.0;
  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_A] = 0.0;
  m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_A] = 0.0;
  m_fitResultsModel.notifyDataChanged(HyperbolaFitResults::Floating::MOBILITY_A, HyperbolaFitResults::Floating::K_CS_A, { Qt::EditRole });

}

void HyperbolaFittingEngine::invalidateAnalyteB()
{
  m_secondAnalyte = nullptr;

  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_B] = "";
  m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_B, HyperbolaFitParameters::String::ANALYTE_B);

  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_B] = 0.0;
  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_B] = 0.0;
  m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_B] = 0.0;
  m_fitResultsModel.notifyDataChanged(HyperbolaFitResults::Floating::MOBILITY_B, HyperbolaFitResults::Floating::K_CS_B, { Qt::EditRole });
}

void HyperbolaFittingEngine::invalidateCurrentConcentration()
{
  m_currentConcentration = nullptr;
  m_currentConcentrationKey = INVAL_CONC_KEY;
}

bool HyperbolaFittingEngine::isEditable() const
{
  if (m_currentFitMode == FitMode::SINGLE)
    return true;

  QMessageBox::warning(nullptr, tr("Input error"), tr("Data tables cannot be edited in double fit mode"));
  return false;
}

void HyperbolaFittingEngine::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  StandardPlotContextSettingsHandler::loadUserSettings(settings, *m_plotCtx.get());

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

QList<QStandardItem *> HyperbolaFittingEngine::makeConcentrationsList(const Analyte::ConcentrationMap &data)
{
  QList<QStandardItem *> list;

  for (std::shared_ptr<Concentration> c : data) {
    QStandardItem *item = new QStandardItem();
    item->setData(c->concentration, Qt::DisplayRole);
    item->setData(c->concentration, Qt::UserRole + 1);

    list.push_back(item);
  }

  return list;
}

QList<QStandardItem *> HyperbolaFittingEngine::makeMobilitiesList(const Concentration::Mobilities &data)
{
  QList<QStandardItem *> list;

  for (const double d : data) {
    QStandardItem *item = new QStandardItem();
    item->setData(d, Qt::DisplayRole);
    item->setData(d, Qt::UserRole + 1);

    list.push_back(item);
  }

  return list;
}

QAbstractItemModel *HyperbolaFittingEngine::mobilitiesModel()
{
  return &m_mobilitiesModel;
}

void HyperbolaFittingEngine::onAddAnalyte(const QString &name, QModelIndex &idx)
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

  idx = m_analytesModel.index(m_analytesModel.rowCount() - 1, 0);

  m_currentAnalyte = analyte;
  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_A] = name;
  m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_A, HyperbolaFitParameters::String::ANALYTE_A);

  clearAnalyteASeries();

  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperbolaFittingEngine::onAddConcentration(const double num, QModelIndex &idx)
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
  idx = m_concentrationsModel.index(m_concentrationsModel.rowCount() - 1, 0);
}

void HyperbolaFittingEngine::onAddMobility(const double u, QModelIndex &idx)
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
  idx = m_mobilitiesModel.index(m_mobilitiesModel.rowCount() - 1, 0);

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperbolaFittingEngine::onAnalyteSwitched(const QModelIndexList &inList)
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
        m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_B] = name2;
        plotPoints(Series::POINTS_B, m_secondAnalyte);

        m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_B, HyperbolaFitParameters::String::ANALYTE_B);
      }
    } else if (list.size() > 2) {
        QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No more that two analytes can be selected at once.")).arg(name));
        return;
    }
  }
  invalidateAnalyteA();
  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_A] = name;
  m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_A, HyperbolaFitParameters::String::ANALYTE_A);

  clearAnalyteASeries();


  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);

  clearAllModels();
  invalidateCurrentConcentration();

  setConcentrationsList(makeConcentrationsList(m_currentAnalyte->concentrations));

  emit sortLists();
}

void HyperbolaFittingEngine::onChartHorizontalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker)
{
  auto intersection = [](const QPointF &a, const QPointF &b, const double x) {
    const double k = (b.y() - a.y()) / (b.x() - a.x());

    return k * (x - a.x()) + a.y();
  };

  switch (marker) {
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    for (int idx = 1; idx < m_statData.size(); idx++) {
      if (m_statData.at(idx).x() >= m_verticalAMarkerPosition) {
        m_horizontalMarkerPosition = intersection(m_statData.at(idx - 1), m_statData.at(idx), m_verticalAMarkerPosition);
        break;
      }
    }
    break;
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
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

  setMarkerPosition(HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER);
  emit chartHorizontalMarkerIntersectionSet(m_horizontalMarkerPosition);
}

void HyperbolaFittingEngine::onChartVerticalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker)
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
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    for (int idx = 1; idx < m_statData.size(); idx++) {
      if (comparator(m_statData.at(idx).y(), m_horizontalMarkerPosition)) {
        m_verticalAMarkerPosition = interpolateVerticalMarkerPosition(m_statData.at(idx - 1), m_statData.at(idx), m_horizontalMarkerPosition);

        setMarkerPosition(marker);
        emit chartVerticalMarkerIntersectionSet(marker, m_verticalAMarkerPosition);
        return;
      }
    }
    break;
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
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

void HyperbolaFittingEngine::onChartMarkerValueChanged(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d)
{
  switch (marker) {
    case HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
      m_horizontalMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showHorizontalMarker) {
        setMarkerPosition(marker);
        m_plotCtx->replot();
      }
      break;
    case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
      m_verticalAMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showVerticalAMarker) {
        setMarkerPosition(marker);
        m_plotCtx->replot();
      }
      break;
    case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
      m_verticalBMarkerPosition = d;
      if ((m_viewMode == ViewMode::STATS) && m_showVerticalBMarker) {
        setMarkerPosition(marker);
        m_plotCtx->replot();
      }
      break;
    default:
      break;
  }
}

void HyperbolaFittingEngine::onClearHyperbola()
{
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FIT_A_CURVE));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::FIT_B_CURVE));
  m_plotCtx->replot();
}

void HyperbolaFittingEngine::onConcentrationSwitched(const QModelIndex &idx)
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

void HyperbolaFittingEngine::onDeserialize()
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
  m_plotCtx->setPlotTitle(finfo.fileName());
}

void HyperbolaFittingEngine::onDoEstimate()
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

void HyperbolaFittingEngine::onDoFit()
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

void HyperbolaFittingEngine::onDoStats(const HyperbolaStats::Intervals intr)
{
  QVector<QPointF> data;
  std::vector<std::pair<double, double> > out;
  double tau;
  double twoSided = false;

  switch (intr) {
  case HyperbolaStats::Intervals::LEFT:
    tau = -TAU_MAX;
    break;
  case HyperbolaStats::Intervals::BOTH:
    twoSided = true;
    /* FALLTHRU */
  case HyperbolaStats::Intervals::RIGHT:
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
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbolaParams::u0);
      break;
    case StatMode::MOBILITY_CS_A:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbolaParams::uS);
      break;
    case StatMode::K_CS_A:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbolaParams::KS);
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
    echmet::regressCore::Profiler<echmet::regressCore::RectangularHyperbola2XType<double>, double> MProfiler;

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
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::u0);
      break;
    case StatMode::MOBILITY_CS_B:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::uS);
      break;
    case StatMode::K_CS_B:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::KS);
      break;
    case StatMode::D_MOBILITY:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::du0);
      break;
    case StatMode::D_MOBILITY_CS:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::duS);
      break;
    case StatMode::D_K_CS:
      paramId = static_cast<msize_t>(echmet::regressCore::RectangularHyperbola2Params::dKS);
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
  m_plotCtx->setSerieSamples(seriesIndex(Series::STATS), data);
  m_plotCtx->clearSerieSamples(seriesIndex(Series::HORIZONTAL_MARKER));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::VERTICAL_A_MARKER));
  m_plotCtx->clearSerieSamples(seriesIndex(Series::VERTICAL_B_MARKER));

  m_statData = data;
  if (m_statData.size() > 0) {
    const int halfSize = m_statData.size() / 2;

    if (m_showHorizontalMarker) {
      m_horizontalMarkerPosition = m_statData.at(halfSize).y();
      setMarkerPosition(HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER);
      emit chartHorizontalMarkerIntersectionSet(m_horizontalMarkerPosition);
    }
    if (m_showVerticalAMarker) {
      m_verticalAMarkerPosition = m_statData.at(halfSize).x();
      setMarkerPosition(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER);
      emit chartVerticalMarkerIntersectionSet(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER, m_verticalAMarkerPosition);
    }
    if (m_showVerticalBMarker) {
      m_verticalBMarkerPosition = m_statData.at(halfSize).x();
      setMarkerPosition(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER);
      emit chartVerticalMarkerIntersectionSet(HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER, m_verticalBMarkerPosition);
    }
  }

  m_plotCtx->scaleToFit();
}

void HyperbolaFittingEngine::onEditConcentration(const double num, const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  if (!isEditable())
    return;

  if (m_currentAnalyte == nullptr)
    return;

  if (num < 0.0) {
    QMessageBox::warning(nullptr, tr("Invalid input"), tr("Entered concentration value is nonsensical."));
    return;
  }

  bool ok;
  const QVariant ocVar = m_concentrationsModel.data(idx, Qt::UserRole + 1);
  const double oldConcentration = ocVar.toDouble(&ok);

  if (!ok) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("Invalid concentration index value %1. Please report this as a bugto CEval developers.")).arg(ocVar.toString()));
    return;
  }

  if (!m_currentAnalyte->concentrations.contains(oldConcentration)) {
    QMessageBox::warning(nullptr, tr("Runtime error"), QString(tr("No concentration with old value of %1. Please report this as a bug to CEval developers")).arg(oldConcentration));
    return;
  }

  std::shared_ptr<Concentration> c = nullptr;
  try {
    const std::shared_ptr<Concentration> oldConcPtr = m_currentAnalyte->concentrations[oldConcentration];
    c = std::make_shared<Concentration>(num, *oldConcPtr.get());

    m_currentAnalyte->concentrations.remove(oldConcentration);
    m_currentAnalyte->concentrations.insert(num, c);
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Insufficient memory"), tr("Unable to edit concentration, current concentration list may be inconsistent!"));
    return;
  }


  m_concentrationsModel.setData(idx, DoubleToStringConvertor::convert(num));
  m_concentrationsModel.setData(idx, num, Qt::UserRole + 1);
  m_currentConcentration = c;
  m_currentConcentrationKey = num;

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperbolaFittingEngine::onEditMobility(const double u, const QModelIndex &idx)
{
  if (!idx.isValid())
    return;

  if (!isEditable())
    return;

  if (m_currentConcentration == nullptr) {
    QMessageBox::information(nullptr, tr("No concentration"), tr("No concentration has been selected."));
    return;
  }

  if (!m_currentConcentration->updateMobility(u, idx.row()))
    return;

  m_mobilitiesModel.setData(idx, DoubleToStringConvertor::convert(u));
  m_mobilitiesModel.setData(idx, u, Qt::UserRole + 1);

  showDataSeries();
  plotPoints(Series::POINTS_A, m_currentAnalyte);
}

void HyperbolaFittingEngine::onEmergencySave()
{
  QString path = QDir::currentPath();

  if (m_analytes.size() > 0)
    doSerialize(path + "/" + EMERG_SAVE_FILE);
}

void HyperbolaFittingEngine::onFitModeChanged(const QVariant &v)
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

void HyperbolaFittingEngine::onNumberFormatChanged(const QLocale *oldLocale)
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

void HyperbolaFittingEngine::onPlotPointSelected(const QPointF &point, const QPoint &cursor)
{
  QAction *trig = m_plotCtxMenu->exec(cursor);

  if (trig != nullptr)
    plotCtxMenuTriggered(trig->data().value<PlotCtxMenuActions>(), point);
}

void HyperbolaFittingEngine::onRedrawDataSeries()
{
  m_plotCtx->hideSerie(seriesIndex(Series::STATS));
  showDataSeries();
  m_plotCtx->scaleToFit();
}

void HyperbolaFittingEngine::onRegisterMobility(const QString &name, const double selConcentration, const double mobility)
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

void HyperbolaFittingEngine::onRemoveAnalyte(const QModelIndex &idx)
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

void HyperbolaFittingEngine::onRemoveConcentration(const QModelIndex &idx)
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

void HyperbolaFittingEngine::onRemoveMobility(const QModelIndex &idx)
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

void HyperbolaFittingEngine::onRenameAnalyte(const QVariant &internalId, const QString &newName, const int idx)
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
  item->setData(newName, Qt::UserRole + 1);

  m_analytesModel.insertRow(idx, item);

  m_currentAnalyte = newA;
  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_A] = newName;
  m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_A, HyperbolaFitParameters::String::ANALYTE_A);
}

void HyperbolaFittingEngine::onSecondAnalyteSameChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
  if (!roles.contains(Qt::EditRole))
    return;

  for (int idx = topLeft.column(); idx <= bottomRight.column(); idx++) {
    HyperbolaFitParameters::Boolean i = m_fitFixedModel.itemFromIndex(idx);
    HyperbolaFitResults::Floating fi = HyperbolaFitResults::Floating::LAST_INDEX;

    bool same = m_fitFixedValues.at(i);

    if (same) {
      switch (i) {
      case HyperbolaFitParameters::Boolean::FIXED_K_CS_B:
        m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_B] = m_fitResultsValues[HyperbolaFitResults::Floating::K_CS_A];
        fi = HyperbolaFitResults::Floating::K_CS_B;
        break;
      case HyperbolaFitParameters::Boolean::FIXED_MOBILITY_B:
        m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_B] =  m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_A];
        fi = HyperbolaFitResults::Floating::MOBILITY_B;
        break;
      case HyperbolaFitParameters::Boolean::FIXED_MOBILITY_CS_B:
        m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_B] = m_fitResultsValues[HyperbolaFitResults::Floating::MOBILITY_CS_A];
        fi = HyperbolaFitResults::Floating::MOBILITY_CS_B;
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

void HyperbolaFittingEngine::onSerialize()
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

void HyperbolaFittingEngine::onShowChartMarker(const HyperbolaFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value)
{
  bool ok;

  value.toDouble(&ok);
  if (ok) {
    const double d = value.toDouble();

    switch (marker) {
    case HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
      m_horizontalMarkerPosition = d;
      break;
    case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
      m_verticalAMarkerPosition = d;
      break;
    case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
      m_verticalBMarkerPosition = d;
      break;
    default:
      break;
    }
  }

  switch (marker) {
  case HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
    m_showHorizontalMarker = visible;
    if (m_showHorizontalMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_plotCtx->showSerie(seriesIndex(Series::HORIZONTAL_MARKER));
        m_plotCtx->replot();
      }
    } else {
      m_plotCtx->hideSerie(seriesIndex(Series::HORIZONTAL_MARKER));
      m_plotCtx->replot();
    }
    break;
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
    m_showVerticalAMarker = visible;
    if (m_showVerticalAMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_plotCtx->showSerie(seriesIndex(Series::VERTICAL_A_MARKER));
        m_plotCtx->replot();
      }
    } else {
      m_plotCtx->hideSerie(seriesIndex(Series::VERTICAL_A_MARKER));
      m_plotCtx->replot();
    }
    break;
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
    m_showVerticalBMarker = visible;
    if (m_showVerticalBMarker) {
      setMarkerPosition(marker);

      if (m_viewMode == ViewMode::STATS) {
        m_plotCtx->showSerie(seriesIndex(Series::VERTICAL_B_MARKER));
        m_plotCtx->replot();
      }
    } else {
      m_plotCtx->hideSerie(seriesIndex(Series::VERTICAL_B_MARKER));
      m_plotCtx->replot();
    }
    break;
  default:
    break;
  }
}

void HyperbolaFittingEngine::onStatModeChanged(const QVariant &v)
{
  if (!v.canConvert<StatMode>())
    return;

  m_currentStatMode = v.value<StatMode>();
}

void HyperbolaFittingEngine::onStatsForAnalyteChanged(const HyperbolaFittingEngineMsgs::AnalyteId aId)
{
  if (m_currentFitMode != FitMode::DOUBLE)
    return;

  switch (aId) {
  case HyperbolaFittingEngineMsgs::AnalyteId::ANALYTE_A:
    m_statsForAnalyte = AnalyteId::ANALYTE_A;
    break;
  case HyperbolaFittingEngineMsgs::AnalyteId::ANALYTE_B:
    m_statsForAnalyte = AnalyteId::ANALYTE_B;
    break;
  default:
    break;
  }
}

void HyperbolaFittingEngine::onStatUnitsChanged(const QVariant &v)
{
  if (!v.canConvert<StatUnits>())
    return;

  m_currentStatUnits = v.value<StatUnits>();
}

void HyperbolaFittingEngine::plotCurve(const Series s, const QVector<QPointF> &data)
{
  m_plotCtx->setSerieSamples(seriesIndex(s), data);

  m_plotCtx->replot();
}

void HyperbolaFittingEngine::plotCtxMenuTriggered(const PlotCtxMenuActions ma, const QPointF &point)
{
  Q_UNUSED(point)

  switch (ma) {
  case PlotCtxMenuActions::SCALE_PLOT_TO_FIT:
    m_plotCtx->scaleToFit();
    break;
  }
}

void HyperbolaFittingEngine::plotDoubleCurve(const DoubleHypResults &dr)
{
    QVector<QPointF> curveA;
    for (double x = 0.0; x <= dr.maxX_A; x += (dr.maxX_A - 0.0) / 100.0){
        echmet::regressCore::RectangularHyperbola2XType<double> mx(0, x);
        curveA.push_back(QPointF(x, (*m_doubleFitRegressor)(mx)));
    }

    QVector<QPointF> curveB;
    for (double x = 0.0; x <= dr.maxX_A; x += (dr.maxX_A - 0.0) / 100.0){
        echmet::regressCore::RectangularHyperbola2XType<double> mx(1, x);
        curveB.push_back(QPointF(x, (*m_doubleFitRegressor)(mx)));
    }

    m_plotCtx->setSerieSamples(seriesIndex(Series::FIT_A_CURVE), curveA);
    m_plotCtx->setSerieSamples(seriesIndex(Series::FIT_B_CURVE), curveB);

    m_plotCtx->replot();
}

void HyperbolaFittingEngine::plotPoints(const Series s, std::shared_ptr<const Analyte> a)
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

  m_plotCtx->setSerieSamples(seriesIndex(s), points);
  m_plotCtx->setSerieSamples(seriesIndex(s) + 1, average);
  m_plotCtx->scaleToFit();
}

void HyperbolaFittingEngine::plotSingleCurve(const HypResults &r)
{
  QVector<QPointF> curve;
  for (double x = 0.0; x <= r.maxX_A; x += (r.maxX_A - 0.0) / 100.0)
    curve.push_back(QPointF(x, (*m_singleFitRegressor)(x)));

  m_plotCtx->setSerieSamples(seriesIndex(Series::FIT_A_CURVE), curve);
  m_plotCtx->replot();
}

void HyperbolaFittingEngine::refreshModels()
{
  m_analytesModel.clear();

  for (auto &a : m_analytes) {
    QStandardItem *item = new QStandardItem(a->name);
    item->setData(a->name);
    m_analytesModel.appendRow(item);
  }

  m_concentrationsModel.clear();
  m_mobilitiesModel.clear();
  m_plotCtx->clearAllSerieSamples();
  m_plotCtx->scaleToFit();

  m_currentAnalyte = nullptr;
  m_secondAnalyte = nullptr;

  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_A] = "";
  m_analyteNamesValues[HyperbolaFitParameters::String::ANALYTE_B] = "";

  m_analyteNamesModel.notifyDataChanged(HyperbolaFitParameters::String::ANALYTE_A, HyperbolaFitParameters::String::ANALYTE_B);
}

QVariant HyperbolaFittingEngine::saveUserSettings() const
{
  EMT::StringVariantMap map = StandardPlotContextSettingsHandler::saveUserSettings(*m_plotCtx.get(), seriesIndex(Series::LAST_INDEX));

  map[LAST_LOADSAVE_PATH_SETTINGS_TAG] = m_lastDataTablePath;
  map[LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG] = m_lastExportToCsvPath;

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

int constexpr HyperbolaFittingEngine::seriesIndex(const Series s)
{
  return static_cast<int>(s);
}

void HyperbolaFittingEngine::setConcentrationsList(const QList<QStandardItem *> &list)
{
  for (QStandardItem *const item : list)
    m_concentrationsModel.appendRow(item);
}

void HyperbolaFittingEngine::setDoubleFitStats()
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

void HyperbolaFittingEngine::setMarkerPosition(const HyperbolaFittingEngineMsgs::MarkerType marker)
{
  const QRectF r = m_plotCtx->range();

  switch (marker) {
  case HyperbolaFittingEngineMsgs::MarkerType::HORIZONTAL_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(r.topLeft().x(), m_horizontalMarkerPosition));
    points.push_back(QPointF(r.topRight().x(), m_horizontalMarkerPosition));

    m_plotCtx->setSerieSamples(seriesIndex(Series::HORIZONTAL_MARKER), points);
    break;
  }
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_A_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(m_verticalAMarkerPosition, r.topLeft().y()));
    points.push_back(QPointF(m_verticalAMarkerPosition, r.bottomLeft().y()));

    m_plotCtx->setSerieSamples(seriesIndex(Series::VERTICAL_A_MARKER), points);
    break;
  }
  case HyperbolaFittingEngineMsgs::MarkerType::VERTICAL_B_MARKER:
  {
    QVector<QPointF> points;
    points.push_back(QPointF(m_verticalBMarkerPosition, r.topLeft().y()));
    points.push_back(QPointF(m_verticalBMarkerPosition, r.bottomLeft().y()));

    m_plotCtx->setSerieSamples(seriesIndex(Series::VERTICAL_B_MARKER), points);
    break;
  }
  default:
    return;
  }
}

void HyperbolaFittingEngine::setMobilitiesList(const QList<QStandardItem *> &list)
{
  for (QStandardItem *const item : list)
    m_mobilitiesModel.appendRow(item);
}

void HyperbolaFittingEngine::setSingleFitStats()
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

void HyperbolaFittingEngine::showDataSeries()
{
  m_plotCtx->hideSerie(seriesIndex(Series::STATS));
  m_plotCtx->hideSerie(seriesIndex(Series::HORIZONTAL_MARKER));
  m_plotCtx->hideSerie(seriesIndex(Series::VERTICAL_A_MARKER));
  m_plotCtx->hideSerie(seriesIndex(Series::VERTICAL_B_MARKER));

  m_plotCtx->showSerie(seriesIndex(Series::FIT_A_CURVE));
  m_plotCtx->showSerie(seriesIndex(Series::FIT_B_CURVE));
  m_plotCtx->showSerie(seriesIndex(Series::POINTS_A));
  m_plotCtx->showSerie(seriesIndex(Series::POINTS_B));
  m_plotCtx->showSerie(seriesIndex(Series::POINTS_A_AVG));
  m_plotCtx->showSerie(seriesIndex(Series::POINTS_B_AVG));

  m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, tr("Mobility"));
  m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, tr("Selector concentration"));

  m_viewMode = ViewMode::DATA;
}

void HyperbolaFittingEngine::showStatsSeries(const StatUnits units, const StatMode mode)
{
  switch (units) {
  case StatUnits::P_VALUE:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_pValueCaption);
    break;
  case StatUnits::CONFIDENCE:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_confidenceCaption);
    break;
  case StatUnits::TAU:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, s_tauCaption);
    break;
  default:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::Y_LEFT, tr("---"));
    break;
  }

  switch (mode) {
  case StatMode::MOBILITY_A:
  case StatMode::MOBILITY_B:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uACaption);
    break;
  case StatMode::D_MOBILITY:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uACaption + tr(", difference"));
    break;
  case StatMode::MOBILITY_CS_A:
  case StatMode::MOBILITY_CS_B:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uCSCaption);
    break;
  case StatMode::D_MOBILITY_CS:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_uCSCaption + tr(", difference"));
    break;
  case StatMode::K_CS_A:
  case StatMode::K_CS_B:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_KCSCaption);
    break;
  case StatMode::D_K_CS:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, s_KCSCaption + tr(", difference"));
    break;
  default:
    m_plotCtx->setAxisTitle(SerieProperties::Axis::X_BOTTOM, "---");
    break;
  }

  m_plotCtx->showSerie(seriesIndex(Series::STATS));

  if (m_showHorizontalMarker)
    m_plotCtx->showSerie(seriesIndex(Series::HORIZONTAL_MARKER));
  if (m_showVerticalAMarker)
    m_plotCtx->showSerie(seriesIndex(Series::VERTICAL_A_MARKER));
  if (m_showVerticalBMarker)
    m_plotCtx->showSerie(seriesIndex(Series::VERTICAL_B_MARKER));

  m_viewMode = ViewMode::STATS;
}

QAbstractItemModel *HyperbolaFittingEngine::statModeModel()
{
  return &m_statModeModel;
}

QAbstractItemModel *HyperbolaFittingEngine::statUnitsModel()
{
  return &m_statUnitsModel;
}
