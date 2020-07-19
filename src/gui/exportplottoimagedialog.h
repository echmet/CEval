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
               const QString &title,
               const qreal &axisTitlesFontSize, const qreal &axisNumbersFontSize, const qreal &chartTitleFontSize);
    Parameters(const Parameters &other);
    Parameters &operator=(const Parameters &other);

    const QString path;
    const QString format;
    const QSizeF dimensions;
    const int dpi;
    const QString title;
    const qreal axisTitlesFontSize;
    const qreal axisNumbersFontSize;
    const qreal chartTitleFontSize;
  };

  explicit ExportPlotToImageDialog(const QStringList &supportedFormats, QWidget *parent = nullptr);
  ~ExportPlotToImageDialog();
  Parameters parameters() const;
  void setAspectRatio(const qreal aspectRatio);

private:
  Ui::ExportPlotToImageDialog *ui;

  qreal m_aspectRatio;
  qreal m_guiAspectRatio;
  const QStringList m_supportedFormats;

private slots:
  void onBrowseClicked();
  void onCancelClicked();
  void onFileFormatChanged(const int idx);
  void onFilePathChanged(const QString &path);
  void onHeightChanged(const qreal h);
  void onKeepAspectRatioClicked();
  void onOkClicked();
  void onResetToAspectRatio();
  void onWidthChanged(const qreal w);


};

#endif // EXPORTPLOTTOIMAGEDIALOG_H
