#ifndef MAINCONTROLSWIDGET_H
#define MAINCONTROLSWIDGET_H

#include <QWidget>
#include "evaluationupperwidget.h"
#include "hyperbolefitwidget.h"

namespace Ui {
class MainControlsWidget;
}

class MainControlsWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MainControlsWidget(QWidget *parent = nullptr);
  ~MainControlsWidget();
  void connectToAccumulator(QObject *dac);
  void setDefaultState();

private:
  Ui::MainControlsWidget *ui;
  EvaluationUpperWidget *m_evaluationUpperWidget;
  HyperboleFitWidget *m_hyperboleFittingWidget;

signals:
  void tabSwitched(const int idx);

private slots:
  void onTabSwitched(const int idx);

};

#endif // MAINCONTROLSWIDGET_H
