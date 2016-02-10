#ifndef EVALUATIONWIDGET_H
#define EVALUATIONWIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QDataWidgetMapper>
#include "commonparameterswidget.h"
#include "evaluatedpeakswidget.h"
#include "../abstractmappermodel.h"
#include "../evaluationenginemsgs.h"
#include "../evaluationparametersitems.h"
#include "../evaluationresultsitems.h"
#include "../floatingvaluedelegate.h"

namespace Ui {
class EvaluationWidget;
}

class EvaluationWidget : public QWidget
{
  Q_OBJECT
public:
  explicit EvaluationWidget(QWidget *parent = nullptr);
  ~EvaluationWidget();
  void connectToAccumulator(QObject *dac);
  void setDefaultState();
  void setEvaluationBaselineModel(QAbstractItemModel *model);
  void setEvaluationHvlFixedModel(AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *model);
  void setEvaluationHvlFitIntModel(AbstractMapperModel<int, HVLFitParametersItems::Int> *model);
  void setEvaluationHvlFitModel(AbstractMapperModel<double, HVLFitResultsItems::Floating> *model);
  void setEvaluationParametersAutoModel(AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model);
  void setEvaluationParametersBooleanModel(AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *model);
  void setEvaluationParametersFloatingModel(AbstractMapperModel<double, EvaluationParametersItems::Floating> *model);
  void setEvaluationResultsModel(AbstractMapperModel<double, EvaluationResultsItems::Floating> *model);
  void setEvaluationShowWindowModel(QAbstractItemModel *model);
  void setEvaluationWindowUnitsModel(QAbstractItemModel *model);

private:
  Ui::EvaluationWidget *ui;
  QDataWidgetMapper *m_evaluationParametersAutoMapper;
  QDataWidgetMapper *m_evaluationParametersBooleanMapper;
  QDataWidgetMapper *m_evaluationParametersFloatingMapper;
  QDataWidgetMapper *m_evaluationResultsMapper;
  QDataWidgetMapper *m_evaluationHvlFitFixedMapper;
  QDataWidgetMapper *m_evaluationHvlFitIntMapper;
  QDataWidgetMapper *m_evaluationHvlFitMapper;

  FloatingValueDelegate m_floatingMapperDelegate;

signals:
  void comboBoxChanged(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void doHvlFit();
  void evaluationSetDefault(const EvaluationEngineMsgs::Default msg);
  void findPeaks();
  void replotHvl();

public slots:
  void onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier);

private slots:
  void onBaselineComboBoxChanged(int idx);
  void onDoHvlFitClicked();
  void onFindPeaksClicked();
  void onReplotHvl();
  void onShowWindowComboBoxChanged(int idx);
  void onWindowUnitsComboBoxChanged(int idx);
  void onEvaluationAutoModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onDefaultFinderParametersClicked();
  void onDefaultPeakPropertiesClicked();
};

#endif // EVALUATIONWIDGET_H
