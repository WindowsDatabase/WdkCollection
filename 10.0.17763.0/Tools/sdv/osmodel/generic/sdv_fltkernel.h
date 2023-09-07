/*  
	Copyright (c) Microsoft Corporation.  All rights reserved.

	This file is included immediately after fltkernel.h and
	redefines the DDIs that filter manager supports.

	See sdv_miniFilter_stubs.c for model implementation of these
	DDIs.

	Khuram Shahid June '09
*/



#ifndef _SDV_FLTKERNEL_H_
#define _SDV_FLTKERNEL_H_




#ifdef  IoGetTopLevelIrp
#undef  IoGetTopLevelIrp
#endif
#define IoGetTopLevelIrp sdv_IoGetTopLevelIrp


#ifdef  FltSetCallbackDataDirty
#undef  FltSetCallbackDataDirty
#endif
#define FltSetCallbackDataDirty sdv_FltSetCallbackDataDirty

#ifdef  FltClearCallbackDataDirty
#undef  FltClearCallbackDataDirty
#endif
#define FltClearCallbackDataDirty sdv_FltClearCallbackDataDirty

#ifdef  FltIsCallbackDataDirty
#undef  FltIsCallbackDataDirty
#endif
#define FltIsCallbackDataDirty sdv_FltIsCallbackDataDirty

#ifdef  FltDoCompletionProcessingWhenSafe
#undef  FltDoCompletionProcessingWhenSafe
#endif
#define FltDoCompletionProcessingWhenSafe sdv_FltDoCompletionProcessingWhenSafe

#ifdef  FltCheckAndGrowNameControl
#undef  FltCheckAndGrowNameControl
#endif
#define FltCheckAndGrowNameControl sdv_FltCheckAndGrowNameControl

#ifdef  FltPurgeFileNameInformationCache
#undef  FltPurgeFileNameInformationCache
#endif
#define FltPurgeFileNameInformationCache sdv_FltPurgeFileNameInformationCache

#ifdef  FltRegisterFilter
#undef  FltRegisterFilter
#endif
#define FltRegisterFilter sdv_FltRegisterFilter

#ifdef  FltUnregisterFilter
#undef  FltUnregisterFilter
#endif
#define FltUnregisterFilter sdv_FltUnregisterFilter

#ifdef  FltStartFiltering
#undef  FltStartFiltering
#endif
#define FltStartFiltering sdv_FltStartFiltering

#ifdef  FltGetRoutineAddress
#undef  FltGetRoutineAddress
#endif
#define FltGetRoutineAddress sdv_FltGetRoutineAddress

#ifdef  FltMgrRoutineName
#undef  FltMgrRoutineName
#endif
#define FltMgrRoutineName sdv_FltMgrRoutineName

#ifdef  FltCompletePendedPreOperation
#undef  FltCompletePendedPreOperation
#endif
#define FltCompletePendedPreOperation sdv_FltCompletePendedPreOperation

#ifdef  FltCompletePendedPostOperation
#undef  FltCompletePendedPostOperation
#endif
#define FltCompletePendedPostOperation sdv_FltCompletePendedPostOperation

#ifdef  FltRequestOperationStatusCallback
#undef  FltRequestOperationStatusCallback
#endif
#define FltRequestOperationStatusCallback sdv_FltRequestOperationStatusCallback

#ifdef  FltAllocatePoolAlignedWithTag
#undef  FltAllocatePoolAlignedWithTag
#endif
#define FltAllocatePoolAlignedWithTag sdv_FltAllocatePoolAlignedWithTag

#ifdef  FltFreePoolAlignedWithTag
#undef  FltFreePoolAlignedWithTag
#endif
#define FltFreePoolAlignedWithTag sdv_FltFreePoolAlignedWithTag

#ifdef  FltGetFileNameFormat
#undef  FltGetFileNameFormat
#endif
#define FltGetFileNameFormat sdv_FltGetFileNameFormat

#ifdef  FltGetFileNameQueryMethod
#undef  FltGetFileNameQueryMethod
#endif
#define FltGetFileNameQueryMethod sdv_FltGetFileNameQueryMethod

#ifdef  FltGetFileNameInformation
#undef  FltGetFileNameInformation
#endif
#define FltGetFileNameInformation sdv_FltGetFileNameInformation

#ifdef  FltGetFileNameInformationUnsafe
#undef  FltGetFileNameInformationUnsafe
#endif
#define FltGetFileNameInformationUnsafe sdv_FltGetFileNameInformationUnsafe

#ifdef  FltReleaseFileNameInformation
#undef  FltReleaseFileNameInformation
#endif
#define FltReleaseFileNameInformation sdv_FltReleaseFileNameInformation

#ifdef  FltReferenceFileNameInformation
#undef  FltReferenceFileNameInformation
#endif
#define FltReferenceFileNameInformation sdv_FltReferenceFileNameInformation

#ifdef  FltParseFileName
#undef  FltParseFileName
#endif
#define FltParseFileName sdv_FltParseFileName

#ifdef  FltParseFileNameInformation
#undef  FltParseFileNameInformation
#endif
#define FltParseFileNameInformation sdv_FltParseFileNameInformation

#ifdef  FltGetTunneledName
#undef  FltGetTunneledName
#endif
#define FltGetTunneledName sdv_FltGetTunneledName

#ifdef  FltGetVolumeName
#undef  FltGetVolumeName
#endif
#define FltGetVolumeName sdv_FltGetVolumeName

#ifdef  FltGetDestinationFileNameInformation
#undef  FltGetDestinationFileNameInformation
#endif
#define FltGetDestinationFileNameInformation sdv_FltGetDestinationFileNameInformation

#ifdef  FltIsDirectory
#undef  FltIsDirectory
#endif
#define FltIsDirectory sdv_FltIsDirectory

#ifdef  FltLoadFilter
#undef  FltLoadFilter
#endif
#define FltLoadFilter sdv_FltLoadFilter

#ifdef  FltUnloadFilter
#undef  FltUnloadFilter
#endif
#define FltUnloadFilter sdv_FltUnloadFilter

#ifdef  FltAttachVolume
#undef  FltAttachVolume
#endif
#define FltAttachVolume sdv_FltAttachVolume

#ifdef  FltAttachVolumeAtAltitude
#undef  FltAttachVolumeAtAltitude
#endif
#define FltAttachVolumeAtAltitude sdv_FltAttachVolumeAtAltitude

#ifdef  FltDetachVolume
#undef  FltDetachVolume
#endif
#define FltDetachVolume sdv_FltDetachVolume

#ifdef  FltAllocateCallbackData
#undef  FltAllocateCallbackData
#endif
#define FltAllocateCallbackData sdv_FltAllocateCallbackData

#ifdef  FltAllocateCallbackDataEx
#undef  FltAllocateCallbackDataEx
#endif
#define FltAllocateCallbackDataEx sdv_FltAllocateCallbackDataEx

#ifdef  FltFreeCallbackData
#undef  FltFreeCallbackData
#endif
#define FltFreeCallbackData sdv_FltFreeCallbackData

#ifdef  FltReuseCallbackData
#undef  FltReuseCallbackData
#endif
#define FltReuseCallbackData sdv_FltReuseCallbackData

#ifdef  FltPerformSynchronousIo
#undef  FltPerformSynchronousIo
#endif
#define FltPerformSynchronousIo sdv_FltPerformSynchronousIo

#ifdef  FltPerformAsynchronousIo
#undef  FltPerformAsynchronousIo
#endif
#define FltPerformAsynchronousIo sdv_FltPerformAsynchronousIo

#ifdef  FltCreateFileEx2
#undef  FltCreateFileEx2
#endif
#define FltCreateFileEx2 sdv_FltCreateFileEx2

#ifdef  FltCreateFileEx
#undef  FltCreateFileEx
#endif
#define FltCreateFileEx sdv_FltCreateFileEx

#ifdef  FltCreateFile
#undef  FltCreateFile
#endif
#define FltCreateFile sdv_FltCreateFile

#ifdef  FltOpenVolume
#undef  FltOpenVolume
#endif
#define FltOpenVolume sdv_FltOpenVolume

#ifdef  FltReadFile
#undef  FltReadFile
#endif
#define FltReadFile sdv_FltReadFile

#ifdef  FltTagFile
#undef  FltTagFile
#endif
#define FltTagFile sdv_FltTagFile

#ifdef  FltUntagFile
#undef  FltUntagFile
#endif
#define FltUntagFile sdv_FltUntagFile

#ifdef  FltWriteFile
#undef  FltWriteFile
#endif
#define FltWriteFile sdv_FltWriteFile

#ifdef  FltQueryInformationFile
#undef  FltQueryInformationFile
#endif
#define FltQueryInformationFile sdv_FltQueryInformationFile

#ifdef  FltSetInformationFile
#undef  FltSetInformationFile
#endif
#define FltSetInformationFile sdv_FltSetInformationFile

#ifdef  FltQueryDirectoryFile
#undef  FltQueryDirectoryFile
#endif
#define FltQueryDirectoryFile sdv_FltQueryDirectoryFile

#ifdef  FltQueryEaFile
#undef  FltQueryEaFile
#endif
#define FltQueryEaFile sdv_FltQueryEaFile

#ifdef  FltSetEaFile
#undef  FltSetEaFile
#endif
#define FltSetEaFile sdv_FltSetEaFile

#ifdef  FltQueryVolumeInformationFile
#undef  FltQueryVolumeInformationFile
#endif
#define FltQueryVolumeInformationFile sdv_FltQueryVolumeInformationFile

#ifdef  FltQuerySecurityObject
#undef  FltQuerySecurityObject
#endif
#define FltQuerySecurityObject sdv_FltQuerySecurityObject

#ifdef  FltSetSecurityObject
#undef  FltSetSecurityObject
#endif
#define FltSetSecurityObject sdv_FltSetSecurityObject

#ifdef  FltFlushBuffers
#undef  FltFlushBuffers
#endif
#define FltFlushBuffers sdv_FltFlushBuffers

#ifdef  FltFsControlFile
#undef  FltFsControlFile
#endif
#define FltFsControlFile sdv_FltFsControlFile

#ifdef  FltDeviceIoControlFile
#undef  FltDeviceIoControlFile
#endif
#define FltDeviceIoControlFile sdv_FltDeviceIoControlFile

#ifdef  FltReissueSynchronousIo
#undef  FltReissueSynchronousIo
#endif
#define FltReissueSynchronousIo sdv_FltReissueSynchronousIo

#ifdef  FltClose
#undef  FltClose
#endif
#define FltClose sdv_FltClose

#ifdef  FltCancelFileOpen
#undef  FltCancelFileOpen
#endif
#define FltCancelFileOpen sdv_FltCancelFileOpen

#ifdef  FltCreateSystemVolumeInformationFolder
#undef  FltCreateSystemVolumeInformationFolder
#endif
#define FltCreateSystemVolumeInformationFolder sdv_FltCreateSystemVolumeInformationFolder

#ifdef  FltSupportsFileContextsEx
#undef  FltSupportsFileContextsEx
#endif
#define FltSupportsFileContextsEx sdv_FltSupportsFileContextsEx

#ifdef  FltSupportsFileContexts
#undef  FltSupportsFileContexts
#endif
#define FltSupportsFileContexts sdv_FltSupportsFileContexts

#ifdef  FltSupportsStreamContexts
#undef  FltSupportsStreamContexts
#endif
#define FltSupportsStreamContexts sdv_FltSupportsStreamContexts

#ifdef  FltSupportsStreamHandleContexts
#undef  FltSupportsStreamHandleContexts
#endif
#define FltSupportsStreamHandleContexts sdv_FltSupportsStreamHandleContexts

#ifdef  FltAllocateContext
#undef  FltAllocateContext
#endif
#define FltAllocateContext sdv_FltAllocateContext

#ifdef  FltGetContexts
#undef  FltGetContexts
#endif
#define FltGetContexts sdv_FltGetContexts

#ifdef  FltReleaseContexts
#undef  FltReleaseContexts
#endif
#define FltReleaseContexts sdv_FltReleaseContexts

#ifdef  FltSetVolumeContext
#undef  FltSetVolumeContext
#endif
#define FltSetVolumeContext sdv_FltSetVolumeContext

#ifdef  FltSetInstanceContext
#undef  FltSetInstanceContext
#endif
#define FltSetInstanceContext sdv_FltSetInstanceContext

#ifdef  FltSetFileContext
#undef  FltSetFileContext
#endif
#define FltSetFileContext sdv_FltSetFileContext

#ifdef  FltSetStreamContext
#undef  FltSetStreamContext
#endif
#define FltSetStreamContext sdv_FltSetStreamContext

#ifdef  FltSetStreamHandleContext
#undef  FltSetStreamHandleContext
#endif
#define FltSetStreamHandleContext sdv_FltSetStreamHandleContext

#ifdef  FltSetTransactionContext
#undef  FltSetTransactionContext
#endif
#define FltSetTransactionContext sdv_FltSetTransactionContext

#ifdef  FltDeleteContext
#undef  FltDeleteContext
#endif
#define FltDeleteContext sdv_FltDeleteContext

#ifdef  FltDeleteVolumeContext
#undef  FltDeleteVolumeContext
#endif
#define FltDeleteVolumeContext sdv_FltDeleteVolumeContext

#ifdef  FltDeleteInstanceContext
#undef  FltDeleteInstanceContext
#endif
#define FltDeleteInstanceContext sdv_FltDeleteInstanceContext

#ifdef  FltDeleteFileContext
#undef  FltDeleteFileContext
#endif
#define FltDeleteFileContext sdv_FltDeleteFileContext

#ifdef  FltDeleteStreamContext
#undef  FltDeleteStreamContext
#endif
#define FltDeleteStreamContext sdv_FltDeleteStreamContext

#ifdef  FltDeleteStreamHandleContext
#undef  FltDeleteStreamHandleContext
#endif
#define FltDeleteStreamHandleContext sdv_FltDeleteStreamHandleContext

#ifdef  FltDeleteTransactionContext
#undef  FltDeleteTransactionContext
#endif
#define FltDeleteTransactionContext sdv_FltDeleteTransactionContext

#ifdef  FltGetVolumeContext
#undef  FltGetVolumeContext
#endif
#define FltGetVolumeContext sdv_FltGetVolumeContext

#ifdef  FltGetInstanceContext
#undef  FltGetInstanceContext
#endif
#define FltGetInstanceContext sdv_FltGetInstanceContext

#ifdef  FltGetFileContext
#undef  FltGetFileContext
#endif
#define FltGetFileContext sdv_FltGetFileContext

#ifdef  FltGetStreamContext
#undef  FltGetStreamContext
#endif
#define FltGetStreamContext sdv_FltGetStreamContext

#ifdef  FltGetStreamHandleContext
#undef  FltGetStreamHandleContext
#endif
#define FltGetStreamHandleContext sdv_FltGetStreamHandleContext

#ifdef  FltGetTransactionContext
#undef  FltGetTransactionContext
#endif
#define FltGetTransactionContext sdv_FltGetTransactionContext

#ifdef  FltReleaseContext
#undef  FltReleaseContext
#endif
#define FltReleaseContext sdv_FltReleaseContext

#ifdef  FltReferenceContext
#undef  FltReferenceContext
#endif
#define FltReferenceContext sdv_FltReferenceContext

#ifdef  FltReleaseContext
#undef  FltReleaseContext
#endif
#define FltReleaseContext sdv_FltReleaseContext

#ifdef  FltGetFilterFromName
#undef  FltGetFilterFromName
#endif
#define FltGetFilterFromName sdv_FltGetFilterFromName

#ifdef  FltGetVolumeFromName
#undef  FltGetVolumeFromName
#endif
#define FltGetVolumeFromName sdv_FltGetVolumeFromName

#ifdef  FltGetVolumeInstanceFromName
#undef  FltGetVolumeInstanceFromName
#endif
#define FltGetVolumeInstanceFromName sdv_FltGetVolumeInstanceFromName

#ifdef  FltGetVolumeFromInstance
#undef  FltGetVolumeFromInstance
#endif
#define FltGetVolumeFromInstance sdv_FltGetVolumeFromInstance

#ifdef  FltGetFilterFromInstance
#undef  FltGetFilterFromInstance
#endif
#define FltGetFilterFromInstance sdv_FltGetFilterFromInstance

#ifdef  FltGetVolumeFromFileObject
#undef  FltGetVolumeFromFileObject
#endif
#define FltGetVolumeFromFileObject sdv_FltGetVolumeFromFileObject

#ifdef  FltGetVolumeFromDeviceObject
#undef  FltGetVolumeFromDeviceObject
#endif
#define FltGetVolumeFromDeviceObject sdv_FltGetVolumeFromDeviceObject

#ifdef  FltIsFltMgrVolumeDeviceObject
#undef  FltIsFltMgrVolumeDeviceObject
#endif
#define FltIsFltMgrVolumeDeviceObject sdv_FltIsFltMgrVolumeDeviceObject

#ifdef  FltGetDeviceObject
#undef  FltGetDeviceObject
#endif
#define FltGetDeviceObject sdv_FltGetDeviceObject

#ifdef  FltGetDiskDeviceObject
#undef  FltGetDiskDeviceObject
#endif
#define FltGetDiskDeviceObject sdv_FltGetDiskDeviceObject

#ifdef  FltGetLowerInstance
#undef  FltGetLowerInstance
#endif
#define FltGetLowerInstance sdv_FltGetLowerInstance

#ifdef  FltGetUpperInstance
#undef  FltGetUpperInstance
#endif
#define FltGetUpperInstance sdv_FltGetUpperInstance

#ifdef  FltGetTopInstance
#undef  FltGetTopInstance
#endif
#define FltGetTopInstance sdv_FltGetTopInstance

#ifdef  FltGetBottomInstance
#undef  FltGetBottomInstance
#endif
#define FltGetBottomInstance sdv_FltGetBottomInstance

#ifdef  FltCompareInstanceAltitudes
#undef  FltCompareInstanceAltitudes
#endif
#define FltCompareInstanceAltitudes sdv_FltCompareInstanceAltitudes

#ifdef  FltGetFilterInformation
#undef  FltGetFilterInformation
#endif
#define FltGetFilterInformation sdv_FltGetFilterInformation

#ifdef  FltGetInstanceInformation
#undef  FltGetInstanceInformation
#endif
#define FltGetInstanceInformation sdv_FltGetInstanceInformation

#ifdef  FltGetVolumeInformation
#undef  FltGetVolumeInformation
#endif
#define FltGetVolumeInformation sdv_FltGetVolumeInformation

#ifdef  FltGetVolumeProperties
#undef  FltGetVolumeProperties
#endif
#define FltGetVolumeProperties sdv_FltGetVolumeProperties

#ifdef  FltIsVolumeWritable
#undef  FltIsVolumeWritable
#endif
#define FltIsVolumeWritable sdv_FltIsVolumeWritable

#ifdef  FltGetFileSystemType
#undef  FltGetFileSystemType
#endif
#define FltGetFileSystemType sdv_FltGetFileSystemType

#ifdef  FltIsVolumeSnapshot
#undef  FltIsVolumeSnapshot
#endif
#define FltIsVolumeSnapshot sdv_FltIsVolumeSnapshot

#ifdef  FltGetVolumeGuidName
#undef  FltGetVolumeGuidName
#endif
#define FltGetVolumeGuidName sdv_FltGetVolumeGuidName

#ifdef  FltQueryVolumeInformation
#undef  FltQueryVolumeInformation
#endif
#define FltQueryVolumeInformation sdv_FltQueryVolumeInformation

#ifdef  FltSetVolumeInformation
#undef  FltSetVolumeInformation
#endif
#define FltSetVolumeInformation sdv_FltSetVolumeInformation

#ifdef  FltEnumerateFilters
#undef  FltEnumerateFilters
#endif
#define FltEnumerateFilters sdv_FltEnumerateFilters

#ifdef  FltEnumerateVolumes
#undef  FltEnumerateVolumes
#endif
#define FltEnumerateVolumes sdv_FltEnumerateVolumes

#ifdef  FltEnumerateInstances
#undef  FltEnumerateInstances
#endif
#define FltEnumerateInstances sdv_FltEnumerateInstances

#ifdef  FltEnumerateFilterInformation
#undef  FltEnumerateFilterInformation
#endif
#define FltEnumerateFilterInformation sdv_FltEnumerateFilterInformation

#ifdef  FltEnumerateInstanceInformationByFilter
#undef  FltEnumerateInstanceInformationByFilter
#endif
#define FltEnumerateInstanceInformationByFilter sdv_FltEnumerateInstanceInformationByFilter

#ifdef  FltEnumerateInstanceInformationByVolume
#undef  FltEnumerateInstanceInformationByVolume
#endif
#define FltEnumerateInstanceInformationByVolume sdv_FltEnumerateInstanceInformationByVolume

#ifdef  FltEnumerateVolumeInformation
#undef  FltEnumerateVolumeInformation
#endif
#define FltEnumerateVolumeInformation sdv_FltEnumerateVolumeInformation

#ifdef  FltObjectReference
#undef  FltObjectReference
#endif
#define FltObjectReference sdv_FltObjectReference

#ifdef  FltObjectDereference
#undef  FltObjectDereference
#endif
#define FltObjectDereference sdv_FltObjectDereference

#ifdef  FltCreateCommunicationPort
#undef  FltCreateCommunicationPort
#endif
#define FltCreateCommunicationPort sdv_FltCreateCommunicationPort

#ifdef  FltCloseCommunicationPort
#undef  FltCloseCommunicationPort
#endif
#define FltCloseCommunicationPort sdv_FltCloseCommunicationPort

#ifdef  FltCloseClientPort
#undef  FltCloseClientPort
#endif
#define FltCloseClientPort sdv_FltCloseClientPort

#ifdef  FltSendMessage
#undef  FltSendMessage
#endif
#define FltSendMessage sdv_FltSendMessage

#ifdef  FltBuildDefaultSecurityDescriptor
#undef  FltBuildDefaultSecurityDescriptor
#endif
#define FltBuildDefaultSecurityDescriptor sdv_FltBuildDefaultSecurityDescriptor

#ifdef  FltFreeSecurityDescriptor
#undef  FltFreeSecurityDescriptor
#endif
#define FltFreeSecurityDescriptor sdv_FltFreeSecurityDescriptor

#ifdef  FltCancelIo
#undef  FltCancelIo
#endif
#define FltCancelIo sdv_FltCancelIo

#ifdef  FltSetCancelCompletion
#undef  FltSetCancelCompletion
#endif
#define FltSetCancelCompletion sdv_FltSetCancelCompletion

#ifdef  FltClearCancelCompletion
#undef  FltClearCancelCompletion
#endif
#define FltClearCancelCompletion sdv_FltClearCancelCompletion

#ifdef  FltIsIoCanceled
#undef  FltIsIoCanceled
#endif
#define FltIsIoCanceled sdv_FltIsIoCanceled

#ifdef  FltAllocateDeferredIoWorkItem
#undef  FltAllocateDeferredIoWorkItem
#endif
#define FltAllocateDeferredIoWorkItem sdv_FltAllocateDeferredIoWorkItem

#ifdef  FltFreeDeferredIoWorkItem
#undef  FltFreeDeferredIoWorkItem
#endif
#define FltFreeDeferredIoWorkItem sdv_FltFreeDeferredIoWorkItem



#ifdef  FltAllocateGenericWorkItem
#undef  FltAllocateGenericWorkItem
#endif
#define FltAllocateGenericWorkItem sdv_FltAllocateGenericWorkItem

#ifdef  FltFreeGenericWorkItem
#undef  FltFreeGenericWorkItem
#endif
#define FltFreeGenericWorkItem sdv_FltFreeGenericWorkItem


#ifdef  FltQueueDeferredIoWorkItem
#undef  FltQueueDeferredIoWorkItem
#endif
#define FltQueueDeferredIoWorkItem sdv_FltQueueDeferredIoWorkItem

#ifdef  FltQueueGenericWorkItem
#undef  FltQueueGenericWorkItem
#endif
#define FltQueueGenericWorkItem sdv_FltQueueGenericWorkItem

#ifdef  FltLockUserBuffer
#undef  FltLockUserBuffer
#endif
#define FltLockUserBuffer sdv_FltLockUserBuffer

#ifdef  FltDecodeParameters
#undef  FltDecodeParameters
#endif
#define FltDecodeParameters sdv_FltDecodeParameters

#ifdef  FltGetSwappedBufferMdlAddress
#undef  FltGetSwappedBufferMdlAddress
#endif
#define FltGetSwappedBufferMdlAddress sdv_FltGetSwappedBufferMdlAddress

#ifdef  FltRetainSwappedBufferMdlAddress
#undef  FltRetainSwappedBufferMdlAddress
#endif
#define FltRetainSwappedBufferMdlAddress sdv_FltRetainSwappedBufferMdlAddress

#ifdef  FltGetNewSystemBufferAddress
#undef  FltGetNewSystemBufferAddress
#endif
#define FltGetNewSystemBufferAddress sdv_FltGetNewSystemBufferAddress

#ifdef  FltCbdqInitialize
#undef  FltCbdqInitialize
#endif
#define FltCbdqInitialize sdv_FltCbdqInitialize

#ifdef  FltCbdqEnable
#undef  FltCbdqEnable
#endif
#define FltCbdqEnable sdv_FltCbdqEnable

#ifdef  FltCbdqDisable
#undef  FltCbdqDisable
#endif
#define FltCbdqDisable sdv_FltCbdqDisable

#ifdef  FltCbdqInsertIo
#undef  FltCbdqInsertIo
#endif
#define FltCbdqInsertIo sdv_FltCbdqInsertIo

#ifdef  FltCbdqRemoveIo
#undef  FltCbdqRemoveIo
#endif
#define FltCbdqRemoveIo sdv_FltCbdqRemoveIo

#ifdef  FltCbdqRemoveNextIo
#undef  FltCbdqRemoveNextIo
#endif
#define FltCbdqRemoveNextIo sdv_FltCbdqRemoveNextIo

#ifdef  FltInitializeOplock
#undef  FltInitializeOplock
#endif
#define FltInitializeOplock sdv_FltInitializeOplock

#ifdef  FltUninitializeOplock
#undef  FltUninitializeOplock
#endif
#define FltUninitializeOplock sdv_FltUninitializeOplock

#ifdef  FltOplockFsctrl
#undef  FltOplockFsctrl
#endif
#define FltOplockFsctrl sdv_FltOplockFsctrl

#ifdef  FltCheckOplock
#undef  FltCheckOplock
#endif
#define FltCheckOplock sdv_FltCheckOplock

#ifdef  FltOplockIsFastIoPossible
#undef  FltOplockIsFastIoPossible
#endif
#define FltOplockIsFastIoPossible sdv_FltOplockIsFastIoPossible

#ifdef  FltCurrentBatchOplock
#undef  FltCurrentBatchOplock
#endif
#define FltCurrentBatchOplock sdv_FltCurrentBatchOplock

#ifdef  FltCheckOplockEx
#undef  FltCheckOplockEx
#endif
#define FltCheckOplockEx sdv_FltCheckOplockEx

#ifdef  FltCurrentOplock
#undef  FltCurrentOplock
#endif
#define FltCurrentOplock sdv_FltCurrentOplock

#ifdef  FltCurrentOplockH
#undef  FltCurrentOplockH
#endif
#define FltCurrentOplockH sdv_FltCurrentOplockH

#ifdef  FltOplockBreakH
#undef  FltOplockBreakH
#endif
#define FltOplockBreakH sdv_FltOplockBreakH

#ifdef  FltOplockBreakToNone
#undef  FltOplockBreakToNone
#endif
#define FltOplockBreakToNone sdv_FltOplockBreakToNone

#ifdef  FltOplockBreakToNoneEx
#undef  FltOplockBreakToNoneEx
#endif
#define FltOplockBreakToNoneEx sdv_FltOplockBreakToNoneEx

#ifdef  FltOplockIsSharedRequest
#undef  FltOplockIsSharedRequest
#endif
#define FltOplockIsSharedRequest sdv_FltOplockIsSharedRequest

#ifdef  FltOplockFsctrlEx
#undef  FltOplockFsctrlEx
#endif
#define FltOplockFsctrlEx sdv_FltOplockFsctrlEx

#ifdef  FltOplockKeysEqual
#undef  FltOplockKeysEqual
#endif
#define FltOplockKeysEqual sdv_FltOplockKeysEqual

#ifdef  FltInitializeFileLock
#undef  FltInitializeFileLock
#endif
#define FltInitializeFileLock sdv_FltInitializeFileLock

#ifdef  FltUninitializeFileLock
#undef  FltUninitializeFileLock
#endif
#define FltUninitializeFileLock sdv_FltUninitializeFileLock

#ifdef  FltAllocateFileLock
#undef  FltAllocateFileLock
#endif
#define FltAllocateFileLock sdv_FltAllocateFileLock

#ifdef  FltFreeFileLock
#undef  FltFreeFileLock
#endif
#define FltFreeFileLock sdv_FltFreeFileLock

#ifdef  FltProcessFileLock
#undef  FltProcessFileLock
#endif
#define FltProcessFileLock sdv_FltProcessFileLock

#ifdef  FltCheckLockForReadAccess
#undef  FltCheckLockForReadAccess
#endif
#define FltCheckLockForReadAccess sdv_FltCheckLockForReadAccess

#ifdef  FltCheckLockForWriteAccess
#undef  FltCheckLockForWriteAccess
#endif
#define FltCheckLockForWriteAccess sdv_FltCheckLockForWriteAccess

#ifdef  FltAcquireResourceExclusive
#undef  FltAcquireResourceExclusive
#endif
#define FltAcquireResourceExclusive sdv_FltAcquireResourceExclusive

#ifdef  FltAcquireResourceShared
#undef  FltAcquireResourceShared
#endif
#define FltAcquireResourceShared sdv_FltAcquireResourceShared

#ifdef  FltReleaseResource
#undef  FltReleaseResource
#endif
#define FltReleaseResource sdv_FltReleaseResource

#ifdef  FltInitializePushLock
#undef  FltInitializePushLock
#endif
#define FltInitializePushLock sdv_FltInitializePushLock

#ifdef  FltDeletePushLock
#undef  FltDeletePushLock
#endif
#define FltDeletePushLock sdv_FltDeletePushLock

#ifdef  FltAcquirePushLockExclusive
#undef  FltAcquirePushLockExclusive
#endif
#define FltAcquirePushLockExclusive sdv_FltAcquirePushLockExclusive

#ifdef  FltAcquirePushLockShared
#undef  FltAcquirePushLockShared
#endif
#define FltAcquirePushLockShared sdv_FltAcquirePushLockShared

#ifdef  FltReleasePushLock
#undef  FltReleasePushLock
#endif
#define FltReleasePushLock sdv_FltReleasePushLock

#ifdef  FltCancellableWaitForSingleObject
#undef  FltCancellableWaitForSingleObject
#endif
#define FltCancellableWaitForSingleObject sdv_FltCancellableWaitForSingleObject

#ifdef  FltCancellableWaitForMultipleObjects
#undef  FltCancellableWaitForMultipleObjects
#endif
#define FltCancellableWaitForMultipleObjects sdv_FltCancellableWaitForMultipleObjects

#ifdef  FltIsOperationSynchronous
#undef  FltIsOperationSynchronous
#endif
#define FltIsOperationSynchronous sdv_FltIsOperationSynchronous

#ifdef  FltIs32bitProcess
#undef  FltIs32bitProcess
#endif
#define FltIs32bitProcess sdv_FltIs32bitProcess

#ifdef  FltGetRequestorProcess
#undef  FltGetRequestorProcess
#endif
#define FltGetRequestorProcess sdv_FltGetRequestorProcess

#ifdef  FltGetRequestorProcessId
#undef  FltGetRequestorProcessId
#endif
#define FltGetRequestorProcessId sdv_FltGetRequestorProcessId

#ifdef  FltGetRequestorProcessIdEx
#undef  FltGetRequestorProcessIdEx
#endif
#define FltGetRequestorProcessIdEx sdv_FltGetRequestorProcessIdEx

#ifdef  FltNotifyFilterChangeDirectory
#undef  FltNotifyFilterChangeDirectory
#endif
#define FltNotifyFilterChangeDirectory sdv_FltNotifyFilterChangeDirectory

#ifdef  FltGetRequestorSessionId
#undef  FltGetRequestorSessionId
#endif
#define FltGetRequestorSessionId sdv_FltGetRequestorSessionId

#ifdef  FltAdjustDeviceStackSizeForIoRedirection
#undef  FltAdjustDeviceStackSizeForIoRedirection
#endif
#define FltAdjustDeviceStackSizeForIoRedirection sdv_FltAdjustDeviceStackSizeForIoRedirection

#ifdef  FltIsIoRedirectionAllowed
#undef  FltIsIoRedirectionAllowed
#endif
#define FltIsIoRedirectionAllowed sdv_FltIsIoRedirectionAllowed

#ifdef  FltIsIoRedirectionAllowedForOperation
#undef  FltIsIoRedirectionAllowedForOperation
#endif
#define FltIsIoRedirectionAllowedForOperation sdv_FltIsIoRedirectionAllowedForOperation

#ifdef  FltEnlistInTransaction
#undef  FltEnlistInTransaction
#endif
#define FltEnlistInTransaction sdv_FltEnlistInTransaction

#ifdef  FltRollbackEnlistment
#undef  FltRollbackEnlistment
#endif
#define FltRollbackEnlistment sdv_FltRollbackEnlistment

#ifdef  FltPrePrepareComplete
#undef  FltPrePrepareComplete
#endif
#define FltPrePrepareComplete sdv_FltPrePrepareComplete

#ifdef  FltPrepareComplete
#undef  FltPrepareComplete
#endif
#define FltPrepareComplete sdv_FltPrepareComplete

#ifdef  FltCommitComplete
#undef  FltCommitComplete
#endif
#define FltCommitComplete sdv_FltCommitComplete

#ifdef  FltCommitFinalizeComplete
#undef  FltCommitFinalizeComplete
#endif
#define FltCommitFinalizeComplete sdv_FltCommitFinalizeComplete

#ifdef  FltRollbackComplete
#undef  FltRollbackComplete
#endif
#define FltRollbackComplete sdv_FltRollbackComplete

#ifdef  FltAllocateExtraCreateParameterList
#undef  FltAllocateExtraCreateParameterList
#endif
#define FltAllocateExtraCreateParameterList sdv_FltAllocateExtraCreateParameterList

#ifdef  FltAllocateExtraCreateParameter
#undef  FltAllocateExtraCreateParameter
#endif
#define FltAllocateExtraCreateParameter sdv_FltAllocateExtraCreateParameter

#ifdef  FltInitExtraCreateParameterLookasideList
#undef  FltInitExtraCreateParameterLookasideList
#endif
#define FltInitExtraCreateParameterLookasideList sdv_FltInitExtraCreateParameterLookasideList

#ifdef  FltDeleteExtraCreateParameterLookasideList
#undef  FltDeleteExtraCreateParameterLookasideList
#endif
#define FltDeleteExtraCreateParameterLookasideList sdv_FltDeleteExtraCreateParameterLookasideList

#ifdef  FltAllocateExtraCreateParameterFromLookasideList
#undef  FltAllocateExtraCreateParameterFromLookasideList
#endif
#define FltAllocateExtraCreateParameterFromLookasideList sdv_FltAllocateExtraCreateParameterFromLookasideList

#ifdef  FltInsertExtraCreateParameter
#undef  FltInsertExtraCreateParameter
#endif
#define FltInsertExtraCreateParameter sdv_FltInsertExtraCreateParameter

#ifdef  FltFindExtraCreateParameter
#undef  FltFindExtraCreateParameter
#endif
#define FltFindExtraCreateParameter sdv_FltFindExtraCreateParameter

#ifdef  FltRemoveExtraCreateParameter
#undef  FltRemoveExtraCreateParameter
#endif
#define FltRemoveExtraCreateParameter sdv_FltRemoveExtraCreateParameter

#ifdef  FltFreeExtraCreateParameterList
#undef  FltFreeExtraCreateParameterList
#endif
#define FltFreeExtraCreateParameterList sdv_FltFreeExtraCreateParameterList

#ifdef  FltFreeExtraCreateParameter
#undef  FltFreeExtraCreateParameter
#endif
#define FltFreeExtraCreateParameter sdv_FltFreeExtraCreateParameter

#ifdef  FltGetEcpListFromCallbackData
#undef  FltGetEcpListFromCallbackData
#endif
#define FltGetEcpListFromCallbackData sdv_FltGetEcpListFromCallbackData

#ifdef  FltSetEcpListIntoCallbackData
#undef  FltSetEcpListIntoCallbackData
#endif
#define FltSetEcpListIntoCallbackData sdv_FltSetEcpListIntoCallbackData

#ifdef  FltGetNextExtraCreateParameter
#undef  FltGetNextExtraCreateParameter
#endif
#define FltGetNextExtraCreateParameter sdv_FltGetNextExtraCreateParameter

#ifdef  FltAcknowledgeEcp
#undef  FltAcknowledgeEcp
#endif
#define FltAcknowledgeEcp sdv_FltAcknowledgeEcp

#ifdef  FltIsEcpAcknowledged
#undef  FltIsEcpAcknowledged
#endif
#define FltIsEcpAcknowledged sdv_FltIsEcpAcknowledged

#ifdef  FltIsEcpFromUserMode
#undef  FltIsEcpFromUserMode
#endif
#define FltIsEcpFromUserMode sdv_FltIsEcpFromUserMode

#ifdef  FltRetrieveIoPriorityInfo
#undef  FltRetrieveIoPriorityInfo
#endif
#define FltRetrieveIoPriorityInfo sdv_FltRetrieveIoPriorityInfo

#ifdef  FltApplyPriorityInfoThread
#undef  FltApplyPriorityInfoThread
#endif
#define FltApplyPriorityInfoThread sdv_FltApplyPriorityInfoThread

#ifdef  FltGetIoPriorityHint
#undef  FltGetIoPriorityHint
#endif
#define FltGetIoPriorityHint sdv_FltGetIoPriorityHint

#ifdef  FltGetIoPriorityHintFromCallbackData
#undef  FltGetIoPriorityHintFromCallbackData
#endif
#define FltGetIoPriorityHintFromCallbackData sdv_FltGetIoPriorityHintFromCallbackData

#ifdef  FltSetIoPriorityHintIntoCallbackData
#undef  FltSetIoPriorityHintIntoCallbackData
#endif
#define FltSetIoPriorityHintIntoCallbackData sdv_FltSetIoPriorityHintIntoCallbackData

#ifdef  FltGetIoPriorityHintFromFileObject
#undef  FltGetIoPriorityHintFromFileObject
#endif
#define FltGetIoPriorityHintFromFileObject sdv_FltGetIoPriorityHintFromFileObject

#ifdef  FltSetIoPriorityHintIntoFileObject
#undef  FltSetIoPriorityHintIntoFileObject
#endif
#define FltSetIoPriorityHintIntoFileObject sdv_FltSetIoPriorityHintIntoFileObject

#ifdef  FltGetIoPriorityHintFromThread
#undef  FltGetIoPriorityHintFromThread
#endif
#define FltGetIoPriorityHintFromThread sdv_FltGetIoPriorityHintFromThread

#ifdef  FltSetIoPriorityHintIntoThread
#undef  FltSetIoPriorityHintIntoThread
#endif
#define FltSetIoPriorityHintIntoThread sdv_FltSetIoPriorityHintIntoThread

#ifdef  FltGetIrpName
#undef  FltGetIrpName
#endif
#define FltGetIrpName sdv_FltGetIrpName





PIRP  sdv_IoGetTopLevelIrp(    VOID    );


VOID
FLTAPI
sdv_FltSetCallbackDataDirty(
    __inout PFLT_CALLBACK_DATA Data
    );

VOID
FLTAPI
sdv_FltClearCallbackDataDirty(
    __inout PFLT_CALLBACK_DATA Data
    );

BOOLEAN
FLTAPI
sdv_FltIsCallbackDataDirty(
    __in PFLT_CALLBACK_DATA Data
    );




BOOLEAN
FLTAPI
sdv_FltDoCompletionProcessingWhenSafe(
    __in PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in_opt PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags,
    __in PFLT_POST_OPERATION_CALLBACK SafePostCallback,
    __out PFLT_POSTOP_CALLBACK_STATUS RetPostOperationStatus
    );

NTSTATUS
FLTAPI
sdv_FltCheckAndGrowNameControl (
    __inout PFLT_NAME_CONTROL NameCtrl,
    __in USHORT NewSize
    );

NTSTATUS
FLTAPI
sdv_FltPurgeFileNameInformationCache (
    __in PFLT_INSTANCE Instance,
    __in_opt PFILE_OBJECT FileObject
    );




NTSTATUS
FLTAPI
sdv_FltRegisterFilter (
    __in PDRIVER_OBJECT Driver,
    __in CONST FLT_REGISTRATION *Registration,
    __deref_out PFLT_FILTER *RetFilter
    );


VOID
FLTAPI
sdv_FltUnregisterFilter (
    __in PFLT_FILTER Filter
    );


NTSTATUS
FLTAPI
sdv_FltStartFiltering (
    __in PFLT_FILTER Filter
    );


PVOID
FLTAPI
sdv_FltGetRoutineAddress (
    __in PCSTR FltMgrRoutineName
    );



VOID
FLTAPI
sdv_FltCompletePendedPreOperation (
    __in PFLT_CALLBACK_DATA CallbackData,
    __in FLT_PREOP_CALLBACK_STATUS CallbackStatus,
    __in_opt PVOID Context
    );


VOID
FLTAPI
sdv_FltCompletePendedPostOperation (
    __in PFLT_CALLBACK_DATA CallbackData
    );




NTSTATUS
FLTAPI
sdv_FltRequestOperationStatusCallback(
    __in PFLT_CALLBACK_DATA Data,
    __in PFLT_GET_OPERATION_STATUS_CALLBACK CallbackRoutine,
    __in_opt PVOID RequesterContext
    );






PVOID
FLTAPI
sdv_FltAllocatePoolAlignedWithTag (
    __in PFLT_INSTANCE Instance,
    __in POOL_TYPE PoolType,
    __in SIZE_T NumberOfBytes,
    __in ULONG Tag
    );


VOID
FLTAPI
sdv_FltFreePoolAlignedWithTag (
    __in PFLT_INSTANCE Instance,
    __in PVOID Buffer,
    __in ULONG Tag
    );

ULONG
  sdv_FltGetFileNameFormat(
    IN FLT_FILE_NAME_OPTIONS  NameOptions
    );

ULONG
  sdv_FltGetFileNameQueryMethod(
    IN FLT_FILE_NAME_OPTIONS  NameOptions
    ); 
	

NTSTATUS
FLTAPI
sdv_FltGetFileNameInformation (
    __in PFLT_CALLBACK_DATA CallbackData,
    __in FLT_FILE_NAME_OPTIONS NameOptions,
    __deref_out PFLT_FILE_NAME_INFORMATION *FileNameInformation
    );


NTSTATUS
FLTAPI
sdv_FltGetFileNameInformationUnsafe (
    __in PFILE_OBJECT FileObject,
    __in_opt PFLT_INSTANCE Instance,
    __in FLT_FILE_NAME_OPTIONS NameOptions,
    __deref_out PFLT_FILE_NAME_INFORMATION *FileNameInformation
    );


VOID
FLTAPI
sdv_FltReleaseFileNameInformation (
    __in PFLT_FILE_NAME_INFORMATION FileNameInformation
    );

 
VOID
FLTAPI
sdv_FltReferenceFileNameInformation (
    __in PFLT_FILE_NAME_INFORMATION FileNameInformation
    );


NTSTATUS
FLTAPI
sdv_FltParseFileName (
    __in PCUNICODE_STRING FileName,
    __inout_opt PUNICODE_STRING Extension,
    __inout_opt PUNICODE_STRING Stream,
    __inout_opt PUNICODE_STRING FinalComponent
    );


NTSTATUS
FLTAPI
sdv_FltParseFileNameInformation (
    __inout PFLT_FILE_NAME_INFORMATION FileNameInformation
    );


NTSTATUS
FLTAPI
sdv_FltGetTunneledName (
    __in PFLT_CALLBACK_DATA CallbackData,
    __in PFLT_FILE_NAME_INFORMATION FileNameInformation,
    __deref_out_opt PFLT_FILE_NAME_INFORMATION *RetTunneledFileNameInformation
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeName (
    __in PFLT_VOLUME Volume,
    __inout_opt PUNICODE_STRING VolumeName,
    __out_opt PULONG BufferSizeNeeded
    );


NTSTATUS
FLTAPI
sdv_FltGetDestinationFileNameInformation (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in_opt HANDLE RootDirectory,
    __in_bcount(FileNameLength) PWSTR FileName,
    __in ULONG FileNameLength,
    __in FLT_FILE_NAME_OPTIONS NameOptions,
    __deref_out PFLT_FILE_NAME_INFORMATION *RetFileNameInformation
    );


NTSTATUS
FLTAPI
sdv_FltIsDirectory (
    __in PFILE_OBJECT FileObject,
    __in PFLT_INSTANCE Instance,
    __out PBOOLEAN IsDirectory
    );




NTSTATUS
FLTAPI
sdv_FltLoadFilter (
    __in PCUNICODE_STRING FilterName
    );


NTSTATUS
FLTAPI
sdv_FltUnloadFilter (
    __in PCUNICODE_STRING FilterName
    );


NTSTATUS
FLTAPI
sdv_FltAttachVolume (
    __inout PFLT_FILTER Filter,
    __inout PFLT_VOLUME Volume,
    __in_opt PCUNICODE_STRING InstanceName,
    __deref_opt_out_opt PFLT_INSTANCE *RetInstance
    );


NTSTATUS
FLTAPI
sdv_FltAttachVolumeAtAltitude (
    __inout PFLT_FILTER Filter,
    __inout PFLT_VOLUME Volume,
    __in PCUNICODE_STRING Altitude,
    __in_opt PCUNICODE_STRING InstanceName,
    __deref_opt_out_opt PFLT_INSTANCE *RetInstance
    );


NTSTATUS
FLTAPI
sdv_FltDetachVolume (
    __inout PFLT_FILTER Filter,
    __inout PFLT_VOLUME Volume,
    __in_opt PCUNICODE_STRING InstanceName
    );




NTSTATUS
FLTAPI
sdv_FltAllocateCallbackData (
    __in PFLT_INSTANCE Instance,
    __in_opt PFILE_OBJECT FileObject,
    __deref_out PFLT_CALLBACK_DATA *RetNewCallbackData
    );

#if FLT_MGR_WIN7
NTSTATUS
FLTAPI
sdv_FltAllocateCallbackDataEx (
    __in PFLT_INSTANCE Instance,
    __in_opt PFILE_OBJECT FileObject,
    __in FLT_ALLOCATE_CALLBACK_DATA_FLAGS Flags,
    __deref_out PFLT_CALLBACK_DATA *RetNewCallbackData
    );
#endif



VOID
FLTAPI
sdv_FltFreeCallbackData(
    __in PFLT_CALLBACK_DATA CallbackData
    );


VOID
FLTAPI
sdv_FltReuseCallbackData (
    __inout PFLT_CALLBACK_DATA CallbackData
    );


VOID
FLTAPI
sdv_FltPerformSynchronousIo (
    __inout PFLT_CALLBACK_DATA CallbackData
    );



NTSTATUS
FLTAPI
sdv_FltPerformAsynchronousIo (
    __inout PFLT_CALLBACK_DATA CallbackData,
    __in PFLT_COMPLETED_ASYNC_IO_CALLBACK CallbackRoutine,
    __in PVOID CallbackContext
    );



NTSTATUS
FLTAPI
sdv_FltCreateFileEx2 (
    __in PFLT_FILTER Filter,
    __in_opt PFLT_INSTANCE Instance,
    __out PHANDLE FileHandle,
    __deref_opt_out PFILE_OBJECT *FileObject,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __out PIO_STATUS_BLOCK IoStatusBlock,
    __in_opt PLARGE_INTEGER AllocationSize,
    __in ULONG FileAttributes,
    __in ULONG ShareAccess,
    __in ULONG CreateDisposition,
    __in ULONG CreateOptions,
    __in_bcount_opt(EaLength) PVOID EaBuffer,
    __in ULONG EaLength,
    __in ULONG Flags,
    __in_opt PIO_DRIVER_CREATE_CONTEXT DriverContext
    );


NTSTATUS
FLTAPI
sdv_FltCreateFileEx (
    __in PFLT_FILTER Filter,
    __in_opt PFLT_INSTANCE Instance,
    __out PHANDLE FileHandle,
    __deref_opt_out PFILE_OBJECT *FileObject,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __out PIO_STATUS_BLOCK IoStatusBlock,
    __in_opt PLARGE_INTEGER AllocationSize,
    __in ULONG FileAttributes,
    __in ULONG ShareAccess,
    __in ULONG CreateDisposition,
    __in ULONG CreateOptions,
    __in_bcount_opt(EaLength) PVOID EaBuffer,
    __in ULONG EaLength,
    __in ULONG Flags
    );

NTSTATUS
FLTAPI
sdv_FltCreateFile (
    __in PFLT_FILTER Filter,
    __in_opt PFLT_INSTANCE Instance,
    __out PHANDLE   FileHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __out PIO_STATUS_BLOCK IoStatusBlock,
    __in_opt PLARGE_INTEGER AllocationSize,
    __in ULONG FileAttributes,
    __in ULONG ShareAccess,
    __in ULONG CreateDisposition,
    __in ULONG CreateOptions,
    __in_bcount_opt(EaLength)PVOID EaBuffer,
    __in ULONG EaLength,
    __in ULONG Flags
    );


NTSTATUS
FLTAPI
sdv_FltOpenVolume (
    __in PFLT_INSTANCE Instance,
    __out PHANDLE VolumeHandle,
    __deref_opt_out PFILE_OBJECT *VolumeFileObject
    );


NTSTATUS
FLTAPI
sdv_FltReadFile (
    __in PFLT_INSTANCE InitiatingInstance,
    __in PFILE_OBJECT FileObject,
    __in_opt PLARGE_INTEGER ByteOffset,
    __in ULONG Length,
    __out_bcount_part(Length,*BytesRead) PVOID Buffer,
    __in FLT_IO_OPERATION_FLAGS Flags,
    __out_opt PULONG BytesRead,
    __in_opt PFLT_COMPLETED_ASYNC_IO_CALLBACK CallbackRoutine,
    __in_opt PVOID CallbackContext
    );


NTSTATUS
FLTAPI
sdv_FltTagFile (
    __in PFLT_INSTANCE InitiatingInstance,
    __in PFILE_OBJECT FileObject,
    __in ULONG FileTag,
    __in_opt GUID *Guid,
    __in_bcount(DataBufferLength) PVOID DataBuffer,
    __in USHORT DataBufferLength
    );


NTSTATUS
FLTAPI
sdv_FltUntagFile(
    __in PFLT_INSTANCE InitiatingInstance,
    __in PFILE_OBJECT FileObject,
    __in ULONG FileTag,
    __in_opt GUID *Guid
    );


NTSTATUS
FLTAPI
sdv_FltWriteFile (
    __in PFLT_INSTANCE InitiatingInstance,
    __in PFILE_OBJECT FileObject,
    __in_opt PLARGE_INTEGER ByteOffset,
    __in ULONG Length,
    __in_bcount(Length) PVOID Buffer,
    __in FLT_IO_OPERATION_FLAGS Flags,
    __out_opt PULONG BytesWritten,
    __in_opt PFLT_COMPLETED_ASYNC_IO_CALLBACK CallbackRoutine,
    __in_opt PVOID CallbackContext
    );


NTSTATUS
FLTAPI
sdv_FltQueryInformationFile (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __out_bcount_part(Length,*LengthReturned) PVOID FileInformation,
    __in ULONG Length,
    __in FILE_INFORMATION_CLASS FileInformationClass,
    __out_opt PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltSetInformationFile (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in_bcount(Length) PVOID FileInformation,
    __in ULONG Length,
    __in FILE_INFORMATION_CLASS FileInformationClass
    );


NTSTATUS
FLTAPI
sdv_FltQueryDirectoryFile (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __out_bcount(Length) PVOID FileInformation,
    __in ULONG Length,
    __in FILE_INFORMATION_CLASS FileInformationClass,
    __in BOOLEAN ReturnSingleEntry,
    __in_opt PUNICODE_STRING FileName,
    __in BOOLEAN RestartScan,
    __out_opt PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltQueryEaFile(
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __out_bcount_part(Length,*LengthReturned) PVOID ReturnedEaData,
    __in ULONG Length,
    __in BOOLEAN ReturnSingleEntry,
    __in_bcount_opt(EaListLength) PVOID EaList,
    __in ULONG EaListLength,
    __in_opt PULONG EaIndex,
    __in BOOLEAN RestartScan,
    __out_opt PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltSetEaFile(
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in_bcount(Length) PVOID EaBuffer,
    __in ULONG Length
    );

NTSTATUS
FLTAPI
sdv_FltQueryVolumeInformationFile (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __out_bcount_part(Length,*LengthReturned) PVOID FsInformation,
    __in ULONG Length,
    __in FS_INFORMATION_CLASS FsInformationClass,
    __out_opt PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltQuerySecurityObject (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in SECURITY_INFORMATION SecurityInformation,
    __inout_bcount_opt(Length) PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in ULONG Length,
    __out_opt PULONG LengthNeeded
    );


NTSTATUS
FLTAPI
sdv_FltSetSecurityObject (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR SecurityDescriptor
    );


NTSTATUS
FLTAPI
sdv_FltFlushBuffers (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject
    );


NTSTATUS
FLTAPI
sdv_FltFsControlFile (
    __in PFLT_INSTANCE Instance,
    __in  PFILE_OBJECT FileObject,
    __in ULONG FsControlCode,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_part_opt(OutputBufferLength,*LengthReturned) PVOID OutputBuffer,
    __in ULONG OutputBufferLength,
    __out_opt PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltDeviceIoControlFile (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in ULONG IoControlCode,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_part_opt(OutputBufferLength,*LengthReturned) PVOID OutputBuffer,
    __in ULONG OutputBufferLength,
    __out_opt PULONG LengthReturned
    );


VOID
FLTAPI
sdv_FltReissueSynchronousIo (
   __in PFLT_INSTANCE InitiatingInstance,
   __in PFLT_CALLBACK_DATA CallbackData
   );


NTSTATUS
FLTAPI
sdv_FltClose(
   __in HANDLE FileHandle
   );


VOID
FLTAPI
sdv_FltCancelFileOpen (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject
    );


NTSTATUS
FLTAPI
sdv_FltCreateSystemVolumeInformationFolder (
    __in PFLT_INSTANCE Instance
    );






BOOLEAN
FLTAPI
sdv_FltSupportsFileContextsEx (
    __in PFILE_OBJECT FileObject,
    __in_opt PFLT_INSTANCE Instance
    );


BOOLEAN
FLTAPI
sdv_FltSupportsFileContexts (
    __in PFILE_OBJECT FileObject
    );


BOOLEAN
FLTAPI
sdv_FltSupportsStreamContexts (
    __in PFILE_OBJECT FileObject
    );


BOOLEAN
FLTAPI
sdv_FltSupportsStreamHandleContexts (
    __in PFILE_OBJECT FileObject
    );



NTSTATUS
FLTAPI
sdv_FltAllocateContext (
    __in PFLT_FILTER Filter,
    __in FLT_CONTEXT_TYPE ContextType,
    __in SIZE_T ContextSize,
    __in POOL_TYPE PoolType,
    __deref_out_bcount(ContextSize) PFLT_CONTEXT *ReturnedContext
    );


VOID
FLTAPI
sdv_FltGetContexts (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_CONTEXT_TYPE DesiredContexts,
    __out PFLT_RELATED_CONTEXTS Contexts
    );


VOID
FLTAPI
sdv_FltReleaseContexts (
    __in PFLT_RELATED_CONTEXTS Contexts
    );




NTSTATUS
FLTAPI
sdv_FltSetVolumeContext (
    __in PFLT_VOLUME Volume,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltSetInstanceContext (
    __in PFLT_INSTANCE Instance,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltSetFileContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltSetStreamContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltSetStreamHandleContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltSetTransactionContext (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in FLT_SET_CONTEXT_OPERATION Operation,
    __in PFLT_CONTEXT NewContext,
    __deref_opt_out PFLT_CONTEXT *OldContext
    );




VOID
FLTAPI
sdv_FltDeleteContext (
    __in PFLT_CONTEXT Context
    );


NTSTATUS
FLTAPI
sdv_FltDeleteVolumeContext (
    __in PFLT_FILTER Filter,
    __in PFLT_VOLUME Volume,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltDeleteInstanceContext (
    __in PFLT_INSTANCE Instance,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltDeleteFileContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltDeleteStreamContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );


NTSTATUS
FLTAPI
sdv_FltDeleteStreamHandleContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_opt_out_opt PFLT_CONTEXT *OldContext
    );



NTSTATUS
FLTAPI
sdv_FltDeleteTransactionContext (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __deref_opt_out PFLT_CONTEXT *OldContext
    );




NTSTATUS
FLTAPI
sdv_FltGetVolumeContext (
    __in PFLT_FILTER Filter,
    __in PFLT_VOLUME Volume,
    __deref_out PFLT_CONTEXT *Context
    );


NTSTATUS
FLTAPI
sdv_FltGetInstanceContext (
    __in PFLT_INSTANCE Instance,
    __deref_out PFLT_CONTEXT *Context
    );


NTSTATUS
FLTAPI
sdv_FltGetFileContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_out PFLT_CONTEXT *Context
    );


NTSTATUS
FLTAPI
sdv_FltGetStreamContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_out PFLT_CONTEXT *Context
    );


NTSTATUS
FLTAPI
sdv_FltGetStreamHandleContext (
    __in PFLT_INSTANCE Instance,
    __in PFILE_OBJECT FileObject,
    __deref_out PFLT_CONTEXT *Context
    );


NTSTATUS
FLTAPI
sdv_FltGetTransactionContext (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __deref_out PFLT_CONTEXT *Context
    );




VOID
FLTAPI
sdv_FltReferenceContext (
    __in PFLT_CONTEXT Context
    );


VOID
FLTAPI
sdv_FltReleaseContext (
    __in PFLT_CONTEXT Context
    );



NTSTATUS
FLTAPI
sdv_FltGetFilterFromName (
    __in PCUNICODE_STRING FilterName,
    __deref_out PFLT_FILTER *RetFilter
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeFromName (
    __in PFLT_FILTER Filter,
    __in PCUNICODE_STRING VolumeName,
    __deref_out PFLT_VOLUME *RetVolume
    );

NTSTATUS
FLTAPI
sdv_FltGetVolumeInstanceFromName (
    __in_opt PFLT_FILTER Filter,
    __in PFLT_VOLUME Volume,
    __in_opt PCUNICODE_STRING InstanceName,
    __deref_out PFLT_INSTANCE *RetInstance
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeFromInstance (
    __in PFLT_INSTANCE Instance,
    __deref_out PFLT_VOLUME *RetVolume
    );


NTSTATUS
FLTAPI
sdv_FltGetFilterFromInstance (
    __in PFLT_INSTANCE Instance,
    __deref_out PFLT_FILTER *RetFilter
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeFromFileObject (
    __in PFLT_FILTER Filter,
    __in PFILE_OBJECT FileObject,
    __deref_out PFLT_VOLUME *RetVolume
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeFromDeviceObject (
    __in PFLT_FILTER Filter,
    __in PDEVICE_OBJECT DeviceObject,
    __deref_out PFLT_VOLUME *RetVolume
    );


BOOLEAN
FLTAPI
sdv_FltIsFltMgrVolumeDeviceObject(
    __in PDEVICE_OBJECT DeviceObject
    );


NTSTATUS
FLTAPI
sdv_FltGetDeviceObject (
    __in PFLT_VOLUME Volume,
    __deref_out PDEVICE_OBJECT *DeviceObject
    );


NTSTATUS
FLTAPI
sdv_FltGetDiskDeviceObject(
    __in PFLT_VOLUME Volume,
    __deref_out PDEVICE_OBJECT *DiskDeviceObject
    );


NTSTATUS
FLTAPI
sdv_FltGetLowerInstance (
    __in PFLT_INSTANCE CurrentInstance,
    __deref_out PFLT_INSTANCE *LowerInstance
    );


NTSTATUS
FLTAPI
sdv_FltGetUpperInstance (
    __in PFLT_INSTANCE CurrentInstance,
    __deref_out PFLT_INSTANCE *UpperInstance
    );


NTSTATUS
FLTAPI
sdv_FltGetTopInstance (
    __in PFLT_VOLUME Volume,
    __deref_out PFLT_INSTANCE *Instance
    );


NTSTATUS
FLTAPI
sdv_FltGetBottomInstance (
    __in PFLT_VOLUME Volume,
    __deref_out PFLT_INSTANCE *Instance
    );

LONG
FLTAPI
sdv_FltCompareInstanceAltitudes (
    __in PFLT_INSTANCE Instance1,
    __in PFLT_INSTANCE Instance2
    );




NTSTATUS
FLTAPI
sdv_FltGetFilterInformation (
    __in PFLT_FILTER Filter,
    __in FILTER_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize, *BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );


NTSTATUS
FLTAPI
sdv_FltGetInstanceInformation (
    __in PFLT_INSTANCE Instance,
    __in INSTANCE_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeInformation (
    __in PFLT_VOLUME Volume,
    __in FILTER_VOLUME_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );



NTSTATUS
FLTAPI
sdv_FltGetVolumeProperties (
    __in PFLT_VOLUME Volume,
    __out_bcount_part_opt(VolumePropertiesLength,*LengthReturned) PFLT_VOLUME_PROPERTIES VolumeProperties,
    __in ULONG VolumePropertiesLength,
    __out PULONG LengthReturned
    );


NTSTATUS
FLTAPI
sdv_FltIsVolumeWritable (
    __in PVOID FltObject,
    __out PBOOLEAN IsWritable
    );


NTSTATUS
FLTAPI
sdv_FltGetFileSystemType (
    __in PVOID FltObject,
    __out PFLT_FILESYSTEM_TYPE FileSystemType
    );


NTSTATUS
FLTAPI
sdv_FltIsVolumeSnapshot (
    __in PVOID FltObject,
    __out PBOOLEAN IsSnapshotVolume
    );


NTSTATUS
FLTAPI
sdv_FltGetVolumeGuidName (
    __in PFLT_VOLUME Volume,
    __out PUNICODE_STRING VolumeGuidName,
    __out_opt PULONG BufferSizeNeeded
    );


NTSTATUS
FLTAPI
sdv_FltQueryVolumeInformation(
    __in PFLT_INSTANCE Instance,
    __out PIO_STATUS_BLOCK Iosb,
    __out_bcount(Length) PVOID FsInformation,
    __in ULONG Length,
    __in FS_INFORMATION_CLASS FsInformationClass
    );


NTSTATUS
FLTAPI
sdv_FltSetVolumeInformation(
    __in PFLT_INSTANCE Instance,
    __out PIO_STATUS_BLOCK Iosb,
    __out_bcount(Length) PVOID FsInformation,
    __in ULONG Length,
    __in FS_INFORMATION_CLASS FsInformationClass
    );



NTSTATUS
FLTAPI
sdv_FltEnumerateFilters (
    __out_ecount_part_opt(FilterListSize,*NumberFiltersReturned) PFLT_FILTER *FilterList,
    __in ULONG FilterListSize,
    __out PULONG NumberFiltersReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateVolumes (
    __in PFLT_FILTER Filter,
    __out_ecount_part_opt(VolumeListSize,*NumberVolumesReturned) PFLT_VOLUME *VolumeList,
    __in ULONG VolumeListSize,
    __out PULONG NumberVolumesReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateInstances (
    __in_opt PFLT_VOLUME Volume,
    __in_opt PFLT_FILTER Filter,
    __out_ecount_part_opt(InstanceListSize,*NumberInstancesReturned) PFLT_INSTANCE *InstanceList,
    __in ULONG InstanceListSize,
    __out PULONG NumberInstancesReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateFilterInformation (
    __in ULONG Index,
    __in FILTER_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateInstanceInformationByFilter (
    __in PFLT_FILTER Filter,
    __in ULONG Index,
    __in INSTANCE_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateInstanceInformationByVolume (
    __in PFLT_VOLUME Volume,
    __in ULONG Index,
    __in INSTANCE_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );


NTSTATUS
FLTAPI
sdv_FltEnumerateVolumeInformation (
    __in PFLT_FILTER Filter,
    __in ULONG Index,
    __in FILTER_VOLUME_INFORMATION_CLASS InformationClass,
    __out_bcount_part_opt(BufferSize,*BytesReturned) PVOID Buffer,
    __in ULONG BufferSize,
    __out PULONG BytesReturned
    );




NTSTATUS
FLTAPI
sdv_FltObjectReference (
    __inout PVOID FltObject
    );


VOID
FLTAPI
sdv_FltObjectDereference (
    __inout PVOID FltObject
    );


NTSTATUS
FLTAPI
sdv_FltCreateCommunicationPort (
    __in PFLT_FILTER Filter,
    __deref_out PFLT_PORT *ServerPort,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PVOID ServerPortCookie,
    __in PFLT_CONNECT_NOTIFY ConnectNotifyCallback,
    __in PFLT_DISCONNECT_NOTIFY DisconnectNotifyCallback,
    __in_opt PFLT_MESSAGE_NOTIFY MessageNotifyCallback,
    __in LONG MaxConnections
    );


VOID
FLTAPI
sdv_FltCloseCommunicationPort (
    __in PFLT_PORT ServerPort
    );


VOID
FLTAPI
sdv_FltCloseClientPort (
    __in PFLT_FILTER Filter,
    __deref_out PFLT_PORT *ClientPort
    );


NTSTATUS
FLTAPI
sdv_FltSendMessage (
    __in PFLT_FILTER Filter,
    __deref_in PFLT_PORT *ClientPort,
    __in_bcount(SenderBufferLength) PVOID SenderBuffer,
    __in ULONG SenderBufferLength,
    __out_bcount_opt(*ReplyLength) PVOID ReplyBuffer,
    __inout_opt PULONG ReplyLength,
    __in_opt PLARGE_INTEGER Timeout
    );


NTSTATUS
FLTAPI
sdv_FltBuildDefaultSecurityDescriptor(
     __deref_out PSECURITY_DESCRIPTOR *SecurityDescriptor,
     __in ACCESS_MASK DesiredAccess
     );


VOID
FLTAPI
sdv_FltFreeSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor
    );





BOOLEAN
FLTAPI
sdv_FltCancelIo(
    __in PFLT_CALLBACK_DATA CallbackData
    );


FLTAPI
sdv_FltSetCancelCompletion (
    __in PFLT_CALLBACK_DATA CallbackData,
    __in PFLT_COMPLETE_CANCELED_CALLBACK CanceledCallback
    );

NTSTATUS
FLTAPI
sdv_FltClearCancelCompletion(
    __in PFLT_CALLBACK_DATA CallbackData
    );

BOOLEAN
FLTAPI
sdv_FltIsIoCanceled(
    __in PFLT_CALLBACK_DATA CallbackData
    );



PFLT_DEFERRED_IO_WORKITEM
FLTAPI
sdv_FltAllocateDeferredIoWorkItem(
    VOID
    );


VOID
FLTAPI
sdv_FltFreeDeferredIoWorkItem (
    __in PFLT_DEFERRED_IO_WORKITEM FltWorkItem
    );


PFLT_GENERIC_WORKITEM
FLTAPI
sdv_FltAllocateGenericWorkItem(
    VOID
    );


VOID
FLTAPI
sdv_FltFreeGenericWorkItem (
    __in PFLT_GENERIC_WORKITEM FltWorkItem
    );


NTSTATUS
FLTAPI
sdv_FltQueueDeferredIoWorkItem (
    __in PFLT_DEFERRED_IO_WORKITEM FltWorkItem,
    __in PFLT_CALLBACK_DATA Data,
    __in PFLT_DEFERRED_IO_WORKITEM_ROUTINE WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in PVOID Context
    );


NTSTATUS
FLTAPI
sdv_FltQueueGenericWorkItem (
    __in PFLT_GENERIC_WORKITEM FltWorkItem,
    __in PVOID FltObject,
    __in PFLT_GENERIC_WORKITEM_ROUTINE WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt PVOID Context
    );





NTSTATUS
FLTAPI
sdv_FltLockUserBuffer(
    __in PFLT_CALLBACK_DATA CallbackData
    );

NTSTATUS
FLTAPI
sdv_FltDecodeParameters(
    __in PFLT_CALLBACK_DATA CallbackData,
    __deref_opt_out PMDL **MdlAddressPointer,
    __deref_opt_out_bcount(**Length) PVOID  **Buffer,
    __deref_opt_out PULONG *Length,
    __out_opt LOCK_OPERATION *DesiredAccess
    );

PMDL
FASTCALL
sdv_FltGetSwappedBufferMdlAddress(
    __in PFLT_CALLBACK_DATA CallbackData
    );

VOID
FASTCALL
sdv_FltRetainSwappedBufferMdlAddress(
    __in PFLT_CALLBACK_DATA CallbackData
    );


PVOID
FLTAPI
sdv_FltGetNewSystemBufferAddress(
    __in PFLT_CALLBACK_DATA CallbackData
    );



NTSTATUS
FLTAPI
sdv_FltCbdqInitialize(
    __in PFLT_INSTANCE                                       Instance,
    __inout PFLT_CALLBACK_DATA_QUEUE                         Cbdq,
    __in PFLT_CALLBACK_DATA_QUEUE_INSERT_IO                  CbdqInsertIo,
    __in PFLT_CALLBACK_DATA_QUEUE_REMOVE_IO                  CbdqRemoveIo,
    __in PFLT_CALLBACK_DATA_QUEUE_PEEK_NEXT_IO               CbdqPeekNextIo,
    __in PFLT_CALLBACK_DATA_QUEUE_ACQUIRE                    CbdqAcquire,
    __in PFLT_CALLBACK_DATA_QUEUE_RELEASE                    CbdqRelease,
    __in PFLT_CALLBACK_DATA_QUEUE_COMPLETE_CANCELED_IO       CbdqCompleteCanceledIo
    );

VOID
FLTAPI
sdv_FltCbdqEnable(
    __inout PFLT_CALLBACK_DATA_QUEUE Cbdq
    );

VOID
FLTAPI
sdv_FltCbdqDisable(
    __inout PFLT_CALLBACK_DATA_QUEUE Cbdq
    );


NTSTATUS
FLTAPI
sdv_FltCbdqInsertIo(
    __inout PFLT_CALLBACK_DATA_QUEUE Cbdq,
    __in PFLT_CALLBACK_DATA Cbd,
    __in_opt PFLT_CALLBACK_DATA_QUEUE_IO_CONTEXT Context,
    __in_opt PVOID InsertContext
    );


PFLT_CALLBACK_DATA
FLTAPI
sdv_FltCbdqRemoveIo(
    __inout PFLT_CALLBACK_DATA_QUEUE Cbdq,
    __in PFLT_CALLBACK_DATA_QUEUE_IO_CONTEXT Context
    );


PFLT_CALLBACK_DATA
FLTAPI
sdv_FltCbdqRemoveNextIo(
    __inout  PFLT_CALLBACK_DATA_QUEUE Cbdq,
    __in_opt  PVOID     PeekContext
    );





VOID
FLTAPI
sdv_FltInitializeOplock (
    __out POPLOCK Oplock
    );


VOID
FLTAPI
sdv_FltUninitializeOplock (
    __in POPLOCK Oplock
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltOplockFsctrl (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in ULONG OpenCount
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltCheckOplock (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in_opt PVOID Context,
    __in_opt PFLTOPLOCK_WAIT_COMPLETE_ROUTINE WaitCompletionRoutine,
    __in_opt PFLTOPLOCK_PREPOST_CALLBACKDATA_ROUTINE PrePostCallbackDataRoutine
    );


BOOLEAN
FLTAPI
sdv_FltOplockIsFastIoPossible (
    __in POPLOCK Oplock
    );

BOOLEAN
FLTAPI
sdv_FltCurrentBatchOplock (
    __in POPLOCK Oplock
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltCheckOplockEx (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in ULONG Flags,
    __in_opt PVOID Context,
    __in_opt PFLTOPLOCK_WAIT_COMPLETE_ROUTINE WaitCompletionRoutine,
    __in_opt PFLTOPLOCK_PREPOST_CALLBACKDATA_ROUTINE PrePostCallbackDataRoutine
    );


BOOLEAN
FLTAPI
sdv_FltCurrentOplock (
    __in POPLOCK Oplock
    );


BOOLEAN
FLTAPI
sdv_FltCurrentOplockH (
    __in POPLOCK Oplock
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltOplockBreakH (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in ULONG Flags,
    __in_opt PVOID Context,
    __in_opt PFLTOPLOCK_WAIT_COMPLETE_ROUTINE WaitCompletionRoutine,
    __in_opt PFLTOPLOCK_PREPOST_CALLBACKDATA_ROUTINE PrePostCallbackDataRoutine
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltOplockBreakToNone (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in_opt PVOID Context,
    __in_opt PFLTOPLOCK_WAIT_COMPLETE_ROUTINE WaitCompletionRoutine,
    __in_opt PFLTOPLOCK_PREPOST_CALLBACKDATA_ROUTINE PrePostCallbackDataRoutine
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltOplockBreakToNoneEx (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in ULONG Flags,
    __in_opt PVOID Context,
    __in_opt PFLTOPLOCK_WAIT_COMPLETE_ROUTINE WaitCompletionRoutine,
    __in_opt PFLTOPLOCK_PREPOST_CALLBACKDATA_ROUTINE PrePostCallbackDataRoutine
    );

BOOLEAN
FLTAPI
sdv_FltOplockIsSharedRequest (
    __in PFLT_CALLBACK_DATA CallbackData
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltOplockFsctrlEx (
    __in POPLOCK Oplock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in ULONG OpenCount,
    __in ULONG Flags
    );

BOOLEAN
FLTAPI
sdv_FltOplockKeysEqual (
    __in_opt PFILE_OBJECT Fo1,
    __in_opt PFILE_OBJECT Fo2
    );




VOID
FLTAPI
sdv_FltInitializeFileLock (
    __out PFILE_LOCK FileLock
    );

VOID
FLTAPI
sdv_FltUninitializeFileLock (
    __in PFILE_LOCK FileLock
    );


PFILE_LOCK
FLTAPI
sdv_FltAllocateFileLock (
    __in_opt PFLT_COMPLETE_LOCK_CALLBACK_DATA_ROUTINE CompleteLockCallbackDataRoutine,
    __in_opt PUNLOCK_ROUTINE UnlockRoutine
    );


VOID
FLTAPI
sdv_FltFreeFileLock (
    __in PFILE_LOCK FileLock
    );


FLT_PREOP_CALLBACK_STATUS
FLTAPI
sdv_FltProcessFileLock (
    __in PFILE_LOCK FileLock,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in_opt PVOID Context
    );


BOOLEAN
FLTAPI
sdv_FltCheckLockForReadAccess (
    __in PFILE_LOCK FileLock,
    __in PFLT_CALLBACK_DATA CallbackData
    );


BOOLEAN
FLTAPI
sdv_FltCheckLockForWriteAccess (
    __in PFILE_LOCK FileLock,
    __in PFLT_CALLBACK_DATA CallbackData
    );



VOID
FLTAPI
sdv_FltAcquireResourceExclusive(
    __inout __deref __drv_neverHold(ResourceLite) 
    __deref __drv_acquiresResource(ResourceLite) 
    PERESOURCE Resource
    );


VOID
FLTAPI
sdv_FltAcquireResourceShared(
    __inout __deref __drv_neverHold(ResourceLite) 
    __deref __drv_acquiresResource(ResourceLite) 
    PERESOURCE Resource
    );


VOID
FLTAPI
sdv_FltReleaseResource(
    __inout __deref __drv_releasesExclusiveResource(ResourceLite) PERESOURCE Resource
    );



VOID
FLTAPI
sdv_FltInitializePushLock(
    __out PEX_PUSH_LOCK PushLock
    );


VOID
FLTAPI
sdv_FltDeletePushLock(
    __in PEX_PUSH_LOCK PushLock
    );


VOID
FLTAPI
sdv_FltAcquirePushLockExclusive(
    __inout __deref __drv_acquiresExclusiveResource(ExPushLockType)
    PEX_PUSH_LOCK PushLock
    );


VOID
FLTAPI
sdv_FltAcquirePushLockShared(
    __inout __deref __drv_acquiresExclusiveResource(ExPushLockType)        
    PEX_PUSH_LOCK PushLock
    );


VOID
FLTAPI
sdv_FltReleasePushLock(
    __inout __deref __drv_releasesExclusiveResource(ExPushLockType)        
    PEX_PUSH_LOCK PushLock
    );



NTSTATUS
FLTAPI
sdv_FltCancellableWaitForSingleObject(
    __in PVOID Object,
    __in_opt PLARGE_INTEGER Timeout,
    __in_opt PFLT_CALLBACK_DATA CallbackData
    );


NTSTATUS
FLTAPI
sdv_FltCancellableWaitForMultipleObjects(
    __in ULONG Count,
    __in_ecount(Count) PVOID ObjectArray[],
    __in WAIT_TYPE WaitType,
    __in_opt PLARGE_INTEGER Timeout,
    __in_opt PKWAIT_BLOCK WaitBlockArray,
    __in PFLT_CALLBACK_DATA CallbackData
    );





BOOLEAN
FLTAPI
sdv_FltIsOperationSynchronous (
    __in PFLT_CALLBACK_DATA CallbackData
    );


BOOLEAN
FLTAPI
sdv_FltIs32bitProcess (
    __in_opt PFLT_CALLBACK_DATA CallbackData
    );


PEPROCESS
FLTAPI
sdv_FltGetRequestorProcess (
    __in PFLT_CALLBACK_DATA CallbackData
    );


ULONG
FLTAPI
sdv_FltGetRequestorProcessId (
    __in PFLT_CALLBACK_DATA CallbackData
    );


HANDLE
FLTAPI
sdv_FltGetRequestorProcessIdEx (
    __in PFLT_CALLBACK_DATA CallbackData
    );


VOID
FLTAPI
sdv_FltNotifyFilterChangeDirectory (
    __inout PNOTIFY_SYNC NotifySync,
    __inout PLIST_ENTRY NotifyList,
    __in PVOID FsContext,
    __in PSTRING FullDirectoryName,
    __in BOOLEAN WatchTree,
    __in BOOLEAN IgnoreBuffer,
    __in ULONG CompletionFilter,
    __in PFLT_CALLBACK_DATA NotifyCallbackData,
    __in_opt PCHECK_FOR_TRAVERSE_ACCESS TraverseCallback,
    __in_opt PSECURITY_SUBJECT_CONTEXT SubjectContext,
    __in_opt PFILTER_REPORT_CHANGE FilterCallback
    );


NTSTATUS
FLTAPI
sdv_FltGetRequestorSessionId(
    __in PFLT_CALLBACK_DATA CallbackData,
    __out PULONG SessionId 
    );



NTSTATUS
FLTAPI
sdv_FltAdjustDeviceStackSizeForIoRedirection(
    __in PFLT_INSTANCE SourceInstance,
    __in PFLT_INSTANCE TargetInstance,
    __out_opt PBOOLEAN SourceDeviceStackSizeModified
    );


NTSTATUS
FLTAPI
sdv_FltIsIoRedirectionAllowed(
    __in PFLT_INSTANCE SourceInstance,   
    __in PFLT_INSTANCE TargetInstance,
    __out PBOOLEAN RedirectionAllowed 
    );


NTSTATUS
FLTAPI
sdv_FltIsIoRedirectionAllowedForOperation(
    __in PFLT_CALLBACK_DATA Data,
    __in PFLT_INSTANCE TargetInstance,
    __out PBOOLEAN RedirectionAllowedThisIo,    
    __out_opt PBOOLEAN RedirectionAllowedAllIo 
    );





NTSTATUS
FLTAPI
sdv_FltEnlistInTransaction (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in PFLT_CONTEXT TransactionContext,
    __in NOTIFICATION_MASK NotificationMask
    );


NTSTATUS
FLTAPI
sdv_FltRollbackEnlistment (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );


NTSTATUS
FLTAPI
sdv_FltPrePrepareComplete (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );


NTSTATUS
FLTAPI
sdv_FltPrepareComplete (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );


NTSTATUS
FLTAPI
sdv_FltCommitComplete (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );


NTSTATUS
FLTAPI
sdv_FltCommitFinalizeComplete (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );


NTSTATUS
FLTAPI
sdv_FltRollbackComplete (
    __in PFLT_INSTANCE Instance,
    __in PKTRANSACTION Transaction,
    __in_opt PFLT_CONTEXT TransactionContext
    );




NTSTATUS
FLTAPI
sdv_FltAllocateExtraCreateParameterList (
    __in PFLT_FILTER Filter,
    __in FSRTL_ALLOCATE_ECPLIST_FLAGS Flags,
    __deref_out PECP_LIST *EcpList
    );


NTSTATUS
FLTAPI
sdv_FltAllocateExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __in LPCGUID EcpType,
    __in_bound ULONG SizeOfContext,
    __in FSRTL_ALLOCATE_ECP_FLAGS Flags,
    __in_opt PFSRTL_EXTRA_CREATE_PARAMETER_CLEANUP_CALLBACK CleanupCallback,
    __in ULONG PoolTag,
    __deref_out PVOID *EcpContext
    );


VOID
FLTAPI
sdv_FltInitExtraCreateParameterLookasideList (
    __in PFLT_FILTER Filter,
    __inout PVOID Lookaside,
    __in FSRTL_ECP_LOOKASIDE_FLAGS Flags,
    __in SIZE_T Size,
    __in ULONG Tag
    );


VOID
FLTAPI
sdv_FltDeleteExtraCreateParameterLookasideList (
    __in PFLT_FILTER Filter,
    __inout PVOID Lookaside,
    __in FSRTL_ECP_LOOKASIDE_FLAGS Flags
    );


NTSTATUS
FLTAPI
sdv_FltAllocateExtraCreateParameterFromLookasideList (
    __in PFLT_FILTER Filter,
    __in LPCGUID EcpType,
    __in ULONG SizeOfContext,
    __in FSRTL_ALLOCATE_ECP_FLAGS Flags,
    __in_opt PFSRTL_EXTRA_CREATE_PARAMETER_CLEANUP_CALLBACK CleanupCallback,
    __inout PVOID LookasideList,
    __deref_out PVOID *EcpContext
    );


NTSTATUS
FLTAPI
sdv_FltInsertExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __inout PECP_LIST EcpList,
    __inout PVOID EcpContext
    );


NTSTATUS
FLTAPI
sdv_FltFindExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __in PECP_LIST EcpList,
    __in LPCGUID EcpType,
    __deref_opt_out PVOID *EcpContext,
    __out_opt ULONG *EcpContextSize
    );


NTSTATUS
FLTAPI
sdv_FltRemoveExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __inout PECP_LIST EcpList,
    __in LPCGUID EcpType,
    __deref_out PVOID *EcpContext,
    __out_opt ULONG *EcpContextSize
    );


VOID
FLTAPI
sdv_FltFreeExtraCreateParameterList (
    __in PFLT_FILTER Filter,
    __in PECP_LIST EcpList
    );


VOID
FLTAPI
sdv_FltFreeExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __in PVOID EcpContext
    );


NTSTATUS
FLTAPI
sdv_FltGetEcpListFromCallbackData (
    __in PFLT_FILTER Filter,
    __in PFLT_CALLBACK_DATA CallbackData,
    __deref_out_opt PECP_LIST *EcpList
    );


NTSTATUS
FLTAPI
sdv_FltSetEcpListIntoCallbackData (
    __in PFLT_FILTER Filter,
    __in PFLT_CALLBACK_DATA CallbackData,
    __in PECP_LIST EcpList
    );


NTSTATUS
FLTAPI
sdv_FltGetNextExtraCreateParameter (
    __in PFLT_FILTER Filter,
    __in PECP_LIST EcpList,
    __in_opt PVOID CurrentEcpContext,
    __out_opt LPGUID NextEcpType,
    __deref_opt_out PVOID *NextEcpContext,
    __out_opt ULONG *NextEcpContextSize
    );


VOID
FLTAPI
sdv_FltAcknowledgeEcp (
    __in PFLT_FILTER Filter,
    __in PVOID EcpContext
    );


BOOLEAN
FLTAPI
sdv_FltIsEcpAcknowledged (
    __in PFLT_FILTER Filter,
    __in PVOID EcpContext
    );


BOOLEAN
FLTAPI
sdv_FltIsEcpFromUserMode (
    __in PFLT_FILTER Filter,
    __in PVOID EcpContext
    );



NTSTATUS
FLTAPI
sdv_FltRetrieveIoPriorityInfo (
    __in_opt PFLT_CALLBACK_DATA Data,
    __in_opt PFILE_OBJECT FileObject,
    __in_opt PETHREAD Thread,
    __inout PIO_PRIORITY_INFO PriorityInfo
    );


NTSTATUS
FLTAPI
sdv_FltApplyPriorityInfoThread(
    __in PIO_PRIORITY_INFO InputPriorityInfo,
    __out_opt PIO_PRIORITY_INFO OutputPriorityInfo,
    __in PETHREAD Thread
    );


IO_PRIORITY_HINT
FLTAPI
sdv_FltGetIoPriorityHint (
    __in PFLT_CALLBACK_DATA Data
    );


IO_PRIORITY_HINT
FLTAPI
sdv_FltGetIoPriorityHintFromCallbackData (
    __in PFLT_CALLBACK_DATA Data
    );


NTSTATUS
FLTAPI
sdv_FltSetIoPriorityHintIntoCallbackData (
    __in PFLT_CALLBACK_DATA Data,
    __in IO_PRIORITY_HINT PriorityHint
    );


IO_PRIORITY_HINT
FLTAPI
sdv_FltGetIoPriorityHintFromFileObject (
    __in PFILE_OBJECT FileObject
    );


NTSTATUS
FLTAPI
sdv_FltSetIoPriorityHintIntoFileObject (
    __in PFILE_OBJECT FileObject,
    __in IO_PRIORITY_HINT PriorityHint
    );


IO_PRIORITY_HINT
FLTAPI
sdv_FltGetIoPriorityHintFromThread (
    __in PETHREAD Thread
    );


NTSTATUS
FLTAPI
sdv_FltSetIoPriorityHintIntoThread (
    __in PETHREAD Thread,
    __in IO_PRIORITY_HINT PriorityHint
    );



PCHAR
FLTAPI
sdv_FltGetIrpName (
    __in UCHAR IrpMajorCode
    );











#endif
