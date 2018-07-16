#include "ediinotfounddialog.h"
#include "ui_ediinotfounddialog.h"

#include "../globals.h"
#include "../efg/efgloaderwatcher.h"

const QString EDIINotFoundDialog::s_dialogText(
    QString(tr("Path to ECHMET Data Import Infrastructure (EDII) service has not been set. "
               "This service is necessary for %1 to load experimental data and %1 cannot function without it.\n\n"
               "Unless you have a reason to set the path manually default path should be safe to use.")).arg(Globals::SOFTWARE_NAME));

EDIINotFoundDialog::EDIINotFoundDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EDIINotFoundDialog)
{
  ui->setupUi(this);

  ui->ql_mainText->setText(s_dialogText);

  connect(ui->qpb_useDefault, &QPushButton::clicked, this, &EDIINotFoundDialog::onUseDefaultClicked);
  connect(ui->qpb_setManually, &QPushButton::clicked, this, &EDIINotFoundDialog::onSetManuallyClicked);
}

EDIINotFoundDialog::~EDIINotFoundDialog()
{
  delete ui;
}

void EDIINotFoundDialog::onUseDefaultClicked()
{
  reject();
}

void EDIINotFoundDialog::onSetManuallyClicked()
{
  accept();
}
