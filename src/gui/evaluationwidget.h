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
  void setClipboardDataArrangementModel(QAbstractItemModel *model);
  void setDefaultState();
  void setEvaluationBaselineModel(QAbstractItemModel *model);
  void setEvaluationHvlExtrapolationModels(std::tuple<AbstractMapperModel<bool, HVLExtrapolationParametersItems::Boolean> *, AbstractMapperModel<double, HVLExtrapolationParametersItems::Floating> *> models);
  void setEvaluationHvlBooleanModel(AbstractMapperModel<bool, HVLFitParametersItems::Boolean> *model);
  void setEvaluationHvlFitIntModel(AbstractMapperModel<int, HVLFitParametersItems::Int> *model);
  void setEvaluationHvlFitModel(AbstractMapperModel<double, HVLFitResultsItems::Floating> *model);
  void setEvaluationHvlFitOptionsModel(AbstractMapperModel<bool, HVLFitOptionsItems::Boolean> *model);
  void setEvaluationParametersAutoModel(AbstractMapperModel<bool, EvaluationParametersItems::Auto> *model);
  void setEvaluationParametersBooleanModel(AbstractMapperModel<bool, EvaluationParametersItems::Boolean> *model);
  void setEvaluationParametersFloatingModel(AbstractMapperModel<double, EvaluationParametersItems::Floating> *model);
  void setEvaluationResultsModel(AbstractMapperModel<double, EvaluationResultsItems::Floating> *model);
  void setEvaluationShowWindowModel(QAbstractItemModel *model);
  void setEvaluationWindowUnitsModel(QAbstractItemModel *model);
  void setExporterBackendsModel(QAbstractItemModel *model);
  void setExporterSchemesModel(QAbstractItemModel *model);
  void setSNRModel(AbstractMapperModel<double, SNRItems::Floating> *model);

private:
  Ui::EvaluationWidget *ui;
  QDataWidgetMapper *m_evaluationParametersAutoMapper;
  QDataWidgetMapper *m_evaluationParametersBooleanMapper;
  QDataWidgetMapper *m_evaluationParametersFloatingMapper;
  QDataWidgetMapper *m_evaluationResultsMapper;
  QDataWidgetMapper *m_evaluationHvlExtrapolationBooleanMapper;
  QDataWidgetMapper *m_evaluationHvlExtrapolationFloatingMapper;
  QDataWidgetMapper *m_evaluationHvlFitBooleanMapper;
  QDataWidgetMapper *m_evaluationHvlFitIntMapper;
  QDataWidgetMapper *m_evaluationHvlFitMapper;
  QDataWidgetMapper *m_evaluationHvlFitOptionsMapper;
  QDataWidgetMapper *m_snrMapper;

  FloatingValueDelegate m_floatingMapperDelegate;

signals:
  void clipboardExporterArrangementChanged(const QModelIndex &idx);
  void clipboardExporterDelimiterChanged(const QString &delimiter);
  void copyToClipboard(const EvaluationEngineMsgs::CopyToClipboard ctc);
  void comboBoxChanged(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void configureExporterBackend();
  void doHvlFit();
  void evaluationSetDefault(const EvaluationEngineMsgs::Default msg);
  void exporterBackendChanged(const QModelIndex &idx);
  void exportOnFileLeftToggled(const bool enabled);
  void exportScheme();
  void exporterSchemeChanged(const QModelIndex &idx);
  void findPeaks();
  void hvlExtrapolationToggled(const bool enabled);
  void manageExporterSchemes();
  void replotHvl();

public slots:
  void onClipboardExporterDataArrangementSet(const QModelIndex &midx);
  void onClipboardExporterDelimiterSet(const QString &text);
  void onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void onExporterBackendSet(const QModelIndex &idx);

private slots:
  void onBaselineComboBoxChanged(int idx);
  void onClipboardExporterArrangementChanged(const int idx);
  void onClipboardExporterDelimiterTextChanged(const QString &text);
  void onConfigureExporterBackendClicked();
  void onCtcEOFClicked();
  void onCtcHVLClicked();
  void onCtcPeakClicked();
  void onCtcPeakDimsClicked();
  void onDoHvlFitClicked();
  void onExporterBackendComboBoxChanged(int idx);
  void onExporterSchemeComboBoxChanged(int idx);
  void onExportOnFileLeftClicked();
  void onExportSchemeClicked();
  void onFindPeaksClicked();
  void onManageExporterSchemesClicked();
  void onReplotHvl();
  void onShowWindowComboBoxChanged(int idx);
  void onWindowUnitsComboBoxChanged(int idx);
  void onEvaluationAutoModelChanged(QModelIndex topLeft, QModelIndex bottomRight, QVector<int> roles);
  void onDefaultFinderParametersClicked();
  void on_qcb_enableHvlExtrapolation_clicked(bool checked);
};

#endif // EVALUATIONWIDGET_H
