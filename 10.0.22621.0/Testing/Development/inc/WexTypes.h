/*
* Copyright (c) Microsoft Corporation.  All Rights Reserved.
*
* Provides typedefs for commonly used types, and template utilities and helpers for working with C++ types
*          
*/
#if !defined(__WEXTYPES_H__)
#define __WEXTYPES_H__

#include <stddef.h> // This defines wchar_t for MSVC when the /Zc:wchar_t- switch is used.

#if defined(_WIN32)
#include <specstrings.h>

typedef int BOOL;
typedef long LONG;
typedef unsigned long DWORD;
typedef DWORD ULONG;
typedef void* HANDLE;
typedef unsigned long (__stdcall *LPHANDLER_FUNCTION_EX)(unsigned long control, unsigned long eventType, void* pEventData, void* pContext);

// We don't call this LPCWSTR since the real LPCWSTR in winnt.h uses CONST and WCHAR, which we don't want to replicate here.
// We only use this in scenarios where there are no good alternatives for annotating that a value is null terminated.
typedef _Null_terminated_ const wchar_t* TAEF_LPCWSTR;

typedef _Return_type_success_(return >= 0) long HRESULT;
#else
#include <cstdint>

typedef int32_t HRESULT;
#endif

#if !defined(TAEF_STDCALL)
# if defined(_WIN32)
#  define TAEF_STDCALL __stdcall
# else
#  define TAEF_STDCALL
# endif
#endif

#if !defined(TAEF_CDECL)
# if defined(_WIN32)
#  define TAEF_CDECL __cdecl
# else
#  define TAEF_CDECL
# endif
#endif

#if !defined(TAEF_THISCALL)
# if defined(_WIN32)
#  define TAEF_THISCALL __thiscall
# else
#  define TAEF_THISCALL
# endif
#endif

// Support for the STL can be disabled with the TAEF_NO_STL_SUPPORT macro.

#if defined(TAEF_STL_SUPPORT) && defined(TAEF_NO_STL_SUPPORT)
# error Both TAEF_STL_SUPPORT and TAEF_NO_STL_SUPPORT are defined. You may only define at most one of these macros.
#endif

// Check to see if we want to turn on STL support automatically.
// We only do this when exceptions are enabled.
// You can explicitly turn it on by defining TAEF_STL_SUPPORT.
#if !defined(TAEF_NO_STL_SUPPORT) && !defined(TAEF_STL_SUPPORT) && defined(__cpp_exceptions)
# if defined(BUILD_WINDOWS)
// When in the Windows build environment, only support STL when it is enabled.
#  if defined(_STL100_) || defined(_STL110_) || defined (_STL120_) || defined(_STL140_)
#   define TAEF_STL_SUPPORT
#  endif
# else
// Default to supporting STL outside of the Windows build environment.
#  define TAEF_STL_SUPPORT
# endif
#endif

#if defined(_WIN32)
// Define a macro for code that causes the process to crash. TAEF uses this in code compiled into the
// test DLL when a crash dump should be generated at the point where some invalid state is detected.
# if !defined(__cplusplus_cli)
#  if !defined(__INTRIN_H_)
// Including intrin.h causes build breaks for some TAEF users; so we forward declare __fastfail here.
extern "C" __declspec(noreturn) void __fastfail(unsigned int);
#  endif // #if !defined(__INTRIN_H_)

#  define TAEF_FAST_FAIL __fastfail(7 /* FAST_FAIL_FATAL_APP_EXIT */)
# else
// Using __fastfail with C++/CLI causes warning C4793 to be raised.
#  define TAEF_FAST_FAIL System::Environment::FailFast(nullptr)
# endif
#endif // #if defined(_WIN32)

namespace WEX { namespace Common
{
    template <typename T1, typename T2>
    class AreSameType
    {
    public:
        static const bool value = false;
    };

    template <typename T>
    class AreSameType<T, T>
    {
    public:
        static const bool value = true;
    };
} /* namespace Common */ } /* namespace WEX */

#endif //__WEXTYPES_H__
