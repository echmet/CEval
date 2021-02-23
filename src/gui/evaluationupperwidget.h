#ifndef EVALUATIONUPPERWIDGET_H
#define EVALUATIONUPPERWIDGET_H

#include <QSplitter>
#include <QWidget>
#include "commonparameterswidget.h"
#include "evaluatedpeakswidget.h"
#include "evaluationwidget.h"

namespace Ui {
class EvaluationUpperWidget;
}

class EvaluationUpperWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EvaluationUpperWidget(QWidget *parent = nullptr);
  ~EvaluationUpperWidget();
  void connectToAccumulator(QObject *dac);
  void setCommonParametersBoolModel(AbstractMapperModel<bool, CommonParametersItems::Boolean> *model);
  void setCommonParametersNumModel(AbstractMapperModel<double, CommonParametersItems::Floating> *model);
  void setDefaultState();
  void setEvaluationLoadedFilesModel(QAbstractItemModel *model);

private:
  Ui::EvaluationUpperWidget *ui;

  QSplitter *m_splitter;
  CommonParametersWidget *m_commonParametersWidget;
  EvaluatedPeaksWidget *m_evaluatedPeaksWidget;
  EvaluationWidget *m_evaluationWidget;

signals:
  void closeFile(const int idx);
  void fileSwitched(const int idx);
  void traverseFiles(const EvaluationEngineMsgs::Traverse dir);

public slots:
  void onComboBoxChangedExt(const EvaluationEngineMsgs::ComboBoxNotifier notifier);
  void onFileSwitched(const int idx);

private slots:
  void onCloseAllFilesClicked();
  void onCloseFileClicked();
  void onFileComboBoxChanged(const int idx);
  void onNextFileClicked();
  void onPreviousFileClicked();

};

#endif // EVALUATIONUPPERWIDGET_H
