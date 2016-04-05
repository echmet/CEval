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
  enum class ExportMode {
    SINGLE_FILE
  };
  Q_ENUM(ExportMode)

  class Parameters {
  public:
    explicit Parameters();
    Parameters(const Parameters &other);
    Parameters(const ExportMode exMode, const QString &path, const QChar &decimalSeparator, const QString &delimiter);

    const ExportMode exMode;
    const QString path;
    const QChar decimalSeparator;
    const QString delimiter;

  };

  explicit ExportDatatableToCsvDialog(QWidget *parent = nullptr);
  ~ExportDatatableToCsvDialog();
  Parameters parameters() const;

private:
  Ui::ExportDatatableToCsvDialog *ui;

  QString m_lastPath;

private slots:
  void onBrowseClicked();
  void onCancelClicked();
  void onOkClicked();
};

#endif // EXPORTDATATABLETOCSVDIALOG_H
