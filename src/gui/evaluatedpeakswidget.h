#ifndef EVALUATEDPEAKSWIDGET_H
#define EVALUATEDPEAKSWIDGET_H

#include <QAbstractItemModel>
#include <QWidget>
#include "../scrollareaeventfilter.h"

namespace Ui {
class EvaluatedPeaksWidget;
}

class EvaluatedPeaksWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EvaluatedPeaksWidget(QWidget *parent = nullptr);
  ~EvaluatedPeaksWidget();
  void connectToAccumulator(QObject *dac);
  void setModel(QAbstractItemModel *model);

private:
  Ui::EvaluatedPeaksWidget *ui;
  ScrollAreaEventFilter *m_scEvFilter;

signals:
  void addPeakClicked();
  void cancelSelection();
  void deletePeakClicked(const QModelIndex &idx);
  void peakSwitched(const QModelIndex &idx);
  void registerInHyperboleFit(const QModelIndex &idx);
  void rename(const QModelIndex &idx);

private slots:
  void onAddPeakClicked();
  void onCancelSelectionClicked();
  void onDeletePeakClicked();
  void onListClicked();
  void onRegisterInHFClicked();
  void onRenameClicked();

};

#endif // EVALUATEDPEAKSWIDGET_H
