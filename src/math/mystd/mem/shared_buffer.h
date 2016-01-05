#ifndef MYSTD_SHARED_BUFFER_H
#define MYSTD_SHARED_BUFFER_H

//===========================================================================
//---------------------------------------------------------------------------

// Includes:

#include "_SHARED\MyStd\bool_counter.h"
#include "_SHARED\MyStd\mem\base.h"

//===========================================================================
//---------------------------------------------------------------------------

// Declarations:

namespace myStd {

namespace mem {

//---------------------------------------------------------------------------
template<typename T, typename Allocator = std::allocator<T> >
class shared_buffer {
public:
	typedef Allocator                           allocator_type;
	typedef typename Allocator::value_type      value_type;
	typedef typename Allocator::pointer         pointer_type;
	typedef typename Allocator::reference       reference_type;
	typedef typename Allocator::const_pointer   const_pointer_type;
	typedef typename Allocator::const_reference const_reference_type;
	typedef typename Allocator::size_type       size_type;
	typedef typename Allocator::difference_type difference_type;

	typedef shared_buffer<T, Allocator> self_type;

private:
  allocator_type a;
  pointer_type   base;
  pointer_type   end;
	mutable pointer_type   p;

  bool_counter<> * counter;

	void _Destroy();

public:

	explicit shared_buffer(size_type n = 0, Allocator _a = Allocator());

	shared_buffer(size_type n, std::nothrow_t, Allocator _a = Allocator());
	shared_buffer(const self_type & other);

	shared_buffer(const self_type & other, size_type offset);

	~shared_buffer() { _Destroy(); }

	void swap(self_type & other);

	shared_buffer<T, Allocator> & operator=(const shared_buffer<T, Allocator> & other);

	self_type &       operator++()                  { ++p;    return *this; }
	const self_type & operator++() const            { ++p;    return *this; }
	self_type &       operator--()                  { --p;    return *this; }
	const self_type & operator--() const            { --p;    return *this; }
	self_type         operator++(int) const         { self_type tmp(*this); ++p; return tmp; }
	self_type         operator--(int) const         { self_type tmp(*this); --p; return tmp; }
	self_type &       operator+=(size_type i)       { p += i; return *this; }
	const self_type & operator+=(size_type i) const { p += i; return *this; }
	self_type &       operator-=(size_type i)       { p += i; return *this; }
	const self_type & operator-=(size_type i) const { p -= i; return *this; }

	const_reference_type          operator*()             const { return *p; }
	reference_type                operator*()                   { return *p; }

	const_reference_type          operator[](size_type i) const { return *(base + i); }
	reference_type                operator[](size_type i)       { return *(base + i); }

																operator bool()         const { return p >= base && p <  end; }
	bool                          operator !()            const { return p <  base || p >= end; }

	size_type          GetSize     () const { return end - base; }

	const_pointer_type GetBase     () const { return base; }
	const_pointer_type GetEnd      () const { return end; }
	const_pointer_type GetPosition () const { return p; }
	bool_counter<>::value_type
										 GetCount    () const { return counter ? counter->value() : 0; }

	self_type &       Reset()       { p = base; return * this; }
	const self_type & Reset() const { p = base; return * this; }

	self_type & Invalidate();
	self_type & Seclude();
	self_type & Copy(const self_type & other);

};

} // namespace

} // namespace

//===========================================================================
//---------------------------------------------------------------------------

namespace std {

template<typename T, typename Allocator>
void swap(myStd::mem::shared_buffer<T, Allocator> & op1, myStd::mem::shared_buffer<T, Allocator> & op2)
{ 
  op1.swap(op2); 
}

} 

//---------------------------------------------------------------------------
#endif // Header
