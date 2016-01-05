#ifndef MODECONTEXT_H
#define MODECONTEXT_H

#include <memory>
#include <QMap>
#include <QPointF>
#include <QRect>
#include "serieproperties.h"
#include "gui/adjustplotvisualsdialog.h"

class QwtPlot;
class QwtPlotCurve;
class QwtPlotPicker;
class QwtPlotZoomer;

class ModeContext : public QObject {
  Q_OBJECT
public:
  explicit ModeContext(QwtPlot *plot, QwtPlotPicker *picker, QwtPlotZoomer *zoomer, QObject *parent = nullptr);
  void activate();
  bool addSerie(const int id, const QString &title, SerieProperties::VisualStyle &style);
  void adjustAppearance();
  QFont axisFont(const SerieProperties::Axis a) const;
  void clearAllSerieSamples();
  void clearSerieSamples(const int id);
  void deactivate();
  void hideSerie(const int id);
  void removeSerie(const int id);
  void replot();
  bool serieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void setAxisTitle(const SerieProperties::Axis axis, const QString &title, bool store = true);
  void setAxisFont(const SerieProperties::Axis axis, const QFont &f);
  void setSerieSamples(const int id, const QVector<QPointF> &samples);
  void setPlotTitle(const QString &title);
  void setSerieVisualStyle(const int id, SerieProperties::VisualStyle &style);
  void showSerie(const int id);

private:
  bool m_active;
  QwtPlot *m_plot;
  QMap<int, std::shared_ptr<QwtPlotCurve>> m_plotCurves;
  QwtPlotPicker *m_plotPicker;
  QwtPlotZoomer *m_plotZoomer;
  QMap<SerieProperties::Axis, QFont> m_axisFont;
  QMap<SerieProperties::Axis, QString> m_axisTitles;
  QString m_plotTitle;
  QRectF m_boundingRect;

  int pointStyleToQwtSymbolStyle(const AdjustPlotVisualsDialog::PointStyles ps) const;
  AdjustPlotVisualsDialog::PointStyles qwtSymbolStypeToPointStyle(const int qwtSymbol) const;
  void setAxisTitleFont(const int a, const QFont &f);
  void setZoomBase(const QRectF &rect);
  QRectF uniteBoundingRects();

signals:
  void pointSelected(const QPointF &point, const QPoint &cursor);

private slots:
  void onPointSelected(const QPointF &pos);

};

#endif // MODECONTEXT_H
