#include "evaluationupperwidgetconnector.h"
#include "gui/evalmainwindow.h"
#include "dataaccumulator.h"
#include "helpers.h"

void EvaluationUpperWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  EvaluationUpperWidget *w = qobject_cast<EvaluationUpperWidget *>(anonWidget);
  const DataAccumulator *dac = qobject_cast<const DataAccumulator *>(anonDac);
  EvaluationEngine *e;

  Q_ASSERT(w != nullptr && dac != nullptr);

  e = dac->evaluationEngine();

  w->setEvaluationLoadedFilesModel(e->loadedFilesModel());

  QObject::connect(w, &EvaluationUpperWidget::fileSwitched, e, &EvaluationEngine::onEvaluationFileSwitched);
  QObject::connect(e, &EvaluationEngine::evaluationFileAdded, w, &EvaluationUpperWidget::onFileSwitched);
  QObject::connect(e, &EvaluationEngine::evaluationFileSwitched, w, &EvaluationUpperWidget::onFileSwitched);
  QObject::connect(w, &EvaluationUpperWidget::closeAllFiles, e, &EvaluationEngine::onCloseAllEvaluationFiles);
  QObject::connect(w, &EvaluationUpperWidget::closeFile, e, &EvaluationEngine::onCloseCurrentEvaluationFile);
  QObject::connect(e, &EvaluationEngine::comboBoxIndexChanged, w, &EvaluationUpperWidget::onComboBoxChangedExt);
  QObject::connect(w, &EvaluationUpperWidget::traverseFiles, e, &EvaluationEngine::onTraverseFiles);
}
