#ifndef ECHMET_REGRESS_H
#define ECHMET_REGRESS_H

//===========================================================================
// SETTINGS

#define ECHMET_REGRESS_WEIGHTS         0 // Not Yet Implemented!

#define ECHMET_REGRESS_DEBUG           1

#define ECHMET_REGRESS_DEBUG_PATH      "Q:\\Users\\Developer"

#define ECHMET_REGRESS_DEBUG_X         1
#define ECHMET_REGRESS_DEBUG_Y         1
#define ECHMET_REGRESS_DEBUG_PARAMS    1
#define ECHMET_REGRESS_DEBUG_FX        1
#define ECHMET_REGRESS_DEBUG_ERROR     1
#define ECHMET_REGRESS_DEBUG_P         1
#define ECHMET_REGRESS_DEBUG_ALPHA     1
#define ECHMET_REGRESS_DEBUG_BETA      1
#define ECHMET_REGRESS_DEBUG_DELTA     1


//===========================================================================
// INCLUDES

#include "../mystd/foostream.h"
#include "../hvl.hpp"

#include <new>
#include <fstream> // debug
#include <vector>
#include <armadillo>

//===========================================================================
// CODE

//---------------------------------------------------------------------------

namespace echmet {

namespace regressCore {

//---------------------------------------------------------------------------
using namespace arma;
using namespace std;


class matrix_error : public logic_error {

    public:

    matrix_error(std::string s) : logic_error(s) {}

};

//---------------------------------------------------------------------------
#if ECHMET_REGRESS_DEBUG == 1

typedef std::fstream dbstream;

#define INSPECT(MATRIX) {                                                      \
    debug << "\n------------------\n"                                          \
          << #MATRIX                                                           \
          << "[" << MATRIX.n_rows << "] [" << MATRIX.n_cols << "]"   \
          << std::endl;                                                        \
    for (msize_t i = 0; i != MATRIX.n_rows; ++i) {                      \
        for (msize_t j = 0; j != MATRIX.n_cols; ++j)                     \
        debug << MATRIX(i,j) << "\t";                                         \
        debug << std::endl;                                                    \
    }                                                                          \
    debug << std::endl;                                                        \
    debug << std::endl;                                                        \
}                                                                              \

#else

typedef foostream dbstream;

#   define INSPECT(MATRIX)

#endif

//===========================================================================
// dcl

typedef size_t msize_t;

//---------------------------------------------------------------------------
template<typename XT = double, typename YT = double> class RegressFunction {

public:

    // Types

    typedef XT x_type;
    typedef YT y_type;

    typedef Mat<YT> MatrixY;

    typedef void (*report_function)(RegressFunction const &);

    // Initializers

    explicit RegressFunction (msize_t params);

    RegressFunction(RegressFunction const &) = delete;
    void operator=(RegressFunction const &)  = delete;

    virtual ~RegressFunction();

    // Controllers

    bool Initialize  (
        vector<XT> const & x,
        MatrixY const & y,
        YT eps, unsigned nmax, bool damp
    );

    void Assign(RegressFunction const &other);

    RegressFunction * Clone() const;
    RegressFunction * Clone(std::nothrow_t) const;

    bool Regress ();
    void Abort   ();
    void Report  () const;

    void RegisterReportFunction(report_function);

    template<typename ENUM> void FixParameter(ENUM id);
    template<typename ENUM> void FixParameter(ENUM id, YT val);
    template<typename ENUM> void ReleaseParameter(ENUM id);
    template<typename ENUM> bool IsFixedParameter(ENUM id);

    // Getters

    report_function GetReportFunction() const { return m_report_function; }

    MatrixY const & GetParameters()       const { return m_params; }
    vector<XT> const & GetXs()            const { return m_x; }
    MatrixY const & GetYs()               const { return m_y; }
    MatrixY const & GetFx()               const { return m_fx; }
    MatrixY const & GetErrors()           const { return m_error; }

    YT      GetRSS()         const { return m_rss; }
    YT      GetImprovement() const { return m_improvement; }

    unsigned GetIterationCounter() const { return m_iterationCounter; }
    msize_t  GetNMax()             const { return m_nmax; }
    YT       GetEps()              const { return m_epsilon; }

    bool    IsDampped () const { return m_damping; }
    bool    IsAborted () const { return m_aborted; }
    bool    IsAccepted() const { return m_accepted; }

    bool    HasConverged() const;

    msize_t GetNCount       () const;
    msize_t GetPCount       () const; // 1
    msize_t GetPTotal       () const; // 2
    msize_t GetDF           () const;
    YT      GetMSE          () const;
    YT      GetS2           () const;
    YT      GetS            () const;
    //------------------------------------------------------------------------
    // 1) Number of not fixed(!) parameters (those to be estimated)
    // 2) Total number of parameters (both fixed and not fixed)

    template<typename T> YT GetParameter (T i) const; // 1
    //-----------------------------------------------------------
    // 1 enables conversion from strongly typed enums to msize_t
    // !! Do not use this method from the virtual functions
    //    Use protected GetParam and SetParam insted
    //    to access the parameters given as function arguments

    YT Evaluate(XT x)   const;
    YT operator()(XT x) const;

protected:

    //---

    virtual RegressFunction * ACreate() const = 0;

    virtual bool AInitialize(
     MatrixY       & params,
     vector<XT> const & x,
     MatrixY const & y
    ) = 0;

    virtual void AAssign(RegressFunction const &other) = 0;

    virtual YT ACalculateFx (
     XT               x,
     MatrixY const &  params,
     msize_t          idx
    ) const = 0;

    virtual YT ACalculateDerivative (
     XT               x,
     MatrixY const &  params,
     msize_t          param_idx,
     msize_t          idx
    ) const = 0;

    virtual bool AAccepted (YT, MatrixY const &) const { return true; }

    template<typename ENUM>
    void SetParam(MatrixY & params, ENUM id, YT);

    template<typename ENUM>
    YT const & GetParam(MatrixY const & params, ENUM id) const;

private:

    report_function   m_report_function;

    // Initialized in constructor
    MatrixY          m_params;          //           [params, 1]
    vector<bool>     m_fixedParams;     //           [params]
    vector<msize_t>  m_pindexes;        //           [params]

    // Initialized in Initialize
    vector<XT>       m_x;   //data x                 [x, 1]
    MatrixY          m_y;   //data y                 [x, 1]
    MatrixY          m_fx;  //fx calculated          [x, 1]

    // Initialized in CalculateRSS
    MatrixY m_error;      //                         [x, 1]

    // Initialized in OnParamsChanged
    MatrixY m_p;          //derivation matrix        [not_fixed, x]

    // Initialized in Regress
    MatrixY m_alpha;      //                         [not_fixed, not_fixed]
    MatrixY m_beta;       //                         [not_fixed, 1]
    MatrixY m_delta;      //                         [not_fixed, 1]

    // Resetable state variables

    unsigned m_iterationCounter;

    bool m_accepted;
    bool m_aborted;

    YT   m_improvement;
    YT   m_lambda;
    YT   m_lambdaCoeff;

    // Non-resetable state variables

    YT   m_rss;
    int  m_notFixed;

    // Setup variables
    unsigned m_nmax;
    YT       m_epsilon;
    bool     m_damping;

    // Methods

    void Reset();

    void OnParamsChanged(bool regressCall = false);

    void CalculateFx();
    void CalculateP ();
    void CalculateRSS();

};


//===========================================================================
// def

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RegressFunction<XT, YT>::RegressFunction (msize_t params)
:
    m_report_function(nullptr),
    m_rss(0),
    m_notFixed(params),
    m_nmax(0),
    m_epsilon(1E-9),
    m_damping(true),
    m_params(MatrixY(params, 1))
{

    Reset();

    m_fixedParams.resize(params);
    m_pindexes.resize(params);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
RegressFunction<XT, YT>::~RegressFunction()
{}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline bool RegressFunction<XT, YT>::Initialize  (const vector<XT> &x,
    const MatrixY &y,
    YT eps, unsigned nmax, bool damp
)
{

    Reset();
    //-------------------------------------------------------------------------
    // Call again in OnParamsChanged but we may return
    // before even getting there

    m_rss      = 0;

    m_nmax     = nmax;
    m_epsilon  = eps;
    m_damping  = damp;

    if (x.size() == 0 || x.size() != y.n_rows) return false;

    m_x = x;
    m_y = y;

    if (!AInitialize(m_params, m_x, m_y)) return false;

    m_fx = MatrixY(m_y.n_rows, 1);

    OnParamsChanged();

    return true;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RegressFunction<XT, YT> *
RegressFunction<XT, YT>::Clone() const
{

    RegressFunction * result = this->ACreate();
    result->Assign(*this);

    return result;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline RegressFunction<XT, YT> *
RegressFunction<XT, YT>::Clone(std::nothrow_t) const
{

    try { return Clone(); } catch(std::bad_alloc &) { return nullptr; }

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::Assign(RegressFunction const & other)
{

    m_report_function = other.m_report_function;

    m_params = other.m_params;
    m_fixedParams = other.m_fixedParams;

    m_x = other.m_x;
    m_y = other.m_y;
    m_fx = other.m_fx;

    m_p = other.m_p;

    m_error = other.m_error;

    m_alpha = other.m_alpha;
    m_beta = other.m_beta;
    m_delta = other.m_delta;

    m_iterationCounter = other.m_iterationCounter;

    m_accepted         = other.m_accepted;
    m_aborted          = other.m_aborted;

    m_improvement      = other.m_improvement;
    m_lambda           = other.m_lambda;
    m_lambdaCoeff      = other.m_lambdaCoeff;

    m_rss              = other.m_rss;
    m_notFixed         = other.m_notFixed;

    m_nmax             = other.m_nmax;
    m_epsilon          = other.m_epsilon;
    m_damping          = other.m_damping;

    AAssign(other);

}


//---------------------------------------------------------------------------
template <typename XT, typename YT>
bool RegressFunction<XT, YT>::Regress() {

    dbstream debug("debug_regress.txt", std::ios_base::trunc | std::ios_base::out);

    // INIT

    Reset();

RESTART:;

    if (m_notFixed != 0) m_improvement = -2. * m_epsilon; // 1)
    else                 m_accepted = true;
    //-------------------------------------------------
    // 1) anything so that fabs(m_improvement) > m_epsilon
    //    positive -> m_lambda *= m_lambdaCoeff at first call
    //    negative -> m_lambda /= m_lambdaCoeff at first call

    m_lambda = 1. / 16384.;

    YT        sAccepted = this->GetS();
    MatrixY   paramsAccepted(m_params);

    // DOIT

    while ( m_iterationCounter != m_nmax && fabs(m_improvement) > m_epsilon && !m_aborted ) {

        // alpha
        // $$$ alpha = p * p.GetTransposed();
        //m_alpha(m_p);

        m_alpha = m_p * trans(m_p);

        if (m_damping) {

            if 	( m_improvement < 0 ) m_lambda *= m_lambdaCoeff;
            else                      m_lambda /= m_lambdaCoeff;

            debug << "lambda: " << m_lambda << std::endl;

            m_alpha.diag(0) *= (1 + m_lambda);

        };

#if ECHMET_REGRESS_DEBUG_ALPHA == 1
        INSPECT(m_alpha)
#endif

        //beta
        // $$$ beta = p * error;
        //m_beta(m_p);
        m_beta = m_p * m_error;

#if ECHMET_REGRESS_DEBUG_BETA == 1
        INSPECT(m_beta)
#endif

        try {

            //delta
            // $$$ delta = alpha.Inverted() * beta;
            //GaussSolver<double, trRows>::Solve(m_alpha, m_delta);

            m_delta = solve(m_alpha, m_beta);

        } catch (matrix_error &) {

            debug << " !!!! SINGULARITY ERROR -> goto FINALIZE !!!! " << std::endl;
            m_accepted = false;
            goto FINALIZE;

        }


#if ECHMET_REGRESS_DEBUG_DELTA == 1
        INSPECT(m_delta)
#endif

        //params
        // $$$ params += delta;
        for (unsigned long i = 0, j = 0; i < m_fixedParams.size(); ++i) {
            if (!m_fixedParams[i]) {
                m_params(i, 0) += m_delta(j, 0);
                ++j;
            }
        }

        YT sOld = this->GetS();
        OnParamsChanged(true);
        YT sNew = this->GetS();
        m_improvement = sOld - sNew;
        m_accepted    = sNew < sAccepted + m_epsilon;
        ++m_iterationCounter;

        debug << "SOld       : " << sOld             << std::endl
              << "SNew       : " << sNew             << std::endl
              << "S Accepted : " << sAccepted        << std::endl
              << "D S        : " << sAccepted - sNew << std::endl
              << "Improvement: " << m_improvement    << std::endl
              << std::endl;

        if ( m_accepted ) {

            debug << "\nACCEPTED\n";

            paramsAccepted = m_params;
            m_improvement = sAccepted - sNew;
            sAccepted     = sNew;

        } else debug << "\nNOT ACCEPTED\n";

        Report();

        debug << "\n===========================================================\n";
        debug << std::endl;
        debug << std::endl;

    }

    // FINALIZE

FINALIZE:;

    m_params = paramsAccepted;
    OnParamsChanged(true);

    if (!m_accepted && m_iterationCounter != m_nmax && !m_aborted) {

        debug << "\n !!!!!!!!!!!!!! RESTART !!!!!!!!!!!!!!!! \n" << std::endl;

        m_lambdaCoeff *= 2;
        debug << "\n lambda: " << m_lambda << ", lamdaCoeff: " << m_lambdaCoeff << "\n" << std::endl;
        goto RESTART;

    }

    Report();

    return HasConverged();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::Abort() { m_aborted = true; }

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::Report() const {

    if (m_report_function != nullptr ) m_report_function(*this);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::RegisterReportFunction(report_function f)
{

    m_report_function = f;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
inline void RegressFunction<XT, YT>::FixParameter(ENUM id)
{

    if (IsFixedParameter(id)) return;

    m_fixedParams[static_cast<msize_t>(id)] = true;
    --m_notFixed;

    OnParamsChanged();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
inline void RegressFunction<XT, YT>::FixParameter(ENUM id, YT val)
{

    m_params(static_cast<msize_t>(id), 0) = val;

    if (!IsFixedParameter(id)) FixParameter(id);
    else                       OnParamsChanged();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
inline void RegressFunction<XT, YT>::ReleaseParameter(ENUM id)
{

    if (!IsFixedParameter(id)) return;

    m_fixedParams[static_cast<msize_t>(id)] = false;
    ++m_notFixed;

    OnParamsChanged();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
inline bool RegressFunction<XT, YT>::IsFixedParameter(ENUM id)
{

    return m_fixedParams[static_cast<msize_t>(id)];

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline bool RegressFunction<XT, YT>::HasConverged() const
{

    return m_accepted && fabs(m_improvement) < m_epsilon;

}


//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline msize_t RegressFunction<XT, YT>::GetNCount() const
{

    return m_y.n_rows;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline msize_t RegressFunction<XT, YT>::GetPCount() const
{


    return m_notFixed;

}

#if 0
//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline msize_t RegressFunction<XT, YT>::GetPTotal() const
{


    return m_params.numRows();

}
#endif

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline msize_t RegressFunction<XT, YT>::GetDF() const
{


    return GetNCount() - GetPCount();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline YT RegressFunction<XT, YT>::GetMSE() const
{

    return GetRSS() / GetNCount();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline YT RegressFunction<XT, YT>::GetS2() const
{

    return GetRSS() / GetDF();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline YT RegressFunction<XT, YT>::GetS() const
{
    using std::sqrt;

    return sqrt(GetS2());

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
inline YT RegressFunction<XT, YT>::GetParameter(ENUM id) const
{

    return m_params(static_cast<msize_t>(id), 0);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline YT RegressFunction<XT, YT>::Evaluate(XT x) const
{

    return ACalculateFx(x, m_params, -1);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline YT RegressFunction<XT, YT>::operator()(XT x) const
{

    return Evaluate(x);

}

//---------------------------------------------------------------------------
// Protected

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
const YT & RegressFunction<XT, YT>::GetParam(MatrixY const & params, ENUM id) const {

    return params(static_cast<msize_t>(id), 0);

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
template<typename ENUM>
void RegressFunction<XT, YT>::SetParam(MatrixY & params, ENUM id, YT val){

    if (!IsFixedParameter(id)) params(static_cast<msize_t>(id), 0) = val;

}

//---------------------------------------------------------------------------
// Private

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::Reset()
{

    m_iterationCounter = 0;

    m_aborted          = false;
    m_accepted         = false;

    m_improvement      = 0;
    m_lambda           = 1. / 16384.;
    m_lambdaCoeff      = 2;

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::OnParamsChanged(bool regressCall) {

    if (!regressCall) {

        Reset();

        m_p = MatrixY(m_notFixed, m_x.size());
        for (msize_t i = 0, nfx = 0; i != m_params.n_rows; ++i)
            if (!IsFixedParameter(i)) m_pindexes[nfx++] = i;

    }

#if ECHMET_REGRESS_DEBUG == 1

    dbstream debug;
    if (regressCall) {

        debug.open(ECHMET_REGRESS_DEBUG_PATH "\\debug_regress_init.txt", std::ios_base::app);
        debug << "\n========================================\n" << std::endl;
        debug << ">> PARAMS CHANGED:" << std::endl;

    } else {

        debug.open(ECHMET_REGRESS_DEBUG_PATH "\\debug_regress_init.txt", std::ios_base::trunc | std::ios_base::out);
        debug << "\n========================================\n" << std::endl;

    }

    #if ECHMET_REGRESS_DEBUG_X == 1
        //INSPECT(m_x)
    #endif

    #if ECHMET_REGRESS_DEBUG_Y == 1
        INSPECT(m_y)
    #endif

    #if ECHMET_REGRESS_DEBUG_PARAMS == 1
        INSPECT(m_params)
    #endif

#endif

    CalculateFx();
    CalculateP();
    CalculateRSS();

#if ECHMET_REGRESS_DEBUG

    #if ECHMET_REGRESS_DEBUG_P == 1
                INSPECT(m_p)
    #endif

    #if ECHMET_REGRESS_DEBUG_FX == 1
        INSPECT(m_fx)
    #endif

    #if ECHMET_REGRESS_DEBUG_ERROR == 1
        INSPECT(m_error)
    #endif

    debug << "RSS: " << m_rss << std::endl;

#endif

    if (!regressCall) Report();

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::CalculateFx () {
#pragma omp parallel for
    for (msize_t i = 0; i < m_x.size(); ++i)
    m_fx(i, 0) = ACalculateFx(m_x[i], m_params, i);
}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::CalculateP () {

    for (size_t i = 0; i != static_cast<size_t>(m_notFixed); ++i) {

        const msize_t pid = m_pindexes[i];
        #pragma omp parallel for
        for (msize_t k = 0; k < m_x.size(); ++k)
            m_p(i,k) = ACalculateDerivative(m_x[k], m_params, pid, k);

    }

#if ECHMET_REGRESS_WEIGHTS == 1

    $$$ See above

    for (size_t i = 0, j = 0; i < m_fixedParams.size(); ++i) {

        if (!m_fixedParams[i]) {

            for (msize_t k = 0; k < m_x.n_rows; ++k)
                m_p(j+1,k+1) =
                        ACalculateDerivative(m_x(k+1,1), m_params, i, k) /
                        m_s(k+1,1);

            ++j;

        }

    }

#endif

}

//---------------------------------------------------------------------------
template <typename XT, typename YT>
inline void RegressFunction<XT, YT>::CalculateRSS () {

    //Error
    // $$$ error = (y - fx) /* / s; */
    m_error = m_y;
    m_error -= m_fx;

#if ECHMET_REGRESS_WEIGHTS == 1
    // Weighing
    for (msize_t i = 0; i < m_error.data.Height(); ++i)
    m_error[i][0] /= m_s[i][0];

    // RSS Error
#   error RSS calculated from weighed errors
#else

    // RSS
    // $$$ RSS = (Error.GetTransposed() * Error)[0][0];
    MatrixY RSStmp(1,1);
    RSStmp = trans(m_error) * m_error;

    m_rss = RSStmp(0,0);

#endif

}

//---------------------------------------------------------------------------
}  //namespace regressCore

}  //namespace echmet

//===========================================================================
#endif
