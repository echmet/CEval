#ifndef SCHEMESMANAGERWIDGET_H
#define SCHEMESMANAGERWIDGET_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
class SchemesManagerWidget;
}

namespace DataExporter {

class SchemesManagerWidget : public QWidget
{
  Q_OBJECT
public:
  explicit SchemesManagerWidget(QWidget *parent = nullptr);
  ~SchemesManagerWidget();
  void setSchemesModel(QAbstractItemModel *model);

private:
  Ui::SchemesManagerWidget *ui;

signals:
  void closed();
  void createScheme();
  void editScheme(const QString &name);
  void loadScheme();
  void removeScheme(const QString &name);
  void saveScheme(const QString &name);
  void useScheme(const QString &name);

private slots:
  void onCloseClicked();
  void onCreateSchemeClicked();
  void onEditSchemeClicked();
  void onLoadSchemeClicked();
  void onRemoveSchemeClicked();
  void onSaveSchemeClicked();

};

} // namespace DataExporter

#endif // SCHEMESMANAGERWIDGET_H
