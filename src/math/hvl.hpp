#ifndef ECHMET_MATH_HVL_HPP
#define ECHMET_MATH_HVL_HPP

#ifndef _USE_MATH_DEFINES
        #define _USE_MATH_DEFINES
#endif

#ifdef ECHMET_MATH_HVL_PLATFORM_WIN
/* windows.h has to be included before tchar.h
 * in order for the ANSI vs. UNICODE detection
 * to work */
#include <windows.h>
#include <tchar.h>

typedef HMODULE hvlhandle_t;
typedef TCHAR *hvlstr_t;
typedef const TCHAR *chvlstr_t;
#define HVLFreeLibrary(HANDLE) FreeLibrary(HANDLE)
#define HVLGetProcAddress(HANDLE, SYMBOL) GetProcAddress(HANDLE, SYMBOL)
#define HVLLoadLibrary(FILENAME) LoadLibrary(FILENAME)

#elif defined ECHMET_MATH_HVL_PLATFORM_UNIX
#define _GNU_SOURCE 1
#include <pthread.h>
#include <sys/sysinfo.h>
#include <dlfcn.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>

typedef void *hvlhandle_t;
typedef char *hvlstr_t;
typedef const char *chvlstr_t;
#define HVLFreeLibrary(HANDLE) dlclose(HANDLE)
#define HVLGetProcAddress(HANDLE, SYMBOL) dlsym(HANDLE, SYMBOL)
#define HVLLoadLibrary(FILENAME) dlopen(FILENAME, RTLD_NOW)

#else
        #error "No platform type has been specified"
#endif

#include <stdexcept>

//=============================================================================
// SETUP

#ifdef ECHMET_MATH_HVL_PLATFORM_WIN
#ifdef UNICODE
        #define ECHMET_MATH_HVL_DLL         _T(L"hvl_mt.dll")
#else
        #define ECHMET_MATH_HVL_DLL         _T("hvl_mt.dll")
#endif
        #define ECHMET_MATH_HVL_DLLCALL     __cdecl
#elif defined ECHMET_MATH_HVL_PLATFORM_UNIX
        #define ECHMET_MATH_HVL_DLL	    "./hvl_mt.so"
        #define ECHMET_MATH_HVL_DLLCALL	     __attribute__((__cdecl__))
#else
        #error "No platform type has been specified!"
#endif // HVL_PLATFORM_
#define ECHMET_MATH_HVL_DLLBUILD            0                            // 1

//-----------------------------------------------------------------------------
// 1 : value | meaning
//       0   | do not build .cpp file (.hpp only)
//       1   | build as dll
//      -1   | build as module (.cpp required in the project)
//
// In the -1 mode, the HVL_dll class interface is preserved but it links against
// global functions from impl namespace rather than a dynamic .dll library

// ============================================================================
// INCLUDES

/* Forward declarations for PIMPL */
struct HVLContext;

namespace echmet {
        namespace math {
                struct HVLInternalValues;

                #if ECHMET_MATH_HVL_DLLBUILD == -1
                namespace impl {
                        /* Forward declarations for (sort of) PIMPL */

                        extern double ECHMET_MATH_HVL_DLLCALL HVL(const HVLContext *, double, double, double, double, double);
                        extern double ECHMET_MATH_HVL_DLLCALL HVLdx(const HVLContext *, double, double, double, double, double);
                        extern double ECHMET_MATH_HVL_DLLCALL HVLda0(const HVLContext *, double, double, double, double, double);
                        extern double ECHMET_MATH_HVL_DLLCALL HVLda1(const HVLContext *, double, double, double, double, double);
                        extern double ECHMET_MATH_HVL_DLLCALL HVLda2(const HVLContext *, double, double, double, double, double);
                        extern double ECHMET_MATH_HVL_DLLCALL HVLda3(const HVLContext *, double, double, double, double, double);

                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVL_range(const HVLContext *, double, double, double, double, double, double, double);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLdx_range(const HVLContext *, double, double, double, double, double, double, double);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLda0_range(const HVLContext *, double, double, double, double, double, double, double);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLda1_range(const HVLContext *, double, double, double, double, double, double, double);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLda2_range(const HVLContext *, double, double, double, double, double, double, double);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLda3_range(const HVLContext *, double, double, double, double, double, double, double);

                        extern bool ECHMET_MATH_HVL_DLLCALL HVLSetPrec(HVLContext **, const int prec);
                        extern void ECHMET_MATH_HVL_DLLCALL HVLFree(HVLInternalValues *ptr);
                        extern void ECHMET_MATH_HVL_DLLCALL HVLFreeContext(HVLContext *ctx);
                        extern HVLInternalValues * ECHMET_MATH_HVL_DLLCALL HVLAlloc(const size_t count);
                } // impl
                #endif // ECHMET_MATH_HVL_DLLBUILD

#include "hvl_types.hpp"

class HVL_dll {
private:
        /*
         * Include internal types privately.
         * User is not meant to access them directly,
         * Wrapping HVLData object is provided instead
         */
        //#include "hvl_types.hpp"

        bool loaded;
        hvlhandle_t m_handle;
        HVLContext *m_ctx;

        typedef double(ECHMET_MATH_HVL_DLLCALL  *fpointer)(const HVLContext *, double, double, double, double, double);
        typedef HVLInternalValues *(ECHMET_MATH_HVL_DLLCALL *rfpointer)(const HVLContext *, double, double, double, double, double, double, double);
        typedef bool(ECHMET_MATH_HVL_DLLCALL *sppointer)(HVLContext **, const int);
        typedef void(ECHMET_MATH_HVL_DLLCALL *frpointer)(HVLInternalValues *);
        typedef void(ECHMET_MATH_HVL_DLLCALL *fctxpointer)(HVLContext *);
        typedef HVLInternalValues *(ECHMET_MATH_HVL_DLLCALL *alpointer)(const size_t count);

        fpointer AHVL;
        fpointer AHVLdx;
        fpointer AHVLda0;
        fpointer AHVLda1;
        fpointer AHVLda2;
        fpointer AHVLda3;

        rfpointer AHVL_range;
        rfpointer AHVLdx_range;
        rfpointer AHVLda0_range;
        rfpointer AHVLda1_range;
        rfpointer AHVLda2_range;
        rfpointer AHVLda3_range;

        sppointer AHVLSetPrec;
        frpointer AHVLFree;
        fctxpointer AHVLFreeContext;
        alpointer AHVLAlloc;

        void Clear()
        {
                loaded = false;
                m_handle = 0;

                AHVL = NULL;
                AHVLdx = NULL;
                AHVLda0 = NULL;
                AHVLda1 = NULL;
                AHVLda2 = NULL;
                AHVLda3 = NULL;

                AHVL_range = NULL;
                AHVLdx_range = NULL;
                AHVLda0_range = NULL;
                AHVLda1_range = NULL;
                AHVLda2_range = NULL;
                AHVLda3_range = NULL;

                AHVLSetPrec = NULL;
                AHVLFree = NULL;
                AHVLFreeContext = NULL;
                AHVLAlloc = NULL;
        }

        void SetErrorMessage(hvlstr_t *errormsg)
        {
         if (errormsg == NULL)
                 return;

        #ifdef ECHMET_MATH_HVL_PLATFORM_WIN
                DWORD ErrId = GetLastError();
                FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                              NULL, ErrId, 0, (hvlstr_t)errormsg, 0, NULL);
        #elif defined ECHMET_MATH_HVL_PLATFORM_UNIX
                *errormsg = static_cast<hvlstr_t>(malloc(256));
                if (*errormsg == NULL)
                        return;
                #if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
                        /* Use XSI strerror_r */
                        strerror_r(errno, *errormsg, 256);
                #else
                        /* Use GNU strerror_r */
                        char *tempstr = strerror_r(errno, *errormsg, 256);
                        /* Handle a case where we get a pointer to some statically allocated string */
                        memmove(*errormsg, tempstr, 256);
                #endif // XSI vs GNU strerror_r
        #endif // ECHMET_MATH_HVL_PLATFORM_
        }

        HVL_dll(HVL_dll const &);
        void operator=(HVL_dll const &);

public:
        class HVLData
        {
        public:
                HVLData(HVLInternalValues *values, frpointer freefunc, alpointer allocfunc);
                HVLData(const HVLData& other);
                HVLData();
                ~HVLData();

                size_t count() const;
                double& x(const size_t idx);
                double& y(const size_t idx);

                HVLData& operator=(const HVLData& other);

        private:
                HVLInternalValues *m_values;
                frpointer m_freefunc;
                alpointer m_allocfunc;

                static void CopyContent(HVLData& to, const HVLData& from);
        };

        HVL_dll()
        {
                Clear();
                m_ctx = NULL;
        }

        HVL_dll(chvlstr_t filename)
        {
                Clear();
                if (!filename) return;
                if (!Open(filename)) std::runtime_error("error opening dynamic library");
                if (!Load())         std::runtime_error("error loading dynamic library");
        }

        ~HVL_dll()
        {
                if(IsLoaded()) {
                        AHVLFreeContext(m_ctx);
                        Close();
                }
        }

        void Close()
        {
        #if ECHMET_MATH_HVL_DLLBUILD != -1
                if (IsOpened()) HVLFreeLibrary(m_handle);
                Clear();
        #endif
        }

        void FreeErrorMessage(hvlstr_t errormsg)
        {
        #ifdef ECHMET_MATH_HVL_PLATFORM_WIN
                LocalFree(errormsg);
        #elif defined ECHMET_MATH_HVL_PLATFORM_UNIX
                free(errormsg);
        #endif
        }

        bool Load()
        {
                if (!IsOpened()) return false;
                if (IsLoaded()) return true;
                loaded = true;

        #if ECHMET_MATH_HVL_DLLBUILD == -1
                AHVL    = impl::HVL;
                AHVLdx  = impl::HVLdx;
                AHVLda0 = impl::HVLda0;
                AHVLda1 = impl::HVLda1;
                AHVLda2 = impl::HVLda2;
                AHVLda3 = impl::HVLda3;

                AHVL_range    = impl::HVL_range;
                AHVLdx_range  = impl::HVLdx_range;
                AHVLda0_range = impl::HVLda0_range;
                AHVLda1_range = impl::HVLda1_range;
                AHVLda2_range = impl::HVLda2_range;
                AHVLda3_range = impl::HVLda3_range;

                AHVLSetPrec = impl::HVLSetPrec;
                AHVLFree = impl::HVLFree;
                AHVLFreeContext = impl::HVLFreeContext;
                AHVLAlloc = impl::HVLAlloc;

        #else
                AHVL = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVL"));
                if (!AHVL) {
                        loaded = false;
                        return false;
                }

                AHVLdx = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVLdx"));
                if (!AHVLdx){
                        loaded = false;
                        return false;
                }

                AHVLda0 = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVLda0"));
                if (!AHVLda0) {
                        loaded = false;
                        return false;
                }
                AHVLda1 = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVLda1"));
                if (!AHVLda1) {
                        loaded = false;
                        return false;
                }

                AHVLda2 = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVLda2"));
                if (!AHVLda2) {
                        loaded = false;
                        return false;
                }

                AHVLda3 = reinterpret_cast<fpointer>(HVLGetProcAddress(m_handle, "HVLda3"));
                if (!AHVLda3) {
                        loaded = false;
                        return false;
                }

                AHVL_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVL_range"));
                if (!AHVL_range) {
                        loaded = false;
                        return false;
                }

                AHVLdx_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVLdx_range"));
                if (!AHVLdx_range) {
                        loaded = false;
                        return false;
                }

                AHVLda0_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVLda0_range"));
                if (!AHVLda0_range) {
                        loaded = false;
                        return false;
                }

                AHVLda1_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVLda1_range"));
                if (!AHVLda1_range) {
                        loaded = false;
                        return false;
                }

                AHVLda2_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVLda2_range"));
                if (!AHVLda2_range) {
                        loaded = false;
                        return false;
                }

                AHVLda3_range = reinterpret_cast<rfpointer>(HVLGetProcAddress(m_handle, "HVLda3_range"));
                if (!AHVLda3_range) {
                        loaded = false;
                        return false;
                }

                AHVLSetPrec = reinterpret_cast<sppointer>(HVLGetProcAddress(m_handle, "HVLSetPrec"));
                if (!AHVLSetPrec) {
                        loaded = false;
                        return false;
                }

                AHVLFree = reinterpret_cast<frpointer>(HVLGetProcAddress(m_handle, "HVLFree"));
                if (!AHVLFree) {
                        loaded = false;
                        return false;
                }

                AHVLFreeContext = reinterpret_cast<fctxpointer>(HVLGetProcAddress(m_handle, "HVLFreeContext"));
                if (!AHVLFreeContext) {
                        loaded = false;
                        return false;
                }

                AHVLAlloc = reinterpret_cast<alpointer>(HVLGetProcAddress(m_handle, "HVLAlloc"));
                if (!AHVLAlloc) {
                        loaded = false;
                        return false;
                }

        #endif
                return AHVLSetPrec(&m_ctx, 16); /* 16 significant digits should correspond to 53 bits of MPFR's default precision */
        }

        bool Open(chvlstr_t filename, hvlstr_t *errormsg = static_cast<hvlstr_t *>(NULL))
        {
                if (IsOpened()) return false;

        #if ECHMET_MATH_HVL_DLLBUILD == -1
                (void)(filename);
                return true;
        #else
                m_handle = HVLLoadLibrary(filename);
                if (m_handle == NULL) {
                                SetErrorMessage(errormsg);
                                return false;
                }
        #endif // ECHMET_MATH_HVL_DLLBUILD

                return true;
        }

        bool IsOpened() const
        {
        #if ECHMET_MATH_HVL_DLLBUILD == -1
                return true;
        #else
                return m_handle ? true : false;
        #endif
        }
        bool IsLoaded() const { return loaded; }

        double HVL(double x, double a0, double a1, double a2, double a3) const { return AHVL(m_ctx, x, a0, a1, a2, a3); }
        double HVLdx(double x, double a0, double a1, double a2, double a3) const { return AHVLdx(m_ctx, x, a0, a1, a2, a3); }
        double HVLda0(double x, double a0, double a1, double a2, double a3) const { return AHVLda0(m_ctx, x, a0, a1, a2, a3); }
        double HVLda1(double x, double a0, double a1, double a2, double a3) const { return AHVLda1(m_ctx, x, a0, a1, a2, a3); }
        double HVLda2(double x, double a0, double a1, double a2, double a3) const { return AHVLda2(m_ctx, x, a0, a1, a2, a3); }
        double HVLda3(double x, double a0, double a1, double a2, double a3) const { return AHVLda3(m_ctx, x, a0, a1, a2, a3); }

        HVLData HVL_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVL_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }
        HVLData HVLdx_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVLdx_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }
        HVLData HVLda0_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVLda0_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }
        HVLData HVLda1_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVLda1_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }
        HVLData HVLda2_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVLda2_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }
        HVLData HVLda3_range(double from, double to, double step, double a0, double a1, double a2, double a3) const
        {
                return HVLData(AHVLda2_range(m_ctx, from, to, step, a0, a1, a2, a3), AHVLFree, AHVLAlloc);
        }


        bool HVLSetPrec(const int digits) { return AHVLSetPrec(&m_ctx, digits); }

};
        } // math
} // echmet

#endif // ECHMET_MATH_HVL_HPP
