#ifndef LOADCSVFILEDIALOG_H
#define LOADCSVFILEDIALOG_H

#include <QDialog>

namespace Ui {
class LoadCsvFileDialog;
}

class LoadCsvFileDialog : public QDialog
{
  Q_OBJECT
public:
  class Parameters {
  public:
    Parameters(const QString &delimiter, const QChar &decimalSeparator,
               const QString &xCaption, const QString &yCaption, const bool hasHeader);
    Parameters();

    const QString delimiter;
    const QChar decimalSeparator;
    const QString xCaption;
    const QString yCaption;
    const bool hasHeader;

    Parameters &operator=(const Parameters &other);
  };

  explicit LoadCsvFileDialog(QWidget *parent = nullptr);
  ~LoadCsvFileDialog();
  Parameters parameters() const;

private:
  Ui::LoadCsvFileDialog *ui;
  Parameters m_parameters;

private slots:
  void onCancelClicked();
  void onHasHeaderClicked();
  void onLoadClicked();

};

#endif // LOADCSVFILEDIALOG_H
