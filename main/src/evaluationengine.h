#ifndef EVALUATIONENGINE_H
#define EVALUATIONENGINE_H

#include <QObject>
#include "assistedpeakfinder.h"
#include "booleanmappermodel.h"
#include "comboboxmodel.h"
#include "commonparametersengine.h"
#include "dynamiccomboboxmodel.h"
#include "evaluatedpeaksmodel.h"
#include "evaluationparametersitems.h"
#include "evaluationresultsitems.h"
#include "evaluationenginemsgs.h"
#include "floatingmappermodel.h"
#include "dataexporter/exporter.h"
#include "integermappermodel.h"
#include "mappedvectorwrapper.h"
#include "plotcontextlimited.h"
#include "peakevaluator.h"
#include "efg/efgtypes.h"
#include "gui/addpeakdialog.h"

class AddPeakDialog;
class QFileDialog;
class QMenu;
class SpecifyPeakBoundariesDialog;
class TextExporterBackendConfigurationDialog;

class EvaluationEngine : public QObject, public DataExporter::IExportable
{
  Q_OBJECT
public:
  enum class DataExporterBackends {
    TEXT,
    HTML
  };
  Q_ENUM(DataExporterBackends)

  enum class FindPeakMenuActions {
    PEAK_FROM_HERE,
    PEAK_FROM_HERE_SIGSNAP,
    SPECIFY_PEAK_BOUNDARIES,
    NOISE_REF_POINT,
    SLOPE_REF_POINT,
    SET_AXIS_TITLES,
    SET_EOF_TIME,
    SCALE_PLOT_TO_FIT
  };
  Q_ENUM(FindPeakMenuActions)

  enum class ManualIntegrationMenuActions {
    FINISH,
    FINISH_SIGSNAP,
    CANCEL
  };
  Q_ENUM(ManualIntegrationMenuActions)

  enum class PostProcessMenuActions {
    NEW_PEAK_FROM,
    NEW_PEAK_FROM_SIGSNAP,
    MOVE_PEAK_FROM,
    MOVE_PEAK_FROM_SIGSNAP,
    SPECIFY_PEAK_BOUNDARIES,
    MOVE_PEAK_TO,
    MOVE_PEAK_TO_SIGSNAP,
    DESELECT_PEAK,
    SET_AXIS_TITLES,
    SET_EOF_TIME,
    SCALE_PLOT_TO_FIT,
    SET_NOISE_REF_BL,
  };
  Q_ENUM(PostProcessMenuActions)

  enum class SetNoiseReferenceBaselineActions {
    FINISH,
    CANCEL
  };
  Q_ENUM(SetNoiseReferenceBaselineActions)

  enum class UserInteractionState {
    FINDING_PEAK,
    MANUAL_PEAK_INTEGRATION,
    PEAK_POSTPROCESSING,
    NOISE_REFERENCE_BL_SETTING
  };
  Q_ENUM(UserInteractionState)

  explicit EvaluationEngine(CommonParametersEngine *commonParamsEngine, QObject *parent = nullptr);
  ~EvaluationEngine();
  void announceDefaultState();
  AbstractMapperModel<bool, EvaluationParametersItems::Auto> *autoValuesModel();
  void assignContext(std::shared_ptr<PlotContextLimited> ctx);
  QAbstractItemModel *baselineModel();
  AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *booleanValuesModel();
  QAbstractItemModel *clipboardDataArrangementModel();
  QAbstractItemModel *evaluatedPeaksModel();
  QAbstractItemModel *exporterBackendsModel();
  QAbstractItemModel *exporterSchemesModel();
  std::tuple<AbstractMapperModel<bool, HVLExtrapolationParametersItems::Boolean> *, AbstractMapperModel<double, HVLExtrapolationParametersItems::Floating> *>
  hvlExtrapolatorModels();
  AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *hvlFitBooleanModel();
  AbstractMapperModel<int, HVLFitParametersItems::Int> *hvlFitIntModel();
  AbstractMapperModel<double, HVLFitResultsItems::Floating> *hvlFitModel();
  AbstractMapperModel<bool, HVLFitOptionsItems::Boolean> *hvlFitOptionsModel();
  QAbstractItemModel *loadedFilesModel();
  void loadUserSettings(const QVariant &settings);
  AbstractMapperModel<double, EvaluationParametersItems::Floating> *floatingValuesModel();
  AbstractMapperModel<double, EvaluationResultsItems::Floating> *resultsValuesModel();
  AbstractMapperModel<double, SNRItems::Floating> *snrModel();
  QVariant saveUserSettings() const;
  QAbstractItemModel *showWindowModel();
  QAbstractItemModel *windowUnitsModel();

private:
  enum class DataLockState : int {
    FREE,
    LOCKED_FOR_DATA_MODIFICATION,
    LOCKED_FOR_OPERATION
  };

  enum class Series : int {
    SIG,
    PEAK_HEIGHT,
    PEAK_TIME,
    BASELINE,
    EOF_MARK,
    FINDER_SYSTEM_A,
    FINDER_SYSTEM_B,
    HVL,
    PROV_PEAK,
    BASELINE_FROM,
    BASELINE_TO,
    PROV_BASELINE,
    A1_PARAM,
    PEAK_MEAN,
    HVL_EXTRAPOLATED,
    HVL_EXTRAPOLATED_BASELINE,
    NOISE_BASELINE,
    LAST_INDEX
  };

  class AssistedFinderContext {
  public:
    explicit AssistedFinderContext();
    AssistedFinderContext(const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &afAutoValues,
                          const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &afBoolValues,
                          const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &afFloatingValues,
                          const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                          const EvaluationParametersItems::ComboShowWindow showWindow,
                          const EvaluationParametersItems::ComboWindowUnits windowUnits);
    AssistedFinderContext & operator=(const AssistedFinderContext &other);
    void setProcessingData(const double noise,
                           const double noiseRefPoint, const double slopeRefPoint,
                           const double slopeThreshold, const double slopeWindow);

    const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> afAutoValues;
    const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> afBoolValues;
    MappedVectorWrapper<double, EvaluationParametersItems::Floating> afFloatingValues; /* This may need to be edited */
    const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm;
    const EvaluationParametersItems::ComboShowWindow showWindow;
    const EvaluationParametersItems::ComboWindowUnits windowUnits;

  };

  /* NOTE: This is *NOT* a thread synchronization mechanism - do not use it as such!
     This only prevents slots executing from the same event queue from messing up
     the data storage. */
  class DataAccessLocker {
  public:
    DataAccessLocker(DataLockState *lockState);
    ~DataAccessLocker();

    bool lockForDataModification();
    bool lockForOperation();

  private:
    bool lock(const DataLockState lockTo);

    DataLockState *m_lockState;
    DataLockState m_releaseToState;

  };

  class PeakContextModels {
  public:
    explicit PeakContextModels(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                               const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                               const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                               const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                               const MappedVectorWrapper<double, SNRItems::Floating> &snrValues);

    const MappedVectorWrapper<double, EvaluationResultsItems::Floating> resultsValues;
    const MappedVectorWrapper<double, HVLFitResultsItems::Floating> hvlValues;
    const MappedVectorWrapper<int, HVLFitParametersItems::Int> hvlFitIntValues;
    const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> hvlFitBooleanValues;
    const MappedVectorWrapper<double, SNRItems::Floating> snrValues;

  };

  class PeakContext : public DataExporter::IExportable {
  public:
    explicit PeakContext();
    explicit PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                         const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                         const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                         const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                         const MappedVectorWrapper<double, SNRItems::Floating> &snrValues,
                         const AssistedFinderContext &afContext,
                         const std::shared_ptr<PeakFinderResults::Result> &finderResults,
                         const double baselineSlope, const double baselineIntercept,
                         const QVector<QPointF> &hvlPlot,
                         const QVector<QPointF> &hvlPlotExtrapolated,
                         const QPointF &noiseFrom = QPointF(),
                         const QPointF &noiseTo = QPointF());
    explicit PeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                         const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                         const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                         const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                         const MappedVectorWrapper<double, SNRItems::Floating> &snrValues,
                         const AssistedFinderContext &afContext,
                         const std::shared_ptr<PeakFinderResults::Result> &finderResults,
                         const double baselineSlope, const double baselineIntercept,
                         QVector<QPointF> &&hvlPlot,
                         QVector<QPointF> &&hvlPlotExtrapolated,
                         const QPointF &noiseFrom = QPointF(),
                         const QPointF &noiseTo = QPointF());
    PeakContext(const PeakContext &other);
    void clearHvlExtrapolation();
    bool hasNoise() const;
    void setNoise(const QPointF &_noiseFrom, const QPointF &_noiseTo, MappedVectorWrapper<double, SNRItems::Floating> &_snrValues);
    void updateHvlData(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &inHvlValues,
                       const MappedVectorWrapper<int, HVLFitParametersItems::Int> &inHvlFitIntValues,
                       const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &inHvlFitBooleanValues);
    void updateHvlPlot(const QVector<QPointF> &plot, const QVector<QPointF> &plotExtrapolated = QVector<QPointF>());
    void updateHvlPlot(QVector<QPointF> &&plot, QVector<QPointF> &&plotExtrapolated);

    const MappedVectorWrapper<double, EvaluationResultsItems::Floating> resultsValues;
    const MappedVectorWrapper<double, HVLFitResultsItems::Floating> hvlValues;
    const MappedVectorWrapper<int, HVLFitParametersItems::Int> hvlFitIntValues;
    const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> hvlFitBooleanValues;
    const AssistedFinderContext afContext;
    const std::shared_ptr<PeakFinderResults::Result> finderResults;
    const double baselineSlope;
    const double baselineIntercept;
    const QVector<QPointF> hvlPlot;
    const QVector<QPointF> hvlPlotExtrapolated;

    const MappedVectorWrapper<double, SNRItems::Floating> snrValues;
    const QPointF noiseFrom;
    const QPointF noiseTo;

    PeakContext &operator=(const PeakContext &other);
    PeakContext &operator=(PeakContext &&other);
  };

  class StoredPeak {
  public:
    explicit StoredPeak();
    explicit StoredPeak(const QString &name, const PeakContext &peakCtx);

    const PeakContext &peak() const;
    void updatePeak(const PeakContext &peakCtx);
    StoredPeak &operator=(const StoredPeak &other);

    QString name;

  private:
    PeakContext m_peakCtx;

  };

  class EvaluationContext {
  public:
    EvaluationContext(const QVector<StoredPeak> &peaks, const int lastIndex);

    const QVector<StoredPeak> peaks;
    const int lastIndex;

    EvaluationContext &operator=(const EvaluationContext &other);

  };

  class DataContext {
  public:
    DataContext(std::shared_ptr<EFGData> data, const QString &name,
                const CommonParametersEngine::Context &commonCtx, const EvaluationContext &evalCtx);

    std::shared_ptr<EFGData> data;
    QString name;
    CommonParametersEngine::Context commonContext;
    EvaluationContext evaluationContext;
  };

  void activateCurrentDataContext();
  void addPeakToList(const PeakContext &ctx, const QString &name, const bool registerInHF, const RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom);
  void beginManualIntegration(const QPointF &from, const bool snap);
  void beginSetNoiseReferenceBaseline(const QPointF &from);
  double calculateA1Mobility(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues, const CommonParametersEngine::Context &commonCtx);
  bool calculateSNR(PeakContext &ctx, MappedVectorWrapper<double, SNRItems::Floating> &snrValue, const QPointF &from, const QPointF &to);
  void calculateSNRTriggered(const SetNoiseReferenceBaselineActions &action, const QPointF &to);
  void clearPeakPlots();
  void createContextMenus() noexcept(false);
  bool createSignalPlot(std::shared_ptr<EFGData> &data, const QString &name);
  DataContext currentDataContext() const;
  EvaluationContext currentEvaluationContext() const;
  QVector<bool> defaultHvlBooleanValues() const;
  QVector<int> defaultHvlIntValues() const;
  QVector<double> defaultSnrValues() const;
  void displayAssistedFinderData(const AssistedFinderContext &afContext);
  void displayCurrentPeak();
  MappedVectorWrapper<double, HVLFitResultsItems::Floating> doHvlFit(const std::shared_ptr<PeakFinderResults::Result> &finderResults,
                                                                     const double estA0, const double estA1, const double estA2, const double estA3,
                                                                     const bool fixA0, const bool fixA1, const bool fixA2, const bool fixA3,
                                                                     const double epsilon,
                                                                     const int iterations, const int digits, const double tUsp,
                                                                     const double baselineSlope, const double baselineIntercept,
                                                                     bool *ok);
  void drawEofMarker();
  PeakContext duplicatePeakContext() const noexcept(false);
  QVector<double> emptyHvlValues() const;
  QVector<double> emptyResultsValues() const;
  void findPeakAssisted();
  void findPeakManually(const QPointF &from, const QPointF &to, const bool snapFrom, const bool snapTo, const bool updatePeak = false);
  void findPeakMenuTriggered(const FindPeakMenuActions &action, const QPointF &point);
  void findPeakPreciseBoundaries();
  EvaluationContext freshEvaluationContext() const;
  PeakContext freshPeakContext() const noexcept(false);
  void fullViewUpdate();
  void initClipboardExporter();
  bool initDataExporter();
  bool isContextAccessible(DataAccessLocker &locker);
  bool isContextValid() const;
  QVector<EvaluatedPeaksModel::EvaluatedPeak> makeEvaluatedPeaks();
  PeakEvaluator::Parameters makeEvaluatorParameters(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults::Result> &fr);
  AssistedPeakFinder::Parameters makeFinderParameters();
  PeakContext makePeakContext(const PeakContextModels &models,
                              const AssistedFinderContext &afContext,
                              const std::shared_ptr<PeakFinderResults::Result>  &fr,
                              const PeakEvaluator::Results &er,
                              const QVector<QPointF> &hvlPlot,
                              const QVector<QPointF> &hvlPlotExtrapolated) const;
  PeakContext makePeakContext(const PeakContextModels &models,
                              const AssistedFinderContext &afContext,
                              const std::shared_ptr<PeakFinderResults::Result>  &fr,
                              const PeakEvaluator::Results &er,
                              QVector<QPointF> &&hvlPlot,
                              QVector<QPointF> &&hvlPlotExtrapolated) const;
  PeakContext makePeakContext(const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                              const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                              const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                              const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                              const MappedVectorWrapper<double, SNRItems::Floating> &snrValues,
                              const PeakContext &oldPeak) const;
  PeakContextModels makePeakContextModels(const std::shared_ptr<PeakFinderResults::Result> &fr, const PeakEvaluator::Results &er, const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlResults,
                                          const MappedVectorWrapper<int, HVLFitParametersItems::Int> &hvlFitIntValues,
                                          const MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> &hvlFitBooleanValues,
                                          const MappedVectorWrapper<double, SNRItems::Floating> &snrValues) const;
  void manualIntegrationMenuTriggered(const ManualIntegrationMenuActions &action, const QPointF &point);
  void plotEvaluatedPeak(const std::shared_ptr<PeakFinderResults::Result> &fr, const double peakX,
                         const double widthHalfLeft, const double widthHalfRight,
                         const double peakHeight, const double peakHeightBaseline,
                         const double centroidX,
                         const QPointF &noiseFrom, const QPointF &noiseTo);
  void postProcessMenuTriggered(const PostProcessMenuActions &action, const QPointF &point);
  PeakContext processFoundPeak(const QVector<QPointF> &data, const std::shared_ptr<PeakFinderResults::Result> &fr,
                               const AssistedFinderContext &afContext, const bool updateCurrentPeak, const bool doHvlFitRq,
                               const PeakContext &srcCtx);
  void replotHvl(const double a0, const double a1, const double a2, const double a3, const double from, const double to, const double step);
  void showSetAxisTitlesDialog();
  void setAxisTitles();
  void setDefaultFinderParameters();
  void setEofTime(const QPointF& point);
  bool setEvaluationContext(const EvaluationContext &ctx);
  void setEvaluationResults(const PeakContext &ctx);
  bool setPeakContext(const PeakContext &ctx);
  void setPeakFinderParameters(const double maxX);
  bool storeCurrentContext();
  void storeCurrentPeak();
  void switchEvaluationContext(const QString &key);
  void switchWindowUnit(const EvaluationParametersItems::ComboWindowUnits unit);
  double timeStep(const int fromIdx = 0, const int toIdx = 1);
  void walkFoundPeaks(const QVector<std::shared_ptr<PeakFinderResults::Result>> &results, const AssistedFinderContext &afContext, const bool updatePeak = false);

  /* All data contexts */
  DataLockState m_dataLockState;
  QMap<QString, std::shared_ptr<DataContext>> m_allDataContexts;
  std::shared_ptr<DataContext> m_currentDataContext;
  QString m_currentDataContextKey;
  /* Current peaks */
  QVector<StoredPeak> m_allPeaks;
  PeakContext m_currentPeak;
  int m_currentPeakIdx;

  AddPeakDialog *m_addPeakDlg;
  std::unique_ptr<QMenu> m_findPeakMenu;
  std::unique_ptr<QMenu> m_manualIntegrationMenu;
  std::unique_ptr<QMenu> m_postProcessMenu;
  std::unique_ptr<QMenu> m_setNoiseReferenceBaselineMenu;
  UserInteractionState m_userInteractionState;

  QPointF m_noiseReferenceBaselineFrom;
  QPointF m_manualPeakFrom;
  bool m_manualPeakSnapFrom;

  CommonParametersEngine *m_commonParamsEngine;
  std::shared_ptr<PlotContextLimited> m_plotCtx;
  MappedVectorWrapper<bool, EvaluationParametersItems::Auto> m_evaluationAutoValues;
  MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> m_evaluationBooleanValues;
  MappedVectorWrapper<double, EvaluationParametersItems::Floating> m_evaluationFloatingValues;
  EvaluationParametersItems::ComboWindowUnits m_windowUnit;
  EvaluationParametersItems::ComboBaselineAlgorithm m_baselineAlgorithm;
  EvaluationParametersItems::ComboShowWindow m_showWindow;
  MappedVectorWrapper<double, EvaluationResultsItems::Floating> m_resultsNumericValues;
  MappedVectorWrapper<int, HVLFitParametersItems::Int> m_hvlFitIntValues;
  MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> m_hvlFitBooleanValues;
  MappedVectorWrapper<double, HVLFitResultsItems::Floating> m_hvlFitValues;
  MappedVectorWrapper<bool, HVLFitOptionsItems::Boolean> m_hvlFitOptionsValues;
  MappedVectorWrapper<bool, HVLExtrapolationParametersItems::Boolean> m_hvlExtrapolationBooleanValues;
  MappedVectorWrapper<double, HVLExtrapolationParametersItems::Floating> m_hvlExtrapolationFloatingValues;
  MappedVectorWrapper<double, SNRItems::Floating> m_snrFloatingValues;

  ComboBoxModel<EvaluationParametersItems::ComboBaselineAlgorithm> m_baselineAlgorithmModel;
  EvaluatedPeaksModel m_evaluatedPeaksModel;
  BooleanMapperModel<EvaluationParametersItems::Auto> m_evaluationAutoModel;
  BooleanMapperModel<EvaluationParametersItems::Boolean> m_evaluationBooleanModel;
  FloatingMapperModel<EvaluationParametersItems::Floating> m_evaluationFloatingModel;
  DynamicComboBoxModel<QString> m_loadedFilesModel;
  ComboBoxModel<EvaluationParametersItems::ComboShowWindow> m_showWindowModel;
  ComboBoxModel<EvaluationParametersItems::ComboWindowUnits> m_windowUnitsModel;
  FloatingMapperModel<EvaluationResultsItems::Floating> m_resultsFloatingModel;
  IntegerMapperModel<HVLFitParametersItems::Int> m_hvlFitIntModel;
  BooleanMapperModel<HVLFitParametersItems::Boolean> m_hvlFitBooleanModel;
  FloatingMapperModel<HVLFitResultsItems::Floating> m_hvlFitModel;
  BooleanMapperModel<HVLFitOptionsItems::Boolean> m_hvlFitOptionsModel;
  BooleanMapperModel<HVLExtrapolationParametersItems::Boolean> m_hvlExtrapolationBooleanModel;
  FloatingMapperModel<HVLExtrapolationParametersItems::Floating> m_hvlExtrapolationFloatingModel;
  FloatingMapperModel<SNRItems::Floating> m_snrFloatingModel;

  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboWindowUnits>> s_windowUnitsValues;
  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboBaselineAlgorithm>> s_baselineAlgorithmValues;
  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboShowWindow>> s_showWindowValues;

  static const QVector<bool> s_defaultEvaluationAutoValues;
  static const QVector<bool> s_defaultEvaluationBooleanValues;
  static const QVector<double> s_defaultEvaluationFloatingValues;

  static const EvaluationParametersItems::ComboBaselineAlgorithm s_defaultBaselineAlgorithm;
  static const EvaluationParametersItems::ComboShowWindow s_defaultShowWindow;
  static const EvaluationParametersItems::ComboWindowUnits s_defaultWindowUnits;

  static const QString s_serieBaselineTitle;
  static const QString s_serieEofTitle;
  static const QString s_seriePeakHeightTitle;
  static const QString s_seriePeakTimeTitle;
  static const QString s_serieProvisionalPeakTitle;
  static const QString s_serieSignalTitle;
  static const QString s_serieFinderSystemATitle;
  static const QString s_serieFinderSystemBTitle;
  static const QString s_serieHVLTitle;
  static const QString s_serieBaselineFromTitle;
  static const QString s_serieBaselineToTitle;
  static const QString s_serieProvisionalBaseline;
  static const QString s_serieA1Param;
  static const QString s_seriePeakMean;
  static const QString s_serieHVLExtrapolated;
  static const QString s_serieHVLExtrapolatedBaseline;
  static const QString s_serieNoiseBaseline;

  static const QString s_emptyCtxKey;

  static const double s_defaultHvlEpsilon;
  static const int s_defaultHvlDigits;
  static const int s_defaultHvlIterations;
  static const int s_defaultSNRAmplifier;

  static const QString HVLFITOPTIONS_DISABLE_AUTO_FIT_TAG;
  static const QString HVLFITOPTIONS_SHOW_FIT_STATS_TAG;
  static const QString CLIPBOARDEXPORTER_DELIMTIER_TAG;
  static const QString CLIPBOARDEXPORTER_DATAARRANGEMENT_TAG;

  static int seriesIndex(const Series iid);

  /* Data export section*/
  DataExporter::Exporter m_dataExporter;
  QStandardItemModel *m_dataExporterBackendsModel;
  QString m_currentDataExporterSchemeId;
  DataExporterBackends m_currentDataExporterBackend;
  QFileDialog *m_dataExporterFileDlg;
  SpecifyPeakBoundariesDialog *m_specifyPeakBoundsDlg;
  TextExporterBackendConfigurationDialog *m_textDataExporterCfgDlg;
  QChar m_textDataExporterDelimiter;
  bool m_exportOnFileLeftEnabled;
  /* Export to clipboard section */
  DataExporter::SchemeBaseRoot *m_ctcEofSchemeBase;
  DataExporter::SchemeBaseRoot *m_ctcHvlSchemeBase;
  DataExporter::SchemeBaseRoot *m_ctcPeakSchemeBase;
  DataExporter::SchemeBaseRoot *m_ctcPeakDimsSchemeBase;
  DataExporter::Scheme *m_ctcEofScheme;
  DataExporter::Scheme *m_ctcHvlScheme;
  DataExporter::Scheme *m_ctcPeakScheme;
  DataExporter::Scheme *m_ctcPeakDimsScheme;
  QString m_ctcDelimiter;
  DataExporter::Globals::DataArrangement m_ctcDataArrangement;
  QStandardItemModel *m_ctcDataArrangementModel;

signals:
  void clipboardExporterDataArrangementSet(const QModelIndex &idx);
  void clipboardExporterDelimiterSet(const QString &text);
  void comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void evaluationFileAdded(const int idx);
  void evaluationFileSwitched(const int idx);
  void exporterBackendSet(const QModelIndex &idx);
  void updateTEof(const double t);
  void registerMeasurement(const QString &name, const double selConcentration, const double mobility);

public slots:
  void onAddPeak();
  void onCancelEvaluatedPeakSelection();
  void onClipboardExporterDataArrangementChanged(const QModelIndex &idx);
  void onClipboardExporterDelimiterChanged(const QString &delimiter);
  void onCloseCurrentEvaluationFile(const int idx);
  void onComboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void onCommonParametersChanged();
  void onConfigureExporterBackend();
  void onCopyToClipboard(const EvaluationEngineMsgs::CopyToClipboard ctc);
  void onDeletePeak(const QModelIndex &idx);
  void onDoHvlFit();
  void onEvaluationFileSwitched(const int idx);
  void onExporterBackendChanged(const QModelIndex &idx);
  void onExporterSchemeChanged(const QModelIndex &idx);
  void onExportFileOnLeftToggled(const bool enabled);
  void onExportScheme();
  void onFindPeaks();
  void onHvlExtrapolationToggled(const bool enabled);
  void onManageExporterScheme();
  void onNoEofStateChanged(const bool noEof);
  void onPeakSwitched(const QModelIndex &idx);
  void onPlotPointHovered(const QPointF &point, const QPoint &cursor);
  void onPlotPointSelected(const QPointF &point, const QPoint &cursor);
  void onReadEof();
  void onRegisterPeakInHyperbolaFit(const QModelIndex &idx);
  void onRenamePeak(const QModelIndex &idx);
  void onReplotHvl();
  void onSetDefault(EvaluationEngineMsgs::Default msg);
  void onTraverseFiles(const EvaluationEngineMsgs::Traverse dir);

private slots:
  void onDataLoaded(EFGDataSharedPtr data, QString fileID, QString fileName);
  void onHvlParametersModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onHvlResultsModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onProvisionalPeakSelected(const QModelIndex index, const QAbstractItemModel *model, const int peakWindow);
  void onUnhighlightProvisionalPeak();

};

#endif // EVALUATIONENGINE_H
