#ifndef HYPERBOLAFITTINGENGINE_H
#define HYPERBOLAFITTINGENGINE_H

#include <QObject>
#include <QMap>
#include <QStandardItemModel>
#include <memory>
#include "booleanmappermodel.h"
#include "evalserializable.h"
#include "floatingmappermodel.h"
#include "hyperbolafititems.h"
#include "hyperbolafittingenginemsgs.h"
#include "integermappermodel.h"
#include "inumberformatchangeable.h"
#include "mappedvectorwrapper.h"
#include "plotcontextlimited.h"
#include "stringmappermodel.h"

class QMenu;
class ExportDatatableToCsvDialog;
namespace echmet {
  namespace regressCore {
    template <typename XT, typename YT> class RectangularHyperbola;
    template <typename XT, typename YT> class RectangularHyperbola2;
  }
}

class HyperbolaFittingEngine : public QObject, public INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(INumberFormatChangeable)
public:
  enum class AnalyteId {
    ANALYTE_A,
    ANALYTE_B
  };
  Q_ENUM(AnalyteId)

  enum class FitMode : int {
    SINGLE,
    DOUBLE
  };
  Q_ENUM(FitMode)

  enum class PlotCtxMenuActions {
    SCALE_PLOT_TO_FIT
  };
  Q_ENUM(PlotCtxMenuActions)

  enum class StatMode : int {
    MOBILITY_A,
    MOBILITY_CS_A,
    K_CS_A,
    MOBILITY_B,
    MOBILITY_CS_B,
    K_CS_B,
    D_MOBILITY,
    D_MOBILITY_CS,
    D_K_CS
  };
  Q_ENUM(StatMode)

  enum class StatUnits : int {
    TAU,
    CONFIDENCE,
    P_VALUE
  };
  Q_ENUM(StatUnits)

  class regressor_initialization_error : public std::exception {
  public:
    regressor_initialization_error(const char *what) :
      std::exception(),
      m_what(nullptr)
    {
      m_what = static_cast<char *>(malloc(strlen(what) + 1));
      if (m_what != nullptr)
        strcpy(m_what, what);
    }
    ~regressor_initialization_error() noexcept
    {
      free(m_what);
    }

    virtual const char *what() const noexcept;

  private:
    char *m_what;
  };

  explicit HyperbolaFittingEngine(QObject *parent = nullptr);
  ~HyperbolaFittingEngine();

  AbstractMapperModel<QString, HyperbolaFitParameters::String> *analyteNamesModel();
  QAbstractItemModel *analytesModel();
  void assignContext(std::shared_ptr<PlotContextLimited> ctx);
  void checkForCrashRecovery();
  QAbstractItemModel *concentrationsModel();
  void exportToCsv();
  bool exportToCsvSingleFile(const QString &path, const QChar &delimiter, const QChar &decimalSeparator, const int precision);
  AbstractMapperModel<bool, HyperbolaFitParameters::Boolean> *fitFixedModel();
  AbstractMapperModel<double, HyperbolaFitResults::Floating> *fitResultsModel();
  AbstractMapperModel<double, HyperbolaFitParameters::Floating> *fitFloatModel();
  AbstractMapperModel<int, HyperbolaFitParameters::Int> *fitIntModel();
  QAbstractItemModel *fitModeModel();
  void loadUserSettings(const QVariant &settings);
  QAbstractItemModel *mobilitiesModel();
  void onNumberFormatChanged(const QLocale *oldLocale) override;
  void refreshModels();
  QVariant saveUserSettings() const;
  QAbstractItemModel *statModeModel();
  QAbstractItemModel *statUnitsModel();

  static const QString EMERG_SAVE_FILE;

private:
  enum class Series : int {
    POINTS_A,
    POINTS_A_AVG,
    POINTS_B,
    POINTS_B_AVG,
    /* ^^^ Do not reorder these! ^^^ */
    FIT_A_CURVE,
    FIT_B_CURVE,
    STATS,
    HORIZONTAL_MARKER,
    VERTICAL_A_MARKER,
    VERTICAL_B_MARKER,
    LAST_INDEX
  };

  enum class ViewMode {
    DATA,
    STATS
  };

  class Concentration {
  public:
    typedef QVector<double> Mobilities;

    Concentration(const double concentration);
    Concentration(const double concentration, const Concentration &other);
    void addMobility(const double mobility);
    double avgMobility() const;
    void removeMobility(const int idx);
    bool updateMobility(const double u, const int idx);
    const Mobilities &mobilities() const;

    const double concentration;

  private:
    void recalculateAverage();

    double m_avgMobility;
    Mobilities m_mobilities;
  };

  class Analyte {
  public:
    typedef QMap<double, std::shared_ptr<Concentration>> ConcentrationMap;

    Analyte(const QString &name);
    Analyte(const QString &name, std::shared_ptr<const Analyte> other);

    ConcentrationMap concentrations;
    const QString name;
  };

  class SerializableConcentration : public EvalSerializable {
  public:
    virtual EvalSerializable::RetCode Operate(DataManipulator &Manipulator) override;
    virtual std::string TypeName() const override;

    double c;
    std::vector<double> mobilities;
  };

  class SerializableAnalyte : public EvalSerializable {
  public:
    virtual EvalSerializable::RetCode Operate(DataManipulator &Manipulator) override;
    virtual std::string TypeName() const override;

    std::string name;
    std::vector<SerializableConcentration> concentrations;
  };

  class SerializableDataTable : public EvalSerializable {
  public:
    virtual EvalSerializable::RetCode Operate(DataManipulator &Manipulator) override;
    virtual std::string TypeName() const override;

    double viscositySlope;
    std::vector<SerializableAnalyte> analytes;
  };

  class HypResults {
  public:
    HypResults(const double u0, const double uCS, const double KCS, const double maxX,
               const double sigma, const int iterations);
    explicit HypResults();
    HypResults(const HypResults &other);
    bool isValid() const;

    const double u0_A;
    const double uCS_A;
    const double KCS_A;
    const double maxX_A;
    const double sigma;
    const int iterations;

    HypResults &operator=(const HypResults &other);

  protected:
    const bool m_valid;
  };

  class DoubleHypResults : public HypResults {
  public:
    DoubleHypResults(const double u0_A, const double uCS_A, const double KCS_A, const double maxX_A,
                     const double rss, const int iterations,
                     const double u0_B, const double uCS_B, const double KCS_B);
    explicit DoubleHypResults();
    DoubleHypResults(const DoubleHypResults &other);

    const double u0_B;
    const double uCS_B;
    const double KCS_B;

    DoubleHypResults &operator=(const DoubleHypResults &other);
  };

  void clearAllModels();
  void clearAnalyteASeries();
  void clearAnalyteBSeries();
  void createContextMenu() noexcept(false);
  void displayHypResults(const HypResults *r);
  bool doDeserialize(const QString &path);
  DoubleHypResults doDoubleEstimate(const bool usedForStats);
  DoubleHypResults doDoubleFit(const DoubleHypResults &dr);
  void doSerialize(const QString &path);
  HypResults doSingleEstimate();
  HypResults doSingleFit(const HypResults &r);
  void hideDataSeries();
  void initFitModeModel();
  void initStatUnitsModel();
  double interpolateVerticalMarkerPosition(const QPointF &a, const QPointF &b, const double y) const;
  void invalidateAll();
  void invalidateAnalyteA();
  void invalidateAnalyteB();
  void invalidateCurrentConcentration();
  bool isEditable() const;
  QList<QStandardItem *> makeConcentrationsList(const Analyte::ConcentrationMap &data);
  QList<QStandardItem *> makeMobilitiesList(const Concentration::Mobilities &data);
  void plotCurve(const Series s, const QVector<QPointF> &data);
  void plotCtxMenuTriggered(const PlotCtxMenuActions ma, const QPointF &point);
  void plotDoubleCurve(const DoubleHypResults &dr);
  void plotPoints(const Series s, std::shared_ptr<const Analyte> a);
  void plotSingleCurve(const HypResults &r);
  void setConcentrationsList(const QList<QStandardItem *> &list);
  void setDoubleFitStats();
  void setMarkerPosition(const HyperbolaFittingEngineMsgs::MarkerType marker);
  void setMobilitiesList(const QList<QStandardItem *> &list);
  void setSingleFitStats();
  void showDataSeries();
  void showStatsSeries(const StatUnits units, const StatMode mode);

  std::shared_ptr<PlotContextLimited> m_plotCtx;
  ViewMode m_viewMode;
  QMenu *m_plotCtxMenu;

  QMap<QString, std::shared_ptr<Analyte>> m_analytes;

  std::shared_ptr<Analyte> m_currentAnalyte;
  std::shared_ptr<Analyte> m_secondAnalyte;
  int m_firstAnalyteIdx;

  std::shared_ptr<Concentration> m_currentConcentration;
  double m_currentConcentrationKey;
  FitMode m_currentFitMode;
  StatMode m_currentStatMode;
  StatUnits m_currentStatUnits;
  AnalyteId m_statsForAnalyte;
  bool m_showHorizontalMarker;
  bool m_showVerticalAMarker;
  bool m_showVerticalBMarker;
  double m_horizontalMarkerPosition;
  double m_verticalAMarkerPosition;
  double m_verticalBMarkerPosition;
  QVector<QPointF> m_statData;

  MappedVectorWrapper<bool, HyperbolaFitParameters::Boolean> m_fitFixedValues;
  MappedVectorWrapper<double, HyperbolaFitResults::Floating> m_fitResultsValues;
  MappedVectorWrapper<double, HyperbolaFitParameters::Floating> m_fitFloatValues;
  MappedVectorWrapper<int, HyperbolaFitParameters::Int> m_fitIntValues;
  MappedVectorWrapper<QString, HyperbolaFitParameters::String> m_analyteNamesValues;

  /* Data exposed to GUI */
  QStandardItemModel m_analytesModel;
  QStandardItemModel m_concentrationsModel;
  QStandardItemModel m_mobilitiesModel;
  QStandardItemModel m_fitModeModel;
  QStandardItemModel m_statModeModel;
  QStandardItemModel m_statUnitsModel;
  BooleanMapperModel<HyperbolaFitParameters::Boolean> m_fitFixedModel;
  FloatingMapperModel<HyperbolaFitResults::Floating> m_fitResultsModel;
  FloatingMapperModel<HyperbolaFitParameters::Floating> m_fitFloatModel;
  IntegerMapperModel<HyperbolaFitParameters::Int> m_fitIntModel;
  StringMapperModel<HyperbolaFitParameters::String> m_analyteNamesModel;

  echmet::regressCore::RectangularHyperbola<double, double> *m_singleFitRegressor;
  echmet::regressCore::RectangularHyperbola2<double, double> *m_doubleFitRegressor;

  QString m_lastDataTablePath;
  QString m_lastExportToCsvPath;
  const QStringList m_dataTablesNameFilter;
  const QStringList m_exportToCsvNameFilter;

  ExportDatatableToCsvDialog *m_exportDTToCsvDlg;


  static const QString s_dataPointsATitle;
  static const QString s_dataPointsBTitle;
  static const QString s_dataPointsAAvgTitle;
  static const QString s_dataPointsBAvgTitle;
  static const QString s_fitCurveATitle;
  static const QString s_fitCurveBTitle;
  static const QString s_fitCurveStatsTitle;
  static const QString s_horizontalMarkerTitle;
  static const QString s_verticalAMarkerTitle;
  static const QString s_verticalBMarkerTitle;

  static const QString s_uACaption;
  static const QString s_uCSCaption;
  static const QString s_KCSCaption;
  static const QString s_tauCaption;
  static const QString s_confidenceCaption;
  static const QString s_pValueCaption;

  static const double s_defaultViscositySlope;
  static const double s_defaultEpsilon;
  static const int s_defaultMaxIterations;

  static const QString CSV_FILE_SUFFIX;
  static const QString DATA_TABLE_FILE_SUFFIX;
  static const double INVAL_CONC_KEY;
  static const QString INVAL_ANALYTE_KEY;

  static const QString LAST_EXPORT_TO_CSV_PATH_SETTINGS_TAG;
  static const QString LAST_LOADSAVE_PATH_SETTINGS_TAG;

  static int constexpr seriesIndex(const Series p);

signals:
  void enableDoubleFit(const bool enable);
  void chartHorizontalMarkerIntersectionSet(const double d);
  void chartVerticalMarkerIntersectionSet(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d);
  void sortLists();

public slots:
  void onAddAnalyte(const QString &name, QModelIndex &idx);
  void onAddConcentration(const double num, QModelIndex &idx);
  void onAddMobility(const double u, QModelIndex &idx);
  void onAnalyteSwitched(const QModelIndexList &inList);
  void onChartHorizontalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker);
  void onChartVerticalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker);
  void onChartMarkerValueChanged(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d);
  void onClearHyperbola();
  void onConcentrationSwitched(const QModelIndex &idx);
  void onDeserialize();
  void onDoEstimate();
  void onDoFit();
  void onDoStats(const HyperbolaStats::Intervals intr);
  void onEditConcentration(const double num, const QModelIndex &idx);
  void onEditMobility(const double u, const QModelIndex &idx);
  void onEmergencySave();
  void onFitModeChanged(const QVariant &v);
  void onRedrawDataSeries();
  void onRegisterMobility(const QString &name, const double selConcentration, const double mobility);
  void onRemoveAnalyte(const QModelIndex &idx);
  void onRemoveConcentration(const QModelIndex &idx);
  void onRemoveMobility(const QModelIndex &idx);
  void onRenameAnalyte(const QVariant &internalId, const QString &newName, const int idx);
  void onSecondAnalyteSameChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
  void onSerialize();
  void onShowChartMarker(const HyperbolaFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value);
  void onStatModeChanged(const QVariant &v);
  void onStatsForAnalyteChanged(const HyperbolaFittingEngineMsgs::AnalyteId aId);
  void onStatUnitsChanged(const QVariant &v);

private slots:
  void onPlotPointSelected(const QPointF &point, const QPoint &cursor);

};

#endif // HYPERBOLAFITTINGENGINE_H
