//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Controller class for event trace sessions</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "EtwProcessor.h"

#include <Windows.h>

namespace Etw { namespace Processor
{
    /// <summary>
    /// Struct to encapsulate options for customizing how to set up an EtwController
    /// </summary>
    struct EtwControllerOptions
    {
        static const unsigned long c_defaultMaxFileSize = 200;
        
        static const unsigned long c_currentVersion = 2;

        /// <summary>Version number of the struct </summary>
        unsigned long Version = c_currentVersion;
        
        /// <summary>How long to wait before flushing ETW events</summary>
        unsigned long FlushTimerInMs = 500;

        /// <summary>Maximum file size in megabytes</summary>
        unsigned long MaximumFileSize = c_defaultMaxFileSize;

        /// <summary>Append existing etl file or write new one</summary>
        bool AppendEtlFile = false; 

        /// <summary>Clock Resolution to use. Note that EVENT_TRACE_CLOCK_SYSTEMTIME (0x00000002) is recommended when AppendEtlFile is set to true</summary>
        unsigned long ClockResolution = 0x00000001; // EVENT_TRACE_CLOCK_PERFCOUNTER -- we don't define to prevent redefine collisions

        /// <summary>Default constructor - will use the default value for all options</summary>
        explicit EtwControllerOptions(unsigned long flushTimerInMs = 500, unsigned long maximumFileSize = c_defaultMaxFileSize, 
            bool appendEtlFile = false, unsigned long clockResolution = 0x00000001 /*EVENT_TRACE_CLOCK_PERFCOUNTER*/)
            : FlushTimerInMs(flushTimerInMs)
            , MaximumFileSize(maximumFileSize)
            , AppendEtlFile(appendEtlFile)
            , ClockResolution(clockResolution)
        {
        }
    };

    /// <summary>
    /// Controller class to start and stop trace sessions
    /// </summary>
    class EtwController
    {
    public:
        /// <summary>Method to start an event trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="maximumFileSize">Maximum file size in megabytes</param>
        /// <returns>S_OK if successfully started the trace</returns>
        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const wchar_t* pszTraceName, unsigned long maximumFileSize = EtwControllerOptions::c_defaultMaxFileSize);

        /// <summary>Method to start an event trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="options">Options to configure the ETW trace session</param>
        /// <returns>S_OK if successfully started the trace</returns>
        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const wchar_t* pszTraceName, const EtwControllerOptions& options);

        /// <summary>Method to start an event trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="pszFileName">Name of the etl file</param>
        /// <param name="maximumFileSize">Maximum file size in megabytes</param>
        /// <returns>S_OK if successfully started the trace</returns>
        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const wchar_t* pszTraceName, const wchar_t* pszFileName, unsigned long maximumFileSize = EtwControllerOptions::c_defaultMaxFileSize);

        /// <summary>Method to start an event trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="pszFileName">Name of the etl file</param>
        /// <param name="options">Options to configure the ETW trace session</param>
        /// <returns>S_OK if successfully started the trace</returns>
        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const wchar_t* pszTraceName, const wchar_t* pszFileName, const EtwControllerOptions& options);

        /// <summary>Method to stop an event trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <returns>S_OK if successfully stopped the trace</returns>
        ETWPROCESSOR_API static HRESULT __stdcall StopEtwTrace(const wchar_t* pszTraceName);

        /// <summary>Method to add/register an event provider in a trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
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
        ETWPROCESSOR_API static HRESULT __stdcall EnableProvider(const wchar_t* pszTraceName, const GUID& providerGuid, unsigned char level,
            unsigned __int64 matchAnyKeyword, unsigned __int64 matchAllKeyword, unsigned long enableProperty);

        /// <summary>Method to add/register an event provider in a trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="providerGuid">Guid of the event provider</param>
        /// <returns>S_OK if successfully enabled the provider</returns>
        ETWPROCESSOR_API static HRESULT __stdcall EnableProvider(const wchar_t* pszTraceName, const GUID& providerGuid);

        /// <summary>Method to un-register/remove an event provider in a trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <param name="providerGuid">Guid of the event provider</param>
        /// <returns>S_OK if successfully disabled the provider</returns>
        ETWPROCESSOR_API static HRESULT __stdcall DisableProvider(const wchar_t* pszTraceName, const GUID& providerGuid);

        /// <summary>Method to flush events of a trace session</summary>
        /// <param name="pszTraceName">Name of the trace session</param>
        /// <returns>S_OK if successfully flushed the events</returns>
        ETWPROCESSOR_API static HRESULT __stdcall Flush(const wchar_t* pszTraceName);

        #ifdef ETWPROCESSOR_FULL_BUILD
        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const __wchar_t* pszTraceName, unsigned long maximumFileSize = EtwControllerOptions::c_defaultMaxFileSize)
        {
            return EtwController::StartEtwTrace(reinterpret_cast<const wchar_t*>(pszTraceName), maximumFileSize);
        }

        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const __wchar_t* pszTraceName, const EtwControllerOptions&)
        {
            return EtwController::StartEtwTrace(reinterpret_cast<const wchar_t*>(pszTraceName));
        }

        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const __wchar_t* pszTraceName, const __wchar_t* pszFileName, unsigned long maximumFileSize = EtwControllerOptions::c_defaultMaxFileSize)
        {
            return EtwController::StartEtwTrace(reinterpret_cast<const wchar_t*>(pszTraceName), reinterpret_cast<const wchar_t*>(pszFileName), maximumFileSize);
        }

        ETWPROCESSOR_API static HRESULT __stdcall StartEtwTrace(const __wchar_t* pszTraceName, const __wchar_t* pszFileName, const EtwControllerOptions&)
        {
            return EtwController::StartEtwTrace(reinterpret_cast<const wchar_t*>(pszTraceName), reinterpret_cast<const wchar_t*>(pszFileName));
        }

        ETWPROCESSOR_API static HRESULT __stdcall StopEtwTrace(const __wchar_t* pszTraceName)
        {
            return EtwController::StopEtwTrace(reinterpret_cast<const wchar_t*>(pszTraceName));
        }

        ETWPROCESSOR_API static HRESULT __stdcall EnableProvider(const __wchar_t* pszTraceName, const GUID& providerGuid, unsigned char level, unsigned __int64 matchAnyKeyword, unsigned __int64 matchAllKeyword, unsigned long enableProperty)
        {
            return EtwController::EnableProvider(reinterpret_cast<const wchar_t*>(pszTraceName), providerGuid, level, matchAnyKeyword, matchAllKeyword, enableProperty);
        }

        ETWPROCESSOR_API static HRESULT __stdcall EnableProvider(const __wchar_t* pszTraceName, const GUID& providerGuid)
        {
            return EtwController::EnableProvider(reinterpret_cast<const wchar_t*>(pszTraceName), providerGuid);
        }

        ETWPROCESSOR_API static HRESULT __stdcall DisableProvider(const __wchar_t* pszTraceName, const GUID& providerGuid)
        {
            return EtwController::DisableProvider(reinterpret_cast<const wchar_t*>(pszTraceName), providerGuid);
        }

        ETWPROCESSOR_API static HRESULT __stdcall Flush(const __wchar_t* pszTraceName)
        {
            return EtwController::Flush(reinterpret_cast<const wchar_t*>(pszTraceName));
        }
        #endif

    private:
        EtwController();//not implemented
        ~EtwController();//not implemented
        EtwController(const EtwController&);//not implemented
        EtwController& operator=(const EtwController&);//not implemented
    };
}/* namespace Processor */}/* namespace Etw */
