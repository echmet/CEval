#include "helpers.h"
#include <QCoreApplication>
#include <QMessageBox>
#include "globals.h"
#include "math/hvl.hpp"
#include <signal.h>

QString Helpers::hvlstrToQString(void *str) {
  const hvlstr_t _str = static_cast<const hvlstr_t>(str);
#ifdef ECHMET_MATH_HVL_PLATFORM_WIN
  return QString::fromWCharArray(_str);
#elif defined ECHMET_MATH_HVL_PLATFORM_UNIX
  return QString::fromLocal8Bit(_str);
#else
  #error "No platform type has been specified!"
#endif // HVL_PLATFORM_
}

void Helpers::execCFIT()
{
  raise(SIGABRT);
}

bool Helpers::exitApplicationWithWarning()
{
  int dlgRet = QMessageBox::question(nullptr, QObject::tr("Exit %1?").arg(Globals::SOFTWARE_NAME),
                                     QObject::tr("Do you really want to exit %1?\n"
                                                 "All unsaved data will be lost.").arg(Globals::SOFTWARE_NAME),
                                     QMessageBox::Yes | QMessageBox::No);

  if (dlgRet == QMessageBox::Yes) {
    qApp->exit();
    return true;
  }

  return false;
}

qreal Helpers::yForX(const qreal x, const QVector<QPointF> &v)
{
  int idx = 0;

  if (v.size() == 0)
    throw std::out_of_range("Vector contains no data");

  if (v.at(0).x() > x)
    throw std::out_of_range("Value of X is lower than the lowest X value in the vector");

  while (idx < v.size()) {
    if (v.at(idx).x() >= x)
      break;

    idx++;
  }

  if (idx >= v.size())
    throw std::out_of_range("Value of X is higher than the highest X value in the vector");

  return v.at(idx).y();
}
