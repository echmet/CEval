#ifndef COLLAPSEEXPANDBUTTON_H
#define COLLAPSEEXPANDBUTTON_H

#include <QPushButton>

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
  void setState(const State state);
  State state() const;

protected:
  void paintEvent(QPaintEvent *ev);

private:
  State m_currentState;

  static const int LINE_WIDTH;

signals:
  void clicked(const Transition state);

private slots:
  void onClicked();

};

#endif // COLLAPSEEXPANDBUTTON_H
