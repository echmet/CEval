#ifndef EXPORTDATATABLETOCSVDIALOG_H
#define EXPORTDATATABLETOCSVDIALOG_H

#include <QDialog>

namespace Ui {
class ExportDatatableToCsvDialog;
}

class ExportDatatableToCsvDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ExportDatatableToCsvDialog(QWidget *parent = 0);
  ~ExportDatatableToCsvDialog();

private:
  Ui::ExportDatatableToCsvDialog *ui;
};

#endif // EXPORTDATATABLETOCSVDIALOG_H
