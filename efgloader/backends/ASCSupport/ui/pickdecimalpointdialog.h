#ifndef PICKDECIMALPOINTDIALOG_H
#define PICKDECIMALPOINTDIALOG_H

#include <QDialog>

namespace Ui {
class PickDecimalPointDialog;
}

class PickDecimalPointDialog : public QDialog
{
  Q_OBJECT
public:
  explicit PickDecimalPointDialog(const QString &name, QWidget *parent = nullptr);
  ~PickDecimalPointDialog();
  char separator() const;

private:
  Ui::PickDecimalPointDialog *ui;
};

#endif // PICKDECIMALPOINTDIALOG_H
