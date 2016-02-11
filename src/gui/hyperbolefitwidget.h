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
#include "../inumberformatchangeable.h"
#include "../scrollareaeventfilter.h"

namespace Ui {
class HyperboleFitWidget;
}

class HyperboleFitWidget : public QWidget, public INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(INumberFormatChangeable)
public:
  explicit HyperboleFitWidget(QWidget *parent = nullptr);
  ~HyperboleFitWidget();
  void connectToAccumulator(QObject *dac);
  void onNumberFormatChanged(const QLocale *oldLocale) override;
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

  QSortFilterProxyModel m_analytesSortProxy;
  QSortFilterProxyModel m_concentrationsSortProxy;
  QSortFilterProxyModel m_mobilitiesSortProxy;

  FloatingValueDelegate m_floatingDelegate;

  ScrollAreaEventFilter *m_saEvFilter;

signals:
  void addAnalyte(const QString &name);
  void addConcentration(const double num);
  void addMobility(const double num);
  void analyteSwitched(const QModelIndexList &list);
  void chartHorizontalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker);
  void chartMarkerValueChanged(const HyperboleFittingEngineMsgs::MarkerType marker, const double d);
  void chartVerticalMarkerIntersection(const HyperboleFittingEngineMsgs::MarkerType marker);
  void concentrationSwitched(const QModelIndex &idx);
  void statsForAnalyteChanged(const HyperboleFittingEngineMsgs::AnalyteId aId);
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

private slots:
  void onAddAnalyteClicked();
  void onAddConcentrationClicked();
  void onAddMobilityClicked();
  void onAnalyteListClicked();
  void onAnalyteListDoubleClicked(const QModelIndex &idx);
  void onChartHorizontalMarkerLeftItscClicked();
  void onChartHorizontalMarkerRightItscClicked();
  void onChartHorizontalMarkerValueChanged(const QString &value);
  void onChartVerticalAMarkerItscClicked();
  void onChartVerticalBMarkerItscClicked();
  void onChartVerticalAMarkerValueChanged(const QString &value);
  void onChartVerticalBMarkerValueChanged(const QString &value);
  void onConcentrationsListClicked(const QModelIndex &idx);
  void onStatsForAnalyteChanged();
  void onEstimateClicked();
  void onFitClicked();
  void onFitModeActivated(const int idx);
  void onRedrawClicked();
  void onRemoveAnalyteClicked();
  void onRemoveConcentrationClicked();
  void onRemoveMobilityClicked();
  void onShowChartHorizontalMarker();
  void onShowChartVerticalAMarker();
  void onShowChartVerticalBMarker();
  void onStatBothClicked();
  void onStatLeftClicked();
  void onStatRightClicked();
  void onStatModeActivated(const int idx);
  void onStatUnitsActivated(const int idx);

public slots:
  void onEnableDoubleFit(const bool enable);
  void onChartHorizontalMarkerIntersectionSet(const double d);
  void onChartVerticalMarkerIntersectionSet(const HyperboleFittingEngineMsgs::MarkerType marker, const double d);

};

#endif // HYPERBOLEFITWIDGET_H
