#include "plotexporter.h"
#include <cmath>
#include <QApplication>
#include <QDesktopWidget>
#include <QImageWriter>
#include <QMessageBox>
#include <qwt_plot.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_widget.h>

PlotExporter::PlotExporter(QObject *parent) : QObject(parent)
{
  for (const QByteArray &a : QImageWriter::supportedImageFormats())
    m_supportedFormats.push_back(QString(a));

  /* Add formats supported by QwtPlotRenderer */
  m_supportedFormats << "pdf" << "svg" << "ps";

  m_exportDlg = new ExportPlotToImageDialog(m_supportedFormats);

  m_plotPalette.setColor(QPalette::WindowText, Qt::black);
  m_plotPalette.setColor(QPalette::Foreground, Qt::black);
  m_plotPalette.setColor(QPalette::Text, Qt::black);
}

PlotExporter::~PlotExporter()
{
  delete m_exportDlg;
}

void PlotExporter::exportPlot(QwtPlot *plot, const QRectF &zoom)
{
  QSizeF guessedDimensions;
  ExportPlotToImageDialog::Parameters p = m_exportDlg->parameters();

  m_exportDlg->setAspectRatio(plot->size().width() / plot->size().height());

  while (m_exportDlg->exec() == QDialog::Accepted) {
    p = m_exportDlg->parameters();

    QString path;

    if (p.path.length() < 0) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid path"));
      continue;
    }
    if (!m_supportedFormats.contains(p.format)) {
      QMessageBox::warning(nullptr, QObject::tr("Invalid input"), QObject::tr("Invalid output format"));
      continue;
    }

    if (p.path.endsWith("." + p.format))
      path = p.path;
    else
      path = p.path + "." + p.format;

    /* Create a temporary QwtPlot to use to write the chart to file */
    QwtPlot exPlot;
    QwtPlotZoomer exPlorZoomer(exPlot.canvas());
    exPlorZoomer.zoom(zoom);

    exPlot.setCanvasBackground(QBrush(Qt::white));
    exPlot.setTitle(plot->title());
    exPlot.setAxisTitle(QwtPlot::xBottom, plot->axisTitle(QwtPlot::xBottom));
    exPlot.setAxisTitle(QwtPlot::xTop, plot->axisTitle(QwtPlot::xTop));
    exPlot.setAxisTitle(QwtPlot::yLeft, plot->axisTitle(QwtPlot::yLeft));
    exPlot.setAxisTitle(QwtPlot::yRight, plot->axisTitle(QwtPlot::yRight));
    QwtPlotItemList curves = plot->itemList();

    /* Attach all plots from the GUI plot to the temporary plot
     * Note that this will detach the plots from the GUI plot! */
    for (QwtPlotItem *i : curves)
      i->attach(&exPlot);

    /* Scale up from millimeters to centimeters*/
    QSizeF dimensionsMM(p.dimensions.width() * 10.0, p.dimensions.height() * 10.0);

    /* Store current properties of the plot as we need to change them for rendering */
    QFont xBottomFont = plot->axisWidget(QwtPlot::xBottom)->font();
    QFont xTopFont = plot->axisWidget(QwtPlot::xTop)->font();
    QFont yLeftFont = plot->axisWidget(QwtPlot::yLeft)->font();
    QFont yRightFont = plot->axisWidget(QwtPlot::yRight)->font();
    QFont xBottomTitleFont = plot->axisTitle(QwtPlot::xBottom).font();
    QFont xTopTitleFont = plot->axisTitle(QwtPlot::xTop).font();
    QFont yLeftTitleFont = plot->axisTitle(QwtPlot::yLeft).font();
    QFont yRightTitleFont = plot->axisTitle(QwtPlot::yRight).font();
    QFont titleFont = plot->title().font();
    const double xBottomPenWidth = plot->axisWidget(QwtPlot::xBottom)->scaleDraw()->penWidth() > 0 ? plot->axisWidget(QwtPlot::xBottom)->scaleDraw()->penWidth() : 1.0;
    const double xTopPenWidth = plot->axisWidget(QwtPlot::xTop)->scaleDraw()->penWidth() > 0 ? plot->axisWidget(QwtPlot::xTop)->scaleDraw()->penWidth() : 1.0;
    const double yLeftPenWidth = plot->axisWidget(QwtPlot::yLeft)->scaleDraw()->penWidth() > 0 ? plot->axisWidget(QwtPlot::yLeft)->scaleDraw()->penWidth() : 1.0;
    const double yRightPenWidth = plot->axisWidget(QwtPlot::yRight)->scaleDraw()->penWidth() > 0 ? plot->axisWidget(QwtPlot::yRight)->scaleDraw()->penWidth() : 1.0;

    /* Recalculate sizes by the DPI for every element that needs it */
    const double outputInPixels = (static_cast<double>(p.dimensions.width()) / 2.54) * p.dpi;
    const double scalingRatio = (static_cast<double>(qApp->desktop()->logicalDpiX()) / p.dpi) * (outputInPixels / plot->geometry().width());

    const double _xBottomPenWidth = floor((xBottomPenWidth * scalingRatio) + 0.45);
    const double _xTopPenWidth = floor((xTopPenWidth * scalingRatio) + 0.45);
    const double _yLeftPenWidth = floor((yLeftPenWidth * scalingRatio) + 0.45);
    const double _yRightPenWidth = floor((yRightPenWidth * scalingRatio) + 0.45);
    xBottomFont.setPointSizeF(p.axisNumbersFontSize * scalingRatio);
    xTopFont.setPointSizeF(p.axisNumbersFontSize * scalingRatio);
    yLeftFont.setPointSizeF(p.axisNumbersFontSize * scalingRatio);
    yRightFont.setPointSizeF(p.axisNumbersFontSize * scalingRatio);
    xBottomTitleFont.setPointSizeF(p.axisTitlesFontSize * scalingRatio);
    xTopTitleFont.setPointSizeF(p.axisTitlesFontSize * scalingRatio);
    yLeftTitleFont.setPointSizeF(p.axisTitlesFontSize * scalingRatio);
    yRightTitleFont.setPointSizeF(p.axisTitlesFontSize * scalingRatio);
    titleFont.setPointSizeF(p.chartTitleFontSize * scalingRatio);
    exPlot.axisWidget(QwtPlot::xBottom)->scaleDraw()->setPenWidth(_xBottomPenWidth);
    exPlot.axisWidget(QwtPlot::xTop)->scaleDraw()->setPenWidth(_xTopPenWidth);
    exPlot.axisWidget(QwtPlot::yLeft)->scaleDraw()->setPenWidth(_yLeftPenWidth);
    exPlot.axisWidget(QwtPlot::yRight)->scaleDraw()->setPenWidth(_yRightPenWidth);

    exPlot.setPalette(m_plotPalette);
    exPlot.axisWidget(QwtPlot::xBottom)->setPalette(m_plotPalette);
    exPlot.axisWidget(QwtPlot::xTop)->setPalette(m_plotPalette);
    exPlot.axisWidget(QwtPlot::yLeft)->setPalette(m_plotPalette);
    exPlot.axisWidget(QwtPlot::yRight)->setPalette(m_plotPalette);
    exPlot.axisWidget(QwtPlot::xBottom)->setFont(xBottomFont);
    exPlot.axisWidget(QwtPlot::xTop)->setFont(xTopFont);
    exPlot.axisWidget(QwtPlot::yLeft)->setFont(yLeftFont);
    exPlot.axisWidget(QwtPlot::yRight)->setFont(yRightFont);
    setAxisTitleFont(&exPlot, QwtPlot::xBottom, xBottomTitleFont);
    setAxisTitleFont(&exPlot, QwtPlot::xTop, xTopTitleFont);
    setAxisTitleFont(&exPlot, QwtPlot::yLeft, yLeftTitleFont);
    setAxisTitleFont(&exPlot, QwtPlot::yRight, yRightTitleFont);
    setTitleFont(&exPlot, titleFont);

    exPlot.replot();

    renderPlotToFile(&exPlot, path, p.format, dimensionsMM, p.dpi);

    /* Reattach the plots back to the GUI plot */
    for (QwtPlotItem *i : curves)
      i->attach(plot);

    break; /* Exit the while loop */
  }
}

void PlotExporter::guessPlotDimensions(const QwtPlot *plot, const double currentWidth, QSizeF &dimensions)
{
  const QRect &plotDims = plot->geometry();
  const double plotRatio = static_cast<double>(plotDims.width()) / static_cast<double>(plotDims.height());

  dimensions.setWidth(currentWidth);
  dimensions.setHeight(currentWidth / plotRatio);
}

void PlotExporter::renderPlotToFile(QwtPlot *plot, const QString &path, const QString &format,
                                    const QSizeF &dimensions, const int dpi)
{
  QwtPlotRenderer renderer(this);

  renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
  renderer.setDiscardFlag(QwtPlotRenderer::DiscardFooter, true);
  renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame, true);
  renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales, true);

  renderer.renderDocument(plot, path, format, dimensions, dpi);
}


void PlotExporter::setAxisTitleFont(QwtPlot *plot, const int a, const QFont &f)
{
  QwtText t = plot->axisTitle(a);
  t.setFont(f);
  plot->setAxisTitle(a, t);
}

void PlotExporter::setTitleFont(QwtPlot *plot, const QFont &f)
{
  QwtText t = plot->title();
  t.setFont(f);
  plot->setTitle(t);
}
