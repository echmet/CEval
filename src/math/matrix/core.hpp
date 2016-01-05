#ifndef ECHMET_MATRIX_COREHPP
#define ECHMET_MATRIX_COREHPP
// ============================================================================
// COMMENTS:


//=============================================================================
// INCLUDES

#include <sstream>   // exception messages

//#include "MyDebug\MyDebug.hpp"

#include "gauss/gauss.hpp"

#include "data.hpp"

#include "impl/core_rowspointer.hpp"

#include "core.h"


//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// CLASS

//-----------------------------------------------------------------------------
// .. rac_facade CRTP Interface

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Core<T, TRANSPOSED>::iterator
Core<T, TRANSPOSED>::AGetIterator(difference_type i) throw()
{

        return iterator(data.AGetIterator(i));

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Core<T, TRANSPOSED>::reference
Core<T, TRANSPOSED>::AGetReference(difference_type i) throw()
{

        return reference(data.AGetReference(i));

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Core<T, TRANSPOSED>::size_type
Core<T, TRANSPOSED>::AGetSize() const throw()
{

        return data.AGetSize();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Core<T, TRANSPOSED>::size_type
Core<T, TRANSPOSED>::AGetMaxSize() const throw()
{

        return data.AGetMaxSize();

}

//-----------------------------------------------------------------------------
// .. Private CCtor

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<class T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED>::Core(Core const & other)
: data(other.data, soft_copy())
{}

//-----------------------------------------------------------------------------
// .. Public

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED>::Core(msize_t adataHeight, msize_t adataWidth)
: data(adataHeight, adataWidth)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED>
::Core(msize_t adataHeight, msize_t adataWidth, const T & d)
: data(adataHeight, adataWidth, d)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED>
::Core(msize_t adataHeight, msize_t adataWidth, const T * data_linear)
: data(adataHeight, adataWidth, data_linear)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY, typename COPY>
Core<T, TRANSPOSED>::Core(Core<T, ANY> const & other, COPY c)
: data(other.data, c)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
Core<T, TRANSPOSED>::Core(Core<T, ANY> & other, steal_copy c)
: data(other.data, c)
{}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline Core<T, TRANSPOSED>::~Core() throw()
{}

//-----------------------------------------------------------------------------
// .. Methods

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::Unique()
{

  data.Unique();
        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::Row
Core<T, TRANSPOSED>::Rows(msize_t i) throw()
{

        return Row( data.Rows(i) );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::ConstRow
Core<T, TRANSPOSED>::Rows(msize_t i) const throw()
{

        return ConstRow( data.Rows(i) );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::Row
Core<T, TRANSPOSED>::RowsEnd() throw()
{

        return Row( data.RowsEnd() );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::ConstRow
Core<T, TRANSPOSED>::RowsEnd() const throw()
{

        return ConstRow( data.RowsEnd() );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::Diagonal
Core<T, TRANSPOSED>::GetDiagonal() throw()
{

        return Diagonal( data.GetDiagonal() );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::ConstDiagonal
Core<T, TRANSPOSED>::GetDiagonal() const throw()
{

        return ConstDiagonal( data.GetDiagonal() );

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Core<T, TRANSPOSED>::Transposed
Core<T, TRANSPOSED>::GetTransposed() throw()
{

        return Transposed(*this, soft_copy());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
const typename Core<T, TRANSPOSED>::Transposed
Core<T, TRANSPOSED>::GetTransposed() const throw()
{

        return Transposed(*this, soft_copy());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void Core<T, TRANSPOSED>::swap(Core<T, TRANSPOSED> & other) throw()
{

        data.swap(other.data);

}

//-----------------------------------------------------------------------------
// .. Arithmetics

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED> template<transtags DTR>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator+=(Core<T, DTR> const & rhs)
{

        // Init

        typedef Core<T, trRows>                       lhs_type;
        typedef Core<T, transtags(TRANSPOSED != DTR)> rhs_type;

        lhs_type alhs(*this, soft_copy());
        rhs_type arhs(rhs,   soft_copy());

        // Doit

        typename rhs_type::ConstRow r = arhs[0];
        for (

                typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
                i.ptr != i_end.ptr
                ;
                ++(i.ptr), ++(r.ptr)

        ) i += r;

        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED> template<transtags DTR>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator-=(Core<T, DTR> const & rhs)
{

        // Init

        typedef Core<T, trRows>                       lhs_type;
        typedef Core<T, transtags(TRANSPOSED != DTR)> rhs_type;

        lhs_type alhs(*this, soft_copy());
        rhs_type arhs(rhs,   soft_copy());

        // Doit

        typename rhs_type::ConstRow r = arhs[0];
        for (

                typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
                i.ptr != i_end.ptr
                ;
                ++(i.ptr), ++(r.ptr)

        ) i -= r;

        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//.............................................................................
template<transtags TRANSPOSED>
inline
msize_t const & CoreMultiplySizeSelection(msize_t const  &, msize_t const  &);

//.............................................................................
template<>
inline
msize_t const & CoreMultiplySizeSelection<trRows>
(msize_t const  & a, msize_t const  & b)
{ return a; }

//.............................................................................
template<>
inline
msize_t const & CoreMultiplySizeSelection<trColumns>
(msize_t const  & a, msize_t const  & b)
{ return b; }


//.............................................................................
template<typename T, transtags TRANSPOSED> template<transtags DTR>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator*=(Core<T, DTR> const & rhs)
{

        Core<T, TRANSPOSED> result
        (

                CoreMultiplySizeSelection<TRANSPOSED>( data.Height()   , rhs.data.Width() ),
                CoreMultiplySizeSelection<TRANSPOSED>( rhs.data.Width(), data.Height()    )

        );

        typename Core<T, DTR>::Transposed rhsT(rhs, soft_copy());

        Row resultRow = result.Rows();
        for (

                ConstRow r = Rows(), r_end = RowsEnd()
                ;
                r.ptr != r_end.ptr
                ;
                ++(r.ptr), ++(resultRow.ptr)

        ) {

                typename Row::iterator resultCell = resultRow.begin();

                for (

                        typename Core<T, DTR>::Transposed::ConstRow
        rr     = rhsT.Rows()    ,
        rr_end = rhsT.RowsEnd()
                        ;
                        rr.ptr != rr_end.ptr
                        ;
                        ++(rr.ptr), ++resultCell

                ) *resultCell = r * rr;

        }

        data.StealCopy(result.data);

        return *this;

}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 1) means Core<T, transtags(TRANSPOSED != DTR)>::Transposed
//    - result[i][j] = [*this::Row] * [rhs::Column]
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator+=(T const & rhs)
{

        // Init

        if (rhs == 0) return *this;

        typedef Core<T, trRows> lhs_type;

        lhs_type alhs(*this, soft_copy());

        // Doit

        for (

                typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
                i.ptr != i_end.ptr
                ;
                ++(i.ptr)

        ) i += rhs;

        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator-=(T const & rhs)
{

        // Init

        if (rhs == 0) return *this;

        typedef Core<T, trRows> lhs_type;

        lhs_type alhs(*this, soft_copy());

        // Doit

        for (

                typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
                i.ptr != i_end.ptr
                ;
                ++(i.ptr)

        ) i -= rhs;

        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator*=(T const & rhs)
{

        // Init

        if (rhs == 1) return *this;

        typedef Core<T, trRows> lhs_type;

        lhs_type alhs(*this, soft_copy());

        // Doit

        if (rhs == 0) {

    for (

      typename lhs_type::Row i     = alhs.Rows(),
                             i_end = alhs.RowsEnd()
      ;
      i.ptr != i_end.ptr
      ;
      ++(i.ptr)

    ) i = 0;

        }	else {

    for (

      typename lhs_type::Row i     = alhs.Rows(),
                             i_end = alhs.RowsEnd()
      ;
      i.ptr != i_end.ptr
      ;
      ++(i.ptr)

    ) i *= rhs;

        }

        return *this;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::operator/=(T const & rhs)
{

        // Init

        if (rhs == 1) return *this;

        typedef Core<T, trRows> lhs_type;

        lhs_type alhs(*this, soft_copy());

        // Doit

  for (

    typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
    i.ptr != i_end.ptr
    ;
    ++(i.ptr)

  ) i /= rhs;

        return *this;

}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
Core<T, TRANSPOSED> &
Core<T, TRANSPOSED>::Negate()
{

        // Init

        typedef Core<T, trRows> lhs_type;

  lhs_type alhs(*this, soft_copy());

        // Doit

  for (

    typename lhs_type::Row i     = alhs.Rows(),
                           i_end = alhs.RowsEnd()
    ;
    i.ptr != i_end.ptr
    ;
    ++(i.ptr)

  ) i->Negate();

        return *this;

}

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline void swap(Core<T, TRANSPOSED> & a1, Core<T, TRANSPOSED> & a2) throw()
{

        a1.swap(a2);

}

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header

