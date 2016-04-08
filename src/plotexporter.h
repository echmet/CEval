#ifndef PLOTEXPORTER_H
#define PLOTEXPORTER_H

#include <QObject>
#include <QWidget>

class PlotExporter : public QObject
{
public:
  PlotExporter(QObject *parent = nullptr);
  ~PlotExporter();

  void exportToBitmap(const QWidget *source);

private:

};

#endif // PLOTEXPORTER_H
