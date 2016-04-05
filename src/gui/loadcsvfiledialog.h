#ifndef LOADCSVFILEDIALOG_H
#define LOADCSVFILEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

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
               const QString &xCaption, const QString &yCaption, const bool hasHeader,
               const bool readBom, const QString &encodingId);
    Parameters();

    const QString delimiter;
    const QChar decimalSeparator;
    const QString xCaption;
    const QString yCaption;
    const bool hasHeader;
    const bool readBom;
    const QString encodingId;

    Parameters &operator=(const Parameters &other);
  };

  explicit LoadCsvFileDialog(QWidget *parent = nullptr);
  ~LoadCsvFileDialog();
  void addEncoding(const QString &id, const QString &name, const bool canHaveBom);
  Parameters parameters() const;

private:
  Ui::LoadCsvFileDialog *ui;
  Parameters m_parameters;
  QStandardItemModel m_encodingsModel;

  static const QString s_qlDelimiterToolTip;
  static const QString s_qlDecimalSeparatorToolTip;
  static const QString s_qlFirstLineIsHeaderToolTip;

private slots:
  void onCancelClicked();
  void onEncodingChanged(const int idx);
  void onHasHeaderClicked();
  void onLoadClicked();

};

#endif // LOADCSVFILEDIALOG_H
