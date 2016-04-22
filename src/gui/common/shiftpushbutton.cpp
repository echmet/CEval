#include "shiftpushbutton.h"
#include <QMouseEvent>

ShiftPushButton::ShiftPushButton(QWidget *parent) : QPushButton(parent)
{
}

void ShiftPushButton::mouseReleaseEvent(QMouseEvent *evt)
{
  if (evt->modifiers() == Qt::ShiftModifier) {
    emit modifierClicked();
    return;
  }

  QPushButton::mouseReleaseEvent(evt);
}
