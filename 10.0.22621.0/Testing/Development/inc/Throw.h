//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>
/// Entry point for throwing WEX::Exceptions.
/// 
/// Provides helper functions for verifying before throwing, as well as throwing without verification.
/// </summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#if defined(_WIN32)
#include "Wex.Common.h"
#include "WexTypes.h"

#include <specstrings.h>

/// <summary>
/// </summary>
namespace WEX { namespace Common { namespace Throw
{
    // Verify before throwing

    /// <summary>Throws a Wex::Exception if -condition- failed</summary>
    WEXCOMMON_API _Post_satisfies_(condition >= 0) void WEXCOMMON_STDCALL IfFailed(HRESULT condition);

    /// <summary>Throws a Wex::Exception if -condition- failed</summary>
    WEXCOMMON_API _Post_satisfies_(condition >= 0) void WEXCOMMON_STDCALL IfFailed(HRESULT condition, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception if -condition- is *false*</summary>
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL IfFalse(bool condition, _When_(!condition, _In_range_(<, 0)) HRESULT errorToThrow);

    /// <summary>Throws a Wex::Exception if -condition- is *false*</summary>
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL IfFalse(bool condition, _When_(!condition, _In_range_(<, 0)) HRESULT errorToThrow, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception if -condition- is *true*</summary>
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL If(bool condition, _When_(condition, _In_range_(<, 0)) HRESULT errorToThrow);

    /// <summary>Throws a Wex::Exception if -condition- is *true*</summary>
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL If(bool condition, _When_(condition, _In_range_(<, 0)) HRESULT errorToThrow, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception (E_POINTER) if -p- is null</summary>
    WEXCOMMON_API _Post_satisfies_(p != nullptr) void WEXCOMMON_STDCALL IfNull(_Maybenull_ const void* p);

    /// <summary>Throws a Wex::Exception (E_POINTER) if -p- is null</summary>
    WEXCOMMON_API _Post_satisfies_(p != nullptr) void WEXCOMMON_STDCALL IfNull(_Maybenull_ const void* p, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception with GetLastError() information if -condition- is *true*</summary>
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL LastErrorIf(bool condition);

    /// <summary>Throws a Wex::Exception with GetLastError() information if -condition- is *true*</summary>
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL LastErrorIf(bool condition, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception with GetLastError() information if -condition- is *false*</summary>
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL LastErrorIfFalse(bool condition);

    /// <summary>Throws a Wex::Exception with GetLastError() information if -condition- is *false*</summary>
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL LastErrorIfFalse(bool condition, _In_opt_z_ const wchar_t* pszMessage);

    // Throw without verification

    /// <summary>Throws a Wex::Exception with the specified parameters</summary>
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL Exception(_In_range_(<, 0) HRESULT errorToThrow);

    /// <summary>Throws a Wex::Exception with the specified parameters</summary>
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL Exception(_In_range_(<, 0) HRESULT errorToThrow, _In_opt_z_ const wchar_t* pszMessage);

    /// <summary>Throws a Wex::Exception with GetLastError() information</summary>
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL LastError();

    /// <summary>Throws a Wex::Exception with GetLastError() information</summary>
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL LastError(_In_opt_z_ const wchar_t* pszMessage);

    // wchar_t native type exports
    #if defined(WEXCOMMON_FULL_BUILD)
    WEXCOMMON_API _Post_satisfies_(condition >= 0) void WEXCOMMON_STDCALL IfFailed(HRESULT condition, _In_opt_z_ const __wchar_t* pszMessage);
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL IfFalse(bool condition, _When_(!condition, _In_range_(<, 0)) HRESULT errorToThrow, _In_opt_z_ const __wchar_t* pszMessage);
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL If(bool condition, _When_(condition, _In_range_(<, 0)) HRESULT errorToThrow, _In_opt_z_ const __wchar_t* pszMessage);
    WEXCOMMON_API _Post_satisfies_(p != nullptr) void WEXCOMMON_STDCALL IfNull(_Maybenull_ const void* p, _In_opt_z_ const __wchar_t* pszMessage);
    WEXCOMMON_API _Post_satisfies_(!condition) void WEXCOMMON_STDCALL LastErrorIf(bool condition, _In_opt_z_ const __wchar_t* pszMessage);
    WEXCOMMON_API _Post_satisfies_(condition) void WEXCOMMON_STDCALL LastErrorIfFalse(bool condition, _In_opt_z_ const __wchar_t* pszMessage);
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL Exception(_In_range_(<, 0) HRESULT errorToThrow, _In_opt_z_ const __wchar_t* pszMessage);
    [[noreturn]] WEXCOMMON_API void WEXCOMMON_STDCALL LastError(_In_opt_z_ const __wchar_t* pszMessage);
    #endif

    namespace Private {
        // not exported, only declared so that WEX::Common::Exception can befriend it
        void ThrowImpl(_In_range_(<, 0) HRESULT errorToThrow, _In_opt_z_ const wchar_t* pszMessage);
    }
} /* namespace Throw */ } /* namespace Common */ } /* namespace WEX */
#endif // #if defined(_WIN32)
