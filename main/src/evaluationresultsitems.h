#ifndef EVALUATIONRESULTSITEMS_H
#define EVALUATIONRESULTSITEMS_H

class EvaluationResultsItems
{
public:
  enum class Floating : int {
    EOF_VELOCITY,
    EOF_MOBILITY,
    PEAK_FROM_X,
    PEAK_FROM_Y,
    PEAK_TO_X,
    PEAK_TO_Y,
    PEAK_X,
    PEAK_HEIGHT,
    PEAK_VELOCITY,
    PEAK_VELOCITY_EFF,
    PEAK_MOBILITY,
    PEAK_MOBILITY_EFF,
    PEAK_AREA,
    PEAK_HEIGHT_BL,
    WIDTH_HALF_MIN_LEFT,
    WIDTH_HALF_MIN_RIGHT,
    WIDTH_HALF_MIN_FULL,
    SIGMA_MIN_LEFT,
    SIGMA_MIN_RIGHT,
    SIGMA_MIN_FULL,
    WIDTH_HALF_MET_LEFT,
    WIDTH_HALF_MET_RIGHT,
    WIDTH_HALF_MET_FULL,
    SIGMA_MET_LEFT,
    SIGMA_MET_RIGHT,
    SIGMA_MET_FULL,
    N_LEFT,
    N_RIGHT,
    N_FULL,
    N_H_LEFT,
    N_H_RIGHT,
    N_H_FULL,
    VARIANCE_APEX,
    VARIANCE_MEAN,
    SIGMA_APEX,
    SIGMA_MEAN,
    MEAN_X,
    LAST_INDEX
  };

};

class HVLFitResultsItems
{
public:
  enum class Floating {
    HVL_A0,
    HVL_A1,
    HVL_A2,
    HVL_A3,
    HVL_S,
    HVL_EPSILON,
    HVL_TUSP,
    HVL_U_EFF_A1,
    LAST_INDEX
  };

};

#endif // EVALUATIONRESULTSITEMS_H

