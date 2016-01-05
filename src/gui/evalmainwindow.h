#ifndef EVALMAINWINDOW_H
#define EVALMAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDataWidgetMapper>
#include <QSplitter>
#include "../datafileloadermsgs.h"
#include "../dataaccumulatormsgs.h"
#include "maincontrolswidget.h"
#include "qwt_plot.h"

namespace Ui {
class EvalMainWindow;
}

class EvalMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit EvalMainWindow(QWidget *parent = nullptr);
  ~EvalMainWindow();
  void connectToAccumulator(QObject *dac);
  QwtPlot *plot();
  void setDefaultState();

private:
  Ui::EvalMainWindow *ui;
  QwtPlot *m_plot;
  QSplitter *m_upmostSplitter;
  QSplitter *m_controlsSplitter;
  MainControlsWidget *m_mainControlsWidget;

  void closeEvent(QCloseEvent *ev) override;

signals:
  void adjustPlotAppearance();
  void loadDataFile(const DataFileLoaderMsgs::LoadableFileTypes type);
  void loadDataTable();
  void saveDataTable();

public slots:

private slots:
  void onActionAbout();
  void onActionAdjustPlotAppearance();
  void onActionExit();
  void onActionLoadChemStationFile();
  void onActionLoadCsvFile();
  void onActionLoadDataTable();
  void onActionSaveDataTable();
  void onActionSetNumberFormat();

};

#endif // EVALMAINWINDOW_H
