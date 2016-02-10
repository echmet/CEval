#include "evaluationupperwidget.h"
#include "ui_evaluationupperwidget.h"
#include "../evaluationupperwidgetconnector.h"

EvaluationUpperWidget::EvaluationUpperWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::EvaluationUpperWidget)
{
  ui->setupUi(this);

  m_splitter = new QSplitter(Qt::Horizontal, this);
  m_commonParametersWidget = new CommonParametersWidget(this);
  m_evaluatedPeaksWidget = new EvaluatedPeaksWidget(this);
  m_evaluationWidget = new EvaluationWidget(this);

  ui->qw_container->setLayout(new QVBoxLayout);
  ui->qw_container->layout()->addWidget(m_splitter);

  m_splitter->addWidget(m_commonParametersWidget);
  m_splitter->addWidget(m_evaluatedPeaksWidget);
  m_splitter->addWidget(m_evaluationWidget);

  connect(ui->qcbox_files, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationUpperWidget::onFileComboBoxChanged);
  connect(ui->qpb_closeFile, &QPushButton::clicked, this, &EvaluationUpperWidget::onCloseFileClicked);
}

EvaluationUpperWidget::~EvaluationUpperWidget()
{
    delete ui;
}

void EvaluationUpperWidget::connectToAccumulator(QObject *dac)
{
  EvaluationUpperWidgetConnector::connectAll(this, dac);
  m_commonParametersWidget->connectToAccumulator(dac);
  m_evaluatedPeaksWidget->connectToAccumulator(dac);
  m_evaluationWidget->connectToAccumulator(dac);
}

void EvaluationUpperWidget::onCloseFileClicked()
{
  emit closeFile(ui->qcbox_files->currentIndex());
}

void EvaluationUpperWidget::onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier)
{
  switch (notifier.id) {
  case EvaluationEngineMsgs::ComboBox::DATA_FILES:
    ui->qcbox_files->setCurrentIndex(notifier.value);
    break;
  default:
    break;
  }
}

void EvaluationUpperWidget::onFileComboBoxChanged(const int idx)
{
  emit fileSwitched(idx);
}

void EvaluationUpperWidget::onFileSwitched(const int idx)
{
  ui->qcbox_files->setCurrentIndex(idx);
}

void EvaluationUpperWidget::setCommonParametersModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model)
{
  m_commonParametersWidget->setCommonParametersModel(model);
}

void EvaluationUpperWidget::setDefaultState()
{
  m_evaluationWidget->setDefaultState();
}

void EvaluationUpperWidget::setEvaluationLoadedFilesModel(QAbstractItemModel *model)
{
  ui->qcbox_files->setModel(model);
}

