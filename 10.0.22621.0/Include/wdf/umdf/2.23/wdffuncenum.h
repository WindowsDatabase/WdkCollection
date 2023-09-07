/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name: WdfFuncEnum.h

Abstract:
    Generated an enum of all WDF API functions

Environment:
    kernel mode only

    Warning: manual changes to this file will be lost.
--*/

#ifndef _WDFFUNCENUM_H_
#define _WDFFUNCENUM_H_

extern PWDF_DRIVER_GLOBALS WdfDriverGlobals;

typedef enum _WDFFUNCENUM {

    WdfCollectionCreateTableIndex = 0,
    WdfCollectionGetCountTableIndex = 1,
    WdfCollectionAddTableIndex = 2,
    WdfCollectionRemoveTableIndex = 3,
    WdfCollectionRemoveItemTableIndex = 4,
    WdfCollectionGetItemTableIndex = 5,
    WdfCollectionGetFirstItemTableIndex = 6,
    WdfCollectionGetLastItemTableIndex = 7,
    WdfCxDeviceInitAllocateTableIndex = 8,
    WdfCxDeviceInitSetRequestAttributesTableIndex = 9,
    WdfCxDeviceInitSetFileObjectConfigTableIndex = 10,
    WdfCxVerifierKeBugCheckTableIndex = 11,
    WdfDeviceGetDeviceStateTableIndex = 12,
    WdfDeviceSetDeviceStateTableIndex = 13,
    WdfDeviceGetDriverTableIndex = 14,
    WdfDeviceGetIoTargetTableIndex = 15,
    WdfDeviceAssignS0IdleSettingsTableIndex = 16,
    WdfDeviceAssignSxWakeSettingsTableIndex = 17,
    WdfDeviceOpenRegistryKeyTableIndex = 18,
    WdfDeviceInitSetPnpPowerEventCallbacksTableIndex = 19,
    WdfDeviceInitSetPowerPolicyEventCallbacksTableIndex = 20,
    WdfDeviceInitSetPowerPolicyOwnershipTableIndex = 21,
    WdfDeviceInitSetIoTypeTableIndex = 22,
    WdfDeviceInitSetFileObjectConfigTableIndex = 23,
    WdfDeviceInitSetRequestAttributesTableIndex = 24,
    WdfDeviceCreateTableIndex = 25,
    WdfDeviceSetStaticStopRemoveTableIndex = 26,
    WdfDeviceCreateDeviceInterfaceTableIndex = 27,
    WdfDeviceSetDeviceInterfaceStateTableIndex = 28,
    WdfDeviceRetrieveDeviceInterfaceStringTableIndex = 29,
    WdfDeviceCreateSymbolicLinkTableIndex = 30,
    WdfDeviceQueryPropertyTableIndex = 31,
    WdfDeviceAllocAndQueryPropertyTableIndex = 32,
    WdfDeviceSetPnpCapabilitiesTableIndex = 33,
    WdfDeviceSetPowerCapabilitiesTableIndex = 34,
    WdfDeviceSetFailedTableIndex = 35,
    WdfDeviceStopIdleNoTrackTableIndex = 36,
    WdfDeviceResumeIdleNoTrackTableIndex = 37,
    WdfDeviceGetFileObjectTableIndex = 38,
    WdfDeviceGetDefaultQueueTableIndex = 39,
    WdfDeviceConfigureRequestDispatchingTableIndex = 40,
    WdfDeviceGetSystemPowerActionTableIndex = 41,
    WdfDeviceInitSetReleaseHardwareOrderOnFailureTableIndex = 42,
    WdfDeviceInitSetIoTypeExTableIndex = 43,
    WdfDevicePostEventTableIndex = 44,
    WdfDeviceMapIoSpaceTableIndex = 45,
    WdfDeviceUnmapIoSpaceTableIndex = 46,
    WdfDeviceGetHardwareRegisterMappedAddressTableIndex = 47,
    WdfDeviceReadFromHardwareTableIndex = 48,
    WdfDeviceWriteToHardwareTableIndex = 49,
    WdfDeviceAssignInterfacePropertyTableIndex = 50,
    WdfDeviceAllocAndQueryInterfacePropertyTableIndex = 51,
    WdfDeviceQueryInterfacePropertyTableIndex = 52,
    WdfDeviceGetDeviceStackIoTypeTableIndex = 53,
    WdfDeviceQueryPropertyExTableIndex = 54,
    WdfDeviceAllocAndQueryPropertyExTableIndex = 55,
    WdfDeviceAssignPropertyTableIndex = 56,
    WdfDriverCreateTableIndex = 57,
    WdfDriverGetRegistryPathTableIndex = 58,
    WdfDriverOpenParametersRegistryKeyTableIndex = 59,
    WdfDriverRetrieveVersionStringTableIndex = 60,
    WdfDriverIsVersionAvailableTableIndex = 61,
    WdfFdoInitOpenRegistryKeyTableIndex = 62,
    WdfFdoInitQueryPropertyTableIndex = 63,
    WdfFdoInitAllocAndQueryPropertyTableIndex = 64,
    WdfFdoInitQueryPropertyExTableIndex = 65,
    WdfFdoInitAllocAndQueryPropertyExTableIndex = 66,
    WdfFdoInitSetFilterTableIndex = 67,
    WdfFileObjectGetFileNameTableIndex = 68,
    WdfFileObjectGetDeviceTableIndex = 69,
    WdfFileObjectGetInitiatorProcessIdTableIndex = 70,
    WdfFileObjectGetRelatedFileObjectTableIndex = 71,
    WdfInterruptCreateTableIndex = 72,
    WdfInterruptQueueDpcForIsrTableIndex = 73,
    WdfInterruptQueueWorkItemForIsrTableIndex = 74,
    WdfInterruptSynchronizeTableIndex = 75,
    WdfInterruptAcquireLockTableIndex = 76,
    WdfInterruptReleaseLockTableIndex = 77,
    WdfInterruptEnableTableIndex = 78,
    WdfInterruptDisableTableIndex = 79,
    WdfInterruptGetInfoTableIndex = 80,
    WdfInterruptSetPolicyTableIndex = 81,
    WdfInterruptSetExtendedPolicyTableIndex = 82,
    WdfInterruptGetDeviceTableIndex = 83,
    WdfInterruptTryToAcquireLockTableIndex = 84,
    WdfIoQueueCreateTableIndex = 85,
    WdfIoQueueGetStateTableIndex = 86,
    WdfIoQueueStartTableIndex = 87,
    WdfIoQueueStopTableIndex = 88,
    WdfIoQueueStopSynchronouslyTableIndex = 89,
    WdfIoQueueGetDeviceTableIndex = 90,
    WdfIoQueueRetrieveNextRequestTableIndex = 91,
    WdfIoQueueRetrieveRequestByFileObjectTableIndex = 92,
    WdfIoQueueFindRequestTableIndex = 93,
    WdfIoQueueRetrieveFoundRequestTableIndex = 94,
    WdfIoQueueDrainSynchronouslyTableIndex = 95,
    WdfIoQueueDrainTableIndex = 96,
    WdfIoQueuePurgeSynchronouslyTableIndex = 97,
    WdfIoQueuePurgeTableIndex = 98,
    WdfIoQueueReadyNotifyTableIndex = 99,
    WdfIoQueueStopAndPurgeTableIndex = 100,
    WdfIoQueueStopAndPurgeSynchronouslyTableIndex = 101,
    WdfIoTargetCreateTableIndex = 102,
    WdfIoTargetOpenTableIndex = 103,
    WdfIoTargetCloseForQueryRemoveTableIndex = 104,
    WdfIoTargetCloseTableIndex = 105,
    WdfIoTargetStartTableIndex = 106,
    WdfIoTargetStopTableIndex = 107,
    WdfIoTargetPurgeTableIndex = 108,
    WdfIoTargetGetStateTableIndex = 109,
    WdfIoTargetGetDeviceTableIndex = 110,
    WdfIoTargetSendReadSynchronouslyTableIndex = 111,
    WdfIoTargetFormatRequestForReadTableIndex = 112,
    WdfIoTargetSendWriteSynchronouslyTableIndex = 113,
    WdfIoTargetFormatRequestForWriteTableIndex = 114,
    WdfIoTargetSendIoctlSynchronouslyTableIndex = 115,
    WdfIoTargetFormatRequestForIoctlTableIndex = 116,
    WdfMemoryCreateTableIndex = 117,
    WdfMemoryCreatePreallocatedTableIndex = 118,
    WdfMemoryGetBufferTableIndex = 119,
    WdfMemoryAssignBufferTableIndex = 120,
    WdfMemoryCopyToBufferTableIndex = 121,
    WdfMemoryCopyFromBufferTableIndex = 122,
    WdfObjectGetTypedContextWorkerTableIndex = 123,
    WdfObjectAllocateContextTableIndex = 124,
    WdfObjectContextGetObjectTableIndex = 125,
    WdfObjectReferenceActualTableIndex = 126,
    WdfObjectDereferenceActualTableIndex = 127,
    WdfObjectCreateTableIndex = 128,
    WdfObjectDeleteTableIndex = 129,
    WdfObjectQueryTableIndex = 130,
    WdfRegistryOpenKeyTableIndex = 131,
    WdfRegistryCreateKeyTableIndex = 132,
    WdfRegistryCloseTableIndex = 133,
    WdfRegistryRemoveKeyTableIndex = 134,
    WdfRegistryRemoveValueTableIndex = 135,
    WdfRegistryQueryValueTableIndex = 136,
    WdfRegistryQueryMemoryTableIndex = 137,
    WdfRegistryQueryMultiStringTableIndex = 138,
    WdfRegistryQueryUnicodeStringTableIndex = 139,
    WdfRegistryQueryStringTableIndex = 140,
    WdfRegistryQueryULongTableIndex = 141,
    WdfRegistryAssignValueTableIndex = 142,
    WdfRegistryAssignMemoryTableIndex = 143,
    WdfRegistryAssignMultiStringTableIndex = 144,
    WdfRegistryAssignUnicodeStringTableIndex = 145,
    WdfRegistryAssignStringTableIndex = 146,
    WdfRegistryAssignULongTableIndex = 147,
    WdfRequestCreateTableIndex = 148,
    WdfRequestReuseTableIndex = 149,
    WdfRequestChangeTargetTableIndex = 150,
    WdfRequestFormatRequestUsingCurrentTypeTableIndex = 151,
    WdfRequestSendTableIndex = 152,
    WdfRequestGetStatusTableIndex = 153,
    WdfRequestMarkCancelableTableIndex = 154,
    WdfRequestMarkCancelableExTableIndex = 155,
    WdfRequestUnmarkCancelableTableIndex = 156,
    WdfRequestIsCanceledTableIndex = 157,
    WdfRequestCancelSentRequestTableIndex = 158,
    WdfRequestIsFrom32BitProcessTableIndex = 159,
    WdfRequestSetCompletionRoutineTableIndex = 160,
    WdfRequestGetCompletionParamsTableIndex = 161,
    WdfRequestAllocateTimerTableIndex = 162,
    WdfRequestCompleteTableIndex = 163,
    WdfRequestCompleteWithInformationTableIndex = 164,
    WdfRequestGetParametersTableIndex = 165,
    WdfRequestRetrieveInputMemoryTableIndex = 166,
    WdfRequestRetrieveOutputMemoryTableIndex = 167,
    WdfRequestRetrieveInputBufferTableIndex = 168,
    WdfRequestRetrieveOutputBufferTableIndex = 169,
    WdfRequestSetInformationTableIndex = 170,
    WdfRequestGetInformationTableIndex = 171,
    WdfRequestGetFileObjectTableIndex = 172,
    WdfRequestGetRequestorModeTableIndex = 173,
    WdfRequestForwardToIoQueueTableIndex = 174,
    WdfRequestGetIoQueueTableIndex = 175,
    WdfRequestRequeueTableIndex = 176,
    WdfRequestStopAcknowledgeTableIndex = 177,
    WdfRequestImpersonateTableIndex = 178,
    WdfRequestGetRequestorProcessIdTableIndex = 179,
    WdfRequestIsFromUserModeDriverTableIndex = 180,
    WdfRequestSetUserModeDriverInitiatedIoTableIndex = 181,
    WdfRequestGetUserModeDriverInitiatedIoTableIndex = 182,
    WdfRequestSetActivityIdTableIndex = 183,
    WdfRequestRetrieveActivityIdTableIndex = 184,
    WdfRequestGetEffectiveIoTypeTableIndex = 185,
    WdfCmResourceListGetCountTableIndex = 186,
    WdfCmResourceListGetDescriptorTableIndex = 187,
    WdfStringCreateTableIndex = 188,
    WdfStringGetUnicodeStringTableIndex = 189,
    WdfObjectAcquireLockTableIndex = 190,
    WdfObjectReleaseLockTableIndex = 191,
    WdfWaitLockCreateTableIndex = 192,
    WdfWaitLockAcquireTableIndex = 193,
    WdfWaitLockReleaseTableIndex = 194,
    WdfSpinLockCreateTableIndex = 195,
    WdfSpinLockAcquireTableIndex = 196,
    WdfSpinLockReleaseTableIndex = 197,
    WdfTimerCreateTableIndex = 198,
    WdfTimerStartTableIndex = 199,
    WdfTimerStopTableIndex = 200,
    WdfTimerGetParentObjectTableIndex = 201,
    WdfUsbTargetDeviceCreateTableIndex = 202,
    WdfUsbTargetDeviceCreateWithParametersTableIndex = 203,
    WdfUsbTargetDeviceRetrieveInformationTableIndex = 204,
    WdfUsbTargetDeviceGetDeviceDescriptorTableIndex = 205,
    WdfUsbTargetDeviceRetrieveConfigDescriptorTableIndex = 206,
    WdfUsbTargetDeviceQueryStringTableIndex = 207,
    WdfUsbTargetDeviceAllocAndQueryStringTableIndex = 208,
    WdfUsbTargetDeviceFormatRequestForStringTableIndex = 209,
    WdfUsbTargetDeviceGetNumInterfacesTableIndex = 210,
    WdfUsbTargetDeviceSelectConfigTableIndex = 211,
    WdfUsbTargetDeviceSendControlTransferSynchronouslyTableIndex = 212,
    WdfUsbTargetDeviceFormatRequestForControlTransferTableIndex = 213,
    WdfUsbTargetDeviceResetPortSynchronouslyTableIndex = 214,
    WdfUsbTargetDeviceQueryUsbCapabilityTableIndex = 215,
    WdfUsbTargetPipeGetInformationTableIndex = 216,
    WdfUsbTargetPipeIsInEndpointTableIndex = 217,
    WdfUsbTargetPipeIsOutEndpointTableIndex = 218,
    WdfUsbTargetPipeGetTypeTableIndex = 219,
    WdfUsbTargetPipeSetNoMaximumPacketSizeCheckTableIndex = 220,
    WdfUsbTargetPipeWriteSynchronouslyTableIndex = 221,
    WdfUsbTargetPipeFormatRequestForWriteTableIndex = 222,
    WdfUsbTargetPipeReadSynchronouslyTableIndex = 223,
    WdfUsbTargetPipeFormatRequestForReadTableIndex = 224,
    WdfUsbTargetPipeConfigContinuousReaderTableIndex = 225,
    WdfUsbTargetPipeAbortSynchronouslyTableIndex = 226,
    WdfUsbTargetPipeFormatRequestForAbortTableIndex = 227,
    WdfUsbTargetPipeResetSynchronouslyTableIndex = 228,
    WdfUsbTargetPipeFormatRequestForResetTableIndex = 229,
    WdfUsbInterfaceGetInterfaceNumberTableIndex = 230,
    WdfUsbInterfaceGetNumEndpointsTableIndex = 231,
    WdfUsbInterfaceGetDescriptorTableIndex = 232,
    WdfUsbInterfaceGetNumSettingsTableIndex = 233,
    WdfUsbInterfaceSelectSettingTableIndex = 234,
    WdfUsbInterfaceGetEndpointInformationTableIndex = 235,
    WdfUsbTargetDeviceGetInterfaceTableIndex = 236,
    WdfUsbInterfaceGetConfiguredSettingIndexTableIndex = 237,
    WdfUsbInterfaceGetNumConfiguredPipesTableIndex = 238,
    WdfUsbInterfaceGetConfiguredPipeTableIndex = 239,
    WdfVerifierDbgBreakPointTableIndex = 240,
    WdfVerifierKeBugCheckTableIndex = 241,
    WdfGetTriageInfoTableIndex = 242,
    WdfWorkItemCreateTableIndex = 243,
    WdfWorkItemEnqueueTableIndex = 244,
    WdfWorkItemGetParentObjectTableIndex = 245,
    WdfWorkItemFlushTableIndex = 246,
    WdfRegistryWdmGetHandleTableIndex = 247,
    WdfDeviceStopIdleActualTableIndex = 248,
    WdfDeviceResumeIdleActualTableIndex = 249,
    WdfDeviceInitEnableHidInterfaceTableIndex = 250,
    WdfDeviceHidNotifyPresenceTableIndex = 251,
    WdfDeviceGetSelfIoTargetTableIndex = 252,
    WdfDeviceInitAllowSelfIoTargetTableIndex = 253,
    WdfIoTargetSelfAssignDefaultIoQueueTableIndex = 254,
    WdfDeviceOpenDevicemapKeyTableIndex = 255,
    WdfIoTargetWdmGetTargetFileHandleTableIndex = 256,
    WdfDeviceWdmDispatchIrpTableIndex = 257,
    WdfDeviceWdmDispatchIrpToIoQueueTableIndex = 258,
    WdfDeviceConfigureWdmIrpDispatchCallbackTableIndex = 259,
    WdfCxDeviceInitSetPnpPowerEventCallbacksTableIndex = 260,
    WdfCompanionCreateTableIndex = 261,
    WdfCompanionWdmGetSecureDeviceHandleTableIndex = 262,
    WdfCompanionCreateTaskQueueTableIndex = 263,
    WdfDeviceInitSetCompanionEventCallbacksTableIndex = 264,
    WdfFunctionTableNumEntries = 265,
} WDFFUNCENUM;

#endif // _WDFFUNCENUM_H_

