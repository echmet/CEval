#include "evaluatedpeakswidget.h"
#include "ui_evaluatedpeakswidget.h"
#include <functional>
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
  connect(ui->qpb_registerInHF, &QPushButton::clicked, this, &EvaluatedPeaksWidget::onRegisterInHFClicked);
  connect(ui->qpb_rename, &QPushButton::clicked, this, &EvaluatedPeaksWidget::onRenameClicked);

  {
    QWidget *me = this;
    QTableView *tbv = ui->qtbv_evaluatedPeaks;
    std::function<void (QObject *, QEvent *)> todo = [me, tbv](QObject *object, QEvent *ev) {
      Q_UNUSED(object);

      if (ev->type() == QEvent::Resize) {
        int width;
        int idx = 0;
        int total = 0;

        while ((width = tbv->horizontalHeader()->sectionSize(idx++)) > 0)
          total += width;

        me->setFixedWidth(total + tbv->horizontalHeader()->sizeHint().width() / 2);
      }
    };

    m_scEvFilter = new ScrollAreaEventFilter(todo, ui->qtbv_evaluatedPeaks, this);
  }

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

void EvaluatedPeaksWidget::onRegisterInHFClicked()
{
  emit registerInHyperboleFit(ui->qtbv_evaluatedPeaks->currentIndex());
}

void EvaluatedPeaksWidget::onRenameClicked()
{
  emit rename(ui->qtbv_evaluatedPeaks->currentIndex());
}

void EvaluatedPeaksWidget::setModel(QAbstractItemModel *model)
{
  ui->qtbv_evaluatedPeaks->setModel(model);
}
