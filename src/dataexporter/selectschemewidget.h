#ifndef SELECTSCHEMEWIDGET_H
#define SELECTSCHEMEWIDGET_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
class SelectSchemeWidget;
}

namespace DataExporter {

class SelectSchemeWidget : public QWidget
{
  Q_OBJECT
public:
  explicit SelectSchemeWidget(QWidget *parent = nullptr);
  ~SelectSchemeWidget();
  void setSchemesModel(QAbstractItemModel *model);

private:
  Ui::SelectSchemeWidget *ui;

signals:
  void closed();
  void createScheme();
  void removeScheme(const QString &name);
  void useScheme(const QString &name);

private slots:
  void onCloseClicked();
  void onCreateSchemeClicked();
  void onEditSchemeClicked();
  void onLoadSchemeClicked();
  void onRemoveSchemeClicked();
  void onSaveSchemeClicked();
  void onUseSchemeClicked();
};

} // namespace DataExporter

#endif // SELECTSCHEMEWIDGET_H
