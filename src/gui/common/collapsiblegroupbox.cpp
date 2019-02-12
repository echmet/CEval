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

void CollapsibleGroupBox::collapseLayout(QLayout *lay)
{
  assert(!m_layoutMargins.contains(lay));

  const int cnt = lay->count();
  for (int idx = 0; idx < cnt; idx++) {
    auto lit = lay->itemAt(idx);

    if (lit->widget()) {
      auto w = lit->widget();
      if (w != m_clExpButton)
        w->setVisible(false);
    }
    else if (lit->spacerItem())
      collapseSpacer(lit->spacerItem());
    else if (lit->layout())
      collapseLayout(lit->layout());
  }

  m_layoutMargins[lay] = lay->contentsMargins();
  lay->setContentsMargins(0, 0, 0, 0);
}

void CollapsibleGroupBox::collapseSpacer(QSpacerItem *spacer)
{
  assert(!m_spacerSizes.contains(spacer));

  m_spacerSizes[spacer] = {spacer->sizeHint(), spacer->sizePolicy()};
  spacer->changeSize(0, 0);
}

void CollapsibleGroupBox::expandLayout(QLayout *lay)
{
  assert(m_layoutMargins.contains(lay));

  const int cnt = lay->count();
  for (int idx = 0; idx < cnt; idx++) {
    auto lit = lay->itemAt(idx);

    if (lit->widget())
      lit->widget()->setVisible(true);
    else if (lit->spacerItem())
      expandSpacer(lit->spacerItem());
    else if (lit->layout())
      expandLayout(lit->layout());
  }

  lay->setContentsMargins(m_layoutMargins[lay]);
}

void CollapsibleGroupBox::expandSpacer(QSpacerItem *spacer)
{
  assert(m_spacerSizes.contains(spacer));

  const auto &sz = m_spacerSizes[spacer].first;
  const auto &pol = m_spacerSizes[spacer].second;

  spacer->changeSize(sz.width(), sz.height(), pol.horizontalPolicy(), pol.verticalPolicy());
}

void CollapsibleGroupBox::onScreenChanged()
{
  resizeCollapseButton();
}

void CollapsibleGroupBox::onVisibilityChanged()
{
  assert(this->layout() != nullptr);

  CollapseExpandButton::State s = m_clExpButton->state();

  switch (s) {
  case CollapseExpandButton::State::COLLAPSED:
    m_layoutMargins.clear();
    m_spacerSizes.clear();

    collapseLayout(this->layout());
    break;
  case CollapseExpandButton::State::EXPANDED:
    expandLayout(this->layout());
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
