#ifndef ECHMET_MATRIX_BASIC_DCLH
#define ECHMET_MATRIX_BASIC_DCLH
//=============================================================================
// COMMENTS

//=============================================================================
// INCLUDES

// ============================================================================
// SETTINGS

// ============================================================================
// DEFINES

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//=============================================================================
// CODE

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// matrix size types

typedef std::size_t    msize_t;
typedef std::ptrdiff_t mptrdiff_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// copy tags

class soft_copy    {};
class raw_copy     {};
class steal_copy   {};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// matrix state tags

enum transtags { trRows    = false, trColumns   = true };

enum invtags   { ivNormal = false, inInversed = true };

enum datatags  {

  dtUnique   = 0x00,
  dtShared   = 0x01,
  dtTemp     = 0x03,

  dtTempMask = 0x02

};

//=============================================================================
}} // namespace echmet :: matrix

#endif // Header
