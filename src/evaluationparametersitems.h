#ifndef EVALUATIONPARAMETERSITEMS_H
#define EVALUATIONPARAMETERSITEMS_H

#include <QObject>

class EvaluationParametersItems : public QObject {
  Q_OBJECT
public:
  EvaluationParametersItems() = delete;
  EvaluationParametersItems(QObject *parent) = delete;

  enum class Auto : int {
    SLOPE_WINDOW,
    NOISE,
    SLOPE_REF_POINT,
    SLOPE_THRESHOLD,
    FROM,
    TO,
    LAST_INDEX
  };

  enum class Boolean : int {
    NOISE_CORRECTION,
    DISTURBANCE_DETECTION,
    LAST_INDEX
  };

  enum class Floating : int {
    NOISE_WINDOW,
    PEAK_WINDOW,
    SLOPE_WINDOW,
    NOISE,
    NOISE_LOD_COEFF,
    SLOPE_SENSITIVITY,
    SLOPE_THRESHOLD,
    NOISE_REF_POINT,
    SLOPE_REF_POINT,
    FROM,
    TO,
    DISTURBANCE_DETECTION,
    LAST_INDEX
  };

  enum class ComboWindowUnits : int {
    MINUTES,
    POINTS,
    LAST_INDEX
  };
  Q_ENUM(ComboWindowUnits)

  enum class ComboBaselineAlgorithm : int {
    SLOPE,
    NOISE,
    LAST_INDEX
  };
  Q_ENUM(ComboBaselineAlgorithm)

  enum class ComboShowWindow : int {
    NONE,
    NOISE,
    SLOPE_THRESHOLD,
    PEAK_WINDOW,
    PEAK_HEIGHT,
    PEAK_WIDTH_HALF,
    PEAK_WIDTH_005,
    BASELINE,
    SMOOTHED,
    FIRST_DERIVATIVE,
    SECOND_DERIVATIVE,
    BOTH_DERIVATIVES,
    LAST_INDEX
  };
  Q_ENUM(ComboShowWindow)

  static constexpr int index(const ComboBaselineAlgorithm item)
  {
    return static_cast<int>(item);
  }

  static constexpr int index(const ComboWindowUnits item)
  {
    return static_cast<int>(item);
  }

  static constexpr int index(const ComboShowWindow item)
  {
    return static_cast<int>(item);
  }

  template <typename T>
  static T fromIndex(const int idx)
  {
    Q_ASSERT(idx >= 0 && idx < static_cast<int>(T::LAST_INDEX));

    return static_cast<T>(idx);
  }

};

class HVLFitParametersItems {
public:
  enum class Boolean {
    HVL_A0,
    HVL_A1,
    HVL_A2,
    HVL_A3,
    LAST_INDEX
  };

  enum class Int {
    ITERATIONS,
    DIGITS,
    LAST_INDEX
  };

};

class HVLFitOptionsItems {
public:
  enum class Boolean {
    DISABLE_AUTO_FIT,
    SHOW_FIT_STATS,
    LAST_INDEX
  };

};

#endif // EVALUATIONPARAMETERSITEMS_H

