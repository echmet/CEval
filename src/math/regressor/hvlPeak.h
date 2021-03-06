#ifndef ECHMET_HVLPEAK_H
#define ECHMET_HVLPEAK_H

//===========================================================================
// INCLUDES

//#include "../../hvllibwrapper.h"
#include "../hvlestimate.h"
#include "regress.h"
#include <cassert>

#include "../vhhvl.hpp"

//===========================================================================
// CODE

namespace echmet {

namespace regressCore {

//===========================================================================
// dcl

//---------------------------------------------------------------------------
enum class HVLPeakParams : msize_t { a0, a1, a2, a3 };

using HVLCalc = VHHVLCalculator<double>;

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double> class HVLPeak
: public RegressFunction<XT, YT> {
public:

    typedef typename RegressFunction<XT, YT>::MatrixY MatrixY;

    HVLPeak ();
    ~HVLPeak();

    bool Initialize (
         Vector<XT> const & x,
         MatrixY const & y,
         YT eps, unsigned nmax, bool dumping,
         HVLCore::Coefficients c, YT bsl, YT bslSlope
    );

protected:

    virtual HVLPeak * ACreate() const override;

    virtual bool AInitialize(
        MatrixY          & params,
        Vector<XT> const & x,
        MatrixY    const & y
    ) override;

    virtual void AAssign(RegressFunction<XT, YT> const & other) override;

    virtual YT ACalculateFx (
         XT                      x,
         MatrixY const &         params,
         msize_t
    ) const override;

    virtual YT ACalculateDerivative (
         XT               x,
         MatrixY const &  params,
         msize_t          param_idx,
         msize_t
    ) const override;

    virtual bool AAccepted (YT, MatrixY const & params) const override;

    virtual void ACalculateP () override;
    virtual void AValidateParameters(MatrixY &params) override;
    virtual void OnParamsChangedInternal () override;

private:
    HVLCore::Coefficients m_c;
    YT m_bsl;
    YT m_bslSlope;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline HVLPeak<XT, YT>::HVLPeak()
:
    RegressFunction<XT, YT>(4)
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
HVLPeak<XT, YT>::~HVLPeak ()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool HVLPeak<XT, YT>::Initialize (
    Vector<XT> const & x,
    MatrixY const & y,
    YT eps, unsigned nmax, bool damping,
    HVLCore::Coefficients c, YT bsl, YT bslSlope
)
{
    m_c        = c;
    m_bsl      = bsl;
    m_bslSlope = bslSlope;

    return RegressFunction<XT, YT>::Initialize(x, y, eps, nmax, damping);

}

//---------------------------------------------------------------------------
// Protected

//---------------------------------------------------------------------------
template <typename XT, typename YT>
HVLPeak<XT, YT> * HVLPeak<XT, YT>::ACreate() const
{

    return new HVLPeak();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool HVLPeak<XT, YT>::AInitialize(
    MatrixY       & params,
    Vector<XT> const &,
    MatrixY const &
)
{

    this->SetParam(params, HVLPeakParams::a0, m_c.a0);
    this->SetParam(params, HVLPeakParams::a1, m_c.a1);
    this->SetParam(params, HVLPeakParams::a2, m_c.a2);
    this->SetParam(params, HVLPeakParams::a3, m_c.a3d);

    return true;

}


//---------------------------------------------------------------------------
template <typename XT, typename YT>
void HVLPeak<XT, YT>::AAssign(RegressFunction<XT, YT> const & other)
{

    HVLPeak const & mother = dynamic_cast<HVLPeak const &>(other);

    m_c          = mother.m_c;
    m_bsl        = mother.m_bsl;
    m_bslSlope   = mother.m_bslSlope;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT HVLPeak<XT, YT>::ACalculateFx (
        XT x,
        MatrixY const & params,
        msize_t
) const {

    YT a0 = this->GetParam(params, HVLPeakParams::a0);
    YT a1 = this->GetParam(params, HVLPeakParams::a1);
    YT a2 = this->GetParam(params, HVLPeakParams::a2);
    YT a3 = this->GetParam(params, HVLPeakParams::a3);

    double t = HVLCalc::y(x, a0, a1, a2, a3);

    return t + m_bslSlope * x + m_bsl;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
YT HVLPeak<XT, YT>::ACalculateDerivative (
    XT               x,
    MatrixY const &  params,
    msize_t          param_idx,
    msize_t
) const {

    YT a0 = this->GetParam(params, HVLPeakParams::a0);
    YT a1 = this->GetParam(params, HVLPeakParams::a1);
    YT a2 = this->GetParam(params, HVLPeakParams::a2);
    YT a3 = this->GetParam(params, HVLPeakParams::a3);

    switch(static_cast<HVLPeakParams>(param_idx))
    {
      case HVLPeakParams::a0 : return HVLCalc::da0(x, a0, a1, a2, a3);
      case HVLPeakParams::a1 : return HVLCalc::da1(x, a0, a1, a2, a3);
      case HVLPeakParams::a2 : return HVLCalc::da2(x, a0, a1, a2, a3);
      case HVLPeakParams::a3 : return HVLCalc::da3d(x, a0, a1, a2, a3);
      default: return YT();
    }
}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool HVLPeak<XT, YT>::AAccepted (YT, MatrixY const & params)
const {


    return this->GetParam(params, HVLPeakParams::a2) > YT(0);

}

template <typename XT, typename YT>
void HVLPeak<XT, YT>::AValidateParameters(MatrixY &params)
{
   (void)params;
  /* NOOP */
}


template <typename XT, typename YT>
void HVLPeak<XT, YT>::OnParamsChangedInternal()
{
  ACalculateP(); /* Our CalculateP calculates Fx as well */
}

template <typename XT, typename YT>
void HVLPeak<XT, YT>::ACalculateP()
{
    /* I must have done something really terrible in my previous life... */
    /* Me from 2021: You don't know the half of it :) */

    using PPUT = std::underlying_type<HVLPeakParams>::type;

    #pragma omp parallel for schedule(static)
    for (msize_t k = 0; k < this->m_x.size(); ++k) {
        const auto pack = HVLCalc::everything(
                         this->m_x[k],
                         this->GetParam(this->m_params, HVLPeakParams::a0),
                         this->GetParam(this->m_params, HVLPeakParams::a1),
                         this->GetParam(this->m_params, HVLPeakParams::a2),
                         this->GetParam(this->m_params, HVLPeakParams::a3));

        const double t = pack.y();
        this->m_fx(k, 0) = t + (this->m_x[k] * m_bslSlope) + m_bsl;

        for (PPUT pIdx = 0; pIdx != static_cast<PPUT>(this->m_notFixed); ++pIdx)
            this->m_p(pIdx, k) = pack.results[pIdx + 2];
    }

}

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

template class HVLPeak<double, double>;

#endif

//---------------------------------------------------------------------------
}  //namespace regressCore

}  //namespace echmet

//---------------------------------------------------------------------------
#endif // ECHMET_REGRESS_HYPERBOLA_H

