#ifndef DATAACCUMULATOR_H
#define DATAACCUMULATOR_H

#include "commonparametersengine.h"
#include "dataaccumulatormsgs.h"
#include "evaluationengine.h"
#include "evaluationenginemsgs.h"
#include "hyperbolafittingengine.h"
#include "plotcontext.h"
#include "plotcontexttypes.h"
#include "plotexporter.h"
#include "qwt_plot.h"
#include "qwt_plot_picker.h"
#include "doubleclickableqwtplotzoomer.h"

class DataAccumulator : public QObject
{
  Q_OBJECT
public:
  explicit DataAccumulator(QwtPlot *plot, QObject *parent = nullptr);
  void announceDefaultState();
  void checkForCrashRecovery();
  CommonParametersEngine *commonParametersEngine() const;
  EvaluationEngine *evaluationEngine() const;
  HyperbolaFittingEngine *hyperbolaFittingEngine() const;
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings() const;

private:
  CommonParametersEngine *m_commonParamsEngine;
  EvaluationEngine *m_evaluationEngine;
  HyperbolaFittingEngine *m_hyperbolaFittingEngine;

  QMap<PlotContextTypes::Types, std::shared_ptr<PlotContext>> m_plotCtxs;
  std::shared_ptr<PlotContext> m_currentPlotCtx;

  QwtPlot *m_plot;
  QwtPlotPicker *m_plotPicker;
  DoubleClickableQwtPlotZoomer *m_plotZoomer;

  PlotExporter *m_plotExporter;

  static const QString EVALUATIONENGINE_SETTINGS_TAG;
  static const QString HYPERBOLAFITTINGENGINE_SETTINGS_TAG;

signals:
  void programModeChanged(const DataAccumulatorMsgs::ProgramMode mode);

public slots:
  void onAdjustPlotAppearance();
  void onExportAction(const DataAccumulatorMsgs::ExportAction action);
  void onTabSwitched(const int idx);

};

#endif // DATAACCUMULATOR_H
