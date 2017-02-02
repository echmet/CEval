#ifndef NULLCRASHHANDLER_H
#define NULLCRASHHANDLER_H

#include "crashhandlerbase.h"

class NullCrashHandler : public CrashHandlerBase {
public:
  virtual bool install() override;
  virtual void uninstall() override;

};

#endif // NULLCRASHHANDLER_H
