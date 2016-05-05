#include "collapsiblegroupbox.h"
#include "collapseexpandbutton.h"
#include <QApplication>
#include <QGuiApplication>
#include <QLayout>
#include <QResizeEvent>
#include <QScreen>
#include <QStyle>

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

  QLayout *master = this->layout();
  QList<QObject *> children = this->children();

  switch (s) {
  case CollapseExpandButton::State::COLLAPSED:
    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr) {
        if (w != m_clExpButton)
          w->setVisible(false);

        continue;
      }

      if (o == master) {
        for (QObject *lo : master->children()) {
          QLayout *loc = qobject_cast<QLayout *>(lo)
                         ;
          if (loc == nullptr)
            continue;

          QMargins m = loc->contentsMargins();
          m_layoutMargins[loc] = m;
          loc->setContentsMargins(0, 0, 0, 0);
        }
        QMargins m = master->contentsMargins();
        m_layoutMargins[master] = m;
        master->setContentsMargins(0, 0, 0, 0);
      }
    }
    break;
  case CollapseExpandButton::State::EXPANDED:
    for (QObject *o : children) {
      QWidget *w = qobject_cast<QWidget *>(o);
      if (w != nullptr) {
        w->setVisible(true);

        continue;
      }

      if (o == master) {
        for (QObject *lo : master->children()) {
          QLayout *loc = qobject_cast<QLayout *>(lo)
                         ;
          if (loc == nullptr)
            continue;

          if (m_layoutMargins.contains(loc)) {
            QMargins m = m_layoutMargins[loc];
            loc->setContentsMargins(m);
          }
        }
        if (m_layoutMargins.contains(master)) {
          QMargins m = m_layoutMargins[master];
          master->setContentsMargins(m);
        }
      }

    }
    break;
  }
}

void CollapsibleGroupBox::resizeCollapseButton(const QSize &size)
{
  const QScreen *scr = QGuiApplication::primaryScreen();
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

void CollapsibleGroupBox::resizeEvent(QResizeEvent *ev)
{
  resizeCollapseButton(ev->size());
}
