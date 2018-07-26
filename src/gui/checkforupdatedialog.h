#ifndef CHECKFORUPDATEDIALOG_H
#define CHECKFORUPDATEDIALOG_H

#include <QDialog>

class SoftwareUpdateResult;

namespace Ui {
class CheckForUpdateDialog;
}

class CheckForUpdateDialog : public QDialog
{
  Q_OBJECT
public:
  explicit CheckForUpdateDialog(QWidget *parent = nullptr);
  ~CheckForUpdateDialog();
  void setCheckOnStartup(const bool checked);

protected:
  void closeEvent(QCloseEvent *ev);

private:
  Ui::CheckForUpdateDialog *ui;

signals:
  void checkForUpdate();
  void closed();
  void setAutoUpdate(const bool enabled);

public slots:
  void onCheckComplete(const SoftwareUpdateResult &result);
  void onAutoUpdateChanged(const bool enabled);

private slots:
  void onCheckNowClicked();
  void onCheckOnStartupClicked();
  void onCloseClicked();

};

#endif // CHECKFORUPDATEDIALOG_H
