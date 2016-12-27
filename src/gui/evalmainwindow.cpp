#include "evalmainwindow.h"
#include "ui_evalmainwindow.h"
#include "aboutdialog.h"
#include "checkforupdatedialog.h"
#include "../evaluationparametersitems.h"
#include "../evalmainwindowconnector.h"
#include "../helpers.h"
#include "../globals.h"
#include "numberformatdialog.h"
#include "../doubletostringconvertor.h"
#include "../witchcraft.h"
#include <QMessageBox>

EvalMainWindow::EvalMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::EvalMainWindow)
{
  m_plot = new QwtPlot(this);
  m_upmostSplitter = new QSplitter(Qt::Vertical, this);
  m_controlsSplitter = new QSplitter(Qt::Horizontal, m_upmostSplitter);
  m_mainControlsWidget = new MainControlsWidget(m_controlsSplitter);
  m_numberFormatDialog = new NumberFormatDialog(this);
  m_updateDlg = new CheckForUpdateDialog(this);

  ui->setupUi(this);

  m_controlsSplitter->addWidget(m_mainControlsWidget);
  m_upmostSplitter->addWidget(m_plot);
  m_upmostSplitter->addWidget(m_controlsSplitter);

  ui->centralWidget->layout()->addWidget(m_upmostSplitter);
  Witchcraft::makeSplitterAppear(m_upmostSplitter);
  m_plot->setCanvasBackground(QBrush(Qt::white));

  m_plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_plot->setMinimumHeight(0);
  m_upmostSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  /* Prepare export menus */
  makeExportMenus();
  ui->menuExport->addMenu(m_exportEvaluationMenu);
  ui->menuExport->insertMenu(m_exportEvaluationMenu->menuAction(), m_exportHyperbolaFitMenu);
  m_exportHyperbolaFitMenu->menuAction()->setVisible(false);
  m_exportEvaluationMenu->menuAction()->setVisible(false);

  setWindowTitle(Globals::VERSION_STRING());

#ifdef Q_OS_LINUX
  ui->actionLoad_ChemStation_file->setIcon(QIcon::fromTheme("document-open"));
  ui->menuLoad_comma_separated_file->setIcon(QIcon::fromTheme("document-open"));
  ui->actionLoad_data_table->setIcon(QIcon::fromTheme("document-open"));
  ui->actionSave_data_table->setIcon(QIcon::fromTheme("document-save"));
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAdjust_plot_appearance->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionSet_number_format->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionCheck_for_update->setIcon(QIcon::fromTheme("system-software-update"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
#else
  ui->actionLoad_ChemStation_file->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->menuLoad_comma_separated_file->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->actionLoad_data_table->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->actionSave_data_table->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  ui->actionAdjust_plot_appearance->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  ui->actionSet_number_format->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  ui->actionCheck_for_update->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));
#endif // Q_OS

  connect(ui->actionAbout, &QAction::triggered, this, &EvalMainWindow::onActionAbout);
  connect(ui->actionAdjust_plot_appearance, &QAction::triggered, this, &EvalMainWindow::onActionAdjustPlotAppearance);
  connect(ui->actionCheck_for_update, &QAction::triggered, this, &EvalMainWindow::onActionCheckForUpdate);
  connect(ui->actionExit, &QAction::triggered, this, &EvalMainWindow::onActionExit);
  connect(ui->actionExport_plot_as_image, &QAction::triggered, this, &EvalMainWindow::onActionExportPlotAsImage);
  connect(ui->actionLoad_ChemStation_file, &QAction::triggered, this, &EvalMainWindow::onActionLoadChemStationFile);
  connect(ui->actionCsv_from_clipboard, &QAction::triggered, this, &EvalMainWindow::onActionLoadCsvClipboard);
  connect(ui->actionCsv_from_file, &QAction::triggered, this, &EvalMainWindow::onActionLoadCsvFile);
  connect(ui->actionLoad_data_table, &QAction::triggered, this, &EvalMainWindow::onActionLoadDataTable);
  connect(ui->actionSave_data_table, &QAction::triggered, this, &EvalMainWindow::onActionSaveDataTable);
  connect(ui->actionSet_number_format, &QAction::triggered, this, &EvalMainWindow::onActionSetNumberFormat);

  this->setMinimumHeight(600);
}

EvalMainWindow::~EvalMainWindow()
{
  delete ui;
}

void EvalMainWindow::closeEvent(QCloseEvent *ev)
{
  if (!Helpers::exitApplicationWithWarning())
    ev->ignore();
}

void EvalMainWindow::connectToAccumulator(QObject *dac)
{
  EvalMainWindowConnector::connectAll(this, dac);
  m_mainControlsWidget->connectToAccumulator(dac);
}

void EvalMainWindow::makeExportMenus()
{
  m_exportHyperbolaFitMenu = new QMenu(QObject::tr("Hyperbola fit export"), this);
  if (m_exportHyperbolaFitMenu == nullptr)
    return;

  QAction *a = new QAction("Export data table", m_exportHyperbolaFitMenu);
  connect(a, &QAction::triggered, this, &EvalMainWindow::onActionExportDataTableCsv);
  m_exportHyperbolaFitMenu->addAction(a);

  m_exportEvaluationMenu = new QMenu(QObject::tr("Export evaluation"), this);
  if (m_exportEvaluationMenu == nullptr)
    return;

  a = new QAction("Whole peak to clipboard", this);
  connect(a, &QAction::triggered, this, &EvalMainWindow::onActionWholePeakToClipboard);
  m_exportEvaluationMenu->addAction(a);
}

void EvalMainWindow::onActionAbout()
{
  AboutDialog dlg;

  dlg.exec();
}

void EvalMainWindow::onActionAdjustPlotAppearance()
{
  emit adjustPlotAppearance();
}

void EvalMainWindow::onActionCheckForUpdate()
{
  m_updateDlg->exec();
}

void EvalMainWindow::onActionExit()
{
  Helpers::exitApplicationWithWarning();
}

void EvalMainWindow::onActionExportDataTableCsv()
{
  emit exportAction(DataAccumulatorMsgs::ExportAction::EXPORT_DATATABLE_CSV);
}

void EvalMainWindow::onActionExportPlotAsImage()
{
  emit exportAction(DataAccumulatorMsgs::ExportAction::EXPORT_PLOT);
}

void EvalMainWindow::onActionLoadChemStationFile()
{
  emit loadDataFile(DataFileLoaderMsgs::LoadableFileTypes::CHEMSTATION);
}

void EvalMainWindow::onActionLoadCsvClipboard()
{
  emit loadDataFile(DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_CLIPBOARD);
}

void EvalMainWindow::onActionLoadCsvFile()
{
  emit loadDataFile(DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED_FILE);
}

void EvalMainWindow::onActionLoadDataTable()
{
  emit loadDataTable();
}

void EvalMainWindow::onActionSaveDataTable()
{
  emit saveDataTable();
}

void EvalMainWindow::onActionSetNumberFormat()
{
  m_numberFormatDialog->setParameters(DoubleToStringConvertor::type(), DoubleToStringConvertor::digits());
  m_numberFormatDialog->exec();
}

void EvalMainWindow::onActionWholePeakToClipboard()
{
  emit exportAction(DataAccumulatorMsgs::ExportAction::WHOLE_PEAK_TO_CLIPBOARD);
}

void EvalMainWindow::onProgramModeChanged(const DataAccumulatorMsgs::ProgramMode mode)
{
  switch (mode) {
  case DataAccumulatorMsgs::ProgramMode::EVALUATION:
    m_exportHyperbolaFitMenu->menuAction()->setVisible(false);
    m_exportEvaluationMenu->menuAction()->setVisible(true);
    break;
  case DataAccumulatorMsgs::ProgramMode::HYPERBOLA_FIT:
    m_exportEvaluationMenu->menuAction()->setVisible(false);
    m_exportHyperbolaFitMenu->menuAction()->setVisible(true);
    break;
  }
}

QwtPlot *EvalMainWindow::plot()
{
  return m_plot;
}

void EvalMainWindow::setDefaultState()
{
  m_mainControlsWidget->setDefaultState();
}
