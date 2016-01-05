#ifndef HVLFITINPROGRESSDIALOG_H
#define HVLFITINPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class HVLFitInProgressDialog;
}

class HVLFitInProgressDialog : public QDialog
{
  Q_OBJECT

public:
  explicit HVLFitInProgressDialog(QWidget *parent = nullptr);
  ~HVLFitInProgressDialog();
  void reject() override;

private:
  Ui::HVLFitInProgressDialog *ui;


public slots:
  void onHvlFitDone();
};

#endif // HVLFITINPROGRESSDIALOG_H
