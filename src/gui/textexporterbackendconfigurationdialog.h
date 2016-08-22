#ifndef TEXTEXPORTERBACKENDCONFIGURATIONDIALOG_H
#define TEXTEXPORTERBACKENDCONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class TextExporterBackendConfigurationDialog;
}

class TextExporterBackendConfigurationDialog : public QDialog
{
  Q_OBJECT
public:
  explicit TextExporterBackendConfigurationDialog(QWidget *parent = nullptr);
  ~TextExporterBackendConfigurationDialog();
  QString interact(bool &canceled, const QChar &currentDelimiter);

private:
  Ui::TextExporterBackendConfigurationDialog *ui;

  void onCancelClicked();
  void onOkClicked();
};

#endif // TEXTEXPORTERBACKENDCONFIGURATIONDIALOG_H
