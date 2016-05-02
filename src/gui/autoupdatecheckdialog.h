#ifndef AUTOUPDATECHECKDIALOG_H
#define AUTOUPDATECHECKDIALOG_H

#include <QDialog>

namespace Ui {
class AutoUpdateCheckDialog;
}

class AutoUpdateCheckDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AutoUpdateCheckDialog(QWidget *parent = 0);
  ~AutoUpdateCheckDialog();

private:
  Ui::AutoUpdateCheckDialog *ui;
};

#endif // AUTOUPDATECHECKDIALOG_H
