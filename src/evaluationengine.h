#ifndef EVALUATIONENGINE_H
#define EVALUATIONENGINE_H

#include <QObject>
#include "booleanmappermodel.h"
#include "comboboxmodel.h"
#include "commonparametersengine.h"
#include "datafileloader.h"
#include "dynamiccomboboxmodel.h"
#include "evaluatedpeaksmodel.h"
#include "evaluationparametersitems.h"
#include "evaluationresultsitems.h"
#include "evaluationenginemsgs.h"
#include "floatingmappermodel.h"
#include "hvlcalculator.h"
#include "integermappermodel.h"
#include "mappedvectorwrapper.h"
#include "modecontextlimited.h"
#include "peakevaluator.h"
#include "assistedpeakfinder.h"

class AddPeakDialog;
class QMenu;

class EvaluationEngine : public QObject
{
  Q_OBJECT
public:
  enum class FindPeakMenuActions {
    PEAK_FROM_HERE,
    NOISE_REF_POINT,
    SLOPE_REF_POINT,
    SET_AXIS_TITLES
  };
  Q_ENUM(FindPeakMenuActions)

  enum class ManualIntegrationMenuActions {
    FINISH,
    CANCEL
  };
  Q_ENUM(ManualIntegrationMenuActions)

  enum class PostProcessMenuActions {
    PEAK_FROM_THIS_X,
    PEAK_FROM_THIS_Y,
    PEAK_FROM_THIS_XY,
    PEAK_TO_THIS_X,
    PEAK_TO_THIS_Y,
    PEAK_TO_THIS_XY,
    SET_AXIS_TITLES
  };
  Q_ENUM(PostProcessMenuActions)

  enum class UserInteractionState {
    FINDING_PEAK,
    MANUAL_PEAK_INTEGRATION,
    PEAK_POSTPROCESSING
  };
  Q_ENUM(UserInteractionState)

  explicit EvaluationEngine(CommonParametersEngine *commonParamsEngine, QObject *parent = nullptr);
  ~EvaluationEngine();
  AbstractMapperModel<bool, EvaluationParametersItems::Auto> *autoValuesModel();
  void assignContext(std::shared_ptr<ModeContextLimited> ctx);
  QAbstractItemModel *baselineModel();
  AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *booleanValuesModel();
  const DataFileLoader *dataFileLoader() const { return m_dataFileLoader; }
  QAbstractItemModel *evaluatedPeaksModel();
  AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *hvlFitFixedModel();
  AbstractMapperModel<int, HVLFitParametersItems::Int> *hvlFitIntModel();
  AbstractMapperModel<double, HVLFitResultsItems::Floating> *hvlFitModel();
  QAbstractItemModel *loadedFilesModel();
  void loadUserSettings(const QVariant &settings);
  AbstractMapperModel<double, EvaluationParametersItems::Floating> *floatingValuesModel();
  AbstractMapperModel<double, EvaluationResultsItems::Floating> *resultsValuesModel();
  QVariant saveUserSettings() const;
  QAbstractItemModel *showWindowModel();
  QAbstractItemModel *windowUnitsModel();

private:
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
    LAST_INDEX
  };

  class PeakContext {
  public:
    explicit PeakContext(const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> &autoValues,
                         const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> &boolValues,
                         const MappedVectorWrapper<double, EvaluationParametersItems::Floating> &floatingValues,
                         const MappedVectorWrapper<double, EvaluationResultsItems::Floating> &resultsValues,
                         const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues,
                         const EvaluationParametersItems::ComboWindowUnits windowUnit, const EvaluationParametersItems::ComboShowWindow showWindow,
                         const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm,
                         const PeakFinderResults finderResults, const QVector<QPointF> &hvlPlot);
    explicit PeakContext();
    void setPeakName(const QString &name);

    const MappedVectorWrapper<bool, EvaluationParametersItems::Auto> autoValues;
    const MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> boolValues;
    const MappedVectorWrapper<double, EvaluationParametersItems::Floating> floatingValues;
    const MappedVectorWrapper<double, EvaluationResultsItems::Floating> resultsValues;
    const MappedVectorWrapper<double, HVLFitResultsItems::Floating> hvlValues;
    const EvaluationParametersItems::ComboWindowUnits windowUnit;
    const EvaluationParametersItems::ComboShowWindow showWindow;
    const EvaluationParametersItems::ComboBaselineAlgorithm baselineAlgorithm;
    const PeakFinderResults finderResults;
    const QVector<QPointF> hvlPlot;
    const QString peakName;

    PeakContext &operator=(const PeakContext &other);
  };

  class EvaluationContext {
  public:
    EvaluationContext(const QVector<PeakContext> &peaks, const int lastIndex);

    const QVector<PeakContext> peaks;
    const int lastIndex;

    EvaluationContext &operator=(const EvaluationContext &other);

  };

  class DataContext {
  public:
    DataContext(std::shared_ptr<DataFileLoader::Data> data, const QString &name,
                const CommonParametersEngine::Context &commonCtx, const EvaluationContext &evalCtx);

    std::shared_ptr<DataFileLoader::Data> data;
    QString name;
    CommonParametersEngine::Context commonContext;
    EvaluationContext evaluationContext;
  };

  double calculateA1Mobility(const MappedVectorWrapper<double, HVLFitResultsItems::Floating> &hvlValues, const MappedVectorWrapper<double, CommonParametersItems::Floating> commonData);
  void clearPeakPlots();
  void connectPeakUpdate();
  void createContextMenus() throw(std::bad_alloc);
  bool createSignalPlot(std::shared_ptr<DataFileLoader::Data> data, const QString &name);
  DataContext currentDataContext() const;
  EvaluationContext currentEvaluationContext() const;
  PeakContext currentPeakContext(const PeakFinderResults finderResults, const QVector<QPointF> &hvlPlot) const;
  void disconnectPeakUpdate();
  void displayAutomatedResults(const PeakFinderResults &fr, const PeakEvaluator::Results &er);
  PeakContext duplicatePeakContext() const;
  QVector<double> emptyHvlValues() const;
  QVector<double> emptyResultsValues() const;
  void findPeak(bool useCurrentPeak);
  void findPeakManually(const QPointF &from, const QPointF &to);
  void findPeakMenuTriggered(const FindPeakMenuActions &action, const QPointF &point);
  EvaluationContext freshEvaluationContext() const;
  PeakContext freshPeakContext() const;
  void fullViewUpdate();
  bool isContextValid() const;
  QVector<EvaluatedPeaksModel::EvaluatedPeak> makeEvaluatedPeaks();
  PeakEvaluator::Parameters makeEvaluatorParameters(const QVector<QPointF> &data, const PeakFinderResults &fr);
  AssistedPeakFinder::Parameters makeFinderParameters(const bool autoPeakProps);
  void manualIntegrationMenuTriggered(const ManualIntegrationMenuActions &action, const QPointF &point);
  void plotEvaluatedPeak(const PeakFinderResults &fr);
  void postProcessMenuTriggered(const PostProcessMenuActions &action, const QPointF &point);
  void processFoundPeak(const QVector<QPointF> &data, const PeakFinderResults &fr, const bool useCurrentPeak = false);
  void showSetAxisTitlesDialog();
  void setAxisTitles();
  void setDefaultFinderParameters();
  void setDefaultPeakProperties();
  bool setEvaluationContext(const EvaluationContext &ctx);
  void setEvaluationResults(const PeakFinderResults &fr, const PeakEvaluator::Results &er);
  bool setPeakContext(const PeakContext &ctx);
  void setPeakFinderParameters(const double maxX);
  bool storeCurrentContext();
  void switchEvaluationContext(const QString &key);
  void switchWindowUnit(const EvaluationParametersItems::ComboWindowUnits unit);
  double timeStep();

  /* All data contexts */
  QMap<QString, std::shared_ptr<DataContext>> m_allDataContexts;
  std::shared_ptr<DataContext> m_currentDataContext;
  QString m_currentDataContextKey;
  /* Current peaks */
  QVector<PeakContext> m_allPeaks;
  PeakContext m_currentPeak;
  int m_currentPeakIdx;

  DataFileLoader *m_dataFileLoader;
  AddPeakDialog *m_addPeakDlg;
  QMenu *m_findPeakMenu;
  QMenu *m_manualIntegrationMenu;
  QMenu *m_postProcessMenu;
  UserInteractionState m_userInteractionState;

  QPointF m_manualPeakFrom;

  CommonParametersEngine *m_commonParamsEngine;
  std::shared_ptr<ModeContextLimited> m_modeCtx;
  MappedVectorWrapper<bool, EvaluationParametersItems::Auto> m_evaluationAutoValues;
  MappedVectorWrapper<bool, EvaluationParametersItems::Boolean> m_evaluationBooleanValues;
  MappedVectorWrapper<double, EvaluationParametersItems::Floating> m_evaluationFloatingValues;
  EvaluationParametersItems::ComboWindowUnits m_windowUnit;
  EvaluationParametersItems::ComboBaselineAlgorithm m_baselineAlgorithm;
  EvaluationParametersItems::ComboShowWindow m_showWindow;
  MappedVectorWrapper<double, EvaluationResultsItems::Floating> m_resultsNumericValues;
  MappedVectorWrapper<int, HVLFitParametersItems::Int> m_hvlFitIntValues;
  MappedVectorWrapper<bool, HVLFitParametersItems::Boolean> m_hvlFitFixedValues;
  MappedVectorWrapper<double, HVLFitResultsItems::Floating> m_hvlFitValues;

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
  BooleanMapperModel<HVLFitParametersItems::Boolean> m_hvlFixedModel;
  FloatingMapperModel<HVLFitResultsItems::Floating> m_hvlFitModel;

  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboWindowUnits>> s_windowUnitsValues;
  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboBaselineAlgorithm>> s_baselineAlgorithmValues;
  static const QVector<ComboBoxItem<EvaluationParametersItems::ComboShowWindow>> s_showWindowValues;

  static const QVector<bool> s_defaultEvaluationAutoValues;
  static const QVector<bool> s_defaultEvaluationBooleanValues;
  static const QVector<double> s_defaultEvaluationFloatingValues;

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

  static const QString s_emptyCtxKey;

  static const double s_defaultHvlEpsilon;
  static const int s_defaultHvlDigits;
  static const int s_defaultHvlIterations;

  static const QString DATAFILELOADER_SETTINGS_TAG;

  static int seriesIndex(const Series iid);

signals:
  void comboBoxIndexChanged(EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void evaluationFileAdded(const int idx);
  void evaluationFileSwitched(const int idx);
  void updateTEof(const double t);
  void registerMeasurement(const QString &name, const double selConcentration, const double mobility);

public slots:
  void onAddPeak();
  void onCancelEvaluatedPeakSelection();
  void onCloseCurrentEvaluationFile(const int idx);
  void onComboBoxChanged(EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void onDeletePeak(const QModelIndex &idx);
  void onDoHvlFit(const bool showStats);
  void onEvaluationFileSwitched(const int idx);
  void onFindPeaks();
  void onPeakSwitched(const QModelIndex &idx);
  void onPlotPointHovered(const QPointF &point, const QPoint &cursor);
  void onPlotPointSelected(const QPointF &point, const QPoint &cursor);
  void onReadEof();
  void onReplotHvl();
  void onSetDefault(EvaluationEngineMsgs::Default msg);
  void onUpdateCurrentPeak();

private slots:
  void onDataLoaded(std::shared_ptr<DataFileLoader::Data> data, const QString &fullPath, const QString &fileName);
  void onHvlResultsModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onProvisionalPeakSelected(const QModelIndex index, const QAbstractItemModel *model, const long peakWindow);
  void onUnhighlightProvisionalPeak();

};

#endif // EVALUATIONENGINE_H
