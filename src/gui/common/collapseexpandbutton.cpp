#include "collapseexpandbutton.h"
#include <QPainter>
#include <QPen>
#include <QResizeEvent>
#include <cmath>

const int CollapseExpandButton::LINE_WIDTH(2);

CollapseExpandButton::CollapseExpandButton(QWidget *parent) :
  QPushButton(parent),
  m_currentState(State::EXPANDED)
{
  this->QPushButton::setText("");

  const QPalette &pal = this->palette();

  m_pen = new QPen(pal.color(QPalette::WindowText));
  m_pen->setWidth(LINE_WIDTH);

  buildPainterPaths(this->size());

  connect(this, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &CollapseExpandButton::onClicked);
}

CollapseExpandButton::~CollapseExpandButton()
{
  delete m_pen;
}

void CollapseExpandButton::buildPainterPaths(const QSize &size)
{
  const int marginLR = floor((size.width() * 0.3) + 0.5);
  const int marginUD = floor((size.height() * 0.3) + 0.5);
  const int middleColl = floor((size.height() / 2.0) + 0.5);
  const int middleExpd = floor((size.width() / 2.0) + 0.5);

  m_collapsedPath = QPainterPath(QPointF(size.width() - marginLR, marginUD));
  m_collapsedPath.lineTo(marginLR, middleColl);
  m_collapsedPath.lineTo(size.width() - marginLR, size.height() - marginUD);

  m_expandedPath = QPainterPath(QPointF(marginLR, marginUD));
  m_expandedPath.lineTo(middleExpd, size.height() - marginUD);
  m_expandedPath.lineTo(size.width() - marginLR, marginUD);
}

void CollapseExpandButton::paintEvent(QPaintEvent *ev)
{
  QPushButton::paintEvent(ev);

  QPainter p(this);
  p.setPen(*m_pen);
  p.setRenderHint(QPainter::Antialiasing, true);

  switch (m_currentState) {
  case State::COLLAPSED:
    p.fillPath(m_collapsedPath, m_pen->brush());
    break;
  case State::EXPANDED:
    p.fillPath(m_expandedPath, m_pen->brush());
    break;
  }
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

void CollapseExpandButton::resizeEvent(QResizeEvent *ev)
{
  buildPainterPaths(ev->size());
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
