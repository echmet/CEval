#ifndef REGISTERINHYPERBOLAFITDIALOG_H
#define REGISTERINHYPERBOLAFITDIALOG_H

#include "registerinhyperbolafitwidget.h"
#include <QDialog>

namespace Ui {
class RegisterInHyperbolaFitDialog;
}

class RegisterInHyperbolaFitDialog : public QDialog
{
  Q_OBJECT

public:
  explicit RegisterInHyperbolaFitDialog(QWidget *parent = nullptr);
  ~RegisterInHyperbolaFitDialog();
  RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom() const;
  void setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  Ui::RegisterInHyperbolaFitDialog *ui;

  RegisterInHyperbolaFitWidget *m_registerInHFWidget;

private slots:
  void onCancelClicked();
  void onOkClicked();
};

#endif // REGISTERINHYPERBOLAFITDIALOG_H
