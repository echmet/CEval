#include "doubletostringconvertor.h"
#include "custommetatypes.h"
#include "inumberformatchangeable.h"

DoubleToStringConvertor *DoubleToStringConvertor::s_me = nullptr;

const QString DoubleToStringConvertor::DIGITS_SETTINGS_TAG("Digits");
const QString DoubleToStringConvertor::NUMBER_FORMAT_SETTINGS_TAG("NumberFormat");
const QString DoubleToStringConvertor::LOCALE_SETTINGS_TAG("Locale");

DoubleToStringConvertor::DoubleToStringConvertor() :
  QObject(nullptr),
  m_locale(QLocale()),
  m_digits(5),
  m_type('g')
{
}

double DoubleToStringConvertor::back(const QString &value, bool *ok)
{
  return s_me->m_locale.toDouble(value, ok);
}

QString DoubleToStringConvertor::convert(const double d)
{
  return s_me->m_locale.toString(d, s_me->m_type, s_me->m_digits);
}

int DoubleToStringConvertor::digits()
{
  return s_me->m_digits;
}

void DoubleToStringConvertor::initialize()
{
  s_me = new DoubleToStringConvertor();
}

QLocale::Country DoubleToStringConvertor::country()
{
  return s_me->m_locale.country();
}

void DoubleToStringConvertor::loadUserSettings(const QVariant &settings)
{
  if (!settings.canConvert<EMT::StringVariantMap>())
    return;

  EMT::StringVariantMap map = settings.value<EMT::StringVariantMap>();

  if (map.contains(DIGITS_SETTINGS_TAG)) {
    QVariant v = map[DIGITS_SETTINGS_TAG];
    bool ok;
    int digits;

    digits = v.toInt(&ok);
    if (ok)
      s_me->m_digits = digits;
  }

  if (map.contains(NUMBER_FORMAT_SETTINGS_TAG)) {
    QVariant v = map[NUMBER_FORMAT_SETTINGS_TAG];
    QChar ch;

    ch = v.toChar();
    if (ch == 'g' || ch == 'f')
      s_me->m_type = ch.toLatin1();
  }

  if (map.contains(LOCALE_SETTINGS_TAG)) {
    QVariant v = map[LOCALE_SETTINGS_TAG];
    int ctry;
    bool ok;

    ctry = v.toInt(&ok);
    if (ok) {
      if (ctry > 0 && ctry <= 246)
        s_me->m_locale = QLocale(QLocale::AnyLanguage, static_cast<QLocale::Country>(ctry));
    }
  }
}

const QLocale &DoubleToStringConvertor::locale()
{
  return s_me->m_locale;
}

void DoubleToStringConvertor::notifyOnFormatChanged(QObject *o)
{
  INumberFormatChangeable *nco = qobject_cast<INumberFormatChangeable *>(o);
  Q_ASSERT(nco != nullptr);

  s_me->m_listeners.push_back(nco);
}

QVariant DoubleToStringConvertor::saveUserSettings()
{
  EMT::StringVariantMap map;

  map[DIGITS_SETTINGS_TAG] = s_me->digits();
  map[NUMBER_FORMAT_SETTINGS_TAG] = s_me->type();
  map[LOCALE_SETTINGS_TAG] = s_me->m_locale.country();

  return QVariant::fromValue<EMT::StringVariantMap>(map);
}

void DoubleToStringConvertor::setParameters(const char type, const int digits, const QLocale::Country ctry)
{
  s_me->m_type = type;
  s_me->m_digits = digits;

  const QLocale *oldLocale = new QLocale(s_me->m_locale);

  s_me->m_locale = QLocale(QLocale::AnyLanguage, ctry);

  for (INumberFormatChangeable *nco : s_me->m_listeners)
    nco->onNumberFormatChanged(oldLocale);

  delete oldLocale;
}

char DoubleToStringConvertor::type()
{
  return s_me->m_type;
}

