#ifndef IPCPROXY_H
#define IPCPROXY_H

#include <QObject>
#include "dataloader.h"

class IPCProxy : public QObject {
  Q_OBJECT

public:
  explicit IPCProxy(DataLoader *loader, QObject *parent = nullptr);
  virtual ~IPCProxy() = 0;

protected:
  const DataLoader *m_loader;

private:
};

#endif // IPCPROXY_H
