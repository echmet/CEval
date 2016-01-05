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
  explicit CrashHandlerDialog(QWidget *parent = nullptr);
  ~CrashHandlerDialog();
  void setBacktrace(const QString &backtrace);

private:
  Ui::CrashHandlerDialog *ui;

  static const QString s_mailSubject;
  static const QString s_reportToDevsCaption;
  const QString m_apologyMessage;

private slots:
  void onOkClicked();
};

#endif // CRASHHANDLERDIALOG_H
