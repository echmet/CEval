#include "helpers.h"
#include <QCoreApplication>
#include <QMessageBox>
#include "globals.h"
#include "math/hvl.hpp"

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
