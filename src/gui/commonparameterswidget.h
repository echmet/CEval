#ifndef COMMONPARAMETERSWIDGET_H
#define COMMONPARAMETERSWIDGET_H

#include <QWidget>
#include <QDataWidgetMapper>
#include <QPalette>
#include "../abstractmappermodel.h"
#include "../commonparametersitems.h"
#include "../floatingvaluedelegate.h"


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
  void setCommonParametersModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model);

private:
  void markAsInvalid(QWidget *w, const bool invalid);

  Ui::CommonParametersWidget *ui;

  QDataWidgetMapper *m_commonParametersMapper;
  FloatingValueDelegate m_commonParametersMapperDelegate;
  QPalette m_invalidPalette;

signals:
  void readEof();

private slots:
  void onReadEofClicked();

public slots:
  void onValidityState(const bool state, const CommonParametersItems::Floating item);

};

#endif // COMMONPARAMETERSWIDGET_H
