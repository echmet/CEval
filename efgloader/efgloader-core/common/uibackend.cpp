#include "uibackend.h"
#include "threadeddialog.h"

UIBackend *UIBackend::s_me{nullptr};

void UIBackend::initialize()
{
  if (s_me == nullptr) {
    s_me = new UIBackend{};
  }
}

UIBackend * UIBackend::instance()
{
  return s_me;
}

UIBackend::UIBackend(QObject *parent) :
  QObject{parent}
{
}

void UIBackend::display(ThreadedDialogBase *disp)
{
  disp->process();
  disp->m_barrier.wakeAll();
}
