#ifndef EXPORTPLOTTOIMAGEDIALOG_H
#define EXPORTPLOTTOIMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ExportPlotToImageDialog;
}

class ExportPlotToImageDialog : public QDialog
{
  Q_OBJECT
public:
  class Parameters {
  public:
    Parameters(const QString &path, const QString &format, const QSizeF &dimensions, const int dpi);
    Parameters &operator=(const Parameters &other);

    const QString path;
    const QString format;
    const QSizeF dimensions;
    const int dpi;
  };

  explicit ExportPlotToImageDialog(const QStringList &supportedFormats, QWidget *parent = nullptr);
  ~ExportPlotToImageDialog();
  Parameters parameters() const;
  void setPlotDimensions(const QSizeF &dimensions);

private:
  Ui::ExportPlotToImageDialog *ui;

private slots:
  void onBrowseClicked();
  void onCancelClicked();
  void onOkClicked();

};

#endif // EXPORTPLOTTOIMAGEDIALOG_H
