#include "evaluatedpeakswidget.h"
#include "ui_evaluatedpeakswidget.h"
#include "../evaluatedpeakswidgetconnector.h"

EvaluatedPeaksWidget::EvaluatedPeaksWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::EvaluatedPeaksWidget)
{
  ui->setupUi(this);

  connect(ui->qpb_addPeak, &QPushButton::clicked, this, &EvaluatedPeaksWidget::onAddPeakClicked);
  connect(ui->qpb_cancelSelection, &QPushButton::clicked, this, &EvaluatedPeaksWidget::onCancelSelectionClicked);
  connect(ui->qpb_deletePeak, &QPushButton::clicked, this, &EvaluatedPeaksWidget::onDeletePeakClicked);
  connect(ui->qtbv_evaluatedPeaks, &QTableView::clicked, this, &EvaluatedPeaksWidget::onListClicked);
}

EvaluatedPeaksWidget::~EvaluatedPeaksWidget()
{
  delete ui;
}

void EvaluatedPeaksWidget::connectToAccumulator(QObject *dac)
{
  EvaluatedPeaksWidgetConnector::connectAll(this, dac);
}

void EvaluatedPeaksWidget::onAddPeakClicked()
{
  emit addPeakClicked();
}

void EvaluatedPeaksWidget::onCancelSelectionClicked()
{
  emit cancelSelection();
}

void EvaluatedPeaksWidget::onDeletePeakClicked()
{
  emit deletePeakClicked(ui->qtbv_evaluatedPeaks->currentIndex());
}

void EvaluatedPeaksWidget::onListClicked()
{
  emit peakSwitched(ui->qtbv_evaluatedPeaks->currentIndex());
}

void EvaluatedPeaksWidget::setModel(QAbstractItemModel *model)
{
  ui->qtbv_evaluatedPeaks->setModel(model);
}
