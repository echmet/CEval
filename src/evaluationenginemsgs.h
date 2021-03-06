#ifndef EVALUATIONENGINEMSGS_H
#define EVALUATIONENGINEMSGS_H

#include <QObject>

class EvaluationEngineMsgs : public QObject
{
  Q_OBJECT
public:
  enum class CheckBox {
    HVL_SHOW_STATS,
    HVL_DISABLE_AUTO_FIT
  };
  Q_ENUM(CheckBox)

  enum class CopyToClipboard {
    EOFLOW,
    HVL,
    PEAK,
    PEAK_DIMS,
    EVERYTHING
  };
  Q_ENUM(CopyToClipboard)

  enum class ComboBox {
    BASELINE_ALGORITHM,
    DATA_FILES,
    SHOW_WINDOW,
    WINDOW_UNITS
  };
  Q_ENUM(ComboBox)

  enum class Default {
    FINDER_PARAMETERS
  };
  Q_ENUM(EvaluationEngineMsgs::Default)

  enum class Traverse {
    PREVIOUS,
    NEXT
  };
  Q_ENUM(Traverse)

  struct ComboBoxNotifier {
    ComboBoxNotifier(const ComboBox id, const int value) :
      id(id), value(value) {}
    ComboBox id;
    int value;
  };

};

#endif // EVALUATIONENGINEMSGS_H
