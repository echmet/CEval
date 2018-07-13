#ifndef EDIINOTFOUNDDIALOG_H
#define EDIINOTFOUNDDIALOG_H

#include <QDialog>

namespace Ui {
class EDIINotFoundDialog;
}

class EDIINotFoundDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EDIINotFoundDialog(QWidget *parent = nullptr);
  ~EDIINotFoundDialog();

private:
  Ui::EDIINotFoundDialog *ui;

  static const QString s_dialogText;

private slots:
  void onUseDefaultClicked();
  void onSetManuallyClicked();

};

#endif // EDIINOTFOUNDDIALOG_H
