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
  enum class HeaderHandling {
    NO_HEADER,             /*<! CSV file has no header */
    HEADER_WITH_UNITS,     /*<! CSV file header contains the unit of the scale */
    HEADER_WITHOUT_UNITS   /*<! CSV file header does not contain the unit of the scale */
  };
  Q_ENUM(HeaderHandling);

  class Parameters {
  public:
    Parameters(const QString &delimiter, const QChar &decimalSeparator,
               const int xColumn, const int yColumn,
               const QString &xType, const QString &yType, const QString &xUnit, const QString &yUnit,
               const HeaderHandling header, const quint32 linesToSkip,
               const bool readBom, const QString &encodingId);
    Parameters();

    const QString delimiter;
    const QChar decimalSeparator;
    const int xColumn;
    const int yColumn;
    const QString xType;
    const QString yType;
    const QString xUnit;
    const QString yUnit;
    const HeaderHandling header;
    const quint32 linesToSkip;
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
  void onHeaderHandlingChanged(const int idx);
  void onLoadClicked();

};

#endif // LOADCSVFILEDIALOG_H
