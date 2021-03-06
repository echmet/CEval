#ifndef ADDPEAKDIALOG_H
#define ADDPEAKDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "registerinhyperbolafitwidget.h"

namespace Ui {
class AddPeakDialog;
}

class AddPeakDialog : public QDialog
{
  Q_OBJECT
public:

  class Answer {
  public:
    Answer(const bool registerInHF, const QString &name, const RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom);

    const bool registerInHF;
    const QString name;
    const RegisterInHyperbolaFitWidget::MobilityFrom mobilityFrom;
  };

  explicit AddPeakDialog(QWidget *parent = nullptr);
  ~AddPeakDialog();

  Answer answer() const;
  void setInformation(const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  Ui::AddPeakDialog *ui;

  RegisterInHyperbolaFitWidget *m_registerInHFWidget;
  QStandardItemModel m_analytesModel;

private slots:
  void onAnalyteTextChanged(const QString &s);
  void onCancelClicked();
  void onOkClicked();
  void onRegisterClicked();

};

#endif // ADDPEAKDIALOG_H
