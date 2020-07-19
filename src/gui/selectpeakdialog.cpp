#include "selectpeakdialog.h"
#include "ui_selectpeakdialog.h"

SelectPeakDialog::SelectPeakDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SelectPeakDialog)
{
  ui->setupUi(this);

  m_selectionMode = SelectionMode::INVALID;
  m_selectedPeakNumbers = QVector<int>();

  connect(ui->qpb_select, &QPushButton::clicked, this, &SelectPeakDialog::onSelectClicked);
  connect(ui->qpb_selectAll, &QPushButton::clicked, this, &SelectPeakDialog::onSelectAllClicked);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, [this]() { reject(); });
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

void SelectPeakDialog::onListClicked(const QModelIndex &index)
{
  if (!index.isValid())
    return;

  const QAbstractItemModel *model = ui->qtbv_listOfPeaks->model();
  const QList<QModelIndex> &selectedIndices = ui->qtbv_listOfPeaks->selectionModel()->selectedIndexes();

  if (selectedIndices.empty())
    emit allPeaksUnselected();

  for (const QModelIndex &idx : selectedIndices) {
    if (idx.row() == index.row())
      emit peakSelected(index, model, m_peakWindow);
  }
}

void SelectPeakDialog::onListDoubleClicked(const QModelIndex &index)
{
  if (index.isValid()) {
    m_selectionMode = SelectionMode::MULTIPLE_PEAK;
    m_selectedPeakNumbers = { index.row() };
    accept();
  }
}

void SelectPeakDialog::onSelectClicked()
{
  QModelIndexList list = ui->qtbv_listOfPeaks->selectionModel()->selectedIndexes();
  QVector<int> selected;

  for (const QModelIndex &midx : list)
    selected.push_back(midx.row());

  m_selectedPeakNumbers = selected;
  m_selectionMode = SelectionMode::MULTIPLE_PEAK;
  accept();
}

void SelectPeakDialog::onSelectAllClicked()
{
  m_selectionMode = SelectionMode::ALL_PEAKS;
  accept();
}

SelectPeakDialog::SelectionMode SelectPeakDialog::selectionMode() const
{
  return m_selectionMode;
}

QVector<int> SelectPeakDialog::selectedPeaks() const
{
  return m_selectedPeakNumbers;
}

void SelectPeakDialog::setPeakWindow(const int peakWindow)
{
  m_peakWindow = peakWindow;
}
