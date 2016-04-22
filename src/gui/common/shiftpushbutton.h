#ifndef SHIFTPUSHBUTTON_H
#define SHIFTPUSHBUTTON_H

#include <QPushButton>

class ShiftPushButton : public QPushButton
{
  Q_OBJECT
public:
  ShiftPushButton(QWidget *parent = nullptr);

protected:
  void mouseReleaseEvent(QMouseEvent *evt);

signals:
  void modifierClicked();
};

#endif // SHIFTPUSHBUTTON_H
