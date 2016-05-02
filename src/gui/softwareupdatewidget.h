#ifndef SOFTWAREUPDATEWIDGET_H
#define SOFTWAREUPDATEWIDGET_H

#include <QWidget>

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
  void setDisplay(const Result result, const QString &versionTag, const QString &downloadLink);

private:
  Ui::SoftwareUpdateWidget *ui;
};

#endif // SOFTWAREUPDATEWIDGET_H
