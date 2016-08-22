#include "schemesmanagerwidget.h"
#include "ui_schemesmanagerwidget.h"
#include <QAbstractItemModel>

using namespace DataExporter;

SchemesManagerWidget::SchemesManagerWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SchemesManagerWidget)
{
  ui->setupUi(this);

  ui->qlv_schemes->setEditTriggers(QListView::NoEditTriggers);

  connect(ui->qpb_createScheme, &QPushButton::clicked, this, &SchemesManagerWidget::onCreateSchemeClicked);
  connect(ui->qpb_editScheme, &QPushButton::clicked, this, &SchemesManagerWidget::onEditSchemeClicked);
  connect(ui->qpb_loadScheme, &QPushButton::clicked, this, &SchemesManagerWidget::onLoadSchemeClicked);
  connect(ui->qpb_removeScheme, &QPushButton::clicked, this, &SchemesManagerWidget::onRemoveSchemeClicked);
  connect(ui->qpb_saveScheme, &QPushButton::clicked, this, &SchemesManagerWidget::onSaveSchemeClicked);
  connect(ui->qpb_close, &QPushButton::clicked, this, &SchemesManagerWidget::onCloseClicked);
  connect(ui->qlv_schemes, &QListView::doubleClicked, this, &SchemesManagerWidget::onEditSchemeClicked);
}

SchemesManagerWidget::~SchemesManagerWidget()
{
  delete ui;
}

void SchemesManagerWidget::onCloseClicked()
{
  emit closed();
}

void SchemesManagerWidget::onCreateSchemeClicked()
{
  emit createScheme();
}

void SchemesManagerWidget::onEditSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();
  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit editScheme(name);
}

void SchemesManagerWidget::onLoadSchemeClicked()
{
  emit loadScheme();
}

void SchemesManagerWidget::onRemoveSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();

  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit removeScheme(name);
}

void SchemesManagerWidget::onSaveSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();

  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit saveScheme(name);
}

void SchemesManagerWidget::setSchemesModel(QAbstractItemModel *model)
{
  ui->qlv_schemes->setModel(model);
}
