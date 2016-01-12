#ifndef ADJUSTPLOTVISUALSDIALOG_H
#define ADJUSTPLOTVISUALSDIALOG_H

#include <QDialog>
#include "../serieproperties.h"

namespace Ui {
class AdjustPlotVisualsDialog;
}

class QComboBox;
class QStandardItem;

class AdjustPlotVisualsDialog : public QDialog
{
  Q_OBJECT
public:
  enum class PointStyles : int {
    NO_SYMBOL,
    ELLIPSE,
    RECT,
    DIAMOND,
    TRIANGLE,
    D_TRIANGLE,
    U_TRIANGLE,
    L_TRIANGLE,
    R_TRIANGLE,
    CROSS,
    X_CROSS,
    H_LINE,
    V_LINE,
    STAR_A,
    STAR_B,
    HEXAGON,
    LAST_INDEX
  };
  Q_ENUM(PointStyles)

  class AxisVisuals {
  public:
    AxisVisuals(const SerieProperties::Axis a = SerieProperties::Axis::INVALID);
    void fromOther(const AxisVisuals &other);

    const SerieProperties::Axis a;
    int fontSize;
    bool bold;

    AxisVisuals &operator=(const AxisVisuals &other);
  };

  class SerieVisuals {
  public:
    SerieVisuals(const int id = -1);
    void fromOther(const SerieVisuals &other);

    const int id;
    QColor lineColor;
    qreal lineThickness;
    QColor pointColor;
    QColor pointFillColor;
    int pointSize;
    qreal pointLineThickness;
    PointStyles pointStyle;

    SerieVisuals &operator=(const SerieVisuals &other);
  };

  explicit AdjustPlotVisualsDialog(QWidget *parent = nullptr);
  ~AdjustPlotVisualsDialog();

  void addAxisVisuals(const QString &title, const AxisVisuals &av);
  void addSerieVisuals(const QString &title, const SerieVisuals &sv);
  QVector<AxisVisuals> axisVisuals() const;
  QVector<SerieVisuals> serieVisuals() const;

  static QString pointStyleName(const PointStyles ps);

private:
  Ui::AdjustPlotVisualsDialog *ui;

  QString backgroundColorToStyleSheet(const QColor &c) const;
  QStandardItem *comboBoxItem(const QComboBox *qcbox, const int idx) const;
  template<typename T> T datatypeFromItem(const QStandardItem *item) const;
  void fillPointStylesComboBox();
  void setLineColorBox(const QColor &c);
  void setPointColorBox(const QColor &c);
  void setPointStyleIndex(const PointStyles ps);
  template<typename T> QVector<T> vectorOfVisuals(const QComboBox *qcbox) const;


private slots:
  void onAxisFontBoldChanged(const int c);
  void onAxisFontSizeChanged(const int s);
  void onAxisSelected(const int idx);
  void onCancelClicked();
  void onOkClicked();
  void onPickLineColorClicked();
  void onPickPointColorClicked();
  void onPickPointFillColorClicked();
  void onLineThicknessChanged(const double t);
  void onPointLineThicknessChanges(const double t);
  void onPointSizeChanged(const int t);
  void onPointStyleSelected(const int idx);
  void onSerieSelected(const int idx);
  void onSetForAllAxesClicked();

};
Q_DECLARE_METATYPE(AdjustPlotVisualsDialog::AxisVisuals)
Q_DECLARE_METATYPE(AdjustPlotVisualsDialog::SerieVisuals)

#endif // ADJUSTPLOTVISUALSDIALOG_H
