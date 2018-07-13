#ifndef COMMONPARAMETERSWIDGET_H
#define COMMONPARAMETERSWIDGET_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QPalette>
#include "../abstractmappermodel.h"
#include "../commonparametersitems.h"
#include "../floatingvaluedelegate.h"
#include "common/qmappedcheckbox.h"


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

private:
  void markAsInvalid(QWidget *w, const bool invalid);

  Ui::CommonParametersWidget *ui;

  QDataWidgetMapper *m_commonParametersBoolMapper;
  QDataWidgetMapper *m_commonParametersNumMapper;
  FloatingValueDelegate m_commonParametersMapperDelegate;
  QPalette m_invalidPalette;

signals:
  void readEof();

private slots:
  void onNoEofClicked();
  void onReadEofClicked();

public slots:
  void onValidityState(const bool state, const CommonParametersItems::Floating item);

};

#endif // COMMONPARAMETERSWIDGET_H
