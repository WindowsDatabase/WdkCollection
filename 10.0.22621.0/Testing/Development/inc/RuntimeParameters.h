//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary> API for accessing runtime parameters from native tests </summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
// The runtime parameter retrieval APIs are not yet implemented on Linux.
#if defined(_WIN32)
#include "TE.Common.h"

#include "WexTypes.h"

#if defined(_WIN32)
#include <BaseTyps.h>
#endif // #if defined(_WIN32)

#if !defined(_WIN32)
#pragma push_macro("_In_z_")
#if !defined(_In_z_)
#define _In_z_
#endif
#endif // #if !defined(_WIN32)

namespace WEX { namespace TestExecution
{
    namespace Private
    {
        /// \internal
        /// WEX::TestExecution::Private::RuntimeParameters must be a templatized *class* so that we can dll export explicit 
        /// instantiations.
        template <typename T>
        class TECOMMON_API RuntimeParameters
        {
        public:
            // Tries to get the requested value in the requested format
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, T& result);

            #if defined(TECOMMON_EXPORTS)
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, T& result);
            #endif

        private:
            RuntimeParameters(const RuntimeParameters&) = delete;
            RuntimeParameters& operator=(const RuntimeParameters&) = delete;
        };
    }

    namespace RuntimeParameterConstants
    {
        // Constant used to query RuntimeParameters for the directory that the test binary is loaded from.
        const wchar_t c_szTestDeploymentDir[] = L"TestDeploymentDir";

        // Constant used to query RuntimeParameters for the name of the test that is currently running.
        const wchar_t c_szTestName[] = L"TestName";

        // Constant used to query RuntimeParameters for the name of the test variation that is currently running.
        const wchar_t c_szFullTestName[] = L"FullTestName";

        // Constant used to query RuntimeParameters for the result of the test(s) run within the scope fo this cleanup function.
        const wchar_t c_szTestResult[] = L"TestResult";
    }

    /// <summary>
    ///  Runtime parameters retrival class
    /// </summary>
    /// \internal
    /// WEX::TestExecution::RuntimeParameters provides 'templatized' *methods* to allow compiler type inference, and simply routes 
    /// the call to the matching WEX::TestExecution::Private::TestData<T> instantiation.
    class RuntimeParameters
    {
    public:
        /// <summary>
        ///  API for runtime parameters retrival
        /// </summary>
        /// \code
        /// Example:
        ///    String value;
        ///    RuntimeParameters::TryGetValue(L"ParameterName3", value);
        /// \endcode
        template <typename T>
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, T& result)
        {
            return Private::RuntimeParameters<T>::TryGetValue(pszString, result);
        }

    private:
        RuntimeParameters(const RuntimeParameters&) = delete;
        RuntimeParameters& operator=(const RuntimeParameters&) = delete;
    };
}/* namespace TestExecution */}/* namespace WEX */

#if !defined(_WIN32)
#pragma pop_macro("_In_z_")
#endif // #if !defined(_WIN32)

#endif // #if defined(_WIN32)
