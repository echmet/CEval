#ifndef ECHMET_REGRESS_HYPERBOLA2_H
#define ECHMET_REGRESS_HYPERBOLA2_H

//===========================================================================
// INCLUDES

#include "regress.h"

#include <boost/type_traits/is_pod.hpp>

//===========================================================================
// CODE

namespace echmet {

namespace regressCore {

//===========================================================================
// dcl

//---------------------------------------------------------------------------

enum class RectangularHyperbola2Params { u0, uS, KS, du0, duS, dKS };

//---------------------------------------------------------------------------

// should not be public but then must be friend to the hyperbola2 template
// too much complications too care now...
template<typename XT>
struct RectangularHyperbola2XType {

    RectangularHyperbola2XType(msize_t index = 0, XT value = XT())
    : index(index), value(value)
    {}

    msize_t index;
    XT      value;
};

//---------------------------------------------------------------------------

template<typename XT>
dbstream & operator<<(dbstream & db, RectangularHyperbola2XType<XT> const & val)
{

    db << "(" << val.index << "; " << val.value << ")";

    return db;

}

} // namespace regressCore
} // namespace echmet

namespace std {

template< class XT >
struct is_arithmetic<echmet::regressCore::RectangularHyperbola2XType<XT> > :
    std::integral_constant<bool, true>
{};

}

namespace boost {

// required for echmet::matrix interface
template<typename XT> class is_pod
        < echmet::regressCore::RectangularHyperbola2XType<XT> >
:  public boost::true_type {};

} // namespace boost

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

template<typename XT>
inline std::ostream &
operator<<(
    std::ostream & os,
    echmet::regressCore::RectangularHyperbola2XType<XT> x
) {

        return os << "(" << x.index << ", " << x.value << ")";

}

#endif

//---------------------------------------------------------------------------
namespace echmet {

namespace regressCore {

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double>
class RectangularHyperbola2
: public RegressFunction<RectangularHyperbola2XType<XT>, YT> {
public:

    typedef RectangularHyperbola2XType<XT> x_type;

    typedef Mat<YT> MatrixY;

    typedef size_t msize_t;

    RectangularHyperbola2 ();

    virtual ~RectangularHyperbola2 () override;

    bool Initialize (
        vector<x_type> const & x,
        MatrixY const & fx,
        YT eps, unsigned nmax, bool dumping,
        YT u01Setting = YT(0), YT u02Setting = YT(0), YT visckoef = YT(0)
    );

private:

 YT m_u0Setting[2];
 YT m_viscoeff;

protected:

 virtual RectangularHyperbola2 * ACreate() const override;

 virtual bool AInitialize(
     MatrixY       & params,
     vector<x_type> const & x,
     MatrixY const & y
 ) override;

 virtual void AAssign(RegressFunction<x_type, YT> const & other) override;

 virtual YT ACalculateFx (
     x_type                 x,
     MatrixY const &        params,
     msize_t
 ) const override;

 virtual YT ACalculateDerivative (
     x_type           x,
     MatrixY const &  params,
     msize_t          param_idx,
     msize_t
 ) const override;

 virtual bool AAccepted (YT, MatrixY const & params) const override;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RectangularHyperbola2<XT, YT>::RectangularHyperbola2()
:
    RegressFunction<x_type, YT>(6),
    m_viscoeff(1)
{

    m_u0Setting[0] = YT(0);
    m_u0Setting[1] = YT(0);


}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
RectangularHyperbola2<XT, YT>
::~RectangularHyperbola2 ()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline bool RectangularHyperbola2<XT, YT>::Initialize(
    vector<x_type> const & x,
    MatrixY const & y,
    YT eps, unsigned nmax, bool damping,
    YT u01Setting, YT u02Setting, YT viscoeff

)
{

    m_u0Setting[0] = u01Setting;
    m_u0Setting[1] = u02Setting;
    m_viscoeff     = viscoeff;
    return RegressFunction<x_type, YT>::Initialize(x, y, eps, nmax, damping);

}

//---------------------------------------------------------------------------
// Protected

//---------------------------------------------------------------------------
template <typename XT, typename YT>
RectangularHyperbola2<XT, YT> *
RectangularHyperbola2<XT, YT>::ACreate() const
{

    return  new RectangularHyperbola2();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbola2<XT, YT>::AInitialize(
    MatrixY       & params,
    vector<x_type> const & x,
    MatrixY const & y
)
{

    // Init

    const size_t count = x.size();

    YT   u0[2] = {YT(0)};
    bool found[2] = {false};
    for (size_t i = 0; i != count; ++i)
        if ( x[i].value == XT(0.)) {
            found[x[i].index] = true;
            u0[ x[i].index ] = y(i,0);
        }

    for (int i = 0; i != 2; ++i) if (!found[i]) u0[i] = m_u0Setting[i];

    this->SetParam(params, RectangularHyperbola2Params::u0, u0[0]);
    this->SetParam(params, RectangularHyperbola2Params::uS, YT(0));
    this->SetParam(params, RectangularHyperbola2Params::KS, YT(0));
    this->SetParam(params, RectangularHyperbola2Params::du0, u0[1] - u0[0]);
    this->SetParam(params, RectangularHyperbola2Params::duS, YT(0));
    this->SetParam(params, RectangularHyperbola2Params::dKS, YT(0));

    long       ndata[2] = {0};
    x_type _X;
    YT _XVAL = YT(0);
    YT _Y    = YT(0);
    YT SummX[2] = {YT(0)}; YT SummXX[2] = {YT(0)};
    YT SummY[2] = {YT(0)}; YT SummXY[2] = {YT(0)};

    // Doit

    for (size_t i = 0; i != count; ++i) {

        // reading x and y
        _X = x[i]; _Y = y(i,0);

        if (_X.value == YT(0) || _Y == (_Y - u0[_X.index]) ) continue;

        // hyperbola linearization
        _XVAL = 1./_X.value;	_Y = 1./(_Y - u0[_X.index]);

        // linregresion sums
        SummX[_X.index]  += _XVAL;
        SummXX[_X.index] += _XVAL * _XVAL;
        SummY[_X.index]  += _Y;
        SummXY[_X.index] += _XVAL * _Y;

        // number of data excluding _Y == u0
        ++ndata[_X.index];
    }


    // Finalise

    YT SLOPE[2], INTERCEPT[2];
    for (int i = 0; i != 2; ++i) {

        if ( (_Y = (ndata[i] * SummXX[i] - SummX[i] * SummX[i])) )
            _XVAL = (ndata[i] * SummXY[i] - SummX[i] * SummY[i]) / _Y;
        else
            _XVAL = YT(0.0);
        if (_Y)
            _Y = (SummY[i] * SummXX[i] - SummX[i] * SummXY[i]) / _Y;
        else
            _Y = YT(0.0);

        SLOPE[i]     = _XVAL;
        INTERCEPT[i] = _Y;

    }

    // hyperbola "delinearization"

    if (INTERCEPT[0])
        this->SetParam(params, RectangularHyperbola2Params::uS, YT(1)/INTERCEPT[0] + u0[0]);

    if (SLOPE[0])
        this->SetParam(params, RectangularHyperbola2Params::KS, INTERCEPT[0] / SLOPE[0]);

    if (INTERCEPT[1])
        this->SetParam(params, RectangularHyperbola2Params::duS, YT(1)/INTERCEPT[1] + u0[1]);

    if (SLOPE[1])
        this->SetParam(params, RectangularHyperbola2Params::dKS, INTERCEPT[1] / SLOPE[1]);

    // conversion to deltas

    this->SetParam(
        params, RectangularHyperbola2Params::duS,
        this->GetParam(params, RectangularHyperbola2Params::duS) -
        this->GetParam(params, RectangularHyperbola2Params::uS)
    );

    this->SetParam(
        params, RectangularHyperbola2Params::dKS,
        this->GetParam(params, RectangularHyperbola2Params::dKS) -
        this->GetParam(params, RectangularHyperbola2Params::KS)
    );

    // Check and Return

    return AAccepted(YT(0), params);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
void RectangularHyperbola2<XT, YT>::AAssign(
    RegressFunction<x_type, YT> const & other
) {

    RectangularHyperbola2
            const & mother =
            dynamic_cast<RectangularHyperbola2 const &>(other);

    m_u0Setting[0] = mother.m_u0Setting[0];
    m_u0Setting[1] = mother.m_u0Setting[1];
    m_viscoeff     = mother.m_viscoeff;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbola2<XT, YT>
::ACalculateFx (
        x_type           x,
        MatrixY const & params,
        msize_t
) const {

    // init

    YT u0  = this->GetParam(params, RectangularHyperbola2Params::u0);
    YT uS  = this->GetParam(params, RectangularHyperbola2Params::uS);
    YT KS  = this->GetParam(params, RectangularHyperbola2Params::KS);
    YT du0 = this->GetParam(params, RectangularHyperbola2Params::du0);
    YT duS = this->GetParam(params, RectangularHyperbola2Params::duS);
    YT dKS = this->GetParam(params, RectangularHyperbola2Params::dKS);

    // viscosity correction

    uS  /= YT(1) + m_viscoeff * x.value;
    duS /= YT(1) + m_viscoeff * x.value;

    // doit

    if (x.index == 0)
        return ( (u0 + uS * KS * x.value) / (1 + KS * x.value) );
    else
        return (
            ( (u0 + du0) + (uS + duS) * (KS + dKS) * x.value ) /
            ( YT(1) + (KS + dKS) * x.value )
        );

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT RectangularHyperbola2<XT, YT>::ACalculateDerivative (
    x_type              x,
    MatrixY const &     params,
    msize_t param_idx,
    msize_t
) const {

    // init

    YT u0  = this->GetParam(params, RectangularHyperbola2Params::u0);
    YT uS  = this->GetParam(params, RectangularHyperbola2Params::uS);
    YT KS  = this->GetParam(params, RectangularHyperbola2Params::KS);
    YT du0 = this->GetParam(params, RectangularHyperbola2Params::du0);
    YT duS = this->GetParam(params, RectangularHyperbola2Params::duS);
    YT dKS = this->GetParam(params, RectangularHyperbola2Params::dKS);

    YT helper1 = YT(1) + (KS * x.value);
    YT helper2 = YT(1) + ((KS + dKS) * x.value);

    // viscosity correction

    uS  /= YT(1) + m_viscoeff * x.value;
    duS /= YT(1) + m_viscoeff * x.value;

    // doit

    if (x.index == 0) {

        switch(static_cast<RectangularHyperbola2Params>(param_idx)) {
        case RectangularHyperbola2Params::u0  : return YT(1) / helper1;
        case RectangularHyperbola2Params::uS  : return (KS * x.value) / (YT(1) + m_viscoeff * x.value) / helper1;
        case RectangularHyperbola2Params::KS  : return ((uS - u0) * x.value) / (helper1 * helper1);
        case RectangularHyperbola2Params::du0 : return YT(0);
        case RectangularHyperbola2Params::duS : return YT(0);
        case RectangularHyperbola2Params::dKS : return YT(0);
        }

    } else {

        switch(static_cast<RectangularHyperbola2Params>(param_idx)) {
        case RectangularHyperbola2Params::u0  :
        case RectangularHyperbola2Params::du0 : return YT(1) / helper2;
        case RectangularHyperbola2Params::uS  :
        case RectangularHyperbola2Params::duS : return ((KS + dKS) * x.value) / (YT(1) + m_viscoeff * x.value) / helper2;
        case RectangularHyperbola2Params::KS  :
        case RectangularHyperbola2Params::dKS : return (((uS + duS) - (u0 + du0)) * x.value) / (helper2 * helper2);
        }
    }

    return YT(); // warning off

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RectangularHyperbola2<XT, YT>
::AAccepted (YT, MatrixY const & params)
const {

#if 0

        YT u0  = this->GetParam(params, RectangularHyperbola2Params::u0);
        YT uS  = this->GetParam(params, RectangularHyperbola2Params::uS);
        YT KS  = this->GetParam(params, RectangularHyperbola2Params::KS);
        YT du0 = this->GetParam(params, RectangularHyperbola2Params::du0);
        YT duS = this->GetParam(params, RectangularHyperbola2Params::duS);
        YT dKS = this->GetParam(params, RectangularHyperbola2Params::dKS);

        fabs(u0)       < YT(1000.) &&
        fabs(uS)       < YT(1000.) &&
        KS             > YT(0.)    &&
        fabs(u0 + du0) < YT(1000.) &&
        fabs(uS + duS) < YT(1000.) &&
        KS + dKS       > YT(0.)
    ;
#else
        (void)params;
        return true;
#endif

}

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

template class RectangularHyperbola2<double, double>;

#endif

//---------------------------------------------------------------------------
}  //namespace regressCore

}  //namespace echmet

//---------------------------------------------------------------------------
#endif // ECHMET_REGRESS_HYPERBOLA_H2

