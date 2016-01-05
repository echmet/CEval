#include "maincontrolswidget.h"
#include "ui_maincontrolswidget.h"
#include "../maincontrolswidgetconnector.h"

MainControlsWidget::MainControlsWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MainControlsWidget)
{
  m_evaluationWidget = new EvaluationWidget(this);
  m_hyperboleFittingWidget = new HyperboleFitWidget(this);

  ui->setupUi(this);
  ui->qtab_evaluation->layout()->addWidget(m_evaluationWidget);
  ui->qtab_hypfit->layout()->addWidget(m_hyperboleFittingWidget);

  connect(ui->qtabw_tabs, &QTabWidget::currentChanged, this, &MainControlsWidget::onTabSwitched);
}

MainControlsWidget::~MainControlsWidget()
{
  delete ui;
}

void MainControlsWidget::connectToAccumulator(QObject *dac)
{
  MainControlsWidgetConnector::connectAll(this, dac);
  m_evaluationWidget->connectToAccumulator(dac);
  m_hyperboleFittingWidget->connectToAccumulator(dac);
}

void MainControlsWidget::onTabSwitched(const int idx)
{
  emit tabSwitched(idx);
}

void MainControlsWidget::setDefaultState()
{
  m_evaluationWidget->setDefaultState();
  ui->qtabw_tabs->setCurrentIndex(0);
  emit tabSwitched(0);
}
