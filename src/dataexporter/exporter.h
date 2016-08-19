#ifndef EXPORTER_H
#define EXPORTER_H

#include "exporterelems.h"

class QDialog;
class QStandardItemModel;

namespace DataExporter {

class Exporter : public QObject {
  Q_OBJECT
public:
  explicit Exporter();
  ~Exporter();
  bool registerSchemeBase(const SchemeBaseRoot *schemeBase);
  void showSchemes(const IExportable *exportee);

private:
  typedef QMap<QString, const SchemeBaseRoot *> SchemeBasesMap;
  typedef QMap<QString, Scheme *> SchemesMap;

  Scheme * createScheme();
  bool registerScheme(Scheme *scheme);

  SchemeCreator *m_schemeCreator;
  SchemeBasesMap m_schemeBases;
  SchemesMap m_schemes;
  QStandardItemModel *m_schemesModel;
  SelectSchemeWidget *m_selectSchemeWidget;
  QDialog *m_selectSchemeDialog;

  const IExportable *m_currentExportee;

private slots:
  void onCreateScheme();
  void onRemoveScheme(const QString &name);
  void onUseScheme(const QString &name);
};

} // namespace DataExporter

#endif // EXPORTER_H
