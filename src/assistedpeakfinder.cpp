#include "assistedpeakfinder.h"
#include "helpers.h"
#include "foundpeaksmodel.h"
#include <QMessageBox>

#define SEARCH_I(PARAM){\
  t##PARAM##i = (t##PARAM - XMin) * ppm;\
  if (t##PARAM##i < 0) t##PARAM##i = 0;\
  if (t##PARAM##i >= C) t##PARAM##i = C - 1;\
  if (Data.at(t##PARAM##i).x() < t##PARAM)\
    for (long CC = C-1;t##A##i < CC && Data.at(t##PARAM##i).x() < t##PARAM; ++t##PARAM##i);\
 else\
    while (t##PARAM##i > 0 && Data.at(t##PARAM##i).x() >= t##PARAM) --t##PARAM##i;\
}

AssistedPeakFinder::Parameters::Parameters(const QVector<QPointF> &data) :
  data(data)
{
}

AssistedPeakFinder::TSearchHandler::TSearchHandler(const QVector<QPointF> &Data, long Begin, long End) :
  m_Data(Data), m_Begin(Begin), m_End(End)
{
}

AssistedPeakFinder::TSearchHandler::TSearchHandler(TSearchHandler &T) :
  m_Data(T.m_Data), m_Begin(T.m_Begin), m_End(T.m_End)
{
}

double AssistedPeakFinder::TSearchHandler::GetX(long I)
{
  Q_ASSERT(I + m_Begin < m_Data.length());

  return m_Data[I + m_Begin].x();
}

double AssistedPeakFinder::TSearchHandler::GetY(long I)
{
  Q_ASSERT(I + m_Begin < m_Data.length());

  return m_Data[I + m_Begin].y();
}

long AssistedPeakFinder::TSearchHandler::Count()
{
  return (m_End == -1 ? m_Data.size() : m_End) - m_Begin;
}

AssistedPeakFinder::TPeaksSearcher::TPeaksSearcher(TSearchHandler *S, long ChainPoints, double Noise, bool LeftBoundary, bool RightBoundary) :
  TExtremeSearcher(S, ChainPoints, Noise, LeftBoundary, RightBoundary)
{
}

AssistedPeakFinder::TPeaksSearcher::TPeaksSearcher(TPeaksSearcher &T) :
  TExtremeSearcher(T)
{
}

void AssistedPeakFinder::TPeaksSearcher::Search()
{
  Extremes.clear();
  TExtremeSearcher::Search();
}

void AssistedPeakFinder::TPeaksSearcher::OnMaximum(double Value, long Index)
{
  Q_UNUSED(Value);

  TSearchHandler * h = dynamic_cast<TSearchHandler *>(Data);

  if (Index == 0 || Index == h->Count() - 1) return;
  Extremes.push_back(Index + h->m_Begin);
}

void AssistedPeakFinder::TPeaksSearcher::OnMinimum(double Value, long Index)
{
  Q_UNUSED(Value);

  TSearchHandler * h = dynamic_cast<TSearchHandler *>(Data);

  if (Index == 0 || Index == h->Count() - 1) return;
  Extremes.push_back(Index + h->m_Begin);
}

bool AssistedPeakFinder::checkBounds(const unsigned long i, const QVector<QPointF> &data)
{
  if (i >= static_cast<unsigned long>(data.length())) {
    QMessageBox::critical(nullptr, QObject::tr("Logic error"), QString(QObject::tr("Data index out of bounds, idx = %1, data length = %2\n"
                                                                                   "Check evaluation parameters.")).arg(i).arg(data.length()));
    return false;
  }

  return true;
}

PeakFinderResults AssistedPeakFinder::findInternal(const AbstractParameters &ap) throw (std::bad_cast)
{
  const Parameters &p = dynamic_cast<const Parameters&>(ap);
  double MaxValue, MinValue, SummValue;
  double SummX, SummXX, SummY, SummXY;
  long diL, diR;
  long tBEGi, tENDi, tnrpi, tsrpi, tAi, tBi, tPi;
  double tBEG, tEND, tnrp, tsrp, tA, tB, tP;
  long twPLefti, twPRighti;
  double twPLeft, twPRight;
  double HP, HP_, HA, HB;
  double BSLSlope, BSLIntercept;
  std::shared_ptr<QVector<QPointF>> seriesA;
  std::shared_ptr<QVector<QPointF>> seriesB;
  PeakFinderResults r;

  try {
    seriesA = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>());
    seriesB = std::shared_ptr<QVector<QPointF>>(new QVector<QPointF>());
  } catch (std::bad_alloc&) {
    QMessageBox::warning(nullptr, QObject::tr("Insufficient memory"), QObject::tr("Cannot find peak"));
    return PeakFinderResults();
  }

  const QVector<QPointF> &Data = p.data;
  const long C = Data.size();

  if (C == 0)
    return PeakFinderResults();

  double XMin = Data.front().x();
  double XMax = Data.back().x();
  double YMin = Helpers::minYValue(Data);
  double YMax = Helpers::maxYValue(Data);

  if (XMax == XMin)
    return PeakFinderResults();

  double ppm = C / (XMax - XMin); /* Points per minute */

  double SlopeThreshold, SlopeSensitivity; /* Declare here and use as synonyme for noise calculations*/

  /* Initialize settings */
  if (p.autoFrom) {
    tBEGi = 0;
    tBEG = XMin;
  } else {
    long idx = 0;
    tBEG = p.from;
    while (Data.at(idx).x() < tBEG)
      idx++;

    tBEGi = idx;

    //SEARCH_I(BEG)

    if (!checkBounds(tBEGi, Data))
        return PeakFinderResults();
  }

  if (p.autoTo) {
    tENDi = C;
    tEND = XMax;
  } else {
    long idx = Data.size() - 1;
    tEND = p.to;
    while (Data.at(idx).x() > tEND)
      idx--;

    tENDi = idx;
    //SEARCH_I(END)
    //++tENDi;

    if (!checkBounds(tENDi, Data))
        return PeakFinderResults();
  }

  long NoiseWindow;
  if (p.windowUnits == EvaluationParametersItems::ComboWindowUnits::MINUTES)
    NoiseWindow = p.noiseWindow * ppm;
  else
    NoiseWindow = p.noiseWindow;

  long PeakWindow;
  if (p.windowUnits == EvaluationParametersItems::ComboWindowUnits::MINUTES)
    PeakWindow = p.peakWindow * ppm;
  else
    PeakWindow = p.peakWindow;

  long SlopeWindow; /* Calculated later, see below */

   /* * Inicialiace::Peaks & BSL * */
  /* Initialize peaks and baseline */
  tA = tBEG;
  tAi = tBEGi;

  tP = tA;
  tPi = tAi;

  tB = tEND;
  tBi = tENDi - 1;

  /* Initialize noise reference point */
  tnrp = p.noisePoint;
  if (tnrp > tEND) {
    QMessageBox::information(nullptr, QObject::tr("Incorrent parameters"), QString(QObject::tr("Noise point reference (%1) is beyond the last time point"
                                                                                               "in the data file (%2). Clamping to %1").arg(tnrp).arg(tEND)));
    tnrp = tEND;
  }
  SEARCH_I(nrp)
  if (!checkBounds(tnrpi, Data))
    return PeakFinderResults();

  /* Noise */
  double Noise;
  #define SLOPE BSLSlope
  #define INTERCEPT BSLIntercept /* Used only if we need to draw noise window */
  if (p.autoNoise || p.showWindow == EvaluationParametersItems::ComboShowWindow::NOISE) {
    diL = tnrpi - (NoiseWindow / 2); if (diL < 0) diL = 0;
    diR = diL + NoiseWindow; if (diR > C) diR = C;

    if (p.noiseCorrection) {
      #define _X MaxValue
      #define _Y MinValue

      SummX = 0.; SummXX = 0.; SummY = 0.; SummXY = 0.;
      for (long i = diL; i < diR; ++i) {
        _X = Data[i].x(); _Y = Data[i].y();
        SummX += _X; SummXX += _X*_X; SummY += _Y; SummXY += _X*_Y;
      }

      if ((_Y = ((diR-diL) * SummXX - SummX * SummX)))
        _X = ((diR-diL) * SummXY - SummX * SummY) / _Y;
      else
        _X = 0.0;
      if (_Y)
        _Y = (SummY * SummXX - SummX * SummXY) / _Y;
      else
        _Y = 0.0;

      SLOPE = _X;
      INTERCEPT = _Y;

      #undef _X
      #undef _Y
    } //Noise::ChcbNoiseCorrection

    MinValue = YMax;
    MaxValue = YMin;
    SummValue = 0.0; /* Used only if we need to draw noise window */
    for (long i = diL; i < diR; ++i) {
      if (p.noiseCorrection)
        Noise = fabs(Data[i].y() - SLOPE * Data[i].x());
      else
        Noise = fabs(Data[i].y());
      //Neni treba uvazovat Intercept, zbytecna operace scitani,
      //Noise = Max - Min => Nezalezi na konstante.
     //Nemusi byt if ... else, ale lepsi vyhodnocovat Checked, nez operace "*,-" s double
      if (Noise > MaxValue)
        MaxValue = Noise;
      if (Noise < MinValue)
        MinValue = Noise;
      SummValue += Noise;
    }
  } //Noise::(ReadOnly || CbxWindow == wkNoise)

  if (p.autoNoise)
    Noise = (MaxValue - MinValue);
  else
    Noise = p.noise;

  double Noise_2 = Noise / 2.;

  /* Noise::SystemWindow */
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::NOISE) {
    #define XL MinValue
    #define XR MaxValue
    XL = Data[diL].x();
    XR = Data[diR-1].x();
    if (!p.noiseCorrection) {
      SLOPE = 0.0;
      if (diR != diL) INTERCEPT = SummValue / (diR - diL);
      else INTERCEPT = Data[diL].y();
    }

    seriesA->push_back(QPointF(XL, SLOPE * XL + INTERCEPT - Noise_2));
    seriesA->push_back(QPointF(XR, SLOPE * XR + INTERCEPT - Noise_2));
    seriesB->push_back(QPointF(XL, SLOPE * XL + INTERCEPT - Noise_2));
    seriesB->push_back(QPointF(XR, SLOPE * XR + INTERCEPT - Noise_2));
    /*
    USD.Add(ResultsEvaluator::XYPair(XL, SLOPE * XL + INTERCEPT - Noise_2), SRsystem1);
    USD.Add(ResultsEvaluator::XYPair(XR, SLOPE * XR + INTERCEPT - Noise_2), SRsystem1);
    USD.Add(ResultsEvaluator::XYPair(XL, SLOPE * XL + INTERCEPT - Noise_2), SRsystem2);
    USD.Add(ResultsEvaluator::XYPair(XR, SLOPE * XR + INTERCEPT - Noise_2), SRsystem2);
    */

    #undef XL
    #undef XR
  } // Noise::SystemWindow
  #undef SLOPE
  #undef INTERCEPT

  /* Slope Reference Point */
  if (p.autoSlopeRefPoint) {
    tsrp = tnrp;
    tsrpi = tnrpi;
  } else {
    tsrp = p.slopeRefPoint;
    SEARCH_I(srp)

    if (!checkBounds(tsrpi, Data))
      return PeakFinderResults();
  }

  /* SlopeThreshold, SlopeSensitivity */
  // Musi byt az po Noise - viz synonyma SLOPE a INTERCEPT pri vypoctu Noise

  SlopeSensitivity = p.slopeSensitivity / 100.;

  if (p.autoSlopeThreshold || p.showWindow == EvaluationParametersItems::ComboShowWindow::SLOPE_THRESHOLD) {
    #define _X MaxValue
    #define _Y MinValue
    diL = tsrpi - (NoiseWindow / 2);
    if (diL < 0)
      diL = 0;

    diR = diL + NoiseWindow;
    if (diR > C)
      diR = C;

    SummX = 0.0; SummXX = 0.0; SummY = 0.0; SummXY = 0.0;
    for (long i = diL; i < diR; ++i) {
      _X = Data[i].x();
      _Y = Data[i].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;
    }

    if ((_Y = ((diR - diL)* SummXX - SummX * SummX)))
      _X = ((diR - diL) * SummXY - SummX * SummY) / _Y;
    else
      _X = 0.0;

    if (_Y)
      _Y = (SummY * SummXX - SummX * SummXY) / _Y;
    else
      _Y = 0.0;
  } //SlopeTreshold::(ReadOnly || SystemWindow)

  /* SlopeThreshold::ReadOnly */
  if (p.autoSlopeThreshold)
    SlopeThreshold = _X;
  else
    SlopeThreshold = p.slopeThreshold;

  /* * SlopeTreshold::SystemWindow * */
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::SLOPE_THRESHOLD) {
    _X = Data[diL].x();
    seriesA->push_back(QPointF(_X, (SlopeThreshold * _X) + _Y));
    _X = Data[diR-1].x();
    seriesB->push_back(QPointF(_X, (SlopeThreshold * _X) + _Y));
  }
  #undef _X
  #undef _Y

  /* Time of peak */
  {
    TSearchHandler Handler(Data, tPi);
    TPeaksSearcher Searcher(&Handler, PeakWindow, Noise * p.noiseCoefficient);
    Searcher.Search();

    /* Peak lookup complete, let`s check the resuts */
    {
      TPeaksSearcher::container_type &maxima = Searcher.Extremes;

      /* No peak has been found */
      if (maxima.size() == 0) {
        QMessageBox::information(nullptr, QObject::tr("No peak"), QObject::tr("No peaks were found for the given parameters."));
        return PeakFinderResults();
      }
      /* Only one peak has been found, select it right away */
      else if (maxima.size() == 1)
        tPi = maxima.at(0);
      /* More than one peak have been found */
      else {
        if (p.inTPiCoarse < 0 || p.inTPiCoarse > Data.length() - 1) {
          QVector<FoundPeaksModel::Peak> modelData;
          int dlgRet;
          int peakNumber;
          bool ok;

          for (const long idx : maxima)
            modelData.push_back(FoundPeaksModel::Peak(Data.at(idx).x(), idx));

          FoundPeaksModel model(modelData);
          p.selPeakDialog->bindModel(&model);
          p.selPeakDialog->setPeakWindow(PeakWindow);
          dlgRet = p.selPeakDialog->exec();
          emit p.selPeakDialog->closedSignal();
          if (dlgRet != QDialog::Accepted)
            return PeakFinderResults();

          peakNumber = p.selPeakDialog->selectedPeak();
          if (peakNumber < 0 || peakNumber >= modelData.length()) {
            QMessageBox::warning(nullptr, QObject::tr("Runtime error"), QObject::tr("Invalid index for selected peak."));
            return PeakFinderResults();
          }

          tPi = model.data(model.index(peakNumber, 0), Qt::UserRole + 1).toInt(&ok);
          if (!ok) {
            QMessageBox::warning(nullptr, QObject::tr("Runtime error"), QObject::tr("Invalid data for selected peak"));
            return PeakFinderResults();
          }
          if (tPi < 0 || tPi >= Data.length()) {
            QMessageBox::warning(nullptr, QObject::tr("Runtime error"), QString(QObject::tr("Peak's tPi (%1) is out of bounds <0; %2>")).arg(tPi).arg(Data.length()));
            return PeakFinderResults();
          }
        } else {
        tPi = p.inTPiCoarse;
        }
      }
    }

    tP = Data[tPi].x();
    r.tPiCoarse = tPi;
  } // Time::ReadOnly

  /* * SlopeWindow (Peak top) , H, zpresneni TP* */

  /* * (SlopeWindow , HP, TP)::Hledani okna * */
  if (p.autoSlopeWindow ||
      p.showWindow == EvaluationParametersItems::ComboShowWindow::PEAK_HEIGHT /* SystemWindows::PEAK_TOP */) {
    diR = tPi;
    diL = tPi;

    bool diLProgress = true;
    bool diRProgress = true;
    SummValue = Data[tPi].y();
    MaxValue = SummValue;
    MinValue = SummValue;

    while (diLProgress || diRProgress) {
      double MeanValue, OldSumm;

      diRProgress = diR + 1 < tENDi;
      if (diRProgress) {
        OldSumm = SummValue;
        ++diR;
        HP = Data[diR].y();

        if (HP < MinValue)
          MinValue = HP;
        if (HP > MaxValue)
          MaxValue = HP;

        SummValue += HP;
        MeanValue = SummValue / (diR - diL + 1); //+1 == tPi

        if (MeanValue - MinValue > Noise_2 || MaxValue - MeanValue > Noise_2) {
          diRProgress = false;
          --diR;
          SummValue = OldSumm;
        } //(SlopeWindow || HP || TP)::Okno::while::diRProgress::if Noise !OK
      } //(SlopeWindow || HP || TP)::Okno::while::if diRPRogress

      diLProgress = diL > tBEGi;
      if (diLProgress) {
        OldSumm = SummValue;
        --diL;
        HP = Data[diL].y();
        if (HP < MinValue)
          MinValue = HP;
        if (HP > MaxValue)
          MaxValue = HP;

        SummValue += HP;
        MeanValue = SummValue / (diR - diL + 1); //+1 == tPi
        if (MeanValue - MinValue > Noise_2 || MaxValue - MeanValue > Noise_2) {
          diLProgress = false;
          ++diL;
          SummValue = OldSumm;
        } //(SlopeWindow || HP || TP)::Okno::while::diLProgress::if !Noise OK
      } //(SlopeWindow || HP || TP)::Okno::while::if diLProgress
    } //(SlopeWindow || HP || TP)::Okno::while Progress

    ++diR; //> "Iterator" diR == end
  } //(SlopeWindow || HP || TP)::Okno

  /* * (SlopeWindow || HP || TP)::SlopeWindow * */
  if (p.autoSlopeWindow)
    SlopeWindow = (diR - diL) * 2.;
  else {
    if (p.windowUnits == EvaluationParametersItems::ComboWindowUnits::MINUTES)
      SlopeWindow = p.slopeWindow * ppm;
    else
      SlopeWindow = p.slopeWindow;
  }

  /* * (SlopeWindow || HP || TP)::HP * */
  HP = SummValue / (diR - diL);

  /* * (SlopeWindow || HP || TP)::TP * */
  // Algoritmus > Vycentrovani tP vzhledem k nalezenemu oknu
  tP = (Data[diL].x() + Data[diR-1].x()) / 2;
  SEARCH_I(P);

  if (!checkBounds(tPi, Data))
    return PeakFinderResults();

    #ifdef DISABLED
      // Algiritmus > smernice == 0
      #define SLOPE SummValue
      #define _X MaxValue
      #define _Y MinValue

      //(SlopeWindow || HP || TP)::TP::Init
      long CC = tENDi - 1;
      diR = tPi + SlopeWindow / 2;
      if (diR > CC)
        diR = CC;

      diL = diR - SlopeWindow;
      if (diL < tBEGi)
        diL = 0;

      t1i = (diR + diL) / 2;
      SummX = 0.; SummXX = 0.; SummY = 0.; SummXY = 0.;
      for (long i = diR; i > diL; --i) {
        _X = Data[i].X;
        _Y = Data[i].Y;
        SummX += _X; SummXX += _X*_X; SummY += _Y; SummXY += _X*_Y;
      }

      if (SLOPE = ((diR-diL) * SummXX - SummX * SummX))
        SLOPE = ((diR-diL) * SummXY - SummX * SummY) / SLOPE;
      else
        SLOPE = 0.0;

      //(SlopeWindow || HP || TP)::TP::Slope > 0.
      if (SLOPE > 0.0) {
        while (diR < CC && SLOPE > 0.0) {
          ++diR;
          ++diL;
          ++t1i;

          _X = Data[diL].x();
          _Y = Data[diL].y();
          SummX -= X;
          SummXX -= X*X;
          SummY -= Y;
          SummXY -= X*Y;

          _X = Data[diR].x();
          _Y = Data[diR].y();
          SummX += _X;
          SummXX += _X*_X;
          SummY += _Y;
          SummXY += _X*_Y;

          if (SLOPE = (SlopeWindow * SummXX - SummX * SummX))
            SLOPE = (SlopeWindow * SummXY - SummX * SummY) / SLOPE;
          else
            SLOPE = 0.0;

      } //(SlopeWindow || HP || TP)::TP::if SLOPE > 0::While
    } //(SlopeWindow || HP || TP)::TP::if SLOPE > 0
    else if (SLOPE < 0.0) {
      while (diL > tBEGi && SLOPE < 0.0) {
        _X = Data[diR].x();
        _Y = Data[diR].y();
        SummX -= _X;
        SummXX -= _X*_X;
        SummY -= _Y;
        SummXY -= _X*_Y;
        --diR;
        --diL;
        --t1i;

        _X = Data[diL].x();
        _Y = Data[diL].y();
        SummX += _X;
        SummXX += _X*_X;
        SummY += _Y;
        SummXY += _X*_Y;

        if (SLOPE = (SlopeWindow * SummXX - SummX * SummX))
          SLOPE = (SlopeWindow * SummXY - SummX * SummY) / SLOPE;
        else SLOPE = 0.0;
      } //(SlopeWindow || HP || TP)::T1::if SLOPE < 0::While
    } //(SlopeWindow || HP || TP)::T1::if SLOPE <0
    //else; //blank else

    #undef SLOPE
    #undef X
    #undef Y
    tP = Data[tPi].x();
  #endif //DISABLED

  /* * (SlopeWindow || HP || TP)::SystemWindow * */
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::PEAK_HEIGHT /* LkP.Window == SystemWindows::PEAK_TOP */) {
    MinValue = Data[diL].x();
    MaxValue = Data[diR-1].x();

  seriesA->push_back(QPointF(MinValue, HP - Noise_2));
  seriesA->push_back(QPointF(MaxValue, HP - Noise_2));
  seriesB->push_back(QPointF(MinValue, HP - Noise_2));
  seriesB->push_back(QPointF(MaxValue, HP - Noise_2));
  }

  /* * BSL A * */
  //BSL A musi byt zde, pro BSL B se muze pocitat nove SlopeWindow pro T2
  //T2 se pocita v EvalChiru - zde ne, ale radsi to nemenim...

  /* * BSL A::tA * */
  if (p.baselineAlgorithm == EvaluationParametersItems::ComboBaselineAlgorithm::NOISE) {
    diL = tPi - NoiseWindow;
    if (diL < tBEGi)
      diL = tBEGi;

    diR = diL + NoiseWindow;
    if (diR > tENDi)
      diR = tENDi;

    while (
      diL > tBEGi &&
      Data[--diR].y() - Data[--diL].y() > Noise_2
    );

    tAi = (diR + diL)  / 2;
  } else { //BSL A::tA::Algirithm == Noise
    // toto usporadani plati pro budouci posun Window k NIZSIM hodnotam t
    // Priorita zachovani sirky SlopeWindow
    diL = tPi - (SlopeWindow / 2);
    if (diL < tBEGi)
      diL = tBEGi;

    diR = diL + SlopeWindow;
    if (diR > tENDi) {
      diR = tENDi;
      diL = diR - SlopeWindow;
      if (diL < tBEGi)
        diL = tBEGi;
    }

    #define SLOPE tA
    #define OLD SummValue
    EState State = stTop;

    #define _X MaxValue
    #define _Y MinValue
    SummX = 0.; SummXX = 0.; SummY = 0.; SummXY = 0.;
    for (long i = diL; i < diR; ++i) {
      _X = Data[i].x();
      _Y = Data[i].y();
      SummX += _X; SummXX += _X*_X; SummY += _Y; SummXY += _X*_Y;
    }

    if ((SLOPE = ((diR-diL) * SummXX - SummX * SummX)))
      SLOPE = ((diR-diL) * SummXY - SummX * SummY) / SLOPE;
    else
      SLOPE = 0.0;

    while (diL > tBEGi && State != stBottom) {
      --diL;
      --diR;
      _X = Data[diR].x();
      _Y = Data[diR].y();
      SummX -= _X;
      SummXX -= _X*_X;
      SummY -= _Y;
      SummXY -= _X*_Y;
      OLD = SLOPE;

      _X = Data[diL].x();
      _Y = Data[diL].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;
      if ((SLOPE = (SlopeWindow * SummXX - SummX * SummX)))
        SLOPE = (SlopeWindow * SummXY - SummX * SummY) / SLOPE;
      else SLOPE = 0.0;

      if ((State == stTop && SLOPE > OLD && 2*(SLOPE - OLD) / (SLOPE + OLD) >= SlopeSensitivity) ||
          (State == stBeforeInflex && SLOPE < OLD && 2*(OLD - SLOPE) / (SLOPE + OLD) >= SlopeSensitivity) ||
          (State == stAfterInflex && (SLOPE <= SlopeThreshold ||
                                      SLOPE < 0 ||
                                      2*fabs(SLOPE - OLD) / (SLOPE + OLD) < SlopeSensitivity)))
        State = static_cast<EState>(static_cast<std::underlying_type<EState>::type>(State) + 1);

    } //BSL::tA::Algirithm == Slope::while State != stBottom
    #undef SLOPE
    #undef OLD

    tAi = diR - SlopeWindow / 2;
    if (tAi < tBEGi)
      tAi = tBEGi;
  } //else BSL:tA::Algirithm == Noise

  tA = Data[tAi].x();

  /* * BSL A::HA  * */
  if (p.baselineAlgorithm == EvaluationParametersItems::ComboBaselineAlgorithm::NOISE) {
    diL = tAi - NoiseWindow / 2;
    diR  = diL + NoiseWindow;
  } else {
    diL = tAi - SlopeWindow / 2;
    diR  = diL + SlopeWindow;
  }
  if (diL < tBEGi)
    diL = tBEGi;

  if (diR > tENDi)
    diR = tENDi;

  HA = Data[diL].y();
  for (long i = diL+1; i < diR; ++i)
    HA += Data[i].y();

  if (diR != diL)
    HA /= (diR - diL);

  /* * BSL B * */
  //BSL B musi byt zde, pro BSL B se muze pocitat nove SlopeWindow pro T2
  //T2 se pocita v EvalChiru - zde ne, ale radsi to nemenim...

  /* * BSL B::tB * */
  if (p.baselineAlgorithm == EvaluationParametersItems::ComboBaselineAlgorithm::NOISE) {
    diR = tPi + NoiseWindow; if (diR > tENDi) diR = tENDi;
    diL = diR - NoiseWindow; if (diL < tBEGi) diL = tBEGi;
    while ( diR < tENDi &&
            (Data[diL++].y() - Data[diR++].y()) > Noise_2
           );

   tBi = (diR + diL)  / 2;
  } else { //BSL B::tB::Algirithm == Noise
    // toto usporadani plati pro budouci posun Window k VYSSIM hodnotam t
    // Priorita zachovani sirky SlopeWindow
    const long CC = tENDi - 1;
    diR = tPi + (SlopeWindow / 2);
    if (diR > CC)
      diR = CC;

    diL = diR - SlopeWindow;
    if (diL < -1) {
      diL = -1;
      diR = diL + SlopeWindow;
      if (diR > CC)
        diR = CC;
    }

    #define SLOPE tB
    #define OLD SummValue

    EState State = stTop;

    #define _X MaxValue
    #define _Y MinValue
    SummX = 0.; SummXX = 0.; SummY = 0.; SummXY = 0.;
    for (long i = diR; i > diL; --i) {
      _X = Data[i].x();
      _Y = Data[i].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;
    }

    if ((SLOPE = ((diR-diL) * SummXX - SummX * SummX)))
      SLOPE = ((diR-diL) * SummXY - SummX * SummY) / SLOPE;
    else
      SLOPE = 0.0;

    while (diR < CC &&  State != stBottom) {
      ++diR;
      ++diL;
      _X = Data[diL].x();
      _Y = Data[diL].y();
      SummX -= _X;
      SummXX -= _X*_X;
      SummY -= _Y;
      SummXY -= _X*_Y;
      OLD = SLOPE;

      _X = Data[diR].x();
      _Y = Data[diR].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;

      if ((SLOPE = (SlopeWindow * SummXX - SummX * SummX)))
        SLOPE = (SlopeWindow * SummXY - SummX * SummY) / SLOPE;
      else SLOPE = 0.0;

      if ((State == stTop && SLOPE < OLD && 2*(OLD - SLOPE) / (SLOPE + OLD) >= SlopeSensitivity) ||
          (State == stBeforeInflex && SLOPE > OLD && 2*(SLOPE - OLD) / (SLOPE + OLD) >= SlopeSensitivity) ||
          (State == stAfterInflex && (SLOPE >= SlopeThreshold || SLOPE > 0 || 2*fabs(SLOPE - OLD) / (SLOPE + OLD) < SlopeSensitivity)))
        State = static_cast<EState>(static_cast<std::underlying_type<EState>::type>(State) + 1);

    } //BSL::tB::Algirithm == Slope::while State != stBottom
    #undef SLOPE
    #undef OLD

    tBi = diL + SlopeWindow / 2;
    if (tBi >= tENDi)
      tBi = tENDi - 1;

  } //else BSL:tB::Algirithm == Noise

    tB = Data[tBi].x();
  //BSL::tB::ReadOnly
  // else bylo v Inicializaci

  /* * BSL B::HB  * */
  if (p.baselineAlgorithm == EvaluationParametersItems::ComboBaselineAlgorithm::NOISE) {
      diL = tBi - NoiseWindow / 2;
      diR  = diL + NoiseWindow;
  } else {
    diL = tBi - SlopeWindow / 2;
    diR  = diL + SlopeWindow;
  }
  if (diL < tBEGi)
    diL = tBEGi;

  if (diR > tENDi)
    diR = tENDi;

  HB = Data[diL].y();
  for (long i = diL+1; i < diR; ++i)
    HB += Data[i].y();

  if (diR != diL)
    HB /= (diR - diL);

  /* * Kresleni SystemWindow : Peak(s) * */
  //az ted, times mohly byt jeste zpresneny pri vypoctu H
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::PEAK_WINDOW) {
    diL = tPi - PeakWindow / 2;
    diR  = tPi + PeakWindow / 2 + 1; //+1 = _end
    if (diL < tBEGi)
      diL = tBEGi;

    if (diR > tENDi)
      diR = tENDi;

    for (long i = diL; i < diR; ++i) {
      seriesA->push_back(QPointF(Data[i].x(), Data[i].y()));
    }

  } // CbxWindow->ItemIndex == wkPeaks

  /* * Plateau * */

  /* * BSL Slope, Intecept * */
  if (tB != tA)
    BSLSlope = (HB - HA) / (tB - tA);
  else
    BSLSlope = 0.0;

  BSLIntercept = HA - BSLSlope * tA;

  /* * H_* */
  HP_ = HP - BSLSlope * tP - BSLIntercept;

  /* * w1/2 * */
  twPLefti = tPi;
  do {
    twPLefti--;
    twPLeft = Data[twPLefti].y() - BSLSlope * Data[twPLefti].x() - BSLIntercept;
  } while ((twPLefti > tAi) && (fabs(HP_) / 2.0 < fabs(twPLeft)));

  twPLeft = Data[twPLefti].x();

  twPRighti = tPi;
  do {
    twPRighti++;
    twPRight = Data[twPRighti].y() - BSLSlope * Data[twPRighti].x() - BSLIntercept;
  } while (twPRighti < tBi && fabs(HP_) / 2.0 < fabs(twPRight));

  twPRight = Data[twPRighti].x();


  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::BASELINE) {
    seriesA->push_back(QPointF(XMin, (BSLSlope * XMin) + BSLIntercept));
    seriesB->push_back(QPointF(XMin, (BSLSlope * XMin)+ BSLIntercept));
  }

   if (p.showWindow == EvaluationParametersItems::ComboShowWindow::PEAK_WIDTH_HALF) {
     seriesA->push_back(QPointF(twPRight, (BSLSlope * twPRight) + BSLIntercept));
     seriesA->push_back(QPointF(twPRight, HP));
     seriesB->push_back(QPointF(twPLeft, (BSLSlope * twPLeft) + BSLIntercept));
     seriesB->push_back(QPointF(twPLeft, HP));
   }

  /* Kresleni SystemWindows::Smoothed * */
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::SMOOTHED) {
    diR = NoiseWindow; if (diR > C) diR = C;
    diL = 0;
    long Position = (diR + diL) / 2;

    #define _Y MinValue
    SummY = 0.;
    for (long i = diL; i < diR; ++i)
      SummY += Data[i].y();

    while (diR < C) {
      seriesA->push_back(QPointF(Data[Position].x(), SummY / NoiseWindow));
      SummY -= Data[diL].y();
      SummY += Data[diR].y();
      ++diR;
      ++diL;
      ++Position;
    } //SystemWindows::Smoothed::While
    #undef _Y
  } //Kresleni SystemWindows::Smoothed

  /* Kresleni SystemWindows::Derivace * */
  if (p.showWindow == EvaluationParametersItems::ComboShowWindow::FIRST_DERIVATIVE ||
      p.showWindow == EvaluationParametersItems::ComboShowWindow::SECOND_DERIVATIVE ||
      p.showWindow == EvaluationParametersItems::ComboShowWindow::BOTH_DERIVATIVES) {
    const long CC = C - 1;
    diR = SlopeWindow; if (diR > CC) diR = CC;
    diL = 0;
    long Position = (diR + diL) / 2;

    double _Slope;
    #define SLOPE _Slope
    #define OLD SummValue

    #define _X MaxValue
    #define _Y MinValue
    SummX = 0.; SummXX = 0.; SummY = 0.; SummXY = 0.;
    for (long i = diR; i > diL; --i) {
      _X = Data[i].x();
      _Y = Data[i].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;
    }

    if ((SLOPE = ((diR-diL) * SummXX - SummX * SummX)))
      SLOPE = ((diR-diL) * SummXY - SummX * SummY) / SLOPE;
    else
      SLOPE = 0.0;

    OLD = SLOPE;

    while (diR < CC) {
      _X = Data[Position].x();
      if (p.showWindow == EvaluationParametersItems::ComboShowWindow::FIRST_DERIVATIVE ||
          p.showWindow == EvaluationParametersItems::ComboShowWindow::BOTH_DERIVATIVES) {
            seriesA->push_back(QPointF(_X, SLOPE));
      }

      if (p.showWindow == EvaluationParametersItems::ComboShowWindow::FIRST_DERIVATIVE ||
          p.showWindow == EvaluationParametersItems::ComboShowWindow::BOTH_DERIVATIVES) {
            seriesB->push_back(QPointF(_X, (SLOPE - OLD) / (_X - Data[Position-1].x())));
      }

      ++diR;
      ++diL;
      ++Position;

      _X = Data[diL].x();
      _Y = Data[diL].y();
      SummX -= _X;
      SummXX -= _X*_X;
      SummY -= _Y;
      SummXY -= _X*_Y;
      OLD = SLOPE;

      _X = Data[diR].x();
      _Y = Data[diR].y();
      SummX += _X;
      SummXX += _X*_X;
      SummY += _Y;
      SummXY += _X*_Y;

      if ((SLOPE = (SlopeWindow * SummXX - SummX * SummX)))
        SLOPE = (SlopeWindow * SummXY - SummX * SummY) / SLOPE;
      else
        SLOPE = 0.0;

    } //SystemWindows::Derivatives::While
    #undef SLOPE
    #undef OLD
    #undef _X
    #undef _Y
    #undef SEARCH_I
  } //Kresleni SystemWindows::Derivatives

  r.baselineIntercept = BSLIntercept;
  r.baselineSlope = BSLSlope;
  r.fromIndex = tBEGi;
  r.toIndex = tENDi;
  r.indexAtMax = tPi;
  r.fromPeakIndex = tAi;
  r.toPeakIndex = tBi;
  r.noise = Noise;
  r.noiseRefPoint = tnrp;
  r.maxY = YMax;
  r.minY = YMin;
  r.peakFromX = tA;
  r.peakFromY = HA;
  r.peakHeight = HP;
  r.peakHeightBaseline = HP_;
  r.peakToX = tB;
  r.peakToY = HB;
  r.peakX = tP;
  r.slopeRefPoint = tsrp;
  r.slopeThreshold = SlopeThreshold;
  r.slopeWindow = SlopeWindow;
  r.toIndex = tENDi;
  r.twPLeft = twPLeft;
  r.twPRight = twPRight;
  r.seriesA = seriesA;
  r.seriesB = seriesB;

  if (p.windowUnits == EvaluationParametersItems::ComboWindowUnits::MINUTES)
    r.slopeWindow /= ppm;

  r.validate();

  return r;
}
