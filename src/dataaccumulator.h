#ifndef DATAACCUMULATOR_H
#define DATAACCUMULATOR_H

#include "commonparametersengine.h"
#include "dataaccumulatormsgs.h"
#include "evaluationengine.h"
#include "evaluationenginemsgs.h"
#include "hyperbolefittingengine.h"
#include "modecontext.h"
#include "modecontexttypes.h"
#include "qwt_plot.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_zoomer.h"

class DataAccumulator : public QObject
{
  Q_OBJECT
public:
  explicit DataAccumulator(QwtPlot *plot, QObject *parent = nullptr);
  void checkForCrashRecovery();
  CommonParametersEngine *commonParametersEngine() const;
  EvaluationEngine *evaluationEngine() const;
  HyperboleFittingEngine *hyperboleFittingEngine() const;
  void loadUserSettings(const QVariant &settings);
  QVariant saveUserSettings() const;

private:
  CommonParametersEngine *m_commonParamsEngine;
  EvaluationEngine *m_evaluationEngine;
  HyperboleFittingEngine *m_hyperboleFittingEngine;

  QMap<ModeContextTypes::Types, std::shared_ptr<ModeContext>> m_modeCtxs;
  std::shared_ptr<ModeContext> m_currentModeCtx;

  QwtPlot *m_plot;
  QwtPlotPicker *m_plotPicker;
  QwtPlotZoomer *m_plotZoomer;

  static const QString EVALUATIONENGINE_SETTINGS_TAG;
  static const QString HYPERBOLEFITTINGENGINE_SETTINGS_TAG;

public slots:
  void onAdjustPlotAppearance();
  void onTabSwitched(const int idx);

};

#endif // DATAACCUMULATOR_H
