#include "selectediipath.h"
#include "ui_selectediipath.h"
#include "../cevalconfig.h"
#include "../efg/efgloaderwatcher.h"
#include "../globals.h"

#include <QFileDialog>
#include <QMessageBox>

SelectEDIIPath::SelectEDIIPath(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SelectEDIIPath)
{
  ui->setupUi(this);

  ui->qle_path->setText(CEvalConfig::value(CEvalConfig::EDII_SERVICE_PATH_TAG).toString());

  connect(ui->qpb_browse, &QPushButton::clicked, this, &SelectEDIIPath::onBrowseClicked);
  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SelectEDIIPath::onOkClicked);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SelectEDIIPath::reject);
}

SelectEDIIPath::~SelectEDIIPath()
{
  delete ui;
}

QString SelectEDIIPath::browseToEDII(const QString &browseFrom)
{
  QString path = browseFrom;
  QFileDialog dlg(nullptr, "Set path to ECHMET Data Import Infrastructure service");

  dlg.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
  dlg.setFileMode(QFileDialog::Directory);


  while (true) {
    dlg.setDirectory(browseFrom);
    if (dlg.exec() != QDialog::Accepted)
      return browseFrom;

    path = dlg.selectedFiles()[0];
    if (efg::EFGLoaderWatcher::isServicePathValid(path))
      return path;

    QMessageBox mbox(QMessageBox::Warning,
                     QObject::tr("ECHMET Data Import Infrastructure error"),
                     QObject::tr("Given path does not appear to contain EDII service executable. Please enter a valid path."));
    mbox.exec();
  }
}

void SelectEDIIPath::onBrowseClicked()
{
  const auto path = browseToEDII(ui->qle_path->text());

  ui->qle_path->setText(path);
}

void SelectEDIIPath::onOkClicked()
{
  const QString path = ui->qle_path->text();
  if (!efg::EFGLoaderWatcher::isServicePathValid(path)) {
    QMessageBox mbox(QMessageBox::Warning,
                     QObject::tr("ECHMET Data Import Infrastructure error"),
                     QObject::tr("Given path does not appear to contain EDII service executable. Please enter a valid path."));
    mbox.exec();

    return;
  }

  CEvalConfig::setValue(CEvalConfig::EDII_SERVICE_PATH_TAG, path);

  QMessageBox mbox(QMessageBox::Information, tr("EDII path changed"),
                   QString(tr("Path to EDII service has been changed. Please restart %1 in order for the change to take effect.")).arg(Globals::SOFTWARE_NAME));
  mbox.exec();

  accept();
}
