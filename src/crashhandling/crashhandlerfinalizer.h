#ifndef CRASHHANDLERFINALIZER_H
#define CRASHHANDLERFINALIZER_H

class CrashHandlerFinalizerRoot
{
public:
  CrashHandlerFinalizerRoot() {}

  virtual void operator()() = 0;
};

template<typename CrashHandlerType>
class CrashHandlerFinalizer : public CrashHandlerFinalizerRoot
{
public:
  explicit CrashHandlerFinalizer(CrashHandlerType *handler) :
    m_handler(handler)
  {
    handler->setFinalizer(this);
  }
  virtual ~CrashHandlerFinalizer() {}

protected:
  CrashHandlerType *m_handler;

};

#endif // CRASHHANDLERFINALIZER_H
