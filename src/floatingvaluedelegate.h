#ifndef FLOATINGVALUEDELEGATE_H
#define FLOATINGVALUEDELEGATE_H

#include <QLocale>
#include <QItemDelegate>

class FloatingValueDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  FloatingValueDelegate(QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

private slots:
  void onTextChanged(const QString);

};

#endif // FLOATINGVALUEDELEGATE_H
