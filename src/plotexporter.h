#ifndef PLOTEXPORTER_H
#define PLOTEXPORTER_H

#include <QObject>
#include <QWidget>
#include "gui/exportplottoimagedialog.h"

class PlotExporter : public QObject
{
  Q_OBJECT
public:
  PlotExporter(QObject *parent = nullptr);
  ~PlotExporter();

  void exportToBitmap(QWidget *source);
  void writePixmapToFile(const QPixmap &pixmap, const QString &path, const QString &format);

private:
  QStringList m_supportedFormats;
  ExportPlotToImageDialog *m_exportDlg;

};

#endif // PLOTEXPORTER_H
