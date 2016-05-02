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
  explicit SoftwareUpdateWidget(QWidget *parent = 0);
  ~SoftwareUpdateWidget();

private:
  Ui::SoftwareUpdateWidget *ui;
};

#endif // SOFTWAREUPDATEWIDGET_H
