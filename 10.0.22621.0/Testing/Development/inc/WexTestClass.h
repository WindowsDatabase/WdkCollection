//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Test Authoring and Execution Framework native macro definitions</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#if defined(_MSC_VER) && _MSC_VER < 1910
# error Unsupported version of Visual Studio. WexTestClass.h requires VS 2017 or later.
#endif

#pragma warning(push)
#pragma warning(disable:4481)

#if defined(_WIN32)
# pragma comment(lib, "TE.Common.lib")
# pragma comment(lib, "Wex.Common.lib")
# pragma comment(lib, "Wex.Logger.lib")
#endif

// Allow anyone who has defined an Assert macro to compile with this header file included
#pragma push_macro("Assert")
#undef Assert

#include "Interruption.h"
#include "RuntimeParameters.h"
#include "TE.Common.h"
#include "TestData.h"
#include "Verify.h"
#include "WexAssert.h"
#include "WexDebug.h"

#if !defined(_WIN32)
#pragma push_macro("_In_")
#pragma push_macro("_In_reads_z_")
#pragma push_macro("_Outptr_")
#pragma push_macro("_Outptr_result_maybenull_z_")
#pragma push_macro("_Post_invalid_")
#pragma push_macro("_Ret_maybenull_z_")
#if !defined(_In_)
#define _In_
#endif
#if !defined(_In_reads_z_)
#define _In_reads_z_(x)
#endif
#if !defined(_Outptr_)
#define _Outptr_
#endif
#if !defined(_Outptr_result_maybenull_z_)
#define _Outptr_result_maybenull_z_
#endif
#if !defined(_Post_invalid_)
#define _Post_invalid_
#endif
#if !defined(_Ret_maybenull_z_)
#define _Ret_maybenull_z_
#endif
#endif

#if defined(_WIN32)
// excpt.h is used for GetExceptionCode, which is used to detect C++ exceptions in a structured exception handler.
#include <excpt.h>
#endif // #if defined(_WIN32)

#if defined(__cpp_exceptions)
# include "WexException.h"

# include <new>
#endif

/// \internal
/// <summary> Wide string conversion helper </summary>
#define TAEF_WIDEN_INT(x) L ## x

/// \internal
/// <summary> Wide string version of "stringizing" operator </summary>
#define TAEF_WIDEN(x) TAEF_WIDEN_INT(x)

/// \internal
/// <summary>Stringize internal macro </summary>
#define TAEF_STRINGIZE_INT(x) #x

/// \internal
/// <summary>Stringize macro </summary>
#define TAEF_STRINGIZE(x) TAEF_STRINGIZE_INT(x)

// TAEF_ABI_VERSION contains the version number of the binary interface that the native test loader depends on in order to parse the test module.
#if defined(TAEF_ABI_VERSION)
// TAEF_ABI_VERSION was user-defined.
# if (TAEF_ABI_VERSION != 10) && (TAEF_ABI_VERSION != 11) && (TAEF_ABI_VERSION != 12) && (TAEF_ABI_VERSION != 13)
#  error TAEF_ABI_VERSION is set to an unsupported value. This version of WexTestClass.h only supports ABI versions 10 through 13.
# endif
# if (TAEF_ABI_VERSION == 10) && (defined(__clang__) || !defined(_WIN32))
#  error TAEF ABI version 10 is no longer supported unless building with MSVC on Windows. Please switch to ABI version 13 or leave TAEF_ABI_VERSION undefined.
# endif
# if (TAEF_ABI_VERSION == 11) && !defined(_WIN32)
#  error TAEF ABI version 11 is only supported when building for Windows. Please switch to ABI version 13 or leave TAEF_ABI_VERSION undefined.
# endif
#else
// TAEF_ABI_VERSION is not defined. Set it to the default value.
// ARM64EC and ARM64X require ABI version 13; so we default to that for ARM64 and ARM64EC builds. Other architectures still default to 12 for now,
// but the default will increase to 13 or greater in an upcoming TAEF version.
# if defined(_M_ARM64) || defined(_M_ARM64EC)
#  define TAEF_ABI_VERSION 13
# else
#  define TAEF_ABI_VERSION 12
# endif
#endif

#if (TAEF_ABI_VERSION < 13) && defined(_M_ARM64EC)
# error TAEF_ABI_VERSION must be defined to 13 in order to build tests as ARM64EC or ARM64X.
#endif

#if TAEF_ABI_VERSION == 10
#define TAEF_WIDEN_IF_ABI_10(x) TAEF_WIDEN_INT(x)
#else
#define TAEF_WIDEN_IF_ABI_10(x) x
#endif

/// \internal
/// <summary>The magic number that leads testdata header. This is "TAEF" in ascii.</summary>
#define TAEF_HEADER_SIGNATURE 0x46454154

// For non-Windows platforms, we force the INLINE_TEST_METHOD_MARKUP setting to be enabled. This is opt-in
// for Windows for compatibility reasons. We would prefer to have it be enabled for all tests.
#if !defined(_WIN32) && !defined(INLINE_TEST_METHOD_MARKUP)
#define INLINE_TEST_METHOD_MARKUP
#endif

#if defined(INLINE_TEST_METHOD_MARKUP)
#define TAEF_TERMINATOR
#else
#define TAEF_TERMINATOR ;
#endif

#if defined(__cpp_inline_variables) && __cpp_inline_variables && !defined(TAEF_AVOID_INLINE_STATICS) && !defined(TAEF_USE_INLINE_STATICS)
# define TAEF_USE_INLINE_STATICS
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr && !defined(TAEF_AVOID_CONSTEXPR) && !defined(TAEF_USE_CONSTEXPR)
# define TAEF_USE_CONSTEXPR
#endif

#if defined(TAEF_USE_CONSTEXPR)
// We mark the testdata section as read-only memory. The structs we place inside this section need to avoid dynamic initializers for two reasons:
// * TAEF reflects over the DLL by reading directly from the testdata section without actually loading the DLL for execution. A dynamic initializer
//   will make it so the data is not written to the testdata section until the function containing the dynamic initializer is run.
// * Since the testdata section is marked as read-only, the dynamic initializer will crash when it tries to write to the testdata section.
// The test code can cause a dyanmic initializer to be used by doing something like setting metadata to a string that is the return value of
// a function. Using constexpr causes these cases to be compiler errors. When constexpr is not available, we fall back to const although that does
// not cause build breaks. Instead, you will get dynamic initializers which may result in crashes when the DLL loads.
# define TAEF_CONST_TESTDATA constexpr
#else
# define TAEF_CONST_TESTDATA const
#endif

#if defined(_MSC_VER)
#define TAEF_MSVC_SUPPRESS_WARNING(warningNumber) __pragma(warning(suppress:warningNumber))
#else
#define TAEF_MSVC_SUPPRESS_WARNING(warningNumber)
#endif

// The WEX::Private namespace contains internal implementation details of TAEF.
// Please do not use the contents of this namespace directly. They are referenced
// via the public macros that tests should use.
namespace WEX { namespace Private
{
    // This function ignores its parameter and returns 0, but the compiler and linker don't
    // know that since it is a DLL import. Its purpose is to prevent the compiler and linker
    // from optimizing away any values passed to this function since they will be exposed to
    // a separate DLL.
    extern "C" WEXLOGGER_API int WEXLOGGER_STDCALL DoNotDiscard(const void*);

#if defined(_WIN32)
    /// \internal
    /// <summary>
    /// A SEH filter.
    /// </summary>
    unsigned long inline DetectMSVCExceptions(unsigned long exceptionCode, bool& isCPlusPlusException)
    {
        // If the 'exceptionCode' is 0xE06D7363 (which is the exception code that MSVC uses to implement
        // C++ exceptions using SEH), then this is a C++ exception. Set isCPlusPlusException to indicate
        // this. This is used by the catch(...) handler which does a fast-fail if it is not a C++
        // exception. The catch(...) will only be invoked for non-C++ exceptions if the /EHa switch is
        // used on the compiler while compiling the test DLL.
        isCPlusPlusException = (exceptionCode == 0xE06D7363);

#if !defined(__cpp_exceptions)
        if (isCPlusPlusException)
        {
            ::WEX::Logging::Log::Error(L"A C++ exception was throw by a test in a DLL that was compiled with C++ exceptions disabled.");
            // In this case, if we let the exception continue the search, it would be caught by TAEF code. We want to avoid
            // catching the exception so the process can crash with the right context.
                
            // To fix this in a Visual Studio project, change the Configuration Properties -> C/C++ ->
            // Code Generation -> Enable C++ Exceptions setting in the project properties from "No" to "Yes (/EHsc)".
            // To fix this in the build.exe-based build system from the old Windows Driver Kit, add
            // "USE_NATIVE_EH=1" to the sources file.
            TAEF_FAST_FAIL;
        }
#endif

        return EXCEPTION_CONTINUE_SEARCH;
    }

    template <typename TFunctor>
    bool SafeInvoke_Impl(const TFunctor& functor, bool& isCPlusPlusException)
    {
        bool result = false;
        __try
        {
            result = functor();
        }
        __except (::WEX::Private::DetectMSVCExceptions(GetExceptionCode(), isCPlusPlusException))
        {
            // This is unreachable because DetectMSVCExceptions returns EXCEPTION_CONTINUE_SEARCH.
            TAEF_FAST_FAIL;
        }
        return result;
    }
#endif // #if defined(_WIN32)

#if TAEF_ABI_VERSION < 12
    struct StringManager
    {
        template <size_t Size>
        _Ret_maybenull_z_ static wchar_t* Allocate(const wchar_t (&szLiteral)[Size]) noexcept
        {
            return Allocate(szLiteral, Size - 1);
        }

        _Ret_maybenull_z_ static wchar_t* Allocate(const ::WEX::Common::NoThrowString& str) noexcept
        {
            if (str.IsValid())
            {
                return Allocate(str, static_cast<size_t>(str.GetLength()));
            }

            return nullptr;
        }

        _Ret_maybenull_z_ static wchar_t* Allocate(_In_reads_z_(length+1) const wchar_t* pszString, size_t length) noexcept
        {
            WEX_ASSERT(pszString[length] == L'\0', L"The string is not null-terminated.");

            const size_t allocationSize = sizeof(wchar_t) * (length + 1);
            wchar_t* pszRet = nullptr;
#if defined(__cpp_exceptions)
            try
            {
#endif
                // We aren't using "new wchar_t[length+1]" as that caused some linking problems for some existing tests.
                pszRet = static_cast<wchar_t*>(::operator new(allocationSize));
#if defined(__cpp_exceptions)
            }
            catch (const std::exception&)
            {
                // If new fails, pszRet will be null.
            }
#endif

            if (pszRet)
            {
                WEX::TestExecution::Private::Memcpy(pszRet, allocationSize, pszString, allocationSize);
            }

            return pszRet;
        }

        static void TAEF_CDECL Deallocate(_In_ _Post_invalid_ wchar_t* pszString)
        {
            ::operator delete(static_cast<void*>(pszString));
        }
    };
#endif // #if TAEF_ABI_VERSION < 12

    /// \internal
    /// <summary>
    /// Identifiers for the structs that make up the native ABI.
    /// </summary>
#if !defined(__cplusplus_cli)
    enum class TaefAbiStructIdentifier : uintptr_t
#else
    enum TaefAbiStructIdentifier : uintptr_t
#endif
    {
        TestClass = 1,
        TestMethod,
        ModuleSetup,
        ModuleCleanup,
        TestClassSetup,
        TestClassCleanup,
        TestMethodSetup,
        TestMethodCleanup,
        ModuleMetadata,
        TestClassMetadata,
        TestMethodMetadata,
        ModuleMetadataNarrow,
        TestClassMetadataNarrow,
        TestMethodMetadataNarrow,
    };

#if !defined(__cplusplus_cli)
    enum class TaefProcessorArchitecture : uintptr_t
#else
    enum TaefProcessorArchitecture : uintptr_t
#endif
    {
        Taef_Unrecognized,
        Taef_X86,
        Taef_X86_64,
        Taef_ARM,
        Taef_ARM64,
        Taef_ARM64EC,
    };

    template <typename TFunctor>
    HRESULT SafeInvoke(const TFunctor& functor)
    {
        // This is set to false if a structured exception which is not a C++ exception is raised by the test or fixture method.
#if defined(_WIN32)
        bool exceptionIsCPlusPlusException = true;
#endif // #if defined(_WIN32)
#if defined(__cpp_exceptions)
        try
        {
#endif
#if defined(_WIN32)
            return ::WEX::Private::SafeInvoke_Impl(functor, exceptionIsCPlusPlusException)
#else
            return functor()
#endif
                ? TAEF_S_OK : TAEF_S_FALSE;
#if defined(__cpp_exceptions)
        }
# if !defined(NO_VERIFY_EXCEPTIONS)
        catch (const ::WEX::TestExecution::VerifyFailureException& e)
        {
            return e.ErrorCode();
        }
# endif
        catch (...)
        {            
# if defined(_WIN32)
            // exceptionIsCPlusPlusException will always be true if the test is not compiled with /EHa.
            if (!exceptionIsCPlusPlusException)
            {
                // This catch (...) just caught a structured exception which is not a C++ exception.
                // This means the test DLL was compiled using the /EHa switch on the compiler.
                // See https://msdn.microsoft.com/en-us/library/1deeycx5.aspx for the /EH documentation.
                // The use of /EHa is discouraged. Please consider switching to /EHsc, which instructs
                // the compiler to only catch C++ exceptions with catch (...) and to assume that functions
                // declared as extern "C" cannot throw.
                // 
                // To fix this in a Visual Studio project, change the Configuration Properties -> C/C++ ->
                // Code Generation -> Enable C++ Exceptions setting in the project properties from
                // "Yes with SEH Exceptions (/EHa)" to "Yes (/EHsc)".
                // To fix this in the build.exe-based build system from the old Windows Driver Kit, change
                // "USE_NATIVE_EH=ASYNC" to "USE_NATIVE_EH=1" in the sources file.
                TAEF_FAST_FAIL;
            }
# endif // #if defined(_WIN32)

            ::WEX::TestExecution::Private::StringStorage messageBuffer;
            HRESULT hr = TAEF_S_OK;
            ::WEX::Logging::Log::Error(::WEX::TestExecution::Private::GetExceptionInfo(messageBuffer, &hr));

            return hr;
        }
#endif // if defined(__cpp_exceptions)
    }

    #pragma warning(push)
    #pragma warning(disable:4702) // "unreachable code"
    #pragma warning(disable:6101)
    template <typename T>
    struct TestClassFactory
    {
        /// \internal
        /// <summary>
        /// Creates an instance of a test class
        /// </summary>
        static HRESULT TAEF_CDECL CreateInstance(_Outptr_ void** ppInstance,
#if TAEF_ABI_VERSION >= 12
            _Outptr_result_maybenull_z_ char** ppszMessage)
#else
            _Outptr_result_maybenull_z_ wchar_t** ppszMessage)
#endif
        {
            *ppszMessage = nullptr;
#if defined(_WIN32)
            // This is set to false if a structured exception which is not a C++ exception is raised by the class constructor.
            bool exceptionIsCPlusPlusException = true;
#endif // #if defined(_WIN32)
#if defined(__cpp_exceptions)
            try
            {
#endif
                *ppInstance =
#if defined(_WIN32)
                    CreateInstanceWithTryExcept(exceptionIsCPlusPlusException);
#else
                    CallDefaultConstructor();
#endif
                if (!*ppInstance)
                {
#if TAEF_ABI_VERSION >= 12
                    *ppszMessage = ::WEX::TestExecution::Private::StringStorage{"Failed to allocate the test class"}.Detach();
#else
                    *ppszMessage = ::WEX::Private::StringManager::Allocate(L"Failed to allocate the test class");
#endif
                    return TAEF_E_OUTOFMEMORY;
                }

                return TAEF_S_OK;
#if defined(__cpp_exceptions)
            }
# if TAEF_ABI_VERSION < 12
#  if !defined(NO_VERIFY_EXCEPTIONS)
            catch (const ::WEX::TestExecution::VerifyFailureException&)
            {
                *ppszMessage = ::WEX::Private::StringManager::Allocate(L"Verify failure");
                return TAEF_E_FAIL;
            }
#  endif // #if !defined(NO_VERIFY_EXCEPTIONS)
#  if defined(_WIN32)
            catch (const ::WEX::Common::Exception& ex)
            {
                *ppszMessage = ::WEX::Private::StringManager::Allocate(
                    ::WEX::Common::NoThrowString().Format(L"Caught WEX::Common::Exception: %s", ex.Message()));
                return ex.ErrorCode();
            }
#  endif // #if defined(_WIN32)
#  if defined(__cplusplus_winrt)
            catch (Platform::Exception^ ex)
            {
                *ppszMessage = ::WEX::Private::StringManager::Allocate(
                    ::WEX::Common::NoThrowString().Format(L"Caught Platform::Exception^: %s", ex->Message->Data()));
                return ex->HResult;
            }
#  endif // #if defined(__cplusplus_winrt)
#  if defined(TAEF_CPPWINRT_SUPPORT)
            catch (const winrt::hresult_error& ex)
            {
                *ppszMessage = ::WEX::Private::StringManager::Allocate(
                    ::WEX::Common::NoThrowString().Format(L"Caught winrt::hresult_error: %s", ex.message().c_str()));
                return ex.code();
            }
#  endif // #if defined(TAEF_CPPWINRT_SUPPORT)
#  if defined(TAEF_SYSTEM_ERROR_SUPPORT)
            catch (const std::system_error& ex)
            {
                bool isExactConversion;
                const HRESULT hr = ::WEX::TestExecution::Private::GetHResultForStdErrorCode(ex.code(), isExactConversion);

                const char* const pszExceptionMessage = ex.what();
                size_t messageLength = ::strlen(pszExceptionMessage);
                // std::system_error often ends its messages with a new-line on Windows. Exclude that from the length if it is present.
                while (messageLength > 0 && ::isspace(static_cast<unsigned char>(pszExceptionMessage[messageLength - 1])))
                {
                    --messageLength;
                }
                if (messageLength > INT_MAX)
                {
                    messageLength = INT_MAX;
                }

                // The HRESULT will be included in the final error message, so we only mention the error code if
                // the HRESULT was not an exact conversion.
                ::WEX::Common::NoThrowString message;
                if (isExactConversion)
                {
                    message.Format(L"Caught std::system_error: %.*S",
                        static_cast<int>(messageLength), pszExceptionMessage);
                }
                else
                {
                    message.Format(L"Caught std::system_error: %.*S [Error code: %d]",
                        static_cast<int>(messageLength), pszExceptionMessage, ex.code().value());
                }
                *ppszMessage = ::WEX::Private::StringManager::Allocate(message);
                return hr;
            }
#  endif // #if defined(TAEF_SYSTEM_ERROR_SUPPORT)
            catch (const std::exception& ex)
            {
                *ppszMessage = ::WEX::Private::StringManager::Allocate(
                    ::WEX::Common::NoThrowString().Format(L"Caught std::exception: %S", ex.what()));
                return TAEF_E_FAIL;
            }
# endif // TAEF_ABI_VERSION < 12
            catch (...)
            {
# if defined(_WIN32)
                // exceptionIsCPlusPlusException will always be true if the test is not compiled with /EHa.
                if (!exceptionIsCPlusPlusException)
                {
                    // This catch (...) just caught a structured exception which is not a C++ exception.
                    // This means the test DLL was compiled using the /EHa switch on the compiler.
                    // See https://msdn.microsoft.com/en-us/library/1deeycx5.aspx for the /EH documentation.
                    // The use of /EHa is discouraged. Please consider switching to /EHsc, which instructs
                    // the compiler to only catch C++ exceptions with catch (...) and to assume that functions
                    // declared as extern "C" cannot throw.
                    // 
                    // To fix this in a Visual Studio project, change the Configuration Properties -> C/C++ ->
                    // Code Generation -> Enable C++ Exceptions setting in the project properties from
                    // "Yes with SEH Exceptions (/EHa)" to "Yes (/EHsc)".
                    // To fix this in the build.exe-based build system from the old Windows Driver Kit, change
                    // "USE_NATIVE_EH=ASYNC" to "USE_NATIVE_EH=1" in the sources file.
                    TAEF_FAST_FAIL;
                }
# endif // #if defined(_WIN32)

                // For TAEF_ABI_VERSION >= 12, we only have a "catch (...)". We call GetExceptionInfo from Verify.h to get the exception details.
                // For TAEF_ABI_VERSION < 12, we have already caught more specific exception types above. The "catch (...)" is just for unrecognized
                // exception types in this case.
# if TAEF_ABI_VERSION >= 12
                ::WEX::TestExecution::Private::StringStorage messageStorage;
                HRESULT exceptionHResult;
                const char* pszMessage = reinterpret_cast<const char*>(::WEX::TestExecution::Private::GetExceptionInfo(messageStorage,
                    &exceptionHResult, false /* includeHResultInMessage */));
                // Use the message from the messageStorage so it can be freed with the string deallocator in the test data header.
                // If messageStorage is already populated, pszMessage is already pointing to the buffer within messageStorage;
                // however, GetExceptionInfo just returns string literals in some cases without populating messageStorage. We need
                // to use messageStorage all the time.
                if (!messageStorage.Data())
                {
                    messageStorage.Set(pszMessage);
                }
                *ppszMessage = messageStorage.Detach();
                return exceptionHResult;
# else
                *ppszMessage = ::WEX::Private::StringManager::Allocate(L"Caught an unidentified C++ exception.");
                return TAEF_E_UNEXPECTED;
# endif
            }
#endif // if defined(__cpp_exceptions)
        }

#if defined(_WIN32)
        /// \internal
        /// <summary>
        /// Creates an instance of a test class
        /// </summary>
        static T* TAEF_CDECL CreateInstanceWithTryExcept(bool& exceptionIsCPlusPlusException)
        {
            T* pInstance = nullptr;
            __try
            {
                // We have to call the constructor in a separate function to avoid error C2712.
                pInstance = CallDefaultConstructor();
            }
            __except (::WEX::Private::DetectMSVCExceptions(GetExceptionCode(), exceptionIsCPlusPlusException))
            {
                // This is unreachable because DetectMSVCExceptions returns EXCEPTION_CONTINUE_SEARCH.
                TAEF_FAST_FAIL;
            }
            return pInstance;
        }
#endif // #if !defined(_WIN32)

        static T* TAEF_CDECL CallDefaultConstructor()
        {
            // What the caller links to controls the throwing behavior. We would prefer to use
            // new (std::nothrow) T(), but that doesn't link for some existing test binaries; so
            // we just use plain new.
            return new T();
        }

        /// \internal
        /// <summary>
        /// Destroys an instance of a test class
        /// </summary>
        static void TAEF_CDECL DestroyInstance(void* pTestClass)
        {
            delete static_cast<T*>(pTestClass);
        }
    };
    #pragma warning(pop)

    template <class T>
    struct TestInvokeFunctor
    {
        typedef void (TAEF_THISCALL T::*TestMethod)();
    public:
        TestInvokeFunctor(T& instance, TestMethod pTestMethod)
            : m_pTestMethod(pTestMethod)
            , m_instance(instance)
        {}

        bool operator() () const
        {
#if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)
            try
            {
#endif
                (m_instance.*m_pTestMethod)();
#if defined(__cpp_exceptions) && !defined(NO_VERIFY_EXCEPTIONS)
            }
            catch (const ::WEX::TestExecution::VerifyFailureException&)
            {
                // Don't re-throw here. The error is already tracked in WEX.Logger.
            }
#endif

            return true;
        }

    private:
        TestInvokeFunctor(const TestInvokeFunctor&) = delete;
        TestInvokeFunctor& operator=(const TestInvokeFunctor&) = delete;

        // Store the pointer-to-member before the reference to avoid warning C4121 for tests
        // that use #pragma pointers_to_members(full_generality, virtual_inheritance).
        TestMethod m_pTestMethod;
        T& m_instance;
    };

    template <class T>
    struct FixtureInvokeFunctor
    {
        typedef bool (TAEF_THISCALL T::*FixtureMethod)();
    public:
        FixtureInvokeFunctor(T& instance, FixtureMethod pFixtureMethod)
            : m_pFixtureMethod(pFixtureMethod)
            , m_instance(instance)
        {}

        bool operator() () const
        {
            return (m_instance.*m_pFixtureMethod)();
        }

    private:
        FixtureInvokeFunctor(const FixtureInvokeFunctor&) = delete;
        FixtureInvokeFunctor& operator=(const FixtureInvokeFunctor&) = delete;

        // Store the pointer-to-member before the reference to avoid warning C4121 for tests
        // that use #pragma pointers_to_members(full_generality, virtual_inheritance).
        FixtureMethod m_pFixtureMethod;
        T& m_instance;
    };

    template <typename NameCharType, typename ValueCharType>
    struct MetadataIdentifierLookup;

    template <typename NameCharType, typename ValueCharType>
    struct MetadataIdentifierLookup<const NameCharType&, const ValueCharType&>
    {
        static_assert(sizeof(NameCharType) == sizeof(ValueCharType),
            "Invalid mix of character sizes in TAEF metadata. The name and value must use the same Unicode encoding type.");
#if defined(_WIN32)
        static_assert(sizeof(NameCharType) == 1 || sizeof(NameCharType) == 2,
            "Unsupported character sizes in TAEF metadata. Metadata strings must be either UTF-8 or UTF-16.");
#else
        static_assert(sizeof(NameCharType) == 1,
            "Unsupported character sizes in TAEF metadata. Metadata strings must be UTF-8 for non-Windows builds.");
#endif

        static const ::WEX::Private::TaefAbiStructIdentifier c_moduleIdentifier = sizeof(NameCharType) == 1
            ? ::WEX::Private::TaefAbiStructIdentifier::ModuleMetadataNarrow
            : ::WEX::Private::TaefAbiStructIdentifier::ModuleMetadata;
        static const ::WEX::Private::TaefAbiStructIdentifier c_classIdentifier = sizeof(NameCharType) == 1
            ? ::WEX::Private::TaefAbiStructIdentifier::TestClassMetadataNarrow
            : ::WEX::Private::TaefAbiStructIdentifier::TestClassMetadata;
        static const ::WEX::Private::TaefAbiStructIdentifier c_methodIdentifier = sizeof(NameCharType) == 1
            ? ::WEX::Private::TaefAbiStructIdentifier::TestMethodMetadataNarrow
            : ::WEX::Private::TaefAbiStructIdentifier::TestMethodMetadata;
    };

    /// \internal
    /// <summary>
    /// Metadata storage classes
    /// Store various metadata about test classes in binary's "testdata" section
    /// </summary>

    /// \internal
    /// <summary>
    ///  Test class related metadata storage class
    ///  Resolves a class name to pointers to class creation and destruction functions
    /// </summary>
    struct TestClassInfo
    {
        using ClassCreatorWithUtf8ErrorString = HRESULT(TAEF_CDECL*)(_Outptr_ void** ppInstance, _Outptr_result_maybenull_z_ char** ppszMessage);
        using ClassCreatorWithWideErrorString = HRESULT(TAEF_CDECL*)(_Outptr_ void** ppInstance, _Outptr_result_maybenull_z_ wchar_t** ppszMessage);

        /// \internal
        /// <summary> Pointer to a class 'creation' function</summary>
#if TAEF_ABI_VERSION >= 12
        using ClassCreator = ClassCreatorWithUtf8ErrorString;
#else
        using ClassCreator = ClassCreatorWithWideErrorString;
#endif

        /// \internal
        /// <summary> Pointer to a class 'destroyer' function</summary>
        using ClassDestroyer = void(TAEF_CDECL*)(void*);

        TaefAbiStructIdentifier m_identifier;
#if TAEF_ABI_VERSION >= 11
        const char* m_pszClassName;
#else
        const wchar_t* m_pszClassName;
#endif

        ClassCreator m_pfnClassCreator;
        ClassDestroyer m_pfnClassDestroyer;
    };

    typedef HRESULT (TAEF_CDECL *MethodInvokerFunction)(void* pTestClass);

    /// \internal
    /// <summary>
    /// Holds test method information that is relevant to the test loader.
    /// </summary>
    struct TestMethodInfo
    {
        TaefAbiStructIdentifier m_identifier;
#if TAEF_ABI_VERSION >= 11
        const char* m_pszMethodName;
        const char* m_pszQualifiedMethodName;
#else
        const wchar_t* m_pszMethodName;
        const wchar_t* m_pszQualifiedMethodName;
#endif
        uintptr_t m_index;
        MethodInvokerFunction m_pfnInvoker;
    };

    /// \internal
    /// <summary>
    /// Holds test or class setup or cleanup method information that is relevant to the test loader.
    /// </summary>
    struct FixtureMethodInfo
    {
        TaefAbiStructIdentifier m_identifier;
#if TAEF_ABI_VERSION >= 11
        const char* m_pszMethodName;
        const char* m_pszQualifiedMethodName;
#else
        const wchar_t* m_pszMethodName;
        const wchar_t* m_pszQualifiedMethodName;
#endif
        MethodInvokerFunction m_pfnInvoker;
    };

    /// \internal
    /// <summary>
    ///  Holds various types of properties
    ///  (module, class or function level)
    ///  pszMetadataIdentifier identifies what kind of property is stored
    /// </summary>
    struct TestPropertyMetadata
    {
        TaefAbiStructIdentifier m_identifier;
#if TAEF_ABI_VERSION >= 11
        // Either "const wchar_t*" or "const char*", depending on the identifier.
        const void* m_pszPropertyName;
        const void* m_pszPropertyValue;
#else
        const wchar_t* m_pszPropertyName;
        const wchar_t* m_pszPropertyValue;
#endif
    };

    /// \internal
    /// <summary>
    /// Dispatches module setup and cleanup function names to their corresponding pointers
    /// </summary>
    struct TestGlobalFunctionInfo
    {
        using TestModuleInvokerFunction = HRESULT(TAEF_CDECL*)();

        TaefAbiStructIdentifier m_identifier;
#if TAEF_ABI_VERSION >= 11
        const char* m_pszFunctionName;
#else
        const wchar_t* m_pszFunctionName;
#endif
        TestModuleInvokerFunction m_pfnInvoker;
    };

    /// \internal
    /// <summary>
    /// Holds framework version info
    /// </summary>
    struct TestVersionInfo
    {
        using DeallocatorForUtf8Strings = void(TAEF_CDECL*)(_In_ _Post_invalid_ char* pStr);
        using DeallocatorForWideStrings = void(TAEF_CDECL*)(_In_ _Post_invalid_ wchar_t* pStr);

#if TAEF_ABI_VERSION >= 12
        using Deallocator = DeallocatorForUtf8Strings;
#else
        using Deallocator = DeallocatorForWideStrings;
#endif

        uintptr_t m_headerSignature;
        uintptr_t m_abiVersion;
        const char* m_pszGenerator;
#if TAEF_ABI_VERSION >= 13
        TaefProcessorArchitecture m_processorArchitecture;
#endif
        Deallocator m_pfnDeallocator;
    };
}/* namespace Private */}/* namespace WEX */

// Add types to the WEX namespace that are needed for compatibility.
namespace WEX
{
    // TAEF test classes used to be required to inherit from this class. There is no need at all to
    // use it anymore, but it is declared here to prevent build breaks for tests that still use it.
    template <class T> class TestClass {};

    // Some tests take dependencies on internal TAEF implementation details. These definitions are
    // here to enable those tests to continue to build. Please do not use these in new tests.
#if defined(TAEF_INCLUDE_WEX_DECLARATIONS_FOR_COMPAT)
    namespace TAEF_Identifier
    {
        enum Value : uintptr_t
        {
            TestClassInfo = 1, TestMethodInfo, ModuleSetup, ModuleCleanup, TestClassSetup, TestClassCleanup,
            TestMethodSetup, TestMethodCleanup, ModuleMetadata, TestClassMetadata, TestMethodMetadata
        };
    }
    template <class T> using FixtureInvokeFunctor = ::WEX::Private::FixtureInvokeFunctor<T>;
    using TestClassInfo = ::WEX::Private::TestClassInfo;
    using TestMethodInfo = ::WEX::Private::TestMethodInfo;
    using FixtureMethodInfo = ::WEX::Private::FixtureMethodInfo;
    using TestPropertyMetadata = ::WEX::Private::TestPropertyMetadata;
    using TestGlobalFunctionInfo = ::WEX::Private::TestGlobalFunctionInfo;
    using TestVersionInfo = ::WEX::Private::TestVersionInfo;
#endif

    // TAEF__WFUNCTION__ was formerly used by TAEF's implementation. Many tests took a dependency on it; so it is
    // preserved here to enable those tests to continue to build. If your test was using this just to pass the file,
    // function, and line number to APIs in the WEX::Logging::Log class, use the WEX_LOGGER_CURRENT_SOURCE_INFO
    // macro instead. It expands to the file, function, and line number separated by commas so it can be passed to
    // the Log APIs. The VC++ CRT also has an equivalent macro named __FUNCTIONW__.
#if defined(_MSC_VER) && !defined(__clang__)
#define TAEF__WFUNCTION__ TAEF_WIDEN(__FUNCTION__)
#endif

    // SafeInvoke is used more frequently than some of the other TAEF internals; so we include it
    // in the WEX namespace without tests having to opt-in.
    template <typename TFunctor>
    HRESULT SafeInvoke(const TFunctor& functor)
    {
        return ::WEX::Private::SafeInvoke(functor);
    }
}/* namespace WEX */

#if defined(_WIN32)
#pragma section("testdata$a_TDH", read)
#pragma section("testdata$b_TCI", read)
#pragma section("testdata$c_TMI", read)
#pragma section("testdata$d_TGFI", read)
#pragma section("testdata$e_FMI", read)
#pragma section("testdata$f_MM", read)
#pragma section("testdata$g_TCM", read)
#pragma section("testdata$h_TMM", read)

#define TAEF_TESTDATA_SECTION2(sectionName)         __declspec(allocate(#sectionName))
#define TAEF_TESTDATA_SECTION(sectionSuffix)        TAEF_TESTDATA_SECTION2(testdata ## sectionSuffix)

#else
# if defined(__clang__)
#  define TAEF_TESTDATA_SECTION(sectionSuffix)        __attribute__((section("testdata")))
# else /* gcc on Linux*/
#  define TAEF_PRIVATE_CAT2(x,y) x##y
#  define TAEF_PRIVATE_CONCAT(x,y) TAEF_PRIVATE_CAT2(x,y)
#  define TAEF_TESTDATA_SECTION(sectionsuffix) __attribute__((section(TAEF_STRINGIZE(TAEF_PRIVATE_CONCAT(testdata., __COUNTER__)))))
# endif
#endif

// The IntelliSense compiler reports errors when trying to parse 'TAEF_PIN_FUNCTION_SYMBOL', so it should be skipped.
#if defined(__INTELLISENSE__)
# define TAEF_PIN_FUNCTION_SYMBOL
#elif defined(__clang__) || defined(__GNUC__)
/// \internal
/// <summary> Instruct the compiler not to remove the function from the obj file during the optimization phase </summary>
# define TAEF_PIN_FUNCTION_SYMBOL                 __attribute__((used)) 
#else
/// \internal
/// <summary> Instruct the compiler not to remove the function from the obj file during the optimization phase </summary>
#if defined(TAEF_USE_INLINE_STATICS)
# define TAEF_PIN_FUNCTION_SYMBOL
#else
# define TAEF_PIN_FUNCTION_SYMBOL                 __pragma (comment(linker, "/include:" __FUNCDNAME__))
#endif
#endif

#if defined(_MSC_VER) && !defined(__clang__)
# define TAEF_QUALIFIED_METHOD_NAME TAEF_WIDEN_IF_ABI_10(__FUNCTION__)
#else
# define TAEF_QUALIFIED_METHOD_NAME __PRETTY_FUNCTION__
#endif

#if defined(_MSC_VER)
// Ignore the following warnings:
// * 28651: static initializer causes copy on write pages due to member function pointers
// * 4640: construction of local static object is not thread-safe
// * 4407: cast between different pointer to member representations, compiler may generate incorrect code
#define TAEF_PUSH_IGNORE_WARNINGS                __pragma (warning(push)) \
                                                 __pragma (warning(disable:28651)) \
                                                 __pragma (warning(disable:4640)) \
                                                 __pragma (warning(disable:4407))

#define TAEF_POP_IGNORE_WARNINGS                 __pragma (warning(pop))
#else
#define TAEF_PUSH_IGNORE_WARNINGS
#define TAEF_POP_IGNORE_WARNINGS
#endif

#if defined(__INTELLISENSE__)
// The IntelliSense compiler reports errors when trying to parse TAEF_REGISTER_FIXTURE_METHOD and TAEF_REGISTER_TEST_METHOD.
# define TAEF_REGISTER_FIXTURE_METHOD(methodName, methodType, registrationMethodName)
# define TAEF_REGISTER_TEST_METHOD(methodName)
#else

// TAEF_REGISTER_FIXTURE_METHOD uses the same value for methodName and methodToInvoke. The macros that start with "TAEF_" in this file are not meant
// for tests to use directly. However, some tests took a dependency on TAEF internals to enable the use of a separate wrapper function that invokes
// the actual fixture. This macro was added so those tests could be updated to use this macro instead of having their own copies of the TAEF macros.
# define TAEF_REGISTER_FIXTURE_METHOD_IMPL(methodName, methodToInvoke, methodType, registrationMethodName) \
    static TAEF_CPP_EXPORT const ::WEX::Private::FixtureMethodInfo* registrationMethodName() \
    { \
        struct TAEF_FixtureInvoker \
        { \
            static HRESULT TAEF_CDECL TAEF_Invoke(void* pTestClass) \
            { \
                /* TAEF_TestClassType was typedef'd in the TEST_CLASS macro so we can safely cast from \
                   void* to TAEF_TestClassType* and invoke the correct derived type's member functions */ \
                return ::WEX::Private::SafeInvoke(::WEX::Private::FixtureInvokeFunctor<TAEF_TestClassType>( \
                    *reinterpret_cast<TAEF_TestClassType*>(pTestClass), &TAEF_TestClassType::methodToInvoke)); \
            } \
        }; \
        TAEF_PUSH_IGNORE_WARNINGS \
        TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($e_FMI)  \
        static TAEF_CONST_TESTDATA ::WEX::Private::FixtureMethodInfo s_testInfo = { \
            methodType, TAEF_WIDEN_IF_ABI_10(#methodName), TAEF_QUALIFIED_METHOD_NAME, &TAEF_FixtureInvoker::TAEF_Invoke}; \
        TAEF_POP_IGNORE_WARNINGS \
        return &s_testInfo; /* Return a pointer to s_testInfo in order to pin the struct into the dll so it does not get stripped out by the linker. */ \
    } \
    TAEF_COMPILER_PIN(registrationMethodName)

# define TAEF_REGISTER_FIXTURE_METHOD(methodName, methodType, registrationMethodName) \
    TAEF_REGISTER_FIXTURE_METHOD_IMPL(methodName, methodName, methodType, registrationMethodName)

// TAEF_REGISTER_TEST_METHOD uses the same value for methodName and methodToInvoke. The macros that start with "TAEF_" in this file are not meant
// for tests to use directly. However, some tests took a dependency on TAEF internals to enable the use of a separate wrapper function that invokes
// the actual test. This macro was added so those tests could be updated to use this macro instead of having their own copies of the TAEF macros.
# define TAEF_REGISTER_TEST_METHOD_IMPL(methodName, methodToInvoke) \
    static const TAEF_CPP_EXPORT ::WEX::Private::TestMethodInfo* methodName##_TAEF_PinTestMethodInfo() \
    { \
        struct TAEF_TestInvoker \
        { \
            static HRESULT TAEF_CDECL TAEF_Invoke(void* pTestClass) \
            { \
                /* TAEF_TestClassType was typedef'd in the TEST_CLASS macro so we can safely cast from \
                   void* to TAEF_TestClassType* and invoke the correct type's member functions */ \
                return ::WEX::Private::SafeInvoke(::WEX::Private::TestInvokeFunctor<TAEF_TestClassType>( \
                    *reinterpret_cast<TAEF_TestClassType*>(pTestClass), &TAEF_TestClassType::methodToInvoke)); \
            } \
        }; \
        TAEF_PUSH_IGNORE_WARNINGS \
        TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($c_TMI) \
        static TAEF_CONST_TESTDATA ::WEX::Private::TestMethodInfo s_testInfo = { \
            ::WEX::Private::TaefAbiStructIdentifier::TestMethod, TAEF_WIDEN_IF_ABI_10(#methodName), TAEF_QUALIFIED_METHOD_NAME, \
            __COUNTER__ - TAEF_TestMethodIndexOffset, &TAEF_TestInvoker::TAEF_Invoke }; \
        TAEF_POP_IGNORE_WARNINGS \
        return &s_testInfo; /* Return a pointer to s_testInfo in order to pin the struct into the dll so it does not get stripped out by the linker. */ \
    } \
    TAEF_COMPILER_PIN(methodName##_TAEF_PinTestMethodInfo)

# define TAEF_REGISTER_TEST_METHOD(methodName) \
    TAEF_REGISTER_TEST_METHOD_IMPL(methodName, methodName)
#endif

#if defined(TAEF_USE_INLINE_STATICS)
# define TAEF_COMPILER_PIN(methodName) \
    inline static int s_doNotDiscard_##methodName = ::WEX::Private::DoNotDiscard(methodName());

# define TAEF_CPP_EXPORT

# define TAEF_STATIC_IF_CALLED_BY_TAEF_COMPILER_PIN static
#else
# define TAEF_COMPILER_PIN(methodName)

# define TAEF_CPP_EXPORT __declspec(dllexport)

# define TAEF_STATIC_IF_CALLED_BY_TAEF_COMPILER_PIN
#endif

// This macro is used directly by some non-TAEF teams. Do not remove it unless those cases are fixed.
#define TAEF_TEST_METHOD(methodName) \
    TAEF_REGISTER_TEST_METHOD(methodName) \
    TAEF_MSVC_SUPPRESS_WARNING(25007) /* Disable warning that member method may be static */ \
    void methodName()

/// <summary>
/// Macro for declaring a test method without associating it with any metadata
/// </summary>
/// Example:
/// \code
///    class TestFeatureClass
///    {
///         TEST_METHOD(FeatureTestMethod1);
///    }
/// \endcode
#define TEST_METHOD(methodName) \
    TAEF_TEST_METHOD(methodName) TAEF_TERMINATOR

#if TAEF_ABI_VERSION == 10
#define TAEF_MODULE_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::ModuleMetadata
#define TAEF_TEST_CLASS_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::TestClassMetadata
#define TAEF_TEST_METHOD_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::TestMethodMetadata
#else
#define TAEF_MODULE_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::ModuleMetadataNarrow
#define TAEF_TEST_CLASS_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::TestClassMetadataNarrow
#define TAEF_TEST_METHOD_METADATA_IDENTIFIER ::WEX::Private::TaefAbiStructIdentifier::TestMethodMetadataNarrow
#endif

// Do not use TAEF_TEST_METHOD_METADATA_START/END directly. Instead use
// BEGIN_TEST_METHOD/END_TEST_METHOD at the class scope or
// BEGIN_TEST_METHOD_PROPERTIES/END_TEST_METHOD_PROPERTIES inside the test method.

#define TAEF_TEST_METHOD_METADATA_START \
    TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($h_TMM) \
    static TAEF_CONST_TESTDATA ::WEX::Private::TestPropertyMetadata s_Metadata[] = { \
        { TAEF_TEST_METHOD_METADATA_IDENTIFIER, TAEF_WIDEN_IF_ABI_10("Name"), TAEF_QUALIFIED_METHOD_NAME}, \

#define TAEF_TEST_METHOD_METADATA_END \
        { TAEF_TEST_METHOD_METADATA_IDENTIFIER, nullptr, nullptr}}; \
    return s_Metadata;

// TAEF_BEGIN_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE and TAEF_END_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE
// are not intended to be used directly by tests, but some tests do use it; so we preserve it for
// compatibility. Use BEGIN_TEST_METHOD/END_TEST_METHOD instead of this.
#if defined(TAEF_USE_INLINE_STATICS)
# define TAEF_BEGIN_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE(methodName) \
    struct methodName##TAEF_MetadataStruct \
    { \
        inline static int s_doNotDiscard_##methodName##_GetTestMethodMetadata = ::WEX::Private::DoNotDiscard([]() \
        { \
            TAEF_TEST_METHOD_METADATA_START \

# define TAEF_END_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE() \
            TAEF_TEST_METHOD_METADATA_END \
        }()); \
    };
#else
# define TAEF_BEGIN_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE(methodName) \
    static TAEF_CPP_EXPORT const ::WEX::Private::TestPropertyMetadata* methodName ## _GetTestMethodMetadata() \
    { \
        TAEF_TEST_METHOD_METADATA_START \

# define TAEF_END_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE() \
        TAEF_TEST_METHOD_METADATA_END \
    }

#endif

/// <summary>
///  Macro for declaring a test method and associating it with metadata
/// </summary>
///  Example:
/// \code
///      class TestFeatureClass
///      {
///          BEGIN_TEST_METHOD(TestFindNext)
///              TEST_METHOD_PROPERTY(L"Priority", L"2")
///          END_TEST_METHOD()
///      }
/// \endcode
# define BEGIN_TEST_METHOD(methodName) \
    TEST_METHOD(methodName); \
    TAEF_BEGIN_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE(methodName)

/// <summary>
///  Macro that ends test method declaration.
///  Must be used with BEGIN_TEST_METHOD()
/// </summary>
# define END_TEST_METHOD() \
    TAEF_END_TEST_METHOD_PROPERTIES_IN_CLASS_SCOPE()

/// <summary>
///  Macro for declaring metadata within inline test methods.
/// </summary>
///  Example:
/// \code
///      class TestFeatureClass
///      {
///          TEST_METHOD(TestFindNext)
///          {
///              BEGIN_TEST_METHOD_PROPERTIES()
///                  TEST_METHOD_PROPERTY(L"Priority", L"2")
///              END_TEST_METHOD_PROPERTIES()
///
///              // Your test code
///          }
///      }
/// \endcode
#define BEGIN_TEST_METHOD_PROPERTIES() \
    struct TAEF_TestMethodProperties \
    { \
        static const ::WEX::Private::TestPropertyMetadata* TAEF_GetTestMethodMetadata() \
        { \
            TAEF_TEST_METHOD_METADATA_START

/// <summary>
///  Macro that ends inline test method property declaration.
///  Must be used with BEGIN_TEST_METHOD_PROPERTIES()
/// </summary>
#define END_TEST_METHOD_PROPERTIES() \
            TAEF_TEST_METHOD_METADATA_END \
        } \
    }; \
    ::WEX::Private::DoNotDiscard(TAEF_TestMethodProperties::TAEF_GetTestMethodMetadata());

/// <summary>
///  Macro for adding a piece of metadata to a test method.
///  Must be used with BEGIN_TEST_METHOD() / END_TEST_METHOD() or within BEGIN_TEST_METHOD_PROPERTIES() / END_TEST_METHOD_PROPERTIES()
/// </summary>
#define TEST_METHOD_PROPERTY(propertyName, propertyValue) \
    { ::WEX::Private::MetadataIdentifierLookup<decltype(*propertyName), decltype(*propertyValue)>::c_methodIdentifier, propertyName, propertyValue },

/// <summary>
///  Macro for defining module properties
/// </summary>
///  Example:
/// \code
///      BEGIN_MODULE()
///          MODULE_PROPERTY(L"Area", L"Desktop Shell")
///          MODULE_PROPERTY(L"SubArea", L"Navigation")
///          MODULE_PROPERTY(L"Component", L"Start Menu")
///      END_MODULE()
/// \endcode
#define BEGIN_MODULE() \
    TAEF_STATIC_IF_CALLED_BY_TAEF_COMPILER_PIN TAEF_CPP_EXPORT const ::WEX::Private::TestPropertyMetadata* TAEF_GetModuleMetadata() \
    { \
        TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($f_MM) \
        static TAEF_CONST_TESTDATA ::WEX::Private::TestPropertyMetadata s_Metadata[] = { \
            { TAEF_MODULE_METADATA_IDENTIFIER, TAEF_WIDEN_IF_ABI_10("Name"), TAEF_WIDEN_IF_ABI_10("___##TestFile##___") },
// BEGIN_MODULE/END_MODULE is intended to be used outside of a class. However, some tests took
// advantage of the fact that it happened to work when placed within a class. To prevent build
// breaks for those tests, we now use static on the TAEF_GetModuleMetadata function when it is
// called within the TAEF_COMPILER_PIN macro. This avoids build breaks when BEGIN_MODULE/END_MODULE
// is used within a class. We don't use static when exporting though because that can cause build
// breaks for Clang since we would be exporting a function without external linkage.

/// <summary>
///  Macro for adding a piece of metadata (a property) to a test module propeties definition.
///  Must be used with BEGIN_MODULE() / END_MODULE()
/// </summary>
#define MODULE_PROPERTY(propertyName, propertyValue) \
    { ::WEX::Private::MetadataIdentifierLookup<decltype(*propertyName), decltype(*propertyValue)>::c_moduleIdentifier, propertyName, propertyValue },

/// <summary>
///  Macro that ends test module properties definition.
///  Must be used with BEGIN_MODULE()
/// </summary>
#define END_MODULE() \
            { TAEF_MODULE_METADATA_IDENTIFIER, nullptr, nullptr } \
        }; \
        return s_Metadata; \
    } \
    TAEF_COMPILER_PIN(TAEF_GetModuleMetadata)

/// <summary>
///  Macro for declaring a test class without associating it with any metadata
/// </summary>
///  Example:
/// \code
///      class TestFeatureClass
///      {
///          TEST_CLASS(TestFeatureClass)
///      }
/// \endcode
#define TEST_CLASS(className) \
    typedef className TAEF_TestClassType; \
    static const uintptr_t TAEF_TestMethodIndexOffset = __COUNTER__; \
    friend struct ::WEX::Private::TestClassFactory<className>; \
    typedef bool (className::*TAEF_MemberMaintFunc)(); \
    TAEF_MSVC_SUPPRESS_WARNING(25007) /* Disable warning that member method may be static */ \
    bool TAEF_DummyMaintFunc() { return true; } \
    /* Tests should not call this method. It is an internal TAEF implementation detail that is subject to change. */ \
    static TAEF_CPP_EXPORT const ::WEX::Private::TestClassInfo* TAEF_GetTestClassInfo() \
    { \
        /* this works similarly to WEX::Common::Conversion<T, U> checking if an expression (&_DummyMaintFunc) \
         * is convertible to the MemberMaintFunc type \
         */ \
        struct TaefClassNameTester { \
            /* these functions are only implemented because they're in an local class, so the compiler warns about missing definitions */ \
            static char TestType(TAEF_MemberMaintFunc) \
            { \
                return 'c'; \
            } \
            static TaefClassNameTester TestType(...) \
            { \
                return TaefClassNameTester(); \
            } \
            char member[2]; /* a two element array to ensure a different size from char returned by TestType(MemberMaintFunc) */ \
        }; \
        static_assert(sizeof(TaefClassNameTester::TestType(&TAEF_TestClassType::TAEF_DummyMaintFunc)) == 1, \
                TAEF_STRINGIZE(className) " is not the name of the current class"); \
 \
        TAEF_PUSH_IGNORE_WARNINGS \
        TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($b_TCI) \
        static TAEF_CONST_TESTDATA ::WEX::Private::TestClassInfo s_ClassInfo =  \
            {::WEX::Private::TaefAbiStructIdentifier::TestClass, TAEF_QUALIFIED_METHOD_NAME, \
             &::WEX::Private::TestClassFactory<className>::CreateInstance, &::WEX::Private::TestClassFactory<className>::DestroyInstance }; \
        TAEF_POP_IGNORE_WARNINGS \
        return &s_ClassInfo; \
    } \
    TAEF_COMPILER_PIN(TAEF_GetTestClassInfo)

/// <summary>
///  Macro for declaring a test class and associating it with metadata
/// </summary>
///  Example:
/// \code
///  class FeatureTestClass
///  {
///      BEGIN_TEST_CLASS(FeatureTestClass)
///          TEST_CLASS_PROPERTY(L"BVT", L"TRUE")
///          TEST_CLASS_PROPERTY(L"STRESS", L"TRUE")
///      END_TEST_CLASS()
///  }
/// \endcode
#define BEGIN_TEST_CLASS(className) \
    TEST_CLASS(className) \
 \
    static TAEF_CPP_EXPORT const ::WEX::Private::TestPropertyMetadata* TAEF_GetClassMetadata() \
    { \
        TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($g_TCM) \
        static TAEF_CONST_TESTDATA ::WEX::Private::TestPropertyMetadata s_Metadata[] =  { \
            { TAEF_TEST_CLASS_METADATA_IDENTIFIER, TAEF_WIDEN_IF_ABI_10("Name"), TAEF_QUALIFIED_METHOD_NAME}, \

/// <summary>
///  Macro for adding a piece of metadata to a test class declaration.
///  Must be used with BEGIN_TEST_CLASS() / END_TEST_CLASS()
/// </summary>
#define TEST_CLASS_PROPERTY(propertyName, propertyValue) \
    { ::WEX::Private::MetadataIdentifierLookup<decltype(*propertyName), decltype(*propertyValue)>::c_classIdentifier, propertyName, propertyValue },

/// <summary>
///  Macro that ends test class declaration.
///  Must be used with BEGIN_TEST_CLASS()
/// </summary>
#define END_TEST_CLASS() \
            { TAEF_TEST_CLASS_METADATA_IDENTIFIER, nullptr, nullptr} \
        }; \
        return s_Metadata; \
    } \
    TAEF_COMPILER_PIN(TAEF_GetClassMetadata)

#define TAEF_MODULE_FIXTURE_INFO_IMPL(methodName, moduleFixtureType) \
    struct TAEF_ModuleFixtureInvoker \
    { \
        static HRESULT TAEF_CDECL TAEF_Invoke() \
        { \
            return ::WEX::Private::SafeInvoke(methodName); \
        } \
    }; \
 \
    TAEF_PUSH_IGNORE_WARNINGS \
    TAEF_PIN_FUNCTION_SYMBOL TAEF_TESTDATA_SECTION($d_TGFI) \
    static TAEF_CONST_TESTDATA ::WEX::Private::TestGlobalFunctionInfo s_moduleFixtureInfo = \
        {moduleFixtureType, TAEF_WIDEN_IF_ABI_10(#methodName), &TAEF_ModuleFixtureInvoker::TAEF_Invoke }; \
    TAEF_POP_IGNORE_WARNINGS \
    return &s_moduleFixtureInfo;

#define TAEF_FIXTURE_IMPL(methodName) \
    TAEF_MSVC_SUPPRESS_WARNING(25007) /* Disable warning that member method may be static */ \
    bool methodName()

#if defined(INLINE_TEST_METHOD_MARKUP)
#define TAEF_DEFINE_FIXTURE(methodName) \
    TAEF_FIXTURE_IMPL(methodName)
#else
#define TAEF_DEFINE_FIXTURE(methodName)
#endif

#if defined(INLINE_TEST_METHOD_MARKUP)
#define TAEF_DECLARE_FIXTURE(methodName)
#else
#define TAEF_DECLARE_FIXTURE(methodName) \
    __if_not_exists(methodName) \
    { \
        TAEF_FIXTURE_IMPL(methodName); \
    }
#endif

#define TAEF_DECLARE_OR_DEFINE_FIXTURE(methodName) \
    TAEF_DECLARE_FIXTURE(methodName) \
    TAEF_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macro for defining global Setup function
/// </summary>
///  The Setup function runs before any test is executed
///  Example:
/// \code
///
///  MODULE_SETUP(AddRegistrySettings) //AddRegistrySettings function adds necessary reg settings for all tests
///
/// \endcode
#define MODULE_SETUP(methodName) \
    TAEF_FIXTURE_IMPL(methodName); \
    TAEF_CPP_EXPORT const ::WEX::Private::TestGlobalFunctionInfo* YOU_CAN_ONLY_DESIGNATE_ONE_FUNCTION_TO_BE_A_MODULE_SETUP_FUNCTION() \
    { \
        TAEF_MODULE_FIXTURE_INFO_IMPL(methodName, ::WEX::Private::TaefAbiStructIdentifier::ModuleSetup) \
    } \
    TAEF_COMPILER_PIN(YOU_CAN_ONLY_DESIGNATE_ONE_FUNCTION_TO_BE_A_MODULE_SETUP_FUNCTION) \
    TAEF_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macro for defining global Teardown function
/// </summary>
///  The Teardown function runs after all the test are executed
///  Example:
/// \code
///
///  MODULE_CLEANUP(CleanupRegistrySettings) //CleanupRegistrySettings restores the registry after all tests
///
/// \endcode
#define MODULE_CLEANUP(methodName) \
    TAEF_FIXTURE_IMPL(methodName); \
    TAEF_CPP_EXPORT const ::WEX::Private::TestGlobalFunctionInfo* YOU_CAN_ONLY_DESIGNATE_ONE_FUNCTION_TO_BE_A_MODULE_CLEANUP_FUNCTION() \
    { \
        TAEF_MODULE_FIXTURE_INFO_IMPL(methodName, ::WEX::Private::TaefAbiStructIdentifier::ModuleCleanup) \
    } \
    TAEF_COMPILER_PIN(YOU_CAN_ONLY_DESIGNATE_ONE_FUNCTION_TO_BE_A_MODULE_CLEANUP_FUNCTION) \
    TAEF_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macro for test setup method declaration
/// </summary>
///  Test setup method gets called before every test method is called
///  Example:
/// \code
///      class NotepadTestClass
///      {
///          // Declare CopyDownTestFiles class method and designate this method to be a test setup method
///          TEST_METHOD_SETUP(CopyDownTestFiles);
///      }
/// \endcode
#define TEST_METHOD_SETUP(methodName) \
    TAEF_REGISTER_FIXTURE_METHOD(methodName, ::WEX::Private::TaefAbiStructIdentifier::TestMethodSetup, \
        YOU_CAN_ONLY_DESIGNATE_ONE_CLASS_METHOD_TO_BE_A_TEST_METHOD_SETUP_METHOD) \
    TAEF_DECLARE_OR_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macros for test cleanup method declaration
/// </summary>
///  Test cleanup method gets called after every test method is called
///  Example:
/// \code
///      class NotepadTestClass
///      {
///          // Declare DeleteCopiedTestFiles class method and designate this method to be a test cleanup method
///          TEST_METHOD_CLEANUP(DeleteCopiedTestFiles);
///      }
/// \endcode
#define TEST_METHOD_CLEANUP(methodName) \
    TAEF_REGISTER_FIXTURE_METHOD(methodName, ::WEX::Private::TaefAbiStructIdentifier::TestMethodCleanup, \
        YOU_CAN_ONLY_DESIGNATE_ONE_CLASS_METHOD_TO_BE_A_TEST_METHOD_CLEANUP_METHOD) \
    TAEF_DECLARE_OR_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macro for test class setup method declaration
/// </summary>
///  Class setup method gets called before the first method in the class is called (after the class constructor)
///  Example:
/// \code
///      class NotepadTestClass
///      {
///          // Declare InstallNotepad class method and designate this method to be a class setup method
///          TEST_CLASS_CLEANUP(InstallNotepad);
///      }
/// \endcode
#define TEST_CLASS_SETUP(methodName) \
    TAEF_REGISTER_FIXTURE_METHOD(methodName, ::WEX::Private::TaefAbiStructIdentifier::TestClassSetup, \
        YOU_CAN_ONLY_DESIGNATE_ONE_CLASS_METHOD_TO_BE_A_TEST_CLASS_SETUP_METHOD) \
    TAEF_DECLARE_OR_DEFINE_FIXTURE(methodName)

/// <summary>
///  Macro for test class cleanup method declaration
/// </summary>
///  Class cleanup method gets called after the last method in the class is called (before the destructor)
///  Example:
/// \code
///      class NotepadTestClass
///      {
///          // Declare UninstallNotepad class method and designate this method to be a class cleanup method
///          TEST_CLASS_CLEANUP(UninstallNotepad);
///      }
/// \endcode
#define TEST_CLASS_CLEANUP(methodName) \
    TAEF_REGISTER_FIXTURE_METHOD(methodName, ::WEX::Private::TaefAbiStructIdentifier::TestClassCleanup, \
        YOU_CAN_ONLY_DESIGNATE_ONE_CLASS_METHOD_TO_BE_A_TEST_CLASS_CLEANUP_METHOD) \
    TAEF_DECLARE_OR_DEFINE_FIXTURE(methodName)

// Add the TestVersionInfo header to the testdata section.
namespace WEX { namespace Private
{
    // We need to ensure the TestVersionInfo is not discared by the linker.
    // For Windows binaries, we use a pragma to do this. For non-Windows binaries, we use the Wex.Logger's
    // DoNotDiscard API. The DoNotDiscard approach would work for Windows too, but the pragma has the advantage
    // that it avoids adding a Wex.Logger.dll dependency to existing binaries which include WexTestClass.h
    // without using a test macro. There are many existing cases of that on Windows; so using DoNotDiscard here
    // would force them to take the Wex.Logger dependency.
#if defined(_WIN32)
    // The TAEF_TESTDATA_SECTION macro will cause MSVC's linker to place the data in the "testdata" section.
    // The suffix after the dollar sign is used to order the data within the section. A suffix that starts with
    // "a" will be before any data with a suffix that starts with "b", "c", etc. Using "a_TDH" for the header
    // ensures the header appears before the rest of the data within the testdata section.
    extern "C" TAEF_TESTDATA_SECTION($a_TDH) __declspec(selectany) TestVersionInfo s_versionInfo = {
        TAEF_HEADER_SIGNATURE, TAEF_ABI_VERSION, "WexTestClass.h",
# if TAEF_ABI_VERSION >= 13
        // Internal detail: Generally we can determine the architecture from the PE file headers, but
        // ARM64X binaries can have both ARM64 and ARM64EC test data in the same DLL. Having the
        // architecture in TestVersionInfo enables TAEF to tell which data belongs to which architecture.
#  if defined(_M_IX86)
        TaefProcessorArchitecture::Taef_X86,
#  elif defined(_M_ARM64EC)
        TaefProcessorArchitecture::Taef_ARM64EC,
#  elif defined(_M_X64)
        TaefProcessorArchitecture::Taef_X86_64,
#  elif defined(_M_ARM64)
        TaefProcessorArchitecture::Taef_ARM64,
#  elif defined(_M_ARM)
        TaefProcessorArchitecture::Taef_ARM,
#  else
        TaefProcessorArchitecture::Taef_Unrecognized,
#  endif
# endif // TAEF_ABI_VERSION >= 13
# if TAEF_ABI_VERSION >= 12
        ::WEX::TestExecution::Private::StringStorage::TaefAbiStringDeallocator
# else
        StringManager::Deallocate
# endif
    };

# if defined(_M_IX86)
#  pragma comment(linker, "/include:_s_versionInfo")
# else
#  pragma comment(linker, "/include:s_versionInfo")
# endif
#else // ^^^ _WIN32 ^^^ // vvv !_WIN32 vvv
    inline const TestVersionInfo* TAEF_GetTestVersionInfo()
    {
        // On non-Windows platforms, we put the header in a separate section since we can't order data within the
        // header without using a linker script.
        __attribute__((section("testdataHeader")))
        static TAEF_CONST_TESTDATA TestVersionInfo s_versionInfo = {
            TAEF_HEADER_SIGNATURE, TAEF_ABI_VERSION, "WexTestClass.h",
# if TAEF_ABI_VERSION >= 13
#  if defined(__x86_64)
            TaefProcessorArchitecture::Taef_X86_64,
#  elif defined(__aarch64__)
            TaefProcessorArchitecture::Taef_ARM64,
#  elif defined(__i386)
            TaefProcessorArchitecture::Taef_X86,
#  else
            // TAEF doesn't currently support 32-bit ARM on Linux; so we leave it as Unrecognized for now so we don't
            // have to decide yet whether to treat Thumb mode differently.
            TaefProcessorArchitecture::Taef_Unrecognized,
#  endif
# endif // TAEF_ABI_VERSION >= 13
            ::WEX::TestExecution::Private::StringStorage::TaefAbiStringDeallocator
        };
        return &s_versionInfo;
    }

# if defined(__cpp_inline_variables) && __cpp_inline_variables
    inline
# else
    __attribute__((visibility("hidden")))
# endif
    const int g_doNotDiscardVersionInfo = DoNotDiscard(TAEF_GetTestVersionInfo());
#endif // _WIN32
}/* namespace Private */}/* namespace WEX */

#if !defined(_WIN32)
#pragma pop_macro("_In_")
#pragma pop_macro("_In_reads_z_")
#pragma pop_macro("_Outptr_")
#pragma pop_macro("_Outptr_result_maybenull_z_")
#pragma pop_macro("_Post_invalid_")
#pragma pop_macro("_Ret_maybenull_z_")
#endif // #if !defined(_WIN32)

#pragma pop_macro("Assert")
#pragma warning(pop)
