#ifndef COMMONPROPERTIESDIALOG_H
#define COMMONPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class CommonPropertiesDialog;
}

class CommonPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CommonPropertiesDialog(QWidget *parent = 0);
  ~CommonPropertiesDialog();

private:
  Ui::CommonPropertiesDialog *ui;
};

#endif // COMMONPROPERTIESDIALOG_H
