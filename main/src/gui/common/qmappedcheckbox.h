#ifndef QMAPPEDCHECKBOX_H
#define QMAPPEDCHECKBOX_H

#include <QCheckBox>

class QMappedCheckBox : public QCheckBox
{
  Q_OBJECT
public:
  QMappedCheckBox(QWidget *parent = nullptr);

private slots:
  void dropFocus();
};

#endif // QMAPPEDCHECKBOX_H
