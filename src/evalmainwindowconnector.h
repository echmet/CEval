#ifndef EVALMAINWINDOWCONNECTOR_H
#define EVALMAINWINDOWCONNECTOR_H

#include <QObject>

class EvalMainWindowConnector
{
public:
  EvalMainWindowConnector() = delete;
  static void connectAll(QObject *anonWidget, QObject *anonDac);
};

#endif // EVALMAINWINDOWCONNECTOR_H
