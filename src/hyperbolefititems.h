#ifndef HYPERBOLEFITITEMS_H
#define HYPERBOLEFITITEMS_H

#include <QObject>

class HyperboleFitResults {
public:
  HyperboleFitResults() = delete;

  enum class Floating : int {
    SIGMA,
    MOBILITY_A,
    MOBILITY_CS_A,
    K_CS_A,
    MOBILITY_B,
    MOBILITY_CS_B,
    K_CS_B,
    ITERATIONS,
    LAST_INDEX
  };

};

class HyperboleFitParameters {
public:
  HyperboleFitParameters() = delete;

  enum class Boolean : int {
    FIXED_MOBILITY_A,
    FIXED_MOBILITY_CS_A,
    FIXED_K_CS_A,
    FIXED_MOBILITY_B,
    FIXED_MOBILITY_CS_B,
    FIXED_K_CS_B,
    LAST_INDEX
  };

  enum class Floating : int {
    EPSILON,
    VISCOSITY_SLOPE,
    LAST_INDEX
  };

  enum class Int : int {
    MAX_ITERATIONS,
    LAST_INDEX
  };

  enum class String : int {
    ANALYTE_A,
    ANALYTE_B,
    LAST_INDEX
  };

};

class HyperboleStats : public QObject {
  Q_OBJECT
public:
  HyperboleStats() = delete;
  HyperboleStats(QObject *parent) = delete;

  enum class Intervals {
    LEFT,
    RIGHT,
    BOTH,
    LAST_INDEX
  };
  Q_ENUM(Intervals)
};

#endif // HYPERBOLEFITITEMS_H

