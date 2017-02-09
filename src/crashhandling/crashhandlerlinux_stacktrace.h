// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0

#ifndef CRASHHANDLERLINUX_STACKTRACE_H
#define CRASHHANDLERLINUX_STACKTRACE_H

#ifdef CRASHHANDLING_LINUX

#include <cstddef>
#include "rawmemblock.h"

class LinuxStackTracer
{
public:
  static bool getBacktrace(RawMemBlock<char> &outbuf, size_t &backtraceLines, const int signum);

  static const size_t MAX_LINE_LENGTH;
  static const size_t MAX_FRAMES;

};

#endif // CRASHHANDLING_LINUX

#endif // CRASHHANDLERLINUX_STACKTRACE_H
