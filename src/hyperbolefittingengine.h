#ifndef HYPERBOLEFITTINGENGINE_H
#define HYPERBOLEFITTINGENGINE_H

#include <QObject>
#include <QMap>
#include <QStandardItemModel>
#include <memory>
#include "booleanmappermodel.h"
#include "evalserializable.h"
#include "floatingmappermodel.h"
#include "hyperbolefititems.h"
#include "hyperbolefittingenginemsgs.h"
#include "integermappermodel.h"
#include "inumberformatchangeable.h"
#include "mappedvectorwrapper.h"
#include "modecontextlimited.h"
#include "stringmappermodel.h"

#include <vector>

#include "math/regressor/hyperbole.h"
#include "math/regressor/hyperbole2.h"

class ExportDatatableToCsvDialog;

class HyperboleFittingEngine : public QObject, public INumberFormatChangeable
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

    virtual const char *what() const noexcept
    {
      if (m_what != nullptr)
        return m_what;

      return "Unable to initialize regressor";
    }

  private:
    char *m_what;
  };

  explicit HyperboleFittingEngine(QObject *parent = nullptr);
  ~HyperboleFittingEngine();

  AbstractMapperModel<QString, HyperboleFitParameters::String> *analyteNamesModel();
  QAbstractItemModel *analytesModel();
  void assignContext(std::shared_ptr<ModeContextLimited> ctx);
  void checkForCrashRecovery();
  QAbstractItemModel *concentrationsModel();
  void exportToCsv();
  AbstractMapperModel<bool, HyperboleFitParameters::Boolean> *fitFixedModel();
  AbstractMapperModel<double, HyperboleFitResults::Floating> *fitResultsModel();
  AbstractMapperModel<double, HyperboleFitParameters::Floating> *fitFloatModel();
  AbstractMapperModel<int, HyperboleFitParameters::Int> *fitIntModel();
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
  typedef echmet::regressCore::RectangularHyperbole2<double, double>::x_type x_type;

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
    void addMobility(const double mobility);
    double avgMobility() const;
    void removeMobility(const int idx);
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

    const double u0_B;
    const double uCS_B;
    const double KCS_B;

    DoubleHypResults &operator=(const DoubleHypResults &other);
  };

  void clearAllModels();
  void clearAnalyteASeries();
  void clearAnalyteBSeries();
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
  void invalidateAnalyteB();
  void invalidateCurrentConcentration();
  bool isEditable() const;
  QList<QStandardItem *> makeConcentrationsList(const Analyte::ConcentrationMap &data);
  QList<QStandardItem *> makeMobilitiesList(const Concentration::Mobilities &data);
  void plotCurve(const Series s, const QVector<QPointF> &data);
  void plotDoubleCurve(const DoubleHypResults &dr);
  void plotPoints(const Series s, std::shared_ptr<const Analyte> a);
  void plotSingleCurve(const HypResults &r);
  void setConcentrationsList(const QList<QStandardItem *> &list);
  void setDoubleFitStats();
  void setMarkerPosition(const HyperboleFittingEngineMsgs::MarkerType marker);
  void setMobilitiesList(const QList<QStandardItem *> &list);
  void setSingleFitStats();
  void showDataSeries();
  void showStatsSeries(const StatUnits units, const StatMode mode);

  std::shared_ptr<ModeContextLimited> m_modeCtx;
  ViewMode m_viewMode;

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

  MappedVectorWrapper<bool, HyperboleFitParameters::Boolean> m_fitFixedValues;
  MappedVectorWrapper<double, HyperboleFitResults::Floating> m_fitResultsValues;
  MappedVectorWrapper<double, HyperboleFitParameters::Floating> m_fitFloatValues;
  MappedVectorWrapper<int, HyperboleFitParameters::Int> m_fitIntValues;
  MappedVectorWrapper<QString, HyperboleFitParameters::String> m_analyteNamesValues;

  /* Data exposed to GUI */
  QStandardItemModel m_analytesModel;
  QStandardItemModel m_concentrationsModel;
  QStandardItemModel m_mobilitiesModel;
  QStandardItemModel m_fitModeModel;
  QStandardItemModel m_statModeModel;
  QStandardItemModel m_statUnitsModel;
  BooleanMapperModel<HyperboleFitParameters::Boolean> m_fitFixedModel;
  FloatingMapperModel<HyperboleFitResults::Floating> m_fitResultsModel;
  FloatingMapperModel<HyperboleFitParameters::Floating> m_fitFloatModel;
  IntegerMapperModel<HyperboleFitParameters::Int> m_fitIntModel;
  StringMapperModel<HyperboleFitParameters::String> m_analyteNamesModel;

  echmet::regressCore::RectangularHyperbole<double, double> *m_singleFitRegressor;
  echmet::regressCore::RectangularHyperbole2<double, double> *m_doubleFitRegressor;

  QString m_lastDataTablePath;
  const QStringList m_dataTablesNameFilter;

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

  static const QString DATA_TABLE_FILE_SUFFIX;
  static const double INVAL_CONC_KEY;
  static const QString INVAL_ANALYTE_KEY;

  static const QString LAST_LOADSAVE_PATH_SETTINGS_TAG;

  static int constexpr seriesIndex(const Series p);

signals:
  void enableDoubleFit(const bool enable);
  void chartHorizontalMarkerIntersectionSet(const double d);
  void chartVerticalMarkerIntersectionSet(const HyperboleFittingEngineMsgs::MarkerType marker, const double d);
  void sortLists();

public slots:
  void onAddAnalyte(const QString &name);
  void onAddConcentration(const double num);
  void onAddMobility(const double u);
  void onAnalyteSwitched(const QModelIndexList &inList);
  void onChartHorizontalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker);
  void onChartVerticalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker);
  void onChartMarkerValueChanged(const HyperboleFittingEngineMsgs::MarkerType marker, const double d);
  void onConcentrationSwitched(const QModelIndex &idx);
  void onDeserialize();
  void onDoEstimate();
  void onDoFit();
  void onDoStats(const HyperboleStats::Intervals intr);
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
  void onShowChartMarker(const HyperboleFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value);
  void onStatModeChanged(const QVariant &v);
  void onStatsForAnalyteChanged(const HyperboleFittingEngineMsgs::AnalyteId aId);
  void onStatUnitsChanged(const QVariant &v);
};

#endif // HYPERBOLEFITTINGENGINE_H
