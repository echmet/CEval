#include "globals.h"

#include <QIcon>

const QString Globals::ECHMET_WEB_LINK("http://echmet.natur.cuni.cz");
const QString Globals::ORG_DOMAIN("echmet.natur.cuni.cz");
const QString Globals::ORG_NAME("ECHMET");
const QString Globals::SOFTWARE_NAME("CEval");
const int Globals::VERSION_MAJ(0);
const int Globals::VERSION_MIN(6);
const QString Globals::VERSION_REV("h4");

const QString Globals::CONFIG_FILE_NAME(QString("%1.conf").arg(SOFTWARE_NAME));

const QVector<Globals::DeveloperID> Globals::DEVELOPERS{ Globals::DeveloperID("Pavel Dubsk\xC3\xBD", "pavel.dubsky@natur.cuni.cz"),
                                                         Globals::DeveloperID("Magda Dovhunov\xC3\xA1", "magda.ordogova@natur.cuni.cz"),
                                                         Globals::DeveloperID("Michal Mal\xC3\xBD", "malymi@natur.cuni.cz")
                                                        };

Globals::DeveloperID::DeveloperID(const QString &name, const QString &mail) :
  name(name),
  mail(mail)
{
}

QString Globals::DeveloperID::linkString() const
{
  return QString("%1 (<a href=\"mailto:%2\">%2</a>)").arg(name).arg(mail.toHtmlEscaped());
}

QString Globals::DeveloperID::prettyString() const
{
  return QString("%1 (%2)").arg(name).arg(mail);
}

QIcon Globals::ICON()
{
#ifdef Q_OS_WIN
  static const QPixmap PROGRAM_ICON(":/images/res/CEval_icon.ico");
#else
  static const QPixmap PROGRAM_ICON(":/images/res/CEval_icon_64.png");
#endif // Q_OS_WIN

  if (PROGRAM_ICON.isNull())
      return {};

  return QIcon(PROGRAM_ICON);
}

QString Globals::VERSION_STRING()
{
  QString s = QString("%1 %2.%3%4").arg(SOFTWARE_NAME).arg(VERSION_MAJ).arg(VERSION_MIN).arg(VERSION_REV);
#ifdef UNSTABLE_VERSION
  s.append("-devel [" + QString(__DATE__) + " - " + QString(__TIME__)  + "]");
#endif

  return s;
}
