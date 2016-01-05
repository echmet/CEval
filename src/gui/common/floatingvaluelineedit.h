#ifndef FLOATINGVALUELINEEDIT_H
#define FLOATINGVALUELINEEDIT_H

#include <QLineEdit>
#include <QLocale>

class FloatingValueLineEdit : public QLineEdit
{
  Q_OBJECT
public:
  FloatingValueLineEdit(QWidget *parent = nullptr);

private:
  QLocale m_locale;

private slots:
  void ensureSanity(QString text);

};

#endif // FLOATINGVALUELINEEDIT_H
