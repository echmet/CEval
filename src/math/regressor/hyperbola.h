#ifndef ECHMET_REGRESS_HYPERBOLA_H
#define ECHMET_REGRESS_HYPERBOLA_H

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
enum class RectangularHyperbolaParams { u0, uS, KS };

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double> class RectangularHyperbola
: public RegressFunction<XT, YT> {
public:

    typedef Mat<YT> MatrixY;

 RectangularHyperbola ();

 ~RectangularHyperbola ();

 bool Initialize (
     vector<XT> const & x,
     MatrixY const & y,
     YT eps, unsigned nmax, bool dumping,
     YT u0Setting = YT(0),
     YT viscoeff =  YT(0)
 );

protected:

 virtual RectangularHyperbola * ACreate() const override;

 virtual bool AInitialize(
     MatrixY       & params,
     vector<XT> const & x,
     MatrixY const & y
 ) override;

 virtual void AAssign(RegressFunction<XT, YT> const & other) override;

 virtual YT ACalculateFx (
     XT                     x,
     MatrixY const &        params,
     msize_t
 ) const override;

 virtual YT ACalculateDerivative (
     XT                      x,
     MatrixY const &         params,
     msize_t param_idx,
     msize_t
 ) const override;

 virtual bool AAccepted (YT, MatrixY const & params) const override;

 virtual void AValidateParameters(MatrixY &params) override;

private:

 YT m_u0Setting;
 YT m_viscoeff;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RectangularHyperbola<XT, YT>::RectangularHyperbola()
:
    RegressFunction<XT, YT>(3),
    m_u0Setting(0.)
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
RectangularHyperbola<XT, YT>::~RectangularHyperbola ()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline bool RectangularHyperbola<XT, YT>::Initialize(
    vector<XT> const & x,
    MatrixY const & y,
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
inline RectangularHyperbola<XT, YT> * RectangularHyperbola<XT, YT>::ACreate() const
{

    return new RectangularHyperbola();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbola<XT, YT>::AInitialize(
    MatrixY       & params,
    vector<XT> const & x,
    MatrixY const & y
)
{

    // Init

    const size_t count = x.size();

    YT   u0 = 0.;
    bool found = false;
    for (size_t i = 0; !found && i != count; ++i)
        if ( x[i] == 0.) {
            found = true;
            u0 = y(i, 0);
        }

    if (!found) u0 = m_u0Setting;

    this->SetParam(params, RectangularHyperbolaParams::u0, u0);
    this->SetParam(params, RectangularHyperbolaParams::uS, YT(0));
    this->SetParam(params, RectangularHyperbolaParams::KS, YT(0));

    long       ndata = 0;
    XT _X = XT(0);
    YT _Y = YT(0);
    YT SummX = YT(0); YT SummXX = YT(0); YT SummY = YT(0); YT SummXY = YT(0);

    // Doit

    for (size_t i = 0; i != count; ++i) {

        // reading x and y
        _X = x[i]; _Y = y(i, 0);

        if (_X == YT(0) || _Y == u0) continue;

        // hyperbola linearization
        _X = 1./_X;	_Y = 1./(_Y - u0);

        // linregresion sums
        SummX += _X; SummXX += _X*_X; SummY += _Y; SummXY += _X*_Y;

        // number of data excluding _Y == u0
        ++ndata;

    }

    // Finalize

    YT SLOPE;
    YT INTERCEPT;

    if ( (_Y = (ndata * SummXX - SummX * SummX)) )
        SLOPE = (ndata * SummXY - SummX * SummY) / _Y;
    else
        SLOPE = 0.0;
    if (_Y)
        INTERCEPT = (SummY * SummXX - SummX * SummXY) / _Y;
    else
        INTERCEPT = 0.0;

    // hyperbola "delinearization"

    if (INTERCEPT)
        this->SetParam(params, RectangularHyperbolaParams::uS, YT(1) /INTERCEPT + u0);

    if (SLOPE )
        this->SetParam(params, RectangularHyperbolaParams::KS, INTERCEPT / SLOPE);

    // Check and Return

    return AAccepted(YT(0), params);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
void RectangularHyperbola<XT, YT>::AAssign(RegressFunction<XT, YT> const & other)
{

    RectangularHyperbola const & mother =
            dynamic_cast<RectangularHyperbola const &>(other);

    m_u0Setting = mother.m_u0Setting;
    m_viscoeff  = mother.m_viscoeff;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbola<XT, YT>::ACalculateFx (
        XT x,
        MatrixY const & params,
        msize_t
) const {

    YT u0 = this->GetParam(params, RectangularHyperbolaParams::u0);
    YT uS = this->GetParam(params, RectangularHyperbolaParams::uS);
    YT KS = this->GetParam(params, RectangularHyperbolaParams::KS);

    uS  /= YT(1) + m_viscoeff * x;

    return ( (u0 + uS * KS * x) / (YT(1) + KS * x) );

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbola<XT, YT>::ACalculateDerivative (
    XT                      x,
    MatrixY const &         params,
    msize_t param_idx,
    msize_t
) const {

    YT u0 = this->GetParam(params, RectangularHyperbolaParams::u0);
    YT uS = this->GetParam(params, RectangularHyperbolaParams::uS);
    YT KS = this->GetParam(params, RectangularHyperbolaParams::KS);

    uS  /= YT(1) + m_viscoeff * x;

    const YT helper = YT(1) + (KS * x);

    switch(static_cast<RectangularHyperbolaParams>(param_idx))
    {
    case RectangularHyperbolaParams::u0 : return YT(1) / helper;
    case RectangularHyperbolaParams::uS : return KS * x / (YT(1) + m_viscoeff * x) / helper;
    case RectangularHyperbolaParams::KS : return (uS - u0) * x / (helper * helper);
    }

    return YT(); // warning off

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbola<XT, YT>::AAccepted (YT, MatrixY const & params)
const {

#if 0

    return
        fabs(this->GetParam(params, RectangularHyperbolaParams::u0)) < 1000 &&
        fabs(this->GetParam(params, RectangularHyperbolaParams::uS)) < 1000 &&
        this->GetParam(params, RectangularHyperbolaParams::KS)      > 0
    ;
#else

    (void)params;
    return true;

#endif

}

template <typename XT, typename YT>
void RectangularHyperbola<XT, YT>::AValidateParameters(MatrixY & params)
{

    if (this->GetParam(params, RectangularHyperbolaParams::KS) <= YT(0.0))
        this->SetParam(params, RectangularHyperbolaParams::KS, YT(1.0e-6));

}

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

template class RectangularHyperbola<double, double>;

#endif

//---------------------------------------------------------------------------
}  //namespace regressCore

}  //namespace echmet

//---------------------------------------------------------------------------
#endif // ECHMET_REGRESS_HYPERBOLA_H

