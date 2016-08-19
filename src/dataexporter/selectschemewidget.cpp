#include "selectschemewidget.h"
#include "ui_selectschemewidget.h"
#include <QAbstractItemModel>

using namespace DataExporter;

SelectSchemeWidget::SelectSchemeWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SelectSchemeWidget)
{
  ui->setupUi(this);

  ui->qlv_schemes->setEditTriggers(QListView::NoEditTriggers);

  connect(ui->qpb_createScheme, &QPushButton::clicked, this, &SelectSchemeWidget::onCreateSchemeClicked);
  connect(ui->qpb_editScheme, &QPushButton::clicked, this, &SelectSchemeWidget::onEditSchemeClicked);
  connect(ui->qpb_removeScheme, &QPushButton::clicked, this, &SelectSchemeWidget::onRemoveSchemeClicked);
  connect(ui->qpb_useScheme, &QPushButton::clicked, this, &SelectSchemeWidget::onUseSchemeClicked);
  connect(ui->qpb_close, &QPushButton::clicked, this, &SelectSchemeWidget::onCloseClicked);
}

SelectSchemeWidget::~SelectSchemeWidget()
{
  delete ui;
}

void SelectSchemeWidget::onCloseClicked()
{
  emit closed();
}

void SelectSchemeWidget::onCreateSchemeClicked()
{
  emit createScheme();
}

void SelectSchemeWidget::onEditSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();
  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit editScheme(name);
}

void SelectSchemeWidget::onLoadSchemeClicked()
{
}

void SelectSchemeWidget::onRemoveSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();

  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit removeScheme(name);
}

void SelectSchemeWidget::onSaveSchemeClicked()
{
}

void SelectSchemeWidget::onUseSchemeClicked()
{
  const QModelIndex &idx = ui->qlv_schemes->currentIndex();
  if (!idx.isValid())
    return;

  const QString name = ui->qlv_schemes->model()->data(idx, Qt::UserRole).toString();

  emit useScheme(name);
}

void SelectSchemeWidget::setSchemesModel(QAbstractItemModel *model)
{
  ui->qlv_schemes->setModel(model);
}
