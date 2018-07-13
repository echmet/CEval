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
  void setDisplay(const SoftwareUpdateWidget::Result result, const QString &versionTag, const QString &downloadLink);

private:
  Ui::AutoUpdateCheckDialog *ui;

signals:
  void setAutoUpdate(const bool enabled);

private slots:
  void onCheckOnStartupClicked();
  void onCloseClicked();

};

#endif // AUTOUPDATECHECKDIALOG_H
