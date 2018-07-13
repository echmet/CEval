#ifndef PLOTEXPORTER_H
#define PLOTEXPORTER_H

#include <QObject>
#include <QWidget>
#include "gui/exportplottoimagedialog.h"

class QwtPlot;

class PlotExporter : public QObject
{
  Q_OBJECT
public:
  PlotExporter(QObject *parent = nullptr);
  ~PlotExporter();

  void exportPlot(QwtPlot *plot, const QRectF &zoom);

private:
  QStringList m_supportedFormats;
  ExportPlotToImageDialog *m_exportDlg;
  QPalette m_plotPalette;

  void guessPlotDimensions(const QwtPlot *plot, const double currentWidth, QSizeF &dimensions);
  void renderPlotToFile(QwtPlot *plot, const QString &path, const QString &format, const QSizeF &dimensions, const int dpi);
  void setAxisTitleFont(QwtPlot *plot, const int a, const QFont &f);
  void setTitleFont(QwtPlot *plot, const QFont &f);

};

#endif // PLOTEXPORTER_H
