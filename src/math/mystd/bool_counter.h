#ifndef MYSTD_BOOL_COUNTER_H
#define MYSTD_BOOL_COUNTER_H

namespace myStd {

//---------------------------------------------------------------------------
template<typename T = unsigned int> class bool_counter {
public:
  typedef T value_type;

private:
  T counter;
 
  bool_counter(const bool_counter &);                                     // $1
  void operator=(const bool_counter &);                                   // $1

public:
	bool_counter(T _counter = 0) : counter(_counter) {}

	bool_counter & operator++() {++counter; return *this;}
	bool_counter & operator--() {--counter; return *this;}
	operator bool() { return counter != T(0); }
	bool operator!(){ return counter == T(0); }

	bool operator==(T t) { return counter == t; }
	bool operator!=(T t) { return counter != t; }

	T value() {return counter;}
};

//---------------------------------------------------------------------------
template<class T = unsigned int> class counter_wrapper {
public:
	typedef T value_type;

private:
	bool_counter<T> * counter;

	counter_wrapper();
	counter_wrapper(const counter_wrapper<T> &);
	void operator=(const counter_wrapper<T> &);

public:
	counter_wrapper(bool_counter<T> * _counter) : counter(_counter) {++(*counter);}
	~counter_wrapper(){--(*counter);}

	const T & value() const {return *counter;}

};

} // namespace


#endif //header
