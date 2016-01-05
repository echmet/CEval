#ifndef DOUBLETOSTRINGCONVERTOR_H
#define DOUBLETOSTRINGCONVERTOR_H

#include <QLocale>

class DoubleToStringConvertor
{
public:
  static QString convert(const double d);
  static int digits();
  static void initialize();
  static void loadUserSettings(const QVariant &settings);
  static QVariant saveUserSettings();
  static void setParameters(const char type, int digits);
  static char type();

private:
  DoubleToStringConvertor();

  QLocale m_locale;
  char m_type;
  int m_digits;

  static DoubleToStringConvertor *s_me;

  static const QString DIGITS_SETTINGS_TAG;
  static const QString NUMBER_FORMAT_SETTINGS_TAG;
};

#endif // DOUBLETOSTRINGCONVERTOR_H
