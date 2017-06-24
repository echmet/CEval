#ifndef NULLCRASHHANDLER_H
#define NULLCRASHHANDLER_H

#include "crashhandlerbase.h"

class NullCrashHandler : public CrashHandlerBase {
public:
  explicit NullCrashHandler(const std::string &);

  virtual const std::string & crashInfo() const override;
  virtual bool mainThreadCrashed() const override;
  virtual bool install() override;
  virtual void uninstall() override;

private:
  const std::string m_nullCrashInfo;

};

#endif // NULLCRASHHANDLER_H
