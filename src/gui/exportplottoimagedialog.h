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
    Parameters(const QString &path, const QString &format, const QSizeF &dimensions, const int dpi,
               const qreal &axisTitlesFontSize, const qreal &axisNumbersFontSize, const qreal &chartTitleFontSize);
    Parameters &operator=(const Parameters &other);

    const QString path;
    const QString format;
    const QSizeF dimensions;
    const int dpi;
    const qreal axisTitlesFontSize;
    const qreal axisNumbersFontSize;
    const qreal chartTitleFontSize;
  };

  explicit ExportPlotToImageDialog(const QStringList &supportedFormats, QWidget *parent = nullptr);
  ~ExportPlotToImageDialog();
  Parameters parameters() const;
  void setPlotDimensions(const QSizeF &dimensions);

private:
  Ui::ExportPlotToImageDialog *ui;
  double m_aspectRatio;

private slots:
  void onBrowseClicked();
  void onCancelClicked();
  void onFilePathChanged(const QString &path);
  void onHeightChanged(const double h);
  void onKeepAspectRatioClicked();
  void onOkClicked();
  void onWidthChanged(const double w);


};

#endif // EXPORTPLOTTOIMAGEDIALOG_H
