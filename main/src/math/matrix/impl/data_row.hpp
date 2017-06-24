#ifndef ECHMET_MATRIX_DATA_ROWHPP
#define ECHMET_MATRIX_DATA_ROWHPP
//==============================================================================
// COMMENTS
//

//==============================================================================
// INCLUDES

#include <cstring> // memcpy

//#include "MyDebug\MyDebug.hpp"

#include "../../mystd/rac_facade.hpp"

#include "cells_pointer.hpp"
#include "rows_proxy.hpp"

#include "data_row.h"


//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix { namespace impl {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// DataRowMethods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
void DataRowMethods<TL, DTL, TR, DTR>::Assign
(

        ldata_type       & lvalue,
        rdata_type const & rvalue

        )
throw()
{

        typename rdata_type::const_iterator r  = rvalue.begin();
        for (

                typename ldata_type::iterator l      = lvalue.begin(),
                                  l_end  = lvalue.end()
                ;

                l != l_end

                ;

                ++l, ++r

        ) *l = *r;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename TR, transtags_ DTR>
void DataRowMethods<TL, DTL, TR, DTR>::Swap
(

        ldata_type const & lvalue,
        rdata_type const & rvalue

) throw()
{

#if ECHMET_MATRIX_DEFS_DEBUG_SWAP

        echmet::debug << "\nRowMethods::Swap<DTL, DTR> " << echmet::endl;

#endif

        using std::swap;

        typename rdata_type::iterator r = const_cast< rdata_type & >(rvalue).begin();

        for (

                typename ldata_type::iterator l     = const_cast<ldata_type &>(lvalue).begin(),
                                                                                                                                        l_end = const_cast<ldata_type &>(lvalue).end()
                ;

                l != l_end

                ;

                ++l, ++r

        ) swap(*l, *r);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, typename TR>
inline void DataRowMethods<TL, trNormal, TR, trNormal>::Assign
(

        ldata_type       & lvalue,
        rdata_type const & rvalue

) throw()
{

        std::memcpy(

                & lvalue[0],
                & rvalue[0],
                lvalue.Width() * sizeof(TL)

        );

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, typename TR>
inline void DataRowMethods<TL, trNormal, TR, trNormal>::Swap
(

        ldata_type const & a1,
        rdata_type const & a2

) throw()
{

        using std::swap;

        // Native

        if (a1.ptr.IsSharedWith(a2.ptr))
        {

#if ECHMET_MATRIX_DEFS_DEBUG_SWAP

                echmet::debug << "\nDataMethods::Swap<trNormal> -> Native" << echmet::endl;

#endif

                swap( *(a1.ptr.rows + a1.ptr.i), *(a2.ptr.rows + a2.ptr.i) );

                return;

        }

        // Alien
        // ! Conditional return above

#if ECHMET_MATRIX_DEFS_DEBUG_SWAP

                echmet::debug << "\nDataMethods::Swap<trNormal> -> Alien " << echmet::endl;

#endif

        union {                                                                  // 1

                int i;
                TL mem[256];

        } pool;

                                 size_t l  = a1.Width() * sizeof(TL);
        const  size_t lm = 256;

        TL * pa1 = *(a1.ptr.rows + a1.ptr.i);
        TL * pa2 = *(a2.ptr.rows + a2.ptr.i);
        while (l >= 256) {

                std::memcpy(pool.mem, pa2      , lm);
                std::memcpy(pa2     , pa1      , lm);
                std::memcpy(pa1     , pool.mem , lm);

                l   -= lm;
                pa1 += lm;
                pa2 += lm;

        }

        if (l) {

                std::memcpy(pool.mem, pa2      , l);
                std::memcpy(pa2     , pa1      , l);
                std::memcpy(pa1     , pool.mem , l);

        }

}
// 1) -> http://stackoverflow.com/questions/8166502/c-fastest-method-to-swap-two-memory-blocks-of-equal-size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ DT> template<typename TR, transtags_ DTR>
inline
DataRow<T, DT> & DataRow<T, DT>::AAssign( DataRow<TR, DTR> const & other)
throw()
{

        if (!ptr.IsSame(other.ptr))
                DataRowMethods<T, DT, TR, DTR>::Assign(*this, other)
        ;

        return *this;

}

//------------------------------------------------------------------------------
// .. rac_facade CRTP Interface

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename DataRow<T, TRANSPOSED>::iterator
DataRow<T, TRANSPOSED>::AGetIterator(difference_type j) throw()
{

        return ptr.Cell(j);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename DataRow<T, TRANSPOSED>::reference
DataRow<T, TRANSPOSED>::AGetReference(difference_type j) throw()
{

        return *AGetIterator(j);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename DataRow<T, TRANSPOSED>::size_type
DataRow<T, TRANSPOSED>::AGetSize() const throw()
{

        return this->Width();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename DataRow<T, TRANSPOSED>::size_type
DataRow<T, TRANSPOSED>::AGetMaxSize() const throw()
{

        return AGetSize();

}


//------------------------------------------------------------------------------
// .. Public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
DataRow<T, TRANSPOSED>::DataRow(RowsProxy<T, TRANSPOSED> const & aptr) throw()
: ptr(aptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
DataRow<T, TRANSPOSED>::DataRow(DataRow const & other) throw()
:
        ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED> template<typename TT>
inline
DataRow<T, TRANSPOSED>::DataRow(
        DataRow<TT, TRANSPOSED> const &	other,
        typename boost::enable_if< boost::is_convertible<TT *, T *> >::type *
) throw()
:
        ptr(other.ptr)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename Operation_Not_Allowed_For_Const_T<T, DataRow<T, TRANSPOSED> &>::type
DataRow<T, TRANSPOSED>::operator=( DataRow const & other) throw()
{

        return AAssign(other);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
template<typename TR, transtags_ TRANSPOSEDR>
inline
typename boost::enable_if
<
        boost::is_same<T, typename boost::remove_const<TR>::type >,
        DataRow<T, TRANSPOSED> &
>::type
DataRow<T, TRANSPOSED>::operator=(DataRow<TR, TRANSPOSEDR> const & other) throw()
{

        return AAssign(other);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
typename Operation_Not_Allowed_For_Const_T<T, DataRow<T, TRANSPOSED> &>::type
DataRow<T, TRANSPOSED>::operator=( T const & t) throw()
{

  for (iterator i = this->begin(), i_end = this->end(); i != i_end; ++i) *i = t;
        return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
inline
msize_t DataRow<T, TRANSPOSED>::Width() const throw()
{

  return ptr.Width();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags_ TRANSPOSED>
template<typename DUMM, transtags_ DTR>
typename boost::enable_if
<
        boost::mpl::and_
        <
                boost::is_same<T, typename boost::remove_const<DUMM>::type> ,
                boost::is_same<T, DUMM>
        >
        , void
>
::type
DataRow<T, TRANSPOSED>::swap(DataRow<DUMM, DTR> const & other)
const throw()
{

#if MATRIX_DEFS_DEBUG_SWAP

                echmet::debug << "\nDataRow::swap @ " << this << echmet::endl;

#endif

        impl::template DataRowMethods<T, TRANSPOSED, T, DTR>::Swap(*this, other);

}

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename TL, transtags_ DTL, typename DUMM, transtags_ DTR>
typename boost::enable_if
<
        boost::mpl::and_
        <
                boost::is_same<TL, typename boost::remove_const<DUMM>::type> ,
                boost::is_same<TL, DUMM>
        >
        , void
>
::type
swap(DataRow<TL, DTL> const & a1 , DataRow<DUMM, DTR> const & a2) throw()
{

#if MATRIX_DEFS_DEBUG_SWAP

                echmet::debug << "\nswap<DataRow>" << echmet::endl;

#endif

        a1.swap(a2);

}

//=============================================================================
}}} // namespace echmet :: matrix :: impl

//=============================================================================
#endif // Header

