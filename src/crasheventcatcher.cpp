#include "crasheventcatcher.h"
#include "crashevent.h"
#include "gui/crashhandlerdialog.h"

CrashEventCatcher::CrashEventCatcher() :
  m_crashEventId(CrashEvent::registerMe())
{
}

bool CrashEventCatcher::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == m_crashEventId) {
    CrashEvent *crashEvt = static_cast<CrashEvent *>(event);
    handleCrash(crashEvt);

    /* We should never get here */
    std::abort();
    return true;
  } else {
    return QObject::eventFilter(watched, event);
  }
}

void CrashEventCatcher::executeEmergency()
{
    emit emergency();
}

void CrashEventCatcher::handleCrash(const CrashEvent *event)
{
  emit emergency();

  CrashHandlerDialog dlg;

  dlg.setBacktrace(event->crashHandler->crashInfo().c_str());
  dlg.exec();

  event->crashHandler->proceedToKill();
}
