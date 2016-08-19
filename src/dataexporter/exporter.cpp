#include "exporter.h"
#include "../globals.h"
#include "selectschemewidget.h"
#include "schemecreator.h"
#include <QDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace DataExporter;

Exporter::Exporter() :
  QObject(nullptr)
{
  m_currentExportee = nullptr;
  m_schemeCreator = new SchemeCreator();
  m_selectSchemeDialog = new QDialog();
  m_selectSchemeDialog->setLayout(new QVBoxLayout());
  m_selectSchemeDialog->setWindowTitle(QObject::tr("Exporter schemes"));

  m_selectSchemeWidget = new SelectSchemeWidget(m_selectSchemeDialog);
  m_selectSchemeDialog->layout()->addWidget(m_selectSchemeWidget);

  m_schemesModel = new QStandardItemModel();
  m_selectSchemeWidget->setSchemesModel(m_schemesModel);

  connect(m_selectSchemeWidget, &SelectSchemeWidget::closed, m_selectSchemeDialog, &QDialog::close);

  connect(m_selectSchemeWidget, &SelectSchemeWidget::createScheme, this, &Exporter::onCreateScheme);
  connect(m_selectSchemeWidget, &SelectSchemeWidget::removeScheme, this, &Exporter::onRemoveScheme);
  connect(m_selectSchemeWidget, &SelectSchemeWidget::useScheme, this, &Exporter::onUseScheme);
}

Exporter::~Exporter()
{
  for (const Scheme *s : m_schemes)
    delete s;

  for (const SchemeBaseRoot *s : m_schemeBases)
    delete s;

  delete m_selectSchemeDialog;
}

Scheme *Exporter::createScheme()
{
  const SchemeCreator::NewScheme ns = m_schemeCreator->interact();
  if (!ns.isValid)
    return nullptr;

  if (!m_schemeBases.contains(ns.baseName))
    return nullptr;

  const SchemeBaseRoot *sbr = m_schemeBases.value(ns.baseName);
  SelectedExportablesMap seMap;

  for (const QString &s : ns.exportables) {
    if (!sbr->exportables.contains(s))
      return nullptr;

    seMap.insert(s, new SelectedExportable(sbr->exportables.value(s)));
  }

  Scheme *s = new Scheme(ns.name, seMap, sbr);

  return s;
}

void Exporter::onCreateScheme()
{
  Scheme *scheme = createScheme();

  if (scheme == nullptr) {
    QMessageBox::warning(nullptr, tr("Cannnot create scheme"), tr("Unable to create scheme. Please check the input and try again."));
    return;
  }

  if (!registerScheme(scheme))
    QMessageBox::warning(m_selectSchemeDialog, tr("Failed to register scheme"), tr("Scheme could not have been registered. Maybe a scheme with the same name is already exists?"));
}

void Exporter::onRemoveScheme(const QString &name)
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

void Exporter::onUseScheme(const QString &name)
{
  if (!m_schemes.contains(name))
    return;

  if (m_currentExportee == nullptr) {
    QMessageBox::critical(m_selectSchemeDialog, tr("Runtime error"), QString(tr("Invalid pointer to \"exportee\". Please report this as a bug to %1 developers.")).arg(Globals::SOFTWARE_NAME));
    return;
  }

  const Scheme *s = m_schemes.value(name);
  s->exportData(m_currentExportee);
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

  QStringList exportables;
  for (const ExportableRoot *e : schemeBase->exportables)
    exportables << e->name;

  return m_schemeCreator->registerSchemeBase(SchemeCreator::SchemeBase(schemeBase->name, schemeBase->description, exportables));
}

void Exporter::showSchemes(const IExportable *exportee)
{
  m_currentExportee = exportee;

  m_selectSchemeDialog->exec();

  m_currentExportee = nullptr;
}
