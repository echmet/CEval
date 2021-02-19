#ifndef EVALUATIONENGINECOMMONPARAMETERSVIEW_H
#define EVALUATIONENGINECOMMONPARAMETERSVIEW_H

#include "commonparametersengine.h"
#include "datahash.h"

#include <QPair>
#include <QString>
#include <QVector>

class EvaluationEngine;

class EvaluationEngineCommonParametersView {
public:
  EvaluationEngineCommonParametersView(EvaluationEngine *ee);

  QVector<QPair<DataHash, QPair<QString, QString>>> dataContexts() const;
  const CommonParametersEngine::Context &parameters(const DataHash &hash);
  void updateParametersForContexts(const CommonParametersEngine::Context &params, const QVector<DataHash> &toUpdate);

private:
  EvaluationEngine *m_ee;

};

#endif // EVALUATIONENGINECOMMONPARAMETERSVIEW_H
