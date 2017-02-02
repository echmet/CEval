#ifndef CRASHHANDLERBASE_H
#define CRASHHANDLERBASE_H

#include "crashhandlerfinalizer.h"
#include <string>

class CrashHandlerBase
{
public:
  explicit CrashHandlerBase(const std::string &miniDumpPath);
  virtual ~CrashHandlerBase() {}
  virtual const std::string & crashInfo() const = 0;
  virtual bool install() = 0;
  virtual bool mainThreadCrashed() const = 0;
  virtual void proceedToKill() const;
  virtual void uninstall() = 0;
  void setFinalizer(CrashHandlerFinalizerRoot *finalizer);
  virtual void waitForKill();

protected:
  void finalize();

  const std::string m_miniDumpPath;

private:
  CrashHandlerFinalizerRoot *m_finalizer;

};

#endif // CRASHHANDLERBASE_H
