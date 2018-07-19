#ifndef EDIICONNECTIONFAILEDDIALOG_H
#define EDIICONNECTIONFAILEDDIALOG_H

#include <QDialog>

namespace Ui {
class EDIIConnectionFailedDialog;
}

class EDIIConnectionFailedDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EDIIConnectionFailedDialog(const QString &error, QWidget *parent = nullptr);
  ~EDIIConnectionFailedDialog();

private:
  Ui::EDIIConnectionFailedDialog *ui;

  static const QString s_infoTemp;
};

#endif // EDIICONNECTIONFAILEDDIALOG_H
