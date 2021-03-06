#include "exporter.h"
#include "../globals.h"
#include "schemesmanagerwidget.h"
#include "schemeeditor.h"
#include "schemeserializer.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace DataExporter;

const QString Exporter::FILEDIALOG_NAME_FILTER = "CEval exporter scheme (*.exs)";

Exporter::Exporter(const QString &exporterId) :
  QObject(nullptr),
  m_exporterId(exporterId)
{
  m_loadSchemeDialog = new QFileDialog(nullptr, tr("Load exporter scheme"));
  m_saveSchemeDialog = new QFileDialog(nullptr, tr("Save exporter scheme"));

  m_loadSchemeDialog->setAcceptMode(QFileDialog::AcceptOpen);
  m_loadSchemeDialog->setNameFilter(FILEDIALOG_NAME_FILTER);
  m_saveSchemeDialog->setAcceptMode(QFileDialog::AcceptSave);
  m_saveSchemeDialog->setNameFilter(FILEDIALOG_NAME_FILTER);

  m_schemeEditor = new SchemeEditor();
  m_schemesManagerDialog = new QDialog();
  m_schemesManagerDialog->setLayout(new QVBoxLayout());
  m_schemesManagerDialog->setWindowTitle(QObject::tr("Manage schemes"));

  m_schemesManagerWidget = new SchemesManagerWidget(m_schemesManagerDialog);
  m_schemesManagerDialog->layout()->addWidget(m_schemesManagerWidget);

  m_schemesModel = new QStandardItemModel();
  m_schemesManagerWidget->setSchemesModel(m_schemesModel);

  connect(m_schemesManagerWidget, &SchemesManagerWidget::closed, m_schemesManagerDialog, &QDialog::close);

  connect(m_schemesManagerWidget, &SchemesManagerWidget::createScheme, this, &Exporter::onCreateScheme);
  connect(m_schemesManagerWidget, &SchemesManagerWidget::editScheme, this, &Exporter::onEditScheme);
  connect(m_schemesManagerWidget, &SchemesManagerWidget::loadScheme, this, &Exporter::onDeserializeScheme);
  connect(m_schemesManagerWidget, &SchemesManagerWidget::removeScheme, this, &Exporter::onRemoveScheme);
  connect(m_schemesManagerWidget, &SchemesManagerWidget::saveScheme, this, &Exporter::onSerializeScheme);
}

Exporter::~Exporter()
{
  for (const Scheme *s : m_schemes)
    delete s;

  for (const SchemeBaseRoot *s : m_schemeBases)
    delete s;

  delete m_schemesManagerDialog;

  delete m_loadSchemeDialog;
  delete m_saveSchemeDialog;
}

Scheme * Exporter::createScheme()
{
  m_schemeEditor->resetForm();

  SchemeEditor::UserScheme ns;

  m_schemeEditor->setWindowTitle(tr("Create scheme"));
  bool canceled;
  while (true) {
    ns = m_schemeEditor->interact(canceled);

    if (canceled)
      return nullptr;

    if (!isUserSchemeValid(ns))
      continue;

    break;
  }

  return makeScheme(ns);
}

bool Exporter::isUserSchemeValid(SchemeEditor::UserScheme &scheme)
{
  return scheme.isValid;
}

Scheme * Exporter::makeScheme(const SchemeEditor::UserScheme &scheme)
{
  if (!m_schemeBases.contains(scheme.baseName))
    return nullptr;

  const SchemeBaseRoot *sbr = m_schemeBases.value(scheme.baseName);
  SelectedExportablesMap seMap;

  for (int idx = 0; idx < scheme.exportables.size(); idx++) {
    const SchemeEditor::UserExportable &ue = scheme.exportables.at(idx);

    if (!sbr->exportables.contains(ue.name))
      return nullptr;

    seMap.insert(ue.name, new SelectedExportable(sbr->exportables.value(ue.name), idx, ue.customName));
  }

  Scheme *s;
  try {
    s = new Scheme(scheme.name, seMap, sbr, scheme.arrangement);
  } catch (std::bad_alloc &) {
    QMessageBox::warning(nullptr, tr("Cannnot create scheme"), tr("Unable to create scheme. Please check the input and try again."));
    return nullptr;
  }

  return s;
}

void Exporter::manageSchemes()
{
  m_schemesManagerDialog->exec();
}

void Exporter::onCreateScheme()
{
  Scheme *scheme = createScheme();

  if (scheme == nullptr)
    return;

  if (!registerScheme(scheme))
    QMessageBox::warning(m_schemesManagerDialog, tr("Failed to register scheme"), tr("Scheme could not have been registered. Maybe a scheme with the same name already exists?"));
}

void Exporter::onDeserializeScheme()
{
  if (m_loadSchemeDialog->exec() == QDialog::Accepted) {
    const QString path = m_loadSchemeDialog->selectedFiles().at(0);
    Scheme *s;

    SchemeSerializer::RetCode tRet = SchemeSerializer::deserializeScheme(&s, m_exporterId, m_schemeBases, path);
    switch (tRet) {
    case SchemeSerializer::RetCode::OK:
      if (!registerScheme(s))
        QMessageBox::warning(m_schemesManagerDialog, tr("Failed to register scheme"), tr("Scheme could not have been registered. Maybe a scheme with the same name is already exists?"));
      return;
    case SchemeSerializer::RetCode::E_CANT_OPEN:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file cannot be opened for reading"));
      return;
    case SchemeSerializer::RetCode::E_INCORRECT_EXPORTER:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file contains a scheme for a different exporter"));
      return;
    case SchemeSerializer::RetCode::E_UNKNOWN_BASE:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file contains unknown SchemeBase"));
      return;
    case SchemeSerializer::RetCode::E_CORRUPTED_FILE:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file appears to be corrupted"));
      return;
    case SchemeSerializer::RetCode::E_UNKNOWN_EXPORTABLE:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file contains unknown Exportable"));
      return;
    case SchemeSerializer::RetCode::E_NO_MEMORY:
      QMessageBox::warning(nullptr, tr("Runtime error"), tr("Insufficient memory to create the scheme"));
      return;
    default:
      QMessageBox::warning(nullptr, tr("Unspecified error"), tr("Unexpected error"));
      return;
    }
  }
}

void Exporter::onEditScheme(const QString &name)
{
  bool canceled;

  if (!m_schemes.contains(name))
    return;

  Scheme *s = m_schemes.value(name);

  SchemeEditor::UserExportablesVector selectedExportables;
  /* Keep the order of selected exportables */
  {
    int max = 0;
    for (const SelectedExportable *se : s->selectedExportables) {
      if (se->position > max)
        max = se->position;
    }

    selectedExportables.resize(s->selectedExportables.count());

    SelectedExportablesMap::ConstIterator cit = s->selectedExportables.cbegin();
    while (cit != s->selectedExportables.cend()) {
      const QString &key = cit.key();
      const SelectedExportable *se = cit.value();

      selectedExportables[se->position] = SchemeEditor::UserExportable(key, se->displayName);
      cit++;
    }
  }

  SchemeEditor::UserScheme us(s->name, s->baseName(), selectedExportables, s->arrangement);

  m_schemeEditor->setWindowTitle(tr("Edit scheme"));
  SchemeEditor::UserScheme ns;
  while (true) {
    ns = m_schemeEditor->interact(us, canceled);

    if (canceled)
      return;

    if (!isUserSchemeValid(ns))
      continue;

    break;
  }

  removeScheme(name);
  s = makeScheme(ns);

  if (s == nullptr)
    return;

  if (!registerScheme(s))
    QMessageBox::warning(m_schemesManagerDialog, tr("Failed to register scheme"), tr("Scheme could not have been registered. Maybe a scheme with the same name already exists?"));
}

void Exporter::onRemoveScheme(const QString &name)
{
  removeScheme(name);
}

void Exporter::onSerializeScheme(const QString &name)
{
  if (!m_schemes.contains(name))
    return;

  const Scheme *s = m_schemes.value(name);

  if (m_saveSchemeDialog->exec() == QDialog::Accepted) {
    const QString path = m_saveSchemeDialog->selectedFiles().at(0);

    SchemeSerializer::RetCode ret = SchemeSerializer::serializeScheme(s, m_exporterId, path);

    switch (ret) {
    case SchemeSerializer::RetCode::OK:
      return;
    case SchemeSerializer::RetCode::E_CANT_OPEN:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Selected file cannot be open for writing"));
      return;
    case SchemeSerializer::RetCode::E_CANT_WRITE:
      QMessageBox::warning(nullptr, tr("IO error"), tr("An error occured while writing scheme"));
      return;
    default:
      QMessageBox::warning(nullptr, tr("IO error"), tr("Unexpected IO error occured while writing scheme"));
      return;
    }
  }
}

bool Exporter::registerScheme(Scheme *scheme)
{
  if (m_schemes.contains(scheme->name))
    return false;

  m_schemes.insert(scheme->name, scheme);

  QStandardItem *item = new QStandardItem(scheme->name);
  item->setData(scheme->name, Qt::UserRole);
  m_schemesModel->appendRow(item);

  return true;
}

bool Exporter::registerSchemeBase(const SchemeBaseRoot *schemeBase)
{
  if (m_schemeBases.contains(schemeBase->name))
    return false;

  m_schemeBases.insert(schemeBase->name, schemeBase);

  QStringVector exportables;
  for (const ExportableRoot *e : schemeBase->exportables)
    exportables << e->name;

  return m_schemeEditor->registerSchemeBase(SchemeEditor::SchemeBase(schemeBase->name, schemeBase->description, exportables));
}

void Exporter::removeScheme(const QString &name)
{
  m_schemes.remove(name);
  for (int idx = 0; idx < m_schemesModel->rowCount(); idx++) {
    const QStandardItem *item = m_schemesModel->item(idx);

    if (item->data(Qt::UserRole).toString() == name) {
      m_schemesModel->removeRows(idx, 1);
      return;
    }
  }
}

const Scheme * Exporter::scheme(const QString &id)
{
  if (!m_schemes.contains(id))
    return nullptr;

  return m_schemes.value(id);
}

QAbstractItemModel *Exporter::schemesModel()
{
  return m_schemesModel;
}
