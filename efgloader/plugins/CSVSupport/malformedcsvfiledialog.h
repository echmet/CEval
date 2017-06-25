#ifndef MALFORMEDCSVFILEDIALOG_H
#define MALFORMEDCSVFILEDIALOG_H

#include <QDialog>

namespace Ui {
class MalformedCsvFileDialog;
}

class MalformedCsvFileDialog : public QDialog
{
  Q_OBJECT
public:
  enum class Error {
    POSSIBLY_INCORRECT_SETTINGS,
    BAD_DELIMITER,
    BAD_TIME_DATA,
    BAD_VALUE_DATA
  };

  explicit MalformedCsvFileDialog(const Error err, const int lineNo, const QString &badString, QWidget *parent = nullptr);
  ~MalformedCsvFileDialog();

private:
  Ui::MalformedCsvFileDialog *ui;

  static const QString POSSIBLY_INCORRECT_SETTINGS_MSG;
  static const QString BAD_DELIMITER_MSG;
  static const QString BAD_TIME_MSG;
  static const QString BAD_VALUE_MSG;

private slots:
  void onOkClicked();

};

#endif // MALFORMEDCSVFILEDIALOG_H
