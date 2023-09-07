//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
// This file contains the PropertyCollection class.
//-----------------------------------------------------------------------------
#pragma once
// The test environment metadata retrieval APIs are not yet implemented on Linux.
#if defined(_WIN32)
#include "TE.Common.h"

#include "Log.h"
#include "WexString.h"
#include "WexTypes.h"

#if defined(__cpp_exceptions)
#include "WexException.h"
#endif // #if defined(__cpp_exceptions)

#include <cstdlib>
#include <winerror.h>

#if defined(TAEF_STL_SUPPORT)
#include <map>
#include <string>
#include <vector>
#endif

namespace WEX { namespace TestExecution
{
    // The code in the Private namespace should not be used directly. Use the wrapper classes that are located after this namespace.
    namespace Private
    {
        using TaefLookupToken = size_t;

        extern "C" TECOMMON_API
        HRESULT __stdcall PropertyCollection_TryGetProperty(TaefLookupToken primaryLookupToken, TaefLookupToken secondaryLookupToken,
            _In_z_ const wchar_t* pszKey, _Outptr_result_nullonfailure_ TAEF_LPCWSTR* ppszValue) noexcept;

        struct PropertyKeyValuePair
        {
            TAEF_LPCWSTR m_pszKey;
            TAEF_LPCWSTR m_pszValue;
        };

        extern "C" TECOMMON_API
        HRESULT __stdcall PropertyCollection_GetAllProperties(TaefLookupToken primaryLookupToken, TaefLookupToken secondaryLookupToken,
            _Out_writes_to_opt_(elementCount, *pPropertyCount) PropertyKeyValuePair* pProperties,
            size_t elementCount, _Out_ size_t* pPropertyCount) noexcept;
    }/* namespace Private */

    // A PropertyCollection instance represents a mapping of strings to strings. This is currently only used with the TestEnvironment
    // APIs in TaefTestEnvironment.h.
    class PropertyCollection
    {
        friend class TestEnvironment;
        friend class TestRole;
        static constexpr const wchar_t* c_pszPropertyConversionWarningFormat =
            L"PropertyCollection::TryGetProperty retrieved a value of \"%ws\" for the \"%ws\" property, "
            L"but it failed to convert the value to %ws.";

    public:
        // If the property exists, this method returns a null-terminated wchar_t pointer pointing to the value of the property;
        // otherwise, this method returns a null pointer. The pointer does not need to be freed.
        _Ret_maybenull_z_ const wchar_t* TryGetProperty(_In_z_ const wchar_t* pszKey) const noexcept
        {
            const wchar_t* pszValue = nullptr;
            const HRESULT hr = Private::PropertyCollection_TryGetProperty(m_primaryLookupToken, m_secondaryLookupToken,
                pszKey, &pszValue);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            {
                // If PropertyCollection_TryGetProperty fails with an error code other than HRESULT_FROM_WIN32(ERROR_NOT_FOUND),
                // that indicates that m_primaryLookupToken and/or m_secondaryLookupToken are invalid. However, the wrapper classes
                // ensure they are valid; so this will only fail if there is data corruption. For this reason, we do a fast-fail to
                // crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }
            // pszValue will still be null if the property was not found.
            return pszValue;
        }

#if defined(TAEF_STL_SUPPORT)
        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or E_OUTOFMEMORY if a memory
        // allocation failure occurs when assigning to the output parameter. In either failure case, the initial value of the
        // output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ std::wstring& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                try
                {
                    value = pszValue;
                    return S_OK;
                }
                catch (const std::bad_alloc&)
                {
                    return E_OUTOFMEMORY;
                }
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // Returns the specified property value. This throws an exception if the property does not exist.
        std::wstring GetProperty(_In_z_ const wchar_t* pszKey) const
        {
            const wchar_t* pszValue = TryGetProperty(pszKey);
            if (!pszValue)
            {
                WEX::Common::Throw::Exception(HRESULT_FROM_WIN32(ERROR_NOT_FOUND),
                    WEX::Common::String{}.Format(
                        L"PropertyCollection::GetProperty failed to get property \"%ws\" because it does not exist. "
                        L"Use the TryGetProperty method if you need to handle cases where the property might not exist.",
                        pszKey));
            }
            return std::wstring{pszValue};
        }
#endif // #if defined(TAEF_STL_SUPPORT)

#if defined(__cpp_exceptions)
        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or a separate error code
        // if a failure occurs when assigning to the output parameter. In either failure case, the initial value of the
        // output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ WEX::Common::String& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                try
                {
                    value = pszValue;
                    return S_OK;
                }
                catch (const WEX::Common::Exception& ex)
                {
                    return ex.ErrorCode();
                }
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
#endif // #if defined(__cpp_exceptions)

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or a separate error code
        // if a failure occurs when assigning to the output parameter. In either failure case, the initial value of the
        // output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ WEX::Common::NoThrowString& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                value = pszValue;
                // This will be S_OK if the string was successfully placed in the output parameter.
                return value.GetLastHResult();
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as a float.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ float& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"a float", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as a double.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ double& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"a double", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as an int.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ int& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"an int", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as an unsigned int.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ unsigned int& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"an unsigned int", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as a long.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ long& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"a long", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as an unsigned long.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ unsigned long& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"an unsigned long", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as a long long.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ long long& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"a long long", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as an unsigned long long.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ unsigned long long& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                // This returns HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be converted to the correct type.
                return ParsePropertyValueUsingCrt(pszKey, pszValue, L"an unsigned long long", value);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // This method returns HRESULT_FROM_WIN32(ERROR_NOT_FOUND) if the value does not exist or
        // HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH) if the value cannot be parsed as a bool.
        // In either failure case, the initial value of the output parameter is unchanged.
        HRESULT TryGetProperty(_In_z_ const wchar_t* pszKey, _Out_ bool& value) const noexcept
        {
            if (const wchar_t* pszValue = TryGetProperty(pszKey))
            {
                if (::_wcsicmp(L"true", pszValue) == 0 || ::wcscmp(L"1", pszValue) == 0)
                {
                    value = true;
                    return S_OK;
                }

                if (::_wcsicmp(L"false", pszValue) == 0 || ::wcscmp(L"0", pszValue) == 0)
                {
                    value = false;
                    return S_OK;
                }

                WEX::Logging::Log::Warning(
                    WEX::Common::NoThrowString{}.Format(c_pszPropertyConversionWarningFormat, pszValue, pszKey, L"a bool"));
                return HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH);
            }
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        // Returns the number of properties in the collection.
        size_t GetPropertyCount() const noexcept
        {
            size_t propertyCount;
            const HRESULT hr = GetAllProperties(nullptr, 0, &propertyCount);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            {
                // If PropertyCollection_GetAllProperties fails with an error code other than
                // HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER), that indicates that one of the parameters are invalid. However, the
                // wrapper classes ensure they are valid; so this will only fail if there is data corruption. For this reason, we do a
                // fast-fail to crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }
            return propertyCount;
        }

#if defined(TAEF_STL_SUPPORT)
        // Converts the PropertyCollection to a std::map containing the same data.
        std::map<std::wstring, std::wstring> GetAllProperties() const
        {
            std::vector<PropertyKeyValuePair> propertiesVector;
            propertiesVector.resize(GetPropertyCount());

            size_t propertyCount;
            const HRESULT hr = GetAllProperties(propertiesVector.data(), propertiesVector.size(), &propertyCount);
            if (FAILED(hr) || propertyCount != propertiesVector.size())
            {
                // PropertyCollection_GetAllProperties should never fail when used with correct parameters and with enough entries in
                // the array. The wrapper classes ensure these failure conditions will not be hit. If this export does fail, it
                // indicates there is data corruption. For this reason, we do a fast-fail to crash the process instead of throwing an
                // exception.
                TAEF_FAST_FAIL;
            }

            std::map<std::wstring, std::wstring> propertiesMap;
            for (const PropertyKeyValuePair& propertyKeyValuePair : propertiesVector)
            {
                // The data from PropertyCollection_GetAllProperties has sorted keys; so we use a hint to tell std::map that the
                // new value will go at the end of the map.
                propertiesMap.emplace_hint(propertiesMap.end(), propertyKeyValuePair.m_pszKey, propertyKeyValuePair.m_pszValue);
            }
            return propertiesMap;
        }
#endif // #if defined(TAEF_STL_SUPPORT)

        using PropertyKeyValuePair = Private::PropertyKeyValuePair;

        // When using the STL, the other GetAllProperties overload provides a simpler way to get all properties. However, we expose
        // this lower-level overload for users not using the STL. The other GetAllProperties overload is implemented by calling this
        // overload; so see the implementation of the other overload for an explanation of how to use this.
        HRESULT GetAllProperties(_Out_writes_to_opt_(elementCount, *pPropertyCount) PropertyKeyValuePair* pProperties,
            size_t elementCount, _Out_ size_t* pPropertyCount) const noexcept
        {
            return Private::PropertyCollection_GetAllProperties(m_primaryLookupToken, m_secondaryLookupToken, pProperties, elementCount,
                pPropertyCount);
        }

    private:
        explicit PropertyCollection(Private::TaefLookupToken primaryLookupToken,
                Private::TaefLookupToken secondaryLookupToken = 0) noexcept
            : m_primaryLookupToken{primaryLookupToken}
            , m_secondaryLookupToken{secondaryLookupToken}
        {}

        static int IdentifyRadix(_In_z_ const wchar_t* pszValue) noexcept
        {
            return ::_wcsnicmp(pszValue, L"0x", 2 /* length of "0x" */) == 0 ? 0x10 : 10;
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ float& value) noexcept
        {
            value = ::wcstof(pszValue, ppszEnd);
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ double& value) noexcept
        {
            value = ::wcstod(pszValue, ppszEnd);
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ int& value) noexcept
        {
            value = static_cast<int>(::wcstol(pszValue, ppszEnd, IdentifyRadix(pszValue)));
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ unsigned int& value) noexcept
        {
            value = static_cast<unsigned int>(::wcstoul(pszValue, ppszEnd, IdentifyRadix(pszValue)));
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ long& value) noexcept
        {
            value = ::wcstol(pszValue, ppszEnd, IdentifyRadix(pszValue));
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ unsigned long& value) noexcept
        {
            value = ::wcstoul(pszValue, ppszEnd, IdentifyRadix(pszValue));
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ long long& value) noexcept
        {
            value = ::wcstoll(pszValue, ppszEnd, IdentifyRadix(pszValue));
        }

        static void CallAppropriateCrtParseFunction(_In_z_ const wchar_t* pszValue, _Out_opt_ _Deref_post_z_ wchar_t** ppszEnd,
            _Out_ unsigned long long& value) noexcept
        {
            value = ::wcstoull(pszValue, ppszEnd, IdentifyRadix(pszValue));
        }

        template <typename T>
        static HRESULT ParsePropertyValueUsingCrt(_In_z_ const wchar_t* pszKey, _In_z_ const wchar_t* pszValue,
            _In_z_ const wchar_t* pszTypeName, T& value) noexcept
        {
            errno = 0;
            wchar_t* pszEnd = nullptr;
            T parsedValue;
            CallAppropriateCrtParseFunction(pszValue, &pszEnd, parsedValue);
            if (*pszEnd != L'\0' || errno == ERANGE)
            {
                // The property exists, but either the whole string was not consumed by the CRT function or the value was out of range.
                WEX::Logging::Log::Warning(
                    WEX::Common::NoThrowString{}.Format(c_pszPropertyConversionWarningFormat, pszValue, pszKey, pszTypeName));
                return HRESULT_FROM_WIN32(ERROR_DATATYPE_MISMATCH);
            }
            value = parsedValue;
            return S_OK;
        }

        Private::TaefLookupToken m_primaryLookupToken;
        Private::TaefLookupToken m_secondaryLookupToken;
    };
}/* namespace TestExecution */}/* namespace WEX */
#endif // #if defined(_WIN32)
