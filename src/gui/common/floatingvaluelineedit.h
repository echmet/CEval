#ifndef FLOATINGVALUELINEEDIT_H
#define FLOATINGVALUELINEEDIT_H

#include <QLineEdit>
#include <QLocale>
#include "../../inumberformatchangeable.h"

class FloatingValueLineEdit : public QLineEdit, public INumberFormatChangeable
{
  Q_OBJECT
  Q_INTERFACES(INumberFormatChangeable)
public:
  FloatingValueLineEdit(QWidget *parent = nullptr);
  void onNumberFormatChanged(const QLocale *oldLocale) override;

private slots:
  void ensureSanity(QString text);

};

#endif // FLOATINGVALUELINEEDIT_H
