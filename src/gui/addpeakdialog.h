#ifndef ADDPEAKDIALOG_H
#define ADDPEAKDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class AddPeakDialog;
}

class AddPeakDialog : public QDialog
{
  Q_OBJECT
public:
  enum class MobilityFrom {
    HVL_A1,
    PEAK_MAXIMUM
  };
  Q_ENUM(MobilityFrom)

  class Answer {
  public:
    Answer(const bool registerInHF, const QString &name, const MobilityFrom mobilityFrom);

    const bool registerInHF;
    const QString name;
    const MobilityFrom mobilityFrom;
  };

  explicit AddPeakDialog(QWidget *parent = nullptr);
  ~AddPeakDialog();

  Answer answer() const;
  void setInformation(const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  Ui::AddPeakDialog *ui;

  QStandardItemModel m_analytesModel;

  void setMobilityText(const MobilityFrom m);

  double m_hvlMobility;
  double m_peakMaxMobility;

private slots:
  void onAnalyteTextChanged(const QString &s);
  void onCancelClicked();
  void onMobilityFromActivated(const int idx);
  void onOkClicked();
  void onRegisterClicked();

};

#endif // ADDPEAKDIALOG_H
