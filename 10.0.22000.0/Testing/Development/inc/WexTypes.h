/*
* Copyright (c) Microsoft Corporation.  All Rights Reserved.
*
* Provides typedefs for commonly used types, and template utilities and helpers for working with C++ types
*          
*/
#ifndef __WEXTYPES_H__
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
#endif // #if defined(_WIN32)

#if !defined(_WIN32)
#pragma push_macro("_Return_type_success_")
#if !defined(_Return_type_success_)
#define _Return_type_success_(x)
#endif
#endif // #if !defined(_WIN32)

typedef _Return_type_success_(return >= 0) long HRESULT;

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
#if !defined(__cplusplus_cli)
# if !defined(__INTRIN_H_)
// Including intrin.h causes build breaks for some TAEF users; so we forward declare __fastfail here.
extern "C" __declspec(noreturn) void __fastfail(unsigned int);
# endif // #if !defined(__INTRIN_H_)

# define TAEF_FAST_FAIL __fastfail(7 /* FAST_FAIL_FATAL_APP_EXIT */)
#else
// Using __fastfail with C++/CLI causes warning C4793 to be raised.
# define TAEF_FAST_FAIL System::Environment::FailFast(nullptr)
#endif // #if !defined(__cplusplus_cli)
#endif // #if defined(_WIN32)

namespace WEX { namespace Common
{
    struct NullType{};
    struct EmptyType{};
    struct TerminatorType{};

    template <class T, class U>
    class Conversion
    {
        typedef char SmallSize;
        struct BigSize 
        { 
            SmallSize a[2]; 
        };

        static SmallSize Test(const U&);
        static BigSize Test(...);
        static const T& MakeT();

    public:
        static const bool exists = (sizeof(Test(MakeT()))) == (sizeof(SmallSize));
        static const bool sameType = false;
    };

    template <>
    class Conversion<void, void>
    {
    public:
        static const bool exists = true;
        static const bool sameType = true;
    };

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

    template <typename T>
    class MakeReference
    {
    public:
        typedef T& Result;
    };

    template <typename T>
    class MakeReference<T&>
    {
    public:
        typedef T& Result;
    };

    template <>
    class MakeReference<NullType>
    {
    public:
        typedef NullType Result;
    };

    template <typename T>
    class RemoveReference
    {
    public:
        typedef T Result;
    };

    template <typename T>
    class RemoveReference<T&>
    {
    public:
        typedef T Result;
    };

    template<typename T>
    struct MakeConst
    {
        typedef const T Type;
    };

    template<typename T>
    struct MakeConst<T&>
    {
        typedef const T& Type;
    };

    template<typename T>
    struct MakeConst<T*>
    {
        typedef const T* Type;
    };

    template<typename T>
    struct RemoveConst
    {
        typedef T Type;
    };

    template<typename T>
    struct RemoveConst<const T>
    {
        typedef T Type;
    };

    template<typename T>
    struct RemoveConst<T&>
    {
        typedef T& Type;
    };

    template<typename T>
    struct RemoveConst<const T&>
    {
        typedef T& Type;
    };

    template<typename T>
    struct RemoveConst<T*>
    {
        typedef T* Type;
    };

    template<typename T>
    struct RemoveConst<const T*>
    {
        typedef T* Type;
    };

#if defined(_WIN32)
    template<typename T>
    class AlignedStorageFactory
    {
        template<size_t Size, size_t Align>
        struct Storage;

        template<size_t Size>
        struct Storage<Size, 0>
        {
        protected:
            __declspec(align(1)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 1>
        {
        protected:
            __declspec(align(1)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 2>
        {
        protected:
            __declspec(align(2)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 4>
        {
        protected:
            __declspec(align(4)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 8>
        {
        protected:
            __declspec(align(8)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 16>
        {
        protected:
            __declspec(align(16)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 32>
        {
        protected:
            __declspec(align(32)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 64>
        {
        protected:
            __declspec(align(64)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 128>
        {
        protected:
            __declspec(align(128)) unsigned char m_raw[Size];
        };

        template<size_t Size>
        struct Storage<Size, 256>
        {
        protected:
            __declspec(align(256)) unsigned char m_raw[Size];
        };

        // not implemented
        AlignedStorageFactory();

    public:
        typedef Storage<sizeof(T), __alignof(T)> Type;
    };
#endif // #if defined(_WIN32)
} /* namespace Common */ } /* namespace WEX */

#if !defined(_WIN32)
#pragma pop_macro("_Return_type_success_")
#endif // #if !defined(_WIN32)

#endif //__WEXTYPES_H__
