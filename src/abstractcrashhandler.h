#ifndef ABSTRACTCRASHHANDLER_H
#define ABSTRACTCRASHHANDLER_H

#include <QObject>

class AbstractCrashHandler : public QObject
{
  Q_OBJECT
public:
  virtual bool install() = 0;
  virtual void uninstall() = 0;

signals:
  void emergency();

};

#endif // ABSTACTCRASHHANDLER_H
