#include "floatingvaluedelegate.h"
#include "doubletostringconvertor.h"
#include <QLineEdit>
#include <QEvent>

#include <QDebug>

FloatingValueDelegate::FloatingValueDelegate(QObject *parent) : QItemDelegate(parent)
{
}

QWidget *FloatingValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(option);
  Q_UNUSED(index);

  QLineEdit *lineEdit = new QLineEdit(parent);

  connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));

  return lineEdit;
}

void FloatingValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  bool ok;
  double value = index.model()->data(index, Qt::EditRole).toDouble(&ok);
  if (!ok)
    return;

  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  if (lineEdit == nullptr)
    return;

  lineEdit->setText(DoubleToStringConvertor::convert(value));
}

void FloatingValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  double value;
  bool ok;
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  if (lineEdit == nullptr)
    return;

  QString s(lineEdit->text());
  s = s.replace(QChar::Nbsp, QString(""));
  value = m_locale.toDouble(s, &ok);
  if (!ok)
    return;

  model->setData(index, value, Qt::EditRole);
}

void FloatingValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  Q_UNUSED(index);

  editor->setGeometry(option.rect);
}

void FloatingValueDelegate::onTextChanged(const QString)
{
  bool ok;
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(QObject::sender());
  if (lineEdit == nullptr)
    return;

  QString s(lineEdit->text());
  s = s.replace(QChar::Nbsp, QString(""));
  m_locale.toDouble(s, &ok);
  if (ok || lineEdit->text().length() == 0)
    lineEdit->setPalette(QPalette());
  else {
    QPalette palette = lineEdit->palette();

    palette.setColor(QPalette::Base, QColor(Qt::red));
    lineEdit->setPalette(palette);
  }
}
