#ifndef MYSTD_RAC_FACADE_HPP
#define MYSTD_RAC_FACADE_HPP
//==============================================================================
// COMMENTS
//

//==============================================================================
// INCLUDES

#include <stdexcept> // out_of_range

#include "rac_facade.h"

//==============================================================================
// DEFINES

#define MYSTD_RAC_FACADE_T       template<typename DERIVED, typename T, typename CIL, typename CTL, typename CSL>
#define MYSTD_RAC_FACADE         rac_facade<DERIVED, T, CIL, CTL, CSL>

//=============================================================================
// NAMESPACE

namespace myStd {

//=============================================================================
// CODE

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
DERIVED &
MYSTD_RAC_FACADE::Derived() throw()
{

	return *static_cast<DERIVED *>(this);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
DERIVED const &
MYSTD_RAC_FACADE::Derived() const throw()
{

	return *static_cast<const DERIVED *>(this);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
MYSTD_RAC_FACADE &
MYSTD_RAC_FACADE::Mutable() const throw()
{

	return *const_cast<MYSTD_RAC_FACADE *>(this);

}

//-----------------------------------------------------------------------------
// CRTP Implementation
//-----------------------------------------------------------------------------

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::iterator
MYSTD_RAC_FACADE::AGetIterator(difference_type i) throw()
{

	return Derived().AGetIterator(i);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reference
MYSTD_RAC_FACADE::AGetReference(difference_type i) throw()
{

	return Derived().AGetReference(i);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::size_type
MYSTD_RAC_FACADE::AGetSize() const throw()
{

	return Derived().AGetSize();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::size_type
MYSTD_RAC_FACADE::AGetMaxSize() const throw()
{

	return Derived().AGetMaxSize();

}

//-----------------------------------------------------------------------------
// Random Access Container Interface
//-----------------------------------------------------------------------------

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::iterator
MYSTD_RAC_FACADE::begin() throw()
{

	return AGetIterator(0);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::iterator
MYSTD_RAC_FACADE::end() throw()
{

	return AGetIterator(size());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reverse_iterator
MYSTD_RAC_FACADE::rbegin() throw()
{

	return reverse_iterator(end());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reverse_iterator
MYSTD_RAC_FACADE::rend() throw()
{

	return reverse_iterator(begin());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_iterator
MYSTD_RAC_FACADE::begin() const throw()
{

	return const_iterator(Mutable().begin());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_iterator
MYSTD_RAC_FACADE::end() const throw()
{

	return const_iterator(Mutable().end());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reverse_iterator
MYSTD_RAC_FACADE::rbegin() const throw()
{

	return const_reverse_iterator(Mutable().rbegin());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reverse_iterator
MYSTD_RAC_FACADE::rend() const throw()
{

	return const_reverse_iterator(Mutable().rend());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::size_type
MYSTD_RAC_FACADE::max_size() const throw()
{

	return AGetMaxSize();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::size_type
MYSTD_RAC_FACADE::size() const throw()
{

	return AGetSize();

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
bool
MYSTD_RAC_FACADE::empty() const throw()
{

	return size() == 0;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reference
MYSTD_RAC_FACADE::front() throw()
{

	return operator[](0);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reference
MYSTD_RAC_FACADE::back() throw()
{

	return operator[](size() - 1);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reference
MYSTD_RAC_FACADE::at(difference_type i)
{

	if (i >= size()) throw std::out_of_range("");

	return operator[](i);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::reference
MYSTD_RAC_FACADE::operator[](difference_type i) throw()
{

	return AGetReference(i);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reference
MYSTD_RAC_FACADE::front() const throw()
{

	return const_reference(Mutable().front());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reference
MYSTD_RAC_FACADE::back() const throw()
{

	return const_reference(Mutable().back());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reference
MYSTD_RAC_FACADE::at(difference_type i) const
{

	return const_reference(Mutable().at(i));

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reference
MYSTD_RAC_FACADE::operator[](difference_type i) const throw()
{

	return const_reference(Mutable().operator[](i));

}

//-----------------------------------------------------------------------------
// C++ 11
//-----------------------------------------------------------------------------

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_iterator
MYSTD_RAC_FACADE::cbegin() const throw()
{

	return const_iterator(Mutable().begin());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_iterator
MYSTD_RAC_FACADE::cend() const throw()
{

	return const_iterator(Mutable().end());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reverse_iterator
MYSTD_RAC_FACADE::crbegin() const throw()
{

	return const_reverse_iterator(Mutable().rbegin());

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MYSTD_RAC_FACADE_T
typename MYSTD_RAC_FACADE::const_reverse_iterator
MYSTD_RAC_FACADE::crend() const throw()
{

	return const_reverse_iterator(Mutable().rend());

}


//=============================================================================
} // namespace

#undef MYSTD_RAC_FACADE_T
#undef MYSTD_RAC_FACADE


#endif // Header

