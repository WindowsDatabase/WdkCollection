//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Consumer class for Etw events</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "EtwProcessor.h"

#include <Windows.h>
#include <Wmistr.h> // In older Windows headers, you have to include Wmistr.h before Evntrace.h.
#include <Evntrace.h>
#include <tdh.h>

namespace Etw { namespace Processor
{
    class EtwConsumerImpl;

    /// <summary>
    /// Base class for consumers of Etw events
    /// </summary>
    class __declspec(novtable) EtwConsumer abstract
    {
        friend class EtwConsumerImpl;
    public:
        virtual ~EtwConsumer()
        {}

        /// <summary>Method to open the trace for processing</summary>
        /// <returns>S_OK on success</returns>
        virtual HRESULT OpenTraceW() = 0;

        /// <summary>Method to Process the trace</summary>
        /// <returns>S_OK on success</returns>
        ETWPROCESSOR_API virtual HRESULT ProcessTrace();

        /// <summary>Method to close the trace</summary>
        /// <returns>S_OK on closing the trace</returns>
        ETWPROCESSOR_API HRESULT CloseTrace();

        // This method is not supported. Use SetEventRecordCallback instead.
        void SetEventCallback(PEVENT_CALLBACK) {}

        /// <summary>Method to set the event record callback function to be executed</summary>
        /// <param name="pEventCallback">Pointer to the event record callback function</param>
        /// <param name="context">Any context information that can be accessed from the callback</param>
        void SetEventRecordCallback(PEVENT_RECORD_CALLBACK pfnEventRecordCallback, LPVOID context)
        {
            m_pfnEventRecordCallback = pfnEventRecordCallback;
            m_pContext = context;
        }

        // This method is not currently exported. It is used internally within ETW Processor.
        void SetBufferCallback(PEVENT_TRACE_BUFFER_CALLBACKW pfnBufferCallback);

        /// <summary>Method to retrieve the tick frequency after a call to OpenTrace</summary>
        long long GetTickFrequency() const
        {
            return m_frequency;
        }

        /// <summary>Method to retrieve the initial start time of the Trace</summary>
        long long GetStartTimeStamp() const
        {
            return m_startTimeStamp;
        }

        /// <summary>Method to retrieve the initial end time of the Trace</summary>
        long long GetEndTimeStamp() const
        {
            return m_endTimeStamp;
        }

        /// <summary>Method to retrieve the timer resolution</summary>
        unsigned long GetTimerResolution() const
        {
            return m_timerResolution;
        }

    protected:
        EtwConsumer() = default;

    private:
        EtwConsumer(const EtwConsumer&) = delete;
        EtwConsumer& operator=(const EtwConsumer&) = delete;

        EtwConsumerImpl& GetEtwConsumerImpl();

    protected:
        // This is actually a pointer to internal state, but we store it in an unsigned 64-bit int
        // for compatibility reasons.
        unsigned __int64 m_privateState = 0;

        // Data variable that can be accessed in the callback via EVENT_RECORD::UserContext.
        LPVOID m_pContext = nullptr;
        
        // This used to store a PEVENT_CALLBACK. We require a PEVENT_RECORD_CALLBACK now.
        void* m_pReserved = nullptr;

        PEVENT_RECORD_CALLBACK m_pfnEventRecordCallback = nullptr;

        long long m_frequency = 0;
        long long m_startTimeStamp = 0;
        long long m_endTimeStamp = 0;
        unsigned long m_timerResolution = 0;
    };
}/* namespace Processor */}/* namespace Etw */
