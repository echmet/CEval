#include "schemeeditor.h"
#include "ui_schemeeditor.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

using namespace DataExporter;

SchemeEditor::SchemeBase::SchemeBase() :
  name(""),
  description(""),
  exportables(QStringVector())
{
}

SchemeEditor::SchemeBase::SchemeBase(const QString &name, const QString &description, const QStringVector &exportables) :
  name(name),
  description(description),
  exportables(exportables)
{
}

SchemeEditor::UserScheme::UserScheme() :
  name(""),
  baseName(""),
  exportables(QStringVector()),
  arrangement(Globals::DataArrangement::VERTICAL),
  isValid(false)
{
}

SchemeEditor::UserScheme::UserScheme(const QString &name, const QString &baseName, const QStringVector &exportables, const Globals::DataArrangement arrangement) :
  name(name),
  baseName(baseName),
  exportables(exportables),
  arrangement(arrangement),
  isValid(true)
{
}

SchemeEditor::UserScheme & SchemeEditor::UserScheme::operator=(const UserScheme &other)
{
  const_cast<QString&>(name) = other.name;
  const_cast<QString&>(baseName) = other.baseName;
  const_cast<QStringVector&>(exportables) = other.exportables;
  const_cast<Globals::DataArrangement&>(arrangement) = other.arrangement;
  const_cast<bool&>(isValid) = other.isValid;

  return *this;
}

SchemeEditor::SchemeEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SchemeEditor)
{
  ui->setupUi(this);

  m_avaliableExportablesModel = new QStandardItemModel(this);
  m_selectedExportablesModel = new QStandardItemModel(this);

  ui->qlv_availableExportables->setModel(m_avaliableExportablesModel);
  ui->qlv_selectedExportables->setModel(m_selectedExportablesModel);

  ui->qcbox_dataArrangement->addItem("Vertical", QVariant::fromValue<Globals::DataArrangement>(Globals::DataArrangement::VERTICAL));
  ui->qcbox_dataArrangement->addItem("Horizontal", QVariant::fromValue<Globals::DataArrangement>(Globals::DataArrangement::HORIZONTAL));

  ui->qlv_availableExportables->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->qlv_selectedExportables->setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(ui->qpb_addExportable, &QPushButton::clicked, this, &SchemeEditor::onAddExportableClicked);
  connect(ui->qpb_removeExportable, &QPushButton::clicked, this, &SchemeEditor::onRemoveExportableClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &SchemeEditor::onCancelClicked);
  connect(ui->qpb_create, &QPushButton::clicked, this, &SchemeEditor::onCreateClicked);
  connect(ui->qcbox_availableSchemeBases, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &SchemeEditor::onSchemeChanged);
  connect(ui->qlv_availableExportables, &QListView::doubleClicked, this, &SchemeEditor::onAddExportableClicked);
  connect(ui->qlv_selectedExportables, &QListView::doubleClicked, this, &SchemeEditor::onRemoveExportableClicked);
}

SchemeEditor::~SchemeEditor()
{
  delete ui;
}

void SchemeEditor::addExportable(const int row)
{
  QList<QStandardItem *> itemList = m_avaliableExportablesModel->takeRow(row);
  if (itemList.size() < 1)
    return;

  m_selectedExportablesModel->appendRow(itemList.at(0));
}

SchemeEditor::UserScheme SchemeEditor::interact(bool &canceled)
{
  ui->qpb_create->setText(tr("Create"));

  return interactInternal(canceled);
}

SchemeEditor::UserScheme SchemeEditor::interact(const UserScheme &scheme, bool &canceled)
{
  resetForm();

  ui->qpb_create->setText(tr("Confim"));

  /* Set base scheme */
  {
    const QAbstractItemModel *model = ui->qcbox_availableSchemeBases->model();

    int idx;
    for (idx = 0; idx < model->rowCount(); idx++) {
      const QVariant &var = model->data(model->index(idx, 0), Qt::UserRole);

      if (var.canConvert<SchemeBase>()) {
        const SchemeBase b = var.value<SchemeBase>();

        if (scheme.baseName == b.name) {
          ui->qcbox_availableSchemeBases->setCurrentIndex(idx);
          onSchemeChanged(idx);
          break;
        }
      }
    }
    if (idx == model->rowCount())
      return UserScheme();
  }

  /* Set selected exporables list */
  {
    const QAbstractItemModel *model = m_avaliableExportablesModel; /* Alias */

    for (const QString &s : scheme.exportables) {
      for (int idx = 0; idx < model->rowCount(); idx++) {
        const QVariant &var = model->data(model->index(idx, 0), Qt::UserRole);

        if (var.toString() == s)
          addExportable(idx);
      }
    }
  }

  /* Set data arrangement */
  {
    const QAbstractItemModel *model = ui->qcbox_dataArrangement->model();

    for (int idx = 0; idx < model->rowCount(); idx++) {
      const QVariant &var = model->data(model->index(idx, 0), Qt::UserRole);

      if (var.canConvert<Globals::DataArrangement>()) {
        Globals::DataArrangement arr = var.value<Globals::DataArrangement>();

        if (arr == scheme.arrangement)
          ui->qcbox_dataArrangement->setCurrentIndex(idx);
      }
    }
  }

  m_currentSchemeName = scheme.name;
  return interactInternal(canceled);
}

SchemeEditor::UserScheme SchemeEditor::interactInternal(bool &canceled)
{
  QStringVector selected;

  while (true) {
    int ret = this->exec();
    if (ret != QDialog::Accepted) {
      canceled = true;
      return UserScheme();
    }
    canceled = false;

    const QVariant base = ui->qcbox_availableSchemeBases->currentData(Qt::UserRole);

    if (!base.canConvert<SchemeBase>())
      return UserScheme();
    const QString baseName = base.value<SchemeBase>().name;

    if (m_selectedExportablesModel->rowCount() < 1)
      continue;
    for (int idx = 0; idx < m_selectedExportablesModel->rowCount(); idx++) {
      const QStandardItem *item = m_selectedExportablesModel->item(idx, 0);

      if (item == nullptr)
        continue;

      selected << item->data(Qt::UserRole).toString();
    }

    QVariant var = ui->qcbox_dataArrangement->currentData(Qt::UserRole);
    if (!var.canConvert<Globals::DataArrangement>())
      return UserScheme();

    Globals::DataArrangement arr = var.value<Globals::DataArrangement>();

    return UserScheme(m_schemeName, baseName, selected, arr);
  }
}

void SchemeEditor::onAddExportableClicked()
{
  const QModelIndex &idx = ui->qlv_availableExportables->currentIndex();
  if (!idx.isValid())
    return;

  addExportable(idx.row());
}

void SchemeEditor::onCancelClicked()
{
  reject();
}

void SchemeEditor::onCreateClicked()
{
  QInputDialog dlg;

  dlg.setLabelText("Enter name for the scheme");
  dlg.setInputMode(QInputDialog::TextInput);
  dlg.setTextValue(m_currentSchemeName);

  if (dlg.exec() != QDialog::Accepted)
    return;

  m_schemeName = dlg.textValue();
  accept();
}

void SchemeEditor::onRemoveExportableClicked()
{
  const QModelIndex &idx = ui->qlv_selectedExportables->currentIndex();
  if (!idx.isValid())
    return;

  QList<QStandardItem *> itemList = m_selectedExportablesModel->takeRow(idx.row());
  if (itemList.size() < 1)
    return;

  bool ok;
  int origRow = itemList.at(0)->data(Qt::UserRole + 1).toInt(&ok);
  if (!ok)
    m_avaliableExportablesModel->appendRow(itemList);

  for (int idx = 0; idx < m_avaliableExportablesModel->rowCount(); idx++) {
    QStandardItem *item = m_avaliableExportablesModel->item(idx);

    int itemRow = item->data(Qt::UserRole + 1).toInt();
    if (itemRow >= origRow) {
      m_avaliableExportablesModel->insertRow(idx, itemList);
      return;
    }
  }

  m_avaliableExportablesModel->appendRow(itemList);
}

void SchemeEditor::onSchemeChanged(const int idx)
{
  m_selectedExportablesModel->clear();
  m_avaliableExportablesModel->clear();

  QVariant v = ui->qcbox_availableSchemeBases->itemData(idx, Qt::UserRole);
  SchemeBase base = v.value<SchemeBase>();

  ui->qpte_descriptionText->setPlainText(base.description);

  int posCounter = 0;
  for (const QString &s : base.exportables) {
    QStandardItem *item = new QStandardItem(s);
    item->setData(s, Qt::UserRole);
    item->setData(posCounter, Qt::UserRole + 1);

    m_avaliableExportablesModel->appendRow(item);
    posCounter++;
  }
}

bool SchemeEditor::registerSchemeBase(const SchemeBase &base)
{
  ui->qcbox_availableSchemeBases->addItem(base.name, QVariant::fromValue<SchemeBase>(base));

  if (ui->qcbox_availableSchemeBases->model()->rowCount() == 1)
    onSchemeChanged(0);

  return true;
}

void SchemeEditor::resetForm()
{
  if (ui->qcbox_availableSchemeBases->model()->rowCount() < 1)
    return;

  ui->qcbox_availableSchemeBases->setCurrentIndex(0);
  onSchemeChanged(0);
  m_currentSchemeName = "";
}
