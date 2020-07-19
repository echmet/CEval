#ifndef SCHEMEEDITOR_H
#define SCHEMEEDITOR_H

#include "exporterglobals.h"
#include <QDialog>
#include <QVariant>

class QStandardItemModel;

namespace Ui {
class SchemeEditor;
}

namespace DataExporter {

class SchemeEditor : public QDialog
{
  Q_OBJECT
public:
  class SchemeBase {
  public:
    explicit SchemeBase();
    explicit SchemeBase(const QString &name, const QString &description, const QStringVector &exportables);

    const QString name;
    const QString description;
    const QStringVector exportables;
  };

  class UserExportable {
  public:
    explicit UserExportable();
    explicit UserExportable(const QString &name, const QString &customName);
    explicit UserExportable(const UserExportable &other);
    UserExportable & operator=(const UserExportable &other);

    const QString name;
    const QString customName;
  };

  typedef QVector<UserExportable> UserExportablesVector;

  class UserScheme {
  public:
    explicit UserScheme();
    explicit UserScheme(const QString &name, const QString &baseName, const UserExportablesVector &exportables, const Globals::DataArrangement arrangement);
    UserScheme(const UserScheme &other);
    UserScheme & operator=(const UserScheme &other);

    const QString name;
    const QString baseName;
    const UserExportablesVector exportables;
    const Globals::DataArrangement arrangement;
    const bool isValid;
  };

  explicit SchemeEditor(QWidget *parent = nullptr);
  ~SchemeEditor();
  UserScheme interact(bool &canceled);
  UserScheme interact(const UserScheme &scheme, bool &canceled);
  bool registerSchemeBase(const SchemeBase &base);
  void resetForm();

private:
  void addExportable(const int row, const QVariant caption = QVariant());
  UserScheme interactInternal(bool &canceled);

  Ui::SchemeEditor *ui;

  QStandardItemModel *m_avaliableExportablesModel;
  QStandardItemModel *m_selectedExportablesModel;
  QString m_schemeName;
  QString m_currentSchemeName;

private slots:
  void onAddExportableClicked();
  void onCancelClicked();
  void onCreateClicked();
  void onRemoveExportableClicked();
  void onSchemeChanged(const int idx);
  void onSetCustomCaptionClicked();
};

} // namespace DataExporter
Q_DECLARE_METATYPE(DataExporter::SchemeEditor::SchemeBase)

#endif // SCHEMEEDITOR_H
