#ifndef GLOBALS_H
#define GLOBALS_H

#include <QPointF>
#include <QString>
#include <QVector>

class QIcon;

class Globals
{
public:
  class DeveloperID {
  public:
    explicit DeveloperID(const QString &name, const QString &mail);

    const QString name;
    const QString mail;

    QString linkString() const;
    QString prettyString() const;
  };

  Globals() = delete;

  static QIcon ICON();
  static QString VERSION_STRING();

  static const QString ECHMET_WEB_LINK;
  static const QString ORG_DOMAIN;
  static const QString ORG_NAME;
  static const QString SOFTWARE_NAME;
  static const int VERSION_MAJ;
  static const int VERSION_MIN;
  static const QString VERSION_REV;
  static const QString CONFIG_FILE_NAME;

  static const QVector<DeveloperID> DEVELOPERS;
};

#endif // GLOBALS_H
