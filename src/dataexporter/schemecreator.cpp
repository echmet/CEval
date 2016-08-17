#include "schemecreator.h"
#include "ui_schemecreator.h"
#include <QInputDialog>
#include <QStandardItem>
#include <QStandardItemModel>

using namespace DataExporter;

SchemeCreator::SchemeBase::SchemeBase() :
  name(""),
  description(""),
  exportables(QStringList())
{
}

SchemeCreator::SchemeBase::SchemeBase(const QString &name, const QString &description, const QStringList &exportables) :
  name(name),
  description(description),
  exportables(exportables)
{
}

SchemeCreator::NewScheme::NewScheme() :
  name(""),
  baseName(""),
  exportables(QStringList()),
  isValid(false)
{
}

SchemeCreator::NewScheme::NewScheme(const QString &name, const QString &baseName, const QStringList &exportables) :
  name(name),
  baseName(baseName),
  exportables(exportables),
  isValid(true)
{
}

SchemeCreator::SchemeCreator(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SchemeCreator)
{
  ui->setupUi(this);

  m_avaliableExportablesModel = new QStandardItemModel(this);
  m_selectedExportablesModel = new QStandardItemModel(this);

  ui->qlv_availableExportables->setModel(m_avaliableExportablesModel);
  ui->qlv_selectedExportables->setModel(m_selectedExportablesModel);

  ui->qlv_availableExportables->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->qlv_selectedExportables->setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(ui->qpb_addExportable, &QPushButton::clicked, this, &SchemeCreator::onAddExportableClicked);
  connect(ui->qpb_removeExportable, &QPushButton::clicked, this, &SchemeCreator::onRemoveExportableClicked);
  connect(ui->qpb_cancel, &QPushButton::clicked, this, &SchemeCreator::onCancelClicked);
  connect(ui->qpb_create, &QPushButton::clicked, this, &SchemeCreator::onCreateClicked);
  connect(ui->qcbox_availableSchemeBases, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &SchemeCreator::onSchemeChanged);
}

SchemeCreator::~SchemeCreator()
{
  delete ui;
}

SchemeCreator::NewScheme SchemeCreator::interact()
{
  QStringList selected;
  const QVariant base = ui->qcbox_availableSchemeBases->currentData(Qt::UserRole);

  if (!base.canConvert<SchemeBase>())
    return NewScheme();
  const QString baseName = base.value<SchemeBase>().name;

  int ret = this->exec();
  if (ret != QDialog::Accepted)
    return NewScheme();

  for (int idx = 0; idx < m_selectedExportablesModel->rowCount(); idx++) {
    const QStandardItem *item = m_selectedExportablesModel->item(idx, 0);

    if (item == nullptr)
      continue;

    selected << item->data(Qt::UserRole).toString();
 }

  return NewScheme(m_schemeName, baseName, selected);
}

void SchemeCreator::onAddExportableClicked()
{
  const QModelIndex &idx = ui->qlv_availableExportables->currentIndex();
  if (!idx.isValid())
    return;

  QList<QStandardItem *> itemList = m_avaliableExportablesModel->takeRow(idx.row());
  if (itemList.size() < 1)
    return;

  m_selectedExportablesModel->appendRow(itemList.at(0));
}

void SchemeCreator::onCancelClicked()
{
  reject();
}

void SchemeCreator::onCreateClicked()
{
  QInputDialog dlg;

  dlg.setLabelText("Enter name for the scheme");
  dlg.setInputMode(QInputDialog::TextInput);

  if (dlg.exec() != QDialog::Accepted)
    return;

  m_schemeName = dlg.textValue();
  accept();
}

void SchemeCreator::onRemoveExportableClicked()
{
  const QModelIndex &idx = ui->qlv_selectedExportables->currentIndex();
  if (!idx.isValid())
    return;

  QList<QStandardItem *> itemList = m_selectedExportablesModel->takeRow(idx.row());
  if (itemList.size() < 1)
    return;

  m_avaliableExportablesModel->appendRow(itemList.at(0));
}

void SchemeCreator::onSchemeChanged(const int idx)
{
  m_selectedExportablesModel->clear();
  m_avaliableExportablesModel->clear();

  QVariant v = ui->qcbox_availableSchemeBases->itemData(idx, Qt::UserRole);
  SchemeBase base = v.value<SchemeBase>();

  ui->qpte_descriptionText->setPlainText(base.description);

  for (const QString &s : base.exportables) {
    QStandardItem *item = new QStandardItem(s);
    item->setData(s, Qt::UserRole);

    m_avaliableExportablesModel->appendRow(item);
  }
}

bool SchemeCreator::registerSchemeBase(const SchemeBase &base)
{
  ui->qcbox_availableSchemeBases->addItem(base.name, QVariant::fromValue<SchemeBase>(base));

  return true;
}
