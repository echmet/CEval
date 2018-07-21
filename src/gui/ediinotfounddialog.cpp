#include "ediinotfounddialog.h"
#include "ui_ediinotfounddialog.h"

#include "../globals.h"
#include "../efg/efgloaderwatcher.h"

EDIINotFoundDialog::EDIINotFoundDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EDIINotFoundDialog),
  m_dialogTextTemp(tr("Path to ECHMET Data Import Infrastructure (EDII) service has not been set. "
                      "This service is necessary for %1 to load experimental data and %1 cannot function without it.\n\n"
                      "Unless you have a reason to set the path manually default path should be safe to use."))
{
  ui->setupUi(this);

  ui->ql_mainText->setText(m_dialogTextTemp.arg(Globals::SOFTWARE_NAME));

  connect(ui->qpb_useDefault, &QPushButton::clicked, this, &EDIINotFoundDialog::reject);
  connect(ui->qpb_setManually, &QPushButton::clicked, this, &EDIINotFoundDialog::accept);
}

EDIINotFoundDialog::~EDIINotFoundDialog()
{
  delete ui;
}
