#ifndef HYPERBOLEFITWIDGET_H
#define HYPERBOLEFITWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QSortFilterProxyModel>
#include "../abstractmappermodel.h"
#include "../floatingvaluedelegate.h"
#include "../hyperbolefititems.h"
#include "../hyperbolefittingenginemsgs.h"

namespace Ui {
class HyperboleFitWidget;
}

class HyperboleFitWidget : public QWidget
{
  Q_OBJECT
public:
  explicit HyperboleFitWidget(QWidget *parent = nullptr);
  ~HyperboleFitWidget();
  void connectToAccumulator(QObject *dac);
  void setAnalytesModel(QAbstractItemModel *model);
  void setAnalyteNamesModel(AbstractMapperModel<QString, HyperboleFitParameters::String> *model);
  void setConcentrationsModel(QAbstractItemModel *model);
  void setFitFixedModel(AbstractMapperModel<bool, HyperboleFitParameters::Boolean> *model);
  void setFitFloatModel(AbstractMapperModel<double, HyperboleFitParameters::Floating> *model);
  void setFitIntModel(AbstractMapperModel<int, HyperboleFitParameters::Int> *model);
  void setFitResultsModel(AbstractMapperModel<double, HyperboleFitResults::Floating> *model);
  void setFitModeModel(QAbstractItemModel *model);
  void setMobilitiesModel(QAbstractItemModel *model);
  void setStatModeModel(QAbstractItemModel *model);
  void setStatUnitsModel(QAbstractItemModel *model);

private:
  Ui::HyperboleFitWidget *ui;

  QDataWidgetMapper m_fitFixedMapper;
  QDataWidgetMapper m_fitFloatMapper;
  QDataWidgetMapper m_fitIntMapper;
  QDataWidgetMapper m_fitResultsMapper;
  QDataWidgetMapper m_analyteNamesMapper;

  QSortFilterProxyModel m_concentrationsSortProxy;
  QSortFilterProxyModel m_mobilitiesSortProxy;

  FloatingValueDelegate m_floatingDelegate;

signals:
  void addAnalyte(const QString &name);
  void addConcentration(const double num);
  void addMobility(const double num);
  void analyteSwitched(const QModelIndexList &list);
  void chartMarkerValueChanged(const HyperboleFittingEngineMsgs::MarkerType marker, const QString &value);
  void concentrationSwitched(const QModelIndex &idx);
  void doEstimate();
  void doFit();
  void doStats(const HyperboleStats::Intervals intr);
  void fitModeChanged(const QVariant &v);
  void redrawDataSeries();
  void removeAnalyte(const QModelIndex &idx);
  void removeConcentration(const QModelIndex &idx);
  void removeMobility(const QModelIndex &idx);
  void renameAnalyte(const QVariant &internalId, const QString &newName, const int idx);
  void showChartMarker(const HyperboleFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value);
  void statModeChanged(const QVariant &v);
  void statUnitsChanged(const QVariant &v);
  void swapAnalytesChanged(const bool swap);

private slots:
  void onAddAnalyteClicked();
  void onAddConcentrationClicked();
  void onAddMobilityClicked();
  void onAnalyteListClicked();
  void onAnalyteListDoubleClicked(const QModelIndex &idx);
  void onChartHorizontalMarkerValueChanged(const QString &value);
  void onConcentrationsListClicked(const QModelIndex &idx);
  void onEstimateClicked();
  void onFitClicked();
  void onFitModeActivated(const int idx);
  void onRedrawClicked();
  void onRemoveAnalyteClicked();
  void onRemoveConcentrationClicked();
  void onRemoveMobilityClicked();
  void onShowChartHorizontalMarker();
  void onStatBothClicked();
  void onStatLeftClicked();
  void onStatRightClicked();
  void onStatModeActivated(const int idx);
  void onStatUnitsActivated(const int idx);
  void onSwapAnalytesClicked();

public slots:
  void onEnableDoubleFit(const bool enable);
  void onSwapAnalyteNamesModel(const bool swap);

};

#endif // HYPERBOLEFITWIDGET_H
