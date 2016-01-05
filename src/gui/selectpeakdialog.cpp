#include "selectpeakdialog.h"
#include "ui_selectpeakdialog.h"

SelectPeakDialog::SelectPeakDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SelectPeakDialog)
{
  ui->setupUi(this);

  m_selectedPeakNumber = -1;

  connect(ui->qpb_select, &QPushButton::clicked, this, &SelectPeakDialog::onSelectClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &SelectPeakDialog::onCancelClicked);
  connect(ui->qtbv_listOfPeaks, &QTableView::clicked, this, &SelectPeakDialog::onListClicked);
  connect(ui->qtbv_listOfPeaks, &QTableView::doubleClicked, this, &SelectPeakDialog::onListDoubleClicked);
}

SelectPeakDialog::~SelectPeakDialog()
{
  delete ui;
}

void SelectPeakDialog::bindModel(QAbstractItemModel *model)
{
  ui->qtbv_listOfPeaks->setModel(model);
}

void SelectPeakDialog::closeEvent(QCloseEvent *ev)
{
  Q_UNUSED(ev);

  emit closedSignal();
}

void SelectPeakDialog::onCancelClicked()
{
  reject();
}

void SelectPeakDialog::onListClicked(const QModelIndex &index)
{
  if (!index.isValid()) {
    m_selectedPeakNumber = -1;
    return;
  }

  m_selectedPeakNumber = index.row();

  emit listClicked(index, ui->qtbv_listOfPeaks->model(), m_peakWindow);
}

void SelectPeakDialog::onListDoubleClicked(const QModelIndex &index)
{
  if (index.isValid()) {
    m_selectedPeakNumber = index.row();
    accept();
  }
}

void SelectPeakDialog::onSelectClicked()
{
  if (m_selectedPeakNumber >= 0 && m_selectedPeakNumber < ui->qtbv_listOfPeaks->model()->rowCount())
    accept();
}

int SelectPeakDialog::selectedPeak() const
{
  return m_selectedPeakNumber;
}

void SelectPeakDialog::setPeakWindow(const long peakWindow)
{
  m_peakWindow = peakWindow;
}
