#ifndef CEVALCRASHHANDLER_H
#define CEVALCRASHHANDLER_H

class CrashHandlerBase;
class UICrashFinalizer;
class CrashEventCatcher;

class CEvalCrashHandler
{
public:
  CEvalCrashHandler() = delete;

  static bool installCrashHandler();
  static void uninstallCrashHandler();
  static CrashHandlerBase * handler();

private:
  static UICrashFinalizer *s_uiFinalizer;
  static CrashEventCatcher *s_catcher;

};

#endif // CEVALCRASHHANDLER_H
