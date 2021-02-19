#ifndef COMMONPARAMETERSWIDGET_H
#define COMMONPARAMETERSWIDGET_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QPalette>
#include "../abstractmappermodel.h"
#include "../commonparametersitems.h"
#include "../floatingvaluedelegate.h"
#include "common/qmappedcheckbox.h"

class EvaluationEngineCommonParametersView;

namespace Ui {
class CommonParametersWidget;
}

class CommonParametersWidget : public QWidget
{
  Q_OBJECT
public:
  explicit CommonParametersWidget(QWidget *parent = nullptr);
  ~CommonParametersWidget();
  void connectToAccumulator(QObject *dac);
  void setCommonParametersBoolModel(AbstractMapperModel<bool, CommonParametersItems::Boolean> *model);
  void setCommonParametersNumModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model);
  void setEvaluationEngineCommonParametersView(EvaluationEngineCommonParametersView *eeView);

private:
  void markAsInvalid(QWidget *w, const bool invalid);

  Ui::CommonParametersWidget *ui;

  QDataWidgetMapper *m_commonParametersBoolMapper;
  QDataWidgetMapper *m_commonParametersNumMapper;
  FloatingValueDelegate m_commonParametersMapperDelegate;
  QPalette m_invalidPalette;
  EvaluationEngineCommonParametersView *m_eeView;

signals:
  void eofSourceChanged(const CommonParametersItems::EOFSource source);
  void readEof();

private slots:
  void onEofSourceChanged();
  void onNoEofClicked();
  void onReadEofClicked();

public slots:
  void onValidityState(const bool state, const CommonParametersItems::Floating item);

};

#endif // COMMONPARAMETERSWIDGET_H
