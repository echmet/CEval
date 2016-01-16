#include "evalmainwindow.h"
#include "ui_evalmainwindow.h"
#include "aboutdialog.h"
#include "../evaluationparametersitems.h"
#include "../evalmainwindowconnector.h"
#include "../helpers.h"
#include "../globals.h"
#include "numberformatdialog.h"
#include "../doubletostringconvertor.h"
#include "common/witchcraft.h"
#include <QMessageBox>

EvalMainWindow::EvalMainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::EvalMainWindow)
{
  m_plot = new QwtPlot(this);
  m_upmostSplitter = new QSplitter(Qt::Vertical, this);
  m_controlsSplitter = new QSplitter(Qt::Horizontal, m_upmostSplitter);
  m_mainControlsWidget = new MainControlsWidget(m_controlsSplitter);

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

  setWindowTitle(Globals::VERSION_STRING());

  ui->actionLoad_ChemStation_file->setIcon(QIcon::fromTheme("document-open"));
  ui->actionLoad_comma_separated_file->setIcon(QIcon::fromTheme("document-open"));
  ui->actionLoad_data_table->setIcon(QIcon::fromTheme("document-open"));
  ui->actionSave_data_table->setIcon(QIcon::fromTheme("document-save"));
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAdjust_plot_appearance->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionSet_number_format->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));

  connect(ui->actionAbout, &QAction::triggered, this, &EvalMainWindow::onActionAbout);
  connect(ui->actionAdjust_plot_appearance, &QAction::triggered, this, &EvalMainWindow::onActionAdjustPlotAppearance);
  connect(ui->actionExit, &QAction::triggered, this, &EvalMainWindow::onActionExit);
  connect(ui->actionLoad_ChemStation_file, &QAction::triggered, this, &EvalMainWindow::onActionLoadChemStationFile);
  connect(ui->actionLoad_comma_separated_file, &QAction::triggered, this, &EvalMainWindow::onActionLoadCsvFile);
  connect(ui->actionLoad_data_table, &QAction::triggered, this, &EvalMainWindow::onActionLoadDataTable);
  connect(ui->actionSave_data_table, &QAction::triggered, this, &EvalMainWindow::onActionSaveDataTable);
  connect(ui->actionSet_number_format, &QAction::triggered, this, &EvalMainWindow::onActionSetNumberFormat);
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

void EvalMainWindow::onActionAbout()
{
  AboutDialog dlg;

  dlg.exec();
}

void EvalMainWindow::onActionAdjustPlotAppearance()
{
  emit adjustPlotAppearance();
}

void EvalMainWindow::onActionExit()
{
  Helpers::exitApplicationWithWarning();
}

void EvalMainWindow::onActionLoadChemStationFile()
{
  emit loadDataFile(DataFileLoaderMsgs::LoadableFileTypes::CHEMSTATION);
}

void EvalMainWindow::onActionLoadCsvFile()
{
  emit loadDataFile(DataFileLoaderMsgs::LoadableFileTypes::COMMA_SEPARATED);
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
  NumberFormatDialog dlg;

  dlg.setParameters(DoubleToStringConvertor::type(), DoubleToStringConvertor::digits());
  dlg.exec();
}

QwtPlot *EvalMainWindow::plot()
{
  return m_plot;
}

void EvalMainWindow::setDefaultState()
{
  m_mainControlsWidget->setDefaultState();
}
