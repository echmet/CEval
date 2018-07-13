#ifndef ECHMET_MATRIX_COREH
#define ECHMET_MATRIX_COREH
//=============================================================================
// COMMENTS

// $1) -> $ C1 in fwdcl.h

// $2) We cannot derive from Data for ambiguous rac_facade interface *)
//     We don't want private Data or else we would have to map the whole
//     public Data interface
//     *) There is a solution but too complex for now
//        -> http://stackoverflow.com/questions/18174441/crtp-and-multilevel-inheritance
//
//     We do not repeat enable_if policy for Core:
//     Data already serves the purpose

//=============================================================================
// INCLUDES

#include "data.h"

#include "impl/core_rowspointer.h"

//=============================================================================
// DEFINES

// $1
#define ECHMET_MATRIX_CORE_FRIENDS                                             \
	template<typename, transtags> friend class Core;                             \

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// CODE

//-----------------------------------------------------------------------------
// > CORE >
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// $2
template<typename T, transtags TRANSPOSED>
class Core
: public

  myStd::mkrac_facade
  <

    Core<T, TRANSPOSED>
    ,
    myStd::container_iterators_list<

      impl::CoreRowsPointer<T,       impl::transtags_(TRANSPOSED)>,
      impl::CoreRowsPointer<const T, impl::transtags_(TRANSPOSED)>

    >      // container_iterators_list

  >::type // rac_facade

{

public:
	typedef impl::CoreRow  <T,       impl::transtags_(TRANSPOSED)> Row;
	typedef impl::CoreRow  <const T, impl::transtags_(TRANSPOSED)> ConstRow;

	typedef impl::CoreRow  <T,       impl::trDiagonal>             Diagonal;
	typedef impl::CoreRow  <const T, impl::trDiagonal>             ConstDiagonal;

	typedef Core<T, transtags(!TRANSPOSED)>                        Transposed;

private:

	ECHMET_MATRIX_CORE_FRIENDS

	// rac_facade CRTP interface

	typedef

		typename myStd::mkrac_facade
		<

			Core<T, TRANSPOSED>
			,
			myStd::container_iterators_list<

        impl::CoreRowsPointer<T,       impl::transtags_(TRANSPOSED)>,
        impl::CoreRowsPointer<const T, impl::transtags_(TRANSPOSED)>

			>      // container_iterators_list

		>::type // rac_facade

	rac_facade_type;

	MYSTD_RAC_FACADE_INTRODUCE(rac_facade_type)
	MYSTD_RAC_FACADE_INTERFACE

	// <

	// NonCopyable

	Core(Core const  &);   	       // implemented soft_copy -> Transpose return
	Core & operator=(Core const & other);

public:

	Data<T, TRANSPOSED> data;                                               // $2

	// Constructors
	explicit Core(msize_t = 0, msize_t = 0);
	Core(msize_t, msize_t, const T &);
	Core(msize_t, msize_t, const T *);

	template<transtags ANY, typename COPY> Core(Core<T, ANY> const &, COPY);
	template<transtags ANY>                Core(Core<T, ANY> &, steal_copy);

	~Core() throw();

	// Allocation, deallocation
	// -> Data

	// Copiers
	// -> Data

	// Data :: Core - specific override

	Core &           Unique();

	Row              Rows(msize_t = 0)       throw();
	ConstRow         Rows(msize_t = 0) const throw();

	Row              RowsEnd()               throw();
	ConstRow         RowsEnd()         const throw();

	Diagonal         GetDiagonal()           throw();
	ConstDiagonal    GetDiagonal()     const throw();

	Transposed       GetTransposed()         throw();
	const Transposed GetTransposed()   const throw();

	void swap(Core<T, TRANSPOSED> &)         throw();

	// Arithmetics

	template<transtags DTR>	Core & operator+=(Core<T, DTR> const &);
	template<transtags DTR>	Core & operator-=(Core<T, DTR> const &);
	template<transtags DTR>	Core & operator*=(Core<T, DTR> const &);

	Core & operator+=(T const &);
	Core & operator-=(T const &);
	Core & operator*=(T const &);
	Core & operator/=(T const &);

	Core & Negate();

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
// STD

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template<typename T, transtags TRANSPOSED>
void swap(Core<T, TRANSPOSED> & a1, Core<T, TRANSPOSED> & a2) throw();

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header

