#include "qmappedcheckbox.h"

QMappedCheckBox::QMappedCheckBox(QWidget *parent) :
  QCheckBox(parent)
{
  connect(this, SIGNAL(clicked()), this, SLOT(dropFocus()));
}

void QMappedCheckBox::dropFocus()
{
  /*
   * Workaround for QDataWidgetMapper behavior
   * which will not update the model until the widget
   * looses focus.
   */
  setFocus();
  clearFocus();
}


