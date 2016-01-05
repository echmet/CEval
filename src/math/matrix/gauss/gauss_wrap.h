#ifndef ECHMET_MATRIX_GAUSS_WRAPH
#define ECHMET_MATRIX_GAUSS_WRAPH

//=============================================================================
// INCLUDES

#include "../fwdcl.h"

#include "fwdcl.h"

//=============================================================================
// NAMESPACE

namespace echmet { namespace matrix {

//-----------------------------------------------------------------------------
struct gswtNull
{

        template<typename T> struct Wrapper {

                typedef Wrapper<T> type;
                typedef type       ref;

                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                struct Row
                {

                        struct aptr {

                                void operator++(){}
                                void operator--(){}
                                bool operator==(aptr){ return true; }
                                bool operator!=(aptr){ return false; }

                        } ptr;

                        void swap          (Row)            throw() {}

                        void LinCombination(Row, T const &) throw() {}

                        void operator*=    (T const &)      throw() {}

                };

                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                Wrapper       (void *)       throw() {}

                Row Rows      (msize_t = 0)  throw() { return Row(); }
                Row RowsEnd   ()             throw() { return Row(); }

        };

};

//-----------------------------------------------------------------------------
template<transtags DTT>
struct gswtReference {

        template<typename T> struct Wrapper {

                typedef Core<T, DTT>   type;
                typedef type         & ref;

        };

};

//=============================================================================
}} // namesapce echmet :: matrix

#endif // Header
