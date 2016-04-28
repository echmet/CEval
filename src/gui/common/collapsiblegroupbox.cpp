#include "collapsiblegroupbox.h"
#include "collapseexpandbutton.h"
#include <QGuiApplication>
#include <QLayout>
#include <QResizeEvent>
#include <QScreen>

CollapsibleGroupBox::CollapsibleGroupBox(QWidget *parent) :
  QGroupBox(parent)
{
  m_clExpButton = new CollapseExpandButton(this);

  resizeCollapseButton(this->size());

  connect(m_clExpButton, &CollapseExpandButton::clicked, this, &CollapsibleGroupBox::onVisibilityChanged);
  connect(qApp, &QGuiApplication::primaryScreenChanged, this, &CollapsibleGroupBox::onPrimaryScreenChanged);
}

void CollapsibleGroupBox::onPrimaryScreenChanged()
{
  resizeCollapseButton(this->size());
}

void CollapsibleGroupBox::onVisibilityChanged()
{
  CollapseExpandButton::State s;

  s = m_clExpButton->state();

  QList<QObject *> children = this->children();

  switch (s) {
  case CollapseExpandButton::State::COLLAPSED:
    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr) {
        if (w != m_clExpButton)
          w->setVisible(false);
      }
    }
    break;
  case CollapseExpandButton::State::EXPANDED:
    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr)
        w->setVisible(true);
    }
    break;
  }
}

void CollapsibleGroupBox::resizeCollapseButton(const QSize &size)
{
  QScreen *scr = QGuiApplication::primaryScreen();

  if (scr == nullptr)
    return;

  const qreal dpi = scr->logicalDotsPerInchX();
  const qreal btnSize = floor((25.0 * dpi / 96.0) + 0.5);

  m_clExpButton->setGeometry(size.width() - btnSize, 0, btnSize, btnSize);
}

void CollapsibleGroupBox::resizeEvent(QResizeEvent *ev)
{
  resizeCollapseButton(ev->size());
}
