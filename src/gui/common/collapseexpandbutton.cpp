#include "collapseexpandbutton.h"
#include <QPainter>

const int CollapseExpandButton::LINE_WIDTH(2);

CollapseExpandButton::CollapseExpandButton(QWidget *parent) :
  QPushButton(parent),
  m_currentState(State::EXPANDED)
{
  this->QPushButton::setText("");

  connect(this, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &CollapseExpandButton::onClicked);
}

void CollapseExpandButton::paintEvent(QPaintEvent *ev)
{
  const int marginLR = floor((this->width() * 0.2) + 0.5);
  const int marginUD = floor((this->height() * 0.2) + 0.5);
  QPoint from;
  QPoint through;
  QPoint to;

  if (m_currentState == State::EXPANDED) {
    const int middle = floor((this->width() / 2.0) + 0.5);
    from = QPoint(marginLR, marginUD);
    through = QPoint(middle, this->height() - marginUD);
    to = QPoint(this->width() - marginLR, marginUD);
  } else {
    const int middle = floor((this->height() / 2.0) + 0.5);
    from = QPoint(this->width() - marginLR, marginUD);
    through = QPoint(marginLR, middle);
    to = QPoint(this->width() - marginLR, this->height() - marginUD);
  }

  QPushButton::paintEvent(ev);

  const QPalette &pal = this->palette();

  QPen pen(pal.color(QPalette::WindowText));
  pen.setWidth(LINE_WIDTH);

  QPainter p(this);
  p.setPen(pen);
  p.setRenderHint(QPainter::Antialiasing, true);

  p.drawLine(from, through);
  p.drawLine(through, to);
}

void CollapseExpandButton::onClicked()
{
  if (m_currentState == State::COLLAPSED) {
    m_currentState = State::EXPANDED;
    emit clicked(Transition::EXPAND);
  } else {
    m_currentState = State::COLLAPSED;
    emit clicked(Transition::COLLAPSE);
  }

  update();
}

void CollapseExpandButton::setState(const State state)
{
  m_currentState = state;

  repaint();
}

CollapseExpandButton::State CollapseExpandButton::state() const
{
  return m_currentState;
}
