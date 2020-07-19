//---------------------------------------------------------------------------
#ifndef ExtremeSearchingH
#define ExtremeSearchingH

//---------------------------------------------------------------------------
class TExtremeSearcher
{
private:

  int QCount, _I_, IMax, IMin, Count, I_end;
  double YMax, YMin;

  double CalcNoise(void);

  int CheckForCentralExtreme();
  int GuessChainPoints(const int ChainPoints);

protected:
  virtual void OnMaximum(double Value, int Index) = 0;
  virtual void OnMinimum(double Value, int Index) = 0;

public:
  class TDataHandler
  {
  public:
    virtual double GetX(int index) = 0;
    virtual double GetY(int index) = 0;
    virtual int Count(void) = 0;
  };

  TDataHandler *Data;
  int ChainPoints;
  double Noise;
  bool LeftBoundary;
  bool RightBoundary;

  TExtremeSearcher(
    TDataHandler *Data = nullptr,
    int ChainPoints   = 3,
    double Noise       = 0,
    bool LeftBoundary  = false,
    bool RightBoundary = false
  ) :
    Data(Data),
    ChainPoints(GuessChainPoints(ChainPoints)),
    Noise(Noise),
    LeftBoundary(LeftBoundary),
    RightBoundary(RightBoundary)
  {}

  TExtremeSearcher(TExtremeSearcher &T) :
    QCount(T.QCount),
    _I_(T._I_),
    IMax(T.IMax),
    IMin(T.IMin),
    Count(T.Count),
    I_end(T.I_end),
    YMax(T.YMax),
    YMin(T.YMin),
    Data(T.Data),
    ChainPoints(T.ChainPoints),
    Noise(T.Noise),
    LeftBoundary(T.LeftBoundary),
    RightBoundary(T.RightBoundary)
  {}

  virtual ~TExtremeSearcher(){}
  virtual void Search();
};
#endif
