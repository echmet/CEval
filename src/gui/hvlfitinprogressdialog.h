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
  explicit HVLFitInProgressDialog(const int maxIterations, QWidget *parent = nullptr);
  ~HVLFitInProgressDialog();
  int exec() override;
  void reject() override;

private:
  Ui::HVLFitInProgressDialog *ui;

  bool m_execable;
  const int m_maxIterations;

  void setCurrentIterationText(const int iteration, const double avgTimePerIter);

signals:
  void abortFit();

private slots:
  void onAbortClicked();

public slots:
  void onHvlFitDone();
  void setCurrentIteration(const int iteration, const double avgTimePerIter);

};

#endif // HVLFITINPROGRESSDIALOG_H
