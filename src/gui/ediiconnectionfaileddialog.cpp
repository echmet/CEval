#include "ediiconnectionfaileddialog.h"
#include "ui_ediiconnectionfaileddialog.h"
#include "../globals.h"

#include <QDialogButtonBox>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QStyle>

const QString EDIIConnectionFailedDialog::s_infoTemp("Failed to connect to EDII service. %1 will terminate.\n"
                                                     "Error reported: %2");

EDIIConnectionFailedDialog::EDIIConnectionFailedDialog(const QString &error, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::EDIIConnectionFailedDialog)
{
  ui->setupUi(this);

  ui->ql_info->setText(s_infoTemp.arg(Globals::SOFTWARE_NAME).arg(error));
#ifdef Q_OS_LINUX
  ui->ql_icon->setPixmap(QIcon::fromTheme("dialog-error").pixmap(64, 64));
#else
  ui->ql_icon->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxCritical).pixmap(64, 64));
#endif // Q_OS_LINUX

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}

EDIIConnectionFailedDialog::~EDIIConnectionFailedDialog()
{
  delete ui;
}
