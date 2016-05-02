#ifndef CHECKFORUPDATEDIALOG_H
#define CHECKFORUPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class CheckForUpdateDialog;
}

class CheckForUpdateDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CheckForUpdateDialog(QWidget *parent = 0);
  ~CheckForUpdateDialog();

private:
  Ui::CheckForUpdateDialog *ui;
};

#endif // CHECKFORUPDATEDIALOG_H
