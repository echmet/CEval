#ifndef ECHMET_REGRESS_HYPERBOLE_H
#define ECHMET_REGRESS_HYPERBOLE_H

//===========================================================================
// INCLUDES

#include "regress.h"

//===========================================================================
// CODE

namespace echmet {

namespace regressCore {

//===========================================================================
// dcl

//---------------------------------------------------------------------------
enum class RectangularHyperboleParams { u0, uS, KS };

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double> class RectangularHyperbole
: public RegressFunction<XT, YT> {
public:

 RectangularHyperbole ();

 ~RectangularHyperbole ();

 bool Initialize (
     Core<XT> const & x,
     Core<YT> const & y,
     YT eps, unsigned nmax, bool dumping,
     YT u0Setting = YT(0),
     YT viscoeff =  YT(0)
 );

protected:

 virtual RectangularHyperbole * ACreate() const override;

 virtual bool AInitialize(
     Core<YT>       & params,
     Core<XT> const & x,
     Core<YT> const & y
 ) override;

 virtual void AAssign(RegressFunction<XT, YT> const & other) override;

 virtual YT ACalculateFx (
     XT                      x,
     Core<YT> const &        params,
     echmet::matrix::msize_t
 ) const override;

 virtual YT ACalculateDerivative (
     XT                      x,
     Core<YT> const &        params,
     echmet::matrix::msize_t param_idx,
     echmet::matrix::msize_t
 ) const override;

 virtual bool AAccepted (YT, Core<YT> const & params) const override;

private:

 YT m_u0Setting;
 YT m_viscoeff;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RectangularHyperbole<XT, YT>::RectangularHyperbole()
:
    RegressFunction<XT, YT>(3),
    m_u0Setting(0.)
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
RectangularHyperbole<XT, YT>::~RectangularHyperbole ()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline bool RectangularHyperbole<XT, YT>::Initialize(
    Core<XT> const & x,
    Core<YT> const & y,
    YT eps, unsigned nmax, bool damping,
    YT u0Setting, YT viscoeff

)
{

    m_u0Setting = u0Setting;
    m_viscoeff  = viscoeff;

    return RegressFunction<XT, YT>::Initialize(x, y, eps, nmax, damping);

}

//---------------------------------------------------------------------------
// Protected

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RectangularHyperbole<XT, YT> * RectangularHyperbole<XT, YT>::ACreate() const
{

    return new RectangularHyperbole();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbole<XT, YT>::AInitialize(
    Core<YT>       & params,
    Core<XT> const & x,
    Core<YT> const & y
)
{

    // Init

    const long count = x.size();

    YT   u0 = 0.;
    bool found = false;
    for (long i = 0; !found && i != count; ++i)
        if ( x[i][0] == YT(0)) {
            found = true;
            u0 = y[i][0];
        }

    if (!found) u0 = m_u0Setting;

    this->SetParam(params, RectangularHyperboleParams::u0, u0);
    this->SetParam(params, RectangularHyperboleParams::uS, YT(0));
    this->SetParam(params, RectangularHyperboleParams::KS, YT(0));

    long       ndata = 0;
    XT _X = XT(0);
    YT _Y = YT(0);
    YT SummX = YT(0); YT SummXX = YT(0); YT SummY = YT(0); YT SummXY = YT(0);

    // Doit

    for (long i = 0; i != count; ++i) {

        // reading x and y
        _X = x[i][0]; _Y = y[i][0];

        if (_X == YT(0) || _Y == YT(0)) continue;

        // hyperbole linearization
        _X = 1./_X;	_Y = 1./(_Y - u0);

        // linregresion sums
        SummX += _X; SummXX += _X*_X; SummY += _Y; SummXY += _X*_Y;

        // number of data excluding _Y == u0
        ++ndata;

    }

    // Finalize

    if ( (_Y = (ndata * SummXX - SummX * SummX)) )
        _X = (ndata * SummXY - SummX * SummY) / _Y;
    else
        _X = 0.0;
    if (_Y)
        _Y = (SummY * SummXX - SummX * SummXY) / _Y;
    else
        _Y = 0.0;

    double SLOPE     = _X;
    double INTERCEPT = _Y;

    // hyperbole "delinearization"

    if (INTERCEPT)
        this->SetParam(params, RectangularHyperboleParams::uS, YT(1) /INTERCEPT + u0);

    if (SLOPE )
        this->SetParam(params, RectangularHyperboleParams::KS, INTERCEPT / SLOPE);

    // Check and Return

    return AAccepted(YT(0), params);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
void RectangularHyperbole<XT, YT>::AAssign(RegressFunction<XT, YT> const & other)
{

    RectangularHyperbole const & mother =
            dynamic_cast<RectangularHyperbole const &>(other);

    m_u0Setting = mother.m_u0Setting;
    m_viscoeff  = mother.m_viscoeff;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbole<XT, YT>::ACalculateFx (
        XT x,
        Core<YT> const & params,
        msize_t
) const {

    YT u0 = this->GetParam(params, RectangularHyperboleParams::u0);
    YT uS = this->GetParam(params, RectangularHyperboleParams::uS);
    YT KS = this->GetParam(params, RectangularHyperboleParams::KS);

    uS  /= YT(1) + m_viscoeff * x;

    return ( (u0 + uS * KS * x) / (YT(1) + KS * x) );

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbole<XT, YT>::ACalculateDerivative (
    XT                      x,
    Core<YT> const &        params,
    echmet::matrix::msize_t param_idx,
    echmet::matrix::msize_t
) const {

    YT u0 = this->GetParam(params, RectangularHyperboleParams::u0);
    YT uS = this->GetParam(params, RectangularHyperboleParams::uS);
    YT KS = this->GetParam(params, RectangularHyperboleParams::KS);

    uS  /= YT(1) + m_viscoeff * x;

    double helper = YT(1) + (KS * x);

    switch(static_cast<RectangularHyperboleParams>(param_idx))
    {
    case RectangularHyperboleParams::u0 : return YT(1) / helper;
    case RectangularHyperboleParams::uS : return KS * x / (YT(1) + m_viscoeff * x) / helper;
    case RectangularHyperboleParams::KS : return (uS - u0) * x / (helper * helper);
    }

    return YT(); // warning off

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbole<XT, YT>::AAccepted (YT, Core<YT> const & params)
const {

#if 0

    return
        fabs(this->GetParam(params, RectangularHyperboleParams::u0)) < 1000 &&
        fabs(this->GetParam(params, RectangularHyperboleParams::uS)) < 1000 &&
        this->GetParam(params, RectangularHyperboleParams::KS)      > 0
    ;
#else

    (void)params;
    return true;

#endif

}

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

template class RectangularHyperbole<double, double>;

#endif

//---------------------------------------------------------------------------
}  //namespace regressCore

}  //namespace echmet

//---------------------------------------------------------------------------
#endif // ECHMET_REGRESS_HYPERBOLE_H

