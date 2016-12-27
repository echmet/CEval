#ifndef HYPERBOLAFITWIDGET_H
#define HYPERBOLAFITWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QSortFilterProxyModel>
#include "../abstractmappermodel.h"
#include "../floatingvaluedelegate.h"
#include "../hyperbolafititems.h"
#include "../hyperbolafittingenginemsgs.h"
#include "../inumberformatchangeable.h"
#include "../scrollareaeventfilter.h"

namespace Ui {
class HyperbolaFitWidget;
}

class HyperbolaFitWidget : public QWidget, public INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(INumberFormatChangeable)
public:
  explicit HyperbolaFitWidget(QWidget *parent = nullptr);
  ~HyperbolaFitWidget();
  void connectToAccumulator(QObject *dac);
  void onNumberFormatChanged(const QLocale *oldLocale) override;
  void setAnalytesModel(QAbstractItemModel *model);
  void setAnalyteNamesModel(AbstractMapperModel<QString, HyperbolaFitParameters::String> *model);
  void setConcentrationsModel(QAbstractItemModel *model);
  void setFitFixedModel(AbstractMapperModel<bool, HyperbolaFitParameters::Boolean> *model);
  void setFitFloatModel(AbstractMapperModel<double, HyperbolaFitParameters::Floating> *model);
  void setFitIntModel(AbstractMapperModel<int, HyperbolaFitParameters::Int> *model);
  void setFitResultsModel(AbstractMapperModel<double, HyperbolaFitResults::Floating> *model);
  void setFitModeModel(QAbstractItemModel *model);
  void setMobilitiesModel(QAbstractItemModel *model);
  void setStatModeModel(QAbstractItemModel *model);
  void setStatUnitsModel(QAbstractItemModel *model);

private:
  Ui::HyperbolaFitWidget *ui;

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
  void addAnalyte(const QString &name, QModelIndex &idx);
  void addConcentration(const double num, QModelIndex &idx);
  void addMobility(const double num, QModelIndex &idx);
  void analyteSwitched(const QModelIndexList &list);
  void chartHorizontalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker);
  void chartMarkerValueChanged(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d);
  void chartVerticalMarkerIntersection(const HyperbolaFittingEngineMsgs::MarkerType marker);
  void concentrationSwitched(const QModelIndex &idx);
  void statsForAnalyteChanged(const HyperbolaFittingEngineMsgs::AnalyteId aId);
  void doEstimate();
  void doFit();
  void doStats(const HyperbolaStats::Intervals intr);
  void fitModeChanged(const QVariant &v);
  void redrawDataSeries();
  void removeAnalyte(const QModelIndex &idx);
  void removeConcentration(const QModelIndex &idx);
  void removeMobility(const QModelIndex &idx);
  void renameAnalyte(const QVariant &internalId, const QString &newName, const int idx);
  void showChartMarker(const HyperbolaFittingEngineMsgs::MarkerType marker, const bool visible, const QString &value);
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
  void onChartVerticalMarkerIntersectionSet(const HyperbolaFittingEngineMsgs::MarkerType marker, const double d);
  void onSortLists();

};

#endif // HYPERBOLAFITWIDGET_H
