#ifndef MYSTD_RAC_FACADE_H
#define MYSTD_RAC_FACADE_H
//==============================================================================
// COMMENTS


//==============================================================================
// INCLUDES

#include <boost/mpl/not.hpp>
#include <boost/mpl/and.hpp>
#include <boost/iterator/reverse_iterator.hpp>

//=============================================================================
// MACROS

#define MYSTD_RAC_FACADE_INTRODUCE(FACADE)                                   \
																																						 \
	friend FACADE;                                                           \
																																						 \
	public:                                                                    \
																																						 \
	typedef typename FACADE::value_type             value_type;                \
	typedef typename FACADE::reference              reference;                 \
	typedef typename FACADE::const_reference        const_reference;           \
	typedef typename FACADE::pointer                pointer;                   \
	typedef typename FACADE::const_pointer          const_pointer;             \
																																						 \
	typedef typename FACADE::iterator               iterator;                  \
	typedef typename FACADE::const_iterator         const_iterator;            \
	typedef typename FACADE::reverse_iterator       reverse_iterator;          \
	typedef typename FACADE::const_reverse_iterator const_reverse_iterator;    \
																																						 \
	typedef typename FACADE::size_type              size_type;                 \
	typedef typename FACADE::difference_type        difference_type;           \
																																						 \
	private:                                                                   \


#define MYSTD_RAC_FACADE_INTERFACE                                           \
	iterator  AGetIterator (difference_type) throw();                          \
	reference AGetReference(difference_type) throw();                          \
																																						 \
	size_type AGetSize     () const throw();                                   \
	size_type AGetMaxSize  () const throw();                                   \




//=============================================================================
// NAMESPACE

namespace myStd {

//=============================================================================
// DCL

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename T                ,
	typename PTR  = T *       ,
	typename REF  = T &       ,
	typename CPRT = T const * ,
	typename CREF = T const &

> struct container_types_list;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename IT                                  ,
	typename CIT                                 ,
	typename RIT   = boost::reverse_iterator<IT> ,
	typename CRIT  = boost::reverse_iterator<CIT>

> struct container_iterators_list;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename SIZE    = std::size_t    ,
	typename PTRDIFF = std::ptrdiff_t

> struct container_sizes_list;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename DERIVED                      ,
	typename T                            ,
	typename CIL                          ,
	typename CTL = container_types_list<T>,
	typename CSL = container_sizes_list<>

> class rac_facade;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename DERIVED            ,
	typename CIL                ,
	typename SIZE = std::size_t

> struct mkrac_facade;


//=============================================================================
// DEF

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename T    ,
	typename PTR  ,
	typename REF  ,
	typename CPTR ,
	typename CREF

> struct container_types_list
{

	typedef T    value_type;
	typedef PTR  pointer;
	typedef REF  reference;
	typedef CPTR const_pointer;
	typedef CREF const_reference;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename IT   ,
	typename CIT  ,
	typename RIT  ,
	typename CRIT

> struct container_iterators_list
{

	typedef IT   iterator;
	typedef CIT  const_iterator;
	typedef RIT  reverse_iterator;
	typedef CRIT const_reverse_iterator;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename SIZE    ,
	typename PTRDIFF

> struct container_sizes_list
{

	typedef SIZE    size_type;
	typedef PTRDIFF difference_type;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename DERIVED,
	typename T,
	typename CIL,
	typename CTL,
	typename CSL

> class rac_facade
{
public:

	typedef typename CTL::value_type             value_type;
	typedef typename CTL::reference              reference;
	typedef typename CTL::const_reference        const_reference;
	typedef typename CTL::pointer                pointer;
	typedef typename CTL::const_pointer          const_pointer;

	typedef typename CIL::iterator               iterator;
	typedef typename CIL::const_iterator         const_iterator;
	typedef typename CIL::reverse_iterator       reverse_iterator;
  typedef typename CIL::const_reverse_iterator const_reverse_iterator;

	typedef typename CSL::size_type              size_type;
  typedef typename CSL::difference_type        difference_type;

protected:

	typedef rac_facade<DERIVED, T, CIL, CTL, CSL> rac_facade_type;

private:

	DERIVED         & Derived()       throw();
	DERIVED   const & Derived() const throw();
	rac_facade_type & Mutable() const throw();


	// CRTP implementation

	iterator  AGetIterator (difference_type) throw();
	reference AGetReference(difference_type) throw();

	size_type AGetSize     ()                const throw();
	size_type AGetMaxSize  ()                const throw();

	// <

public:

  iterator               begin     ()                      throw();
	iterator               end       ()                      throw();
  reverse_iterator       rbegin    ()                      throw();
  reverse_iterator       rend      ()                      throw();

	const_iterator         begin     ()                const throw();
	const_iterator         end       ()                const throw();
	const_reverse_iterator rbegin    ()                const throw();
	const_reverse_iterator rend      ()                const throw();

	size_type              max_size  ()                const throw();
  size_type              size      ()                const throw();
	bool                   empty     ()                const throw();

	reference              front     ()                      throw();
	reference              back      ()                      throw();
  reference              at        (difference_type) ;
  reference              operator[](difference_type)       throw();

	const_reference        front     ()                const throw();
	const_reference        back      ()                const throw();
	const_reference        at        (difference_type) const;
	const_reference        operator[](difference_type) const throw();

	 // C++11

	const_iterator         cbegin     ()               const throw();
	const_iterator         cend       ()               const throw();
	const_reverse_iterator crbegin    ()               const throw();
	const_reverse_iterator crend      ()               const throw();

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template
<

	typename DERIVED ,
	typename CIL     ,
	typename SIZE

> struct mkrac_facade
{

	typedef CIL iterators;

	typedef typename iterators::iterator::value_type value_type;

	typedef container_types_list
	<

		value_type                               ,
		value_type *                             ,
		typename iterators::iterator::reference  ,
		value_type const *                       ,
		typename iterators::const_iterator::reference

	> types;

	typedef container_sizes_list
	<

		SIZE,
		typename iterators::iterator::difference_type

	> sizes;

	// ---

	typedef rac_facade<DERIVED, value_type, iterators, types, sizes> type;

};

//==============================================================================
} // namespace myStd

#endif // Header

