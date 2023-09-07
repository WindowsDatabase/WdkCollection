//---------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>File used for test verification purposes.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//---------------------------------------------------------------------------------------------------------------------
#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1910
# error Unsupported version of Visual Studio. Verify.h requires VS 2017 or later.
#endif

#include "Log.h"
#include "PreserveLastError.h"
#include "WexAssert.h"
#include "WexDebug.h"
#include "WexString.h"
#include "WexTypes.h"

#if defined(__has_include)
# define TAEF_HAS_INCLUDE(X) __has_include(X)
#else
# define TAEF_HAS_INCLUDE(X) 0
#endif

#if defined(__cpp_exceptions)
# include "WexException.h"
// The logic to define TAEF_STL_SUPPORT is in WexTypes.h.
# if defined(TAEF_STL_SUPPORT)
// Include <version> to define library feature test macros.
#  if !defined(TAEF_NO_VERSION_SUPPORT) && TAEF_HAS_INCLUDE(<version>)
#   include <version>
#  endif

#  pragma push_macro("HUGE")
#  undef HUGE
#  include <string>
#  if defined(__cpp_lib_string_view)
#   include <string_view>
#  endif
#  pragma pop_macro("HUGE")

#  if !defined(TAEF_NO_SYSTEM_ERROR_SUPPORT) && TAEF_HAS_INCLUDE(<system_error>)
#   define TAEF_SYSTEM_ERROR_SUPPORT
#   include <ctype.h> // for ::isspace
#   include <limits.h> // for INT_MAX
#   include <system_error>
#  endif

// Verify.h has two implementations for formatting values: a newer one based on std::format
// and an older one that works even when exceptions and the STL are disabled. Tests can define
// TAEF_NO_STD_FORMAT to opt into using the older implementation even when std::format is
// available. Tests that want to use the std::format implementation but don't yet have the
// STL <format> header available can define TAEF_USE_FMTLIB to have TAEF use fmt::format from
// the open source fmtlib project (https://github.com/fmtlib/fmt) instead of using std::format.
#  if !defined(TAEF_NO_STD_FORMAT) && defined(__cpp_concepts) && defined(__cpp_lib_string_view)
#   if defined(TAEF_USE_FMTLIB)
#    define TAEF_USE_STD_FORMAT
#    define TAEF_STD_FORMAT_NAMESPACE fmt
#    include <fmt/format.h>
#   elif defined(__cpp_lib_format)
#    define TAEF_USE_STD_FORMAT
#    define TAEF_STD_FORMAT_NAMESPACE std
#    include <format>
#   endif
#   if defined(TAEF_USE_STD_FORMAT)
// <iterator> is needed for using std::back_inserter to append to a string using format_to.
#    include <iterator>
// <type_traits> is for std::underlying_type_t and std::is_same_v.
#    include <type_traits>
// <utility> is for std::move.
#    include <utility>
#   endif
#  endif // #if !defined(TAEF_NO_STD_FORMAT)
# endif // #if defined(TAEF_STL_SUPPORT)

# include <exception>

// When TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE is defined, VerifyFailureException inherits from std::exception,
// rather than WEX::Common::Exception. In the future, WEX::Common::Exception will likely be updated to itself inherit from
// std::exception. When this occurs, this configuration option might be removed since VerifyFailureException would inherit
// from std::exception either way. This is on by default for Linux, but is opt-in for Windows.
# if !defined(_WIN32) && !defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
#  define TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE
# endif

#else // ^^ defined(__cpp_exceptions) ^^ | vv !defined(__cpp_exceptions) vv
// Disable verify exception usage if exceptions are disabled for this project.
# if !defined(NO_VERIFY_EXCEPTIONS)
#  define NO_VERIFY_EXCEPTIONS
# endif
#endif


#include <string.h> // For memcpy, memcmp, and strlen. On Windows, we also use wcslen.

#if defined(_WIN32)
// Without deprecation support StrSafe.h will define 'malicious' macros in an attempt to prevent older API usage.
// These errors are confusing - deprecation is a lot clearer.
# if !defined(MIDL_PASS) && !defined(DEPRECATE_SUPPORTED)
#  define DEPRECATE_SUPPORTED
# endif
# include <StrSafe.h>

# include <winerror.h>
#endif

// These macros are equivalent to the Windows macros of the same name except without "TAEF_".
// TAEF defines its own version so it can use the same error code values on Linux.
#define TAEF_S_OK static_cast<HRESULT>(0)
#define TAEF_S_FALSE static_cast<HRESULT>(1)
#define TAEF_E_FAIL static_cast<HRESULT>(0x80004005)
#define TAEF_E_OUTOFMEMORY static_cast<HRESULT>(0x8007000E)
#define TAEF_E_UNEXPECTED static_cast<HRESULT>(0x8000FFFF)

#if defined(CPPWINRT_VERSION)
#define TAEF_CPPWINRT_SUPPORT
#endif

#if !defined(_WIN32)
#pragma push_macro("_In_")
#pragma push_macro("_In_reads_bytes_")
#pragma push_macro("_Out_")
#pragma push_macro("_Out_opt_")
#pragma push_macro("_Out_writes_bytes_to_")
#pragma push_macro("_Post_equal_to_")
#pragma push_macro("_Post_invalid_")
#pragma push_macro("_Printf_format_string_")
#pragma push_macro("_Success_")
#if !defined(_In_)
#define _In_
#endif
#if !defined(_In_reads_bytes_)
#define _In_reads_bytes_(x)
#endif
#if !defined(_Out_)
#define _Out_
#endif
#if !defined(_Out_opt_)
#define _Out_opt_
#endif
#if !defined(_Out_writes_bytes_to_)
#define _Out_writes_bytes_to_(x,y)
#endif
#if !defined(_Post_equal_to_)
#define _Post_equal_to_(x)
#endif
#if !defined(_Post_invalid_)
#define _Post_invalid_
#endif
#if !defined(_Printf_format_string_)
#define _Printf_format_string_
#endif
#if !defined(_Success_)
#define _Success_(x)
#endif
#endif // #if !defined(_WIN32)

// Allow anyone who has defined a Verify, or Trace macro to compile with this header file included.
#pragma push_macro("Verify")
#undef Verify
#pragma push_macro("Trace")
#undef Trace

/// <summary>
/// \def VERIFY_ARE_EQUAL(expected, actual, ...)
/// Verifies that two specified objects are equal.
/// </summary>
#define VERIFY_ARE_EQUAL(expected, actual, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::AreEqual((expected), (actual), \
        u8 ## #expected, u8 ## #actual, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that two specified objects are not equal.
#define VERIFY_ARE_NOT_EQUAL(unexpected, actual, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::AreNotEqual((unexpected), (actual), \
        u8 ## #unexpected, u8 ## #actual, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the first parameter is greater than the second parameter.
#define VERIFY_IS_GREATER_THAN(expectedGreater, expectedLess, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsGreaterThan((expectedGreater), (expectedLess), \
        u8 ## #expectedGreater, u8 ## #expectedLess, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the first parameter is greater than or equal to the second parameter.
#define VERIFY_IS_GREATER_THAN_OR_EQUAL(expectedGreater, expectedLess, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsGreaterThanOrEqual((expectedGreater), (expectedLess), \
        u8 ## #expectedGreater, u8 ## #expectedLess, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the first parameter is less than the second parameter.
#define VERIFY_IS_LESS_THAN(expectedLess, expectedGreater, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsLessThan((expectedLess), (expectedGreater), \
        u8 ## #expectedLess, u8 ## #expectedGreater, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the first parameter is less than or equal to the second parameter.
#define VERIFY_IS_LESS_THAN_OR_EQUAL(expectedLess, expectedGreater, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsLessThanOrEqual((expectedLess), (expectedGreater), \
        u8 ## #expectedLess, u8 ## #expectedGreater, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the two parameters specified refer to the same object.
#define VERIFY_ARE_SAME(expected, actual, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::AreSame((expected), (actual), \
        u8 ## #expected, u8 ## #actual, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the two parameters specified do not refer to the same object.
#define VERIFY_ARE_NOT_SAME(unexpected, actual, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::AreNotSame((unexpected), (actual), \
        u8 ## #unexpected, u8 ## #actual, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Fails without checking any conditions.
#define VERIFY_FAIL(...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Fail( \
        __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified condition is true.
#define VERIFY_IS_TRUE(condition, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsTrue((condition), \
        u8 ## #condition, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified condition is false.
#define VERIFY_IS_FALSE(condition, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsFalse((condition), \
        u8 ## #condition, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified parameter is null.
#define VERIFY_IS_NULL(object, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsNull((object), \
        u8 ## #object, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified parameter is not null.
#define VERIFY_IS_NOT_NULL(object, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::IsNotNull((object), \
        u8 ## #object, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

#if defined(_WIN32)
// Verifies that the specified HRESULT is successful.
# define VERIFY_SUCCEEDED(hresult, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Succeeded((hresult), \
        u8 ## #hresult, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified HRESULT is successful, and returns the HRESULT that was passed in.
# define VERIFY_SUCCEEDED_RETURN(hresult, ...) \
    (HRESULT)WEX::TestExecution::Private::MacroVerify::SucceededReturn((hresult), \
        u8 ## #hresult, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified HRESULT is not successful.
# define VERIFY_FAILED(hresult, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Failed((hresult), \
        u8 ## #hresult, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified HRESULT is not successful, and returns the HRESULT that was passed in.
# define VERIFY_FAILED_RETURN(hresult, ...) \
    (HRESULT)WEX::TestExecution::Private::MacroVerify::FailedReturn((hresult), \
        u8 ## #hresult, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 result succeeded.
# define VERIFY_WIN32_SUCCEEDED(win32Result, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Win32Succeeded((win32Result), \
        u8 ## #win32Result, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 result succeeded, and returns the Win32 value that was passed in.
# define VERIFY_WIN32_SUCCEEDED_RETURN(win32Result, ...) \
    (::LONG)WEX::TestExecution::Private::MacroVerify::Win32SucceededReturn((win32Result), \
        u8 ## #win32Result, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 result failed.
# define VERIFY_WIN32_FAILED(win32Result, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Win32Failed((win32Result), \
        u8 ## #win32Result, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 result failed, and returns the Win32 value that was passed in.
# define VERIFY_WIN32_FAILED_RETURN(win32Result, ...) \
    (::LONG)WEX::TestExecution::Private::MacroVerify::Win32FailedReturn((win32Result), \
        u8 ## #win32Result, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 BOOL succeeded and logs the error code returned from GetLastError if not.
# define VERIFY_WIN32_BOOL_SUCCEEDED(win32Bool, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Win32BoolSucceeded((win32Bool), \
        u8 ## #win32Bool, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 BOOL succeeded and logs the error code returned from GetLastError if not; returns the Win32 BOOL that was passed in.
# define VERIFY_WIN32_BOOL_SUCCEEDED_RETURN(win32Bool, ...) \
    (::BOOL)WEX::TestExecution::Private::MacroVerify::Win32BoolSucceededReturn((win32Bool), \
        u8 ## #win32Bool, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 BOOL failed.
# define VERIFY_WIN32_BOOL_FAILED(win32Bool, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::Win32BoolFailed((win32Bool), \
        u8 ## #win32Bool, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

// Verifies that the specified Win32 BOOL failed and returns the Win32 BOOL that was passed in.
# define VERIFY_WIN32_BOOL_FAILED_RETURN(win32Bool, ...) \
    (::BOOL)WEX::TestExecution::Private::MacroVerify::Win32BoolFailedReturn((win32Bool), \
        u8 ## #win32Bool, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

# if defined(NT_SUCCESS)
#  define VERIFY_NT_SUCCESS(ntStatus, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::NtSuccess((ntStatus), \
        u8 ## #ntStatus, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)

#  define VERIFY_NOT_NT_SUCCESS(ntStatus, ...) \
    (bool)WEX::TestExecution::Private::MacroVerify::NotNtSuccess((ntStatus), \
        u8 ## #ntStatus, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__)
# endif // #if defined(NT_SUCCESS)
#endif // #if defined(_WIN32)

// Only enable the THROWS/NO_THROW macros if exceptions are enabled.
#if defined(__cpp_exceptions)
// Verifies that the specified operation throws an exception of the specified type that meets the functor criteria.
// The temporary TAEF_exceptionIsExpected variable is necessary to workaround a bug in nested lambdas used in conditionals.
# define VERIFY_THROWS_SPECIFIC(operation, exceptionType, func, ...)                                                                                 \
{                                                                                                                                                    \
    bool TAEF_exceptionHit = false;                                                                                                                  \
    try                                                                                                                                              \
    {                                                                                                                                                \
        operation;                                                                                                                                   \
    }                                                                                                                                                \
    catch(exceptionType& TAEF_caughtException)                                                                                                       \
    {                                                                                                                                                \
        const bool TAEF_exceptionIsExpected = func(TAEF_caughtException);                                                                            \
        if (TAEF_exceptionIsExpected)                                                                                                                \
        {                                                                                                                                            \
            WEX::TestExecution::Private::MacroVerify::ExpectedExceptionThrown(u8 ## #exceptionType, u8 ## #operation, ## __VA_ARGS__);               \
            TAEF_exceptionHit = true;                                                                                                                \
        }                                                                                                                                            \
        else                                                                                                                                         \
        {                                                                                                                                            \
            throw;                                                                                                                                   \
        }                                                                                                                                            \
    }                                                                                                                                                \
                                                                                                                                                     \
    if (!TAEF_exceptionHit)                                                                                                                          \
    {                                                                                                                                                \
        WEX::TestExecution::Private::MacroVerify::ExpectedExceptionNotThrown(                                                                        \
            u8 ## #exceptionType, u8 ## #operation, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__);                                               \
    }                                                                                                                                                \
}

// Verifies that the specified operation throws the specified exception.
# define VERIFY_THROWS(operation, exceptionType, ...) \
    VERIFY_THROWS_SPECIFIC(operation, exceptionType, WEX::TestExecution::Private::TrueUnaryFunctor<exceptionType&>(), ## __VA_ARGS__)

# if defined(__cplusplus_winrt)
#  if !defined(VERIFY_THROWS_SPECIFIC_CX)
// C++/CX versions of VERIFY_THROWS macros  
#   define VERIFY_THROWS_SPECIFIC_CX(operation, exceptionType, func, ...)                                                                            \
{                                                                                                                                                    \
    bool TAEF_exceptionHit = false;                                                                                                                  \
    try                                                                                                                                              \
    {                                                                                                                                                \
        operation;                                                                                                                                   \
    }                                                                                                                                                \
    catch (exceptionType^ TAEF_caughtException)                                                                                                      \
    {                                                                                                                                                \
        const bool TAEF_exceptionIsExpected = func(TAEF_caughtException);                                                                            \
        if (TAEF_exceptionIsExpected)                                                                                                                \
        {                                                                                                                                            \
            WEX::TestExecution::Private::MacroVerify::ExpectedExceptionThrown(u8 ## #exceptionType, u8 ## #operation, ## __VA_ARGS__);               \
            TAEF_exceptionHit = true;                                                                                                                \
        }                                                                                                                                            \
        else                                                                                                                                         \
        {                                                                                                                                            \
            throw;                                                                                                                                   \
        }                                                                                                                                            \
    }                                                                                                                                                \
                                                                                                                                                     \
    if (!TAEF_exceptionHit)                                                                                                                          \
    {                                                                                                                                                \
        WEX::TestExecution::Private::MacroVerify::ExpectedExceptionNotThrown(                                                                        \
            u8 ## #exceptionType, u8 ## #operation, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__);                                               \
    }                                                                                                                                                \
}
#  endif  // VERIFY_THROWS_SPECIFIC_CX is not defined

#  if !defined(VERIFY_THROWS_CX)
#   define VERIFY_THROWS_CX(operation, exceptionType, ...)                                                                                           \
        VERIFY_THROWS_SPECIFIC_CX(operation, exceptionType, WEX::TestExecution::Private::TrueUnaryFunctor<exceptionType^>(), ## __VA_ARGS__)
#  endif // VERIFY_THROWS_CX is not defined 
# endif // __cplusplus_winrt is defined

// Verifies that the specified operation does not throw an exception.
# define VERIFY_NO_THROW(operation, ...)                                                                                             \
{                                                                                                                                    \
    bool TAEF_exceptionHit = false;                                                                                                  \
    try                                                                                                                              \
    {                                                                                                                                \
        operation;                                                                                                                   \
    }                                                                                                                                \
    catch(...)                                                                                                                       \
    {                                                                                                                                \
        WEX::TestExecution::Private::MacroVerify::UnexpectedExceptionThrown(u8 ## #operation,                                        \
            __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__);                                                                       \
        TAEF_exceptionHit = true;                                                                                                    \
    }                                                                                                                                \
                                                                                                                                     \
    if (!TAEF_exceptionHit)                                                                                                          \
    {                                                                                                                                \
        WEX::TestExecution::Private::MacroVerify::UnexpectedExceptionNotThrown(u8 ## #operation, ## __VA_ARGS__);                    \
    }                                                                                                                                \
}
#endif // #if defined(__cpp_exceptions)

// There are two VERIFY implementations for formatting values so they can be included in log messages.
// One depends on C++20 features including concepts and std::format. The other works even for tests which
// have disabled C++ exceptions and the STL.
//
// When the VERIFY implementation is using std::format as the mechanism to display values, it passes the
// values to GetFormattableVerifyOutputValue and uses the return value as the format argument with {}.
// The code is similar to this:
//   std::format(" - Value ({})", GetFormattableVerifyOutputValue(value))
//
// GetFormattableVerifyOutputValue overloads are provided that work for most cases. You can add more
// overloads for your own types if desired. Overloads for your own types can be in the same namespace as
// the type due to C++ argument-dependent lookup. If a custom implementation throws an exception, the
// verify message will still be logged, but no values will be included in the message.
//
// If your custom type can be used with std::format directly due to having a std::formatter
// specialization, then you don't need to write a GetFormattableVerifyOutputValue function. One of the
// provided overloads takes care of this case for you.
//
// In the fallback implementation which does not use std::format, you can format custom types by
// specializing the VerifyOutputTraits class template. On Windows, specializations of the
// VerifyOutputTraits class template still work with the new std::format-based implementation, but a
// std::formatter specialization is preferred over a VerifyOutputTraits specialization if both are
// available. The VerifyOutputTraits class is currently Windows-specific; so Linux tests using the
// fallback implementation have no mechanism to provide formatting for custom types.

// These definitions allow you to customize formatting for the VerifyOutputTraits specializations
// for common types. If these are customized, the std::format implementation is disabled so the
// fallback implemention will be used since only it honors these macros.
#if defined(LONG_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define LONG_VERIFY_FORMATTING L"0x%x"  // Default to formatting for HRESULTS
#endif

#if defined(INT_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define INT_VERIFY_FORMATTING L"%d"
#endif

#if defined(CHAR_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define CHAR_VERIFY_FORMATTING L"%c"
#endif

#if defined(SHORT_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define SHORT_VERIFY_FORMATTING INT_VERIFY_FORMATTING
#endif

#if defined(FLOAT_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define FLOAT_VERIFY_FORMATTING L"%fl"
#endif

#if defined(SIZE_T_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define SIZE_T_VERIFY_FORMATTING L"%d"
#endif

#if defined(DOUBLE_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define DOUBLE_VERIFY_FORMATTING FLOAT_VERIFY_FORMATTING
#endif

#if defined(UNSIGNED_LONG_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define UNSIGNED_LONG_VERIFY_FORMATTING L"%u"
#endif

#if defined(UNSIGNED_INT_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define UNSIGNED_INT_VERIFY_FORMATTING L"%u"
#endif

#if defined(UNSIGNED_CHAR_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define UNSIGNED_CHAR_VERIFY_FORMATTING UNSIGNED_INT_VERIFY_FORMATTING
#endif

#if defined(UNSIGNED_SHORT_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define UNSIGNED_SHORT_VERIFY_FORMATTING UNSIGNED_INT_VERIFY_FORMATTING
#endif

#if defined(INT_64_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define INT_64_VERIFY_FORMATTING L"%I64d"
#endif

#if defined(UNSIGNED_INT_64_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define UNSIGNED_INT_64_VERIFY_FORMATTING L"%I64u"
#endif

#if defined(POINTER_VERIFY_FORMATTING)
# undef TAEF_USE_STD_FORMAT
#else
# define POINTER_VERIFY_FORMATTING L"0x%p"
#endif

#pragma warning(push)
// Disable "Prefer enum class over enum".
#pragma warning(disable:26812)
namespace WEX { namespace TestExecution
{
    namespace Private
    {
        // Please do not call functions in the Private namespace directly. They are implementation details of the code in the
        // public interface.
        extern "C"
        {
            _Success_(return)
            WEXLOGGER_API bool WEXLOGGER_STDCALL TAEF_TryGetUtf8ByteCountForCharStringValue(const char* string, size_t stringSize,
                _Out_ size_t& utf8ByteCount) noexcept;
            _Success_(return)
            WEXLOGGER_API bool WEXLOGGER_STDCALL TAEF_TryWriteUtf8ForCharStringValue(const char* string, size_t stringSize,
                char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) noexcept;

#if defined(_WIN32)
            _Success_(return)
            WEXLOGGER_API bool WEXLOGGER_STDCALL TAEF_TryGetUtf8ByteCountForWCharStringValue(const wchar_t* string, size_t stringSize,
                _Out_ size_t& utf8ByteCount) noexcept;
            _Success_(return)
            WEXLOGGER_API bool WEXLOGGER_STDCALL TAEF_TryWriteUtf8ForWCharStringValue(const wchar_t* string, size_t stringSize,
                char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) noexcept;
#endif // #if defined(_WIN32)
        }

#if defined(TAEF_USE_STD_FORMAT)
        bool TryConvertWideStringToUtf8String(const wchar_t* string, size_t stringSize, std::string& result) noexcept
        {
            result.clear();

            size_t utf8ByteCount = 0;
            if (TAEF_TryGetUtf8ByteCountForWCharStringValue(string, stringSize, utf8ByteCount))
            {
                try
                {
                    result.resize(utf8ByteCount);
                }
                catch (std::bad_alloc&)
                {
                    return false;
                }
                size_t bytesWritten = 0;
                if (TAEF_TryWriteUtf8ForWCharStringValue(string, stringSize, result.data(), utf8ByteCount, bytesWritten))
                {
                    result.resize(bytesWritten);
                    return true;
                }

                result.clear();
            }

            return false;
        }
#endif // #if defined(TAEF_USE_STD_FORMAT)

        struct VerifyFormatAsHResult { HRESULT m_hr; };

        struct DoNotFormatWithVerify {};
    }/* namespace Private */

#if defined(_WIN32)
    // See the large comment above for an explanation of this class.
    template <typename T>
    class VerifyOutputTraits;

    template <typename T>
    class VerifyOutputTraits<T&>
    {
    public:
        static WEX::Common::NoThrowString ToString(const T& ref)
        {
            // Default implementation call through to non-reference type.
            return VerifyOutputTraits<T>::ToString(ref);
        }
    };

    template <typename T>
    class VerifyOutputTraits<T*>
    {
    public:
        static WEX::Common::NoThrowString ToString(T* const& p)
        {
            return WEX::Common::NoThrowString().Format(POINTER_VERIFY_FORMATTING, p);
        }
    };

    template <typename T>
    class VerifyOutputTraits<volatile T>
    {
    public:
        static WEX::Common::NoThrowString ToString(const volatile T& ref)
        {
            // Default implementation call through to non-volatile type.
            // The const_cast removes volatile, which reduces the risk of getting a confusing error message such as
            // VERIFY_ARE_EQUAL failing with (4, 4) if a volatile value is changed between the equality check and the error message.
            return VerifyOutputTraits<T>::ToString(const_cast<const T&>(ref));
        }
    };

#define BUILD_OUTPUT_TRAIT(Type, FormatString) \
    template<> \
    class VerifyOutputTraits<Type> \
    { \
    public: \
        static WEX::Common::NoThrowString ToString(Type const& val) \
        { \
            return WEX::Common::NoThrowString().Format(FormatString, val); \
        } \
    };

    BUILD_OUTPUT_TRAIT(long, LONG_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(int, INT_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(char, CHAR_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(short, SHORT_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(float, FLOAT_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(double, DOUBLE_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(unsigned long, UNSIGNED_LONG_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(unsigned int, UNSIGNED_INT_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(unsigned char, UNSIGNED_CHAR_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(unsigned short, UNSIGNED_SHORT_VERIFY_FORMATTING);
#if defined(_WIN32)
    BUILD_OUTPUT_TRAIT(__int64, INT_64_VERIFY_FORMATTING);
    BUILD_OUTPUT_TRAIT(unsigned __int64, UNSIGNED_INT_64_VERIFY_FORMATTING);
#endif // #if defined(_WIN32)
    BUILD_OUTPUT_TRAIT(bool, UNSIGNED_CHAR_VERIFY_FORMATTING);

    #undef BUILD_OUTPUT_TRAIT

#if defined(TAEF_STL_SUPPORT)
    // Custom class to output the value of std::wstring
    template<>
    class VerifyOutputTraits<std::wstring>
    {
    public:
        static WEX::Common::NoThrowString ToString(std::wstring const& string)
        {
            return WEX::Common::NoThrowString(string.c_str());
        }
    };

    // Custom class to output the value of std::string
    template<>
    class VerifyOutputTraits<std::string>
    {
    public:
        static WEX::Common::NoThrowString ToString(std::string const& string)
        {
            return WEX::Common::NoThrowString(string.c_str());
        }
    };
#endif // #if defined(TAEF_STL_SUPPORT)

    // Custom class to output the value of WEX::Common::String
    template<>
    class VerifyOutputTraits<WEX::Common::String>
    {
    public:
        static WEX::Common::NoThrowString ToString(WEX::Common::String const& string)
        {
            return WEX::Common::NoThrowString(string);
        }
    };

    // Custom class to output the value of WEX::Common::NoThrowString
    template<>
    class VerifyOutputTraits<WEX::Common::NoThrowString>
    {
    public:
        static WEX::Common::NoThrowString ToString(WEX::Common::NoThrowString const& string)
        {
            return string;
        }
    };

#if !defined(TAEF_USE_CUSTOM_GUID_VERIFY) && defined(GUID_DEFINED)
    // Custom class to output the value of GUID
    template <>
    class VerifyOutputTraits<GUID>
    {
    public:
        static WEX::Common::NoThrowString ToString(GUID const & Guid) 
        {
            return WEX::Common::NoThrowString().Format(L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                Guid.Data1,
                Guid.Data2,
                Guid.Data3,
                Guid.Data4[0],
                Guid.Data4[1],
                Guid.Data4[2],
                Guid.Data4[3],
                Guid.Data4[4],
                Guid.Data4[5],
                Guid.Data4[6],
                Guid.Data4[7]);
        }
    };
#endif //TAEF_USE_CUSTOM_GUID_VERIFY && GUID_DEFINED

    template <typename T, bool IsEnum>
    class DefaultOutputTraits;

    template <typename T>
    class DefaultOutputTraits<T, true>
    {
    public:
        static WEX::Common::NoThrowString ToString(const T& t)
        {
            // Return value as an unsigned long for enums
            return VerifyOutputTraits<unsigned long>::ToString(static_cast<unsigned long>(t));
        }
    };

    template <typename T>
    class DefaultOutputTraits<T, false>
    {
    public:
        static WEX::Common::NoThrowString ToString(const T&)
        {
            // Just return an empty string by default
            return WEX::Common::NoThrowString();
        }
    };

    // Default implementation
    template <typename T>
    class VerifyOutputTraits
    {
    public:
        struct IsDefaultImplementation {};

        static WEX::Common::NoThrowString ToString(const T& t)
        {
            return DefaultOutputTraits<T, __is_enum(T)>::ToString(t);
        }
    };
#endif // #if defined(_WIN32)

#if defined(TAEF_USE_STD_FORMAT)
    namespace Private
    {
        template <typename T>
        concept HasStdFormatter = requires(const T& value, TAEF_STD_FORMAT_NAMESPACE::format_context& formatContext) {
            std::declval<TAEF_STD_FORMAT_NAMESPACE::formatter<T>>().format(value, formatContext);
        };

        template <typename T>
        concept HasVerifyOutputTraits = !requires(const T& value) {
            typename VerifyOutputTraits<T>::IsDefaultImplementation;
        };
    }/* namespace Private */

# if defined(__cpp_char8_t)
    char GetFormattableVerifyOutputValue(char8_t value) noexcept { return static_cast<char>(value); }
    const char* GetFormattableVerifyOutputValue(const char8_t* pszValue) noexcept { return reinterpret_cast<const char*>(pszValue); }
    const char* GetFormattableVerifyOutputValue(char8_t* pszValue) noexcept { return reinterpret_cast<const char*>(pszValue); }
#  if defined(__cpp_lib_char8_t)
    std::string_view GetFormattableVerifyOutputValue(std::u8string_view value) noexcept
    {
        return std::string_view{reinterpret_cast<const char*>(value.data()), value.size()};
    }
    std::string_view GetFormattableVerifyOutputValue(const std::u8string& value) noexcept
    {
        return std::string_view{reinterpret_cast<const char*>(value.c_str()), value.size()};
    }
#  endif // #if defined(__cpp_lib_char8_t)
# endif // #if defined(__cpp_char8_t)

# if defined(_WIN32)
    std::string GetFormattableVerifyOutputValue(const wchar_t* pszValue) noexcept
    {
        std::string result;
        Private::TryConvertWideStringToUtf8String(pszValue, ::wcslen(pszValue), result);
        // On failure, result will be empty.
        return result;
    }
    std::string GetFormattableVerifyOutputValue(wchar_t* pszValue) noexcept
    {
        return GetFormattableVerifyOutputValue(static_cast<const wchar_t*>(pszValue));
    }
    std::string GetFormattableVerifyOutputValue(std::wstring_view value) noexcept
    {
        std::string result;
        Private::TryConvertWideStringToUtf8String(value.data(), value.size(), result);
        // On failure, result will be empty.
        return result;
    }
    std::string GetFormattableVerifyOutputValue(const std::wstring& value) noexcept
    {
        return GetFormattableVerifyOutputValue(std::wstring_view{value});
    }
    std::string GetFormattableVerifyOutputValue(wchar_t value) noexcept
    {
        return GetFormattableVerifyOutputValue(std::wstring_view{&value, 1});
    }
    std::string GetFormattableVerifyOutputValue(const WEX::Common::String& value) noexcept
    {
        return GetFormattableVerifyOutputValue(std::wstring_view{static_cast<const wchar_t*>(value), static_cast<size_t>(value.GetLength())});
    }
    std::string GetFormattableVerifyOutputValue(const WEX::Common::NoThrowString& value) noexcept
    {
        return GetFormattableVerifyOutputValue(std::wstring_view{static_cast<const wchar_t*>(value), static_cast<size_t>(value.GetLength())});
    }

    std::string GetFormattableVerifyOutputValue(const Private::VerifyFormatAsHResult& value) noexcept
    {
        // The static_cast here is based on the definition of HRESULT in WexTypes.h.
#if defined(_WIN32)
        return TAEF_STD_FORMAT_NAMESPACE::format("0x{:x}", static_cast<unsigned long>(value.m_hr));
#else
        return TAEF_STD_FORMAT_NAMESPACE::format("0x{:x}", static_cast<uint32_t>(value.m_hr));
#endif
    }

    // Use VerifyOutputTraits for types that have a custom VerifyOutputTraits specialization,
    // but not a std::formatter specialization.
    template <Private::HasVerifyOutputTraits T> requires !Private::HasStdFormatter<T>
    std::string GetFormattableVerifyOutputValue(const T& value) noexcept
    {
        return GetFormattableVerifyOutputValue(VerifyOutputTraits<T>::ToString(value));
    }
# endif // #if defined(_WIN32)

    // Pointers without a std::formatter implementation and without a more specific overload
    // are converted to a const void*, which std::format displays as an address.
    template <typename T> requires !Private::HasStdFormatter<T*>
    const void* GetFormattableVerifyOutputValue(T* pValue) noexcept { return pValue; }

    // If the type has a std::formatter implementation, we can let it through as is.
    template <Private::HasStdFormatter T>
    const T& GetFormattableVerifyOutputValue(const T& value) noexcept { return value; }

    // This overload handles enums which have an underlying type that can be formatted.
    template <typename T> requires Private::HasStdFormatter<std::underlying_type_t<T>>
    auto GetFormattableVerifyOutputValue(const T& value) noexcept
    {
        return static_cast<std::underlying_type_t<T>>(value);
    }

    // For any other type, do not include values in the verify output.
    template <typename T>
    Private::DoNotFormatWithVerify GetFormattableVerifyOutputValue(const T&) noexcept { return {}; }
#endif // #if defined(TAEF_USE_STD_FORMAT)

    // Users can specialize VerifyCompareTraits for their classes to perform more detailed comparisons.
    template <typename T1, typename T2 = T1>
    class VerifyCompareTraits
    {
    public:
        static bool AreEqual(const T1& expected, const T2& actual)
        {
            return (expected == actual) != 0;  // != 0 to handle overloads of operator== that return BOOL instead of bool
        }

        static bool AreSame(const T1& expected, const T2& actual)
        {
            return static_cast<const void*>(&expected) == static_cast<const void*>(&actual);
        }

        static bool IsLessThan(const T1& expectedLess, const T2& expectedGreater)
        {
            return expectedLess < expectedGreater;
        }

        static bool IsGreaterThan(const T1& expectedGreater, const T2& expectedLess)
        {
            return expectedGreater > expectedLess;
        }

        static bool IsNull(const T1& object)
        {
            // This works with various kinds of bool checkable types, such as smart pointers.
            return object ? false : true;
        }
    };

    template <typename T1, typename T2>
    class VerifyCompareTraits<T1*, T2*>
    {
    public:
        static bool AreEqual(T1* const& expected, T2* const& actual)
        {
            return static_cast<const void*>(expected) == static_cast<const void*>(actual);
        }

        static bool AreSame(T1* const& expected, T2* const& actual)
        {
            return static_cast<const void*>(&expected) == static_cast<const void*>(&actual);
        }

        static bool IsLessThan(T1* const& expectedLess, T2* const& expectedGreater)
        {
            return static_cast<const void*>(expectedLess) < static_cast<const void*>(expectedGreater);
        }

        static bool IsGreaterThan(T1* const& expectedGreater, T2* const& expectedLess)
        {
            return static_cast<const void*>(expectedGreater) > static_cast<const void*>(expectedLess);
        }

        static bool IsNull(T1* const& object)
        {
            return object == nullptr;
        }
    };

#if !defined(TAEF_USE_CUSTOM_GUID_VERIFY) && defined(GUID_DEFINED)
    template <>
    class VerifyCompareTraits<GUID>
    {
    public:
        static bool AreEqual(GUID const & expected, GUID const & actual) 
        {
            return ::memcmp(&expected, &actual, sizeof(GUID)) == 0;
        }

        static bool AreSame(GUID const & expected, GUID const & actual) 
        {
            return &expected == &actual;
        }

        static bool IsLessThan(GUID const & expectedLess, GUID const & expectedGreater) 
        {
            return ::memcmp(&expectedLess, &expectedGreater, sizeof(GUID)) < 0;
        }

        static bool IsGreaterThan(GUID const & expectedGreater, GUID const & expectedLess) 
        {
            return ::memcmp(&expectedGreater, &expectedLess, sizeof(GUID)) > 0;
        }

        static bool IsNull(GUID const & object) 
        {
            // GUID_NULL is defined as a GUID with all zeros
            const GUID c_guidNull = { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

            return ::memcmp(&object, &c_guidNull, sizeof(GUID)) == 0;
        }
    };
#endif // TAEF_USE_CUSTOM_GUID_VERIFY && GUID_DEFINED

    class ComVerify;
    class Verify;
    namespace Private
    {
        class MacroVerify;

#if defined(__cpp_char8_t)
        using TaefChar8_t = char8_t;
#else
        using TaefChar8_t = char;
#endif

        inline void Memcpy(
            _Out_writes_bytes_to_(destSize, count) void* pDest, size_t destSize,
            _In_reads_bytes_(count) const void* pSrc, size_t count)
        {
            // Internal details: One existing test had a linker error when we added a memcpy dependency.
            // We updated it to define TAEF_NO_MEMCPY to activate a manual implementation of the API.
#if defined(TAEF_NO_MEMCPY)
            char* pDestAsChar = static_cast<char*>(pDest);
            const char* pSrcAsChar = static_cast<const char*>(pSrc);
            for (size_t i = 0; i < count; ++i)
            {
                pDestAsChar[i] = pSrcAsChar[i];
            }
#elif defined(__STDC_WANT_SECURE_LIB__) && __STDC_WANT_SECURE_LIB__
            ::memcpy_s(pDest, destSize, pSrc, count);
#else
            ::memcpy(pDest, pSrc, count);
#endif
        }

        // This is used to store string data for cases where TAEF needs to work with exceptions and the STL disabled.
        // Do not use this directly. It is an internal implementation detail of TAEF and is subject to change.
        class StringStorage
        {
        public:
            StringStorage() = default;
            explicit StringStorage(const char* pszString)
            {
                Set(pszString);
            }
            ~StringStorage() noexcept
            {
                Clear();
            }
            StringStorage(const StringStorage&) = delete;
            StringStorage& operator=(const StringStorage&) = delete;

            void Set(const char* pszString) noexcept
            {
                const size_t stringLength = ::strlen(pszString);
                if (TryAllocate(stringLength))
                {
                    Memcpy(m_pData, m_allocationSize, pszString, stringLength + 1);
                }
            }

            bool TryAllocate(size_t stringLength) noexcept
            {
                const size_t allocationSize = sizeof(TaefChar8_t) * (stringLength + 1);
                m_pData = AllocateString(allocationSize);
                if (m_pData)
                {
                    m_allocationSize = allocationSize;
                    return true;
                }
                return false;
            }

            char* Data() const noexcept { return reinterpret_cast<char*>(m_pData); }
            TaefChar8_t* DataAsChar8T() const noexcept { return m_pData; }
            size_t AllocationSize() const noexcept { return m_allocationSize; }

            char* Detach() noexcept
            {
                char* const pData = Data();
                m_pData = nullptr;
                m_allocationSize = 0;
                return pData;
            }

            static void TAEF_CDECL TaefAbiStringDeallocator(_In_ _Post_invalid_ char* pStr)
            {
                FreeString(pStr);
            }

        private:
            void Clear() noexcept
            {
                if (m_pData)
                {
                    FreeString(m_pData);
                    m_pData = nullptr;
                    m_allocationSize = 0;
                }
            }

            static TaefChar8_t* AllocateString(size_t allocationSize) noexcept
            {
#if defined(__cpp_exceptions)
                try
                {
#endif
                    // Ideally we would use a std::nothrow here to use a non-throwing new, but we are very restricted in what we can
                    // do while still having all existing TAEF tests be able to compile. Using raw sized operator new is the most portable
                    // across all the different configurations of TAEF tests.
                    return static_cast<TaefChar8_t*>(::operator new(allocationSize));
#if defined(__cpp_exceptions)
                }
                catch (const std::exception&)
                {
                    return nullptr;
                }
#endif
            }
            static void FreeString(_In_ _Post_invalid_ void* pszString) noexcept
            {
                // This corresponds with the use of ::operator new(size_t) to allocate the string.
                ::operator delete(pszString);
            }

            TaefChar8_t* m_pData = nullptr;
            size_t m_allocationSize = 0;
        };
    }/* namespace Private */

#if defined(_MSC_VER)
# pragma warning(push)
// Disable warnings related to implicitly deleted special member functions.
# pragma warning(disable: 4623 4625 4626)
#endif
#if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)
    // Thrown when a Verify method fails
    class VerifyFailureException
        // TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE is always defined on Linux, but it is off-by-default on Windows for compatibility.
        // This options will likely be removed in the future once WEX::Common::Exception is updated to itself inherit from std::exception.
# if defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
        : public std::exception
# else
        : public WEX::Common::Exception
# endif
    {
    public:
# if defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
        HRESULT ErrorCode() const noexcept
        {
            return m_errorCode;
        }

        const char* what() const noexcept override
        {
            return m_message.Data() ? m_message.Data() : "An allocation failure occurred when storing the message for the VerifyFailureException.";
        }
# endif

    private:
        friend class Private::MacroVerify;
        friend class Verify;
        VerifyFailureException(HRESULT errorCode, const char* pszMessage)
# if defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
            : m_errorCode{errorCode}
            , m_message{pszMessage}
# else
            : WEX::Common::Exception{errorCode, pszMessage}
# endif
        {}
# if !defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
        VerifyFailureException(HRESULT errorCode, const wchar_t* pszMessage)
            : WEX::Common::Exception{errorCode, pszMessage}
        {}
# endif

        ~VerifyFailureException() = default;

        // Making 'operator new' private prevents heap allocation of Exception, and forces Exception instances
        // to be thrown by value.
        static void* operator new(size_t) = delete;
        static void operator delete(void*) {}

# if defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE)
        const HRESULT m_errorCode;
        // When exceptions are enabled while STL support is disabled, we include <exception> but not any other STL headers.
        // This means that we can't use std::string here as it won't be available in this scenario.
        const Private::StringStorage m_message;
# endif
    };
#if defined(_MSC_VER)
# pragma warning(pop)
#endif

    namespace Private
    {
        extern "C"
        {
            // One counter per thread across all modules.
            WEXLOGGER_API int WEXLOGGER_STDCALL GetDisabledVerifyThrowCount() noexcept;
            WEXLOGGER_API void WEXLOGGER_STDCALL IncrementDisabledVerifyThrowCount() noexcept;
            WEXLOGGER_API void WEXLOGGER_STDCALL DecrementDisabledVerifyThrowCount() noexcept;
        }
    }

    // Class used to disable VerifyFailureException throwing for its lifetime (on a per-thread basis)
    class DisableVerifyExceptions final
    {
    public:
        DisableVerifyExceptions() noexcept
        {
            Private::IncrementDisabledVerifyThrowCount();
        }

        ~DisableVerifyExceptions() noexcept
        {
            Private::DecrementDisabledVerifyThrowCount();
        }

        int GetDisabledVerifyThrowCount() noexcept
        {
            return Private::GetDisabledVerifyThrowCount();
        }

    private:
        // Making 'operator new' private prevents heap allocation
        static void* operator new(size_t) = delete;
        static void operator delete(void*) = delete;

        DisableVerifyExceptions(const DisableVerifyExceptions&) = delete;
        DisableVerifyExceptions& operator=(const DisableVerifyExceptions&) = delete;
    };
#endif // #if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)

    namespace VerifyOutputSettings
    {
        enum Setting
        {
            None = 0,
            LogOnlyFailures = 0x01,
            LogFailuresAsBlocked = 0x02,
            LogFailuresAsWarnings = 0x04,
            LogValuesOnSuccess = 0x08
        };
    }

    inline VerifyOutputSettings::Setting& operator|=(VerifyOutputSettings::Setting& lhs, const VerifyOutputSettings::Setting& rhs)
    {
        lhs = static_cast<VerifyOutputSettings::Setting>((static_cast<int>(lhs) | static_cast<int>(rhs)));
        return lhs;
    }

    inline VerifyOutputSettings::Setting operator|(VerifyOutputSettings::Setting lhs, const VerifyOutputSettings::Setting& rhs)
    {
        return lhs |= rhs;
    }

    inline int operator&(VerifyOutputSettings::Setting lhs, const VerifyOutputSettings::Setting& rhs)
    {
        return (static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    namespace Private
    {
        extern "C"
        {
            // One setting per thread across all modules.
            WEXLOGGER_API VerifyOutputSettings::Setting WEXLOGGER_STDCALL GetVerifyOutputSettings() noexcept;
            WEXLOGGER_API void WEXLOGGER_STDCALL SetVerifyOutputSettings(VerifyOutputSettings::Setting settings) noexcept;
        }

        inline bool LogValuesOnSuccess() noexcept
        {
#if defined(LOG_VALUES_ON_SUCCESS)
            return true;
#else
            // If LOG_VALUES_ON_SUCCESS wasn't globally defined, look at the current VerifyOutputSettings.
            return ((GetVerifyOutputSettings() & VerifyOutputSettings::LogValuesOnSuccess) != 0);
#endif
        }

        inline bool LogOnlyFailures() noexcept
        {
#if defined(LOG_ONLY_FAILURES)
            return true;
#else
            // If LOG_ONLY_FAILURES wasn't globally defined, look at the current VerifyOutputSettings.
            return ((GetVerifyOutputSettings() & VerifyOutputSettings::LogOnlyFailures) != 0);
#endif
        }
    }

    // This class modifies the verify logging settings for the current thread while it is in scope.
    class SetVerifyOutput final
    {
    public:
        SetVerifyOutput(VerifyOutputSettings::Setting outputSettings) noexcept
            : m_previousSetting(Private::GetVerifyOutputSettings())
        {
            Private::SetVerifyOutputSettings(outputSettings);
        }

        ~SetVerifyOutput() noexcept
        {
            Private::SetVerifyOutputSettings(m_previousSetting);
        }

    private:
        VerifyOutputSettings::Setting m_previousSetting;

        // Making 'operator new' private prevents heap allocation, and forces instances
        // to be thrown by value.
        static void* operator new(size_t) = delete;
        static void operator delete(void*) = delete;

        SetVerifyOutput(const SetVerifyOutput&) = delete;
        SetVerifyOutput& operator=(const SetVerifyOutput&) = delete;
    };

    // When using TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE, you must use the VERIFY macros instead
    // of the Verify class. This is because the VERIFY macros have switched over to UTF-8, but the
    // Verify class still uses UTF-16.
#if defined(_WIN32) && (defined(NO_VERIFY_EXCEPTIONS) || !defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE))
    // Macro for creating an ErrorInfo object with file, function and line information
# define PRIVATE_VERIFY_ERROR_INFO WEX::TestExecution::ErrorInfo(__WFILE__, __WFUNCTION__, __LINE__)

    struct ErrorInfo
    {
        ErrorInfo(const wchar_t* pszFileName, const wchar_t* pszFunctionName, int lineNumber) noexcept
            : pszFile(pszFileName)
            , pszFunction(pszFunctionName)
            , line(lineNumber)
        {
        }

        ErrorInfo(const ErrorInfo& other) noexcept
            : pszFile(other.pszFile)
            , pszFunction(other.pszFunction)
            , line(other.line)
        {
        }

        const wchar_t* pszFile;
        const wchar_t* pszFunction;
        int line;
    };

    const wchar_t c_szVerifyContext[] = L"Verify";
    const wchar_t c_szErrorGeneratingPassMessage[] = L"Error generating pass message; possibly out of memory.";
    const wchar_t c_szErrorGeneratingFailureMessage[] = L"Error generating failure message; possibly out of memory.";

    // Tests should generally use the VERIFY macros. However, this class is an alternative which is still provided for
    // backwards compatibility.
    class Verify final
    {
        friend class WEX::TestExecution::ComVerify;
        friend class WEX::TestExecution::Private::MacroVerify;
    public:
        // Verifies that two specified objects are equal.
        template <typename T1, typename T2>
        static bool AreEqual(const T1& expected, const T2& actual, const ErrorInfo& errorInfo)
        {
            return AreEqual<T1, T2>(expected, actual, nullptr, errorInfo);
        }

        // Verifies that two specified objects are equal.
        template <typename T1, typename T2>
        static bool AreEqual(const T1& expected, const T2& actual, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"AreEqual", pszMessage);
            return AreEqualImpl<T1, T2>(expected, actual, errorInfo, buildMessage);
        }

        // Verifies that two specified objects are not equal.
        template <typename T1, typename T2>
        static bool AreNotEqual(const T1& expected, const T2& actual, const ErrorInfo& errorInfo)
        {
            return AreNotEqual<T1, T2>(expected, actual, nullptr, errorInfo);
        }

        // Verifies that two specified objects are not equal.
        template <typename T1, typename T2>
        static bool AreNotEqual(const T1& expected, const T2& actual, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"AreNotEqual", pszMessage);
            return AreNotEqualImpl<T1, T2>(expected, actual, errorInfo, buildMessage);
        }

        // Verifies that the first parameter is less than the second parameter.
        template <typename T1, typename T2>
        static bool IsLessThan(const T1& expectedLess, const T2& expectedGreater, const ErrorInfo& errorInfo)
        {
            return IsLessThan<T1, T2>(expectedLess, expectedGreater, nullptr, errorInfo);
        }

        // Verifies that the first parameter is less than the second parameter.
        template <typename T1, typename T2>
        static bool IsLessThan(const T1& expectedLess, const T2& expectedGreater, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsLessThan", pszMessage);
            return IsLessThanImpl<T1, T2>(expectedLess, expectedGreater, errorInfo, buildMessage);
        }

        // Verifies that the first parameter is less than or equal to the second parameter.
        template <typename T1, typename T2>
        static bool IsLessThanOrEqual(const T1& expectedLess, const T2& expectedGreater, const ErrorInfo& errorInfo)
        {
            return IsLessThanOrEqual<T1, T2>(expectedLess, expectedGreater, nullptr, errorInfo);
        }

        // Verifies that the first parameter is less than or equal to the second parameter.
        template <typename T1, typename T2>
        static bool IsLessThanOrEqual(const T1& expectedLess, const T2& expectedGreater, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsLessThanOrEqual", pszMessage);
            return IsLessThanOrEqualImpl<T1, T2>(expectedLess, expectedGreater, errorInfo, buildMessage);
        }

        // Verifies that the first parameter is greater than the second parameter.
        template <typename T1, typename T2>
        static bool IsGreaterThan(const T1& expectedGreater, const T2& expectedLess, const ErrorInfo& errorInfo)
        {
            return IsGreaterThan<T1, T2>(expectedGreater, expectedLess, nullptr, errorInfo);
        }

        // Verifies that the first parameter is greater than the second parameter.
        template <typename T1, typename T2>
        static bool IsGreaterThan(const T1& expectedGreater, const T2& expectedLess, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsGreaterThan", pszMessage);
            return IsGreaterThanImpl<T1, T2>(expectedGreater, expectedLess, errorInfo, buildMessage);
        }

        // Verifies that the first parameter is greater than or equal to the second parameter.
        template <typename T1, typename T2>
        static bool IsGreaterThanOrEqual(const T1& expectedGreater, const T2& expectedLess, const ErrorInfo& errorInfo)
        {
            return IsGreaterThanOrEqual<T1, T2>(expectedGreater, expectedLess, nullptr, errorInfo);
        }

        // Verifies that the first parameter is greater than or equal to the second parameter.
        template <typename T1, typename T2>
        static bool IsGreaterThanOrEqual(const T1& expectedGreater, const T2& expectedLess, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsGreaterThanOrEqual", pszMessage);
            return IsGreaterThanOrEqualImpl<T1, T2>(expectedGreater, expectedLess, errorInfo, buildMessage);
        }

    private:
        // Returns the address of T
        template <typename T>
        class GetAddressOf
        {
        public:
            static const T* Value(const T& ref)
            {
                return &ref;
            }
            typedef const T* Type;
        };

        // Returns the address of T
        template <typename T>
        class GetAddressOf<T&>
        {
        public:
            static const T* Value(const T& ref)
            {
                return &ref;
            }
            typedef const T* Type;
        };

        // Returns the address T* (a pointer-to-pointer) as void*
        template <typename T>
        class GetAddressOf<T*>
        {
        public:
            static const void* Value(T* const& p)
            {
                return &p;
            }
            typedef const void* Type;
        };

    public:
        // Verifies that the two parameters specified refer to the same object.
        template <typename T1, typename T2>
        static bool AreSame(const T1& expected, const T2& actual, const ErrorInfo& errorInfo)
        {
            return AreSame<T1, T2>(expected, actual, nullptr, errorInfo);
        }

        // Verifies that the two parameters specified refer to the same object.
        template <typename T1, typename T2>
        static bool AreSame(const T1& expected, const T2& actual, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"AreSame", pszMessage);
            return AreSameImpl<T1, T2>(expected, actual, errorInfo, buildMessage);
        }

        // Verifies that the two parameters specified do not refer to the same object.
        template <typename T1, typename T2>
        static bool AreNotSame(const T1& expected, const T2& actual, const ErrorInfo& errorInfo)
        {
            return AreNotSame<T1, T2>(expected, actual, nullptr, errorInfo);
        }

        // Verifies that the two parameters specified do not refer to the same object.
        template <typename T1, typename T2>
        static bool AreNotSame(const T1& expected, const T2& actual, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"AreNotSame", pszMessage);
            return AreNotSameImpl<T1, T2>(expected, actual, errorInfo, buildMessage);
        }

        // Fails without checking any conditions.
        _Post_equal_to_(false)
        static bool Fail(const ErrorInfo& errorInfo)
        {
            return Fail(nullptr, errorInfo);
        }

        // Fails without checking any conditions.
        _Post_equal_to_(false)
        static bool Fail(const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            if (WEX::Common::NoThrowString::IsNullOrEmpty(pszMessage))
            {
                return VerificationFailed([]()->WEX::Common::NoThrowString { return L"Fail"; }, errorInfo);
            }

            return VerificationFailed([&]()->WEX::Common::NoThrowString { return pszMessage; }, errorInfo);
        }

        // Verifies that the specified condition is true.
        _Post_equal_to_(condition)
        static bool IsTrue(bool condition, const ErrorInfo& errorInfo)
        {
            return IsTrue(condition, nullptr, errorInfo);
        }

        // Verifies that the specified condition is true.
        _Post_equal_to_(condition)
        static bool IsTrue(bool condition, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsTrue", pszMessage);
            return IsTrueImpl(condition, errorInfo, buildMessage);
        }

        // Verifies that the specified condition is false.
        _Post_equal_to_(!condition)
        static bool IsFalse(bool condition, const ErrorInfo& errorInfo)
        {
            return IsFalse(condition, nullptr, errorInfo);
        }

        // Verifies that the specified condition is false.
        _Post_equal_to_(!condition)
        static bool IsFalse(bool condition, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsFalse", pszMessage);
            return IsFalseImpl(condition, errorInfo, buildMessage);
        }

        // Verifies that the specified parameter is null.
        template <typename T>
        static bool IsNull(const T& object, const ErrorInfo& errorInfo)
        {
            return IsNull(object, nullptr, errorInfo);
        }

        // Verifies that the specified parameter is null.
        template <typename T>
        static bool IsNull(const T& object, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsNull", pszMessage);
            return IsNullImpl(object, errorInfo, buildMessage);
        }

        // Verifies that the specified parameter is not null.
        template <typename T>
        static bool IsNotNull(const T& object, const ErrorInfo& errorInfo)
        {
            return IsNotNull(object, nullptr, errorInfo);
        }

        // Verifies that the specified parameter is not null.
        template <typename T>
        static bool IsNotNull(const T& object, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"IsNotNull", pszMessage);
            return IsNotNullImpl(object, errorInfo, buildMessage);
        }

        // Verifies that the specified HRESULT is successful.
        _Post_equal_to_(SUCCEEDED(hr))
        static bool Succeeded(HRESULT hr, const ErrorInfo& errorInfo)
        {
            return Succeeded(hr, nullptr, errorInfo);
        }

        // Verifies that the specified HRESULT is successful.
        _Post_equal_to_(SUCCEEDED(hr))
        static bool Succeeded(HRESULT hr, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Succeeded", pszMessage);
            return SucceededImpl(hr, errorInfo, buildMessage);
        }

        // Verifies that the specified HRESULT is not successful.
        _Post_equal_to_(FAILED(hr))
        static bool Failed(HRESULT hr, const ErrorInfo& errorInfo)
        {
            return Failed(hr, nullptr, errorInfo);
        }

        // Verifies that the specified HRESULT is not successful.
        _Post_equal_to_(FAILED(hr))
        static bool Failed(HRESULT hr, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Failed", pszMessage);
            return FailedImpl(hr, errorInfo, buildMessage);
        }

        // Verifies that the specified Win32 error code is successful.
        _Post_equal_to_(win32ErrorCode == 0)
        static bool Win32Succeeded(::LONG win32ErrorCode, const ErrorInfo& errorInfo)
        {
            return Win32Succeeded(win32ErrorCode, nullptr, errorInfo);
        }

        // Verifies that the specified Win32 error code is successful.
        _Post_equal_to_(win32ErrorCode == 0)
        static bool Win32Succeeded(::LONG win32ErrorCode, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Win32Succeeded", pszMessage);
            return Win32SucceededImpl(win32ErrorCode, errorInfo, buildMessage);
        }

        // Verifies that the specified Win32 error code is not successful.
        _Post_equal_to_(win32ErrorCode != 0)
        static bool Win32Failed(::LONG win32ErrorCode, const ErrorInfo& errorInfo)
        {
            return Win32Failed(win32ErrorCode, nullptr, errorInfo);
        }

        // Verifies that the specified Win32 error code is not successful.
        _Post_equal_to_(win32ErrorCode != 0)
        static bool Win32Failed(::LONG win32ErrorCode, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Win32Failed", pszMessage);
            return Win32FailedImpl(win32ErrorCode, errorInfo, buildMessage);
        }

        // Verifies that the specified Win32 BOOL succeeded, and logs the error code returned from GetLastError if not.
        _Post_equal_to_(win32Bool != 0)
        static bool Win32BoolSucceeded(::BOOL win32Bool, const ErrorInfo& errorInfo)
        {
            return Win32BoolSucceeded(win32Bool, nullptr, errorInfo);
        }

        // Verifies that the specified Win32 BOOL succeeded, and logs the error code returned from GetLastError if not.
        _Post_equal_to_(win32Bool != 0)
        static bool Win32BoolSucceeded(::BOOL win32Bool, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Win32BoolSucceeded", pszMessage);
            return Win32BoolSucceededImpl(win32Bool, errorInfo, buildMessage);
        }

        // Verifies that the specified Win32 error code is not successful.
        _Post_equal_to_(win32Bool == 0)
        static bool Win32BoolFailed(::BOOL win32Bool, const ErrorInfo& errorInfo)
        {
            return Win32BoolFailed(win32Bool, nullptr, errorInfo);
        }

        // Verifies that the specified Win32 error code is not successful.
        _Post_equal_to_(win32Bool == 0)
        static bool Win32BoolFailed(::BOOL win32Bool, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"Win32BoolFailed", pszMessage);
            return Win32BoolFailedImpl(win32Bool, errorInfo, buildMessage);
        }

# if defined(NT_SUCCESS)
        // Verifies that the specified NTSTATUS error code is successful.
        _Post_equal_to_(NT_SUCCESS(ntStatus))
        static bool NtSuccess(NTSTATUS ntStatus, const ErrorInfo& errorInfo)
        {
            return NtSuccess(ntStatus, nullptr, errorInfo);
        }

        // Verifies that the specified NTSTATUS error code is successful.
        _Post_equal_to_(NT_SUCCESS(ntStatus))
        static bool NtSuccess(NTSTATUS ntStatus, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"NtSuccess", pszMessage);
            return NtSuccessImpl(ntStatus, errorInfo, buildMessage);
        }

        // Verifies that the specified NTSTATUS error code is not successful.
        _Post_equal_to_(!NT_SUCCESS(ntStatus))
        static bool NotNtSuccess(NTSTATUS ntStatus, const ErrorInfo& errorInfo)
        {
            return NotNtSuccess(ntStatus, nullptr, errorInfo);
        }

        // Verifies that the specified NTSTATUS error code is not successful.
        _Post_equal_to_(!NT_SUCCESS(ntStatus))
        static bool NotNtSuccess(NTSTATUS ntStatus, const wchar_t* pszMessage, const ErrorInfo& errorInfo)
        {
            VerifyMessageFunctor buildMessage(L"NotNtSuccess", pszMessage);
            return NotNtSuccessImpl(ntStatus, errorInfo, buildMessage);
        }
# endif // #if defined(NT_SUCCESS)

    private:
        template <typename T>
        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const T& value)
        {
            WEX::Common::NoThrowString valueString(VerifyOutputTraits<T>::ToString(value));
            if (!valueString.IsEmpty())
            {
                message.Append(valueString);
                return true;
            }
            return false;
        }

        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const wchar_t* pszValue)
        {
            message.Append(pszValue);
            return true;
        }

        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const char* paszValue)
        {
            message.Append(WEX::Common::NoThrowString(paszValue));
            return true;
        }

        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const WEX::Common::String& value)
        {
            message.Append(static_cast<const wchar_t*>(value));
            return true;
        }

        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const WEX::Common::NoThrowString& value)
        {
            message.Append(value);
            return true;
        }

# if defined(TAEF_STL_SUPPORT)
        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const std::wstring& value)
        {
            message.Append(value.c_str());
            return true;
        }

        static bool AppendValueToMessage(WEX::Common::NoThrowString& message, const std::string& value)
        {
            message.Append(WEX::Common::NoThrowString(value.c_str()));
            return true;
        }
# endif // #if defined(TAEF_STL_SUPPORT)

        template <typename T1, typename T2>
        static void AddValuesListToMessage(WEX::Common::NoThrowString& message, const T1& expected, const T2& actual)
        {
            const int lengthWithoutValues = message.GetLength();

            message.Append(L" - Values (");
            if (!AppendValueToMessage(message, expected))
            {
                // Roll back the append of " - Values (".
                message.GetBufferSetLength(lengthWithoutValues);
                message.ReleaseBuffer(lengthWithoutValues);
                return;
            }
            message.Append(L", ");
            AppendValueToMessage(message, actual);
            message.Append(L")");
        }

        template <typename T>
        static void AddValuesListToMessage(WEX::Common::NoThrowString& message, const T& expected)
        {
            const int lengthWithoutValue = message.GetLength();

            message.Append(L" - Value (");
            if (!AppendValueToMessage(message, expected))
            {
                // Roll back the append of " - Value (".
                message.GetBufferSetLength(lengthWithoutValue);
                message.ReleaseBuffer(lengthWithoutValue);
                return;
            }
            message.Append(L")");
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool VerificationFailed(const T1& expected, const T2& actual, const TBuildMessage& buildMessage, const ErrorInfo& errorInfo)
        {
            WEX::Common::PreserveLastError preserveLastError;
            WEX::Common::NoThrowString message{ buildMessage() };
            AddValuesListToMessage(message, expected, actual);
            return FailImpl(message, errorInfo, TAEF_E_FAIL /*Work around the compiler issue on the woa build*/);
        }

        template <typename T, typename TBuildMessage>
        static bool VerificationFailed(const T& expected, const TBuildMessage& buildMessage, const ErrorInfo& errorInfo)
        {
            WEX::Common::PreserveLastError preserveLastError;
            WEX::Common::NoThrowString message{ buildMessage() };
            AddValuesListToMessage(message, expected);
            return FailImpl(message, errorInfo, TAEF_E_FAIL /*Work around the compiler issue on the woa build*/);
        }

        template <typename TBuildMessage>
        _Post_equal_to_(false)
        static bool VerificationFailed(const TBuildMessage& buildMessage, const ErrorInfo& errorInfo)
        {
            WEX::Common::PreserveLastError preserveLastError;
            return FailImpl(buildMessage(), errorInfo);
        }

        _Post_equal_to_(false)
        static bool FailImpl(const WEX::Common::NoThrowString& message, const ErrorInfo& errorInfo, HRESULT hrToThrow = TAEF_E_FAIL)
        {
            if (!message.IsEmpty())
            {
                VerifyOutputSettings::Setting outputSetting = Private::GetVerifyOutputSettings();

                if (outputSetting & VerifyOutputSettings::LogFailuresAsBlocked)
                {
                    WEX::Logging::Log::Result(WEX::Logging::TestResults::Blocked,
                        message.ToCStrWithFallbackTo(c_szErrorGeneratingFailureMessage), c_szVerifyContext);
                }
                else if (outputSetting & VerifyOutputSettings::LogFailuresAsWarnings)
                {
                    WEX::Logging::Log::Warning(message.ToCStrWithFallbackTo(c_szErrorGeneratingFailureMessage), c_szVerifyContext,
                        errorInfo.pszFile, errorInfo.pszFunction, errorInfo.line);
                }
                else
                {
                    WEX::Logging::Log::Error(message.ToCStrWithFallbackTo(c_szErrorGeneratingFailureMessage), c_szVerifyContext,
                        errorInfo.pszFile, errorInfo.pszFunction, errorInfo.line);
                }
            }

# if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)
            if (Private::GetDisabledVerifyThrowCount() == 0)
            {
                throw VerifyFailureException(hrToThrow, message);
            }
# else
            UNREFERENCED_PARAMETER(hrToThrow); // Unreferenced if exceptions are disabled
# endif

            return false;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        _Post_equal_to_(true)
        static bool VerificationPassed(const T1& expected, const T2& actual, const TBuildMessage& buildMessage)
        {
            if (Private::LogOnlyFailures())
            {
                return true;
            }

            WEX::Common::PreserveLastError preserveLastError;
            WEX::Common::NoThrowString message{ buildMessage() };
            if (Private::LogValuesOnSuccess())
            {
                AddValuesListToMessage(message, expected, actual);
            }

            return PassImpl(message);
        }

        template <typename T, typename TBuildMessage>
        _Post_equal_to_(true)
        static bool VerificationPassed(const T& expected, const TBuildMessage& buildMessage)
        {
            if (Private::LogOnlyFailures())
            {
                return true;
            }

            WEX::Common::PreserveLastError preserveLastError;
            WEX::Common::NoThrowString message{ buildMessage() };
            if (Private::LogValuesOnSuccess())
            {
                AddValuesListToMessage(message, expected);
            }

            return PassImpl(message);
        }

        template <typename TBuildMessage>
        _Post_equal_to_(true)
        static bool VerificationPassed(const TBuildMessage& buildMessage)
        {
            if (Private::LogOnlyFailures())
            {
                return true;
            }

            WEX::Common::PreserveLastError preserveLastError;
            return PassImpl(buildMessage());
        }

        _Post_equal_to_(true)
        static bool PassImpl(const WEX::Common::NoThrowString& message)
        {
            if (!message.IsEmpty())
            {
                WEX::Logging::Log::Comment(message.ToCStrWithFallbackTo(c_szErrorGeneratingPassMessage), c_szVerifyContext);
            }

            return true;
        }

        class VerifyMessageFunctor
        {
        public:
            VerifyMessageFunctor(const wchar_t* pszDefaultMessage, const wchar_t* pszMessage)
            : m_pszDefaultMessage(pszDefaultMessage)
            , m_pszMessage(pszMessage)
            {
            }

            WEX::Common::NoThrowString operator()() const
            {
                if (WEX::Common::NoThrowString::IsNullOrEmpty(m_pszMessage))
                {
                    return WEX::Common::NoThrowString(m_pszDefaultMessage);
                }

                return WEX::Common::NoThrowString().Format(L"%s: %s", m_pszDefaultMessage, m_pszMessage);
            }

        private:
            const wchar_t* m_pszDefaultMessage;
            const wchar_t* m_pszMessage;
        };

        template <typename T1, typename T2, typename TBuildMessage>
        static bool AreEqualImpl(const T1& expected, const T2& actual, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::AreEqual(expected, actual))
            {
                return VerificationFailed(expected, actual, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expected, actual, buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool AreNotEqualImpl(const T1& expected, const T2& actual, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (VerifyCompareTraits<T1, T2>::AreEqual(expected, actual))
            {
                return VerificationFailed(expected, actual, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expected, actual, buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool IsLessThanImpl(const T1& expectedLess, const T2& expectedGreater, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::IsLessThan(expectedLess, expectedGreater))
            {
                return VerificationFailed(expectedLess, expectedGreater, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expectedLess, expectedGreater, buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool IsLessThanOrEqualImpl(const T1& expectedLess, const T2& expectedGreater,
            const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::IsLessThan(expectedLess, expectedGreater) &&
                !VerifyCompareTraits<T1, T2>::AreEqual(expectedLess, expectedGreater))
            {
                return VerificationFailed(expectedLess, expectedGreater, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expectedLess, expectedGreater, buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool IsGreaterThanImpl(const T1& expectedGreater, const T2& expectedLess, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::IsGreaterThan(expectedGreater, expectedLess))
            {
                return VerificationFailed(expectedGreater, expectedLess, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expectedGreater, expectedLess, buildMessage);
            }

            return true;
        }


        template <typename T1, typename T2, typename TBuildMessage>
        static bool IsGreaterThanOrEqualImpl(const T1& expectedGreater, const T2& expectedLess,
            const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::IsGreaterThan(expectedGreater, expectedLess) &&
                !VerifyCompareTraits<T1, T2>::AreEqual(expectedGreater, expectedLess))
            {
                return VerificationFailed(expectedGreater, expectedLess, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(expectedGreater, expectedLess, buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool AreSameImpl(const T1& expected, const T2& actual, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T1, T2>::AreSame(expected, actual))
            {
                return VerificationFailed<GetAddressOf<T1>::Type, GetAddressOf<T2>::Type>(
                    GetAddressOf<T1>::Value(expected), GetAddressOf<T2>::Value(actual), buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<GetAddressOf<T1>::Type, GetAddressOf<T2>::Type>(
                    GetAddressOf<T1>::Value(expected), GetAddressOf<T2>::Value(actual), buildMessage);
            }

            return true;
        }

        template <typename T1, typename T2, typename TBuildMessage>
        static bool AreNotSameImpl(const T1& expected, const T2& actual, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (VerifyCompareTraits<T1, T2>::AreSame(expected, actual))
            {
                return VerificationFailed<GetAddressOf<T1>::Type, GetAddressOf<T2>::Type>(
                    GetAddressOf<T1>::Value(expected), GetAddressOf<T2>::Value(actual), buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<GetAddressOf<T1>::Type, GetAddressOf<T2>::Type>(
                    GetAddressOf<T1>::Value(expected), GetAddressOf<T2>::Value(actual), buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        static bool IsTrueImpl(bool condition, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!condition)
            {
                return VerificationFailed(buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        static bool IsFalseImpl(bool condition, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (condition)
            {
                return VerificationFailed(buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed(buildMessage);
            }

            return true;
        }

        template <typename T, typename TBuildMessage>
        static bool IsNullImpl(const T& object, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (VerifyCompareTraits<T, T>::IsNull(object))
            {
                if (!Private::LogOnlyFailures())
                {
                    return VerificationPassed(object, buildMessage);
                }

                return true;
            }

            return VerificationFailed(object, buildMessage, errorInfo);
        }

        // Verifies that the specified parameter is not null.
        template <typename T, typename TBuildMessage>
        static bool IsNotNullImpl(const T& object, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!VerifyCompareTraits<T, T>::IsNull(object))
            {
                if (!Private::LogOnlyFailures())
                {
                    return VerificationPassed(object, buildMessage);
                }

                return true;
            }

            return VerificationFailed(object, buildMessage, errorInfo);
        }

        template <typename TBuildMessage>
        _Post_equal_to_(SUCCEEDED(hr))
        static bool SucceededImpl(HRESULT hr, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (FAILED(hr))
            {
                return VerificationFailed<HRESULT>(hr, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<HRESULT>(hr, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(FAILED(hr))
        static bool FailedImpl(HRESULT hr, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!FAILED(hr))
            {
                return VerificationFailed<HRESULT>(hr, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<HRESULT>(hr, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(win32ErrorCode == 0)
        static bool Win32SucceededImpl(::LONG win32ErrorCode, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (win32ErrorCode != ERROR_SUCCESS)
            {
                return VerificationFailed< ::LONG>(win32ErrorCode, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed< ::LONG>(win32ErrorCode, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(win32ErrorCode != 0)
        static bool Win32FailedImpl(::LONG win32ErrorCode, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (win32ErrorCode == ERROR_SUCCESS)
            {
                return VerificationFailed< ::LONG>(win32ErrorCode, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed< ::LONG>(win32ErrorCode, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(win32Bool != 0)
        static bool Win32BoolSucceededImpl(::BOOL win32Bool, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (0 == win32Bool)
            {
                // Log the value returned from Debug::GetLastError()
                return VerificationFailed< ::DWORD>(WEX::Common::Debug::GetLastError(), buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed< ::BOOL>(win32Bool, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(win32Bool == 0)
        static bool Win32BoolFailedImpl(::BOOL win32Bool, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (0 != win32Bool)
            {
                return VerificationFailed< ::BOOL>(win32Bool, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed< ::BOOL>(win32Bool, buildMessage);
            }

            return true;
        }

# if defined(NT_SUCCESS)
        template <typename TBuildMessage>
        _Post_equal_to_(NT_SUCCESS(ntStatus))
        static bool NtSuccessImpl(NTSTATUS ntStatus, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (!NT_SUCCESS(ntStatus))
            {
                return VerificationFailed<NTSTATUS>(ntStatus, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<NTSTATUS>(ntStatus, buildMessage);
            }

            return true;
        }

        template <typename TBuildMessage>
        _Post_equal_to_(!NT_SUCCESS(ntStatus))
        static bool NotNtSuccessImpl(NTSTATUS ntStatus, const ErrorInfo& errorInfo, const TBuildMessage& buildMessage)
        {
            if (NT_SUCCESS(ntStatus))
            {
                return VerificationFailed<NTSTATUS>(ntStatus, buildMessage, errorInfo);
            }

            if (!Private::LogOnlyFailures())
            {
                return VerificationPassed<NTSTATUS>(ntStatus, buildMessage);
            }

            return true;
        }
# endif // #if defined(NT_SUCCESS)

        // Disallow construction of static class
        Verify() = delete;
        ~Verify() = delete;
        Verify(const Verify&) = delete;
        Verify& operator=(const Verify&) = delete;
    };
#endif // #if defined(_WIN32) && (defined(NO_VERIFY_EXCEPTIONS) || !defined(TAEF_USE_STD_EXCEPTION_FOR_VERIFY_FAILURE))

    namespace Private
    {
#if defined(_MSC_VER)
# pragma warning(push)
        // Disable warnings related to implicitly deleted special member functions.
# pragma warning(disable: 4623 4625 4626)
#endif
        class Utf8StringView
        {
        public:
            template <size_t LiteralSize>
            Utf8StringView(const TaefChar8_t(&szLiteral)[LiteralSize]) noexcept
                : m_pData{szLiteral}
                // Subtract off the null terminator.
                , m_size{LiteralSize - 1}
            {}

            Utf8StringView(const TaefChar8_t* pszString) noexcept
                : m_pData{pszString}
                , m_size{::strlen(reinterpret_cast<const char*>(pszString))}
            {}

            const TaefChar8_t* Data() const noexcept { return m_pData; }
            size_t Size() const noexcept { return m_size; }

        private:
            const TaefChar8_t* const m_pData;
            const size_t m_size;
        };

        class StringWriter
        {
        public:
            StringWriter(char* pData, size_t size) noexcept
                : m_pData{pData}
                , m_allocationSize{size}
            {}

            void Append(const TaefChar8_t* pszValue, size_t length) noexcept
            {
                Memcpy(m_pData + m_initializedCharCount, m_allocationSize - m_initializedCharCount, pszValue, length);
                m_initializedCharCount += length;
            }
            void Append(Utf8StringView utf8StringView) noexcept
            {
                Append(utf8StringView.Data(), utf8StringView.Size());
            }

            void AppendChar(char charToAppend) noexcept
            {
                m_pData[m_initializedCharCount] = charToAppend;
                ++m_initializedCharCount;
            }

            // Appends " [HRESULT: 0x%x]".
            // 14 characters for " [HRESULT: 0x]" plus 8 for the number in hex.
            static constexpr size_t c_maxStringSizeForHResult = 14 + 8;
            void AppendHResult(HRESULT hr) noexcept
            {
                Append(Utf8StringView{u8" [HRESULT: 0x"});
                constexpr size_t c_hexDigitsInHResult = 8;
                for (size_t i = c_hexDigitsInHResult; i > 0; --i)
                {
                    unsigned char valueForChar = static_cast<unsigned char>(hr & 0xf);
                    m_pData[m_initializedCharCount + i - 1] = valueForChar < 10 ? valueForChar + '0' : valueForChar - 10 + 'a';
                    hr >>= 4;
                }
                Advance(c_hexDigitsInHResult);
                AppendChar(']');
            }

            // Appends " [Error code: %d]".
            // 15 characters for " [Error code: ]" plus 11 for the longest possible int (-1234567890).
            static constexpr size_t c_maxStringSizeForUnrecognizedErrorCode = 15 + 11;
            void AppendUnrecognizedErrorCode(int errorCode) noexcept
            {
                Append(Utf8StringView{u8" [Error code: "});
                // This is similar to std::to_chars from <charconv>, but not all tests have access to that.
                unsigned int unsignedErrorCode = static_cast<unsigned int>(errorCode);
                if (errorCode < 0)
                {
                    AppendChar('-');
                    unsignedErrorCode = static_cast<unsigned int>(0-unsignedErrorCode);
                }

                static_assert(sizeof(unsignedErrorCode) == 4, "Verify.h currently depends on unsigned ints being four bytes.");
                constexpr size_t c_maxStringSizeForUnsignedInt = 10;
                TaefChar8_t buffer[c_maxStringSizeForUnsignedInt];
                TaefChar8_t* pEndOfBuffer = buffer + c_maxStringSizeForUnsignedInt;
                TaefChar8_t* pStartOfNumber = pEndOfBuffer;
                // Using do instead of while ensures this works when unsignedErrorCode is 0.
                do
                {
                    --pStartOfNumber;
                    *pStartOfNumber = static_cast<TaefChar8_t>(unsignedErrorCode % 10 + u8'0');
                    unsignedErrorCode /= 10;
                } while (unsignedErrorCode > 0);

                Append(pStartOfNumber, static_cast<size_t>(pEndOfBuffer - pStartOfNumber));
                AppendChar(']');
            }

            char* GetWriteBuffer() const noexcept
            {
                return &m_pData[m_initializedCharCount];
            }
            void Advance(size_t count) noexcept
            {
                m_initializedCharCount += count;
            }

            size_t GetInitializedCharCount() const noexcept
            {
                return m_initializedCharCount;
            }
            void SetInitializedCharCount(size_t initializedCharCount) noexcept
            {
                m_initializedCharCount = initializedCharCount;
            }

            void WriteNullTerminator() noexcept
            {
                AppendChar('\0');
            }

        private:
            char* const m_pData;
            const size_t m_allocationSize;
            size_t m_initializedCharCount = 0;
        };
#if defined _MSC_VER
# pragma warning(pop)
#endif

#if defined(TAEF_SYSTEM_ERROR_SUPPORT)
        inline HRESULT GetHResultForStdErrorCode(const std::error_code& errorCode, bool& isExactConvesion)
        {
# if defined(_WIN32)
            if (errorCode.category() == std::system_category())
            {
                // MSVC's STL uses Win32 error codes for the error code when system_category is used.
                // We also allow the use of HRESULT for the error code in this case.

                if (errorCode.value() != 0)
                {
                    isExactConvesion = true;
                    // If the error code is already a failed HRESULT, HRESULT_FROM_WIN32 will leave it unchanged.
                    return HRESULT_FROM_WIN32(static_cast<unsigned long>(errorCode.value()));
                }

                // An errorCode of 0 is not expected for an exception. Fall back to TAEF_E_UNEXPECTED.
            }
# endif

            // If the error category is not system_category, then there is no prescribed
            // way to translate to an HRESULT.
            isExactConvesion = false;
            return TAEF_E_UNEXPECTED;
        }
#endif // if defined(TAEF_SYSTEM_ERROR_SUPPORT)


#if defined(__cpp_exceptions)
# if defined(_WIN32)
        // This returns a message with one of these formats:
        //   "{beginningOfMessage}: {pszExceptionMessage} [HRESULT: 0x{exceptionHResult}]" (if includeHResultInMessage is true)
        //   "{beginningOfMessage}: {pszExceptionMessage}" (if includeHResultInMessage is false)
        // If formatting fails, this returns beginningOfMessage.Data() as a fallback; so beginningOfMessage must be null terminated.
        inline const TaefChar8_t* GetExceptionMessageForExceptionWithHResultAndWideMessage(StringStorage& messageStorage,
            Utf8StringView beginningOfMessage, const wchar_t* pszExceptionMessage, size_t exceptionMessageSize, HRESULT exceptionHResult,
            bool includeHResultInMessage) noexcept
        {
            size_t utf8ByteCountForExceptionMessage = 0;
            if (TAEF_TryGetUtf8ByteCountForWCharStringValue(pszExceptionMessage, exceptionMessageSize, utf8ByteCountForExceptionMessage))
            {
                // beginningOfMessage + ::strlen(": ") + pszExceptionMessage + pExceptionHResult (if includeHResultInMessage is true)
                const size_t totalSize = beginningOfMessage.Size() + 2 + utf8ByteCountForExceptionMessage +
                    (includeHResultInMessage ? StringWriter::c_maxStringSizeForHResult : 0);
                if (messageStorage.TryAllocate(totalSize))
                {
                    StringWriter messageWriter{messageStorage.Data(), messageStorage.AllocationSize()};
                    messageWriter.Append(beginningOfMessage);
                    messageWriter.AppendChar(':');
                    messageWriter.AppendChar(' ');

                    size_t bytesWritten = 0;
                    if (TAEF_TryWriteUtf8ForWCharStringValue(pszExceptionMessage, exceptionMessageSize,
                        messageWriter.GetWriteBuffer(), utf8ByteCountForExceptionMessage, bytesWritten))
                    {
                        messageWriter.Advance(bytesWritten);
                        if (includeHResultInMessage)
                        {
                            messageWriter.AppendHResult(exceptionHResult);
                        }
                        messageWriter.WriteNullTerminator();
                        return messageStorage.DataAsChar8T();
                    }
                }
            }

            return beginningOfMessage.Data();
        }
# endif // #if defined(_WIN32)

        inline const TaefChar8_t* GetExceptionInfo(StringStorage& messageStorage, _Out_opt_ HRESULT* pHResult = nullptr,
            bool includeHResultInMessage = true)
        {
            try
            {
                throw;
            }
# if !defined(NO_VERIFY_EXCEPTIONS)
            catch (const ::WEX::TestExecution::VerifyFailureException&)
            {
                if (pHResult)
                {
                    *pHResult = TAEF_E_FAIL;
                }

                return u8"Caught a VerifyFailureException";
            }
# endif // #if !defined(NO_VERIFY_EXCEPTIONS)
# if defined(_WIN32)
            catch (const WEX::Common::Exception& ex)
            {
                if (pHResult)
                {
                    *pHResult = ex.ErrorCode();
                }

                const wchar_t* const pszExceptionMessage = reinterpret_cast<const wchar_t*>(ex.Message());
                return GetExceptionMessageForExceptionWithHResultAndWideMessage(messageStorage, u8"Caught WEX::Common::Exception",
                    pszExceptionMessage, ::wcslen(pszExceptionMessage), ex.ErrorCode(), includeHResultInMessage);
            }
# endif // #if defined(_WIN32)
# if defined(_WIN32) && defined(__cplusplus_winrt)
            catch (Platform::Exception^ ex)
            {
                if (pHResult)
                {
                    *pHResult = ex->HResult;
                }

                Platform::String^ exceptionMessage = ex->Message;
                // The cast is needed for code compiling with /Zc:wchar_t-. In that case, Platform::String::Data returns "const __wchar_t*".
                return GetExceptionMessageForExceptionWithHResultAndWideMessage(messageStorage, u8"Caught Platform::Exception^",
                    reinterpret_cast<const wchar_t*>(exceptionMessage->Data()), exceptionMessage->Length(), ex->HResult, includeHResultInMessage);
            }
# endif // if defined(_WIN32) && defined(__cplusplus_winrt)
# if defined(_WIN32) && defined(TAEF_CPPWINRT_SUPPORT)
            catch (const winrt::hresult_error& ex)
            {
                if (pHResult)
                {
                    *pHResult = ex.code();
                }

                const winrt::hstring exceptionMessage = ex.message();
                return GetExceptionMessageForExceptionWithHResultAndWideMessage(messageStorage, u8"Caught winrt::hresult_error",
                    exceptionMessage.data(), exceptionMessage.size(), ex.code(), includeHResultInMessage);
            }
# endif // if defined(_WIN32) && defined(TAEF_CPPWINRT_SUPPORT)
# if defined(TAEF_SYSTEM_ERROR_SUPPORT)
            catch (const std::system_error& ex)
            {
                bool isExactConvesion;
                const HRESULT errorCode = GetHResultForStdErrorCode(ex.code(), isExactConvesion);
                if (pHResult)
                {
                    *pHResult = errorCode;
                }

                const char* const pszExceptionMessage = ex.what();
                size_t exceptionMessageLength = ::strlen(pszExceptionMessage);
                // std::system_error often ends its messages with a new-line on Windows. Exclude that from the length if it is present.
                while (exceptionMessageLength > 0 && ::isspace(static_cast<unsigned char>(pszExceptionMessage[exceptionMessageLength - 1])))
                {
                    --exceptionMessageLength;
                }
                if (exceptionMessageLength > INT_MAX)
                {
                    exceptionMessageLength = INT_MAX;
                }

                // This returns a message with one of these formats:
                //   "Caught std::system_error: {pszExceptionMessage} [HRESULT: 0x{errorCode}]" (if isExactConvesion and includeHResultInMessage are true)
                //   "Caught std::system_error: {pszExceptionMessage}" (if isExactConvesion is true and includeHResultInMessage is false)
                //   "Caught std::system_error: {pszExceptionMessage} [Error code: {ex.code().value()}]" (if isExactConvesion is false)
                const Utf8StringView beginningOfMessage{u8"Caught std::system_error: "};

                // beginningOfMessage + pszExceptionMessage + exceptionHResult
                size_t totalSize = beginningOfMessage.Size() + exceptionMessageLength;
                if (!isExactConvesion)
                {
                    totalSize += StringWriter::c_maxStringSizeForUnrecognizedErrorCode;
                }
                else if (includeHResultInMessage)
                {
                    totalSize += StringWriter::c_maxStringSizeForHResult;
                }

                if (messageStorage.TryAllocate(totalSize))
                {
                    StringWriter messageWriter{messageStorage.Data(), messageStorage.AllocationSize()};
                    messageWriter.Append(beginningOfMessage);
                    // Currently we assume std::system_error messages are UTF-8 encoded.
                    messageWriter.Append(reinterpret_cast<const TaefChar8_t*>(pszExceptionMessage), exceptionMessageLength);
                    if (!isExactConvesion)
                    {
                        messageWriter.AppendUnrecognizedErrorCode(ex.code().value());
                    }
                    else if (includeHResultInMessage)
                    {
                        messageWriter.AppendHResult(errorCode);
                    }
                    messageWriter.WriteNullTerminator();
                    return messageStorage.DataAsChar8T();
                }

                return u8"Caught std::system_error";
            }
# endif // if defined(TAEF_SYSTEM_ERROR_SUPPORT)
            catch (const std::exception& ex)
            {
                if (pHResult)
                {
                    *pHResult = TAEF_E_UNEXPECTED;
                }

                // This returns a message with this format:
                //   "Caught std::exception: {ex.what()}"
                const Utf8StringView beginningOfMessage{u8"Caught std::exception: "};

                const char* const pszExceptionMessage = ex.what();
                const size_t exceptionMessageSize = ::strlen(pszExceptionMessage);

                // beginningOfMessage + pszExceptionMessage
                const size_t totalSize = beginningOfMessage.Size() + exceptionMessageSize;
                if (messageStorage.TryAllocate(totalSize))
                {
                    StringWriter messageWriter{messageStorage.Data(), messageStorage.AllocationSize()};
                    messageWriter.Append(beginningOfMessage);
                    // Currently we assume std::exception messages are UTF-8 encoded.
                    messageWriter.Append(reinterpret_cast<const TaefChar8_t*>(pszExceptionMessage), exceptionMessageSize);
                    messageWriter.WriteNullTerminator();
                    return messageStorage.DataAsChar8T();
                }

                return u8"Caught std::exception";
            }
            catch (...)
            {
                if (pHResult)
                {
                    *pHResult = TAEF_E_UNEXPECTED;
                }

                return u8"Caught an unidentified C++ exception";
            }
        }
#endif // #if defined(__cpp_exceptions)

#if !defined(TAEF_USE_STD_FORMAT)
# if defined _MSC_VER
#  pragma warning(push)
// Disable warnings related to implicitly deleted special member functions.
#  pragma warning(disable: 4623 4625 4626)
# endif
        // This would be in the private section of the MacroVerify class, but it needs to be outside the class at namespace scope to avoid
        // build breaks with GCC due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282.
        template <typename T>
        struct ValueOutputHandler;

        template <>
        struct ValueOutputHandler<const char*>
        {
            // On Windows, TAEF treats "char" strings as using the active code page; so we need to convert to UTF-8 for logging.
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForCharStringValue(m_pszString, ::strlen(m_pszString), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForCharStringValue(m_pszString, ::strlen(m_pszString), pOutputBuffer, utf8ByteCount, bytesWritten);
            }

            const char* const m_pszString;
        };
        template <>
        struct ValueOutputHandler<char*> : public ValueOutputHandler<const char*>
        {
            ValueOutputHandler(char* pszString) noexcept
                : ValueOutputHandler<const char*>{pszString}
            {}
        };

# if defined(__cpp_char8_t)
        template <>
        struct ValueOutputHandler<const char8_t*>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                utf8ByteCount = ::strlen(reinterpret_cast<const char*>(m_pszString));
                return true;
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                Memcpy(pOutputBuffer, utf8ByteCount, m_pszString, utf8ByteCount);
                bytesWritten = utf8ByteCount;
                return true;
            }
            const char8_t* const m_pszString;
        };
        template <>
        struct ValueOutputHandler<char8_t*> : public ValueOutputHandler<const char8_t*> {};
# endif // #if defined(__cpp_char8_t)

# if defined(_WIN32)
        template <>
        struct ValueOutputHandler<const wchar_t*>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_pszString, ::wcslen(m_pszString), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_pszString, ::wcslen(m_pszString), pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const wchar_t* const m_pszString;
        };
        template <>
        struct ValueOutputHandler<wchar_t*> : public ValueOutputHandler<const wchar_t*>
        {
            ValueOutputHandler(wchar_t* pszString) noexcept
                : ValueOutputHandler<const wchar_t*>{pszString}
            {}
        };

        template <>
        struct ValueOutputHandler<::WEX::Common::String>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_string, static_cast<size_t>(m_string.GetLength()), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_string, static_cast<size_t>(m_string.GetLength()),
                    pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const ::WEX::Common::String& m_string;
        };

        template <>
        struct ValueOutputHandler<::WEX::Common::NoThrowString>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_string, static_cast<size_t>(m_string.GetLength()), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_string, static_cast<size_t>(m_string.GetLength()),
                    pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const ::WEX::Common::NoThrowString& m_string;
        };
# endif // #if defined(_WIN32)

# if defined(TAEF_STL_SUPPORT)
#  if defined(__cpp_lib_string_view)
        template <>
        struct ValueOutputHandler<std::string_view>
        {
            // On Windows, TAEF treats "char" strings as using the active code page; so we need to convert to UTF-8 for logging.
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForCharStringValue(m_stringView.data(), m_stringView.length(), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForCharStringValue(m_stringView.data(), m_stringView.length(), pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const std::string_view m_stringView;
        };
#  endif // #if defined(__cpp_lib_string_view)
        template <>
        struct ValueOutputHandler<std::string>
        {
            // On Windows, TAEF treats "char" strings as using the active code page; so we need to convert to UTF-8 for logging.
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForCharStringValue(m_string.c_str(), m_string.length(), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForCharStringValue(m_string.c_str(), m_string.length(), pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const std::string& m_string;
        };

#  if defined(__cpp_lib_char8_t)
        template <>
        struct ValueOutputHandler<std::u8string_view>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                utf8ByteCount = m_stringView.length();
                return true;
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                Memcpy(pOutputBuffer, utf8ByteCount, m_stringView.data(), m_stringView.length());
                bytesWritten = m_stringView.length();
                return true;
            }
            const std::u8string_view m_stringView;
        };
        template <>
        struct ValueOutputHandler<std::u8string> : public ValueOutputHandler<std::u8string_view> {};
#  endif // #if defined(__cpp_lib_char8_t)

#  if defined(_WIN32)
#   if defined(__cpp_lib_string_view)
        template <>
        struct ValueOutputHandler<std::wstring_view>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_stringView.data(), m_stringView.length(), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_stringView.data(), m_stringView.length(), pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const std::wstring_view m_stringView;
        };

#   endif // #if defined(__cpp_lib_string_view)
        template <>
        struct ValueOutputHandler<std::wstring>
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_string.c_str(), m_string.length(), utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_string.c_str(), m_string.length(), pOutputBuffer, utf8ByteCount, bytesWritten);
            }
            const std::wstring& m_string;
        };
#  endif // #if defined(_WIN32)
# endif // #if defined(TAEF_STL_SUPPORT)

        // Default implementation:
# if defined(_WIN32)
        template <typename T>
        struct ValueOutputHandler
        {
            ValueOutputHandler(const T& value) noexcept
                : m_formattedValue{VerifyOutputTraits<T>::ToString(value)}
            {}

            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                return TAEF_TryGetUtf8ByteCountForWCharStringValue(m_formattedValue, static_cast<size_t>(m_formattedValue.GetLength()),
                    utf8ByteCount);
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                return TAEF_TryWriteUtf8ForWCharStringValue(m_formattedValue, static_cast<size_t>(m_formattedValue.GetLength()),
                    pOutputBuffer, utf8ByteCount, bytesWritten);
            }

            WEX::Common::NoThrowString m_formattedValue;
        };

        template <>
        struct ValueOutputHandler<VerifyFormatAsHResult> : public ValueOutputHandler<HRESULT>
        {
            ValueOutputHandler(const VerifyFormatAsHResult& value) noexcept
                : ValueOutputHandler<HRESULT>{value.m_hr}
            {}
        };
# else // ^^ _WIN32 ^^ | vv __linux__ vv
        template <typename T>
        struct ValueOutputHandler
        {
            _Success_(return)
                bool TryGetUtf8ByteCountForValue(_Out_ size_t& utf8ByteCount) const noexcept
            {
                utf8ByteCount = 0;
                return true;
            }
            _Success_(return)
                bool TryWriteUtf8ForValue(char* pOutputBuffer, size_t utf8ByteCount, _Out_ size_t& bytesWritten) const noexcept
            {
                bytesWritten = 0;
                return true;
            }

            const T& m_value;
        };
# endif
# if defined _MSC_VER
#  pragma warning(pop)
# endif
#endif // #if !defined(TAEF_USE_STD_FORMAT)

        class MacroVerify final
        {
            class VerifyMessageBuilder;

            template <typename T> struct RemoveVolatile { using Type = T; };
            template <typename T> struct RemoveVolatile<volatile T> { using Type = T; };

            // This is used to determine what template parameters to use.
            // For arrays, it decays the array to a pointer to const T.
            template <typename T> struct GetVerifyType { using Type = T; };
            template <typename T> struct GetVerifyType<T[]> { using Type = const T*; };
            template <typename T, size_t N> struct GetVerifyType<T[N]> { using Type = const T*; };

        public:
            // Disallow construction of static class
            MacroVerify() = delete;
            ~MacroVerify() = delete;
            MacroVerify(const MacroVerify&) = delete;
            MacroVerify& operator=(const MacroVerify&) = delete;

            template <typename T1, typename T2>
            static bool AreEqual(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return AreEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expected, actual,
                    pszExpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool AreEqual(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return AreEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expected, actual,
                    pszExpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateAreEqualMessage(const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("AreEqual({}, {})",
                        reinterpret_cast<const char*>(pszExpectedSourceString), reinterpret_cast<const char*>(pszActualSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool AreEqualImpl(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = VerifyCompareTraits<T1, T2>::AreEqual(expected, actual);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateAreEqualMessage(pszExpectedSourceString, pszActualSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"AreEqual(", pszExpectedSourceString, u8", ", pszActualSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(expected, actual, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(expected, actual, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool AreNotEqual(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return AreNotEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(unexpected, actual,
                    pszUnexpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool AreNotEqual(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return AreNotEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(unexpected, actual,
                    pszUnexpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateAreNotEqualMessage(const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("AreNotEqual({}, {})",
                        reinterpret_cast<const char*>(pszUnexpectedSourceString), reinterpret_cast<const char*>(pszActualSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool AreNotEqualImpl(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = !VerifyCompareTraits<T1, T2>::AreEqual(unexpected, actual);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateAreNotEqualMessage(pszUnexpectedSourceString, pszActualSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"AreNotEqual(", pszUnexpectedSourceString, u8", ", pszActualSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(unexpected, actual, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(unexpected, actual, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool IsLessThan(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsLessThanImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedLess, expectedGreater,
                    pszExpectedLessSourceString, pszExpectedGreaterSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool IsLessThan(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsLessThanImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedLess, expectedGreater,
                    pszExpectedLessSourceString, pszExpectedGreaterSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsLessThanMessage(
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsLessThan({}, {})",
                        reinterpret_cast<const char*>(pszExpectedLessSourceString), reinterpret_cast<const char*>(pszExpectedGreaterSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool IsLessThanImpl(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = VerifyCompareTraits<T1, T2>::IsLessThan(expectedLess, expectedGreater);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateIsLessThanMessage(pszExpectedLessSourceString, pszExpectedGreaterSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsLessThan(", pszExpectedLessSourceString, u8", ", pszExpectedGreaterSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(expectedLess, expectedGreater, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(expectedLess, expectedGreater, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool IsLessThanOrEqual(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsLessThanOrEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedLess, expectedGreater,
                    pszExpectedLessSourceString, pszExpectedGreaterSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool IsLessThanOrEqual(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsLessThanOrEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedLess, expectedGreater,
                    pszExpectedLessSourceString, pszExpectedGreaterSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsLessThanOrEqualMessage(
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsLessThanOrEqual({}, {})",
                        reinterpret_cast<const char*>(pszExpectedLessSourceString), reinterpret_cast<const char*>(pszExpectedGreaterSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool IsLessThanOrEqualImpl(const T1& expectedLess, const T2& expectedGreater,
                const TaefChar8_t* pszExpectedLessSourceString, const TaefChar8_t* pszExpectedGreaterSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed =
                    VerifyCompareTraits<T1, T2>::IsLessThan(expectedLess, expectedGreater) ||
                    VerifyCompareTraits<T1, T2>::AreEqual(expectedLess, expectedGreater);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsLessThanOrEqualMessage(pszExpectedLessSourceString, pszExpectedGreaterSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsLessThanOrEqual(", pszExpectedLessSourceString, u8", ", pszExpectedGreaterSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(expectedLess, expectedGreater, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(expectedLess, expectedGreater, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool IsGreaterThan(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsGreaterThanImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedGreater, expectedLess,
                    pszExpectedGreaterSourceString, pszExpectedLessSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool IsGreaterThan(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsGreaterThanImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedGreater, expectedLess,
                    pszExpectedGreaterSourceString, pszExpectedLessSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsGreaterThanMessage(
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsGreaterThan({}, {})",
                        reinterpret_cast<const char*>(pszExpectedGreaterSourceString), reinterpret_cast<const char*>(pszExpectedLessSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool IsGreaterThanImpl(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = VerifyCompareTraits<T1, T2>::IsGreaterThan(expectedGreater, expectedLess);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateIsGreaterThanMessage(pszExpectedGreaterSourceString, pszExpectedLessSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsGreaterThan(", pszExpectedGreaterSourceString, u8", ", pszExpectedLessSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(expectedGreater, expectedLess, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(expectedGreater, expectedLess, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool IsGreaterThanOrEqual(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsGreaterThanOrEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedGreater, expectedLess,
                    pszExpectedGreaterSourceString, pszExpectedLessSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool IsGreaterThanOrEqual(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsGreaterThanOrEqualImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expectedGreater, expectedLess,
                    pszExpectedGreaterSourceString, pszExpectedLessSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsGreaterThanOrEqualMessage(
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsGreaterThanOrEqual({}, {})",
                        reinterpret_cast<const char*>(pszExpectedGreaterSourceString), reinterpret_cast<const char*>(pszExpectedLessSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool IsGreaterThanOrEqualImpl(const T1& expectedGreater, const T2& expectedLess,
                const TaefChar8_t* pszExpectedGreaterSourceString, const TaefChar8_t* pszExpectedLessSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed =
                    VerifyCompareTraits<T1, T2>::IsGreaterThan(expectedGreater, expectedLess) ||
                    VerifyCompareTraits<T1, T2>::AreEqual(expectedGreater, expectedLess);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsGreaterThanOrEqualMessage(pszExpectedGreaterSourceString, pszExpectedLessSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {
                    u8"IsGreaterThanOrEqual(", pszExpectedGreaterSourceString, u8", ", pszExpectedLessSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<T1, T2>(expectedGreater, expectedLess, messageBuilder);
                }
                else
                {
                    VerificationFailed<T1, T2>(expectedGreater, expectedLess, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool AreSame(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return AreSameImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expected, actual,
                    pszExpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool AreSame(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return AreSameImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(expected, actual,
                    pszExpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateAreSameMessage(
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("AreSame({}, {})",
                        reinterpret_cast<const char*>(pszExpectedSourceString), reinterpret_cast<const char*>(pszActualSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool AreSameImpl(const T1& expected, const T2& actual,
                const TaefChar8_t* pszExpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = VerifyCompareTraits<T1, T2>::AreSame(expected, actual);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateAreSameMessage(pszExpectedSourceString, pszActualSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"AreSame(", pszExpectedSourceString, u8", ", pszActualSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<const T1*, const T2*>(&expected, &actual, messageBuilder);
                }
                else
                {
                    VerificationFailed<const T1*, const T2*>(&expected, &actual, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T1, typename T2>
            static bool AreNotSame(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return AreNotSameImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(unexpected, actual,
                    pszUnexpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T1, typename T2>
            static bool AreNotSame(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return AreNotSameImpl<typename GetVerifyType<T1>::Type, typename GetVerifyType<T2>::Type>(unexpected, actual,
                    pszUnexpectedSourceString, pszActualSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateAreNotSameMessage(
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("AreNotSame({}, {})",
                        reinterpret_cast<const char*>(pszUnexpectedSourceString), reinterpret_cast<const char*>(pszActualSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T1, typename T2, typename MessageCharType>
            static bool AreNotSameImpl(const T1& unexpected, const T2& actual,
                const TaefChar8_t* pszUnexpectedSourceString, const TaefChar8_t* pszActualSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = !VerifyCompareTraits<T1, T2>::AreSame(unexpected, actual);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateAreNotSameMessage(pszUnexpectedSourceString, pszActualSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"AreNotSame(", pszUnexpectedSourceString, u8", ", pszActualSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<const T1*, const T2*>(&unexpected, &actual, messageBuilder);
                }
                else
                {
                    VerificationFailed<const T1*, const T2*>(&unexpected, &actual, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(false)
            static bool Fail(const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                if (!pszMessage || *pszMessage == u8'\0')
                {
                    pszMessage = u8"Fail";
                }

#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                VerificationFailed(reinterpret_cast<const char*>(pszMessage), pszFileName, pszFunctionName, lineNumber);
                return false;
            }
#if defined(_WIN32)
            _Post_equal_to_(false)
            static bool Fail(const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                WEX::Common::PreserveLastError preserveLastError;
                const char* pszUtf8Message = "Fail";
                StringStorage utf8Message;
                if (pszMessage && *pszMessage != L'\0')
                {
                    const size_t messageSize = ::wcslen(pszMessage);
                    size_t utf8ByteCount = 0;
                    size_t bytesWritten = 0;
                    if (TAEF_TryGetUtf8ByteCountForWCharStringValue(pszMessage, messageSize, utf8ByteCount) &&
                        utf8Message.TryAllocate(utf8ByteCount) &&
                        TAEF_TryWriteUtf8ForWCharStringValue(pszMessage, messageSize, utf8Message.Data(), utf8ByteCount, bytesWritten))
                    {
                        utf8Message.Data()[bytesWritten] = 0;
                        pszUtf8Message = utf8Message.Data();
                    }
                }

                VerificationFailed(pszUtf8Message, pszFileName, pszFunctionName, lineNumber);
                return false;
            }
#endif // #if defined(_WIN32)

            _Post_equal_to_(condition)
            static bool IsTrue(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsTrueImpl(condition, pszConditionSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            _Post_equal_to_(condition)
            static bool IsTrue(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsTrueImpl(condition, pszConditionSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsTrueMessage(const TaefChar8_t* pszConditionSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsTrue({})", reinterpret_cast<const char*>(pszConditionSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(condition)
            static bool IsTrueImpl(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = condition;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsTrueMessage(pszConditionSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsTrue(", pszConditionSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed(messageBuilder);
                }
                else
                {
                    VerificationFailed(pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(!condition)
            static bool IsFalse(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsFalseImpl(condition, pszConditionSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            _Post_equal_to_(!condition)
            static bool IsFalse(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsFalseImpl(condition, pszConditionSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsFalseMessage(const TaefChar8_t* pszConditionSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsFalse({})", reinterpret_cast<const char*>(pszConditionSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(!condition)
            static bool IsFalseImpl(bool condition, const TaefChar8_t* pszConditionSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = !condition;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsFalseMessage(pszConditionSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsFalse(", pszConditionSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed(messageBuilder);
                }
                else
                {
                    VerificationFailed(pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T>
            static bool IsNull(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsNullImpl<typename GetVerifyType<T>::Type>(object, pszObjectSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T>
            static bool IsNull(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsNullImpl<typename GetVerifyType<T>::Type>(object, pszObjectSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsNullMessage(const TaefChar8_t* pszObjectSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsNull({})", reinterpret_cast<const char*>(pszObjectSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T, typename MessageCharType>
            static bool IsNullImpl(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = VerifyCompareTraits<T, T>::IsNull(object);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsNullMessage(pszObjectSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsNull(", pszObjectSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed(object, messageBuilder);
                }
                else
                {
                    VerificationFailed(object, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            template <typename T>
            static bool IsNotNull(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return IsNotNullImpl<typename GetVerifyType<T>::Type>(object, pszObjectSourceString,
                    pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#if defined(_WIN32)
            template <typename T>
            static bool IsNotNull(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return IsNotNullImpl<typename GetVerifyType<T>::Type>(object, pszObjectSourceString,
                    pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
#endif // #if defined(_WIN32)
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateIsNotNullMessage(const TaefChar8_t* pszObjectSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("IsNotNull({})", reinterpret_cast<const char*>(pszObjectSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename T, typename MessageCharType>
            static bool IsNotNullImpl(const T& object, const TaefChar8_t* pszObjectSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = !VerifyCompareTraits<T, T>::IsNull(object);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{
                    GenerateIsNotNullMessage(pszObjectSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"IsNotNull(", pszObjectSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed(object, messageBuilder);
                }
                else
                {
                    VerificationFailed(object, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

#if defined(_WIN32)
        public:
            // Internal detail: We are adding an overload with nullptr_t because a lot of code calls
            // VERIFY_SUCCEEDED(hresult, nullptr) and without this overload, compiler can not resolve
            // the ambiguity between the char and wchar_t overloads.
            _Post_equal_to_(SUCCEEDED(hr))
            static bool Succeeded(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, decltype(nullptr) pszMessage)
            {
                return SucceededImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber,
                    static_cast<const TaefChar8_t*>(pszMessage));
            }

            _Post_equal_to_(SUCCEEDED(hr))
            static bool Succeeded(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return SucceededImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(SUCCEEDED(hr))
            static bool Succeeded(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return SucceededImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(hr)
            static HRESULT SucceededReturn(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage = nullptr)
            {
                SucceededImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return hr;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateSucceededMessage(const TaefChar8_t* pszHRSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("SUCCEEDED({})", reinterpret_cast<const char*>(pszHRSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(SUCCEEDED(hr))
            static bool SucceededImpl(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = SUCCEEDED(hr);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateSucceededMessage(pszHRSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"SUCCEEDED(", pszHRSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<VerifyFormatAsHResult>(VerifyFormatAsHResult{hr}, messageBuilder);
                }
                else
                {
                    VerificationFailed<VerifyFormatAsHResult>(VerifyFormatAsHResult{hr}, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(FAILED(hr))
            static bool Failed(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return FailedImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(FAILED(hr))
            static bool Failed(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return FailedImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(hr)
            static HRESULT FailedReturn(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                FailedImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return hr;
            }
            _Post_equal_to_(hr)
            static HRESULT FailedReturn(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                FailedImpl(hr, pszHRSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return hr;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateFailedMessage(const TaefChar8_t* pszHRSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("FAILED({})", reinterpret_cast<const char*>(pszHRSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(FAILED(hr))
            static bool FailedImpl(HRESULT hr, const TaefChar8_t* pszHRSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = FAILED(hr);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateFailedMessage(pszHRSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"FAILED(", pszHRSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<VerifyFormatAsHResult>(VerifyFormatAsHResult{hr}, messageBuilder);
                }
                else
                {
                    VerificationFailed<VerifyFormatAsHResult>(VerifyFormatAsHResult{hr}, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(win32ErrorCode == 0)
            static bool Win32Succeeded(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return Win32SucceededImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(win32ErrorCode == 0)
            static bool Win32Succeeded(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return Win32SucceededImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32SucceededReturn(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                Win32SucceededImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32ErrorCode;
            }
            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32SucceededReturn(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                Win32SucceededImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32ErrorCode;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateWin32SucceededMessage(const TaefChar8_t* pszWin32ErrorCodeSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Win32Succeeded({})", reinterpret_cast<const char*>(pszWin32ErrorCodeSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(win32ErrorCode == 0)
            static bool Win32SucceededImpl(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = win32ErrorCode == ERROR_SUCCESS;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateWin32SucceededMessage(pszWin32ErrorCodeSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"Win32Succeeded(", pszWin32ErrorCodeSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed< ::LONG>(win32ErrorCode, messageBuilder);
                }
                else
                {
                    VerificationFailed< ::LONG>(win32ErrorCode, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(win32ErrorCode != 0)
            static bool Win32Failed(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return Win32FailedImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(win32ErrorCode != 0)
            static bool Win32Failed(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return Win32FailedImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32FailedReturn(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                Win32FailedImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32ErrorCode;
            }
            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32FailedReturn(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                Win32FailedImpl(win32ErrorCode, pszWin32ErrorCodeSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32ErrorCode;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateWin32FailedMessage(const TaefChar8_t* pszWin32ErrorCodeSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Win32Failed({})", reinterpret_cast<const char*>(pszWin32ErrorCodeSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(win32ErrorCode != 0)
            static bool Win32FailedImpl(::LONG win32ErrorCode, const TaefChar8_t* pszWin32ErrorCodeSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = win32ErrorCode != ERROR_SUCCESS;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateWin32FailedMessage(pszWin32ErrorCodeSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"Win32Failed(", pszWin32ErrorCodeSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed< ::LONG>(win32ErrorCode, messageBuilder);
                }
                else
                {
                    VerificationFailed< ::LONG>(win32ErrorCode, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(win32Bool != 0)
            static bool Win32BoolSucceeded(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return Win32BoolSucceededImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(win32Bool != 0)
            static bool Win32BoolSucceeded(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return Win32BoolSucceededImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolSucceededReturn(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                Win32BoolSucceededImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32Bool;
            }
            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolSucceededReturn(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                Win32BoolSucceededImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32Bool;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateWin32BoolSucceededMessage(const TaefChar8_t* pszWin32BoolSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Win32BoolSucceeded({})", reinterpret_cast<const char*>(pszWin32BoolSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(win32Bool != 0)
            static bool Win32BoolSucceededImpl(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = win32Bool != 0;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateWin32BoolSucceededMessage(pszWin32BoolSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"Win32BoolSucceeded(", pszWin32BoolSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed< ::BOOL>(win32Bool, messageBuilder);
                }
                else
                {
                    // Log the value returned from Debug::GetLastError().
                    VerificationFailed< ::DWORD>(WEX::Common::Debug::GetLastError(), pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(win32Bool == 0)
            static bool Win32BoolFailed(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return Win32BoolFailedImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(win32Bool == 0)
            static bool Win32BoolFailed(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return Win32BoolFailedImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }

            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolFailedReturn(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                Win32BoolFailedImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32Bool;
            }
            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolFailedReturn(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                Win32BoolFailedImpl(win32Bool, pszWin32BoolSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
                return win32Bool;
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateWin32BoolFailedMessage(const TaefChar8_t* pszWin32BoolSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Win32BoolFailed({})", reinterpret_cast<const char*>(pszWin32BoolSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(win32Bool == 0)
            static bool Win32BoolFailedImpl(::BOOL win32Bool, const TaefChar8_t* pszWin32BoolSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = win32Bool == 0;

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateWin32BoolFailedMessage(pszWin32BoolSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"Win32BoolFailed(", pszWin32BoolSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed< ::BOOL>(win32Bool, messageBuilder);
                }
                else
                {
                    VerificationFailed< ::BOOL>(win32Bool, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

#if defined(NT_SUCCESS)
        public:
            _Post_equal_to_(NT_SUCCESS(ntStatus))
            static bool NtSuccess(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return NtSuccessImpl(ntStatus, pszNtStatusSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(NT_SUCCESS(ntStatus))
            static bool NtSuccess(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return NtSuccessImpl(ntStatus, pszNtStatusSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateNtSuccessMessage(const TaefChar8_t* pszNtStatusSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("NtSuccess({})", reinterpret_cast<const char*>(pszNtStatusSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(NT_SUCCESS(ntStatus))
            static bool NtSuccessImpl(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = NT_SUCCESS(ntStatus);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateNtSuccessMessage(pszNtStatusSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"NtSuccess(", pszNtStatusSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<NTSTATUS>(ntStatus, messageBuilder);
                }
                else
                {
                    VerificationFailed<NTSTATUS>(ntStatus, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }

        public:
            _Post_equal_to_(!NT_SUCCESS(ntStatus))
            static bool NotNtSuccess(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                return NotNtSuccessImpl(ntStatus, pszNtStatusSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
            _Post_equal_to_(!NT_SUCCESS(ntStatus))
            static bool NotNtSuccess(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                return NotNtSuccessImpl(ntStatus, pszNtStatusSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
        private:
#if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateNotNtSuccessMessage(const TaefChar8_t* pszNtStatusSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("NotNtSuccess({})", reinterpret_cast<const char*>(pszNtStatusSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
#endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            _Post_equal_to_(!NT_SUCCESS(ntStatus))
            static bool NotNtSuccessImpl(NTSTATUS ntStatus, const TaefChar8_t* pszNtStatusSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                const bool passed = !NT_SUCCESS(ntStatus);

                if (passed && LogOnlyFailures())
                {
                    return true;
                }

#if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateNotNtSuccessMessage(pszNtStatusSourceString), pszMessage};
#else
                const Utf8StringView messageSegments[] = {u8"NotNtSuccess(", pszNtStatusSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
#endif
                if (passed)
                {
                    VerificationPassed<NTSTATUS>(ntStatus, messageBuilder);
                }
                else
                {
                    VerificationFailed<NTSTATUS>(ntStatus, pszFileName, pszFunctionName, lineNumber, messageBuilder);
                }

                return passed;
            }
#endif // #if defined(NT_SUCCESS)
#endif // #if defined(_WIN32)

#if defined(__cpp_exceptions)
        public:
            static void ExpectedExceptionThrown(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const TaefChar8_t* pszMessage = nullptr)
            {
                ExpectedExceptionThrownImpl(pszExceptionSourceString, pszOperationSourceString, pszMessage);
            }
# if defined(_WIN32)
            static void ExpectedExceptionThrown(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const wchar_t* pszMessage)
            {
                ExpectedExceptionThrownImpl(pszExceptionSourceString, pszOperationSourceString, pszMessage);
            }
# endif // #if defined(_WIN32)
        private:
# if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateExpectedExceptionThrownMessage(const TaefChar8_t* pszExceptionSourceString,
                const TaefChar8_t* pszOperationSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Expected exception ({}) caught during ({})",
                        reinterpret_cast<const char*>(pszExceptionSourceString), reinterpret_cast<const char*>(pszOperationSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
# endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            static void ExpectedExceptionThrownImpl(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const MessageCharType* pszMessage)
            {
                if (LogOnlyFailures())
                {
                    return;
                }

# if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateExpectedExceptionThrownMessage(pszExceptionSourceString, pszOperationSourceString), pszMessage};
# else
                const Utf8StringView messageSegments[] = {
                    u8"Expected exception (", pszExceptionSourceString, u8") caught during (", pszOperationSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
# endif
                VerificationPassed(messageBuilder);
            }

        public:
            static void ExpectedExceptionNotThrown(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                ExpectedExceptionNotThrownImpl(pszExceptionSourceString, pszOperationSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
# if defined(_WIN32)
            static void ExpectedExceptionNotThrown(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                ExpectedExceptionNotThrownImpl(pszExceptionSourceString, pszOperationSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
# endif // #if defined(_WIN32)
        private:
# if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateExpectedExceptionNotThrownMessage(const TaefChar8_t* pszExceptionSourceString,
                const TaefChar8_t* pszOperationSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Expected exception ({}) not caught during ({})",
                        reinterpret_cast<const char*>(pszExceptionSourceString), reinterpret_cast<const char*>(pszOperationSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
# endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            static void ExpectedExceptionNotThrownImpl(const TaefChar8_t* pszExceptionSourceString, const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
# if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateExpectedExceptionNotThrownMessage(pszExceptionSourceString, pszOperationSourceString),
                    pszMessage};
# else
                const Utf8StringView messageSegments[] = {
                    u8"Expected exception (", pszExceptionSourceString, u8") not caught during (", pszOperationSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
# endif
                VerificationFailed(pszFileName, pszFunctionName, lineNumber, messageBuilder);
            }

        public:
            static void UnexpectedExceptionThrown(const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const TaefChar8_t* pszMessage = nullptr)
            {
                UnexpectedExceptionThrownImpl(pszOperationSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
# if defined(_WIN32)
            static void UnexpectedExceptionThrown(const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const wchar_t* pszMessage)
            {
                UnexpectedExceptionThrownImpl(pszOperationSourceString, pszFileName, pszFunctionName, lineNumber, pszMessage);
            }
# endif // #if defined(_WIN32)
        private:
# if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateUnexpectedExceptionThrownMessage(const TaefChar8_t* pszExceptionInfo,
                const TaefChar8_t* pszOperationSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("{} during ({})",
                        reinterpret_cast<const char*>(pszExceptionInfo), reinterpret_cast<const char*>(pszOperationSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
# endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            static void UnexpectedExceptionThrownImpl(const TaefChar8_t* pszOperationSourceString,
                const char* pszFileName, const char* pszFunctionName, int lineNumber, const MessageCharType* pszMessage)
            {
                StringStorage exceptionInfoStorage;
                const TaefChar8_t* pszExceptionInfo = GetExceptionInfo(exceptionInfoStorage);

# if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateUnexpectedExceptionThrownMessage(pszExceptionInfo, pszOperationSourceString), pszMessage};
# else
                const Utf8StringView messageSegments[] = {pszExceptionInfo, u8" during (", pszOperationSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
# endif
                VerificationFailed(pszFileName, pszFunctionName, lineNumber, messageBuilder);
            }

        public:
            static void UnexpectedExceptionNotThrown(const TaefChar8_t* pszOperationSourceString,
                const TaefChar8_t* pszMessage = nullptr)
            {
                UnexpectedExceptionNotThrownImpl(pszOperationSourceString, pszMessage);
            }
# if defined(_WIN32)
            static void UnexpectedExceptionNotThrown(const TaefChar8_t* pszOperationSourceString,
                const wchar_t* pszMessage)
            {
                UnexpectedExceptionNotThrownImpl(pszOperationSourceString, pszMessage);
            }
# endif // #if defined(_WIN32)
        private:
# if defined(TAEF_USE_STD_FORMAT)
            static std::string GenerateUnexpectedExceptionNotThrownMessage(const TaefChar8_t* pszOperationSourceString) noexcept
            {
                std::string message;
                try
                {
                    message = TAEF_STD_FORMAT_NAMESPACE::format("Exception not caught during ({})",
                        reinterpret_cast<const char*>(pszOperationSourceString));
                }
                catch (std::exception&)
                {
                    // Leave message empty.
                }
                return message;
            }
# endif // #if defined(TAEF_USE_STD_FORMAT)
            template <typename MessageCharType>
            static void UnexpectedExceptionNotThrownImpl(const TaefChar8_t* pszOperationSourceString, const MessageCharType* pszMessage)
            {
                if (LogOnlyFailures())
                {
                    return;
                }

# if defined(TAEF_USE_STD_FORMAT)
                VerifyMessageBuilder messageBuilder{GenerateUnexpectedExceptionNotThrownMessage(pszOperationSourceString), pszMessage};
# else
                const Utf8StringView messageSegments[] = {u8"Exception not caught during (", pszOperationSourceString, u8")"};
                VerifyMessageBuilder messageBuilder{messageSegments, pszMessage};
# endif
                VerificationPassed(messageBuilder);
            }
#endif // #if defined(__cpp_exceptions)

        private:
#if defined(TAEF_USE_STD_FORMAT)
            class VerifyMessageBuilder
            {
            public:
                VerifyMessageBuilder(std::string&& verifyMessage, const TaefChar8_t* pszUtf8CustomMessage) noexcept
                    : m_verifyMessage{std::move(verifyMessage)}
                {
                    if (!m_verifyMessage.empty())
                    {
                        const size_t initialSize = m_verifyMessage.size();
                        try
                        {
                            TAEF_STD_FORMAT_NAMESPACE::format_to(std::back_inserter(m_verifyMessage),
                                ": {}", reinterpret_cast<const char*>(pszUtf8CustomMessage));
                        }
                        catch (std::exception&)
                        {
                            m_verifyMessage.resize(initialSize);
                        }
                    }
                }
#if defined(_WIN32)
                VerifyMessageBuilder(std::string&& verifyMessage, const wchar_t* pszUtf16CustomMessage) noexcept
                    : m_verifyMessage{std::move(verifyMessage)}
                {
                    if (!m_verifyMessage.empty())
                    {
                        std::string utf8CustomMessage;
                        if (TryConvertWideStringToUtf8String(pszUtf16CustomMessage, ::wcslen(pszUtf16CustomMessage), utf8CustomMessage))
                        {
                            const size_t initialSize = m_verifyMessage.size();
                            try
                            {
                                TAEF_STD_FORMAT_NAMESPACE::format_to(std::back_inserter(m_verifyMessage),
                                    ": {}", utf8CustomMessage);
                            }
                            catch (std::exception&)
                            {
                                m_verifyMessage.resize(initialSize);
                            }
                        }
                    }
                }
#endif // #if defined(_WIN32)

                template <typename T1, typename T2>
                const char* BuildMessageWithValues(const T1& value1, const T2& value2) noexcept
                {
                    if (m_verifyMessage.empty())
                    {
                        return nullptr;
                    }

                    if constexpr (
                        !std::is_same_v<DoNotFormatWithVerify, decltype(GetFormattableVerifyOutputValue(value1))> &&
                        !std::is_same_v<DoNotFormatWithVerify, decltype(GetFormattableVerifyOutputValue(value2))>)
                    {
                        const size_t initialSize = m_verifyMessage.size();
                        try
                        {
                            TAEF_STD_FORMAT_NAMESPACE::format_to(std::back_inserter(m_verifyMessage), " - Values ({}, {})",
                                GetFormattableVerifyOutputValue(value1), GetFormattableVerifyOutputValue(value2));
                        }
                        catch (...)
                        {
                            m_verifyMessage.resize(initialSize);
                        }
                    }

                    return m_verifyMessage.c_str();
                }

                template <typename T>
                const char* BuildMessageWithValue(const T& value) noexcept
                {
                    if (m_verifyMessage.empty())
                    {
                        return nullptr;
                    }

                    if constexpr (!std::is_same_v<DoNotFormatWithVerify, decltype(GetFormattableVerifyOutputValue(value))>)
                    {
                        const size_t initialSize = m_verifyMessage.size();
                        try
                        {
                            TAEF_STD_FORMAT_NAMESPACE::format_to(std::back_inserter(m_verifyMessage), " - Value ({})",
                                GetFormattableVerifyOutputValue(value));
                        }
                        catch (...)
                        {
                            m_verifyMessage.resize(initialSize);
                        }
                    }

                    return m_verifyMessage.c_str();
                }

                const char* BuildMessage() const noexcept
                {
                    return m_verifyMessage.empty() ? nullptr : m_verifyMessage.c_str();
                }

            private:
                std::string m_verifyMessage;
            };
#else // ^^ defined(TAEF_USE_STD_FORMAT) ^^ | vv !defined(TAEF_USE_STD_FORMAT) vv

# if defined(_MSC_VER)
#  pragma warning(push)
// Disable warnings related to implicitly deleted special member functions.
# pragma warning(disable: 4623 4625 4626)
# endif
            class VerifyMessageBuilder
            {
            public:
                template <size_t SegmentCount>
                VerifyMessageBuilder(const Utf8StringView(&messageSegments)[SegmentCount], const TaefChar8_t* pszMessage) noexcept
                    : m_pMessageSegments{messageSegments}
                    , m_messageSegmentCount{SegmentCount}
                    , m_pszUtf8CustomMessage{pszMessage}
                {}
#if defined(_WIN32)
                template <size_t SegmentCount>
                VerifyMessageBuilder(const Utf8StringView(&messageSegments)[SegmentCount], const wchar_t* pszMessage) noexcept
                    : m_pMessageSegments{messageSegments}
                    , m_messageSegmentCount{SegmentCount}
                    , m_pszUtf16CustomMessage{pszMessage}
                {}
#endif // #if defined(_WIN32)

                template <typename T1, typename T2>
                const char* BuildMessageWithValues(const T1& value1, const T2& value2) noexcept
                {
                    ValueOutputHandler<T1> valueOutputHandler1{value1};
                    ValueOutputHandler<T2> valueOutputHandler2{value2};

                    const Utf8StringView valuesSuffixStart{u8" - Values ("};
                    const Utf8StringView valuesSuffixMid{u8", "};
                    const Utf8StringView valuesSuffixEnd{u8")"};

                    const size_t messageSegmentSize = GetRequiredSizeForMessageSegments();
                    const size_t customMessageSize = GetRequiredSizeForCustomMessage();

                    size_t totalSize = messageSegmentSize + customMessageSize;

                    size_t value1Size = 0;
                    size_t value2Size = 0;
                    bool writeValues = false;

                    if (valueOutputHandler1.TryGetUtf8ByteCountForValue(value1Size) &&
                        valueOutputHandler2.TryGetUtf8ByteCountForValue(value2Size))
                    {
                        totalSize += valuesSuffixStart.Size() + value1Size + valuesSuffixMid.Size() + value2Size + valuesSuffixEnd.Size();
                        writeValues = true;
                    }

                    if (m_verifyMessageStorage.TryAllocate(totalSize))
                    {
                        StringWriter messageWriter{m_verifyMessageStorage.Data(), m_verifyMessageStorage.AllocationSize()};

                        AppendMessageSegmentsToOutput(messageWriter);
                        AppendCustomMessageToOutput(messageWriter, customMessageSize);

                        if (writeValues)
                        {
                            const size_t lengthWithoutValues = messageWriter.GetInitializedCharCount();

                            bool successfullyAddedValues = false;
                            messageWriter.Append(valuesSuffixStart);
                            size_t bytesWritten = 0;
                            if (valueOutputHandler1.TryWriteUtf8ForValue(messageWriter.GetWriteBuffer(), value1Size, bytesWritten))
                            {
                                messageWriter.Advance(bytesWritten);
                                messageWriter.Append(valuesSuffixMid);
                                if (valueOutputHandler2.TryWriteUtf8ForValue(messageWriter.GetWriteBuffer(), value2Size, bytesWritten))
                                {
                                    messageWriter.Advance(bytesWritten);
                                    messageWriter.Append(valuesSuffixEnd);
                                    successfullyAddedValues = true;
                                }
                            }

                            if (!successfullyAddedValues)
                            {
                                messageWriter.SetInitializedCharCount(lengthWithoutValues);
                            }
                        }

                        messageWriter.WriteNullTerminator();
                    }

                    return m_verifyMessageStorage.Data();
                }

                template <typename T>
                const char* BuildMessageWithValue(const T& value) noexcept
                {
                    ValueOutputHandler<T> valueOutputHandler{value};

                    const Utf8StringView valueSuffixStart{u8" - Value ("};
                    const Utf8StringView valueSuffixEnd{u8")"};

                    const size_t messageSegmentSize = GetRequiredSizeForMessageSegments();
                    const size_t customMessageSize = GetRequiredSizeForCustomMessage();

                    size_t totalSize = messageSegmentSize + customMessageSize;

                    size_t valueSize = 0;
                    bool writeValue = false;

                    if (valueOutputHandler.TryGetUtf8ByteCountForValue(valueSize))
                    {
                        totalSize += valueSuffixStart.Size() + valueSize + valueSuffixEnd.Size();
                        writeValue = true;
                    }

                    if (m_verifyMessageStorage.TryAllocate(totalSize))
                    {
                        StringWriter messageWriter{m_verifyMessageStorage.Data(), m_verifyMessageStorage.AllocationSize()};

                        AppendMessageSegmentsToOutput(messageWriter);
                        AppendCustomMessageToOutput(messageWriter, customMessageSize);

                        if (writeValue)
                        {
                            const size_t lengthWithoutValue = messageWriter.GetInitializedCharCount();

                            messageWriter.Append(valueSuffixStart);
                            size_t bytesWritten = 0;
                            if (valueOutputHandler.TryWriteUtf8ForValue(messageWriter.GetWriteBuffer(), valueSize, bytesWritten))
                            {
                                messageWriter.Advance(bytesWritten);
                                messageWriter.Append(valueSuffixEnd);
                            }
                            else
                            {
                                messageWriter.SetInitializedCharCount(lengthWithoutValue);
                            }
                        }

                        messageWriter.WriteNullTerminator();
                    }

                    return m_verifyMessageStorage.Data();
                }

                const char* BuildMessage() noexcept
                {
                    const size_t messageSegmentSize = GetRequiredSizeForMessageSegments();
                    const size_t customMessageSize = GetRequiredSizeForCustomMessage();

                    const size_t totalSize = messageSegmentSize + customMessageSize;

                    if (m_verifyMessageStorage.TryAllocate(totalSize))
                    {
                        StringWriter messageWriter{m_verifyMessageStorage.Data(), m_verifyMessageStorage.AllocationSize()};

                        AppendMessageSegmentsToOutput(messageWriter);
                        AppendCustomMessageToOutput(messageWriter, customMessageSize);
                        messageWriter.WriteNullTerminator();
                    }

                    return m_verifyMessageStorage.Data();
                }

            private:
                size_t GetRequiredSizeForMessageSegments() const noexcept
                {
                    size_t totalSize = 0;
                    for (size_t segmentIndex = 0; segmentIndex < m_messageSegmentCount; ++segmentIndex)
                    {
                        totalSize += m_pMessageSegments[segmentIndex].Size();
                    }
                    return totalSize;
                }
                void AppendMessageSegmentsToOutput(StringWriter& messageWriter) const noexcept
                {
                    for (size_t segmentIndex = 0; segmentIndex < m_messageSegmentCount; ++segmentIndex)
                    {
                        messageWriter.Append(m_pMessageSegments[segmentIndex]);
                    }
                }

                size_t GetRequiredSizeForCustomMessage() const noexcept
                {
                    size_t totalSize = 0;
                    if (m_pszUtf8CustomMessage)
                    {
                        // The format is ": {}".
                        totalSize = 2 + ::strlen(reinterpret_cast<const char*>(m_pszUtf8CustomMessage));
                    }
#if defined(_WIN32)
                    else if (m_pszUtf16CustomMessage)
                    {
                        // The format is ": {}".
                        size_t customMessageSize = 0;
                        if (TAEF_TryGetUtf8ByteCountForWCharStringValue(m_pszUtf16CustomMessage, ::wcslen(m_pszUtf16CustomMessage), customMessageSize))
                        {
                            totalSize = 2 + customMessageSize;
                        }
                    }
#endif // #if defined(_WIN32)
                    return totalSize;
                }
                void AppendCustomMessageToOutput(StringWriter& messageWriter, size_t customMessageSize) const noexcept
                {
                    if (m_pszUtf8CustomMessage)
                    {
                        messageWriter.Append(u8": ", 2);
                        messageWriter.Append(m_pszUtf8CustomMessage, customMessageSize - 2);
                    }
#if defined(_WIN32)
                    else if (m_pszUtf16CustomMessage)
                    {
                        // If customMessageSize is zero, this means we failed to get the size in GetRequiredSizeForCustomMessage.
                        if (customMessageSize > 0)
                        {
                            const size_t sizeWithoutCustomMessage = messageWriter.GetInitializedCharCount();
                            messageWriter.Append(u8": ", 2);

                            size_t bytesWritten = 0;
                            if (TAEF_TryWriteUtf8ForWCharStringValue(m_pszUtf16CustomMessage, ::wcslen(m_pszUtf16CustomMessage),
                                messageWriter.GetWriteBuffer(), customMessageSize - 2, bytesWritten))
                            {
                                messageWriter.Advance(bytesWritten);
                            }
                            else
                            {
                                // Roll back the ": " that we added earlier.
                                messageWriter.SetInitializedCharCount(sizeWithoutCustomMessage);
                            }
                        }
                    }
#endif // #if defined(_WIN32)
                }

                const Utf8StringView* const m_pMessageSegments;
                const size_t m_messageSegmentCount;

                const TaefChar8_t* const m_pszUtf8CustomMessage = nullptr;
#if defined(_WIN32)
                const wchar_t* const m_pszUtf16CustomMessage = nullptr;
#endif // #if defined(_WIN32)

                StringStorage m_verifyMessageStorage;
            };
# if defined(_MSC_VER)
#  pragma warning(pop)
# endif
#endif // !defined(TAEF_USE_STD_FORMAT)

            template <typename T1, typename T2>
            static void VerificationFailed(const T1& value1, const T2& value2, const char* pszFileName, const char* pszFunctionName, int lineNumber,
                VerifyMessageBuilder& messageBuilder)
            {
                // We cast away volatile if it is present. This might reduce the chance of getting a confusing
                // error message such as VERIFY_ARE_EQUAL failing with (4, 4) if a volatile value is changed
                // between the equality check and the error message.
                using T1_NoVolatile = typename RemoveVolatile<T1>::Type;
                using T2_NoVolatile = typename RemoveVolatile<T2>::Type;

#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = messageBuilder.BuildMessageWithValues<T1_NoVolatile, T2_NoVolatile>(
                    const_cast<const T1_NoVolatile&>(value1), const_cast<const T2_NoVolatile&>(value2));
                if (!pszMessage)
                {
                    pszMessage = "Error generating failure message; possibly out of memory.";
                }

                VerificationFailed(pszMessage, pszFileName, pszFunctionName, lineNumber);
            }

            template <typename T>
            static void VerificationFailed(const T& value, const char* pszFileName, const char* pszFunctionName, int lineNumber,
                VerifyMessageBuilder& messageBuilder)
            {
                using T_NoVolatile = typename RemoveVolatile<T>::Type;

#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = messageBuilder.BuildMessageWithValue<T_NoVolatile>(const_cast<const T_NoVolatile&>(value));
                if (!pszMessage)
                {
                    pszMessage = "Error generating failure message; possibly out of memory.";
                }

                VerificationFailed(pszMessage, pszFileName, pszFunctionName, lineNumber);
            }

            static void VerificationFailed(const char* pszFileName, const char* pszFunctionName, int lineNumber, VerifyMessageBuilder& messageBuilder)
            {
#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = messageBuilder.BuildMessage();
                if (!pszMessage)
                {
                    pszMessage = "Error generating failure message; possibly out of memory.";
                }

                VerificationFailed(pszMessage, pszFileName, pszFunctionName, lineNumber);
            }

            static void VerificationFailed(const char* pszMessage, const char* pszFileName, const char* pszFunctionName, int lineNumber)
            {
                const VerifyOutputSettings::Setting outputSetting = Private::GetVerifyOutputSettings();
                if (outputSetting & VerifyOutputSettings::LogFailuresAsBlocked)
                {
                    WEX::Logging::Log::Trace(WEX::Logging::ResultTrace{WEX::Logging::TestResults::Blocked, pszMessage}
                        .WithContext("Verify").WithSourceInfo(pszFileName, pszFunctionName, lineNumber));
                }
                else if (outputSetting & VerifyOutputSettings::LogFailuresAsWarnings)
                {
                    WEX::Logging::Log::Trace(WEX::Logging::WarningTrace{pszMessage}
                        .WithContext("Verify").WithSourceInfo(pszFileName, pszFunctionName, lineNumber));
                }
                else
                {
                    WEX::Logging::Log::Trace(WEX::Logging::ErrorTrace{pszMessage}
                        .WithContext("Verify").WithSourceInfo(pszFileName, pszFunctionName, lineNumber));
                }

#if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)
                if (Private::GetDisabledVerifyThrowCount() == 0)
                {
                    throw VerifyFailureException{TAEF_E_FAIL, pszMessage};
                }
#endif
            }

            template <typename T1, typename T2>
            static void VerificationPassed(const T1& value1, const T2& value2, VerifyMessageBuilder& messageBuilder)
            {
                using T1_NoVolatile = typename RemoveVolatile<T1>::Type;
                using T2_NoVolatile = typename RemoveVolatile<T2>::Type;

#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = LogValuesOnSuccess()
                    ? messageBuilder.BuildMessageWithValues<T1_NoVolatile, T2_NoVolatile>(
                        const_cast<const T1_NoVolatile&>(value1), const_cast<const T2_NoVolatile&>(value2))
                    : messageBuilder.BuildMessage();
                if (!pszMessage)
                {
                    pszMessage = "Error generating pass message; possibly out of memory.";
                }
                WEX::Logging::Log::Comment(pszMessage, "Verify");
            }

            template <typename T>
            static void VerificationPassed(const T& value, VerifyMessageBuilder& messageBuilder)
            {
                using T_NoVolatile = typename RemoveVolatile<T>::Type;

#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = LogValuesOnSuccess()
                    ? messageBuilder.BuildMessageWithValue<T_NoVolatile>(const_cast<const T_NoVolatile&>(value))
                    : messageBuilder.BuildMessage();
                if (!pszMessage)
                {
                    pszMessage = "Error generating pass message; possibly out of memory.";
                }
                WEX::Logging::Log::Comment(pszMessage, "Verify");
            }

            static void VerificationPassed(VerifyMessageBuilder& messageBuilder)
            {
#if defined(_WIN32)
                WEX::Common::PreserveLastError preserveLastError;
#endif
                const char* pszMessage = messageBuilder.BuildMessage();
                if (!pszMessage)
                {
                    pszMessage = "Error generating pass message; possibly out of memory.";
                }
                WEX::Logging::Log::Comment(pszMessage, "Verify");
            }

#if defined(_WIN32)
        public:
            // The remaining methods in this class are not called by the official VERIFY macros anymore, but they are here to enable tests to
            // build which copied the old version of the VERIFY macros into their own code.
            template <typename T1, typename T2>
            static bool AreEqual(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"AreEqual(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::AreEqualImpl<T1, T2>(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool AreNotEqual(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"AreNotEqual(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::AreNotEqualImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool IsLessThan(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsLessThan(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::IsLessThanImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool IsLessThanOrEqual(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsLessThanOrEqual(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::IsLessThanOrEqualImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool IsGreaterThan(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsGreaterThan(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::IsGreaterThanImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool IsGreaterThanOrEqual(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsGreaterThanOrEqual(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::IsGreaterThanOrEqualImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool AreSame(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"AreSame(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::AreSameImpl(expected, actual, errorInfo, buildMessage);
            }

            template <typename T1, typename T2>
            static bool AreNotSame(const T1& expected, const T2& actual, const wchar_t* pszExpected, const wchar_t* pszActual,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"AreNotSame(%s, %s)", pszExpected, pszActual, pszMessage);
                return WEX::TestExecution::Verify::AreNotSameImpl(expected, actual, errorInfo, buildMessage);
            }

            _Post_equal_to_(false)
            static bool Fail(const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                return WEX::TestExecution::Verify::Fail(pszMessage, errorInfo);
            }

            _Post_equal_to_(condition)
            static bool IsTrue(bool condition, const wchar_t* pszCondition, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsTrue(%s)", pszCondition, pszMessage);
                return WEX::TestExecution::Verify::IsTrueImpl(condition, errorInfo, buildMessage);
            }

            _Post_equal_to_(!condition)
            static bool IsFalse(bool condition, const wchar_t* pszCondition, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsFalse(%s)", pszCondition, pszMessage);
                return WEX::TestExecution::Verify::IsFalseImpl(condition, errorInfo, buildMessage);
            }

            template <typename T>
            static bool IsNull(const T& object, const wchar_t* pszObject, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsNull(%s)", pszObject, pszMessage);
                return WEX::TestExecution::Verify::IsNullImpl(object, errorInfo, buildMessage);
            }

            template <typename T>
            static bool IsNotNull(const T& object, const wchar_t* pszObject, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"IsNotNull(%s)", pszObject, pszMessage);
                return WEX::TestExecution::Verify::IsNotNullImpl(object, errorInfo, buildMessage);
            }

            _Post_equal_to_(SUCCEEDED(hr))
            static bool Succeeded(HRESULT hr, const wchar_t* pszHresult, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"SUCCEEDED(%s)", pszHresult, pszMessage);
                return WEX::TestExecution::Verify::SucceededImpl(hr, errorInfo, buildMessage);
            }

            _Post_equal_to_(hr)
            static HRESULT SucceededReturn(HRESULT hr, const wchar_t* pszHresult, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Succeeded(hr, pszHresult, errorInfo, pszMessage);
                return hr;
            }

            _Post_equal_to_(FAILED(hr))
            static bool Failed(HRESULT hr, const wchar_t* pszHresult, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"FAILED(%s)", pszHresult, pszMessage);
                return WEX::TestExecution::Verify::FailedImpl(hr, errorInfo, buildMessage);
            }

            _Post_equal_to_(hr)
            static HRESULT FailedReturn(HRESULT hr, const wchar_t* pszHresult, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Failed(hr, pszHresult, errorInfo, pszMessage);
                return hr;
            }

            _Post_equal_to_(win32ErrorCode == 0)
            static bool Win32Succeeded(::LONG win32ErrorCode, const wchar_t* pszWin32ErrorCode,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"Win32Succeeded(%s)", pszWin32ErrorCode, pszMessage);
                return WEX::TestExecution::Verify::Win32SucceededImpl(win32ErrorCode, errorInfo, buildMessage);
            }

            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32SucceededReturn(::LONG win32ErrorCode, const wchar_t* pszWin32ErrorCode,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Win32Succeeded(win32ErrorCode, pszWin32ErrorCode, errorInfo, pszMessage);
                return win32ErrorCode;
            }

            _Post_equal_to_(win32ErrorCode != 0)
            static bool Win32Failed(::LONG win32ErrorCode, const wchar_t* pszWin32ErrorCode,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"Win32Failed(%s)", pszWin32ErrorCode, pszMessage);
                return WEX::TestExecution::Verify::Win32FailedImpl(win32ErrorCode, errorInfo, buildMessage);
            }

            _Post_equal_to_(win32ErrorCode)
            static ::LONG Win32FailedReturn(::LONG win32ErrorCode, const wchar_t* pszWin32ErrorCode,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Win32Failed(win32ErrorCode, pszWin32ErrorCode, errorInfo, pszMessage);
                return win32ErrorCode;
            }

            _Post_equal_to_(win32Bool != 0)
            static bool Win32BoolSucceeded(::BOOL win32Bool, const wchar_t* pszWin32Bool,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"Win32BoolSucceeded(%s)", pszWin32Bool, pszMessage);
                return WEX::TestExecution::Verify::Win32BoolSucceededImpl(win32Bool, errorInfo, buildMessage);
            }

            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolSucceededReturn(::BOOL win32Bool, const wchar_t* pszWin32Bool,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Win32BoolSucceeded(win32Bool, pszWin32Bool, errorInfo, pszMessage);
                return win32Bool;
            }

            _Post_equal_to_(win32Bool == 0)
            static bool Win32BoolFailed(::BOOL win32Bool, const wchar_t* pszWin32Bool,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"Win32BoolFailed(%s)", pszWin32Bool, pszMessage);
                return WEX::TestExecution::Verify::Win32BoolFailedImpl(win32Bool, errorInfo, buildMessage);
            }

            _Post_equal_to_(win32Bool)
            static ::BOOL Win32BoolFailedReturn(::BOOL win32Bool, const wchar_t* pszWin32Bool,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                Win32BoolFailed(win32Bool, pszWin32Bool, errorInfo, pszMessage);
                return win32Bool;
            }

#if defined(NT_SUCCESS)
            _Post_equal_to_(NT_SUCCESS(ntStatus))
            static bool NtSuccess(NTSTATUS ntStatus, const wchar_t* pszNtStatus,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"NtSuccess(%s)", pszNtStatus, pszMessage);
                return WEX::TestExecution::Verify::NtSuccessImpl(ntStatus, errorInfo, buildMessage);
            }

            _Post_equal_to_(!NT_SUCCESS(ntStatus))
            static bool NotNtSuccess(NTSTATUS ntStatus, const wchar_t* pszNtStatus,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                MacroVerifyMessageFunctor buildMessage(L"NotNtSuccess(%s)", pszNtStatus, pszMessage);
                return WEX::TestExecution::Verify::NotNtSuccessImpl(ntStatus, errorInfo, buildMessage);
            }
#endif // #if defined(NT_SUCCESS)

            template <typename T>
            _Post_equal_to_(true)
            static bool ExpectedExceptionThrown(const T& TException,
                const wchar_t* pszException, const wchar_t* pszOperation, const wchar_t* pszMessage = nullptr)
            {
                UNREFERENCED_PARAMETER(TException);

                if (LogOnlyFailures())
                {
                    return true;
                }

                static const wchar_t c_szFormat[] = L"Expected exception (%s) caught during (%s)";
                return WEX::TestExecution::Verify::VerificationPassed([&]()->WEX::Common::NoThrowString
                    {
                        return BuildMessage(c_szFormat, pszException, pszOperation, pszMessage);
                    });
            }

            _Post_equal_to_(false)
            static bool ExpectedExceptionNotThrown(const wchar_t* pszException, const wchar_t* pszOperation,
                const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                static const wchar_t c_szFormat[] = L"Expected exception (%s) not caught during (%s)";
                return WEX::TestExecution::Verify::VerificationFailed([&]()->WEX::Common::NoThrowString
                    {
                        return BuildMessage(c_szFormat, pszException, pszOperation, pszMessage);
                    },
                    errorInfo);
            }

            _Post_equal_to_(false)
            static bool UnexpectedExceptionThrown(const wchar_t* pszOperation, const ErrorInfo& errorInfo, const wchar_t* pszMessage = nullptr)
            {
                static const wchar_t c_szFormat[] = L"Unexpected exception caught during (%s)";
                return WEX::TestExecution::Verify::VerificationFailed([&]()->WEX::Common::NoThrowString
                    {
                        return BuildMessage(c_szFormat, pszOperation, pszMessage);
                    },
                    errorInfo);
            }

            _Post_equal_to_(false)
            static bool UnexpectedExceptionThrownWithSpecifics(const wchar_t* pszOperation, const ErrorInfo& errorInfo, const wchar_t* pszSpecificException,
                const wchar_t* pszMessage = nullptr)
            {
                static const wchar_t c_szFormat[] = L"%s during (%s)";
                return WEX::TestExecution::Verify::VerificationFailed([&]()->WEX::Common::NoThrowString
                    {
                        return BuildMessage(c_szFormat, pszSpecificException, pszOperation, pszMessage);
                    },
                    errorInfo);
            }

            _Post_equal_to_(true)
            static bool UnexpectedExceptionNotThrown(const wchar_t* pszOperation, const wchar_t* pszMessage = nullptr)
            {
                if (LogOnlyFailures())
                {
                    return true;
                }

                static const wchar_t c_szFormat[] = L"Exception not caught during (%s)";
                return WEX::TestExecution::Verify::VerificationPassed([&]()->WEX::Common::NoThrowString
                    {
                        return BuildMessage(c_szFormat, pszOperation, pszMessage);
                    });
            }

        private:
            class MacroVerifyMessageFunctor
            {
            public:
                MacroVerifyMessageFunctor(const wchar_t* pszFormat, const wchar_t* psz1, const wchar_t* psz2, const wchar_t* pszMessage)
                    : m_pszFormat(pszFormat)
                    , m_psz1(psz1)
                    , m_psz2(psz2)
                    , m_pszMessage(pszMessage)
                {}

                MacroVerifyMessageFunctor(const wchar_t* pszFormat, const wchar_t* psz1, const wchar_t* pszMessage)
                    : m_pszFormat(pszFormat)
                    , m_psz1(psz1)
                    , m_psz2(nullptr)
                    , m_pszMessage(pszMessage)
                {}

                WEX::Common::NoThrowString operator()() const
                {
                    if (m_psz2)
                    {
                        return BuildMessage(m_pszFormat, m_psz1, m_psz2, m_pszMessage);
                    }

                    return BuildMessage(m_pszFormat, m_psz1, m_pszMessage);
                }

            private:
                const wchar_t* m_pszFormat;
                const wchar_t* m_psz1;
                const wchar_t* m_psz2;
                const wchar_t* m_pszMessage;
            };

            static WEX::Common::NoThrowString BuildMessage(const wchar_t* pszFormat, const wchar_t* psz1, const wchar_t* psz2, const wchar_t* pszMessage)
            {
                WEX::Common::NoThrowString message;
                message.Format(pszFormat, psz1, psz2);
                if (!WEX::Common::String::IsNullOrEmpty(pszMessage))
                {
                    message.AppendFormat(L": %s", pszMessage);
                }
                return message;
            }

            static WEX::Common::NoThrowString BuildMessage(const wchar_t* pszFormat, const wchar_t* psz1, const wchar_t* pszMessage)
            {
                WEX::Common::NoThrowString message;
                message.Format(pszFormat, psz1);
                if (!WEX::Common::String::IsNullOrEmpty(pszMessage))
                {
                    message.AppendFormat(L": %s", pszMessage);
                }
                return message;
            }
#endif // #if defined(_WIN32)
        };

        template<typename Parameter>
        class TrueUnaryFunctor
        {
        public:
            bool operator()(Parameter)
            {
                return true;
            }
        };
    }/* namespace Private */
}/* namespace TestExecution */}/* namespace WEX */
#pragma warning(pop)

#pragma pop_macro("Trace")
#pragma pop_macro("Verify")

#if !defined(_WIN32)
#pragma pop_macro("_Success_")
#pragma pop_macro("_Printf_format_string_")
#pragma pop_macro("_Post_invalid_")
#pragma pop_macro("_Post_equal_to_")
#pragma pop_macro("_Out_writes_bytes_to_")
#pragma pop_macro("_Out_opt_")
#pragma pop_macro("_Out_")
#pragma pop_macro("_In_reads_bytes_")
#pragma pop_macro("_In_")
#endif // #if !defined(_WIN32)
