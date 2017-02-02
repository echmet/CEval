#include "crashhandlerdialog.h"
#include "ui_crashhandlerdialog.h"
#include "../globals.h"
#include "../hyperbolafittingengine.h"
#include <QSysInfo>
#include <QDesktopServices>
#include <QMessageBox>

const QString CrashHandlerDialog::s_reportToDevsCaption(QObject::tr("Report to developers"));
const QString CrashHandlerDialog::s_dialogCaptionDuring("D\xC3\xA9j\xC3\xA0 vu, I fall apart!");
const QString CrashHandlerDialog::s_dialogCaptionPostCrash("D\xC3\xA9j\xC3\xA0 vu, I fell apart!");

CrashHandlerDialog::CrashHandlerDialog(const bool postCrash, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CrashHandlerDialog),
  m_apologyMessagePartOneDuring(QString(QObject::tr("We are sorry, but %1 has encountered an internal error from which it cannot recover.")).arg(Globals::SOFTWARE_NAME)),
  m_apologyMessagePartOnePostCrash(QString(QObject::tr("We are sorry, but it seems that %1 crashed last time it was run.")).arg(Globals::SOFTWARE_NAME)),
  m_apologyMessagePartTwo(QString(QObject::tr("\n"
                                              "Your %4 data table has been saved to \"%1\" file in %2\x27s working directory. "
                                              "%3"
                                              "You may want to report the backtrace below along with a description (in English or Czech) "
                                              "of what exactly happened to the developers."))
                                              .arg(HyperbolaFittingEngine::EMERG_SAVE_FILE)
                                              .arg(Globals::SOFTWARE_NAME)
                                              .arg(postCrash ? "" : QString(QObject::tr("It will be available for loading the next time you start %1." )).arg(Globals::SOFTWARE_NAME)))
{
  ui->setupUi(this);
  setWindowTitle(QObject::tr("Crash handler"));
  if (postCrash) {
    ui->ql_fallApart->setText(s_dialogCaptionPostCrash);
    m_apologyMessage = m_apologyMessagePartOnePostCrash;
  } else {
    ui->ql_fallApart->setText(s_dialogCaptionDuring);
    m_apologyMessage = m_apologyMessagePartOneDuring;
  }

  m_apologyMessage += m_apologyMessagePartTwo.arg(postCrash ? QObject::tr("last") : QObject::tr("current"));

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
  m_mailToDevelopers = QString("mailto:%1?subject=%2&body=%3")
                               .arg(mails)
                               .arg(QString("%1 (%2 %3) crash report").arg(Globals::SOFTWARE_NAME).arg(QGuiApplication::platformName()).arg(QSysInfo::buildCpuArchitecture()))
                               .arg(debugInfo.toHtmlEscaped());

  ui->ql_message->setText(m_apologyMessage);
  ui->qte_backtrace->setText(backtrace);
}
