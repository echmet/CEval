#ifndef EXPORTER_H
#define EXPORTER_H

#include "exporterelems.h"
#include "schemecreator.h"

class QDialog;
class QFileDialog;
class QStandardItemModel;

namespace DataExporter {

class SelectSchemeWidget;

class Exporter : public QObject {
  Q_OBJECT
public:
  explicit Exporter(const QString &exporterId);
  ~Exporter();
  bool registerSchemeBase(const SchemeBaseRoot *schemeBase);
  void showSchemes(const IExportable *exportee);

private:
  Scheme * createScheme();
  bool isUserSchemeValid(SchemeCreator::UserScheme &scheme);
  Scheme * makeScheme(const SchemeCreator::UserScheme &scheme);
  bool registerScheme(Scheme *scheme);
  void removeScheme(const QString &name);

  SchemeCreator *m_schemeCreator;
  SchemeBasesMap m_schemeBases;
  SchemesMap m_schemes;
  QStandardItemModel *m_schemesModel;
  SelectSchemeWidget *m_selectSchemeWidget;
  QDialog *m_selectSchemeDialog;

  QFileDialog *m_loadSchemeDialog;
  QFileDialog *m_saveSchemeDialog;

  const IExportable *m_currentExportee;
  const QString m_exporterId;

  static const QString FILEDIALOG_NAME_FILTER;

private slots:
  void onCreateScheme();
  void onDeserializeScheme();
  void onEditScheme(const QString &name);
  void onRemoveScheme(const QString &name);
  void onUseScheme(const QString &name);
  void onSerializeScheme(const QString &name);

};

} // namespace DataExporter

#endif // EXPORTER_H
