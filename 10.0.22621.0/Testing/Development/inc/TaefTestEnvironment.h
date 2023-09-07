//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//
// This file contains the TestEnvironment, TestRoleGroup, TestRole, and NetworkConnectionInfo classes.
// The TestEnvironment class contains only static methods and is the entry point into the API.
// These classes expose information passed down from the outer test infrastructure that launched TAEF.
//-----------------------------------------------------------------------------
#pragma once
// The test environment metadata retrieval APIs are not yet implemented on Linux.
#if defined(_WIN32)
#include "TE.Common.h"

#include "Log.h"
#include "TaefPropertyCollection.h"
#include "WexString.h"
#include "WexTypes.h"

#if defined(__cpp_exceptions)
#include "WexException.h"
#endif // #if defined(__cpp_exceptions)

#include <winerror.h>

#if !defined(TAEF_NO_CONDUIT)
#include <Conduit/BrokerManagerClient.h>
#include <Conduit/ComputeSystem.h>
#endif // #if !defined(TAEF_NO_CONDUIT)

#if !defined(TAEF_NO_WINDOWS_H)
#include <Windows.h> // Needed for IPExport.h
#include <IPExport.h> // Needed for in_addr and in6_addr
#endif // #if !defined(TAEF_NO_WINDOWS_H)

#if defined(TAEF_STL_SUPPORT)
#include <string>
#include <vector>
#endif // #if defined(TAEF_STL_SUPPORT)

namespace WEX { namespace TestExecution
{
    // The code in the Private namespace should not be used directly. Use the wrapper classes that are located after this namespace.
    namespace Private
    {
        extern "C" TECOMMON_API
        bool __stdcall TestEnvironment_IsAvailable() noexcept;

        extern "C" TECOMMON_API
        TaefLookupToken __stdcall TestEnvironment_GetPropertyLookupToken() noexcept;

        enum class TestRoleGroupFlags : unsigned int
        {
            None = 0x0,
            ContainsCurrentProcess = 0x1,
            ContainsDefaultExecutionRole = 0x2,
        };

        struct TestRoleGroupData
        {
            bool IsFlagSet(TestRoleGroupFlags flag) const noexcept
            {
                return (static_cast<unsigned int>(m_flags) & static_cast<unsigned int>(flag)) != 0;
            }

            // m_lookupToken is for looking up additional data that isn't included in this struct.
            TaefLookupToken m_lookupToken;
            size_t m_testRoleCount;
            TestRoleGroupFlags m_flags;
        };

        extern "C" TECOMMON_API
        HRESULT __stdcall TestEnvironment_TryGetTestRoleGroup(
            _In_z_ const wchar_t* pszTestRoleGroupName,
            _Out_ TestRoleGroupData* pTestRoleGroupData) noexcept;

        extern "C" TECOMMON_API
        HRESULT __stdcall TestEnvironment_GetTestRoleGroupNames(
            _Out_writes_to_opt_(elementCount, *pTestRoleGroupCount) TAEF_LPCWSTR* ppszTestRoleGroupNames,
            size_t elementCount, _Always_(_Out_) size_t* pTestRoleGroupCount) noexcept;

        enum class TestRoleCapabilities : unsigned long long
        {
            None = 0x0,
            NetworkConnectionInfo = 0x1,
            ConduitConnector = 0x2,
        };

        struct TestRoleData
        {
            bool IsCapabilityAvailable(TestRoleCapabilities capability) const noexcept
            {
                return (static_cast<unsigned long long>(m_availableCapabilities) & static_cast<unsigned long long>(capability)) != 0;
            }

            TestRoleCapabilities m_availableCapabilities;
            TaefLookupToken m_lookupToken;
        };

        extern "C" TECOMMON_API
        HRESULT __stdcall TestRoleGroup_GetTestRole(TaefLookupToken roleGroupToken, size_t index,
            _Out_ TestRoleData* pTestRoleData) noexcept;

        extern "C" TECOMMON_API
        HRESULT __stdcall NetworkConnectionInfo_GetHostName(TaefLookupToken roleGroupToken, TaefLookupToken roleToken,
            _Outptr_result_z_ const wchar_t** ppszHostName) noexcept;

#if !defined(TAEF_NO_WINDOWS_H)
        extern "C" TECOMMON_API
        HRESULT __stdcall NetworkConnectionInfo_TryGetIPv4Address(TaefLookupToken roleGroupToken, TaefLookupToken roleToken,
            _Out_ in_addr* pIPv4Address) noexcept;

        extern "C" TECOMMON_API
        HRESULT __stdcall NetworkConnectionInfo_TryGetIPv6Address(TaefLookupToken roleGroupToken, TaefLookupToken roleToken,
            _Out_ in6_addr* pIPv6Address) noexcept;
#endif // #if !defined(TAEF_NO_WINDOWS_H)

#define TAEF_MAX_IPADDR_STRING_LEN 46

        extern "C" TECOMMON_API
        HRESULT __stdcall NetworkConnectionInfo_GetIPAddressAsString(TaefLookupToken roleGroupToken, TaefLookupToken roleToken,
            _Out_writes_z_(TAEF_MAX_IPADDR_STRING_LEN) wchar_t* pszIPAddress) noexcept;

#if !defined(TAEF_NO_CONDUIT)
        extern "C" TECOMMON_API
        HRESULT __stdcall ConduitConnector_GetBrokerManagerClient(TaefLookupToken roleGroupToken, TaefLookupToken roleToken,
            _Outptr_ Conduit::Broker::BrokerManagerClientNoThrow::HandleType* pBrokerManagerClientHandle,
            Conduit::Common::ErrorMessage* pErrorMessage) noexcept;
#endif // #if !defined(TAEF_NO_CONDUIT)

        // The DLL exports declared in this file are designed to avoid failures when possible. They do not do any memory
        // allocations or IO. They only fail in the following ways:
        // 1. Some exports can fail due to invalid parameters being passed to them.
        // 2. Lookup functions can fail with HERSULT_FROM_WIN32(ERROR_NOT_FOUND).
        // 3. Functions that fill an array can fail with HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) if the array is too small.
        //
        // Users should not use the exports directly. Instead wrapper classes are provided to abstract away the details with a
        // higher-level interface. The wrapper classes ensure that invalid parameters are not passed to the exports so in cases
        // where the exports fail due to invalid parameters, the wrapper class will intentionaly crash the process due to the
        // expectation that data corruption has occurred.
        //
        // In some cases, wrapper classes may want to raise errors to the caller as an exception. This can happen for cases such
        // as for looking up a value that did not exist or providing an index that was too large. For methods that use the STL,
        // they will throw an exception using WEX::Common::Throw in these cases. However, methods that do not use the STL can be
        // used by tests that have exceptions disabled. Those methods use this HandleTestEnvironmentFailure function to handle
        // cases where they want to throw an exception. If exceptions are enabled, HandleTestEnvironmentFailure throws an
        // exception; but if exceptions are disabled, HandleTestEnvironmentFailure logs an error and crashes the process. This
        // is fine since all cases where an exception would be thrown are avoidable if the code is more careful to check whether
        // a value or index exists before it does the lookup.
        template <typename... TArgs>
        [[noreturn]]
        void HandleTestEnvironmentFailure(_In_range_(<, 0) HRESULT hr,
            _In_z_ _Printf_format_string_ const wchar_t* pszFormatString, TArgs... args)
        {
            WEX::Common::NoThrowString formattedErrorMessage;
#if !defined(__cpp_exceptions)
            formattedErrorMessage.Format(L"[HRESULT 0x%08X] ", hr);
#endif

            formattedErrorMessage.AppendFormat(pszFormatString, args...);

            const wchar_t* const pszErrorMessage = formattedErrorMessage.ToCStrWithFallbackTo(pszFormatString);
#if defined(__cpp_exceptions)
            WEX::Common::Throw::Exception(hr, pszErrorMessage);
#else
            // For code that builds with exceptions disabled, we log an error and crash the process since we can't throw an exception.
            WEX::Logging::Log::Error(pszErrorMessage);
            TAEF_FAST_FAIL;
#endif
        }
    }/* namespace Private */

    // Provides access to the host name and IP address for a test machine. Call TestRole::GetConnectionInfo<NetworkConnectionInfo> to
    // get an instance of this class.
    class NetworkConnectionInfo
    {
        friend class TestRole;
    public:
        // Gets the host name that can be used to reach the test device over the network.
        _Ret_z_ const wchar_t* GetHostName() const noexcept
        {
            const wchar_t* pszHostName;
            const HRESULT hr = Private::NetworkConnectionInfo_GetHostName(m_roleGroupToken, m_roleToken, &pszHostName);
            if (FAILED(hr))
            {
                // TestRole_GetHostName just retrieves a stored value; so it only fails when m_roleGroupToken and/or
                // m_roleToken are invalid or if this role does not have the NetworkConnectionInfo capability.
                // However, the wrapper classes avoid these cases; so failures will only occurs if there is data corruption.
                // For this reason, we do a fast-fail to crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }

            return pszHostName;
        }

#if !defined(TAEF_NO_WINDOWS_H)
        // If this test device has an associated IPv4 address, this returns true and sets the output parameter to the IPv4 address.
        // Otherwise, this returns false and clears the output parameter. At least one of TryGetIPv4Address and TryGetIPv6Address
        // will return true.
        bool TryGetIPv4Address(_Out_ in_addr& ipv4Address) const noexcept
        {
            const HRESULT hr = Private::NetworkConnectionInfo_TryGetIPv4Address(m_roleGroupToken, m_roleToken, &ipv4Address);
            if (FAILED(hr))
            {
                // TestRole_TryGetIPv4Address just retrieves a stored value; so it only fails when m_roleGroupToken and/or
                // m_roleToken are invalid or if this role does not have the NetworkConnectionInfo capability.
                // However, the wrapper classes avoid these cases; so failures will only occurs if there is data corruption.
                // For this reason, we do a fast-fail to crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }

            // If the IPv4 address does not exist, hr is S_FALSE; so we will return false here.
            return hr == S_OK;
        }

        // If this test device has an associated IPv6 address, this returns true and sets the output parameter to the IPv6 address.
        // Otherwise, this returns false and clears the output parameter. At least one of TryGetIPv4Address and TryGetIPv6Address
        // will return true.
        bool TryGetIPv6Address(_Out_ in6_addr& ipv6Address) const noexcept
        {
            const HRESULT hr = Private::NetworkConnectionInfo_TryGetIPv6Address(m_roleGroupToken, m_roleToken, &ipv6Address);
            if (FAILED(hr))
            {
                // TestRole_TryGetIPv6Address just retrieves a stored value; so it only fails when m_roleGroupToken and/or
                // m_roleToken are invalid or if this role does not have the NetworkConnectionInfo capability.
                // However, the wrapper classes avoid these cases; so failures will only occurs if there is data corruption.
                // For this reason, we do a fast-fail to crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }

            // If the IPv6 address does not exist, hr is S_FALSE; so we will return false here.
            return hr == S_OK;
        }
#endif // #if !defined(TAEF_NO_WINDOWS_H)

#if defined(TAEF_STL_SUPPORT)
        // Gets the IP address formatted as a string. This prefers the IPv6 address, but falls back to IPv4 if there is no IPv6
        // address.
        std::wstring GetIPAddressAsString() const
        {
            wchar_t szIPAddress[TAEF_MAX_IPADDR_STRING_LEN];

            const HRESULT hr = Private::NetworkConnectionInfo_GetIPAddressAsString(m_roleGroupToken, m_roleToken, szIPAddress);
            if (FAILED(hr))
            {
                // TestRole_GetIPAddressAsString only fails when m_roleGroupToken and/or m_roleToken are invalid or if
                // this role does not have the NetworkConnectionInfo capability. However, the wrapper classes avoid these
                // cases; so failures will only occurs if there is data corruption. For this reason, we do a fast-fail to
                // crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }

            return std::wstring{szIPAddress};
        }
#endif

    private:
        NetworkConnectionInfo(Private::TaefLookupToken roleGroupToken, Private::TaefLookupToken roleToken) noexcept
            : m_roleGroupToken{roleGroupToken}
            , m_roleToken{roleToken}
        {}

        Private::TaefLookupToken m_roleGroupToken;
        Private::TaefLookupToken m_roleToken;
    };

#if !defined(TAEF_NO_CONDUIT)
    // This class connects to the test device using Conduit. You can use the Conduit classes to interact with the remote
    // machine. Call TestRole::GetConnector<ConduitConnector> to get an instance of this class.
    class ConduitConnector
    {
        friend class TestRole;
    public:
#if defined(__cpp_exceptions)
        // Gets a BrokerManagerClient instance connected to the BrokerManager hosted in the Conduit service on the target machine.
        // Throws std::system_error on failure.
        Conduit::Broker::BrokerManagerClient GetBrokerManagerClient() const
        {
            Conduit::Common::ErrorMessage errorMessage;
            Conduit::Broker::BrokerManagerClient result{nullptr};
            const HRESULT hr = Private::ConduitConnector_GetBrokerManagerClient(m_roleGroupToken, m_roleToken,
                result.GetHandleWrapper().GetHandleAddress(), &errorMessage);
            Conduit::Common::ThrowOnFailure(hr, errorMessage);
            return result;
        }

        // Launches a Conduit.Environment broker on the target machine and connects to it. The returned ComputeSystem instance
        // enables interaction with the target machine. Throws std::system_error on failure.
        Conduit::Environment::ComputeSystem GetComputeSystem() const
        {
            return Conduit::Environment::ComputeSystem{GetBrokerManagerClient()};
        }
#endif

        // Gets a BrokerManagerClient instance connected to the BrokerManager hosted in the Conduit service on the target machine.
        // This method does not throw. Use GetBrokerManagerClient for an equivalent method that throws on failure.
        HRESULT GetBrokerManagerClientNoThrow(_Out_ Conduit::Broker::BrokerManagerClientNoThrow& brokerManagerClient,
            Conduit::Common::ErrorMessage* pErrorMessage = nullptr) const noexcept
        {
            return Private::ConduitConnector_GetBrokerManagerClient(m_roleGroupToken, m_roleToken,
                brokerManagerClient.GetHandleWrapper().ReleaseAndGetHandleAddress(), pErrorMessage);
        }

        // Launches a Conduit.Environment broker on the target machine and connects to it. The returned ComputeSystem instance
        // enables interaction with the target machine. This method does not throw. Use GetComputeSystem for an equivalent method
        // that throws on failure.
        HRESULT GetComputeSystemNoThrow(_Out_ Conduit::Environment::ComputeSystemNoThrow& computeSystem,
            Conduit::Common::ErrorMessage* pErrorMessage = nullptr) const noexcept
        {
            Conduit::Broker::BrokerManagerClientNoThrow brokerManagerClient;
            const HRESULT hr = GetBrokerManagerClientNoThrow(brokerManagerClient, pErrorMessage);
            if (FAILED(hr))
            {
                return hr;
            }

            return computeSystem.Create(brokerManagerClient, pErrorMessage);
        }

    private:
        ConduitConnector(Private::TaefLookupToken roleGroupToken, Private::TaefLookupToken roleToken) noexcept
            : m_roleGroupToken{roleGroupToken}
            , m_roleToken{roleToken}
        {}

        Private::TaefLookupToken m_roleGroupToken;
        Private::TaefLookupToken m_roleToken;
    };
#endif // #if !defined(TAEF_NO_CONDUIT)

    // Represents an entity in the test environment that the test can interact with. For example, this might be a test machine
    // that is part of a multi-machine test.
    class TestRole
    {
        friend class TestRoleGroup;
    public:
        // Gets the properties specific to this test role. These properties come from the outer test infrastructure that launched
        // TAEF. TAEF doesn't guarantee that any particular keys are present. Consult the documentation for the outer test
        // infrastructure that launched TAEF to see if it exposes any properties to the test through this mechanism.
        PropertyCollection GetProperties() const noexcept
        {
            return PropertyCollection{m_roleGroupToken, m_testRoleData.m_lookupToken};
        }

        // Gets a class which provides connection info for the test role. Different connection info classes may be available depending
        // on the kind of role this TestRole represents. If this role does not support the provided connection info class, this method
        // will throw an exception (or log an error and crash the process if exceptions are disabled). You can optionally use
        // IsConnectionInfoAvailable to query whether a given type is supported by the role.
        // 
        // There is currently only one class that can be used here: NetworkConnectionInfo.
        //
        // Example:
        //   NetworkConnectionInfo networkConnectionInfo = testRole.GetConnectionInfo<NetworkConnectionInfo>();
        template <typename TConnectionInfo>
        TConnectionInfo GetConnectionInfo() const
        {
            static_assert(CapabilityLookup<TConnectionInfo>::c_isValidConnectionInfoType,
                "The type passed as the template parameter to TestRole::GetConnectionInfo was not a valid type for this method.");

            if (!IsConnectionInfoAvailable<TConnectionInfo>())
            {
                Private::HandleTestEnvironmentFailure(HRESULT_FROM_WIN32(ERROR_NOT_CAPABLE),
                    L"TestRole::GetConnectionInfo<%ws> was called on a role that does not have an available implementation for %ws.",
                    CapabilityLookup<TConnectionInfo>::c_pszCapabilityName, CapabilityLookup<TConnectionInfo>::c_pszCapabilityName);
            }

            return TConnectionInfo{m_roleGroupToken, m_testRoleData.m_lookupToken};
        }

        // Returns whether the role supports the provided connection info interface.
        template <typename TConnectionInfo>
        bool IsConnectionInfoAvailable() const noexcept
        {
            static_assert(CapabilityLookup<TConnectionInfo>::c_isValidConnectionInfoType,
                "The type passed as the template parameter to TestRole::IsConnectionInfoAvailable was not a valid type for this method.");

            return m_testRoleData.IsCapabilityAvailable(CapabilityLookup<TConnectionInfo>::c_capabilityFlag);
        }

        // Gets a class to use for connecting to the test role. Different connector classes may be available depending on the kind of
        // role this TestRole represents. If this role does not support the provided connector class, this method will throw an exception
        // (or log an error and crash the process if exceptions are disabled). You can optionally use IsConnectorAvailable to query
        // whether a given type is supported by the role.
        // 
        // There is currently only one class that can be used here: ConduitConnector.
        //
        // Example:
        //   ConduitConnector conduitConnector = testRole.GetConnector<ConduitConnector>();
        template <typename TConnector>
        TConnector GetConnector() const
        {
            static_assert(CapabilityLookup<TConnector>::c_isValidConnectorType,
                "The type passed as the template parameter to TestRole::GetConnector was not a valid type for this method.");

            if (!IsConnectorAvailable<TConnector>())
            {
                Private::HandleTestEnvironmentFailure(HRESULT_FROM_WIN32(ERROR_NOT_CAPABLE),
                    L"TestRole::GetConnector<%ws> was called on a role that does not have an available implementation for %ws.",
                    CapabilityLookup<TConnector>::c_pszCapabilityName, CapabilityLookup<TConnector>::c_pszCapabilityName);
            }

            return TConnector{m_roleGroupToken, m_testRoleData.m_lookupToken};
        }

        // Returns whether the role supports the provided connector class.
        template <typename TConnector>
        bool IsConnectorAvailable() const noexcept
        {
            static_assert(CapabilityLookup<TConnector>::c_isValidConnectorType,
                "The type passed as the template parameter to TestRole::IsConnectorAvailable was not a valid type for this method.");

            return m_testRoleData.IsCapabilityAvailable(CapabilityLookup<TConnector>::c_capabilityFlag);
        }

        // Gets a class to use for controlling some aspect of the test role. Different controller classes may be available depending on
        // the kind of role this TestRole represents. If this role does not support the provided controller class, this method will throw
        // an exception (or log an error and crash the process if exceptions are disabled). You can optionally use IsControllerAvailable
        // to query whether a given type is supported by the role.
        // 
        // Currently, there are no controller classes that can be used here.
        template <typename TController>
        TController GetController() const
        {
            static_assert(CapabilityLookup<TController>::c_isValidControllerType,
                "The type passed as the template parameter to TestRole::GetController was not a valid type for this method.");

            if (!IsControllerAvailable<TController>())
            {
                Private::HandleTestEnvironmentFailure(HRESULT_FROM_WIN32(ERROR_NOT_CAPABLE),
                    L"TestRole::GetController<%ws> was called on a role that does not have an available implementation for %ws.",
                    CapabilityLookup<TController>::c_pszCapabilityName, CapabilityLookup<TController>::c_pszCapabilityName);
            }

            return TController{m_roleGroupToken, m_testRoleData.m_lookupToken};
        }

        // Returns whether the role supports the provided controller class.
        template <typename TController>
        bool IsControllerAvailable() const noexcept
        {
            static_assert(CapabilityLookup<TController>::c_isValidControllerType,
                "The type passed as the template parameter to TestRole::IsControllerAvailable was not a valid type for this method.");

            return m_testRoleData.IsCapabilityAvailable(CapabilityLookup<TController>::c_capabilityFlag);
        }

    private:
        TestRole(Private::TaefLookupToken roleGroupToken, const Private::TestRoleData& testRoleData) noexcept
            : m_roleGroupToken{roleGroupToken}
            , m_testRoleData{testRoleData}
        {}

        template <typename TCapability>
        struct CapabilityLookup
        {
            static constexpr bool c_isValidConnectionInfoType = false;
            static constexpr bool c_isValidConnectorType = false;
            static constexpr bool c_isValidControllorType = false;
            // c_capabilityFlag and c_pszCapabilityName are only provided in the base case to reduce the number of compiler
            // errors when the user passes an invalid type to one of the method templates in the TestRole class.
            static constexpr Private::TestRoleCapabilities c_capabilityFlag = Private::TestRoleCapabilities::None;
            static constexpr const wchar_t* c_pszCapabilityName = L"invalid";
        };

        template <>
        struct CapabilityLookup<NetworkConnectionInfo>
        {
            static constexpr bool c_isValidConnectionInfoType = true;
            static constexpr bool c_isValidConnectorType = false;
            static constexpr bool c_isValidControllorType = false;
            static constexpr Private::TestRoleCapabilities c_capabilityFlag = Private::TestRoleCapabilities::NetworkConnectionInfo;
            static constexpr const wchar_t* c_pszCapabilityName = L"NetworkConnectionInfo";
        };

#if !defined(TAEF_NO_CONDUIT)
        template <>
        struct CapabilityLookup<ConduitConnector>
        {
            static constexpr bool c_isValidConnectionInfoType = false;
            static constexpr bool c_isValidConnectorType = true;
            static constexpr bool c_isValidControllorType = false;
            static constexpr Private::TestRoleCapabilities c_capabilityFlag = Private::TestRoleCapabilities::ConduitConnector;
            static constexpr const wchar_t* c_pszCapabilityName = L"ConduitConnector";
        };
#endif // #if !defined(TAEF_NO_CONDUIT)

        Private::TaefLookupToken m_roleGroupToken;
        Private::TestRoleData m_testRoleData;
    };

    // Represents a set of entities in the test environment that the test can interact with. For example, these might be
    // test machines that are part of a multi-machine test.
    class TestRoleGroup
    {
        friend class TestEnvironment;
    public:
        // Returns whether the current process is executing on a device that is part of this test role group.
        bool ContainsCurrentProcess() const noexcept
        {
            return m_testRoleGroupData.IsFlagSet(Private::TestRoleGroupFlags::ContainsCurrentProcess);
        }

        // Returns whether the TAEF engine process (typically TE.exe) is executing on a device that is part of this
        // test role group.
        bool ContainsDefaultExecutionRole() const noexcept
        {
            return m_testRoleGroupData.IsFlagSet(Private::TestRoleGroupFlags::ContainsDefaultExecutionRole);
        }

        // Gets the number of test roles in this role group.
        size_t GetTestRoleCount() const noexcept
        {
            return m_testRoleGroupData.m_testRoleCount;
        }

        // Gets a specific test role from the this role group. Valid parameters range from 0 to GetTestRoleCount() - 1.
        TestRole GetTestRole(size_t index) const
        {
            if (index >= m_testRoleGroupData.m_testRoleCount)
            {
                Private::HandleTestEnvironmentFailure(E_BOUNDS,
                    L"An index of %Iu was passed to TestRoleGroup::GetTestRole, but this role group has only %Iu test roles.",
                    index, m_testRoleGroupData.m_testRoleCount);
            }

            return TestRole{m_testRoleGroupData.m_lookupToken, GetTestRoleData(index)};
        }

#if defined(TAEF_STL_SUPPORT)
        // Gets a vector containing all the test roles in this role group.
        std::vector<TestRole> GetTestRoles() const
        {
            std::vector<TestRole> testRoles;
            testRoles.reserve(m_testRoleGroupData.m_testRoleCount);
            for (size_t index = 0; index < m_testRoleGroupData.m_testRoleCount; ++index)
            {
                testRoles.push_back(TestRole{m_testRoleGroupData.m_lookupToken, GetTestRoleData(index)});
            }
            return testRoles;
        }
#endif // #if defined(TAEF_STL_SUPPORT)

    private:
        explicit TestRoleGroup(const Private::TestRoleGroupData& testRoleGroupData) noexcept
            : m_testRoleGroupData{testRoleGroupData}
        {}

        Private::TestRoleData GetTestRoleData(size_t index) const noexcept
        {
            Private::TestRoleData testRoleData;
            const HRESULT hr = Private::TestRoleGroup_GetTestRole(m_testRoleGroupData.m_lookupToken, index, &testRoleData);
            if (FAILED(hr))
            {
                // TestRoleGroup_GetTestRole only fails on invalid or out-of-range parameters. However, the wrapper
                // classes ensure they are valid and within range; so this will only fail if there is data corruption.
                // For this reason, we do a fast-fail to crash the process instead of throwing an exception.
                TAEF_FAST_FAIL;
            }
            return testRoleData;
        }

        Private::TestRoleGroupData m_testRoleGroupData;
    };

    // The test environment contains a set of properties and test roles from the outer test infrastructure that launched TAEF.
    // These APIs can be used to retrieve information about the environment that was set up for the test to use. For example,
    // if the test environment has multiple machines, tests can use these APIs to get info about the other machines.
    class TestEnvironment
    {
    public:
        // Returns whether the test environment has any property or role groups data available to be read.
        static bool IsAvailable() noexcept
        {
            return Private::TestEnvironment_IsAvailable();
        }

        // Gets the properties associated with the test environment. These properties come from the outer test infrastructure that
        // launched TAEF. TAEF doesn't guarantee that any particular keys are present. Consult the documentation for the outer test
        // infrastructure that launched TAEF to see if it exposes any properties to the test through this mechanism.
        static PropertyCollection GetProperties() noexcept
        {
            return PropertyCollection{Private::TestEnvironment_GetPropertyLookupToken()};
        }

        // Looks up a named group of test roles that were set up by the test infrastructure layer above TAEF.
        // For multi-machine tests, information about the other machines are in the test role groups.
        static TestRoleGroup GetTestRoleGroup(_In_z_ const wchar_t* pszRoleGroupName)
        {
            Private::TestRoleGroupData testRoleGroupData;
            const HRESULT hr = Private::TestEnvironment_TryGetTestRoleGroup(pszRoleGroupName, &testRoleGroupData);
            if (FAILED(hr))
            {
                // TestEnvironment_TryGetRoleGroup only fails if the role group does not exist.
                Private::HandleTestEnvironmentFailure(hr,
                    L"Failed to retrieve a test role group named \"%s\" because it does not exist in the current test environment.",
                    pszRoleGroupName);
            }

            return TestRoleGroup{testRoleGroupData};
        }

        // Returns whether TAEF has information available for a given test role group.
        static bool IsTestRoleGroupAvailable(_In_z_ const wchar_t* pszRoleGroupName) noexcept
        {
            Private::TestRoleGroupData testRoleGroupData;
            return SUCCEEDED(Private::TestEnvironment_TryGetTestRoleGroup(pszRoleGroupName, &testRoleGroupData));
        }

#if defined(TAEF_STL_SUPPORT)
        // Gets the set of names for all the test role groups that can be queried using the TestEnvironment APIs.
        static std::vector<TAEF_LPCWSTR> GetTestRoleGroupNames()
        {
            size_t testRoleGroupCount = GetTestRoleGroupCount();

            std::vector<TAEF_LPCWSTR> testRoleGroupNames;
            testRoleGroupNames.resize(testRoleGroupCount);

            const HRESULT hr = GetTestRoleGroupNames(testRoleGroupNames.data(), testRoleGroupCount, &testRoleGroupCount);
            if (FAILED(hr) || testRoleGroupCount != testRoleGroupNames.size())
            {
                // TestEnvironment_GetTestRoleGroupNames should never fail when used with correct parameters and with enough entries in
                // the array. The wrapper classes ensure these failure conditions will not be hit. If this export does fail, it
                // indicates there is data corruption. For this reason, we do a fast-fail to crash the process instead of throwing an
                // exception.
                TAEF_FAST_FAIL;
            }
            return testRoleGroupNames;
        }
#endif // #if defined(TAEF_STL_SUPPORT)

        // Gets the number of test role groups that can be queried using the TestEnvironment APIs.
        static size_t GetTestRoleGroupCount() noexcept
        {
            size_t roleGroupCount;
            const HRESULT hr = GetTestRoleGroupNames(nullptr, 0, &roleGroupCount);
            if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
            {
                // TestEnvironment_GetTestRoleGroupNames always succeeds or returns HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)
                // when these parameters are used.
                TAEF_FAST_FAIL;
            }
            return roleGroupCount;
        }

        // When using the STL, the other GetTestRoleGroupNames overload provides a simpler way to get all test role group names.
        // However, we expose this lower-level overload for users not using the STL. The other GetTestRoleGroupNames overload is
        // implemented by calling this overload; so see the implementation of the other overload for an explanation of how to use
        // this.
        static HRESULT GetTestRoleGroupNames(
            _Out_writes_to_opt_(elementCount, *roleGroupCount) TAEF_LPCWSTR* roleGroupNames,
            size_t elementCount,
            _Always_(_Out_) size_t* roleGroupCount) noexcept
        {
            return Private::TestEnvironment_GetTestRoleGroupNames(roleGroupNames, elementCount, roleGroupCount);
        }
    };
}/* namespace TestExecution */}/* namespace WEX */
#endif // #if defined(_WIN32)
