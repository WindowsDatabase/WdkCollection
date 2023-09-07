/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    UcxRootHub.h

Abstract:

    UCX Root Hub object types and methods.

Environment:

    Kernel-mode only.

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _UCXROOTHUB_H_
#define _UCXROOTHUB_H_

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



typedef enum _CONTROLLER_TYPE {
    ControllerTypeXhci = 0,
    ControllerTypeSoftXhci,
} CONTROLLER_TYPE;

typedef enum _TRISTATE {
    TriStateUnknown = 'u',
    TriStateFalse = 'f',
    TriStateTrue = 't',
} TRISTATE;



typedef union _CONTROLLER_USB_20_HARDWARE_LPM_FLAGS {
    UCHAR   AsUchar;
    struct {
        UCHAR   L1CapabilitySupported:1;
        UCHAR   BeslLpmCapabilitySupported:1;
#ifdef __cplusplus
    } Flags;
#else
    };
#endif
} CONTROLLER_USB_20_HARDWARE_LPM_FLAGS, *PCONTROLLER_USB_20_HARDWARE_LPM_FLAGS;

typedef struct _ROOTHUB_INFO {
    ULONG           Size;
    CONTROLLER_TYPE ControllerType;
    USHORT          NumberOf20Ports;
    USHORT          NumberOf30Ports;
    USHORT          MaxU1ExitLatency;
    USHORT          MaxU2ExitLatency;
} ROOTHUB_INFO, *PROOTHUB_INFO;

typedef struct _ROOTHUB_20PORT_INFO {
    USHORT                                  PortNumber;
    UCHAR                                   MinorRevision;
    UCHAR                                   HubDepth;
    TRISTATE                                Removable;
    TRISTATE                                IntegratedHubImplemented;
    TRISTATE                                DebugCapable;
    CONTROLLER_USB_20_HARDWARE_LPM_FLAGS    ControllerUsb20HardwareLpmFlags;
} ROOTHUB_20PORT_INFO, *PROOTHUB_20PORT_INFO;

typedef struct _ROOTHUB_20PORTS_INFO {
    ULONG                   Size;
    USHORT                  NumberOfPorts;
    USHORT                  PortInfoSize;
    PROOTHUB_20PORT_INFO *  PortInfoArray;
} ROOTHUB_20PORTS_INFO, *PROOTHUB_20PORTS_INFO;

typedef struct _ROOTHUB_30PORT_INFO {
    USHORT                                       PortNumber;
    UCHAR                                        MinorRevision;
    UCHAR                                        HubDepth;
    TRISTATE                                     Removable;
    TRISTATE                                     DebugCapable;
} ROOTHUB_30PORT_INFO, *PROOTHUB_30PORT_INFO;

//
// Extended 3.0 port info additionally contains speed information.
//
typedef struct _ROOTHUB_30PORT_INFO_EX {
#ifdef __cplusplus
    ROOTHUB_30PORT_INFO                          Info;
#else
    ROOTHUB_30PORT_INFO;
#endif
    USHORT                                       MaxSpeedsCount;
    USHORT                                       SpeedsCount;
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  Speeds;
} ROOTHUB_30PORT_INFO_EX, *PROOTHUB_30PORT_INFO_EX;

//
// Maximum number of speeds in the table is 32; also allocate 16 IDs for default speeds.
//
#define MAX_SPEEDS_COUNT                         (32 + 16)

typedef struct _ROOTHUB_30PORTS_INFO {
    ULONG                   Size;
    USHORT                  NumberOfPorts;
    USHORT                  PortInfoSize;
    PROOTHUB_30PORT_INFO *  PortInfoArray;
} ROOTHUB_30PORTS_INFO, *PROOTHUB_30PORTS_INFO;

typedef union _PARENT_HUB_FLAGS {
    ULONG   AsUlong32;
    struct {
        ULONG   DisableLpmForAllDownstreamDevices:1;
        ULONG   HubIsHighSpeedCapable:1;
        ULONG   DisableUpdateMaxExitLatency:1;
        ULONG   DisableU1:1;
#ifdef __cplusplus
    } Flags;
#else
    };
#endif
} PARENT_HUB_FLAGS, *PPARENT_HUB_FLAGS;

C_ASSERT(sizeof(PARENT_HUB_FLAGS) == sizeof(ULONG));

typedef struct _HUB_INFO_FROM_PARENT {
    PDEVICE_OBJECT          IoTarget;
    USB_DEVICE_DESCRIPTOR   DeviceDescriptor;
    USHORT                  U1ExitLatency;
    USHORT                  U2ExitLatency;
    USHORT                  ExitLatencyOfSlowestLinkForU1;
    UCHAR                   DepthOfSlowestLinkForU1;
    USHORT                  ExitLatencyOfSlowestLinkForU2;
    UCHAR                   DepthOfSlowestLinkForU2;
    USHORT                  HostInitiatedU1ExitLatency;
    USHORT                  HostInitiatedU2ExitLatency;
    UCHAR                   TotalHubDepth;
    USHORT                  TotalTPPropogationDelay;
    PARENT_HUB_FLAGS        HubFlags;
    PUSB_DEVICE_CAPABILITY_SUPERSPEEDPLUS_SPEED  SublinkSpeedAttr;
    ULONG                   SublinkSpeedAttrCount;
} HUB_INFO_FROM_PARENT, *PHUB_INFO_FROM_PARENT;

typedef
_Function_class_(EVT_UCX_ROOTHUB_INTERRUPT_TX)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EVT_UCX_ROOTHUB_INTERRUPT_TX(
    __in
    UCXROOTHUB  UcxRootHub,
    __in
    WDFREQUEST  Request
);

typedef EVT_UCX_ROOTHUB_INTERRUPT_TX *PFN_UCX_ROOTHUB_INTERRUPT_TX;

typedef
_Function_class_(EVT_UCX_ROOTHUB_CONTROL_URB)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EVT_UCX_ROOTHUB_CONTROL_URB(
    __in
    UCXROOTHUB  UcxRootHub,
    __in
    WDFREQUEST  Request
);

typedef EVT_UCX_ROOTHUB_CONTROL_URB *PFN_UCX_ROOTHUB_CONTROL_URB;

typedef
_Function_class_(EVT_UCX_ROOTHUB_GET_INFO)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EVT_UCX_ROOTHUB_GET_INFO(
    __in
    UCXROOTHUB  UcxRootHub,
    __in
    WDFREQUEST  Request
);

typedef EVT_UCX_ROOTHUB_GET_INFO *PFN_UCX_ROOTHUB_GET_INFO;

typedef
_Function_class_(EVT_UCX_ROOTHUB_GET_20PORT_INFO)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EVT_UCX_ROOTHUB_GET_20PORT_INFO(
    __in
    UCXROOTHUB  UcxRootHub,
    __in
    WDFREQUEST  Request
);

typedef EVT_UCX_ROOTHUB_GET_20PORT_INFO *PFN_UCX_ROOTHUB_GET_20PORT_INFO;

typedef
_Function_class_(EVT_UCX_ROOTHUB_GET_30PORT_INFO)
_IRQL_requires_same_
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EVT_UCX_ROOTHUB_GET_30PORT_INFO(
    __in
    UCXROOTHUB  UcxRootHub,
    __in
    WDFREQUEST  Request
);

typedef EVT_UCX_ROOTHUB_GET_30PORT_INFO *PFN_UCX_ROOTHUB_GET_30PORT_INFO;

typedef struct _UCX_ROOTHUB_CONFIG {
    //
    // Size in bytes of this structure
    //
    ULONG                                       Size;
    ULONG                                       NumberOfPresentedControlUrbCallbacks;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubClearHubFeature;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubClearPortFeature;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubGetHubStatus;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubGetPortStatus;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubSetHubFeature;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubSetPortFeature;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubGetPortErrorCount;
    PFN_UCX_ROOTHUB_CONTROL_URB                EvtRootHubControlUrb;
    PFN_UCX_ROOTHUB_INTERRUPT_TX               EvtRootHubInterruptTx;
    PFN_UCX_ROOTHUB_GET_INFO                   EvtRootHubGetInfo;
    PFN_UCX_ROOTHUB_GET_20PORT_INFO            EvtRootHubGet20PortInfo;
    PFN_UCX_ROOTHUB_GET_30PORT_INFO            EvtRootHubGet30PortInfo;
    WDF_OBJECT_ATTRIBUTES                       WdfRequestAttributes;

} UCX_ROOTHUB_CONFIG, *PUCX_ROOTHUB_CONFIG;

__drv_at(Config->Size, __drv_out (__range(!=,0)))
__drv_at(Config->EvtRootHubControlUrb, __drv_out  (__notnull))
__drv_at(Config->EvtRootHubClearHubFeature, __drv_out (__null))
__drv_at(Config->EvtRootHubClearPortFeature, __drv_out (__null))
__drv_at(Config->EvtRootHubGetHubStatus, __drv_out (__null))
__drv_at(Config->EvtRootHubGetPortStatus, __drv_out (__null))
__drv_at(Config->EvtRootHubSetHubFeature, __drv_out (__null))
__drv_at(Config->EvtRootHubSetPortFeature, __drv_out (__null))
__drv_at(Config->EvtRootHubGetPortErrorCount, __drv_out (__null))
__drv_at(Config->EvtRootHubInterruptTx, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGetInfo, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGet20PortInfo, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGet30PortInfo, __drv_out (__notnull))
VOID
FORCEINLINE
UCX_ROOTHUB_CONFIG_INIT_WITH_CONTROL_URB_HANDLER(
    __out
        PUCX_ROOTHUB_CONFIG                  Config,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB         EvtRootHubControlUrb,
    __in
        PFN_UCX_ROOTHUB_INTERRUPT_TX        EvtRootHubInterruptTx,
    __in
        PFN_UCX_ROOTHUB_GET_INFO            EvtRootHubGetInfo,
    __in
        PFN_UCX_ROOTHUB_GET_20PORT_INFO     EvtRootHubGet20PortInfo,
    __in
        PFN_UCX_ROOTHUB_GET_30PORT_INFO     EvtRootHubGet30PortInfo
    )
{
    RtlZeroMemory(Config, sizeof(UCX_ROOTHUB_CONFIG));
    Config->Size = sizeof(UCX_ROOTHUB_CONFIG);
    Config->NumberOfPresentedControlUrbCallbacks = 1;
    Config->EvtRootHubControlUrb = EvtRootHubControlUrb;
    Config->EvtRootHubInterruptTx = EvtRootHubInterruptTx;
    Config->EvtRootHubGetInfo = EvtRootHubGetInfo;
    Config->EvtRootHubGet20PortInfo = EvtRootHubGet20PortInfo;
    Config->EvtRootHubGet30PortInfo = EvtRootHubGet30PortInfo;

    WDF_OBJECT_ATTRIBUTES_INIT(&Config->WdfRequestAttributes);

    __assume(Config->EvtRootHubClearHubFeature == NULL);
    __assume(Config->EvtRootHubClearPortFeature == NULL);
    __assume(Config->EvtRootHubGetHubStatus == NULL);
    __assume(Config->EvtRootHubGetPortStatus == NULL);
    __assume(Config->EvtRootHubSetHubFeature == NULL);
    __assume(Config->EvtRootHubSetPortFeature == NULL);
    __assume(Config->EvtRootHubGetPortErrorCount == NULL);
}

__drv_at(Config->Size, __drv_out (__range(!=,0)))
__drv_at(Config->EvtRootHubControlUrb, __drv_out  (__null))
__drv_at(Config->EvtRootHubClearHubFeature, __drv_out (__notnull))
__drv_at(Config->EvtRootHubClearPortFeature, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGetHubStatus, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGetPortStatus, __drv_out (__notnull))
__drv_at(Config->EvtRootHubSetHubFeature, __drv_out (__notnull))
__drv_at(Config->EvtRootHubSetPortFeature, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGetPortErrorCount, __drv_out (__notnull))
__drv_at(Config->EvtRootHubInterruptTx, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGetInfo, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGet20PortInfo, __drv_out (__notnull))
__drv_at(Config->EvtRootHubGet30PortInfo, __drv_out (__notnull))
VOID
FORCEINLINE
UCX_ROOTHUB_CONFIG_INIT(
    __out
        PUCX_ROOTHUB_CONFIG              Config,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubClearHubFeature,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubClearPortFeature,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubGetHubStatus,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubGetPortStatus,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubSetHubFeature,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubSetPortFeature,
    __in
        PFN_UCX_ROOTHUB_CONTROL_URB     EvtRootHubGetPortErrorCount,
    __in
        PFN_UCX_ROOTHUB_INTERRUPT_TX    EvtRootHubInterruptTx,
    __in
        PFN_UCX_ROOTHUB_GET_INFO        EvtRootHubGetInfo,
    __in
        PFN_UCX_ROOTHUB_GET_20PORT_INFO EvtRootHubGet20PortInfo,
    __in
        PFN_UCX_ROOTHUB_GET_30PORT_INFO EvtRootHubGet30PortInfo
    )
{
    RtlZeroMemory(Config, sizeof(UCX_ROOTHUB_CONFIG));
    Config->Size = sizeof(UCX_ROOTHUB_CONFIG);
    Config->NumberOfPresentedControlUrbCallbacks = 1;
    Config->EvtRootHubClearHubFeature = EvtRootHubClearHubFeature;
    Config->EvtRootHubClearPortFeature = EvtRootHubClearPortFeature;
    Config->EvtRootHubGetHubStatus = EvtRootHubGetHubStatus;
    Config->EvtRootHubGetPortStatus = EvtRootHubGetPortStatus;
    Config->EvtRootHubSetHubFeature = EvtRootHubSetHubFeature;
    Config->EvtRootHubSetPortFeature = EvtRootHubSetPortFeature;
    Config->EvtRootHubGetPortErrorCount = EvtRootHubGetPortErrorCount;
    Config->EvtRootHubInterruptTx = EvtRootHubInterruptTx;
    Config->EvtRootHubGetInfo = EvtRootHubGetInfo;
    Config->EvtRootHubGet20PortInfo = EvtRootHubGet20PortInfo;
    Config->EvtRootHubGet30PortInfo = EvtRootHubGet30PortInfo;

    WDF_OBJECT_ATTRIBUTES_INIT(&Config->WdfRequestAttributes);

    __assume(Config->EvtRootHubControlUrb == NULL);
}

//
// In an earlier version of this header, the function pointers for the callbacks used to be prefixed
// with "PEVT" instead of the standard "PFN". Typedef those old names so that existing clients that
// may be using the old names continue to compile with the new header.
//

typedef PFN_UCX_ROOTHUB_CONTROL_URB PEVT_UCX_ROOTHUB_CONTROL_URB;
typedef PFN_UCX_ROOTHUB_GET_20PORT_INFO PEVT_UCX_ROOTHUB_GET_20PORT_INFO;
typedef PFN_UCX_ROOTHUB_GET_30PORT_INFO PEVT_UCX_ROOTHUB_GET_30PORT_INFO;
typedef PFN_UCX_ROOTHUB_GET_INFO PEVT_UCX_ROOTHUB_GET_INFO;
typedef PFN_UCX_ROOTHUB_INTERRUPT_TX PEVT_UCX_ROOTHUB_INTERRUPT_TX;

//
// UCX Function: UcxRootHubCreate
//
typedef
_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_at(Config->Size, __range(!=,0))
__drv_when(NULL == Config->EvtRootHubControlUrb,
__drv_at(Config->EvtRootHubClearHubFeature, __notnull)
__drv_at(Config->EvtRootHubClearPortFeature, __notnull)
__drv_at(Config->EvtRootHubGetHubStatus, __notnull)
__drv_at(Config->EvtRootHubGetPortStatus, __notnull)
__drv_at(Config->EvtRootHubSetHubFeature, __notnull)
__drv_at(Config->EvtRootHubSetPortFeature, __notnull)
__drv_at(Config->EvtRootHubGetPortErrorCount, __notnull)
)
__drv_at(Config->EvtRootHubInterruptTx, __notnull)
__drv_at(Config->EvtRootHubGetInfo, __notnull)
__drv_at(Config->EvtRootHubGet20PortInfo, __notnull)
__drv_at(Config->EvtRootHubGet30PortInfo, __notnull)
WDFAPI
NTSTATUS
(*PFN_UCXROOTHUBCREATE)(
    _In_
    PUCX_DRIVER_GLOBALS DriverGlobals,
    __in
    UCXCONTROLLER Controller,
    __in
    PUCX_ROOTHUB_CONFIG Config,
    __in_opt
    PWDF_OBJECT_ATTRIBUTES Attributes,
    __out
    UCXROOTHUB* RootHub
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
__drv_at(Config->Size, __range(!=,0))
__drv_when(NULL == Config->EvtRootHubControlUrb,
__drv_at(Config->EvtRootHubClearHubFeature, __notnull)
__drv_at(Config->EvtRootHubClearPortFeature, __notnull)
__drv_at(Config->EvtRootHubGetHubStatus, __notnull)
__drv_at(Config->EvtRootHubGetPortStatus, __notnull)
__drv_at(Config->EvtRootHubSetHubFeature, __notnull)
__drv_at(Config->EvtRootHubSetPortFeature, __notnull)
__drv_at(Config->EvtRootHubGetPortErrorCount, __notnull)
)
__drv_at(Config->EvtRootHubInterruptTx, __notnull)
__drv_at(Config->EvtRootHubGetInfo, __notnull)
__drv_at(Config->EvtRootHubGet20PortInfo, __notnull)
__drv_at(Config->EvtRootHubGet30PortInfo, __notnull)
NTSTATUS
FORCEINLINE
UcxRootHubCreate(
    __in
    UCXCONTROLLER Controller,
    __in
    PUCX_ROOTHUB_CONFIG Config,
    __in_opt
    PWDF_OBJECT_ATTRIBUTES Attributes,
    __out
    UCXROOTHUB* RootHub
    )
{
    return ((PFN_UCXROOTHUBCREATE) UcxFunctions[UcxRootHubCreateTableIndex])(UcxDriverGlobals, Controller, Config, Attributes, RootHub);
}

//
// UCX Function: UcxRootHubPortChanged
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_UCXROOTHUBPORTCHANGED)(
    _In_
    PUCX_DRIVER_GLOBALS DriverGlobals,
    __in
    UCXROOTHUB UcxRootHub
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
FORCEINLINE
UcxRootHubPortChanged(
    __in
    UCXROOTHUB UcxRootHub
    )
{
    ((PFN_UCXROOTHUBPORTCHANGED) UcxFunctions[UcxRootHubPortChangedTableIndex])(UcxDriverGlobals, UcxRootHub);
}



WDF_EXTERN_C_END

#endif // _UCXROOTHUB_H_


