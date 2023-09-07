/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Wdfio.h

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

// 
// Types
// 

// 
// This defines the dispatch type of the queue. This controls how
// the queue raises I/O events to the driver through the registered
// callbacks.
// 
// Sequential allows the driver to have the queue automatically dispatch
// one request at a time, and will hold requests until a current request
// is completed.
// 
// Parallel has the queue dispatch requests to the driver as they arrive
// at the queue, and the queue is in a processing state. The driver can
// look at the requests in real time, and decide to service them, forward them
// to another queue, pend them, or return a status to have the queue held
// due to a hardware or other resource limit.
// 
// Manual allows the driver to create multiple queues for requests, and control
// when it wants to retrieve requests from the queue by calling the queues
// WdfIoQueueRetrieveNextRequest() API.
// 

typedef enum _WDF_IO_QUEUE_DISPATCH_TYPE {
    WdfIoQueueDispatchInvalid = 0,
    WdfIoQueueDispatchSequential,
    WdfIoQueueDispatchParallel,
    WdfIoQueueDispatchManual,
    WdfIoQueueDispatchMax,
} WDF_IO_QUEUE_DISPATCH_TYPE;


// 
// This defines the status of the queue.
// 
// WdfIoQueueAcceptRequests   - If TRUE, Queue will accept requests from WDM
// dispatch through WdfDeviceConfigureRequestDispatching,
// or from WdfRequestForwardToIoQueue.
// 
// If FALSE, Queue will complete requests with
// STATUS_CANCELLED from WdfDeviceConfigureRequestDispatching,
// and fail WdfRequestForwardToIoQueue with
// STATUS_WDF_BUSY.
// 
// 
// WdfIoQueueDispatchRequests - If TRUE, and the Queue is configured for
// automatic dispatch as either
// WdfIoQueueDispatchSequential,
// or WdfIoQueueDispatchParallel, the Queue will
// present the requests to the driver according
// to the drivers configuration.
// 
// If FALSE, requests are not automatically
// presented to the device driver.
// 
// This has no effect on the drivers ability to
// retrieve requests with WdfIoQueueRetrieveNextRequest.
// 
// WdfIoQueueNoRequests       - If TRUE, the Queue has no requests to present
// or return to the device driver.
// 
// WdfIoQueueDriverNoRequests - If TRUE, the device driver is not operating
// on any requests retrieved from or presented
// by this Queue.
// 
// WdfIoQueuePnpHeld          - The Framework PnP stage has requested that
// the device driver stop receiving new requests.
// 
// Automatic request dispatch stops, and
// WdfIoQueueRetrieveNextRequest returns STATUS_WDF_BUSY.
// 

typedef enum _WDF_IO_QUEUE_STATE {
    WdfIoQueueAcceptRequests = 0x01,
    WdfIoQueueDispatchRequests = 0x02,
    WdfIoQueueNoRequests = 0x04,
    WdfIoQueueDriverNoRequests = 0x08,
    WdfIoQueuePnpHeld = 0x10,
} WDF_IO_QUEUE_STATE;



//
// These macros represent some common Queue states
//

//
// A Queue is idle if it has no requests, and the driver
// is not operating on any.
//

BOOLEAN
FORCEINLINE
WDF_IO_QUEUE_IDLE(
    _In_ WDF_IO_QUEUE_STATE State
    )
{
    return ((State & WdfIoQueueNoRequests) &&
            (State & WdfIoQueueDriverNoRequests)) ? TRUE: FALSE;
}

//
// A Queue is ready if it can accept and dispatch requests and
// queue is not held by PNP
//
BOOLEAN
FORCEINLINE
WDF_IO_QUEUE_READY(
    _In_ WDF_IO_QUEUE_STATE State
    )
{
    return ((State & WdfIoQueueDispatchRequests) &&
        (State & WdfIoQueueAcceptRequests) &&
        ((State & WdfIoQueuePnpHeld)==0)) ? TRUE: FALSE;
}

//
// A Queue is stopped if it can accept new requests, but
// is not automatically delivering them to the device driver,
// and the queue is idle.
//
BOOLEAN
FORCEINLINE
WDF_IO_QUEUE_STOPPED(
    _In_ WDF_IO_QUEUE_STATE State
    )
{
    return (((State & WdfIoQueueDispatchRequests) == 0) &&
        (State & WdfIoQueueAcceptRequests) &&
        (State & WdfIoQueueDriverNoRequests)) ? TRUE: FALSE;

}

//
// A Queue is drained if it can not accept new requests but
// can dispatch existing requests, and there are no requests
// either in the Queue or the device driver.
//

BOOLEAN
FORCEINLINE
WDF_IO_QUEUE_DRAINED(
    _In_ WDF_IO_QUEUE_STATE State
    )
{
    return ( ((State & WdfIoQueueAcceptRequests)==0) &&
          (State & WdfIoQueueDispatchRequests) &&
          (State & WdfIoQueueNoRequests)  &&
          (State & WdfIoQueueDriverNoRequests) ) ? TRUE: FALSE;

}

//
// A Queue is purged if it can not accept new requests
// and there are no requests either in the Queue or
// the device driver.
//
BOOLEAN
FORCEINLINE
WDF_IO_QUEUE_PURGED(
    _In_ WDF_IO_QUEUE_STATE State
    )
{
    return ( ((State & WdfIoQueueAcceptRequests)==0) &&
          (State & WdfIoQueueNoRequests) &&
          (State & WdfIoQueueDriverNoRequests) ) ? TRUE: FALSE;

}

//
// Event callback definitions
//

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_DEFAULT)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_DEFAULT(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_IO_QUEUE_IO_DEFAULT *PFN_WDF_IO_QUEUE_IO_DEFAULT;


typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_STOP)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_STOP(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request,
    _In_
    ULONG ActionFlags
    );

typedef EVT_WDF_IO_QUEUE_IO_STOP *PFN_WDF_IO_QUEUE_IO_STOP;

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_RESUME)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_RESUME(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_IO_QUEUE_IO_RESUME *PFN_WDF_IO_QUEUE_IO_RESUME;

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_READ)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_READ(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request,
    _In_
    size_t Length
    );

typedef EVT_WDF_IO_QUEUE_IO_READ *PFN_WDF_IO_QUEUE_IO_READ;

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_WRITE)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_WRITE(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request,
    _In_
    size_t Length
    );

typedef EVT_WDF_IO_QUEUE_IO_WRITE *PFN_WDF_IO_QUEUE_IO_WRITE;

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request,
    _In_
    size_t OutputBufferLength,
    _In_
    size_t InputBufferLength,
    _In_
    ULONG IoControlCode
    );

typedef EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL *PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL;

typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request,
    _In_
    size_t OutputBufferLength,
    _In_
    size_t InputBufferLength,
    _In_
    ULONG IoControlCode
    );

typedef EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL *PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL;


typedef
_Function_class_(EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE *PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE;


typedef
_Function_class_(EVT_WDF_IO_QUEUE_STATE)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
EVT_WDF_IO_QUEUE_STATE(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFCONTEXT Context
    );

typedef EVT_WDF_IO_QUEUE_STATE *PFN_WDF_IO_QUEUE_STATE;

//
// This is the structure used to configure an IoQueue and
// register callback events to it.
//

typedef struct _WDF_IO_QUEUE_CONFIG {

    ULONG                                       Size;

    WDF_IO_QUEUE_DISPATCH_TYPE                  DispatchType;

    WDF_TRI_STATE                               PowerManaged;

    BOOLEAN                                     AllowZeroLengthRequests;

    BOOLEAN                                     DefaultQueue;

    PFN_WDF_IO_QUEUE_IO_DEFAULT                 EvtIoDefault;

    PFN_WDF_IO_QUEUE_IO_READ                    EvtIoRead;

    PFN_WDF_IO_QUEUE_IO_WRITE                   EvtIoWrite;

    PFN_WDF_IO_QUEUE_IO_DEVICE_CONTROL          EvtIoDeviceControl;

    PFN_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoInternalDeviceControl;

    PFN_WDF_IO_QUEUE_IO_STOP                    EvtIoStop;

    PFN_WDF_IO_QUEUE_IO_RESUME                  EvtIoResume;

    PFN_WDF_IO_QUEUE_IO_CANCELED_ON_QUEUE       EvtIoCanceledOnQueue;

    union {
        struct {
            ULONG NumberOfPresentedRequests;
        } Parallel;
    } Settings;

    WDFDRIVER                                   Driver;
} WDF_IO_QUEUE_CONFIG, *PWDF_IO_QUEUE_CONFIG;

VOID
FORCEINLINE
WDF_IO_QUEUE_CONFIG_INIT(
    _Out_ PWDF_IO_QUEUE_CONFIG      Config,
    _In_ WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    )
{
    RtlZeroMemory(Config, sizeof(WDF_IO_QUEUE_CONFIG));

    Config->Size = sizeof(WDF_IO_QUEUE_CONFIG);
    Config->PowerManaged = WdfUseDefault;
    Config->DispatchType = DispatchType;
    if (Config->DispatchType == WdfIoQueueDispatchParallel) {
        Config->Settings.Parallel.NumberOfPresentedRequests = (ULONG)-1;    
    }
}

VOID
FORCEINLINE
WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
    _Out_ PWDF_IO_QUEUE_CONFIG      Config,
    _In_ WDF_IO_QUEUE_DISPATCH_TYPE DispatchType
    )
{
    RtlZeroMemory(Config, sizeof(WDF_IO_QUEUE_CONFIG));

    Config->Size = sizeof(WDF_IO_QUEUE_CONFIG);
    Config->PowerManaged = WdfUseDefault;
    Config->DefaultQueue = TRUE;
    Config->DispatchType = DispatchType;
    if (Config->DispatchType == WdfIoQueueDispatchParallel) {
        Config->Settings.Parallel.NumberOfPresentedRequests = (ULONG)-1;    
    }    
}

typedef enum _WDF_IO_FORWARD_PROGRESS_ACTION {
    WdfIoForwardProgressActionInvalid = 0x0,
    WdfIoForwardProgressActionFailRequest,
    WdfIoForwardProgressActionUseReservedRequest
} WDF_IO_FORWARD_PROGRESS_ACTION;

typedef enum _WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY  {
  WdfIoForwardProgressInvalidPolicy =0x0,
  WdfIoForwardProgressReservedPolicyAlwaysUseReservedRequest,
  WdfIoForwardProgressReservedPolicyUseExamine,
  WdfIoForwardProgressReservedPolicyPagingIO
} WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY;

typedef
_Function_class_(EVT_WDF_IO_ALLOCATE_RESOURCES_FOR_RESERVED_REQUEST)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
EVT_WDF_IO_ALLOCATE_RESOURCES_FOR_RESERVED_REQUEST(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_IO_ALLOCATE_RESOURCES_FOR_RESERVED_REQUEST *PFN_WDF_IO_ALLOCATE_RESOURCES_FOR_RESERVED_REQUEST;

typedef
_Function_class_(EVT_WDF_IO_ALLOCATE_REQUEST_RESOURCES)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
EVT_WDF_IO_ALLOCATE_REQUEST_RESOURCES(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST Request
    );

typedef EVT_WDF_IO_ALLOCATE_REQUEST_RESOURCES *PFN_WDF_IO_ALLOCATE_REQUEST_RESOURCES;

typedef
_Function_class_(EVT_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS)
_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDF_IO_FORWARD_PROGRESS_ACTION
EVT_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS(
    _In_
    WDFQUEUE Queue,
    _In_
    PIRP Irp
    );

typedef EVT_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS *PFN_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS;

typedef  struct _WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS {
    union {

        struct {
          PFN_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS     EvtIoWdmIrpForForwardProgress;
        } ExaminePolicy;

    } Policy;
} WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS;

typedef struct _WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY {
    ULONG  Size;

    ULONG TotalForwardProgressRequests;

    //
    // Specify the type of the policy here.
    //
    WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY ForwardProgressReservedPolicy;
    
    //
    // Structure which contains the policy specific fields
    //
    WDF_IO_FORWARD_PROGRESS_RESERVED_POLICY_SETTINGS ForwardProgressReservePolicySettings;

    //
    // Callback for reserved request given at initialization time
    //
    PFN_WDF_IO_ALLOCATE_RESOURCES_FOR_RESERVED_REQUEST EvtIoAllocateResourcesForReservedRequest;

    //
    // Callback for reserved request given at run time
    //
    PFN_WDF_IO_ALLOCATE_REQUEST_RESOURCES  EvtIoAllocateRequestResources;       

}  WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY, *PWDF_IO_QUEUE_FORWARD_PROGRESS_POLICY;

VOID
FORCEINLINE
WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY_DEFAULT_INIT(
    _Out_ PWDF_IO_QUEUE_FORWARD_PROGRESS_POLICY Policy,
    _In_ ULONG TotalForwardProgressRequests
    )
{
    RtlZeroMemory(Policy, sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY));

    Policy->Size = sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY);
    Policy->TotalForwardProgressRequests = TotalForwardProgressRequests;
    Policy->ForwardProgressReservedPolicy = WdfIoForwardProgressReservedPolicyAlwaysUseReservedRequest;
}


VOID
FORCEINLINE
WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY_EXAMINE_INIT(
    _Out_ PWDF_IO_QUEUE_FORWARD_PROGRESS_POLICY      Policy,
    _In_ ULONG TotalForwardProgressRequests,
    _In_ PFN_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS EvtIoWdmIrpForForwardProgress
    )
{
    RtlZeroMemory(Policy, sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY));

    Policy->Size = sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY);
    Policy->TotalForwardProgressRequests = TotalForwardProgressRequests;
    Policy->ForwardProgressReservedPolicy =  WdfIoForwardProgressReservedPolicyUseExamine;
    Policy->ForwardProgressReservePolicySettings.Policy.ExaminePolicy.EvtIoWdmIrpForForwardProgress =
            EvtIoWdmIrpForForwardProgress;
}

VOID
FORCEINLINE
WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY_PAGINGIO_INIT(
    _Out_ PWDF_IO_QUEUE_FORWARD_PROGRESS_POLICY      Policy,
    _In_ ULONG TotalForwardProgressRequests
    )
{
    RtlZeroMemory(Policy, sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY));

    Policy->Size = sizeof(WDF_IO_QUEUE_FORWARD_PROGRESS_POLICY);
    Policy->TotalForwardProgressRequests = TotalForwardProgressRequests;
    Policy->ForwardProgressReservedPolicy = WdfIoForwardProgressReservedPolicyPagingIO;
}


//
// WDF Function: WdfIoQueueCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUECREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_IO_QUEUE_CONFIG Config,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    _Out_opt_
    WDFQUEUE* Queue
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueCreate(
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_IO_QUEUE_CONFIG Config,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES QueueAttributes,
    _Out_opt_
    WDFQUEUE* Queue
    )
{
    return ((PFN_WDFIOQUEUECREATE) WdfFunctions[WdfIoQueueCreateTableIndex])(WdfDriverGlobals, Device, Config, QueueAttributes, Queue);
}


//
// WDF Function: WdfIoQueueGetState
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDF_IO_QUEUE_STATE
(*PFN_WDFIOQUEUEGETSTATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _Out_opt_
    PULONG QueueRequests,
    _Out_opt_
    PULONG DriverRequests
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDF_IO_QUEUE_STATE
FORCEINLINE
WdfIoQueueGetState(
    _In_
    WDFQUEUE Queue,
    _Out_opt_
    PULONG QueueRequests,
    _Out_opt_
    PULONG DriverRequests
    )
{
    return ((PFN_WDFIOQUEUEGETSTATE) WdfFunctions[WdfIoQueueGetStateTableIndex])(WdfDriverGlobals, Queue, QueueRequests, DriverRequests);
}


//
// WDF Function: WdfIoQueueStart
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUESTART)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfIoQueueStart(
    _In_
    WDFQUEUE Queue
    )
{
    ((PFN_WDFIOQUEUESTART) WdfFunctions[WdfIoQueueStartTableIndex])(WdfDriverGlobals, Queue);
}


//
// WDF Function: WdfIoQueueStop
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUESTOP)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopComplete,
    _When_(StopComplete != 0, _In_)
    _When_(StopComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfIoQueueStop(
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopComplete,
    _When_(StopComplete != 0, _In_)
    _When_(StopComplete == 0, _In_opt_)
    WDFCONTEXT Context
    )
{
    ((PFN_WDFIOQUEUESTOP) WdfFunctions[WdfIoQueueStopTableIndex])(WdfDriverGlobals, Queue, StopComplete, Context);
}


//
// WDF Function: WdfIoQueueStopSynchronously
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUESTOPSYNCHRONOUSLY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfIoQueueStopSynchronously(
    _In_
    WDFQUEUE Queue
    )
{
    ((PFN_WDFIOQUEUESTOPSYNCHRONOUSLY) WdfFunctions[WdfIoQueueStopSynchronouslyTableIndex])(WdfDriverGlobals, Queue);
}


//
// WDF Function: WdfIoQueueGetDevice
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
(*PFN_WDFIOQUEUEGETDEVICE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFDEVICE
FORCEINLINE
WdfIoQueueGetDevice(
    _In_
    WDFQUEUE Queue
    )
{
    return ((PFN_WDFIOQUEUEGETDEVICE) WdfFunctions[WdfIoQueueGetDeviceTableIndex])(WdfDriverGlobals, Queue);
}


//
// WDF Function: WdfIoQueueRetrieveNextRequest
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUERETRIEVENEXTREQUEST)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueRetrieveNextRequest(
    _In_
    WDFQUEUE Queue,
    _Out_
    WDFREQUEST* OutRequest
    )
{
    return ((PFN_WDFIOQUEUERETRIEVENEXTREQUEST) WdfFunctions[WdfIoQueueRetrieveNextRequestTableIndex])(WdfDriverGlobals, Queue, OutRequest);
}


//
// WDF Function: WdfIoQueueRetrieveRequestByFileObject
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUERETRIEVEREQUESTBYFILEOBJECT)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_
    WDFFILEOBJECT FileObject,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueRetrieveRequestByFileObject(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFFILEOBJECT FileObject,
    _Out_
    WDFREQUEST* OutRequest
    )
{
    return ((PFN_WDFIOQUEUERETRIEVEREQUESTBYFILEOBJECT) WdfFunctions[WdfIoQueueRetrieveRequestByFileObjectTableIndex])(WdfDriverGlobals, Queue, FileObject, OutRequest);
}


//
// WDF Function: WdfIoQueueFindRequest
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUEFINDREQUEST)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_opt_
    WDFREQUEST FoundRequest,
    _In_opt_
    WDFFILEOBJECT FileObject,
    _Inout_opt_
    PWDF_REQUEST_PARAMETERS Parameters,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueFindRequest(
    _In_
    WDFQUEUE Queue,
    _In_opt_
    WDFREQUEST FoundRequest,
    _In_opt_
    WDFFILEOBJECT FileObject,
    _Inout_opt_
    PWDF_REQUEST_PARAMETERS Parameters,
    _Out_
    WDFREQUEST* OutRequest
    )
{
    return ((PFN_WDFIOQUEUEFINDREQUEST) WdfFunctions[WdfIoQueueFindRequestTableIndex])(WdfDriverGlobals, Queue, FoundRequest, FileObject, Parameters, OutRequest);
}


//
// WDF Function: WdfIoQueueRetrieveFoundRequest
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUERETRIEVEFOUNDREQUEST)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST FoundRequest,
    _Out_
    WDFREQUEST* OutRequest
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueRetrieveFoundRequest(
    _In_
    WDFQUEUE Queue,
    _In_
    WDFREQUEST FoundRequest,
    _Out_
    WDFREQUEST* OutRequest
    )
{
    return ((PFN_WDFIOQUEUERETRIEVEFOUNDREQUEST) WdfFunctions[WdfIoQueueRetrieveFoundRequestTableIndex])(WdfDriverGlobals, Queue, FoundRequest, OutRequest);
}


//
// WDF Function: WdfIoQueueDrainSynchronously
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUEDRAINSYNCHRONOUSLY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfIoQueueDrainSynchronously(
    _In_
    WDFQUEUE Queue
    )
{
    ((PFN_WDFIOQUEUEDRAINSYNCHRONOUSLY) WdfFunctions[WdfIoQueueDrainSynchronouslyTableIndex])(WdfDriverGlobals, Queue);
}


//
// WDF Function: WdfIoQueueDrain
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUEDRAIN)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE DrainComplete,
    _When_(DrainComplete != 0, _In_)
    _When_(DrainComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfIoQueueDrain(
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE DrainComplete,
    _When_(DrainComplete != 0, _In_)
    _When_(DrainComplete == 0, _In_opt_)
    WDFCONTEXT Context
    )
{
    ((PFN_WDFIOQUEUEDRAIN) WdfFunctions[WdfIoQueueDrainTableIndex])(WdfDriverGlobals, Queue, DrainComplete, Context);
}


//
// WDF Function: WdfIoQueuePurgeSynchronously
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUEPURGESYNCHRONOUSLY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfIoQueuePurgeSynchronously(
    _In_
    WDFQUEUE Queue
    )
{
    ((PFN_WDFIOQUEUEPURGESYNCHRONOUSLY) WdfFunctions[WdfIoQueuePurgeSynchronouslyTableIndex])(WdfDriverGlobals, Queue);
}


//
// WDF Function: WdfIoQueuePurge
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUEPURGE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE PurgeComplete,
    _When_(PurgeComplete != 0, _In_)
    _When_(PurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfIoQueuePurge(
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE PurgeComplete,
    _When_(PurgeComplete != 0, _In_)
    _When_(PurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    )
{
    ((PFN_WDFIOQUEUEPURGE) WdfFunctions[WdfIoQueuePurgeTableIndex])(WdfDriverGlobals, Queue, PurgeComplete, Context);
}


//
// WDF Function: WdfIoQueueReadyNotify
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFIOQUEUEREADYNOTIFY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _In_opt_
    PFN_WDF_IO_QUEUE_STATE QueueReady,
    _In_opt_
    WDFCONTEXT Context
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfIoQueueReadyNotify(
    _In_
    WDFQUEUE Queue,
    _In_opt_
    PFN_WDF_IO_QUEUE_STATE QueueReady,
    _In_opt_
    WDFCONTEXT Context
    )
{
    return ((PFN_WDFIOQUEUEREADYNOTIFY) WdfFunctions[WdfIoQueueReadyNotifyTableIndex])(WdfDriverGlobals, Queue, QueueReady, Context);
}


//
// WDF Function: WdfIoQueueStopAndPurge
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUESTOPANDPURGE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopAndPurgeComplete,
    _When_(StopAndPurgeComplete != 0, _In_)
    _When_(StopAndPurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
VOID
FORCEINLINE
WdfIoQueueStopAndPurge(
    _In_
    WDFQUEUE Queue,
    _When_(Context != 0, _In_)
    _When_(Context == 0, _In_opt_)
    PFN_WDF_IO_QUEUE_STATE StopAndPurgeComplete,
    _When_(StopAndPurgeComplete != 0, _In_)
    _When_(StopAndPurgeComplete == 0, _In_opt_)
    WDFCONTEXT Context
    )
{
    ((PFN_WDFIOQUEUESTOPANDPURGE) WdfFunctions[WdfIoQueueStopAndPurgeTableIndex])(WdfDriverGlobals, Queue, StopAndPurgeComplete, Context);
}


//
// WDF Function: WdfIoQueueStopAndPurgeSynchronously
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFIOQUEUESTOPANDPURGESYNCHRONOUSLY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFQUEUE Queue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfIoQueueStopAndPurgeSynchronously(
    _In_
    WDFQUEUE Queue
    )
{
    ((PFN_WDFIOQUEUESTOPANDPURGESYNCHRONOUSLY) WdfFunctions[WdfIoQueueStopAndPurgeSynchronouslyTableIndex])(WdfDriverGlobals, Queue);
}

WDF_EXTERN_C_END

