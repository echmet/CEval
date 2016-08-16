#ifndef SCHEMECREATOR_H
#define SCHEMECREATOR_H

#include <QDialog>
class QStandardItemModel;

namespace Ui {
class SchemeCreator;
}

namespace DataExporter {

class SchemeCreator : public QDialog
{
  Q_OBJECT
public:
  class SchemeBase {
  public:
    explicit SchemeBase();
    explicit SchemeBase(const QString &name, const QString &description, const QStringList &exportables);

    const QString name;
    const QString description;
    const QStringList exportables;
  };

  class NewScheme {
  public:
    explicit NewScheme();
    explicit NewScheme(const QString &name, const QString &baseName, const QStringList &exportables);

    const QString name;
    const QString baseName;
    const QStringList exportables;
    const bool isValid;
  };

  explicit SchemeCreator(QWidget *parent = nullptr);
  ~SchemeCreator();
  NewScheme interact();
  bool registerSchemeBase(const SchemeBase &base);

private:
  Ui::SchemeCreator *ui;

  QStandardItemModel *m_avaliableExportablesModel;
  QStandardItemModel *m_selectedExportablesModel;

private slots:
  void onAddExportableClicked();
  void onCancelClicked();
  void onExportClicked();
  void onRemoveExportableClicked();
  void onSchemeChanged(const int idx);
};

} // namespace DataExporter
Q_DECLARE_METATYPE(DataExporter::SchemeCreator::SchemeBase)

#endif // SCHEMECREATOR_H
