//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Consumer class for Real time Etw events</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "EtwProcessor.h"

#include "EtwConsumer.h"

// Suppress warning 4481: nonstandard extension used: override specifier 'override'
#pragma warning(push)
#pragma warning(disable: 4481)

namespace Etw { namespace Processor
{
    /// <summary>
    /// Struct to encapsulate options for customizing how to set up an EtwRealtimeConsumer
    /// </summary>
    struct EtwRealtimeConsumerOptions
    {
        static const unsigned long c_currentVersion = 1;

        /// <summary>Version number of the struct </summary>
        unsigned long Version = c_currentVersion;

        /// <summary>How long to wait before flushing ETW events</summary>
        unsigned long FlushTimerInMs = 500;

        explicit EtwRealtimeConsumerOptions(unsigned long flushTimerInMs = 500)
            : FlushTimerInMs(flushTimerInMs)
        {
        }
    };

    /// <summary>
    /// Consumer class for Real time Etw events
    /// </summary>
    class EtwRealtimeConsumer : public EtwConsumer 
    {
    public:
        /// <summary>Construct a real time etw consumer</summary>
        /// <param name="pszTraceName">Name of the trace session to consume the events from</param>
        EtwRealtimeConsumer(const wchar_t* pszTraceName)
        {
            Initialize(pszTraceName, EtwRealtimeConsumerOptions());
        }

        /// <summary>Construct a real time etw consumer</summary>
        /// <param name="pszTraceName">Name of the trace session to consume the events from</param>
        /// <param name="options">Options to configure the consumer</param>
        EtwRealtimeConsumer(const wchar_t* pszTraceName, const EtwRealtimeConsumerOptions& options)
        {
            Initialize(pszTraceName, options);
        }

        ETWPROCESSOR_API ~EtwRealtimeConsumer();

        /// <summary>Method to add/register an event provider in this trace session</summary>
        /// <param name="providerGuid">Guid of the event provider</param>
        /// <returns>S_OK if successfully enabled the provider</returns>
        ETWPROCESSOR_API HRESULT EnableProvider(const GUID& providerGuid);

        /// <summary>Method to add/register an event provider in this trace session</summary>
        /// <param name="providerGuid">Guid of the event provider</param>
        /// <param name="level">Provider-defined value that specifies level of detail included in the event.
        ///     This is used for the Level parameter of EnableTraceEx2.</param>
        /// <param name="matchAnyKeyword">Bit mask of keywords that determine the category of events.
        ///     This is used for the MatchAnyKeyword parameter of EnableTraceEx2.</param>
        /// <param name="matchAllKeyword">Bit mask of keywords that determine the category of events.
        ///     This is used for the MatchAllKeyword parameter of EnableTraceEx2.</param>
        /// <param name="enableProperty">Optional information that Etw can include when writing the events.
        ///     This is used in ENABLE_TRACE_PARAMETERS::EnableProperty.</param>
        /// <returns>S_OK if successfully enabled the provider</returns>
        ETWPROCESSOR_API HRESULT EnableProvider(const GUID& providerGuid, unsigned char level,
            unsigned __int64 matchAnyKeyword, unsigned __int64 matchAllKeyword, unsigned long enableProperty);

        /// <summary>Method to remove/un-register an event provider in this trace session</summary>
        /// <param name="providerGuid">Guid of the event provider</param>
        /// <returns>S_OK if successfully disabled the provider</returns>
        ETWPROCESSOR_API HRESULT DisableProvider(const GUID& provierGuid);

        // Open a trace for the registered providers.
        ETWPROCESSOR_API virtual HRESULT OpenTraceW() override;

        // Flush the underlying ETW trace session.
        ETWPROCESSOR_API HRESULT Flush();

    private:
        EtwRealtimeConsumer(const EtwRealtimeConsumer&) = delete;
        EtwRealtimeConsumer& operator=(const EtwRealtimeConsumer&) = delete;

        // We export Initialize methods instead of exporting the constructor to force the calling DLL to use its
        // own virtual function table. That way the new and delete happen from the same DLL.
        ETWPROCESSOR_API void Initialize(const wchar_t* pszTraceName, const EtwRealtimeConsumerOptions& options);
#if defined(ETWPROCESSOR_FULL_BUILD)
        ETWPROCESSOR_API void Initialize(const __wchar_t* pszTraceName, const EtwRealtimeConsumerOptions& options);
#endif

        // EventProvider is an internal class used within ETW Processor as part of the EtwWaiter implementation.
        // It is a friend to provide access to FlushImpl and m_status. We don't currently expose those publicly.
        friend class EventProvider;
        void FlushImpl() const;

        // This stores the result of construction. The constructor itself does not throw on failure.
        // Any method call on an instance which failed construction will return this error code.
        HRESULT m_status = S_OK;

        struct EtwRealImpl;
        EtwRealImpl* m_pImpl = nullptr;
    };
}/* namespace Processor */}/* namespace Etw */

#pragma warning(pop)
