#include "collapsiblegroupbox.h"
#include "collapseexpandbutton.h"
#include <QLayout>
#include <QResizeEvent>

#include <QDebug>

CollapsibleGroupBox::CollapsibleGroupBox(QWidget *parent) :
  QGroupBox(parent)
{

  qDebug() << "CGBox" << this->width();

  m_clExpButton = new CollapseExpandButton(this);
  m_clExpButton->setGeometry(this->width() - 25, 0, 25, 25);

  connect(m_clExpButton, &CollapseExpandButton::clicked, this, &CollapsibleGroupBox::onVisibilityChanged);
}

void CollapsibleGroupBox::resizeEvent(QResizeEvent *ev)
{
  m_clExpButton->setGeometry(ev->size().width() - 25, 0, 25, 25);
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
