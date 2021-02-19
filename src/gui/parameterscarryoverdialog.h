#ifndef PARAMETERSCARRYOVERDIALOG_H
#define PARAMETERSCARRYOVERDIALOG_H

#include "../commonparametersengine.h"
#include "../datahash.h"

#include <QDialog>

class EvaluationEngineCommonParametersView;

namespace Ui {
class ParametersCarryOverDialog;
}

class ParametersCarryOverDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ParametersCarryOverDialog(EvaluationEngineCommonParametersView *eeView, QWidget *parent = nullptr);
  ~ParametersCarryOverDialog();

private:
  const CommonParametersEngine::Context &getParameters(const QModelIndex &idx);

  Ui::ParametersCarryOverDialog *ui;
  EvaluationEngineCommonParametersView *m_eeView;

private slots:
  void onApplyClicked();
  void onApplyToSelected(const QModelIndex &idx, const QModelIndex & /*unused*/);
  void onAvailableSelected(const QModelIndex &idx, const QModelIndex & /*unused */);
};

#endif // PARAMETERSCARRYOVERDIALOG_H
