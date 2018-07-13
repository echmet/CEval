#include "ediinotfounddialog.h"
#include "ui_ediinotfounddialog.h"

#include "../globals.h"
#include "../efg/efgloaderwatcher.h"

const QString EDIINotFoundDialog::s_dialogText(tr(
    "<html><head/><body><p>Path to ECHMET Data Import Infrastructure (EDII) service has not been set. "
    "This service is necessary for %1 to load experimental data and %1 cannot function without it.</p><p>"
    "If you have downloaded %1 from official sources "
    "(<a href=\"https://echmet.natur.cuni.cz/software/download\"><span style=\" text-decoration: underline; color:#007af4;\">ECHMET research group website</span></a>) "
    "you may select the &quot;Default path&quot;. %1 will then use EDII binaries shipped with the installation package.</p><p>"
    "If you already use a software that makes use of EDII you may specify the path to EDII service manually. "
    "This will ensure better consistency of supported data formats. Manually specified path shall contain a "
    "\"%2\" directory with \"%3\" executable file inside."
    "</p></body></html>"));

EDIINotFoundDialog::EDIINotFoundDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EDIINotFoundDialog)
{
  ui->setupUi(this);

  ui->ql_mainText->setText(s_dialogText.arg(Globals::SOFTWARE_NAME)
                           .arg(efg::EFGLoaderWatcher::s_EFGLoaderPathPrefix)
                           .arg(efg::EFGLoaderWatcher::s_EFGLoaderBinaryName));

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
