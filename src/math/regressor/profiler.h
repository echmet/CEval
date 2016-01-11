#ifndef ECHMET_REGRESS_PROFILER_H
#define ECHMET_REGRESS_PROFILER_H

//===========================================================================
// INCLUCE

#include <utility>
#include <vector>
#include "regress.h"
#include "src/math/alglib/alglib.h"

//===========================================================================
// CODE

//---------------------------------------------------------------------------
namespace echmet {

namespace regressCore {

//---------------------------------------------------------------------------
// dcl

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double>
class Profiler {
public:

    typedef XT x_type;
    typedef YT y_type;
    typedef RegressFunction<XT, YT> regressor_type;
    typedef y_type ( * converter_type)(y_type tau, double p, regressor_type const & original, regressor_type const & disrupted);

    static y_type tau_to_tau        (y_type tau, double p, regressor_type const & , regressor_type const & );
    static y_type tau_to_abs        (y_type tau, double p, regressor_type const & , regressor_type const & );
    static y_type tau_to_confidenceL(y_type tau, double p, regressor_type const & original, regressor_type const & );
    static y_type tau_to_confidenceR(y_type tau, double p, regressor_type const & original, regressor_type const & );
    static y_type tau_to_confidenceT(y_type tau, double p, regressor_type const & original, regressor_type const & );
    static y_type tau_to_pvalueL    (y_type tau, double p, regressor_type const & original, regressor_type const & foo);
    static y_type tau_to_pvalueR    (y_type tau, double p, regressor_type const & original, regressor_type const & foo);
    static y_type tau_to_pvalueT    (y_type tau, double p, regressor_type const & original, regressor_type const & foo);

    y_type         toTau;
    bool           twoSided;
    unsigned       nmax;
    converter_type converter;

    Profiler();
    ~Profiler();
    Profiler(Profiler const &) = delete;
    void operator=(Profiler const &) = delete;

    bool Apply (
        std::vector<std::pair<y_type, y_type> > & out,
        regressor_type const                         & f,
        matrix::msize_t                           param_id
    );

    bool operator()(
        std::vector<std::pair<y_type, y_type> > & out,
        regressor_type const                         & f,
        matrix::msize_t                           param_id
    )
    { return Apply(out, f, param_id); }

private:

    regressor_type * m_reg;
    unsigned  m_counter;

    bool profile(
        std::vector<std::pair<y_type, y_type>> & out,
        regressor_type const             & f,
        echmet::matrix::msize_t       param_id,
        y_type                        sign
    );

};

//---------------------------------------------------------------------------
template<typename XT, typename YT>
Profiler<XT, YT>::Profiler()
:   m_reg(nullptr),
    m_counter(0),
    toTau(3),
    twoSided(true),
    nmax(50)
{

    converter = tau_to_tau;

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
Profiler<XT, YT>::~Profiler()
{

    delete m_reg;

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
bool Profiler<XT, YT>::Apply(
    std::vector<std::pair<y_type, y_type> > & out,
    regressor_type const                    & f,
    matrix::msize_t                           param_id
) {

    delete m_reg;
    m_reg = f.Clone(std::nothrow);
    if (!m_reg) throw std::bad_alloc();


    m_counter = 0;

    bool result = true;
    if (toTau <= y_type(0) || twoSided)
        result = profile(out, f, param_id, y_type(-1));
    if (toTau  > y_type(0) || twoSided)
        result = profile(out, f, param_id, y_type(1) ) && result ; // !1
    //-----------------------------------------------------------------------
    // !1) order matters

    return result;

}

//---------------------------------------------------------------------------
// private

//---------------------------------------------------------------------------
template<typename XT, typename YT>
bool Profiler<XT, YT>::profile(
    std::vector< std::pair<y_type, y_type> > & out,
    regressor_type const                          & f,
    echmet::matrix::msize_t                  param_id,
    y_type                                   sign
){

    dbstream debug;

    if (sign < 0) debug.open(ECHMET_REGRESS_DEBUG_PATH "\\debug_profileL.txt", std::ios_base::trunc | std::ios_base::out);
    else          debug.open(ECHMET_REGRESS_DEBUG_PATH "\\debug_profileR.txt", std::ios_base::trunc | std::ios_base::out);


    double coeff     = 1. / 64.;

    const y_type rss0    = f.GetRSS();
    const y_type s0      = f.GetS();

    y_type rssOld    = rss0;
    y_type rss;

    y_type param     = f.GetParameter(param_id);
    y_type paramOld  = param;

    y_type tau       = y_type(0);
    y_type tauOld    = tau;

    double pOld      = 0.5;
    double p;

    int    df        = f.GetDF();

    m_reg->Assign(f);
    m_reg->FixParameter(param_id, param);
    out.push_back(std::make_pair(param, converter( 0, 0.5, f, *m_reg )));

    for (
        y_type end = fabs(toTau);
        tau < end && m_counter != nmax;
        ++m_counter
     ) {

        debug << "\n---------------------------------------------\n" << std::endl;

        if (param == 0) param  = sign * coeff;
        else            param += sign * fabs(param) * coeff;
        m_reg->FixParameter(param_id, param);

        debug << "param : " << param << ", param - paramOld " << param - paramOld << std::endl;

        if (!m_reg->Regress()) {

            param = paramOld;
            coeff /= 2.;
            debug << "REGRESSION FAILED : Coeff : " << coeff << std::endl;
            continue;

        }

        rss = m_reg->GetRSS();

        debug <<  "REGRESSION OK : RSS - RSSOld : " << rss - rssOld << std::endl;

#if 1
        if (rss < rssOld ) {

            debug << "NOT ACCEPTED : Coeff : " << coeff << std::endl;

            continue;

        }

#endif

        tau = sqrt(rss - rss0) / s0;

        debug << "RSS : " << rss << ", RSS0 : " << rss0 << ", S0 : " << s0 << std::endl;
        debug << "tau : " << tau << ", tau - tauOld : " << tau - tauOld << std::endl;

#if 1

        p = AlgLib::studenttdistribution(df, sign * tau);
        double pdiff = sign * (p - pOld);

        debug << "p : " << p << ", pdiff : " << pdiff << std::endl;


        double pdiffmin, pdiffmax;

        pdiffmin = sign * (pOld - 0.5);  // temporary
        if (pdiffmin > 0.45) {           // 95% level

            pdiffmax = 0.002;
            pdiffmin = 0.0001;

        } else if (pdiffmin > 0.40) {    // 90% level

            pdiffmax = 0.01;
            pdiffmin = 0.001;

        } else if (pdiffmin > 0.25) {    // 75% level

            pdiffmax = 0.02;
            pdiffmin = 0.001;

        } else if (pdiffmin > 0.10) {    // 60% level

            pdiffmax = 0.05;
            pdiffmin = 0.01;

        } else {                        // < 60% level

            pdiffmax = 0.1;
            pdiffmin = 0.01;

        }

        if (pdiff > pdiffmax ) {

            param = paramOld;
            tau   = tauOld;
            rss   = rssOld;
            p     = pOld;

            coeff /= 2.;

            debug << "DIFFERENCE TOO LARGE: Coeff : " << coeff << std::endl;

            continue;
        }

        if (pdiff < pdiffmin) {

            coeff *= 1.25;

            debug << "Step coeff increased : Coeff : " << coeff << std::endl;

        }

#endif

        y_type val = converter( sign * tau, p, f, *m_reg );
        out.push_back(std::make_pair(param, val));

        debug << "ACCEPTED : value : " << val << ", coeff : " << coeff << std::endl;
        debug << std::endl;

        paramOld = param;
        rssOld   = rss;
        tauOld   = tau;
        pOld     = p;

    };

    return m_counter != nmax;

}

//---------------------------------------------------------------------------
// convertors

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_tau(
    y_type            tau,
    double                ,
    regressor_type const &,
    regressor_type const &
)
{

    return tau;

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_abs(
    y_type            tau,
    double                ,
    regressor_type const &,
    regressor_type const &
)
{

    return fabs(tau);

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_confidenceL(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const &
)
{

    // return y_type(1) - AlgLib::studenttdistribution(original.GetDF(), tau);
    return 1 - p;

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_confidenceR(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const &
)
{

    // return AlgLib::studenttdistribution(original.GetDF(), tau);

    return p;


}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_confidenceT(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const &
)
{

    return std::fabs(
        p - AlgLib::studenttdistribution(original.GetDF(), -1 * tau)
    );

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_pvalueL(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const & foo
)
{

    return y_type(1) - tau_to_confidenceL(tau, p, original, foo);

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_pvalueR(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const & foo
)
{

    return y_type(1) - tau_to_confidenceR(tau, p, original, foo);

}

//---------------------------------------------------------------------------
template<typename XT, typename YT>
inline typename Profiler<XT, YT>::y_type Profiler<XT, YT>::tau_to_pvalueT(
    y_type            tau,
    double              p,
    regressor_type const & original,
    regressor_type const & foo
)
{

    return y_type(1) - tau_to_confidenceT(tau, p, original, foo);

}


//---------------------------------------------------------------------------
} // namespace regressCore

} // namespace echmet

//---------------------------------------------------------------------------
#endif // ECHMET_REGRESS_PROFILER_H

