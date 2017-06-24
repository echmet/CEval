#ifndef CRASHHANDLERDIALOG_H
#define CRASHHANDLERDIALOG_H

#include <QDialog>

namespace Ui {
class CrashHandlerDialog;
}

class CrashHandlerDialog : public QDialog
{
  Q_OBJECT
public:
  explicit CrashHandlerDialog(const bool postCrash = false, QWidget *parent = nullptr);
  ~CrashHandlerDialog();
  void setBacktrace(const QString &backtrace);

private:
  Ui::CrashHandlerDialog *ui;

  QString m_apologyMessage;
  QString m_mailToDevelopers;

  const QString m_apologyMessagePartOneDuring;
  const QString m_apologyMessagePartOnePostCrash;
  const QString m_apologyMessagePartTwo;

  static const QString s_mailSubject;
  static const QString s_reportToDevsCaption;
  static const QString s_dialogCaptionDuring;
  static const QString s_dialogCaptionPostCrash;

private slots:
  void onOkClicked();
  void onReportToDevelopersClicked();

};

#endif // CRASHHANDLERDIALOG_H
