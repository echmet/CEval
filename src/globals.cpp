#include "globals.h"

const QString Globals::ECHMET_WEB_LINK = "http://echmet.natur.cuni.cz";
const QString Globals::ORG_NAME = "ECHMET";
const QString Globals::SOFTWARE_NAME = "CEval";
const int Globals::VERSION_MAJ = 0;
const int Globals::VERSION_MIN = 6;
const QString Globals::VERSION_REV = "g";

const QVector<Globals::DeveloperID> Globals::DEVELOPERS = { Globals::DeveloperID("Pavel Dubsk\xC3\xBD", "pavel.dubsky@natur.cuni.cz"),
                                                            Globals::DeveloperID("Magda \xC3\x96rd\xC3\xB6gov\xC3\xA1", "magda.ordogova@natur.cuni.cz"),
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

QString Globals::VERSION_STRING()
{
  QString s = QString("%1 %2.%3%4").arg(SOFTWARE_NAME).arg(VERSION_MAJ).arg(VERSION_MIN).arg(VERSION_REV);
#ifdef UNSTABLE_VERSION
  s.append("-devel [" + QString(__DATE__) + " - " + QString(__TIME__)  + "]");
#endif

  return s;
}
