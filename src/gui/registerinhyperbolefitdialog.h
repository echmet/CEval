#ifndef REGISTERINHYPERBOLEFITDIALOG_H
#define REGISTERINHYPERBOLEFITDIALOG_H

#include "registerinhyperbolefitwidget.h"
#include <QDialog>

namespace Ui {
class RegisterInHyperboleFitDialog;
}

class RegisterInHyperboleFitDialog : public QDialog
{
  Q_OBJECT

public:
  explicit RegisterInHyperboleFitDialog(QWidget *parent = nullptr);
  ~RegisterInHyperboleFitDialog();
  RegisterInHyperboleFitWidget::MobilityFrom mobilityFrom() const;
  void setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  Ui::RegisterInHyperboleFitDialog *ui;

  RegisterInHyperboleFitWidget *m_registerInHFWidget;

private slots:
  void onCancelClicked();
  void onOkClicked();
};

#endif // REGISTERINHYPERBOLEFITDIALOG_H
