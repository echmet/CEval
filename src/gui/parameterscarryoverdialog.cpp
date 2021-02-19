#include "parameterscarryoverdialog.h"
#include "ui_parameterscarryoverdialog.h"
#include "../commonparametersitems.h"
#include "../doubletostringconvertor.h"
#include "../evaluationenginecommonparametersview.h"

#include <QPushButton>
#include <QStandardItemModel>

#include <algorithm>
#include <cassert>

ParametersCarryOverDialog::ParametersCarryOverDialog(EvaluationEngineCommonParametersView *eeView, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ParametersCarryOverDialog),
  m_eeView(eeView)
{
  assert(eeView != nullptr);

  ui->setupUi(this);

  ui->qlv_applyToDatasets->setSelectionMode(QAbstractItemView::MultiSelection);
  ui->qlv_availableDatasets->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->qlv_applyToDatasets->setEditTriggers(QAbstractItemView::NoEditTriggers);

  auto contexts = eeView->dataContexts();
  using Ctx = std::decay<decltype(contexts.first())>::type;
  std::sort(contexts.begin(), contexts.end(),
            [](const Ctx &left, const Ctx &right) {
    return left.second.second < right.second.second;
  });

  auto availableModel = new QStandardItemModel(this);
  auto applyToModel = new QStandardItemModel(this);
  for (const auto &item : contexts) {
    auto si = new QStandardItem(item.second.first);
    auto si2 = new QStandardItem(item.second.first);

    auto v = QVariant::fromValue(item.first);
    si->setData(v);
    si->setData(item.second.second, Qt::UserRole + 2);
    si2->setData(v);
    si2->setData(item.second.second, Qt::UserRole + 2);

    availableModel->appendRow(si);
    applyToModel->appendRow(si2);
  }
  ui->qlv_availableDatasets->setModel(availableModel);
  ui->qlv_applyToDatasets->setModel(applyToModel);

  ui->ql_availablePath->setText("");
  ui->ql_applyToPath->setText("");

  connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ParametersCarryOverDialog::onApplyClicked);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ParametersCarryOverDialog::reject);
  connect(ui->qlv_applyToDatasets->selectionModel(), &QItemSelectionModel::currentChanged, this, &ParametersCarryOverDialog::onApplyToSelected);
  connect(ui->qlv_availableDatasets->selectionModel(), &QItemSelectionModel::currentChanged, this, &ParametersCarryOverDialog::onAvailableSelected);
}

ParametersCarryOverDialog::~ParametersCarryOverDialog()
{
    delete ui;
}

const CommonParametersEngine::Context &ParametersCarryOverDialog::getParameters(const QModelIndex &idx)
{
  auto v = idx.data(Qt::UserRole + 1);
  assert(!v.isNull());

  return m_eeView->parameters(qvariant_cast<DataHash>(v));
}

void ParametersCarryOverDialog::onApplyClicked()
{
  const auto sel = ui->qlv_availableDatasets->selectionModel()->selectedIndexes();
  if (sel.empty())
    return;

  const auto &params = getParameters(sel.constFirst());
  const auto srcHash = qvariant_cast<DataHash>(sel.constFirst().data(Qt::UserRole + 1));

  QVector<DataHash>  toUpdate{};
  for (const auto &idx : ui->qlv_applyToDatasets->selectionModel()->selectedIndexes()) {
    auto v = idx.data(Qt::UserRole + 1);
    assert(!v.isNull());
    const auto hash = qvariant_cast<DataHash>(v);

    if (hash == srcHash)
      continue;

    toUpdate.append(qvariant_cast<DataHash>(v));
  }

  m_eeView->updateParametersForContexts(params, toUpdate);
}

void ParametersCarryOverDialog::onApplyToSelected(const QModelIndex &idx, const QModelIndex & /*unused*/)
{
  const auto path = idx.data(Qt::UserRole + 2).toString();

  ui->ql_applyToPath->setText(path);
}

void ParametersCarryOverDialog::onAvailableSelected(const QModelIndex &idx, const QModelIndex & /*unused*/)
{
  const auto &params = getParameters(idx);
  const auto path = idx.data(Qt::UserRole + 2).toString();

  ui->qle_capillary->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::CAPILLARY)));
  ui->qle_detector->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::DETECTOR)));
  ui->qle_voltage->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::VOLTAGE)));
  ui->qle_fieldStrength->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::FIELD)));
  ui->qle_cSelector->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::SELECTOR)));

  const bool noEof = params.boolData.at(CommonParametersItems::Boolean::NO_EOF);
  if (noEof)
    ui->qle_tEof->setText(tr("No EOF"));
  else
    ui->qle_tEof->setText(DoubleToStringConvertor::convert(params.numData.at(CommonParametersItems::Floating::T_EOF)));

  ui->ql_availablePath->setText(path);
}
