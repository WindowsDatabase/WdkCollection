/*  
	Copyright (c) Microsoft Corporation.  All rights reserved.
	Stub functions for DSM driver
*/

#include "sdv_wdm.h"

#ifndef _SDV_MPIO_H_
#define _SDV_MPIO_H_




#ifdef  DSM_FATAL_ERROR
#undef  DSM_FATAL_ERROR
#endif
#define DSM_FATAL_ERROR             0x20000000

#ifdef  DSM_MOVE_ADMIN_REQUEST
#undef  DSM_MOVE_ADMIN_REQUEST
#endif
#define DSM_MOVE_ADMIN_REQUEST      0x20000000

#ifdef  STATUS_INSUFFICIENT_RESOURCES
#undef  STATUS_INSUFFICIENT_RESOURCES
#endif
#define STATUS_INSUFFICIENT_RESOURCES    96   

/*
#ifdef  DSM_CNTRL_FLAGS_ALLOCATE
#undef  DSM_CNTRL_FLAGS_ALLOCATE
#endif
#define DSM_CNTRL_FLAGS_ALLOCATE    1   
*/

/*
#ifdef  DSM_RETRY_DONT_DECREMENT
#undef  DSM_RETRY_DONT_DECREMENT
#endif
#define DSM_RETRY_DONT_DECREMENT    8

#ifdef  DSM_ADMIN_FO
#undef  DSM_ADMIN_FO
#endif
#define DSM_ADMIN_FO                9

#ifdef  DSM_FATAL_ERROR_OEM_MASK
#undef  DSM_FATAL_ERROR_OEM_MASK
#endif
#define DSM_FATAL_ERROR_OEM_MASK    10

#ifdef  DSM_FATAL_ERROR_RESERVED
#undef  DSM_FATAL_ERROR_RESERVED
#endif
#define DSM_FATAL_ERROR_RESERVED    11*/



#ifdef DsmSendDeviceIoControlSynchronous
#undef DsmSendDeviceIoControlSynchronous
#endif
#define DsmSendDeviceIoControlSynchronous sdv_DsmSendDeviceIoControlSynchronous
VOID
DsmSendDeviceIoControlSynchronous(
    __in IN ULONG IoControlCode,
    __in IN PDEVICE_OBJECT TargetDeviceObject,
    __in_opt IN PVOID InputBuffer OPTIONAL,
    __inout_opt IN OUT PVOID OutputBuffer OPTIONAL,
    __in IN ULONG InputBufferLength,
    __in IN ULONG OutputBufferLength,
    __in IN BOOLEAN InternalDeviceIoControl,
    __out OUT PIO_STATUS_BLOCK IoStatus
    );

#ifdef DsmSendPassThroughDirect
#undef DsmSendPassThroughDirect
#endif
#define DsmSendPassThroughDirect sdv_DsmSendPassThroughDirect
NTSTATUS
DsmSendPassThroughDirect(
    __in IN PDEVICE_OBJECT DeviceObject,
    __in IN PSCSI_PASS_THROUGH_DIRECT ScsiPassThrough,
    __in IN ULONG InputBufferLength,
    __in IN ULONG OutputBufferLength
    );

#ifdef DsmGetScsiAddress
#undef DsmGetScsiAddress
#endif
#define DsmGetScsiAddress sdv_DsmGetScsiAddress
NTSTATUS
DsmGetScsiAddress(
    __in IN PDEVICE_OBJECT DeviceObject,
    __in __drv_in(__drv_aliasesMem) IN PSCSI_ADDRESS *ScsiAddress
    );

#ifdef DsmGetAssociatedDevice
#undef DsmGetAssociatedDevice
#endif
#define DsmGetAssociatedDevice sdv_DsmGetAssociatedDevice
PDSM_IDS
DsmGetAssociatedDevice(
    __in IN PVOID MPIOContext,
    __in IN PDEVICE_OBJECT PortFdo,
    __in IN UCHAR DeviceType
    );

#ifdef DsmReleaseQueue
#undef DsmReleaseQueue
#endif
#define DsmReleaseQueue sdv_DsmReleaseQueue
NTSTATUS
DsmReleaseQueue(
    __in IN PDEVICE_OBJECT TargetDevice
    );

#ifdef DsmSendTUR
#undef DsmSendTUR
#endif
#define DsmSendTUR sdv_DsmSendTUR
NTSTATUS
DsmSendTUR(
    __in IN PDEVICE_OBJECT TargetDevice
    );

#ifdef DsmSendRequest
#undef DsmSendRequest
#endif
#define DsmSendRequest sdv_DsmSendRequest
NTSTATUS
DsmSendRequest(
    __in IN PVOID MPIOContext,
    __in IN PDEVICE_OBJECT TargetDevice,
    __in IN PIRP Irp,
    __in IN PVOID DsmId
    );

#ifdef DsmCompleteRequest
#undef DsmCompleteRequest
#endif
#define DsmCompleteRequest sdv_DsmCompleteRequest
VOID
DsmCompleteRequest(
    __in IN PVOID MPIOContext,
    __in IN PVOID DsmId
    );

#ifdef DsmGetSystemWeight
#undef DsmGetSystemWeight
#endif
#define DsmGetSystemWeight sdv_DsmGetSystemWeight
ULONG
DsmGetSystemWeight(
    __in IN PVOID MPIOContext,
    __in IN PIRP Irp,
    __in IN PVOID PathId
    );
    
#ifdef DsmGetPDO
#undef DsmGetPDO
#endif
#define DsmGetPDO sdv_DsmGetPDO
PDEVICE_OBJECT
DsmGetPDO(
    __in IN PVOID MPIOContext,
    __in IN PDEVICE_OBJECT DeviceObject
    );

#ifdef DsmNotification
#undef DsmNotification
#endif
#define DsmNotification sdv_DsmNotification

VOID
DsmNotification(
     __in IN PVOID MPIOContext,
     __in IN DSM_NOTIFICATION_TYPE NotificationType,
     ...
     );

/*
#ifdef sdv_stub_DsmNotification_special

/*    VOID
    DsmNotification(
        __in IN PVOID MPIOContext,
        __in IN DSM_NOTIFICATION_TYPE NotificationType,
        __in IN PVOID DsmId
        );

    VOID
    DsmNotification(
        __in IN PVOID MPIOContext,
        __in IN DSM_NOTIFICATION_TYPE NotificationType,
        __in IN PVOID DsmId,
        __in IN BOOLEAN Reserved1,
        __inout IN OUT PNTSTATUS Status,
        __in IN LONG Reserved
        );
/*
    VOID
    DsmNotification(
        __in IN PVOID MPIOContext,
        __in IN DSM_NOTIFICATION_TYPE NotificationType,
        __in IN PVOID PathId,
        __in IN PVOID DsmId
        );

    VOID
    DsmNotification(
        __in IN PVOID MPIOContext,
        __in IN DSM_NOTIFICATION_TYPE NotificationType,
        __in IN PVOID PathId,
        __in IN PVOID DsmId,
        __inout IN OUT PNTSTATUS Status,
        __in IN LONG Reserved
        );    

/*        
#else

    VOID
    DsmNotification(
        __in IN PVOID MPIOContext,
        __in IN DSM_NOTIFICATION_TYPE NotificationType,
        ...
        );
        
#endif
*/    
    
    
#ifdef DsmWriteEvent
#undef DsmWriteEvent
#endif
#define DsmWriteEvent sdv_DsmWriteEvent
NTSTATUS
DsmWriteEvent(
    __in IN PVOID MPIOContext,
    __in IN PWSTR ComponentName,
    __in IN PWSTR EventDescription,
    __in IN ULONG Severity
    );

#ifdef DsmGetVersion
#undef DsmGetVersion
#endif
#define DsmGetVersion sdv_DsmGetVersion
NTSTATUS
DsmGetVersion(
    __inout IN OUT PMPIO_VERSION_INFO MpioVersion,
    __in IN ULONG MpioVersionSize
    );
    
#ifdef DsmGetContextFromSrb
#undef DsmGetContextFromSrb
#endif
#define DsmGetContextFromSrb sdv_DsmGetContextFromSrb    
PVOID
DsmGetContextFromSrb(
    __in IN PSCSI_REQUEST_BLOCK Srb
    );




#define	__sdv_save_request sdv_save_request
void sdv_save_request(void *r);
#define	__sdv_retrieve_request sdv_retrieve_request
void sdv_retrieve_request(void *r);


void sdv_io_read_begin();
void sdv_io_read_end();
void sdv_io_write_begin();
void sdv_io_write_end();
void sdv_io_default_begin();
void sdv_io_default_end();
void sdv_device_control_begin();
void sdv_device_control_end();
void sdv_internal_device_control_begin();
void sdv_internal_device_control_end();


#ifdef mpio_ReleaseQueue

#ifdef IoSetCompletionRoutine
#undef IoSetCompletionRoutine
#define IoSetCompletionRoutine sdv_mpio_IoSetCompletionRoutine
#endif
NTKERNELAPI
VOID
sdv_mpio_IoSetCompletionRoutine(
    IN PIRP pirp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID Context,
    IN BOOLEAN InvokeOnSuccess,
    IN BOOLEAN InvokeOnError,
    IN BOOLEAN InvokeOnCancel
    );

#ifdef IoFreeIrp
#undef IoFreeIrp
#define IoFreeIrp sdv_mpio_IoFreeIrp
#endif
NTKERNELAPI
VOID
sdv_mpio_IoFreeIrp(
    IN PIRP pirp
    );
    
#endif


#ifdef ExFreeToNPagedLookasideList
#undef ExFreeToNPagedLookasideList
#define ExFreeToNPagedLookasideList sdv_ExFreeToNPagedLookasideList
#endif

VOID
sdv_ExFreeToNPagedLookasideList (
    IN PNPAGED_LOOKASIDE_LIST Lookaside,
    IN PVOID Entry
    );


#endif
