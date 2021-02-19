#include "commonparameterswidgetconnector.h"
#include "commonparametersengine.h"
#include "gui/commonparameterswidget.h"
#include "dataaccumulator.h"
#include "evaluationengine.h"

void CommonParametersWidgetConnector::connectAll(QObject *anonWidget, QObject *anonDac)
{
  CommonParametersWidget *w = qobject_cast<CommonParametersWidget *>(anonWidget);
  DataAccumulator *dac = qobject_cast<DataAccumulator *>(anonDac);
  EvaluationEngine *e;
  CommonParametersEngine *ce;

  Q_ASSERT(w != nullptr && dac != nullptr);
  e = dac->evaluationEngine();
  ce = dac->commonParametersEngine();

  w->setCommonParametersNumModel(dac->commonParametersEngine()->numModel());
  w->setCommonParametersBoolModel(dac->commonParametersEngine()->boolModel());
  w->setEvaluationEngineCommonParametersView(dac->commonParametersEngine()->evaluationEngineView());

  QObject::connect(ce, &CommonParametersEngine::validityState, w, &CommonParametersWidget::onValidityState);
  QObject::connect(w, &CommonParametersWidget::readEof, e, &EvaluationEngine::onReadEof);
  QObject::connect(ce, &CommonParametersEngine::noEofStateChanged, e, &EvaluationEngine::onNoEofStateChanged);
  QObject::connect(w, &CommonParametersWidget::eofSourceChanged, ce, &CommonParametersEngine::onEofSourceChanged);
}

