#include "crashhandlerdialog.h"
#include "ui_crashhandlerdialog.h"
#include "../globals.h"
#include "../hyperbolafittingengine.h"
#include <QSysInfo>
#include <QDesktopServices>
#include <QMessageBox>

const QString CrashHandlerDialog::s_reportToDevsCaption(QObject::tr("Report to developers"));


CrashHandlerDialog::CrashHandlerDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CrashHandlerDialog),
  m_apologyMessage(QString(QObject::tr("We are sorry, but %2 has encountered an internal error from which it cannot recover. "
                           "Your current data table has been saved to \"%1\" file in %2´s working directory. "
                           "It will be available for loading the next time you start %2. "
                           "You may want to report the backtrace below along with a description (in English or Czech) "
                           "of what exactly happened to the developers."))
                           .arg(HyperbolaFittingEngine::EMERG_SAVE_FILE)
                           .arg(Globals::SOFTWARE_NAME))
{
  ui->setupUi(this);
  setWindowTitle(QObject::tr("Crash handler"));

  connect(ui->qpb_ok, &QPushButton::clicked, this, &CrashHandlerDialog::onOkClicked);
  connect(ui->qpb_reportToDevelopers, &QPushButton::clicked, this, &CrashHandlerDialog::onReportToDevelopersClicked);
}

CrashHandlerDialog::~CrashHandlerDialog()
{
  delete ui;
}

void CrashHandlerDialog::onOkClicked()
{
  accept();
}

void CrashHandlerDialog::onReportToDevelopersClicked()
{
  bool sent = QDesktopServices::openUrl(QUrl(m_mailToDevelopers));

  if (!sent)
    QMessageBox::warning(nullptr, QObject::tr("Cannot send report"),
                         QObject::tr("Cannot send report. Check that you have an e-mail client installed and set up."));
}

void CrashHandlerDialog::setBacktrace(const QString &backtrace)
{
  QString mails;

  for (const Globals::DeveloperID &dev : Globals::DEVELOPERS)
    mails.append(QString("%1;").arg(dev.mail.toHtmlEscaped()));

  QString debugInfo = QString("Version: %1\n\nBacktrace:\n%2").arg(Globals::VERSION_STRING()).arg(backtrace);
  m_mailToDevelopers = QString("<a href=\"mailto:%1?subject=%2&body=%3\">%4</a>")
                               .arg(mails)
                               .arg(QString("%1 (%2 %3) crash report").arg(Globals::SOFTWARE_NAME).arg(QGuiApplication::platformName()).arg(QSysInfo::buildCpuArchitecture()))
                               .arg(debugInfo.toHtmlEscaped())
                               .arg(s_reportToDevsCaption);

  ui->ql_message->setText(m_apologyMessage);
  ui->qte_backtrace->setText(backtrace);
}
