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
  class Answer {
  public:
    Answer(const bool registerInHF, const QString &name);

    const bool registerInHF;
    const QString name;
  };

  explicit AddPeakDialog(QWidget *parent = nullptr);
  ~AddPeakDialog();

  Answer answer() const;
  void setInformation(const double selConcentration, const double mobility);

private:
  Ui::AddPeakDialog *ui;

  QStandardItemModel m_analytesModel;

private slots:
  void onAnalyteTextChanged(const QString &s);
  void onCancelClicked();
  void onOkClicked();
  void onRegisterClicked();

};

#endif // ADDPEAKDIALOG_H
