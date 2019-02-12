#include "collapsiblegroupbox.h"
#include "collapseexpandbutton.h"
#include <QApplication>
#include <QGuiApplication>
#include <QLayout>
#include <QResizeEvent>
#include <QScreen>
#include <QStyle>
#include <QTimer>
#include <QWindow>
#include <cmath>

inline
QWindow *findWindowForWidget(const QWidget *widget)
{
  for (;;) {
    QWindow *wh = widget->window()->windowHandle();
    if (wh != nullptr)
      return wh;

    widget = qobject_cast<const QWidget *>(widget->parent());
    if (widget == nullptr)
      return nullptr;
  }
}

inline
QScreen * findScreenForWidget(const QWidget *widget)
{
  for (;;) {
    QWindow *wh = widget->window()->windowHandle();
    if (wh != nullptr) {
      QScreen *scr = wh->screen();
      if (scr != nullptr)
        return scr;
    }

    widget = qobject_cast<const QWidget *>(widget->parent());
    if (widget == nullptr)
      return nullptr;
  }
}

CollapsibleGroupBox::CollapsibleGroupBox(QWidget *parent) :
  QGroupBox(parent)
{
  m_clExpButton = new CollapseExpandButton(this);

  connect(m_clExpButton, &CollapseExpandButton::clicked, this, &CollapsibleGroupBox::onVisibilityChanged);

  QTimer::singleShot(0, this, [this] {
    auto wh = findWindowForWidget(this);
    if (wh != nullptr)
      connect(wh, &QWindow::screenChanged, this, &CollapsibleGroupBox::onScreenChanged);
  });

  QTimer::singleShot(0, this, &CollapsibleGroupBox::resizeCollapseButton);
}

void CollapsibleGroupBox::collapseLayout(QLayout *layout)
{
  for (QObject *o : layout->children()) {
    QLayout *l= qobject_cast<QLayout *>(o);

    if (l == nullptr)
      continue;

    collapseLayout(l);
  }
  if (m_layoutMargins.contains(layout))
    return;

  QMargins m = layout->contentsMargins();
  m_layoutMargins[layout] = m;
  layout->setContentsMargins(0, 0, 0, 0);
}

void CollapsibleGroupBox::expandLayout(QLayout *layout)
{
  for (QObject *o : layout->children()) {
    QLayout *l = qobject_cast<QLayout *>(o);

    if (l == nullptr)
      continue;

    if (m_layoutMargins.contains(l))
      expandLayout(l);
  }
  if (m_layoutMargins.contains(layout)) {
    QMargins m = m_layoutMargins[layout];
    layout->setContentsMargins(m);
  }
}

void CollapsibleGroupBox::onScreenChanged()
{
  resizeCollapseButton();
}

void CollapsibleGroupBox::onVisibilityChanged()
{
  CollapseExpandButton::State s;

  s = m_clExpButton->state();

  QList<QObject *> children = this->children();

  switch (s) {
  case CollapseExpandButton::State::COLLAPSED:
    m_layoutMargins.clear();

    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr) {
        if (w != m_clExpButton)
          w->setVisible(false);

        continue;
      }

      QLayout *l = qobject_cast<QLayout *>(o);
      if (l != nullptr)
        collapseLayout(l);
    }
    break;
  case CollapseExpandButton::State::EXPANDED:
    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr) {
        w->setVisible(true);

        continue;
      }

      QLayout *l = qobject_cast<QLayout *>(o);
      if (l != nullptr)
        expandLayout(l);

    }
    break;
  }
}

void CollapsibleGroupBox::resizeCollapseButton()
{
  const QScreen *scr = findScreenForWidget(this);

  if (scr == nullptr)
    return;

  const auto &size = this->size();

#ifdef Q_OS_WIN
  qreal baseSize = 15.0;
  int yOffset = 5;
#else
  qreal baseSize = 22.0;
  int yOffset = 0;
#endif

  if (scr == nullptr)
    return;

  if (QString::compare(QApplication::style()->objectName(), "fusion") == 0)
    baseSize = 15.0;

  const qreal dpi = scr->logicalDotsPerInchX();
  const qreal btnSize = floor((baseSize * dpi / 96.0) + 0.5);

  m_clExpButton->setGeometry(size.width() - btnSize, yOffset, btnSize, btnSize);
}

void CollapsibleGroupBox::resizeEvent(QResizeEvent *)
{
  resizeCollapseButton();
}
