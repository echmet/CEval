#ifndef REGISTERINHYPERBOLEFITWIDGET_H
#define REGISTERINHYPERBOLEFITWIDGET_H

#include <QWidget>

namespace Ui {
class RegisterInHyperboleFitWidget;
}

class RegisterInHyperboleFitWidget : public QWidget
{
  Q_OBJECT

public:
  enum class MobilityFrom {
    HVL_A1,
    PEAK_MAXIMUM
  };
  Q_ENUM(MobilityFrom)

  explicit RegisterInHyperboleFitWidget(QWidget *parent = nullptr);
  ~RegisterInHyperboleFitWidget();
  MobilityFrom mobilityFrom() const;
  void setAnalyteText(const QString &text);
  void setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  void setMobilityText(const MobilityFrom m);

  Ui::RegisterInHyperboleFitWidget *ui;

  double m_hvlMobility;
  double m_peakMaxMobility;

private slots:
  void onMobilityFromActivated(const int idx);

};

#endif // REGISTERINHYPERBOLEFITWIDGET_H
