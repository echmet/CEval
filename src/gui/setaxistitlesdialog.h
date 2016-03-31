#ifndef SETAXISTITLESDIALOG_H
#define SETAXISTITLESDIALOG_H

#include <QDialog>

namespace Ui {
class SetAxisTitlesDialog;
}

class SetAxisTitlesDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SetAxisTitlesDialog(const QString &xType, const QString &xUnit, const QString &yType, const QString &yUnit, QWidget *parent = nullptr);
  ~SetAxisTitlesDialog();
  QString xType() const;
  QString xUnit() const;
  QString yType() const;
  QString yUnit() const;

private:
  Ui::SetAxisTitlesDialog *ui;

private slots:
  void onCancelClicked();
  void onOkClicked();

};

#endif // SETAXISTITLESDIALOG_H
