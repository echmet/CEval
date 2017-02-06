// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0

#ifndef CRASHHANDLERLINUX_STACKTRACE_H
#define CRASHHANDLERLINUX_STACKTRACE_H

#ifdef CRASHHANDLING_LINUX

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <QStringList>

/** Print a demangled stack backtrace of the caller function to FILE* out. */
static inline QString getBacktrace(unsigned int max_frames = 63)
{
    QString btrace;

    // storage array for stack trace address data
    void** addrlist = (void**)calloc(sizeof(void*), max_frames+1);
    memset(addrlist, 0, max_frames+1);

    if (addrlist == nullptr)
        return btrace;

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, max_frames+1);

    if (addrlen == 0) {
        free(addrlist);
        return QString("  <empty, possibly corrupt>\n");
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 2; i < addrlen; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        //fprintf(out, "%s TT\n", symbollist[i]);
        for (char *p = symbollist[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
            }
        }

        if (begin_name && begin_offset && end_offset
            && begin_name < begin_offset)
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
                                            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                btrace.append(QString("%1 : %2+%3 %4\n").arg(QString::fromLatin1(symbollist[i]))
                                                      .arg(QString::fromLatin1(funcname))
                                                      .arg(QString::fromLatin1(begin_offset))
                                                      .arg(QString::fromLatin1(++end_offset)));
            }
            else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                btrace.append(QString("%1 : %2+%3 %4\n").arg(QString::fromLatin1(symbollist[i]))
                                                      .arg(QString::fromLatin1(funcname))
                                                      .arg(QString::fromLatin1(begin_offset))
                                                      .arg(QString::fromLatin1(++end_offset)));
            }
        }
        else
        {
            // couldn't parse the line? print the whole line.
            btrace.append(QString::fromLatin1(symbollist[i]) + "\n");
        }
    }

    free(funcname);
    free(symbollist);
    free(addrlist);

    return btrace;
}

#endif // CRASHHANDLING_LINUX

#endif // CRASHHANDLERLINUX_STACKTRACE_H
