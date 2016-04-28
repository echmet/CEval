#ifndef COLLAPSEEXPANDBUTTON_H
#define COLLAPSEEXPANDBUTTON_H

#include <QPushButton>

class QPen;

class CollapseExpandButton : public QPushButton
{
  Q_OBJECT
public:
  enum class State {
    COLLAPSED,
    EXPANDED
  };
  Q_ENUM(State)

  enum class Transition {
    COLLAPSE,
    EXPAND
  };
  Q_ENUM(Transition)

  explicit CollapseExpandButton(QWidget *parent = nullptr);
  ~CollapseExpandButton();
  void setState(const State state);
  State state() const;

protected:
  void paintEvent(QPaintEvent *ev);
  void resizeEvent(QResizeEvent *ev);

private:
  void buildPainterPaths(const QSize &size);
  State m_currentState;

  QPen *m_pen;
  QPainterPath m_collapsedPath;
  QPainterPath m_expandedPath;

  static const int LINE_WIDTH;

signals:
  void clicked(const Transition state);

private slots:
  void onClicked();

};

#endif // COLLAPSEEXPANDBUTTON_H
