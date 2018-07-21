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

  const QString m_dialogTextTemp;

private slots:
  void onUseDefaultClicked();
  void onSetManuallyClicked();

};

#endif // EDIINOTFOUNDDIALOG_H
