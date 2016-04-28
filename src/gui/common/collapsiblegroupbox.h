#ifndef COLLAPSIBLEGROUPBOX_H
#define COLLAPSIBLEGROUPBOX_H

#include <QGroupBox>

class QResizeEvent;
class CollapseExpandButton;

class CollapsibleGroupBox : public QGroupBox
{
public:
  explicit CollapsibleGroupBox(QWidget *parent = nullptr);

protected:
  void resizeEvent(QResizeEvent *ev);

private:
  void resizeCollapseButton(const QSize &size);

  CollapseExpandButton *m_clExpButton;

private slots:
  void onPrimaryScreenChanged();
  void onVisibilityChanged();

};

#endif // COLLAPSIBLEGROUPBOX_H
