#ifndef CRASHHANDLERPROVIDER_H
#define CRASHHANDLERPROVIDER_H

class AbstractCrashHandler;

class CrashHandlerProvider
{
public:
  CrashHandlerProvider() = delete;

  static bool installCrashHandler();
  static void uninstallCrashHandler();
  static AbstractCrashHandler * handler();

private:
  static AbstractCrashHandler *s_handler;
};

#endif // CRASHHANDLERPROVIDER_H
