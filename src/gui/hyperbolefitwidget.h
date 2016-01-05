#ifndef HYPERBOLEFITWIDGET_H
#define HYPERBOLEFITWIDGET_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include "../abstractmappermodel.h"
#include "../floatingvaluedelegate.h"
#include "../hyperbolefititems.h"

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

  FloatingValueDelegate m_floatingDelegate;

signals:
  void addAnalyte(const QString &name);
  void addConcentration(const double num);
  void addMobility(const double num);
  void analyteSwitched(const QModelIndexList &list);
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
  void statModeChanged(const QVariant &v);
  void statUnitsChanged(const QVariant &v);

private slots:
  void onAddAnalyteClicked();
  void onAddConcentrationClicked();
  void onAddMobilityClicked();
  void onAnalyteListClicked();
  void onAnalyteListDoubleClicked(const QModelIndex &idx);
  void onConcentrationsListClicked(const QModelIndex &idx);
  void onEstimateClicked();
  void onFitClicked();
  void onFitModeActivated(const int idx);
  void onRedrawClicked();
  void onRemoveAnalyteClicked();
  void onRemoveConcentrationClicked();
  void onRemoveMobilityClicked();
  void onStatBothClicked();
  void onStatLeftClicked();
  void onStatRightClicked();
  void onStatModeActivated(const int idx);
  void onStatUnitsActivated(const int idx);

public slots:
  void onEnableDoubleFit(const bool enable);

};

#endif // HYPERBOLEFITWIDGET_H
