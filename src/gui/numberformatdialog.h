#ifndef NUMBERFORMATDIALOG_H
#define NUMBERFORMATDIALOG_H

#include <QDialog>
#include <QIntValidator>

namespace Ui {
class NumberFormatDialog;
}

class NumberFormatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit NumberFormatDialog(QWidget *parent = nullptr);
  ~NumberFormatDialog();
  void setParameters(const char type, const int digits);


private:
  Ui::NumberFormatDialog *ui;

  QIntValidator m_intValidator;

private slots:
  void onCancelClicked();
  void onOkClicked();

};

#endif // NUMBERFORMATDIALOG_H
