//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Waiter classes for Etw events.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "EtwProcessor.h"

#include <windows.h>

// Forward define VARIANT struct
typedef struct tagVARIANT VARIANT;

typedef struct _EVENT_RECORD EVENT_RECORD;

/// \namespace Etw::Processor
/// <summary>
/// The Etw::Processor namespace contains Etw waiter class declarations.
/// </summary>
namespace Etw { namespace Processor
{
    /// <summary>
    /// Base class for all Waiter classes.
    /// </summary>
    class Waiter
    {
        friend class CachingWaiter;
        friend class EtwWaiter;
        friend class MultiplicityWaiter;
        friend class WaiterCollection;

    public:
        ETWPROCESSOR_API virtual ~Waiter();
        Waiter(Waiter&& other)
            : m_pImpl(other.m_pImpl)
            , m_status(other.m_status)
        {
            other.m_pImpl = nullptr;
        }

        Waiter(const Waiter& other)
        {
            CopyFrom(other);
        }
        ETWPROCESSOR_API Waiter& operator=(Waiter other);

        /// <summary>Method that blocks the current thread until the required event is received</summary>
        /// <param name="timeout">Time in milliseconds to wait for</param>
        /// <returns>S_OK if received the event. HRESULT_FROM_WIN32(WAIT_TIMEOUT) on timeout.</returns>
        ETWPROCESSOR_API virtual HRESULT Wait(unsigned int timeout);

        virtual HRESULT Wait()
        {
            // We would use a default argument for this, but Wait with no parameters exists as a virtual method;
            // so removing this overload could break users that override this method.
            return Wait(c_defaultTimeout);
        }

        /// <summary>Method to reset the current waiter</summary>
        ETWPROCESSOR_API virtual void Reset();

        HRESULT GetStatus() const
        {
            return m_status;
        }

        /// <summary>Method to retrieve the message associated with the last error that modified the error code returned
        /// from GetStatus.</summary>
        ETWPROCESSOR_API const wchar_t* GetLastErrorMessage() const;

    protected:
        Waiter() = default;
        void CommonInitialize(const Waiter& waiter);

        struct Impl;
        Impl* m_pImpl = nullptr;
        HRESULT m_status = S_OK; // hr stored if any exception encountered

        static const unsigned int c_defaultTimeout = 5000; // 5 seconds

    private:
        ETWPROCESSOR_API void CopyFrom(const Waiter& other);
    };

    /// <summary>
    /// Struct to encapsulate options for customizing how to set up an EtwWaiter
    /// </summary>
    struct EtwWaiterOptions
    {
        static const unsigned long c_currentVersion = 1;

        /// <summary>Version number of the struct </summary>
        unsigned long Version = c_currentVersion;

        /// <summary>How long to wait before flushing ETW events</summary>
        unsigned long FlushTimerInMs = 500;

        explicit EtwWaiterOptions(unsigned long flushTimerInMs = 500)
            : FlushTimerInMs(flushTimerInMs)
        {
        }
    };

    /// <summary>
    /// Light weight waiter to wait for an Etw event
    /// </summary>
    class EtwWaiter : public Waiter
    {
    public:
        /// <summary>Construct an EtwWaiter</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="eventId">Event Identifier</param>
        EtwWaiter(GUID providerGuid, unsigned long eventId)
        {
            Initialize(providerGuid, eventId, L"", EtwWaiterOptions());
        }
        /// <summary>Construct an EtwWaiter</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="eventId">Event Identifier</param>
        /// <param name="pszPayloadCriteria">String representing payload to filter the event</param>
        EtwWaiter(GUID providerGuid, unsigned long eventId, const wchar_t* pszPayloadCriteria)
        {
            Initialize(providerGuid, eventId, pszPayloadCriteria, EtwWaiterOptions());
        }
        /// <summary>Construct an EtwWaiter</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="eventId">Event Identifier</param>
        /// <param name="options">Options on ETW trace session</param>
        EtwWaiter(GUID providerGuid, unsigned long eventId, const EtwWaiterOptions& options)
        {
            Initialize(providerGuid, eventId, L"", options);
        }
        /// <summary>Construct an EtwWaiter</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="eventId">Event Identifier</param>
        /// <param name="pszPayloadCriteria">String representing payload to filter the event</param>
        /// <param name="options">Options on ETW trace session</param>
        EtwWaiter(GUID providerGuid, unsigned long eventId, const wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options)
        {
            Initialize(providerGuid, eventId, pszPayloadCriteria, options);
        }

        /// <summary>Construct an EtwWaiter for a TraceLogging event</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="pszTaskName">Task Name</param>
        EtwWaiter(GUID providerGuid, const wchar_t* pszTaskName)
        {
            Initialize(providerGuid, pszTaskName, L"", EtwWaiterOptions());
        }
        /// <summary>Construct an EtwWaiter for a TraceLogging event</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="pszTaskName">Task Name</param>
        /// <param name="pszPayloadCriteria">String representing payload to filter the event</param>
        EtwWaiter(GUID providerGuid, const wchar_t* pszTaskName, const wchar_t* pszPayloadCriteria)
        {
            Initialize(providerGuid, pszTaskName, pszPayloadCriteria, EtwWaiterOptions());
        }
        /// <summary>Construct an EtwWaiter for a TraceLogging event</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="pszTaskName">Task Name</param>
        /// <param name="options">Options on ETW trace session</param>
        EtwWaiter(GUID providerGuid, const wchar_t* pszTaskName, const EtwWaiterOptions& options)
        {
            Initialize(providerGuid, pszTaskName, L"", options);
        }
        /// <summary>Construct an EtwWaiter for a TraceLogging event</summary>
        /// <param name="providerGuid">Guid of the provider that raises events</param>
        /// <param name="pszTaskName">Task Name</param>
        /// <param name="pszPayloadCriteria">String representing payload to filter the event</param>
        /// <param name="options">Options on ETW trace session</param>
        EtwWaiter(GUID providerGuid, const wchar_t* pszTaskName, const wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options)
        {
            Initialize(providerGuid, pszTaskName, pszPayloadCriteria, options);
        }

        ETWPROCESSOR_API ~EtwWaiter();

    private:
        ETWPROCESSOR_API void Initialize(const GUID& providerGuid, unsigned long eventId, const wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options);
        ETWPROCESSOR_API void Initialize(const GUID& providerGuid, const wchar_t* pszTaskName, const wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options);

#if defined(ETWPROCESSOR_FULL_BUILD)
        ETWPROCESSOR_API void Initialize(const GUID& providerGuid, unsigned long eventId, const __wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options);
        ETWPROCESSOR_API void Initialize(const GUID& providerGuid, const __wchar_t* pszTaskName, const __wchar_t* pszPayloadCriteria, const EtwWaiterOptions& options);
#endif
    };

    struct PayloadProperty;

    /// <summary>
    /// Class that represents an etw event
    /// </summary>
    class EtwEvent
    {
        friend class CachingWaiter;

    public:
        ETWPROCESSOR_API static EtwEvent __stdcall FromEventRecord(EVENT_RECORD* pEventRecord);

        ETWPROCESSOR_API ~EtwEvent();
        ETWPROCESSOR_API EtwEvent(const EtwEvent& other);
        ETWPROCESSOR_API EtwEvent& operator=(EtwEvent other);

        HRESULT GetStatus() const
        {
            return m_status;
        }

        /// <summary>Method to retrieve the message associated with the last error that modified the error code returned
        /// from GetStatus.</summary>
        ETWPROCESSOR_API const wchar_t* GetLastErrorMessage() const;

        /// <summary>Method to get the event identifier</summary>
        ETWPROCESSOR_API unsigned long GetEventId() const;
        /// <summary>Method to get the event version</summary>
        ETWPROCESSOR_API unsigned char GetVersion() const;
        /// <summary>Method to get the event Channel</summary>
        ETWPROCESSOR_API unsigned char GetChannel() const;
        /// <summary>Method to get the event Level</summary>
        ETWPROCESSOR_API unsigned char GetLevel() const;
        /// <summary>Method to get the event OpCode</summary>
        ETWPROCESSOR_API unsigned char GetOpCode() const;
        /// <summary>Method to get the event thred Identifier</summary>
        ETWPROCESSOR_API unsigned long GetThreadId() const;
        /// <summary>Method to get the event Process Identifier</summary>
        ETWPROCESSOR_API unsigned long GetProcessId() const;
        /// <summary>Method to get the event provider Guid</summary>
        ETWPROCESSOR_API GUID GetProviderGuid() const;
        /// <summary>Method to get the event activity ID</summary>
        ETWPROCESSOR_API GUID GetActivityId() const;
        /// <summary>
        /// Method to get the event timestamp. Beware that the timestamp is calculated using the current machine's
        /// performance frequency. If this event is being read from a file, the timestamp may be inaccurate if the
        /// frequency was different when the trace was recorded.
        /// </summary>
        ETWPROCESSOR_API LARGE_INTEGER GetTimeStamp() const;
        /// <summary>Method to get the whether the event has a related (parent) activity</summary>
        ETWPROCESSOR_API bool HasRelatedActivity() const;
        /// <summary>Method to get the event related (parent) activity ID</summary>
        ETWPROCESSOR_API GUID GetRelatedActivityId() const;

        ETWPROCESSOR_API const unsigned short* GetProviderName() const;
        ETWPROCESSOR_API const unsigned short* GetLevelName() const;
        ETWPROCESSOR_API const unsigned short* GetChannelName() const;
        ETWPROCESSOR_API const unsigned short* GetKeywordsName() const;
        ETWPROCESSOR_API const unsigned short* GetTaskName() const;
        ETWPROCESSOR_API const unsigned short* GetOpCodeName() const;
        ETWPROCESSOR_API const unsigned short* GetEventMessage() const;
        ETWPROCESSOR_API const unsigned short* GetProviderMessage() const;

        /// <summary>Method that prints the event properties to the console</summary>
        ETWPROCESSOR_API void PrintProperties();
        /// <summary>Method to retrieve payload property value of the event</summary>
        /// <param name="pszPropertyName">Name of the property to retrieve</param>
        /// <param name="variant">VARIANT structure that holds the value of the property</param>
        ETWPROCESSOR_API HRESULT GetPayloadProperty(const wchar_t* pszPropertyName, VARIANT& variant) const;

        /// <summary>Method to retrieve the list of Payload Property names available into a buffer and count of avaialble properties;
        /// this function does not allocate memory. It is valid to call this with a null buffer with 0 length
        /// if you only want the property count. If buffer is provided but length is less than property count then
        /// ERROR_INSUFFICIENT_BUFFER will be returned</summary>
        /// <param name="ppszEventPropertyNames">The array buffer that will be filled with available payload property names</param>
        /// <param name="length">The array buffer length that indicates max size of the buffer that will be filled to size of buffer
        ///   or total of properties available (whichever is greater)</param>
        /// <param name="pLengthNeeded">Pointer that will be given the total count of properties this payload has available</param>
        ETWPROCESSOR_API HRESULT GetPayloadPropertyNames(_Out_writes_to_opt_(length, *pLengthNeeded) LPCWSTR* ppszEventPropertyNames, const unsigned long length, _Out_ unsigned long* pLengthNeeded) const;

        #if defined(ETWPROCESSOR_FULL_BUILD)
        ETWPROCESSOR_API HRESULT GetPayloadProperty(const __wchar_t* pszPropertyName, VARIANT& variant) const;
        ETWPROCESSOR_API HRESULT GetPayloadPropertyNames(_Out_writes_to_opt_(length, *pLengthNeeded) const __wchar_t** ppEventPropertyList, const unsigned long length, _Out_ unsigned long* pLengthNeeded) const;
        #endif

    private:
        static HRESULT __stdcall EtwEvent_GetPayloadProperty(EtwEvent* pEtwEvent, const wchar_t* pszPropertyName, const PayloadProperty** ppValue);
        static HRESULT __stdcall EtwEvent_GetPayloadPropertyLegacy(EtwEvent* pEtwEvent, const wchar_t* pszPropertyName, VARIANT& variant);

        static HRESULT __stdcall EtwEvent_GetPayloadPropertyNames(EtwEvent *pEtwEvent, _Out_writes_to_opt_(length, *pLengthNeeded) LPCWSTR* ppszEventPropertyNames, const unsigned long length, _Out_ unsigned long* pLengthNeeded);

        EtwEvent();

        struct Impl;
        Impl* m_pImpl = nullptr;
        HRESULT m_status = S_OK;
    };

    /// <summary>
    /// Waiter that also caches the event that is waiting for
    /// </summary>
    class CachingWaiter : public Waiter
    {
    public:
        /// <summary>Constructor for CachingWaiter</summary>
        /// <param name="waiter">Waiter object</param>
        CachingWaiter(const Waiter& waiter)
        {
            Initialize(waiter);
        }

        /// <summary>Constructor for CachingWaiter</summary>
        /// <param name="waiter">Waiter object</param>
        /// <param name="pszPayloadCriteria">String representing payload to filter the event</param>
        CachingWaiter(const Waiter& waiter, const wchar_t* pszPayloadCriteria)
        {
            Initialize(waiter, pszPayloadCriteria);
        }
        
        ~CachingWaiter() {}

        /// <summary>Method to get the Cached event</summary>
        ETWPROCESSOR_API EtwEvent GetCachedEvent();

    private:
        ETWPROCESSOR_API void Initialize(const Waiter& waiter);
        ETWPROCESSOR_API void Initialize(const Waiter& waiter, const wchar_t* pszPayloadCriteria);
#if defined(ETWPROCESSOR_FULL_BUILD)
        ETWPROCESSOR_API void Initialize(const Waiter& waiter, const __wchar_t* pszPayloadCriteria);
#endif
    };


    /// <summary>
    /// Class that represents a collection of Etw waiter classes
    /// </summary>
    class WaiterCollection
    {
        friend class CompositeWaiter;
    public:
        ETWPROCESSOR_API WaiterCollection();
        ETWPROCESSOR_API ~WaiterCollection();

        ETWPROCESSOR_API WaiterCollection(const WaiterCollection& other);
        ETWPROCESSOR_API WaiterCollection& operator=(WaiterCollection other);

        HRESULT GetStatus() const
        {
            return m_status;
        }

        /// <summary>Method to retrieve the message associated with the last error that modified the error code returned
        /// from GetStatus.</summary>
        ETWPROCESSOR_API const wchar_t* GetLastErrorMessage() const;

        /// <summary>Method to add a waiter to the collection</summary>
        /// <param name="waiter">Waiter object</param>
        ETWPROCESSOR_API void Add(const Waiter& waiter);

    protected:
        struct Impl;
        Impl* m_pImpl = nullptr;
        HRESULT m_status = S_OK;
    };

    /// <summary>
    /// Waiter to wait for multiple instances of a same event
    /// </summary>
    class MultiplicityWaiter : public Waiter
    {
    public:
        /// <summary>Constructor for MultiplicityWaiter</summary>
        /// <param name="waiter">Waiter object</param>
        /// <param name="instances">Number of instances of the Event</param>
        MultiplicityWaiter(const Waiter& waiter, unsigned int instances)
        {
            Initialize(waiter, instances);
        }

        ~MultiplicityWaiter() {}

    private:
        ETWPROCESSOR_API void Initialize(const Waiter& waiter, unsigned int instances);
    };

    enum class DefaultWaitBehavior
    {
        Any = 0,
        All
    };

    /// <summary>
    /// Waiter to WaitForAny or WaitForAll events from multiple providers at the same time
    /// </summary>
    class CompositeWaiter : public Waiter
    {
    public:
        CompositeWaiter(const WaiterCollection& waiterCollection)
        {
            Initialize(waiterCollection, DefaultWaitBehavior::Any);
        }
        CompositeWaiter(const WaiterCollection& waiterCollection, DefaultWaitBehavior defaultWaitBehavior)
        {
            Initialize(waiterCollection, defaultWaitBehavior);
        }
        ~CompositeWaiter() {}

        ETWPROCESSOR_API HRESULT WaitAny(unsigned int timeout = c_defaultTimeout);
        ETWPROCESSOR_API HRESULT WaitAll(unsigned int timeout = c_defaultTimeout);

    private:
        ETWPROCESSOR_API void Initialize(const WaiterCollection& waiterCollection, DefaultWaitBehavior defaultWaitBehavior);
    };
}/* namespace Processor */}/* namespace Etw */
