/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name:

    WdfMemory.h

Abstract:

    Contains prototypes for managing memory objects in the driver frameworks.

Author:

Environment:

    kernel mode only

Revision History:

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _WDFMEMORY_H_
#define _WDFMEMORY_H_

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



#if (NTDDI_VERSION >= NTDDI_WIN2K)

typedef enum _WDF_MEMORY_DESCRIPTOR_TYPE {
    WdfMemoryDescriptorTypeInvalid = 0,
    WdfMemoryDescriptorTypeBuffer,
    WdfMemoryDescriptorTypeMdl,
    WdfMemoryDescriptorTypeHandle,
} WDF_MEMORY_DESCRIPTOR_TYPE;



typedef struct _WDFMEMORY_OFFSET {
    //
    // Offset into the WDFMEMORY that the operation should start at.
    //
    size_t BufferOffset;

    //
    // Number of bytes that the operation should access.  If 0, the entire
    // length of the WDFMEMORY buffer will be used in the operation or ignored
    // depending on the API.
    //
    size_t BufferLength;

} WDFMEMORY_OFFSET, *PWDFMEMORY_OFFSET;

typedef struct _WDF_MEMORY_DESCRIPTOR {
    WDF_MEMORY_DESCRIPTOR_TYPE Type;

    union {
        struct {
            PVOID Buffer;

            ULONG Length;
        } BufferType;

        struct {
            PMDL Mdl;

            ULONG BufferLength;
        } MdlType;

        struct {
            WDFMEMORY Memory;
            PWDFMEMORY_OFFSET Offsets;
        } HandleType;
    } u;

} WDF_MEMORY_DESCRIPTOR, *PWDF_MEMORY_DESCRIPTOR;

VOID
FORCEINLINE
WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
    _Out_ PWDF_MEMORY_DESCRIPTOR Descriptor,
    _In_  PVOID Buffer,
    _In_  ULONG BufferLength
    )
{
    RtlZeroMemory(Descriptor, sizeof(WDF_MEMORY_DESCRIPTOR));

    Descriptor->Type = WdfMemoryDescriptorTypeBuffer;
    Descriptor->u.BufferType.Buffer = Buffer;
    Descriptor->u.BufferType.Length = BufferLength;
}

VOID
FORCEINLINE
WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
    _Out_ PWDF_MEMORY_DESCRIPTOR Descriptor,
    _In_ WDFMEMORY Memory,
    _In_opt_ PWDFMEMORY_OFFSET Offsets
    )
{
    RtlZeroMemory(Descriptor, sizeof(WDF_MEMORY_DESCRIPTOR));

    Descriptor->Type = WdfMemoryDescriptorTypeHandle;
    Descriptor->u.HandleType.Memory = Memory;
    Descriptor->u.HandleType.Offsets = Offsets;
}


VOID
FORCEINLINE
WDF_MEMORY_DESCRIPTOR_INIT_MDL(
    _Out_ PWDF_MEMORY_DESCRIPTOR Descriptor,
    _In_ PMDL Mdl,
    _In_ ULONG BufferLength
    )
{
    RtlZeroMemory(Descriptor, sizeof(WDF_MEMORY_DESCRIPTOR));

    Descriptor->Type = WdfMemoryDescriptorTypeMdl;
    Descriptor->u.MdlType.Mdl = Mdl;
    Descriptor->u.MdlType.BufferLength = BufferLength;
}

//
// WDF Function: WdfMemoryCreate
//
typedef
_Must_inspect_result_
_When_(PoolType == 1 || PoolType == 257, _IRQL_requires_max_(APC_LEVEL))
_When_(PoolType == 0 || PoolType == 256 || PoolType == 512, _IRQL_requires_max_(DISPATCH_LEVEL))
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYCREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    ULONG PoolTag,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory,
    _Outptr_opt_result_bytebuffer_(BufferSize)
    PVOID* Buffer
    );

// 
// Creates a memory handle which allocates the underlying memory for the buffer
// and context internal to the handle
// 
_Must_inspect_result_
_When_(PoolType == 1 || PoolType == 257, _IRQL_requires_max_(APC_LEVEL))
_When_(PoolType == 0 || PoolType == 256 || PoolType == 512, _IRQL_requires_max_(DISPATCH_LEVEL))
FORCEINLINE
NTSTATUS
WdfMemoryCreate(
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    ULONG PoolTag,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory,
    _Outptr_opt_result_bytebuffer_(BufferSize)
    PVOID* Buffer
    )
{
    return ((PFN_WDFMEMORYCREATE) WdfFunctions[WdfMemoryCreateTableIndex])(WdfDriverGlobals, Attributes, PoolType, PoolTag, BufferSize, Memory, Buffer);
}

//
// WDF Function: WdfMemoryCreatePreallocated
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYCREATEPREALLOCATED)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_ __drv_aliasesMem
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
WdfMemoryCreatePreallocated(
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES Attributes,
    _In_ __drv_aliasesMem
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _Out_
    WDFMEMORY* Memory
    )
{
    return ((PFN_WDFMEMORYCREATEPREALLOCATED) WdfFunctions[WdfMemoryCreatePreallocatedTableIndex])(WdfDriverGlobals, Attributes, Buffer, BufferSize, Memory);
}

//
// WDF Function: WdfMemoryGetBuffer
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
PVOID
(NTAPI *PFN_WDFMEMORYGETBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY Memory,
    _Out_opt_
    size_t* BufferSize
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
PVOID
WdfMemoryGetBuffer(
    _In_
    WDFMEMORY Memory,
    _Out_opt_
    size_t* BufferSize
    )
{
    return ((PFN_WDFMEMORYGETBUFFER) WdfFunctions[WdfMemoryGetBufferTableIndex])(WdfDriverGlobals, Memory, BufferSize);
}

//
// WDF Function: WdfMemoryAssignBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYASSIGNBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY Memory,
    _Pre_notnull_ _Pre_writable_byte_size_(BufferSize)
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
WdfMemoryAssignBuffer(
    _In_
    WDFMEMORY Memory,
    _Pre_notnull_ _Pre_writable_byte_size_(BufferSize)
    PVOID Buffer,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize
    )
{
    return ((PFN_WDFMEMORYASSIGNBUFFER) WdfFunctions[WdfMemoryAssignBufferTableIndex])(WdfDriverGlobals, Memory, Buffer, BufferSize);
}

//
// WDF Function: WdfMemoryCopyToBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYCOPYTOBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY SourceMemory,
    _In_
    size_t SourceOffset,
    _Out_writes_bytes_( NumBytesToCopyTo )
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyTo == 0, __drv_reportError(NumBytesToCopyTo cannot be zero))
    size_t NumBytesToCopyTo
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
WdfMemoryCopyToBuffer(
    _In_
    WDFMEMORY SourceMemory,
    _In_
    size_t SourceOffset,
    _Out_writes_bytes_( NumBytesToCopyTo )
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyTo == 0, __drv_reportError(NumBytesToCopyTo cannot be zero))
    size_t NumBytesToCopyTo
    )
{
    return ((PFN_WDFMEMORYCOPYTOBUFFER) WdfFunctions[WdfMemoryCopyToBufferTableIndex])(WdfDriverGlobals, SourceMemory, SourceOffset, Buffer, NumBytesToCopyTo);
}

//
// WDF Function: WdfMemoryCopyFromBuffer
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYCOPYFROMBUFFER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFMEMORY DestinationMemory,
    _In_
    size_t DestinationOffset,
    _In_
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
    size_t NumBytesToCopyFrom
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
WdfMemoryCopyFromBuffer(
    _In_
    WDFMEMORY DestinationMemory,
    _In_
    size_t DestinationOffset,
    _In_
    PVOID Buffer,
    _In_
    _When_(NumBytesToCopyFrom == 0, __drv_reportError(NumBytesToCopyFrom cannot be zero))
    size_t NumBytesToCopyFrom
    )
{
    return ((PFN_WDFMEMORYCOPYFROMBUFFER) WdfFunctions[WdfMemoryCopyFromBufferTableIndex])(WdfDriverGlobals, DestinationMemory, DestinationOffset, Buffer, NumBytesToCopyFrom);
}

//
// WDF Function: WdfLookasideListCreate
//
typedef
_Must_inspect_result_
_When_(PoolType == 1 || PoolType == 257, _IRQL_requires_max_(APC_LEVEL))
_When_(PoolType == 0 || PoolType == 256, _IRQL_requires_max_(DISPATCH_LEVEL))
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFLOOKASIDELISTCREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES LookasideAttributes,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES MemoryAttributes,
    _In_opt_
    ULONG PoolTag,
    _Out_
    WDFLOOKASIDE* Lookaside
    );

_Must_inspect_result_
_When_(PoolType == 1 || PoolType == 257, _IRQL_requires_max_(APC_LEVEL))
_When_(PoolType == 0 || PoolType == 256, _IRQL_requires_max_(DISPATCH_LEVEL))
FORCEINLINE
NTSTATUS
WdfLookasideListCreate(
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES LookasideAttributes,
    _In_
    _When_(BufferSize == 0, __drv_reportError(BufferSize cannot be zero))
    size_t BufferSize,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES MemoryAttributes,
    _In_opt_
    ULONG PoolTag,
    _Out_
    WDFLOOKASIDE* Lookaside
    )
{
    return ((PFN_WDFLOOKASIDELISTCREATE) WdfFunctions[WdfLookasideListCreateTableIndex])(WdfDriverGlobals, LookasideAttributes, BufferSize, PoolType, MemoryAttributes, PoolTag, Lookaside);
}

//
// WDF Function: WdfMemoryCreateFromLookaside
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(NTAPI *PFN_WDFMEMORYCREATEFROMLOOKASIDE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFLOOKASIDE Lookaside,
    _Out_
    WDFMEMORY* Memory
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
WdfMemoryCreateFromLookaside(
    _In_
    WDFLOOKASIDE Lookaside,
    _Out_
    WDFMEMORY* Memory
    )
{
    return ((PFN_WDFMEMORYCREATEFROMLOOKASIDE) WdfFunctions[WdfMemoryCreateFromLookasideTableIndex])(WdfDriverGlobals, Lookaside, Memory);
}



#endif // (NTDDI_VERSION >= NTDDI_WIN2K)


WDF_EXTERN_C_END

#endif // _WDFMEMORY_H_

