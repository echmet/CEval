#ifndef ECHMET_MATRIX_DATAH
#define ECHMET_MATRIX_DATAH
//=============================================================================
// COMMENTS

// $ 0) -> DataAccessPolicy.docx

// $!1) Matrix must be based on a NON-const POD type T
//      - If encountering a hardly understandable compile-time error
//        saying something related to the boost::enable_if,
//        the T most probably does not fulfill this requirements
//
//      Note: Specialize boost::is_pod for any non-build-in type class T
//            as (and if) need and appropriate
//            -> http://www.boost.org/doc/libs/1_57_0/libs/type_traits/doc/html/boost_typetraits/reference/is_pod.html
//
//      For some arithmetic operations, T mus be further comparable to int
//      ( specifically 0 and 1)
//
// -> referenced from data.hpp



// $ 2) IsUnique returns true for uninitialized data, too

//=============================================================================
// INCLUDES

#include <boost/type_traits/is_pod.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/or.hpp>

#include "../mystd/rac_facade.h"

#include "impl/data_rowspointer.h"

//=============================================================================
// MACROS

#define ECHMET_MATRIX_DATA_FRIENDS                                             \
        template<typename TT, transtags ANY> friend class Data;                      \
        friend struct impl::DataMethods<T, TRANSPOSED>;                               \
        friend class Core<T, TRANSPOSED>;                                            \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// NAMESPACE impl

namespace impl {

//-----------------------------------------------------------------------------
// DataMethods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> struct DataMethods<T, trRows> {
private:
        friend class Data<T, trRows>;

        static msize_t Height(const Data<T, trRows> &) throw();
        static msize_t Width (const Data<T, trRows> &) throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T> struct DataMethods<T, trColumns> {
private:
        friend class Data<T, trColumns>;

        static msize_t Height(const Data<T, trColumns> &) throw();
        static msize_t Width (const Data<T, trColumns> &) throw();

};

//=============================================================================
} // namespace impl

//-----------------------------------------------------------------------------
// CLASS

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $!1)
template<typename T, transtags TRANSPOSED>
class Data :
public

  boost::disable_if
        <

                boost::mpl::or_
                <
      boost::mpl::not_< boost::is_pod<T> > ,
      boost::is_const<T>
    >

                ,

                typename myStd::mkrac_facade
                <

                        Data<T, TRANSPOSED>
                        ,
                        myStd::container_iterators_list<

                                impl::RowsPointer<T,       impl::transtags_(TRANSPOSED)>,
                                impl::RowsPointer<const T, impl::transtags_(TRANSPOSED)>

                        >     // container_iterators_list

                >::type // rac_facade

        >::type   // disable_if

{

public:

        typedef impl::DataRow  <T,       impl::transtags_(TRANSPOSED)> Row;
        typedef impl::DataRow  <const T, impl::transtags_(TRANSPOSED)> ConstRow;

        typedef impl::DataRow  <T,       impl::trDiagonal>             Diagonal;
        typedef impl::DataRow  <const T, impl::trDiagonal>             ConstDiagonal;

        typedef Data<T, transtags(!TRANSPOSED)>                        Transposed;

private:

        ECHMET_MATRIX_DATA_FRIENDS

        T              * data;                    // shared memory for data
        size_t         * counter;                 // shared references' counter
        T             ** rows;                    // row indexes (pointers)
        msize_t          dataHeight, dataWidth;

        template<transtags ANY>
        void   AShallowCopy (const Data<T, ANY> &) throw();

        // rac_facade CRTP interface

        typedef typename
        myStd::mkrac_facade<

                Data<T, TRANSPOSED>
                ,
                myStd::container_iterators_list<

                        impl::RowsPointer<T,       impl::transtags_(TRANSPOSED)> ,
                        impl::RowsPointer<const T, impl::transtags_(TRANSPOSED)>

                >

        >::type rac_facade_type;

        MYSTD_RAC_FACADE_INTRODUCE(rac_facade_type)
        MYSTD_RAC_FACADE_INTERFACE

        // <

        // NonCopyable

        Data(Data const &);           // implemented : soft_copy -> Transpose return
        void operator=(const Data &); // N/A

        // ~~~

        void Null() throw();

public:

        // Constructors

        explicit Data(msize_t = 0, msize_t = 0);
        Data(msize_t, msize_t, const T &);
        Data(msize_t, msize_t, const T *);

        template<transtags ANY> Data(Data<T, ANY> const &, soft_copy);
        template<transtags ANY> Data(Data<T, ANY> const &, raw_copy);
        template<transtags ANY> Data(Data<T, ANY>       &, steal_copy);

        ~Data() throw();

        // Allocation, deallocation

        Data & New    (msize_t, msize_t) ;
        Data & New    (msize_t, msize_t, const T &) ;
        Data & New    (msize_t, msize_t, const T *) ;
        void   Free   (void)             throw();

        // Copiers

        template<transtags ANY> Data & RawCopy   (const Data<T, ANY> &);
        template<transtags ANY> Data & SoftCopy  (const Data<T, ANY> &) throw();
        template<transtags ANY> Data & StealCopy (Data<T, ANY> &)       throw();

        Data &        Unique();

        // Methods

        Row              Rows(msize_t = 0)       throw();
        ConstRow         Rows(msize_t = 0) const throw();

        Row              RowsEnd()               throw();
        ConstRow         RowsEnd()         const throw();

        Diagonal         GetDiagonal()           throw();
        ConstDiagonal    GetDiagonal()     const throw();

        Transposed       GetTransposed()         throw();
        const Transposed GetTransposed()   const throw();

        // Informers

        msize_t Height()        const throw();
        msize_t Width()         const throw();

        msize_t DataHeight()    const throw();
        msize_t DataWidth()     const throw();

        size_t  GetRefCounter() const throw();

        bool    IsInitialized() const throw();
        bool    IsUnique()      const throw();                                  // $2
        bool    IsShared()      const throw();

        template<transtags OTHERTRANS>
        bool    IsSharedWith(const Data<T, OTHERTRANS> & other) const throw();

        // Std

        void swap(Data<T, TRANSPOSED> &) throw();

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, bool TRANSPOSED>
void swap(Data<T, TRANSPOSED> & a1, Data<T, TRANSPOSED> & a2) throw();

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header

