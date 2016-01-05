#ifndef MYSTD_SHARED_BUFFER_HPP
#define MYSTD_SHARED_BUFFER_HPP

//===========================================================================
//---------------------------------------------------------------------------

// Includes:

#include <stdexcept>

#include "_SHARED\MyStd\mem\mem.hpp"
#include "_SHARED\MyStd\bool_counter.hpp"

#include "shared_buffer.h"

//===========================================================================
//---------------------------------------------------------------------------

// Definitions:

namespace myStd {

namespace mem {

#define MYSTD_SHARED_BUFFER_NULL allocator_extensions<Allocator>::null_type()

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
void shared_buffer<T, Allocator>::_Destroy()
{
	if (!counter || (--(*counter))) return;

	Release<T, Allocator>(base, GetSize(), a);
	delete counter;

	base     = MYSTD_SHARED_BUFFER_NULL;
	end      = MYSTD_SHARED_BUFFER_NULL;
	p        = MYSTD_SHARED_BUFFER_NULL;

	counter  = NULL;
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator>::shared_buffer(size_type n, Allocator _a)
:
	a       (_a),
	base    (MYSTD_SHARED_BUFFER_NULL),
	end     (MYSTD_SHARED_BUFFER_NULL),
	p       (MYSTD_SHARED_BUFFER_NULL),
	counter (NULL)
{

	counter = new myStd::bool_counter<>;

	if (!n) goto END;

	base = Create<T, Allocator>(n, a);
	end  = base + n;
	p    = base;

	END:

	++(*counter);
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator>::shared_buffer(size_type n, std::nothrow_t, Allocator _a)
:
	base    (MYSTD_SHARED_BUFFER_NULL),
	base_end(MYSTD_SHARED_BUFFER_NULL),
	p       (MYSTD_SHARED_BUFFER_NULL),
	counter (NULL)
{

	counter = new(std::nothrow) myStd::bool_counter<>;

	if (!counter) return;

	base     = Create<T, Allocator>(std::nothrow, n, a);

	if (!base) goto END;

	base_end = base + n;
	p        = base;

	END:

	++(*counter);
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator>::shared_buffer(const shared_buffer<T, Allocator> & other)
:
	a       (other.a),
	base    (other.base),
	end     (other.end),
	p       (other.p),
	counter (other.counter)
{
	if (counter) ++(*counter);
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator>::shared_buffer(const shared_buffer<T, Allocator> & other, size_type offset)
:
	a       (other.a),
	base    (other.base),
	end     (other.end),
	p       (other.base + offset),
	counter (other.counter)
{
	if (counter) ++(*counter);
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
void shared_buffer<T, Allocator>::swap(shared_buffer<T, Allocator> & other)
{
  std::swap(p,       other.p);

  if (counter == other.counter) return;

  std::swap(a,       other.a);
  std::swap(base,    other.base);
  std::swap(end,     other.end);
  std::swap(counter, other.counter);
}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator> &
shared_buffer<T, Allocator>::operator=(const shared_buffer<T, Allocator> & other)
{
	_Destroy();

	a        = other.a;
	base     = other.base;
	end      = other.end;
	p        = other.p;
	counter  = other.counter;

	if (counter) ++(*counter);

	return * this;

}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator> &
shared_buffer<T, Allocator>::Invalidate()
{
	if (!counter || *counter == 1) { Reset(); return *this; }

	shared_buffer<T, Allocator> tmp(GetSize(), a);
	tmp.p     = tmp.base;
	operator=(tmp);

	return * this;

}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator> &
shared_buffer<T, Allocator>::Seclude()
{
	if (!counter || *counter == 1) { Reset(); return *this; }

	shared_buffer<T, Allocator> tmp(GetSize(), a);
	tmp.Copy(*this);
	operator=(tmp);

	return * this;

}

//---------------------------------------------------------------------------
inline template<typename T, typename Allocator>
shared_buffer<T, Allocator> &
shared_buffer<T, Allocator>::Copy(const shared_buffer<T, Allocator> & other)
{
	if (base == other.base && counter && *counter == 1) return * this;
	if (GetSize() != other.GetSize()) operator=(other);

	Invalidate();
	::myStd::mem::Copy<T, Allocator>(other.base, base, GetSize());

	return *this;

}
//---------------------------------------------------------------------------

#undef MYSTD_SHARED_BUFFER_NULL

} // namespace

} // namespace

//---------------------------------------------------------------------------
#endif // Header
