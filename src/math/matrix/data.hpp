#ifndef ECHMET_MATRIX_DATAHPP
#define ECHMET_MATRIX_DATAHPP
// ============================================================================
// COMMENTS:

// $1) casting to (void *) in order to be able to compare
//     cores of different transposition states
//
//     -> gcc error: comparison between distinct pointer types
//        'const Matrix::Data<double, (transtags)1u>*' and
//        'Matrix::Data<double, (transtags)0u>*'
//        lacks a cast

//=============================================================================
// INCLUDES

#include <algorithm> // swap
#include <cstring>   // memcpy

//#include "MyDebug\MyDebug.hpp"

#include "../mystd/rac_facade.hpp"

#include "impl/data_rowspointer.hpp"

#include "data.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// NAMESPACE impl

namespace impl {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline msize_t
DataMethods<T, trRows>::Height(const Data<T, trRows> & adata) throw()
{

 return adata.dataHeight;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline msize_t
DataMethods<T, trRows>::Width(const Data<T, trRows> & adata) throw()
{

 return adata.dataWidth;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline msize_t
DataMethods<T, trColumns>::Height(const Data<T, trColumns> & adata)
throw()
{

 return adata.dataWidth;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T>
inline msize_t
impl::DataMethods<T, trColumns>::Width(const Data<T, trColumns> & adata)
throw()
{

 return adata.dataHeight;

}

//=============================================================================
} // namespace impl

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
inline void Data<T, TRANSPOSED>::AShallowCopy(const Data<T, ANY> & other)
throw()
{

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "\nACpy @ " << this << echmet::endl;

#endif

  data        = other.data;
  rows        = other.rows;
  counter     = other.counter;
  dataHeight  = other.dataHeight;
  dataWidth   = other.dataWidth;

}

//-----------------------------------------------------------------------------
// .. rac_facade CRTP Interface

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Data<T, TRANSPOSED>::iterator
Data<T, TRANSPOSED>::AGetIterator(difference_type i) throw()
{

        return iterator(impl::RowsProxy<T, impl::transtags_(TRANSPOSED)>(rows, Width(), i));

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Data<T, TRANSPOSED>::reference
Data<T, TRANSPOSED>::AGetReference(difference_type i) throw()
{

        return reference(impl::RowsProxy<T, impl::transtags_(TRANSPOSED)>(rows, Width(), i));

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Data<T, TRANSPOSED>::size_type
Data<T, TRANSPOSED>::AGetSize() const throw()
{

        return Height();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
typename Data<T, TRANSPOSED>::size_type
Data<T, TRANSPOSED>::AGetMaxSize() const throw()
{

        return AGetSize();

}

//-----------------------------------------------------------------------------
// .. Private Ctor and Null

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline Data<T, TRANSPOSED>::Data(Data const & other)
: data(other.data), counter(other.counter), rows(other.rows),
        dataHeight(other.dataHeight), dataWidth(other.dataWidth)
{

        if (counter) ++(*counter);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline void
Data<T, TRANSPOSED>::Null() throw()
{

        counter    = NULL;
        data       = NULL;
        rows       = NULL;
        dataHeight = 0;
        dataWidth  = 0;

}

//------------------------------------------------------------------------------
// .. Public

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline Data<T, TRANSPOSED>::Data(msize_t adataHeight, msize_t adataWidth)
: data(NULL), counter(NULL), rows(NULL), dataHeight(0), dataWidth(0)
{

  New(adataHeight, adataWidth);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
Data<T, TRANSPOSED>::Data
(msize_t adataHeight, msize_t adataWidth, const T & d)
: data(NULL), counter(NULL), rows(NULL), dataWidth(0), dataHeight(0)
{

  New(adataHeight, adataWidth, d);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
Data<T, TRANSPOSED>::Data
(msize_t adataHeight, msize_t adataWidth, const T * data_linear)
: data(NULL), counter(NULL), rows(NULL), dataHeight(0), dataWidth(0)
{

        New(adataHeight, adataWidth, data_linear);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
inline Data<T, TRANSPOSED>::Data(Data<T, ANY> const & other, soft_copy)
: data(other.data), counter(other.counter), rows(other.rows),
        dataHeight(other.dataHeight), dataWidth(other.dataWidth)
{

        if (counter) ++(*counter);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
inline Data<T, TRANSPOSED>::Data(Data<T, ANY> const & other, raw_copy)
: data(NULL), counter(NULL), rows(NULL), dataHeight(0), dataWidth(0)
{

        RawCopy(other);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
inline Data<T, TRANSPOSED>::Data(Data<T, ANY> & other, steal_copy)
: data(other.data), counter(other.counter), rows(other.rows),
        dataHeight(other.dataHeight), dataWidth(other.dataWidth)
{

        other.Null();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline Data<T, TRANSPOSED>::~Data() throw()
{

        Free();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
Data<T, TRANSPOSED> & Data<T, TRANSPOSED>::New
(msize_t adataHeight, msize_t adataWidth)
{
#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "\nNew     @ " << this << echmet::endl << "..." << echmet::endl;

#endif

  if (!counter || *counter || adataHeight != dataHeight || adataWidth != dataWidth) {       // 1), 2)

    Free();

    if ( adataHeight == 0 || adataWidth == 0 ) {

#if MATRIX_DEFS_DEBUG_ALLOC

                        echmet::debug << "...(New @ " << this << ") ..."                               << echmet::endl
                    << "Zero allocation -> return"                                   << echmet::endl
                    << "Data    @ " << data                                          << echmet::endl
                    << "Rows    @ " << rows                                          << echmet::endl
                    << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                    << echmet::endl
      ;

#endif

      return *this;

    }

    try{

      counter  = new size_t;
      *counter = 0;

      data = new T  [adataHeight * adataWidth];
      rows = new T* [adataHeight];

    }
    catch(...){

#if MATRIX_DEFS_DEBUG_ALLOC

      echmet::debug << "...(New @ " << this << ") ..."                               << echmet::endl
                    << "<EXCEPTION>"                                                 << echmet::endl
                    << "Data    @ " << data                                          << echmet::endl
                    << "Rows    @ " << rows                                          << echmet::endl
                    << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                    << echmet::endl
      ;

#endif

      Free();
      throw;

    }

  }

  // mapping rows
  T **r_end  = rows + adataHeight;
  T *p       = data;
  for (T **r = rows; r != r_end; ++r, p += adataWidth) *r = p; //nothing if _m == 0

  dataHeight        = adataHeight;
  dataWidth         = adataWidth;

#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "...(New @ " << this << ") ..."                               << echmet::endl
                 << "Data    @ " << data                                          << echmet::endl
                 << "Rows    @ " << rows                                          << echmet::endl
                 << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                 << echmet::endl
   ;

#endif

  return *this;

}
// 1) 1.a) if adataHeight != dataHeight, Rows must be reallocated
//    1.b) if adataWidth  != dataWidth,  data reallocation is needed since
//         adataWidth*adataHeight != dataWidth*dataHeight
//         ( or otherwise 1.a) )
//
// 2) !counter || *counter means "not allocated at all OR IsShared()"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
Data<T, TRANSPOSED> & Data<T, TRANSPOSED>::New
(msize_t adataHeight, msize_t adataWidth, const T & d)
{

  New(adataHeight, adataWidth);

  for (int i = 0; i != adataHeight; ++i)
    for (int j = 0; j != adataWidth; ++j)
      if (i == j) rows[i][j] = d;
      else        rows[i][j] = 0
  ;

 return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
Data<T, TRANSPOSED> & Data<T, TRANSPOSED>::New
(msize_t adataHeight, msize_t adataWidth, const T * data_linear)
{

  New(adataHeight, adataWidth);

  std::memcpy(data, data_linear, adataHeight * adataWidth * sizeof(T));  // $! 1

 return *this;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void Data<T, TRANSPOSED>::Free(void) throw()
{

#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "\nFree    @ " << this                                          << echmet::endl
                 << "Data    @ "   << data                                          << echmet::endl
                 << "Rows    @ "   << rows                                          << echmet::endl
                 << "Counter @ "   << counter << " = " << (counter ? *counter : -1) << echmet::endl
                 << (counter ? "..." : "")                                          << echmet::endl
   ;

#endif

  if (!counter) return;

        if (*counter) --(*counter);
  else
  {

#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "... (Free @" << this << ") ..."  << echmet::endl
                 << "FREE"                            << echmet::endl
   ;

#endif

    delete   counter;
    delete[] data;
    delete[] rows;

  }

        Null();                                                                 // 1

}
// 1) Nullifying is not normally necessary but we want to be
//    on the safe side in case of something goes wrong in the New method
//    - and it is not too high overhead
//      (optimizer may even optimize this out when called from the dtor)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
Data<T, TRANSPOSED> &
Data<T, TRANSPOSED>::RawCopy(const Data<T, ANY> & other)
{

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "\nRawCpy @ " << this << echmet::endl;

#endif

  if ( (void*)(&other) == (void*)(this) && IsUnique() ) {              // 0, $1
#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "...(RawCpy @ " << this << ") ..."    << echmet::endl
                 << "Self copy of Unique -> nothing done" << echmet::endl
                 << echmet::endl;

#endif // MATRIX_DEFS_DEBUG_ALLOC

    return *this;

  }

  const T *  cpdata = other.data;                                          // 1
  T **       cprows = other.rows;

  New(other.dataHeight, other.dataWidth);                                  // 2

  // -> $!1 in data.h
  std::memcpy(data, cpdata, dataHeight*dataWidth*sizeof(T));

  // mapping rows
  for (

    T **r = rows, **r_end   = rows + dataHeight;
    r != r_end;
    ++r, ++cprows

  ) *r = data + (*cprows - cpdata);

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "...(RawCpy @ " << this << ") ..."                            << echmet::endl
                << "Data    @ " << data                                          << echmet::endl
                << "Rows    @ " << rows                                          << echmet::endl
                << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                << echmet::endl
  ;

#endif

  return *this;

}
// 0) Reallocation of itself with no data sharing -> ignore

// 1) New calls Free as needed
//    This may lower *counter if IsSharedWith(other) but will never release
//    data and rows (or otherwise 0) )

// 2) Need to backup data and rows in case of self-copy.
//    cpdata and cprows will remain valid after calling New since $this$ is
//    shared (or otherwise 0) )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
inline Data<T, TRANSPOSED> &
Data<T, TRANSPOSED>::SoftCopy(const Data<T, ANY> & other) throw()
{

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "\nSoftCpy @ " << this << echmet::endl;

#endif

  if (IsSharedWith(other)) {
#if MATRIX_DEFS_DEBUG_ALLOC

    echmet::debug << "...(SoftCpy @ " << this << ") ..." << echmet::endl
                  << "Self copy -> nothing done"         << echmet::endl
                  << echmet::endl;

#endif // MATRIX_DEFS_DEBUG_ALLOC

    return *this;

  }

        Free();

        AShallowCopy(other);
        if (counter) ++(*counter);                                               // 1

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "...(SoftCpy @ " << this << ") ..."                           << echmet::endl
                << "Data    @ " << data                                          << echmet::endl
                << "Rows    @ " << rows                                          << echmet::endl
                                                                << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                                                                << echmet::endl
  ;

#endif

  return *this;

}
// 1) if (couter) : In case of copy from an uninitialized
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
template<transtags ANY>
Data<T, TRANSPOSED> &
Data<T, TRANSPOSED>::StealCopy(Data<T, ANY> & other) throw()
{
#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "\nStealCpy @ " << this << echmet::endl;

#endif

  if ( (void *)(&other) == (void *)(this) ) {                          // 1, $1
#if MATRIX_DEFS_DEBUG_ALLOC

   echmet::debug << "...(StealCpy @ " << this << ") ..." << echmet::endl
                 << "Stealing from self -> nothing done" << echmet::endl
                 << echmet::endl;

#endif // MATRIX_DEFS_DEBUG_ALLOC

    return *this;

  }

        Free();

        AShallowCopy(other);
        other.Null();

#if MATRIX_DEFS_DEBUG_ALLOC

  echmet::debug << "...(StealCpy @ " << this << ") ..."                          << echmet::endl
                << "Data    @ " << data                                          << echmet::endl
                << "Rows    @ " << rows                                          << echmet::endl
                                                                << "Counter @ " << counter << " = " << (counter ? *counter : -1) << echmet::endl
                << echmet::endl
  ;

#endif

  return *this;

}
// 1) Not(!) $if IsSharedWith(other)$.
//    If IsSharedWith(other) we still want to call
//    other.Free()
//    Note: in this case we don't need to call Free and AShallowCopy
//          but I decided not to complicate the whole thing with another if
//          - the overhead is not too big and StealCopy-ing from IsShared(other)
//            will be rare

// 2) casting pointer to (void *) in order to be able to compare
//    trRows and trColumns - based cores
//    -> gcc error: comparison between distinct pointer types ... and ... lacks a cast
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline Data<T, TRANSPOSED> & Data<T, TRANSPOSED>::Unique()
{

        return IsUnique() ? *this : RawCopy(*this);

}

//----------------------------------------------------------------------------
// .. Methods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::Row
Data<T, TRANSPOSED>::Rows(msize_t i) throw()
{

        return this->operator[](i);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::ConstRow
Data<T, TRANSPOSED>::Rows(msize_t i) const throw()
{

        return this->operator[](i);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::Row
Data<T, TRANSPOSED>::RowsEnd() throw()
{

        return Rows(Height());

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::ConstRow
Data<T, TRANSPOSED>::RowsEnd() const throw()
{

        return Rows(Height());

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::Diagonal
Data<T, TRANSPOSED>::GetDiagonal() throw()
{

  return Diagonal( impl::RowsProxy<T, impl::trDiagonal>(rows, Height(), 0) );

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::ConstDiagonal
Data<T, TRANSPOSED>::GetDiagonal() const throw()
{

  return ConstDiagonal( impl::RowsProxy<const T, impl::trDiagonal>((const T **)rows, Height(), 0) );

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
typename Data<T, TRANSPOSED>::Transposed
Data<T, TRANSPOSED>::GetTransposed() throw()
{

        return Transposed(*this, soft_copy());

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline
const typename Data<T, TRANSPOSED>::Transposed
Data<T, TRANSPOSED>::GetTransposed() const throw()
{

        return Transposed(*this, soft_copy());

}

//-----------------------------------------------------------------------------
// .. Informers

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline msize_t Data<T, TRANSPOSED>::Height() const throw()
{

        return impl::DataMethods<T, TRANSPOSED>::Height(*this);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline msize_t Data<T, TRANSPOSED>::Width() const throw()
{

  return impl::DataMethods<T, TRANSPOSED>::Width(*this);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline msize_t Data<T, TRANSPOSED>::DataHeight() const throw()
{

  return dataHeight;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline msize_t Data<T, TRANSPOSED>::DataWidth() const throw()
{

  return dataWidth;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline size_t Data<T, TRANSPOSED>::GetRefCounter() const throw()
{

        return counter ? *counter : 0;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline bool Data<T, TRANSPOSED>::IsInitialized() const throw()
{

  return counter != NULL;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline bool Data<T, TRANSPOSED>::IsUnique() const throw()
{

  return GetRefCounter() == 0;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline bool Data<T, TRANSPOSED>::IsShared() const throw()
{

  return GetRefCounter() != 0;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED> template<transtags OTHERTRANS>
bool Data<T, TRANSPOSED>::IsSharedWith(const Data<T, OTHERTRANS> & other)
const throw()
{

        return counter == other.counter;

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void Data<T, TRANSPOSED>::swap(Data<T, TRANSPOSED> & other) throw()
{

        using std::swap;

        swap(data,       other.data);
        swap(counter,    other.counter);
        swap(rows,       other.rows);
        swap(dataHeight, other.dataHeight);
        swap(dataWidth,  other.dataWidth);

}

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
inline void swap(Data<T, TRANSPOSED> & a1, Data<T, TRANSPOSED> & a2) throw()
{

  a1.swap(a2);

}

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header
