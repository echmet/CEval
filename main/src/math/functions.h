#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//=============================================================================
// DEFS

#define HVLDEBUG  false
#define HVLFACTOR std::exp(a3 / 2)

//=============================================================================
// INLCUDES

#include <utility>
#include <limits>
#include "bool_counter.h"

#define FUNCTIONS_NaN      (std::numeric_limits<double>::quiet_NaN())
#define FUNCTIONS_Infinity (std::numeric_limits<double>::infinity())


//=============================================================================
// CODE

//---------------------------------------------------------------------------

namespace myStd {

inline double Exp(double x)
{
	return x > 500 ? FUNCTIONS_Infinity : ( std::fabs(x) < 1E-16 ? 1 : std::exp(x));
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class function_base {
private:
	myStd::bool_counter<> pcounter;

public:
	inline static function_base * Register(function_base * p) { if (p) ++(p->pcounter); return p;}
	inline static function_base * Release(function_base * p)  { if ( p && !(--(p->pcounter)) ) delete p; return NULL; }

	virtual ~function_base() {}

	virtual double          f(double)   = 0;
	virtual function_base * der()       = 0;
};

//---------------------------------------------------------------------------
class function {
private:
	function_base         * fce;

	inline void assign(const function & other) {
		if (fce == other.fce) return;
		function_base::Release(fce);
		fce = function_base::Register(other.fce);
	}

public:

	inline function(function_base * _fce = NULL)   {	fce = function_base::Register(_fce); }

	inline function(const function & other) : fce(NULL) {	assign(other); }
	inline ~function()                      { function_base::Release(fce); }

	inline function & operator=(const function & other) {	assign(other);	return * this;	}

	inline double operator()(double x)     const { return fce->f(x);	}
	inline function der()                  const { return function(fce->der()); }

};

//---------------------------------------------------------------------------
class TGauss : public function_base {
private:
	double a0, u, s;

	TGauss(const TGauss & other);
	void operator=(const TGauss & other);

public:

	inline TGauss(double _a0 = 1, double _u = 0, double _s = 1)
	: a0(_a0), u(_u), s(_s)
	{}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TGaussD1 : public function_base {
private:
	double a0, u, s;

	TGaussD1(const TGaussD1 & other);
	void operator=(const TGaussD1 & other) ;

public:

	inline TGaussD1(double _a0 = 1, double _u = 0, double _s = 1)
	: a0(_a0), u(_u), s(_s)
	{}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TGaussD2 : public function_base {
private:
	double a0, u, s;

	TGaussD2(const TGaussD2 & other);
	void operator=(const TGaussD2 & other) ;

public:

	inline TGaussD2(double _a0 = 1, double _u = 0, double _s = 1)
	: a0(_a0), u(_u), s(_s)
	{}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class THVL : public TGauss {
private:
	double a0, a1, a2, a3;

	THVL(const THVL & other);
	void operator=(const THVL & other);


public:

	inline THVL(double _a0 = 1, double _a1 = 0, double _a2 = 1, double _a3 = 0)
	: TGauss(_a0, _a1, _a2), a0(_a0), a1(_a1), a2(_a2), a3(_a3)
	{}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TDER : public function_base {
private:
	function fce;
	double dx;


	TDER(const TDER & other);
	void operator=(const TDER & other);

public:

        inline TDER(function _fce, double _dx = 1E-3) : fce(_fce), dx(_dx) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TDELTA : public function_base {
private:
	function fce1;
	function fce2;

	TDELTA(const TDELTA & other);
	void operator=(const TDELTA & other);

public:

	inline TDELTA(function _fce1, function _fce2) : fce1(_fce1), fce2(_fce2) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TMIN : public function_base {
private:
	function fce1;
	function fce2;

	TMIN(const TMIN & other);
	void operator=(const TMIN & other);

public:

	inline TMIN(function _fce1, function _fce2) : fce1(_fce1), fce2(_fce2) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TMOMENT : public function_base {
private:
	function fce;
	int k;	double c;

	TMOMENT(const TMOMENT & other);
	void operator=(const TMOMENT & other);

public:

	inline TMOMENT(function _fce, int _k, double _c = 0) : fce(_fce), k(_k), c(_c) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TTIMES : public function_base {
private:
	function fce1;
	function fce2;

	TTIMES(const TTIMES & other);
	void operator=(const TTIMES & other);

public:

	inline TTIMES(function _fce1, function _fce2) : fce1(_fce1), fce2(_fce2) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
class TPOW : public function_base {
private:
	function fce;
	double   pow;

	TPOW(const TPOW & other);
	void operator=(const TPOW & other);

public:

	inline TPOW(function _fce, double _pow) : fce(_fce), pow(_pow) {}

	virtual double          f(double);
	virtual function_base * der();

};

//---------------------------------------------------------------------------
extern ::std::pair<double, double>
solve(const function & f, double x1, double x2, double c = 0, double eps = 1E-6, int max = 100);

extern double
integral(const function & f, double x1, double x2, double dx);


//=============================================================================
} // namespace

#endif // FUNCTIONS_H
