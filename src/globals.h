#ifndef GLOBALS_H
#define GLOBALS_H

#include <QPointF>
#include <QString>
#include <QVector>

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

  static QString VERSION_STRING();

  static const QString ECHMET_WEB_LINK;
  static const QString ORG_NAME;
  static const QString SOFTWARE_NAME;
  static const int VERSION_MAJ;
  static const int VERSION_MIN;
  static const QString VERSION_REV;

  static const QVector<DeveloperID> DEVELOPERS;

  static bool qpointfXComparator(const QPointF &first, const QPointF &second);
};

#endif // GLOBALS_H
