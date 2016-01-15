#ifndef DOUBLETOSTRINGCONVERTOR_H
#define DOUBLETOSTRINGCONVERTOR_H

#include <QLocale>

class DoubleToStringConvertor
{
public:
  static double back(const QString &value, bool *ok);
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
  int m_digits;
  char m_type;

  static DoubleToStringConvertor *s_me;

  static const QString DIGITS_SETTINGS_TAG;
  static const QString NUMBER_FORMAT_SETTINGS_TAG;
};

#endif // DOUBLETOSTRINGCONVERTOR_H
