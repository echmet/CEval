#include "evaluationupperwidget.h"
#include "ui_evaluationupperwidget.h"
#include "../evaluationupperwidgetconnector.h"
#include <QMessageBox>

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

  m_splitter->setStretchFactor(0, 0);
  m_splitter->setStretchFactor(1, 0);
  m_splitter->setStretchFactor(2, 1);

  connect(ui->qcbox_files, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &EvaluationUpperWidget::onFileComboBoxChanged);
  connect(ui->qpb_closeAllFiles, &QPushButton::clicked, this, &EvaluationUpperWidget::onCloseAllFilesClicked);
  connect(ui->qpb_closeFile, &QPushButton::clicked, this, &EvaluationUpperWidget::onCloseFileClicked);
  connect(ui->qpb_nextFile, &QPushButton::clicked, this, &EvaluationUpperWidget::onNextFileClicked);
  connect(ui->qpb_prevFile, &QPushButton::clicked, this, &EvaluationUpperWidget::onPreviousFileClicked);
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

void EvaluationUpperWidget::onCloseAllFilesClicked()
{
  if (QMessageBox::question(this, tr("Close all files?"), tr("Are you sure you want to close all files?"))!= QMessageBox::Yes)
    return;

  emit closeAllFiles();
}

void EvaluationUpperWidget::onCloseFileClicked()
{
  emit closeFile(ui->qcbox_files->currentIndex());

  const int newIdx = ui->qcbox_files->currentIndex();

  if (newIdx >= 0)
      emit fileSwitched(newIdx);
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

void EvaluationUpperWidget::onNextFileClicked()
{
  emit traverseFiles(EvaluationEngineMsgs::Traverse::NEXT);
}

void EvaluationUpperWidget::onPreviousFileClicked()
{
  emit traverseFiles(EvaluationEngineMsgs::Traverse::PREVIOUS);
}

void EvaluationUpperWidget::setCommonParametersBoolModel(AbstractMapperModel<bool, CommonParametersItems::Boolean> *model)
{
  m_commonParametersWidget->setCommonParametersBoolModel(model);
}

void EvaluationUpperWidget::setCommonParametersNumModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model)
{
  m_commonParametersWidget->setCommonParametersNumModel(model);
}

void EvaluationUpperWidget::setDefaultState()
{
  m_evaluationWidget->setDefaultState();
}

void EvaluationUpperWidget::setEvaluationLoadedFilesModel(QAbstractItemModel *model)
{
  ui->qcbox_files->setModel(model);
}

