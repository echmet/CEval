#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
class ThreadedDialogBase;

class UIBackend : public QObject
{
  Q_OBJECT
public:
  static void initialize();
  static UIBackend * instance();

public slots:
  virtual void display(ThreadedDialogBase *disp);

private:
  explicit UIBackend(QObject *parent = nullptr);

  static UIBackend *s_me;
};

#endif // UIBACKEND_H
