#include "../../doubletostringconvertor.h"
#include "floatingvaluelineedit.h"
#include <QLocale>

#include <QDebug>

FloatingValueLineEdit::FloatingValueLineEdit(QWidget *parent) :
  QLineEdit(parent)
{
  this->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  connect(this, &FloatingValueLineEdit::textChanged, this, &FloatingValueLineEdit::ensureSanity);
  DoubleToStringConvertor::notifyOnFormatChanged(this);
}

void FloatingValueLineEdit::ensureSanity(QString text)
{
  bool ok;
  double dv;
  const QLocale &loc = DoubleToStringConvertor::locale();

  QString _text = text.replace(QChar::Nbsp, QString(""), Qt::CaseInsensitive);
  dv = loc.toDouble(_text, &ok);
  if (ok || _text.length() == 0) {
    this->setPalette(QPalette());

    /* You are not supposed to try to figure this out */
    if (_text.endsWith(loc.decimalPoint()) ||
        _text.startsWith(loc.decimalPoint()) ||
        _text.startsWith(loc.negativeSign()) ||
        _text.endsWith('0') ||
        dv == 0.0 ||
        _text.length() == 0 ||
        _text.contains('e') || _text.contains('E')) {
      return;
    } else {
      /* Do not re-emit textChanged */
      blockSignals(true);
      setText(DoubleToStringConvertor::convert(dv));
      blockSignals(false);
    }
  } else {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::red);
    this->setPalette(palette);
  }
}

void FloatingValueLineEdit::onNumberFormatChanged(const QLocale *oldLocale)
{
  bool ok;
  double dv;

  dv = oldLocale->toDouble(this->text(), &ok);
  if (ok) {
    blockSignals(true);
    this->setText(DoubleToStringConvertor::convert(dv));
    blockSignals(false);
  }
}

