#ifndef COMMONPROPERTIESDIALOG_H
#define COMMONPROPERTIESDIALOG_H

#include <QDialog>
#include "../supportedencodings.h"

namespace Ui {
class CommonPropertiesDialog;
}

class CommonPropertiesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CommonPropertiesDialog(const SupportedEncodings::EncodingsVec &encs, QWidget *parent = nullptr);
  ~CommonPropertiesDialog();
  std::string encoding() const;

private:
  void fillEncodings(const SupportedEncodings::EncodingsVec &encs);

  Ui::CommonPropertiesDialog *ui;
};

#endif // COMMONPROPERTIESDIALOG_H
