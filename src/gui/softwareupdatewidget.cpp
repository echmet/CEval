#include "softwareupdatewidget.h"
#include "ui_softwareupdatewidget.h"

SoftwareUpdateWidget::SoftwareUpdateWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SoftwareUpdateWidget)
{
  ui->setupUi(this);
}

SoftwareUpdateWidget::~SoftwareUpdateWidget()
{
  delete ui;
}
