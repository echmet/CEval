#ifndef EVALMAINWINDOW_H
#define EVALMAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDataWidgetMapper>
#include <QSplitter>
#include "../dataaccumulatormsgs.h"
#include "maincontrolswidget.h"
#include "qwt_plot.h"

class CheckForUpdateDialog;
class NumberFormatDialog;
class SoftwareUpdater;

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
  void connectToSoftwareUpdater(SoftwareUpdater *updater);
  QwtPlot *plot();
  void setDefaultState();

private:
  Ui::EvalMainWindow *ui;
  QwtPlot *m_plot;
  QSplitter *m_upmostSplitter;
  QSplitter *m_controlsSplitter;
  QVector<QAction *> m_loadEFGActions;
  MainControlsWidget *m_mainControlsWidget;
  NumberFormatDialog *m_numberFormatDialog;

  CheckForUpdateDialog *m_updateDlg;
  QMenu *m_exportEvaluationMenu;
  QMenu *m_exportHyperbolaFitMenu;

  void closeEvent(QCloseEvent *ev) override;
  void makeExportMenus();
  void makeSupportedFileFormatsActions();

signals:
  void adjustPlotAppearance();
  void exportAction(const DataAccumulatorMsgs::ExportAction action);
  void loadElectrophoregram(const QString &formatTag);
  void loadDataTable();
  void saveDataTable();
  void testExporter();

public slots:
  void onProgramModeChanged(const DataAccumulatorMsgs::ProgramMode mode);

private slots:
  void onActionAbout();
  void onActionAdjustPlotAppearance();
  void onActionCheckForUpdate();
  void onActionExit();
  void onActionExportDataTableCsv();
  void onActionExportPlotAsImage();
  void onActionLoadDataTable();
  void onActionLoadElectrophoregram();
  void onActionSaveDataTable();
  void onActionSetNumberFormat();
  void onActionWholePeakToClipboard();

};

#endif // EVALMAINWINDOW_H
