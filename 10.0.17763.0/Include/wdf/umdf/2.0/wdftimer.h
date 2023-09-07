/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Wdftimer.h

Environment:

    user mode

NOTE: This header is generated by stubwork.

      To modify contents, add or remove <shared> or <umdf>
      tags in the corresponding .x and .y template files.

--*/

#pragma once

#ifndef WDF_EXTERN_C
  #ifdef __cplusplus
    #define WDF_EXTERN_C       extern "C"
    #define WDF_EXTERN_C_START extern "C" {
    #define WDF_EXTERN_C_END   }
  #else
    #define WDF_EXTERN_C
    #define WDF_EXTERN_C_START
    #define WDF_EXTERN_C_END
  #endif
#endif

WDF_EXTERN_C_START


#define TolerableDelayUnlimited ((ULONG)-1)

//
// This is the function that gets called back into the driver
// when the TIMER fires.
//
typedef
_Function_class_(EVT_WDF_TIMER)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_TIMER(
    _In_
    WDFTIMER Timer
    );

typedef EVT_WDF_TIMER *PFN_WDF_TIMER;

//
// Disable warning C4324: structure was padded due to DECLSPEC_ALIGN
// This padding is intentional and necessary.
#pragma warning(push)
#pragma warning(disable: 4324)

typedef struct _WDF_TIMER_CONFIG {
    ULONG Size;
    PFN_WDF_TIMER EvtTimerFunc;

    ULONG Period;

    //
    // If this is TRUE, the Timer will automatically serialize
    // with the event callback handlers of its Parent Object.
    //
    // Parent Object's callback constraints should be compatible
    // with the Timer DPC (DISPATCH_LEVEL), or the request will fail.
    //
    BOOLEAN AutomaticSerialization;

    //
    // Optional tolerance for the timer in milliseconds.
    //
    ULONG TolerableDelay;
    
    //
    // If this is TRUE, high resolution timers will be used. The default
    // value is FALSE
    //
    DECLSPEC_ALIGN(8) BOOLEAN UseHighResolutionTimer;

} WDF_TIMER_CONFIG, *PWDF_TIMER_CONFIG; 

#pragma warning(pop)

VOID
FORCEINLINE
WDF_TIMER_CONFIG_INIT(
    _Out_ PWDF_TIMER_CONFIG Config,
    _In_  PFN_WDF_TIMER     EvtTimerFunc
    )
{
    RtlZeroMemory(Config, sizeof(WDF_TIMER_CONFIG));
    Config->Size = sizeof(WDF_TIMER_CONFIG);
    Config->EvtTimerFunc = EvtTimerFunc;
    Config->Period = 0;
    Config->AutomaticSerialization = TRUE;
    Config->TolerableDelay = 0;
}

VOID
FORCEINLINE
WDF_TIMER_CONFIG_INIT_PERIODIC(
    _Out_ PWDF_TIMER_CONFIG Config,
    _In_  PFN_WDF_TIMER     EvtTimerFunc,
    _In_  LONG             Period
    )
{
    RtlZeroMemory(Config, sizeof(WDF_TIMER_CONFIG));
    Config->Size = sizeof(WDF_TIMER_CONFIG);
    Config->EvtTimerFunc = EvtTimerFunc;
    Config->Period = Period;
    Config->AutomaticSerialization = TRUE;
    Config->TolerableDelay = 0;
}

//
// WDF Function: WdfTimerCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFTIMERCREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    PWDF_TIMER_CONFIG Config,
    _In_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFTIMER* Timer
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfTimerCreate(
    _In_
    PWDF_TIMER_CONFIG Config,
    _In_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _Out_
    WDFTIMER* Timer
    )
{
    return ((PFN_WDFTIMERCREATE) WdfFunctions[WdfTimerCreateTableIndex])(WdfDriverGlobals, Config, Attributes, Timer);
}


//
// WDF Function: WdfTimerStart
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFTIMERSTART)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer,
    _In_
    LONGLONG DueTime
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfTimerStart(
    _In_
    WDFTIMER Timer,
    _In_
    LONGLONG DueTime
    )
{
    return ((PFN_WDFTIMERSTART) WdfFunctions[WdfTimerStartTableIndex])(WdfDriverGlobals, Timer, DueTime);
}


//
// WDF Function: WdfTimerStop
//
typedef
_When_(Wait == __true, _IRQL_requires_max_(PASSIVE_LEVEL))
_When_(Wait == __false, _IRQL_requires_max_(DISPATCH_LEVEL))
WDFAPI
BOOLEAN
(*PFN_WDFTIMERSTOP)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer,
    _In_
    BOOLEAN Wait
    );

_When_(Wait == __true, _IRQL_requires_max_(PASSIVE_LEVEL))
_When_(Wait == __false, _IRQL_requires_max_(DISPATCH_LEVEL))
BOOLEAN
FORCEINLINE
WdfTimerStop(
    _In_
    WDFTIMER Timer,
    _In_
    BOOLEAN Wait
    )
{
    return ((PFN_WDFTIMERSTOP) WdfFunctions[WdfTimerStopTableIndex])(WdfDriverGlobals, Timer, Wait);
}


//
// WDF Function: WdfTimerGetParentObject
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFOBJECT
(*PFN_WDFTIMERGETPARENTOBJECT)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFTIMER Timer
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFOBJECT
FORCEINLINE
WdfTimerGetParentObject(
    _In_
    WDFTIMER Timer
    )
{
    return ((PFN_WDFTIMERGETPARENTOBJECT) WdfFunctions[WdfTimerGetParentObjectTableIndex])(WdfDriverGlobals, Timer);
}

WDF_EXTERN_C_END

