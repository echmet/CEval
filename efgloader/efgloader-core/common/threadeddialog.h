#ifndef THREADEDDIALOG_H
#define THREADEDDIALOG_H

#include "uibackend.h"

#include <functional>
#include <memory>
#include <QMutex>
#include <QWaitCondition>
#include <QMetaMethod>
#include <QMessageBox>

class ThreadedDialogBase
{
public:
  explicit ThreadedDialogBase(UIBackend *backend) :
    m_backend{backend}
  {}

  virtual ~ThreadedDialogBase() {}

  int execute()
  {
    int mthIdx = m_backend->metaObject()->indexOfMethod("display(ThreadedDialogBase*)");
    QMetaMethod mth = m_backend->metaObject()->method(mthIdx);

    m_lock.lock();
    mth.invoke(m_backend, Qt::QueuedConnection, Q_ARG(void *, this));
    m_barrier.wait(&m_lock);
    m_lock.unlock();

    return m_dlgRet;
  }

protected:
  virtual void process() = 0;

  UIBackend *const m_backend;
  int m_dlgRet;

  QMutex m_lock;
  QWaitCondition m_barrier;

  friend void UIBackend::display(ThreadedDialogBase *);
};

template <typename DialogType>
class ThreadedDialog : public ThreadedDialogBase
{
public:
  typedef std::function<DialogType * ()> DispFunc;
  typedef std::unique_ptr<ThreadedDialog<DialogType>> Ptr;

  explicit ThreadedDialog(UIBackend *backend, const DispFunc &dispFunc) :
    ThreadedDialogBase{backend},
    m_dispFunc{dispFunc}
  {}

  explicit ThreadedDialog(UIBackend *backend, DispFunc &&dispFunc) :
    ThreadedDialogBase{backend},
    m_dispFunc{dispFunc}
  {}

  virtual ~ThreadedDialog() override
  {
    m_dialog->deleteLater();
  }

  DialogType * dialog()
  {
    return m_dialog;
  }

private:
  virtual void process() override
  {
    m_dialog = m_dispFunc();
    m_dlgRet = m_dialog->exec();
  }

  const DispFunc m_dispFunc;
  DialogType *m_dialog;
};

template <>
class ThreadedDialog<QMessageBox> : ThreadedDialogBase
{
public:
  typedef std::function<QMessageBox * ()> DispFunc;
  typedef std::unique_ptr<ThreadedDialog<QMessageBox>> Ptr;

  explicit ThreadedDialog(UIBackend *backend, const DispFunc &dispFunc) :
    ThreadedDialogBase{backend},
    m_dispFunc{dispFunc}
  {}

  virtual ~ThreadedDialog() override
  {
    m_dialog->deleteLater();
  }

  static int displayCritical(UIBackend *backend, const QString &title, const QString &message)
  {
    auto dFunc = [=]() {
      return new QMessageBox{QMessageBox::Critical, title, message};
    };

    auto disp = Ptr{new ThreadedDialog<QMessageBox>{backend, std::move(dFunc)}};
    return disp->execute();
  }

  static int displayInformation(UIBackend *backend, const QString &title, const QString &message)
  {
    auto dFunc = [=]() {
      return new QMessageBox{QMessageBox::Information, title, message};
    };

    auto disp = Ptr{new ThreadedDialog<QMessageBox>{backend, std::move(dFunc)}};
    return disp->execute();
  }

  static int displayWarning(UIBackend *backend, const QString &title, const QString &message)
  {
    auto dFunc = [=]() {
      return new QMessageBox{QMessageBox::Warning, title, message};
    };

    auto disp = Ptr{new ThreadedDialog<QMessageBox>{backend, std::move(dFunc)}};
    return disp->execute();
  }

private:
  virtual void process() override
  {
    m_dialog = m_dispFunc();
    m_dlgRet = m_dialog->exec();
  }

  const DispFunc m_dispFunc;
  QMessageBox *m_dialog;
};

#endif // THREADEDDIALOG_H
