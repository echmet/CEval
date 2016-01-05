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
  void setCommonParametersModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model);
  void setDefaultState();
  void setEvaluationBaselineModel(QAbstractItemModel *model);
  void setEvaluationHvlFixedModel(AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *model);
  void setEvaluationHvlFitIntModel(AbstractMapperModel<int, HVLFitParametersItems::Int> *model);
  void setEvaluationHvlFitModel(AbstractMapperModel<double, HVLFitResultsItems::Floating> *model);
  void setEvaluationLoadedFilesModel(QAbstractItemModel *model);
  void setEvaluationParametersAutoModel(AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model);
  void setEvaluationParametersBooleanModel(AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *model);
  void setEvaluationParametersFloatingModel(AbstractMapperModel<double, EvaluationParametersItems::Floating> *model);
  void setEvaluationResultsModel(AbstractMapperModel<double, EvaluationResultsItems::Floating> *model);
  void setEvaluationShowWindowModel(QAbstractItemModel *model);
  void setEvaluationWindowUnitsModel(QAbstractItemModel *model);

private:
  Ui::EvaluationWidget *ui;
  QSplitter m_splitter;
  CommonParametersWidget *m_commonParametersWidget;
  EvaluatedPeaksWidget *m_evaluatedPeaksWidget;
  QDataWidgetMapper *m_evaluationParametersAutoMapper;
  QDataWidgetMapper *m_evaluationParametersBooleanMapper;
  QDataWidgetMapper *m_evaluationParametersFloatingMapper;
  QDataWidgetMapper *m_evaluationResultsMapper;
  QDataWidgetMapper *m_evaluationHvlFitFixedMapper;
  QDataWidgetMapper *m_evaluationHvlFitIntMapper;
  QDataWidgetMapper *m_evaluationHvlFitMapper;

  FloatingValueDelegate m_floatingMapperDelegate;

signals:
  void closeFile(const int idx);
  void comboBoxChanged(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void doHvlFit();
  void evaluationSetDefault(const EvaluationEngineMsgs::Default msg);
  void fileSwitched(const int idx);
  void findPeaks();
  void replotHvl();

public slots:
  void onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void onFileSwitched(const int idx);

private slots:
  void onBaselineComboBoxChanged(int idx);
  void onCloseFileClicked();
  void onDoHvlFitClicked();
  void onFileComboBoxChanged(int idx);
  void onFindPeaksClicked();
  void onReplotHvl();
  void onShowWindowComboBoxChanged(int idx);
  void onWindowUnitsComboBoxChanged(int idx);
  void onEvaluationAutoModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onDefaultFinderParametersClicked();
  void onDefaultPeakPropertiesClicked();
};

#endif // EVALUATIONWIDGET_H
