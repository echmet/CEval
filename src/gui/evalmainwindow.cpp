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
#include "efgloadinfo.h"
#include "selectediipath.h"

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
  ui->actionLoad_data_table->setIcon(QIcon::fromTheme("document-open"));
  ui->actionSave_data_table->setIcon(QIcon::fromTheme("document-save"));
  ui->actionExit->setIcon(QIcon::fromTheme("application-exit"));
  ui->actionAdjust_plot_appearance->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionSet_number_format->setIcon(QIcon::fromTheme("preferences-system"));
  ui->actionCheck_for_update->setIcon(QIcon::fromTheme("system-software-update"));
  ui->actionAbout->setIcon(QIcon::fromTheme("help-about"));
  ui->actionSet_path_to_EDII_service->setIcon(QIcon::fromTheme("preferences-system"));
#else
  ui->actionLoad_data_table->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
  ui->actionSave_data_table->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
  ui->actionExit->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
  ui->actionAdjust_plot_appearance->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  ui->actionSet_number_format->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
  ui->actionCheck_for_update->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
  ui->actionAbout->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));
  ui->actionSet_path_to_EDII_service->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
#endif // Q_OS

  connect(ui->actionAbout, &QAction::triggered, this, &EvalMainWindow::onActionAbout);
  connect(ui->actionAdjust_plot_appearance, &QAction::triggered, this, &EvalMainWindow::onActionAdjustPlotAppearance);
  connect(ui->actionCheck_for_update, &QAction::triggered, this, &EvalMainWindow::onActionCheckForUpdate);
  connect(ui->actionExit, &QAction::triggered, this, &EvalMainWindow::onActionExit);
  connect(ui->actionExport_plot_as_image, &QAction::triggered, this, &EvalMainWindow::onActionExportPlotAsImage);
  connect(ui->actionLoad_data_table, &QAction::triggered, this, &EvalMainWindow::onActionLoadDataTable);
  connect(ui->actionSave_data_table, &QAction::triggered, this, &EvalMainWindow::onActionSaveDataTable);
  connect(ui->actionSet_number_format, &QAction::triggered, this, &EvalMainWindow::onActionSetNumberFormat);
  connect(ui->actionSet_path_to_EDII_service, &QAction::triggered, this, &EvalMainWindow::onActionSetPathToEDII);

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

void EvalMainWindow::onActionLoadDataTable()
{
  emit loadDataTable();
}

void EvalMainWindow::onActionLoadElectrophoregram()
{
  QAction *a = qobject_cast<QAction *>(sender());

  if (a == nullptr)
    return;

  QVariant info = a->data();
  if (!info.canConvert<EFGLoadInfo>())
    return;

  EFGLoadInfo infoVal = info.value<EFGLoadInfo>();

  emit loadElectrophoregram(infoVal.formatTag, infoVal.loadOption);
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

void EvalMainWindow::onActionSetPathToEDII()
{
  SelectEDIIPath dlg;

  dlg.exec();
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
    ui->actionLoad_data_table->setVisible(false);
    ui->actionSave_data_table->setVisible(false);
    for (QAction *a : m_loadEFGActions)
      a->setVisible(true);
    break;
  case DataAccumulatorMsgs::ProgramMode::HYPERBOLA_FIT:
    m_exportEvaluationMenu->menuAction()->setVisible(false);
    m_exportHyperbolaFitMenu->menuAction()->setVisible(true);
    ui->actionLoad_data_table->setVisible(true);
    ui->actionSave_data_table->setVisible(true);
    for (QAction *a : m_loadEFGActions)
      a->setVisible(false);
    break;
  }
}

void EvalMainWindow::onSupportedFileFormatsRetrieved(EFGSupportedFileFormatVec supportedFormats)
{
  auto itemTitle = [](const QString &s) {
    return QString("Load %1 file").arg(s);
  };
  auto addAction = [this](const EFGLoadInfo &info, const QString &title, QMenu *m, QAction *before) {
    QAction *a = new QAction(title, this);
    a->setData(QVariant::fromValue<EFGLoadInfo>(info));
    connect(a, &QAction::triggered, this, &EvalMainWindow::onActionLoadElectrophoregram);

    if (before != nullptr)
      m->insertAction(ui->actionLoad_data_table, a);
    else
      m->addAction(a);

    m_loadEFGActions.push_back(a);
    #ifdef Q_OS_LINUX
    a->setIcon(QIcon::fromTheme("document-open"));
    #else
    a->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    #endif // Q_OS_LINUX
  };

  for (const EFGSupportedFileFormat &sff : supportedFormats) {
    if (sff.loadOptions.size() > 1) {
      QMenu *m = new QMenu(itemTitle(sff.shortDescription), this);

      for (auto it = sff.loadOptions.cbegin(); it != sff.loadOptions.cend(); it++)
        addAction(EFGLoadInfo{sff.formatTag, it.key()}, QString("From %1").arg(it.value()), m, nullptr);

      ui->menuFile->insertMenu(ui->actionLoad_data_table, m);
      m_loadEFGActions.push_back(m->menuAction());
    } else
      addAction(EFGLoadInfo{sff.formatTag, 0}, itemTitle(sff.shortDescription), ui->menuFile, ui->actionLoad_data_table);
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
