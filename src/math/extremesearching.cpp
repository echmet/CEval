//=============================================================================
// INCLUDE
#include "extremesearching.h"
#include <cmath>

//=============================================================================
// SETTINGS

//=============================================================================
// CODE

double TExtremeSearcher::CalcNoise()
{
  double result = 0.;

  long diL = _I_ - QCount;      if (diL < 0)     diL = 0;
  long diR = diL + ChainPoints; if (diR > Count) diR = Count;

  double X, Y /*, slope */;

  double SummX = 0., SummXX = 0., SummY = 0., SummXY = 0.;
  for (long i = diL; i < diR; ++i) {
        X = Data->GetX(i); Y = Data->GetY(i);
        SummX += X; SummXX += X*X; SummY += Y; SummXY += X*Y;
  }

  if ((Y = ((diR-diL) * SummXX - SummX * SummX)))
    X = ((diR-diL) * SummXY - SummX * SummY) / Y;
  else
    X = 0.0;
  if (Y)
    Y = (SummY * SummXX - SummX * SummXY) / Y;
  else
    Y = 0.0;

  for (long i = diL; i < diR; ++i) {
    double d = fabs(Data->GetY(i) - X * Data->GetX(i) - Y);
    if (d > result)
      result = d;
  }

  return result;
}

//-----------------------------------------------------------------------------
int TExtremeSearcher::CheckForCentralExtreme()
{
    long diL = _I_ - QCount;      if (diL < 0)     diL = 0;
    long diR = diL + ChainPoints; if (diR > Count) diR = Count;

    debug << "\n ------------------ \n" << std::endl;
    for (long i = diL; i != diR; ++i) {
        debug << "_I_ " << _I_
              << " IMax " << IMax << " YMax " << YMax
              << " IMin " << IMin << " YMin " << YMin
              << std::endl;

        debug << i << " : " << Data->GetY(i);

        if (Data->GetY(i) > YMax) { IMax = i; YMax = Data->GetY(IMax); debug << " -> *** MAX ***";}
        if (Data->GetY(i) < YMin) { IMin = i; YMin = Data->GetY(IMin); debug << " -> *** MIN ***";}

        debug << std::endl;

    }

    if (IMax < _I_) { IMax = diR - 1; YMax = Data->GetY(IMax); }
    if (IMin < _I_) { IMin = diR - 1; YMin = Data->GetY(IMin); }

    debug << " == RETURN == " << std::endl;
    debug << "_I_ " << _I_
          << " IMax " << IMax << " YMax " << YMax
          << " IMin " << IMin << " YMin " << YMin
          << std::endl;

    return IMax == _I_ ? 1 : (IMin == _I_ ? -1 : 0);

}


//-----------------------------------------------------------------------------
void TExtremeSearcher::Search()
{
    /*debug.close();
    debug.clear();
    debug.open("N:\\Pavel\\PRG\\CPP" "\\debug.txt", std::ios_base::trunc | std::ios_base::out);*/
    QCount = ChainPoints / 2 + 1;

    Count = Data->Count();
    I_end = Count - QCount;

    // INIT

    IMax = 0; YMax = Data->GetY(0); YMin = Data->GetY(0);
    _I_ = LeftBoundary ? QCount : 0;
    const long iEnd = RightBoundary ? I_end : Count;
    while (_I_ < iEnd){

        int answer = CheckForCentralExtreme();
        if (answer) {
            if (Noise == 0. || CalcNoise() >= Noise)
                answer > 0 ? OnMaximum(YMax, IMax) : OnMinimum(YMin, IMin);

            ++_I_;                                                        // 1)
            // ----------------------------------------
            // 1) NOT _I_ += QCount -> RightBoundary

        } else _I_ = std::min(IMax, IMin);

    }

    /*debug.close();
    debug.clear();*/
}
