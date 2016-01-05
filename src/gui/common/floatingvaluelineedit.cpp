#include "../../doubletostringconvertor.h"
#include "floatingvaluelineedit.h"
#include <QLocale>

#include <QDebug>

FloatingValueLineEdit::FloatingValueLineEdit(QWidget *parent) :
  QLineEdit(parent)
{
  this->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  connect(this, &FloatingValueLineEdit::textChanged, this, &FloatingValueLineEdit::ensureSanity);
}

void FloatingValueLineEdit::ensureSanity(QString text)
{
  bool ok;
  double dv;

  QString _text = text.replace(QChar::Nbsp, QString(""), Qt::CaseInsensitive);
  dv = m_locale.toDouble(_text, &ok);
  if (ok || _text.length() == 0) {
    this->setPalette(QPalette());

    /* You are not supposed to try to figure this out */
    if (_text.endsWith(m_locale.decimalPoint()) ||
        _text.startsWith(m_locale.decimalPoint()) ||
        _text.startsWith(m_locale.negativeSign()) ||
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

