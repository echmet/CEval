#ifndef MYSTD_EXT_H
#define MYSTD_EXT_H

//===========================================================================
//---------------------------------------------------------------------------

// Settings

#define MYSTD_DEFINE_NANINF      true
#define MYSTD_MAX_EXP_ARGUMENT   500


//===========================================================================
//---------------------------------------------------------------------------

// Defines

#if MYSTD_DEFINE_NANINF

	//http://qc.embarcadero.com/wc/qcmain.aspx?d=108800
	#include <limits>
	#define MYSTD_NaN (std::numeric_limits<double>::quiet_NaN())
	#define MYSTD_Infinity (std::numeric_limits<double>::infinity())

#endif

//---------------------------------------------------------------------------
#endif // Header
