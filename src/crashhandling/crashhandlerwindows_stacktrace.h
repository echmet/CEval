/***************************************************************************
*   Copyright (C) 2005-09 by the Quassel Project                          *
*   devel@quassel-irc.org                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) version 3.                                           *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef CRASHHANDLERWINDOWS_STACKTRACE_H
#define CRASHHANDLERWINDOWS_STACKTRACE_H

#ifdef CRASHHANDLING_WIN32

#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#ifdef __MINGW32__
#include <cxxabi.h>
#endif

namespace straceWin
{
    void loadHelpStackFrame(IMAGEHLP_STACK_FRAME&, const STACKFRAME64&);
    BOOL CALLBACK EnumSymbolsCB(PSYMBOL_INFO, ULONG, PVOID);
    BOOL CALLBACK EnumModulesCB(LPCSTR, DWORD64, PVOID);
    const std::string getBacktrace(std::ostringstream &logStream, const DWORD threadId, const PCONTEXT exctx);
    struct EnumModulesContext;
    // Also works for MinGW64
#ifdef __MINGW32__
    void demangle(std::string& str);
#endif
}

#ifdef __MINGW32__
void straceWin::demangle(std::string& str)
{
    const std::string inStr = "_" + str; // Really need that underline or demangling will fail
    int status = 0;
    size_t outSz = 0;
    char* demangled_name = abi::__cxa_demangle(inStr.c_str(), 0, &outSz, &status);
    if (status == 0) {
        str = std::string(demangled_name);
        if (outSz > 0)
            free(demangled_name);
    }
}
#endif

void straceWin::loadHelpStackFrame(IMAGEHLP_STACK_FRAME& ihsf, const STACKFRAME64& stackFrame)
{
    ZeroMemory(&ihsf, sizeof(IMAGEHLP_STACK_FRAME));
    ihsf.InstructionOffset = stackFrame.AddrPC.Offset;
    ihsf.FrameOffset = stackFrame.AddrFrame.Offset;
}

BOOL CALLBACK straceWin::EnumSymbolsCB(PSYMBOL_INFO symInfo, ULONG size, PVOID user)
{
    (void)size;
    std::vector<std::string> *params = static_cast<std::vector<std::string> *>(user);
    if (symInfo->Flags & SYMFLAG_PARAMETER)
        params->push_back(symInfo->Name);
    return TRUE;
}


struct straceWin::EnumModulesContext
{
    HANDLE hProcess;
    std::ostringstream& stream;
    EnumModulesContext(HANDLE hProcess, std::ostringstream& stream): hProcess(hProcess), stream(stream) {}
};

BOOL CALLBACK straceWin::EnumModulesCB(LPCSTR ModuleName, DWORD64 BaseOfDll, PVOID UserContext)
{
    (void)ModuleName;
    IMAGEHLP_MODULE64 mod;
    EnumModulesContext* context = (EnumModulesContext*)UserContext;
    mod.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    if (SymGetModuleInfo64(context->hProcess, BaseOfDll, &mod)) {
        std::ostringstream moduleBase;
        moduleBase << "0x" << std::setw(16) << std::setfill('0') <<std::hex  << BaseOfDll;

        std::ostringstream line;
        line << mod.ModuleName << " " << moduleBase.str() << " " << mod.LoadedImageName;
        context->stream << line.str() << '\n';

        std::string pdbName(mod.LoadedPdbName);
        if (pdbName.length() > 0) {
            std::ostringstream lineTwo;
            lineTwo << std::setw(35) << std::setfill(' ') << pdbName;
            context->stream << " " << lineTwo.str() << '\n';
        }
    }
    return TRUE;
}



#if defined( _M_IX86 ) && defined(_MSC_VER)
// Disable global optimization and ignore /GS waning caused by
// inline assembly.
// not needed with mingw cause we can tell mingw which registers we use
#pragma optimize("g", off)
#pragma warning(push)
#pragma warning(disable : 4748)
#endif
const std::string straceWin::getBacktrace(std::ostringstream &logStream, const DWORD threadId, const PCONTEXT exctx)
{
    DWORD MachineType;
    STACKFRAME64 StackFrame;

    ZeroMemory(&StackFrame, sizeof(STACKFRAME64));
#ifdef _M_IX86
    MachineType                 = IMAGE_FILE_MACHINE_I386;
    StackFrame.AddrPC.Offset    = exctx->Eip;
    StackFrame.AddrPC.Mode      = AddrModeFlat;
    StackFrame.AddrFrame.Offset = exctx->Ebp;
    StackFrame.AddrFrame.Mode   = AddrModeFlat;
    StackFrame.AddrStack.Offset = exctx->Esp;
    StackFrame.AddrStack.Mode   = AddrModeFlat;
#elif _M_X64
    MachineType                 = IMAGE_FILE_MACHINE_AMD64;
    StackFrame.AddrPC.Offset    = exctx->Rip;
    StackFrame.AddrPC.Mode      = AddrModeFlat;
    StackFrame.AddrFrame.Offset = exctx->Rsp;
    StackFrame.AddrFrame.Mode   = AddrModeFlat;
    StackFrame.AddrStack.Offset = exctx->Rsp;
    StackFrame.AddrStack.Mode   = AddrModeFlat;
#elif _M_IA64
    MachineType                 = IMAGE_FILE_MACHINE_IA64;
    StackFrame.AddrPC.Offset    = exctx->StIIP;
    StackFrame.AddrPC.Mode      = AddrModeFlat;
    StackFrame.AddrFrame.Offset = exctx->IntSp;
    StackFrame.AddrFrame.Mode   = AddrModeFlat;
    StackFrame.AddrBStore.Offset = exctx->RsBSP;
    StackFrame.AddrBStore.Mode  = AddrModeFlat;
    StackFrame.AddrStack.Offset = exctx->IntSp;
    StackFrame.AddrStack.Mode   = AddrModeFlat;
#else
#error "Unsupported platform"
#endif

    logStream << "```\n";

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadId);

    SymInitialize(hProcess, NULL, TRUE);

    DWORD64 dwDisplacement;

    ULONG64 buffer[(sizeof(SYMBOL_INFO) +
                    MAX_SYM_NAME * sizeof(TCHAR) +
                    sizeof(ULONG64) - 1) /  sizeof(ULONG64)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;

    IMAGEHLP_MODULE64 mod;
    mod.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

    IMAGEHLP_STACK_FRAME ihsf;
    ZeroMemory(&ihsf, sizeof(IMAGEHLP_STACK_FRAME));

    int i = 0;

    while(StackWalk64(MachineType, hProcess, hThread, &StackFrame, exctx , NULL, NULL, NULL, NULL)) {
        if(i == 128)
            break;

        loadHelpStackFrame(ihsf, StackFrame);
        if(StackFrame.AddrPC.Offset != 0) { // Valid frame.

            std::string fileName("???");
            if(SymGetModuleInfo64(hProcess, ihsf.InstructionOffset, &mod)) {
                fileName = std::string(mod.ImageName);
                size_t slashPos = fileName.find_last_of('\\');
                if(slashPos != std::string::npos)
                    fileName = fileName.substr(slashPos + 1);
            }
            std::string funcName;
            if(SymFromAddr(hProcess, ihsf.InstructionOffset, &dwDisplacement, pSymbol)) {
                funcName = std::string(pSymbol->Name);
#ifdef __MINGW32__
                demangle(funcName);
#endif
            } else {
                std::ostringstream buffer;
                buffer << "0x" << std::setw(8) << std::setfill('0') << std::hex << ihsf.InstructionOffset;
                funcName = buffer.str();
            }
            SymSetContext(hProcess, &ihsf, NULL);
#ifndef __MINGW32__
            std::vector<std::string> params;
            SymEnumSymbols(hProcess, 0, NULL, EnumSymbolsCB, (PVOID)&params);
#endif

            {
                std::ostringstream buffer;
                buffer << " (0x" << std::setw(8) << std::setfill('0') << std::hex << ihsf.InstructionOffset << ")";
                funcName += buffer.str();
            }

            std::ostringstream insOffset;
            insOffset << "0x" << std::setw(16) << std::setfill('0') << std::hex << ihsf.InstructionOffset;

            std::string debugLine = std::to_string(i) + " " + fileName + " " + insOffset.str() + " " + funcName;

#ifndef __MINGW32__
            debugLine += " ";
            for (const std::string &s : params)
                debugLine += s + ", ";
#else
                                ;
#endif
            logStream << debugLine << '\n';
            i++;
        }
        else {
            break; // we're at the end.
        }
    }

    logStream << "\n\nList of linked Modules:\n";
    EnumModulesContext modulesContext(hProcess, logStream);
    SymEnumerateModules64(hProcess, EnumModulesCB, (PVOID)&modulesContext);
    logStream << "```";
    return logStream.str();
}
#if defined(_M_IX86) && defined(_MSC_VER)
#pragma warning(pop)
#pragma optimize("g", on)
#endif

#endif // CRASHHANDLING_WIN32

#endif // CRASHHANDLERWINDOWS_STACKTRACE_H
