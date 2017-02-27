#ifndef HVLESTIMATEINPROGRESSDIALOG_H
#define HVLESTIMATEINPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class HVLEstimateInProgressDialog;
}

class HVLEstimateInProgressDialog : public QDialog
{
  Q_OBJECT

public:
  explicit HVLEstimateInProgressDialog(QWidget *parent = nullptr);
  ~HVLEstimateInProgressDialog();

private:
  Ui::HVLEstimateInProgressDialog *ui;

public slots:
  void onEstimateDone();

};

#endif // HVLESTIMATEINPROGRESSDIALOG_H
