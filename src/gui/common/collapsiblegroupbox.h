#ifndef COLLAPSIBLEGROUPBOX_H
#define COLLAPSIBLEGROUPBOX_H

#include <QGroupBox>
#include <QMap>
#include <QMargins>

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
  void collapseLayout(QLayout *layout);
  void expandLayout(QLayout *layout);

  CollapseExpandButton *m_clExpButton;
  QMap<const void *, QMargins> m_layoutMargins;

private slots:
  void onScreenChanged();
  void onVisibilityChanged();

};

#endif // COLLAPSIBLEGROUPBOX_H
