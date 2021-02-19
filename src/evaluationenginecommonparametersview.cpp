#include "evaluationenginecommonparametersview.h"
#include "evaluationengine.h"

#include <cassert>

EvaluationEngineCommonParametersView::EvaluationEngineCommonParametersView(EvaluationEngine *ee) :
  m_ee(ee)
{
}

QVector<QPair<DataHash, QPair<QString, QString>>> EvaluationEngineCommonParametersView::dataContexts() const
{
  const auto &ctxMap = m_ee->allDataContexts();

  QVector<QPair<DataHash, QPair<QString, QString>>> contextsList{};
  contextsList.reserve(ctxMap.size());
  for (auto it = ctxMap.cbegin(); it != ctxMap.cend(); it++) {
    /* I have a question - this is horrible! */
    QPair<QString, QString> desc(it.value()->name, it.value()->path);
    contextsList.append(QPair<DataHash, QPair<QString, QString>>(it.key(), std::move(desc)));
  }

  return contextsList;
}

const CommonParametersEngine::Context &EvaluationEngineCommonParametersView::parameters(const DataHash &hash)
{
  const auto &ctxMap = m_ee->allDataContexts();
  auto it = ctxMap.find(hash);
  assert(it != ctxMap.cend());

  return it.value()->commonContext;
}

void EvaluationEngineCommonParametersView::updateParametersForContexts(const CommonParametersEngine::Context &params, const QVector<DataHash> &toUpdate)
{
  for (const auto &hash : toUpdate)
    m_ee->updateCommonParameters(params, hash);
}
