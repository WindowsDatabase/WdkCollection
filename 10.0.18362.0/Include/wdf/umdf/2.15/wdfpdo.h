/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Wdfpdo.h

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

typedef
_Function_class_(EVT_WDF_DEVICE_RESOURCES_QUERY)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_RESOURCES_QUERY(
    _In_
    WDFDEVICE Device,
    _In_
    WDFCMRESLIST Resources
    );

typedef EVT_WDF_DEVICE_RESOURCES_QUERY *PFN_WDF_DEVICE_RESOURCES_QUERY;

typedef
_Function_class_(EVT_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY(
    _In_
    WDFDEVICE Device,
    _In_
    WDFIORESREQLIST IoResourceRequirementsList
    );

typedef EVT_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY *PFN_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY;

typedef
_Function_class_(EVT_WDF_DEVICE_EJECT)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_EJECT(
    _In_
    WDFDEVICE Device
    );

typedef EVT_WDF_DEVICE_EJECT *PFN_WDF_DEVICE_EJECT;

typedef
_Function_class_(EVT_WDF_DEVICE_SET_LOCK)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_SET_LOCK(
    _In_
    WDFDEVICE Device,
    _In_
    BOOLEAN IsLocked
    );

typedef EVT_WDF_DEVICE_SET_LOCK *PFN_WDF_DEVICE_SET_LOCK;

typedef
_Function_class_(EVT_WDF_DEVICE_ENABLE_WAKE_AT_BUS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_ENABLE_WAKE_AT_BUS(
    _In_
    WDFDEVICE Device,
    _In_
    SYSTEM_POWER_STATE PowerState
    );

typedef EVT_WDF_DEVICE_ENABLE_WAKE_AT_BUS *PFN_WDF_DEVICE_ENABLE_WAKE_AT_BUS;

typedef
_Function_class_(EVT_WDF_DEVICE_DISABLE_WAKE_AT_BUS)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
EVT_WDF_DEVICE_DISABLE_WAKE_AT_BUS(
    _In_
    WDFDEVICE Device
    );

typedef EVT_WDF_DEVICE_DISABLE_WAKE_AT_BUS *PFN_WDF_DEVICE_DISABLE_WAKE_AT_BUS;

typedef
_Function_class_(EVT_WDF_DEVICE_REPORTED_MISSING)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
EVT_WDF_DEVICE_REPORTED_MISSING(
    _In_
    WDFDEVICE Device
    );

typedef EVT_WDF_DEVICE_REPORTED_MISSING *PFN_WDF_DEVICE_REPORTED_MISSING;

typedef struct _WDF_PDO_EVENT_CALLBACKS {
    //
    // The size of this structure in bytes
    //
    ULONG Size;

    //
    // Called in response to IRP_MN_QUERY_RESOURCES
    //
    PFN_WDF_DEVICE_RESOURCES_QUERY EvtDeviceResourcesQuery;

    //
    // Called in response to IRP_MN_QUERY_RESOURCE_REQUIREMENTS
    //
    PFN_WDF_DEVICE_RESOURCE_REQUIREMENTS_QUERY EvtDeviceResourceRequirementsQuery;

    //
    // Called in response to IRP_MN_EJECT
    //
    PFN_WDF_DEVICE_EJECT EvtDeviceEject;

    //
    // Called in response to IRP_MN_SET_LOCK
    //
    PFN_WDF_DEVICE_SET_LOCK EvtDeviceSetLock;

    //
    // Called in response to the power policy owner sending a wait wake to the
    // PDO.  Bus generic arming shoulding occur here.
    //
    PFN_WDF_DEVICE_ENABLE_WAKE_AT_BUS       EvtDeviceEnableWakeAtBus;

    //
    // Called in response to the power policy owner sending a wait wake to the
    // PDO.  Bus generic disarming shoulding occur here.
    //
    PFN_WDF_DEVICE_DISABLE_WAKE_AT_BUS      EvtDeviceDisableWakeAtBus;

    //
    // Called when reporting the PDO missing to PnP manager in response to 
    // IRP_MN_QUERY_DEVICE_RELATIONS for Bus Relations. 
    //
    PFN_WDF_DEVICE_REPORTED_MISSING EvtDeviceReportedMissing;

} WDF_PDO_EVENT_CALLBACKS, *PWDF_PDO_EVENT_CALLBACKS;

VOID
FORCEINLINE
WDF_PDO_EVENT_CALLBACKS_INIT(
    _Out_ PWDF_PDO_EVENT_CALLBACKS Callbacks
    )
{
    RtlZeroMemory(Callbacks, sizeof(WDF_PDO_EVENT_CALLBACKS));
    Callbacks->Size = sizeof(WDF_PDO_EVENT_CALLBACKS);
}
WDF_EXTERN_C_END

