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
    Parameters(const ExportMode exMode, const QString &path, const QChar &decimalSeparator, const QString &delimiter, int precision);

    const ExportMode exMode;
    const QString path;
    const QChar decimalSeparator;
    const QString delimiter;
    const int precision;

  };

  explicit ExportDatatableToCsvDialog(const QStringList &nameFilter, const QString &lastPath, QWidget *parent = nullptr);
  ~ExportDatatableToCsvDialog();
  QString lastPath() const;
  Parameters parameters() const;
  void setLastPath(const QString &path);

private:
  Ui::ExportDatatableToCsvDialog *ui;

  QString m_lastPath;
  const QStringList &m_nameFilter;

private slots:
  void onBrowseClicked();
  void onCancelClicked();
  void onOkClicked();
};

#endif // EXPORTDATATABLETOCSVDIALOG_H
