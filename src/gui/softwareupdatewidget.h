#ifndef SOFTWAREUPDATEWIDGET_H
#define SOFTWAREUPDATEWIDGET_H

#include <QWidget>

class SoftwareUpdateResult;

namespace Ui {
class SoftwareUpdateWidget;
}

class SoftwareUpdateWidget : public QWidget
{
  Q_OBJECT
public:
  enum class Result {
    FAILED,
    UP_TO_DATE,
    UPDATE_AVAILABLE
  };
  Q_ENUM(Result)

  explicit SoftwareUpdateWidget(QWidget *parent = nullptr);
  ~SoftwareUpdateWidget();
  void setDisplay(const SoftwareUpdateResult &result);

private:
  Ui::SoftwareUpdateWidget *ui;
};

#endif // SOFTWAREUPDATEWIDGET_H
