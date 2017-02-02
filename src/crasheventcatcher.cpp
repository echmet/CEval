#include "crasheventcatcher.h"
#include "crashevent.h"
#include "gui/crashhandlerdialog.h"

#include <QDebug>

CrashEventCatcher::CrashEventCatcher() :
  m_crashEventId(CrashEvent::registerMe())
{
}

bool CrashEventCatcher::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == m_crashEventId) {
    emit emergency();

    CrashEvent *crashEvt = static_cast<CrashEvent *>(event);
    CrashHandlerDialog dlg;

    dlg.setBacktrace(crashEvt->crashHandler->crashInfo().c_str());
    dlg.exec();

    crashEvt->crashHandler->proceedToKill();

    /* We should never get here */
    std::abort();
    return true;
  } else {
    return QObject::eventFilter(watched, event);
  }
}
