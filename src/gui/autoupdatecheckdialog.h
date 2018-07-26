#ifndef AUTOUPDATECHECKDIALOG_H
#define AUTOUPDATECHECKDIALOG_H

#include <QDialog>
#include "softwareupdatewidget.h"

namespace Ui {
class AutoUpdateCheckDialog;
}

class AutoUpdateCheckDialog : public QDialog
{
  Q_OBJECT
public:
  explicit AutoUpdateCheckDialog(QWidget *parent = nullptr);
  ~AutoUpdateCheckDialog();
  void setDisplay(const SoftwareUpdateResult &result);

private:
  Ui::AutoUpdateCheckDialog *ui;

signals:
  void setAutoUpdate(const bool enabled);

private slots:
  void onCheckOnStartupClicked();
  void onCloseClicked();

};

#endif // AUTOUPDATECHECKDIALOG_H
