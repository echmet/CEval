#ifndef EXPORTER_H
#define EXPORTER_H

#include "exporterelems.h"
#include "schemecreator.h"

class QDialog;
class QStandardItemModel;

namespace DataExporter {

class SelectSchemeWidget;

class Exporter : public QObject {
  Q_OBJECT
public:
  explicit Exporter();
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

  const IExportable *m_currentExportee;

private slots:
  void onCreateScheme();
  void onDeserializeScheme();
  void onEditScheme(const QString &name);
  void onRemoveScheme(const QString &name);
  void onUseScheme(const QString &name);
  void onSerializeScheme();
};

} // namespace DataExporter

#endif // EXPORTER_H
