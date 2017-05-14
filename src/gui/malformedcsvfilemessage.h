#ifndef MALFORMEDCSVFILEMESSAGE_H
#define MALFORMEDCSVFILEMESSAGE_H

#include <QDialog>

namespace Ui {
class MalformedCsvFileMessage;
}

class MalformedCsvFileMessage : public QDialog
{
  Q_OBJECT
public:
  enum class Error {
    POSSIBLY_INCORRECT_SETTINGS,
    BAD_DELIMITER,
    BAD_TIME_DATA,
    BAD_VALUE_DATA
  };

  explicit MalformedCsvFileMessage(const Error err, const int lineNo, const QString &badString, QWidget *parent = nullptr);
  ~MalformedCsvFileMessage();

private:
  Ui::MalformedCsvFileMessage *ui;

  static const QString POSSIBLY_INCORRECT_SETTINGS_MSG;
  static const QString BAD_DELIMITER_MSG;
  static const QString BAD_TIME_MSG;
  static const QString BAD_VALUE_MSG;

private slots:
  void onOkClicked();

};

#endif // MALFORMEDCSVFILEMESSAGE_H
