/*
	Copyright (c) Microsoft Corporation.  All rights reserved.
*/

/* This file is a subset of the files: sdv_wdm.h, sdv_ntddk.h,
   sdv_ntifs.h, sdv_csq and sdv_wdmsec.h.  It is used when we compile
   the slic rules, so that no matter whether they are stated in term
   of function foo or function sdv_foo, they will work correctly.
*/
#define ExAllocatePoolWithQuota sdv_ExAllocatePoolWithQuota
#define ExAllocatePoolWithQuotaTag sdv_ExAllocatePoolWithQuotaTag
#define ExAllocatePoolZero sdv_ExAllocatePoolZero
#define ExAllocatePoolQuotaZero sdv_ExAllocatePoolQuotaZero
#define ExAllocatePoolPriorityZero sdv_ExAllocatePoolPriorityZero
#define ExAllocatePoolUninitialized sdv_ExAllocatePoolUninitialized
#define ExAllocatePoolQuotaUninitialized sdv_ExAllocatePoolQuotaUninitialized
#define ExAllocatePoolPriorityUninitialized sdv_ExAllocatePoolPriorityUninitialized
#define KeGetCurrentThread sdv_KeGetCurrentThread
#define ExInitializeFastMutex sdv_ExInitializeFastMutex
#define HalQuerySystemInformation sdv_HalQuerySystemInformation
#define InitializeObjectAttributes sdv_InitializeObjectAttributes
#define InterlockedDecrement sdv_InterlockedDecrement
#define InterlockedIncrement sdv_InterlockedIncrement
#define IoAcquireRemoveLock sdv_IoAcquireRemoveLock
#define IoAdjustPagingPathCount sdv_IoAdjustPagingPathCount
#define IoAllocateAdapterChannel sdv_IoAllocateAdapterChannel
#define IoAssignArcName sdv_IoAssignArcName
#define IoCallDriver sdv_IoCallDriver
#define IoCompleteRequest sdv_IoCompleteRequest
#define IoCopyCurrentIrpStackLocationToNext sdv_IoCopyCurrentIrpStackLocationToNext
#define IoCreateDeviceSecure sdv_IoCreateDeviceSecure
#define IoCsqInitialize sdv_IoCsqInitialize
#define IoCsqInitializeEx sdv_IoCsqInitializeEx
#define IoCsqInsertIrp sdv_IoCsqInsertIrp
#define IoCsqInsertIrpEx sdv_IoCsqInsertIrpEx
#define IoCsqRemoveIrp sdv_IoCsqRemoveIrp
#define IoCsqRemoveNextIrp sdv_IoCsqRemoveNextIrp
#define IoDeassignArcName sdv_IoDeassignArcName
#define IoFlushAdapterBuffers sdv_IoFlushAdapterBuffers
#define IoFreeAdapterChannel sdv_IoFreeAdapterChannel
#define IoFreeMapRegisters sdv_IoFreeMapRegisters
#define IoGetCurrentIrpStackLocation sdv_IoGetCurrentIrpStackLocation
#define IoGetFunctionCodeFromCtlCode sdv_IoGetFunctionCodeFromCtlCode
#define IoGetNextIrpStackLocation sdv_IoGetNextIrpStackLocation
#define IoGetRemainingStackSize sdv_IoGetRemainingStackSize
#define IoInitializeDpcRequest sdv_IoInitializeDpcRequest
#define IoInitializeRemoveLock sdv_IoInitializeRemoveLock
#define IoIsErrorUserInduced sdv_IoIsErrorUserInduced
#define IoMapTransfer sdv_IoMapTransfer
#define IoMarkIrpPending sdv_IoMarkIrpPending
#define IoReleaseRemoveLock sdv_IoReleaseRemoveLock
#define IoReleaseRemoveLockAndWait sdv_IoReleaseRemoveLockAndWait
#define IoRequestDpc sdv_IoRequestDpc
#define IoSetCancelRoutine sdv_IoSetCancelRoutine
#define IoSetCompletionRoutine sdv_IoSetCompletionRoutine
#define IoSetNextIrpStackLocation sdv_IoSetNextIrpStackLocation
#define IoSizeOfIrp sdv_IoSizeOfIrp
#define IoSizeofWorkItem sdv_IoSizeofWorkItem
#define IoSkipCurrentIrpStackLocation sdv_IoSkipCurrentIrpStackLocation
#define IoWMIDeviceObjectToProviderId sdv_IoWMIDeviceObjectToProviderId
#define KeAcquireSpinLock sdv_KeAcquireSpinLock
#define KeAcquireSpinLockAtDpcLevel sdv_KeAcquireSpinLockAtDpcLevel
#define KeFlushIoBuffers sdv_KeFlushIoBuffers
#define KeGetCurrentIrql sdv_KeGetCurrentIrql
#define KeLowerIrql sdv_KeLowerIrql
#define KeRaiseIrql sdv_KeRaiseIrql
#define KeRaiseIrqlToDpcLevel sdv_KeRaiseIrqlToDpcLevel
#define KeRaiseIrqlToSynchLevel sdv_KeRaiseIrqlToSynchLevel
#define KeInitializeSpinLock sdv_KeInitializeSpinLock
#define KeReleaseSpinLock sdv_KeReleaseSpinLock
#define KeReleaseSpinLockFromDpcLevel sdv_KeReleaseSpinLockFromDpcLevel
#define KeWaitForMutexObject sdv_KeWaitForMutexObject
#define MmGetSystemAddressForMdl sdv_MmGetSystemAddressForMdl
#define MmGetSystemAddressForMdlSafe sdv_MmGetSystemAddressForMdlSafe
#define MmLockPagableCodeSection sdv_MmLockPagableCodeSection
#define MmMapLockedPagesSpecifyCache sdv_MmMapLockedPagesSpecifyCache
#define ObDereferenceObject sdv_ObDereferenceObject
#define ObReferenceObject sdv_ObReferenceObject
#define RtlMoveMemory sdv_RtlMoveMemory
#define RtlAnsiStringToUnicodeString sdv_RtlAnsiStringToUnicodeString
#define RtlUnicodeStringToAnsiString sdv_RtlUnicodeStringToAnsiString
#define RtlUpcaseUnicodeStringToAnsiString sdv_RtlUpcaseUnicodeStringToAnsiString
#define RtlOemStringToUnicodeString sdv_RtlOemStringToUnicodeString
#define RtlUnicodeStringToOemString sdv_RtlUnicodeStringToOemString
#define RtlUpcaseUnicodeStringToOemString sdv_RtlUpcaseUnicodeStringToOemString
#define RtlOemStringToCountedUnicodeString sdv_RtlOemStringToCountedUnicodeString
#define RtlUnicodeStringToCountedOemString sdv_RtlUnicodeStringToCountedOemString
#define RtlUpcaseUnicodeStringToCountedOemString sdv_RtlUpcaseUnicodeStringToCountedOemString
#define RtlUpcaseUnicodeString sdv_RtlUpcaseUnicodeString
#define RtlDowncaseUnicodeString sdv_RtlDowncaseUnicodeString
#define RtlCreateUnicodeString sdv_RtlCreateUnicodeString
#define RtlDuplicateUnicodeString sdv_RtlDuplicateUnicodeString
#define RtlCompareUnicodeString sdv_RtlCompareUnicodeString
#define RtlEqualUnicodeString sdv_RtlEqualUnicodeString
#define RtlHashUnicodeString sdv_RtlHashUnicodeString
#define RtlUpcaseUnicodeChar sdv_RtlUpcaseUnicodeChar
#define RtlDowncaseUnicodeChar sdv_RtlDowncaseUnicodeChar
#define RtlFreeUnicodeString sdv_RtlFreeUnicodeString
#define RtlxAnsiStringToUnicodeSize sdv_RtlxAnsiStringToUnicodeSize
#define RtlxUnicodeStringToAnsiSize sdv_RtlxUnicodeStringToAnsiSize
#define RtlUnicodeToUTF8N sdv_RtlUnicodeToUTF8N
#define RtlUTF8ToUnicodeN sdv_RtlUTF8ToUnicodeN
#define RtlStringFromGUID sdv_RtlStringFromGUID
#define RtlGUIDFromString sdv_RtlGUIDFromString
#define RtlGenerateClass5Guid sdv_RtlGenerateClass5Guid
#define InsertTailList sdv_InsertTailList
#define RemoveEntryList sdv_RemoveEntryList
#define IsListEmpty sdv_IsListEmpty
#define ExAllocateFromNPagedLookasideList sdv_ExAllocateFromNPagedLookasideList
#define ExReleaseResourceForThread sdv_ExReleaseResourceForThread 
#define ExIsResourceAcquiredShared sdv_ExIsResourceAcquiredShared 
#define ExIsResourceAcquired sdv_ExIsResourceAcquired
#define ExIsResourceAcquiredExclusive sdv_ExIsResourceAcquiredExclusive
#define ExInitializeResource sdv_ExInitializeResource
#define ExAllocateFromPagedLookasideList sdv_ExAllocateFromPagedLookasideList
#define ExFreeToPagedLookasideList sdv_ExFreeToPagedLookasideList
#define _ReadWriteBarrier sdv__ReadWriteBarrier
#define ExInterlockedCompareExchange64 sdv_ExInterlockedCompareExchange64
#define ExInterlockedPopEntrySList sdv_ExInterlockedPopEntrySList 
#define ExInterlockedPopEntrySList sdv_ExInterlockedPopEntrySList 
#define ExInterlockedPushEntrySList sdv_ExInterlockedPushEntrySList 
#define ExAcquireSpinLock sdv_ExAcquireSpinLock
#define ExAcquireSpinLockAtDpcLevel sdv_ExAcquireSpinLockAtDpcLevel
#define ExReleaseSpinLock sdv_ExReleaseSpinLock 
#define ExReleaseSpinLockFromDpcLevel sdv_ExReleaseSpinLockFromDpcLevel
#define KeReleaseSpinLockFromDpcLevel sdv_KeReleaseSpinLockFromDpcLevel 
#define KeAcquireSpinLockAtDpcLevel sdv_KeAcquireSpinLockAtDpcLevel
#define ExIsResourceAcquiredLite sdv_ExIsResourceAcquiredLite 
#define ExInterlockedInsertHeadList sdv_ExInterlockedInsertHeadList 
#define ExInterlockedInsertTailList sdv_ExInterlockedInsertTailList
#define ExInterlockedPopEntryList sdv_ExInterlockedPopEntryList
#define ExInterlockedPushEntryList sdv_ExInterlockedPushEntryList
#define ExInterlockedRemoveHeadList sdv_ExInterlockedRemoveHeadList
#define IoReleaseRemoveLock sdv_IoReleaseRemoveLock
#define IoReleaseRemoveLockAndWait sdv_IoReleaseRemoveLockAndWait
#define IoRequestDpc sdv_IoRequestDpc
#define InterlockedCompareExchangeAcquire sdv_InterlockedCompareExchangeAcquire
#define ExInterlockedPopEntrySList sdv_ExInterlockedPopEntrySList
#define InterlockedPushEntrySList sdv_InterlockedPushEntrySList
#define InterlockedPopEntrySList sdv_InterlockedPopEntrySList
#define ObDereferenceObjectWithTag sdv_ObDereferenceObjectWithTag
#define ObReferenceObjectWithTag sdv_ObReferenceObjectWithTag
#define HalAllocateCommonBuffer sdv_HalAllocateCommonBuffer
#define HalAllocateCrashDumpRegisters sdv_HalAllocateCrashDumpRegisters
#define HalFreeCommonBuffer sdv_HalFreeCommonBuffer
#define memcpy sdv_memcpy
#define ExFreePool sdv_ExFreePool
#define ExInterlockedFlushSList sdv_ExInterlockedFlushSList
#define CONTAINING_RECORD sdv_containing_record
#define READ_REGISTER_ULONG64 sdv_READ_REGISTER_ULONG64
#define READ_REGISTER_BUFFER_ULONG64 sdv_READ_REGISTER_BUFFER_ULONG64
#define WRITE_REGISTER_ULONG64 sdv_WRITE_REGISTER_ULONG64
#define sdv_maskedEflags (sdv_maskedEflags & EFLAGS_IF_MASK)
#define RtlQueryRegistryValues sdv_RtlQueryRegistryValues
#define ExQueueWorkItem sdv_ExQueueWorkItem
#define DbgBreakPoint sdv_DbgBreakPoint
#define PoFxRegisterDevice sdv_PoFxRegisterDevice
#define PoFxStartDevicePowerManagement sdv_PoFxStartDevicePowerManagement
#define PoFxUnregisterDevice sdv_PoFxUnregisterDevice
#define PoFxActivateComponent sdv_PoFxActivateComponent
#define PoFxIdleComponent sdv_PoFxIdleComponent
#define PoFxSetComponentLatency sdv_PoFxSetComponentLatency
#define PoFxSetComponentResidency sdv_PoFxSetComponentResidency
#define PoFxSetComponentWake sdv_PoFxSetComponentWake
#define PoFxReportDevicePoweredOn sdv_PoFxReportDevicePoweredOn
#define PoFxCompleteIdleState sdv_PoFxCompleteIdleState
#define PoFxPowerControl sdv_PoFxPowerControl
#define PoFxNotifySurprisePowerOn sdv_PoFxNotifySurprisePowerOn
#define PoFxSetDeviceIdleTimeout sdv_PoFxSetDeviceIdleTimeout
#define PoFxCompleteIdleCondition sdv_PoFxCompleteIdleCondition
#define PoFxCompleteDevicePowerNotRequired sdv_PoFxCompleteDevicePowerNotRequired
#define KeGetCurrentProcessorNumber sdv_KeGetCurrentProcessorNumber
