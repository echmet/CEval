#include "crashhandlerdialog.h"
#include "ui_crashhandlerdialog.h"
#include "../globals.h"
#include "../hyperbolefittingengine.h"
#include <QSysInfo>

const QString CrashHandlerDialog::s_reportToDevsCaption(tr("Report to developers"));


CrashHandlerDialog::CrashHandlerDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CrashHandlerDialog),
  m_apologyMessage(QString(tr("We are sorry, but %2 has encountered an internal error from which it cannot recover."
                           "Your current data table has been saved to \"%1\" file in %2´s working directory. "
                           "It will be available for loading the next time you start %2. "
                           "You may want to report the backtrace below along with a description (in English or Czech) "
                           "of what exactly happened to the developers."))
                           .arg(HyperboleFittingEngine::EMERG_SAVE_FILE)
                           .arg(Globals::SOFTWARE_NAME))
{
  ui->setupUi(this);
  setWindowTitle(tr("Crash handler"));

  ui->ql_reportToDevs->setOpenExternalLinks(true);

  connect(ui->qpb_ok, &QPushButton::clicked, this, &CrashHandlerDialog::onOkClicked);
}

CrashHandlerDialog::~CrashHandlerDialog()
{
  delete ui;
}

void CrashHandlerDialog::onOkClicked()
{
  accept();
}

void CrashHandlerDialog::setBacktrace(const QString &backtrace)
{
  QString mails;

  for (const Globals::DeveloperID &dev : Globals::DEVELOPERS)
    mails.append(QString("%1;").arg(dev.mail.toHtmlEscaped()));

  QString debugInfo = QString("Version: %1\n\nBacktrace:\n%2").arg(Globals::VERSION_STRING()).arg(backtrace);
  QString mailLink = QString("<a href=\"mailto:%1?subject=%2&body=%3\">%4</a>")
                             .arg(mails)
                             .arg(QString("%1 (%2 %3) crash report").arg(Globals::SOFTWARE_NAME).arg(QGuiApplication::platformName()).arg(QSysInfo::buildCpuArchitecture()))
                             .arg(debugInfo.toHtmlEscaped())
                             .arg(s_reportToDevsCaption);

  ui->ql_message->setText(m_apologyMessage);
  ui->qte_backtrace->setText(backtrace);
  ui->ql_reportToDevs->setText(mailLink);
}
