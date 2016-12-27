#ifndef REGISTERINHYPERBOLAFITWIDGET_H
#define REGISTERINHYPERBOLAFITWIDGET_H

#include <QWidget>

namespace Ui {
class RegisterInHyperbolaFitWidget;
}

class RegisterInHyperbolaFitWidget : public QWidget
{
  Q_OBJECT

public:
  enum class MobilityFrom {
    HVL_A1,
    PEAK_MAXIMUM
  };
  Q_ENUM(MobilityFrom)

  explicit RegisterInHyperbolaFitWidget(QWidget *parent = nullptr);
  ~RegisterInHyperbolaFitWidget();
  MobilityFrom mobilityFrom() const;
  void setAnalyteText(const QString &text);
  void setInformation(const QString &analyte, const double selConcentration, const double hvlMobility, const double peakMaxMobility);

private:
  void setMobilityText(const MobilityFrom m);

  Ui::RegisterInHyperbolaFitWidget *ui;

  double m_hvlMobility;
  double m_peakMaxMobility;

private slots:
  void onMobilityFromActivated(const int idx);

};

#endif // REGISTERINHYPERBOLAFITWIDGET_H
