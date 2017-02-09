// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0

#ifndef CRASHHANDLERLINUX_STACKTRACE_H
#define CRASHHANDLERLINUX_STACKTRACE_H

#ifdef CRASHHANDLING_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <sys/syscall.h>
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


#define MAX_FRAMES 63
#define MAX_LINE_LENGTH 512
#define MAX_FUNCNAME_LENGTH 256
#define ADVANCE_OUTPTR(f, b, tb) { tb += b; if (tb >= MAX_LINE_LENGTH - 1) break; f += b; }

static bool getBacktrace(RawMemBlock<char> &outbuf, size_t &backtraceLines)
{
  RawMemBlock<void *> addrList(sizeof(void *) * (MAX_FRAMES + 1));

  if (!outbuf.allocate(MAX_FRAMES * MAX_LINE_LENGTH))
    return false;

  const size_t addrlen = backtrace(addrList.mem(), MAX_FRAMES + 1);

  /* Empty or corrupted address list */
  if (addrlen == 0)
    return true;

  /* TODO: Pass the backtrace to file descriptor to avoid malloc()ing would be a lot better */
  char **symbolList = backtrace_symbols(addrList.mem(), addrlen);

  /* Skip the top of the backtrace */
  for (size_t idx = 0; idx < addrlen; idx++) {
    char *outstr = outbuf.mem() + ((idx - 0) * MAX_LINE_LENGTH);
    char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

    for (char *p = symbolList[idx - 0]; *p; ++p) {
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
        bytes = st_strcpy(outstr, symbolList[idx - 0]); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
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
        bytes = st_strcpy(outstr, symbolList[idx - 0]); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " : "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, begin_name); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, "()+"); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, begin_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, " "); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        bytes = st_strcpy(outstr, ++end_offset); ADVANCE_OUTPTR(outstr, bytes, totalBytes);
        st_strcpy(outstr, "\n");
      }
    }
  }

  backtraceLines = addrlen;
  return true;
}

#endif // CRASHHANDLING_LINUX

#endif // CRASHHANDLERLINUX_STACKTRACE_H
