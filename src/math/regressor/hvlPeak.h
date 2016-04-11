#ifndef ECHMET_HVLPEAK_H
#define ECHMET_HVLPEAK_H

//===========================================================================
// INCLUDES

#include "../hvl.hpp"
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

    math::HVL_dll const * dll;

    HVLPeak (math::HVL_dll const * dll = nullptr);
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

private:

    HVLCore::Coefficients m_c;
    YT m_bsl;
    YT m_bslSlope;

};

//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline HVLPeak<XT, YT>::HVLPeak(math::HVL_dll const * dll)
:
    RegressFunction<XT, YT>(4),
    dll(dll)
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
    YT eps, unsigned nmax, bool dumping,
    HVLCore::Coefficients c, YT bsl, YT bslSlope
)
{
    if (!dll || !dll->IsLoaded()) return false;

    m_c        = c;
    m_bsl      = bsl;
    m_bslSlope = bslSlope;

    return RegressFunction<XT, YT>::Initialize(x, y, eps, nmax, dumping);

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

    if (!dll || !dll->IsLoaded()) return false;

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

    return dll->HVL(x, a0, a1, a2, a3) + m_bslSlope * x + m_bsl;

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
    case HVLPeakParams::a0 : return dll->HVLda0(x, a0, a1, a2, a3);
    case HVLPeakParams::a1 : return dll->HVLda1(x, a0, a1, a2, a3);
    case HVLPeakParams::a2 : return dll->HVLda2(x, a0, a1, a2, a3);
    case HVLPeakParams::a3 : return dll->HVLda3(x, a0, a1, a2, a3);
    }

    return YT(); // warning off

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool HVLPeak<XT, YT>::AAccepted (YT, MatrixY const & params)
const {


    return this->GetParam(params, HVLPeakParams::a2) > YT(0);

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

