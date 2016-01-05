#include "maincontrolswidgetconnector.h"
#include "gui/maincontrolswidget.h"
#include "dataaccumulator.h"

void MainControlsWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  MainControlsWidget *w = qobject_cast<MainControlsWidget *>(anonWidget);

  Q_ASSERT(w != nullptr && dac != nullptr);

  QObject::connect(w, &MainControlsWidget::tabSwitched, dac, &DataAccumulator::onTabSwitched);
}
