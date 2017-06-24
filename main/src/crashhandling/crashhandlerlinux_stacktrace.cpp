#ifdef CRASHHANDLING_LINUX
#include "crashhandlerlinux_stacktrace.h"

#include <cstdio>
#include <cstdlib>
#include <execinfo.h>
#include <cxxabi.h>
#include <signal.h>
#include "rawmemblock.h"

void st_memcpy(void *dest, const void *src, const size_t size)
{
  char *_dest = static_cast<char *>(dest);
  const char *_src = static_cast<const char *>(src);

  for (size_t idx = 0; idx < size; idx++)
    _dest[idx] = _src[idx];
}

size_t st_strlen(const char *str)
{
  size_t len = 0;

  while (str[len] != '\0')
    len++;

  return len;
}


size_t st_strcpy(char *dest, const char *str)
{
  const size_t len = st_strlen(str);

  if (len == 0)
    return 0;

  st_memcpy(dest, str, len);
  dest[len] = '\0';

  return len;
}

void st_zeromem(void *dest, const char v, const size_t size)
{
  char * _dest = static_cast<char *>(dest);

  for (size_t idx = 0; idx < size; idx++)
    _dest[idx] = v;
}

void signumToString(char *dest, const int signum)
{
  const char *signame;

  switch (signum) {
  case SIGSEGV:
    signame = "Segmentation fault\n";
    break;
  case SIGABRT:
    signame = "Aborted\n";
    break;
  case SIGFPE:
    signame = "Floating point exception\n";
    break;
  case SIGILL:
    signame = "Illegal instruction\n";
    break;
  case SIGBUS:
    signame = "Bus error\n";
    break;
  case SIGTRAP:
    signame = "Trap\n";
    break;
  default:
    signame = "Unknown\n";
  }

  st_strcpy(dest, signame);
}

const size_t LinuxStackTracer::MAX_FRAMES(63);
const size_t LinuxStackTracer::MAX_LINE_LENGTH(512);

#define MAX_FUNCNAME_LENGTH 256
#define ADVANCE_OUTPTR(f, b, tb) { tb += b; if (tb >= MAX_LINE_LENGTH - 1) break; f += b; }

bool LinuxStackTracer::getBacktrace(RawMemBlock<char> &outbuf, size_t &backtraceLines, const int signum)
{
  RawMemBlock<void *> addrList(sizeof(void *) * (MAX_FRAMES + 1));

  if (!outbuf.allocate((MAX_FRAMES + 1) * MAX_LINE_LENGTH))
    return false;

  const size_t addrlen = backtrace(addrList.mem(), MAX_FRAMES + 1);

  /* Empty or corrupted address list */
  if (addrlen == 0)
    return true;

  /* TODO: Pass the backtrace to file descriptor to avoid malloc()ing would be a lot better */
  char **symbolList = backtrace_symbols(addrList.mem(), addrlen);

  signumToString(outbuf.mem(), signum);

  /* Skip the top of the backtrace */
  for (size_t idx = 0; idx < addrlen; idx++) {
    char *outstr = outbuf.mem() + ((idx + 1) * MAX_LINE_LENGTH);
    char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

    for (char *p = symbolList[idx]; *p; ++p) {
      if (*p == '(')
        begin_name = p;
      else if (*p == '+')
        begin_offset = p;
      else if (*p == ')' && begin_offset) {
        end_offset = p;
        break;
      }
    }

    if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
      *begin_name++ = '\0';
      *begin_offset++ = '\0';
      *end_offset = '\0';

      RawMemBlock<char> funcName(MAX_FUNCNAME_LENGTH);
      size_t funcNameLen = MAX_FUNCNAME_LENGTH;

      int status;
      /* This is potentially dangerous since the function may try to realloc()
       * an mmap()'ed region of memory if it's not large enough. */
      char *ret = abi::__cxa_demangle(begin_name, funcName.mem(), &funcNameLen, &status);
      (void)ret;
      size_t totalBytes = 0;
      size_t bytes;
      if (status == 0) {
        bytes = st_strcpy(outstr, symbolList[idx]); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " : "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, funcName.mem()); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, "+"); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, begin_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, ++end_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        st_strcpy(outstr, "\n");
      } else {
        // demangling failed. Output function name as a C function with
        // no arguments.
        bytes = st_strcpy(outstr, symbolList[idx]); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " : "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, begin_name); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, "()+"); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, begin_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, ++end_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        st_strcpy(outstr, "\n");
      }
    } else {
      /* The line could not have been parsed so use the whole line as-is */
      if (st_strlen(symbolList[idx]) > 510) {
        st_memcpy(outstr, symbolList[idx], 510);
        outstr[510] = '\n';
        outstr[511] = '\0';
      } else {
        size_t bytes = st_strcpy(outstr, symbolList[idx]);
        st_strcpy(outstr + bytes, "\n");
      }
    }
  }

  backtraceLines = addrlen;
  return true;
}

#endif // CRASHHANDLING_LINUX
