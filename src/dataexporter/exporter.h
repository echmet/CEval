#ifndef EXPORTER_H
#define EXPORTER_H

#include "exporterelems.h"
#include "schemeeditor.h"

class QAbstractItemModel;
class QDialog;
class QFileDialog;
class QStandardItemModel;

namespace DataExporter {

class SchemesManagerWidget;

class Exporter : public QObject {
  Q_OBJECT
public:
  explicit Exporter(const QString &exporterId);
  ~Exporter();
  void manageSchemes();
  bool registerSchemeBase(const SchemeBaseRoot *schemeBase);
  QAbstractItemModel * schemesModel();

private:
  Scheme * createScheme();
  bool isUserSchemeValid(SchemeEditor::UserScheme &scheme);
  Scheme * makeScheme(const SchemeEditor::UserScheme &scheme);
  bool registerScheme(Scheme *scheme);
  void removeScheme(const QString &name);

  SchemeEditor *m_schemeEditor;
  SchemeBasesMap m_schemeBases;
  SchemesMap m_schemes;
  QStandardItemModel *m_schemesModel;
  SchemesManagerWidget *m_schemesManagerWidget;
  QDialog *m_schemesManagerDialog;

  QFileDialog *m_loadSchemeDialog;
  QFileDialog *m_saveSchemeDialog;

  const QString m_exporterId;

  static const QString FILEDIALOG_NAME_FILTER;

private slots:
  void onCreateScheme();
  void onDeserializeScheme();
  void onEditScheme(const QString &name);
  void onRemoveScheme(const QString &name);
  void onSerializeScheme(const QString &name);

};

} // namespace DataExporter

#endif // EXPORTER_H
