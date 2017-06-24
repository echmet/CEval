#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../globals.h"
#include <QFontMetrics>
#include <QSysInfo>

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);

  ui->ql_echmetLink->setOpenExternalLinks(true);

  ui->ql_sotfwareName->setText(Globals::VERSION_STRING());
  ui->ql_echmetLink->setText(QString("<a href=\"%1\">%1</a>").arg(Globals::ECHMET_WEB_LINK.toHtmlEscaped()));
  ui->ql_thankYouQt->setText(QString("Based on Qt libraries. Version in use: %1 (%2 %3)")
                                     .arg(QT_VERSION_STR)
                                     .arg(QGuiApplication::platformName())
                                     .arg(QSysInfo::buildCpuArchitecture()));

  {
    for (const Globals::DeveloperID &dev : Globals::DEVELOPERS) {
      const QString s = dev.linkString();

      QLabel *l = new QLabel(s, this);
      l->setAlignment(Qt::AlignHCenter);
      l->setOpenExternalLinks(true);
      ui->gridLayout->addWidget(l);
    }
  }

  {
    QFontMetricsF fm(ui->ql_thankYouQt->font());
    setMinimumWidth(fm.boundingRect(ui->ql_thankYouQt->text()).width() * 1.15);
  }

  connect(ui->qpb_close, &QPushButton::clicked, this, &AboutDialog::onCloseClicked);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}

void AboutDialog::onCloseClicked()
{
  accept();
}
