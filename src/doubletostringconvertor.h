#ifndef DOUBLETOSTRINGCONVERTOR_H
#define DOUBLETOSTRINGCONVERTOR_H

#include <QLocale>
#include <QVector>

class INumberFormatChangeable;

class DoubleToStringConvertor : public QObject
{
public:
  static double back(const QString &value, bool *ok);
  static QString convert(const double d);
  static int digits();
  static void initialize();
  static void loadUserSettings(const QVariant &settings);
  static const QLocale &locale();
  static void notifyOnFormatChanged(QObject *o);
  static QVariant saveUserSettings();
  static void setInitial();
  static void setParameters(const char type, int digits, const QString locName);
  static char type();

private:
  DoubleToStringConvertor();

  QLocale m_locale;
  int m_digits;
  char m_type;

  QVector<INumberFormatChangeable *> m_listeners;

  static DoubleToStringConvertor *s_me;

  static const QString DIGITS_SETTINGS_TAG;
  static const QString NUMBER_FORMAT_SETTINGS_TAG;
  static const QString LOCALE_SETTINGS_TAG;
};

#endif // DOUBLETOSTRINGCONVERTOR_H
