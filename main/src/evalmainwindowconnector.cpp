#include "evalmainwindowconnector.h"
#include "gui/evalmainwindow.h"
#include "dataaccumulator.h"
#include "efg/efgloaderinterface.h"

void EvalMainWindowConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  const EvalMainWindow *w = qobject_cast<EvalMainWindow *>(anonWidget);
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);

  Q_ASSERT(w != nullptr && dac != nullptr);

  QObject::connect(w, &EvalMainWindow::adjustPlotAppearance, dac, &DataAccumulator::onAdjustPlotAppearance);
  QObject::connect(w, &EvalMainWindow::exportAction, dac, &DataAccumulator::onExportAction);
  QObject::connect(&EFGLoaderInterface::instance(), &EFGLoaderInterface::supportedFileFormatsRetrieved, w, &EvalMainWindow::onSupportedFileFormatsRetrieved, Qt::QueuedConnection);
  QObject::connect(dac, &DataAccumulator::programModeChanged, w, &EvalMainWindow::onProgramModeChanged);
}

