#ifndef ECHMET_HVLPEAK_H
#define ECHMET_HVLPEAK_H

//===========================================================================
// INCLUDES

#include "../../hvllibwrapper.h"
#include "../hvlestimate.h"
#include "regress.h"


//===========================================================================
// CODE

namespace echmet {

namespace regressCore {

//===========================================================================
// dcl

//---------------------------------------------------------------------------
enum class HVLPeakParams { a0, a1, a2, a3 };

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double> class HVLPeak
: public RegressFunction<XT, YT> {
public:

    typedef Mat<YT> MatrixY;

    typedef size_t msize_t;

    HVLPeak (const HVLLibWrapper *hvlLib = nullptr);
    ~HVLPeak();

    bool Initialize (
         vector<XT> const & x,
         MatrixY const & y,
         YT eps, unsigned nmax, bool dumping,
         HVLCore::Coefficients c, YT bsl, YT bslSlope
    );

protected:

    virtual HVLPeak * ACreate() const override;

    virtual bool AInitialize(
        MatrixY          & params,
        vector<XT> const & x,
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

    virtual void CalculateP () override;

private:

    const HVLLibWrapper *m_hvlLib;
    HVLCore::Coefficients m_c;
    YT m_bsl;
    YT m_bslSlope;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline HVLPeak<XT, YT>::HVLPeak(const HVLLibWrapper *hvlLib)
:
    RegressFunction<XT, YT>(4),
    m_hvlLib(hvlLib)
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
HVLPeak<XT, YT>::~HVLPeak ()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool HVLPeak<XT, YT>::Initialize (
    vector<XT> const & x,
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
    vector<XT> const &,
    MatrixY const &
)
{
    if (m_hvlLib == nullptr) return false;

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

    if (m_hvlLib == nullptr)
      return YT();

    YT a0 = this->GetParam(params, HVLPeakParams::a0);
    YT a1 = this->GetParam(params, HVLPeakParams::a1);
    YT a2 = this->GetParam(params, HVLPeakParams::a2);
    YT a3 = this->GetParam(params, HVLPeakParams::a3);

    double t = m_hvlLib->calculate(HVLLibWrapper::Parameter::T, x, a0, a1, a2, a3).y;

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
      case HVLPeakParams::a0 : return m_hvlLib->calculate(HVLLibWrapper::Parameter::DA0, x, a0, a1, a2, a3).y;
      case HVLPeakParams::a1 : return m_hvlLib->calculate(HVLLibWrapper::Parameter::DA1, x, a0, a1, a2, a3).y;
      case HVLPeakParams::a2 : return m_hvlLib->calculate(HVLLibWrapper::Parameter::DA2, x, a0, a1, a2, a3).y;
      case HVLPeakParams::a3 : return m_hvlLib->calculate(HVLLibWrapper::Parameter::DA3, x, a0, a1, a2, a3).y;
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
void HVLPeak<XT, YT>::CalculateP()
{
    /* I must have done something really terrible in my previous life... */

    typedef typename std::underlying_type<HVLPeakParams>::type PPUT;

    PPUT pa0 = static_cast<PPUT>(HVLPeakParams::a0);
    PPUT pa1 = static_cast<PPUT>(HVLPeakParams::a1);
    PPUT pa2 = static_cast<PPUT>(HVLPeakParams::a2);
    PPUT pa3 = static_cast<PPUT>(HVLPeakParams::a3);

    #pragma omp parallel for
    for (msize_t k = 0; k < this->m_x.size(); ++k) {
        HVLLibWrapper::AllParameters allParams = m_hvlLib->calculateAll(this->m_x[k],
                                                                        this->GetParam(this->m_params, HVLPeakParams::a0),
                                                                        this->GetParam(this->m_params, HVLPeakParams::a1),
                                                                        this->GetParam(this->m_params, HVLPeakParams::a2),
                                                                        this->GetParam(this->m_params, HVLPeakParams::a3));

        for (PPUT fix = 0; fix != static_cast<PPUT>(this->m_notFixed); ++fix) {
            const HVLPeakParams tfix = static_cast<HVLPeakParams>(fix);

            switch (tfix) {
            case HVLPeakParams::a0:
                (this->m_p)(pa0, k) = allParams.da0.y;
                break;
            case HVLPeakParams::a1:
                (this->m_p)(pa1, k) = allParams.da1.y;
                break;
            case HVLPeakParams::a2:
                (this->m_p)(pa2, k) = allParams.da2.y;
                break;
            case HVLPeakParams::a3:
                (this->m_p)(pa3, k) = allParams.da3.y;
                break;
            }
        }
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
#endif // ECHMET_REGRESS_HYPERBOLE_H

