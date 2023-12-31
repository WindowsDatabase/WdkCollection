/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name:

    WdfRegistry.h

Abstract:

    This is the interface to registry access.

Environment:

    kernel mode only

Revision History:

--*/

#ifndef _WDFREGISTRY_1_9_H_
#define _WDFREGISTRY_1_9_H_



#if (NTDDI_VERSION >= NTDDI_WIN2K)



//
// WDF Function: WdfRegistryOpenKey
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYOPENKEY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryOpenKey(
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    )
{
    return ((PFN_WDFREGISTRYOPENKEY) WdfFunctions[WdfRegistryOpenKeyTableIndex])(WdfDriverGlobals, ParentKey, KeyName, DesiredAccess, KeyAttributes, Key);
}

//
// WDF Function: WdfRegistryCreateKey
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYCREATEKEY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_
    ULONG CreateOptions,
    _Out_opt_
    PULONG CreateDisposition,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryCreateKey(
    _In_opt_
    WDFKEY ParentKey,
    _In_
    PCUNICODE_STRING KeyName,
    _In_
    ACCESS_MASK DesiredAccess,
    _In_
    ULONG CreateOptions,
    _Out_opt_
    PULONG CreateDisposition,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES KeyAttributes,
    _Out_
    WDFKEY* Key
    )
{
    return ((PFN_WDFREGISTRYCREATEKEY) WdfFunctions[WdfRegistryCreateKeyTableIndex])(WdfDriverGlobals, ParentKey, KeyName, DesiredAccess, CreateOptions, CreateDisposition, KeyAttributes, Key);
}

//
// WDF Function: WdfRegistryClose
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFREGISTRYCLOSE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfRegistryClose(
    _In_
    WDFKEY Key
    )
{
    ((PFN_WDFREGISTRYCLOSE) WdfFunctions[WdfRegistryCloseTableIndex])(WdfDriverGlobals, Key);
}

//
// WDF Function: WdfRegistryWdmGetHandle
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
HANDLE
(*PFN_WDFREGISTRYWDMGETHANDLE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
HANDLE
FORCEINLINE
WdfRegistryWdmGetHandle(
    _In_
    WDFKEY Key
    )
{
    return ((PFN_WDFREGISTRYWDMGETHANDLE) WdfFunctions[WdfRegistryWdmGetHandleTableIndex])(WdfDriverGlobals, Key);
}

//
// WDF Function: WdfRegistryRemoveKey
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYREMOVEKEY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryRemoveKey(
    _In_
    WDFKEY Key
    )
{
    return ((PFN_WDFREGISTRYREMOVEKEY) WdfFunctions[WdfRegistryRemoveKeyTableIndex])(WdfDriverGlobals, Key);
}

//
// WDF Function: WdfRegistryRemoveValue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYREMOVEVALUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryRemoveValue(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName
    )
{
    return ((PFN_WDFREGISTRYREMOVEVALUE) WdfFunctions[WdfRegistryRemoveValueTableIndex])(WdfDriverGlobals, Key, ValueName);
}

//
// WDF Function: WdfRegistryQueryValue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYVALUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueLength,
    _Out_writes_bytes_opt_( ValueLength)
    PVOID Value,
    _Out_opt_
    PULONG ValueLengthQueried,
    _Out_opt_
    PULONG ValueType
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryValue(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueLength,
    _Out_writes_bytes_opt_( ValueLength)
    PVOID Value,
    _Out_opt_
    PULONG ValueLengthQueried,
    _Out_opt_
    PULONG ValueType
    )
{
    return ((PFN_WDFREGISTRYQUERYVALUE) WdfFunctions[WdfRegistryQueryValueTableIndex])(WdfDriverGlobals, Key, ValueName, ValueLength, Value, ValueLengthQueried, ValueType);
}

//
// WDF Function: WdfRegistryQueryMemory
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYMEMORY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES MemoryAttributes,
    _Out_
    WDFMEMORY* Memory,
    _Out_opt_
    PULONG ValueType
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryMemory(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    _Strict_type_match_
    POOL_TYPE PoolType,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES MemoryAttributes,
    _Out_
    WDFMEMORY* Memory,
    _Out_opt_
    PULONG ValueType
    )
{
    return ((PFN_WDFREGISTRYQUERYMEMORY) WdfFunctions[WdfRegistryQueryMemoryTableIndex])(WdfDriverGlobals, Key, ValueName, PoolType, MemoryAttributes, Memory, ValueType);
}

//
// WDF Function: WdfRegistryQueryMultiString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYMULTISTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES StringsAttributes,
    _In_
    WDFCOLLECTION Collection
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryMultiString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES StringsAttributes,
    _In_
    WDFCOLLECTION Collection
    )
{
    return ((PFN_WDFREGISTRYQUERYMULTISTRING) WdfFunctions[WdfRegistryQueryMultiStringTableIndex])(WdfDriverGlobals, Key, ValueName, StringsAttributes, Collection);
}

//
// WDF Function: WdfRegistryQueryUnicodeString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYUNICODESTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_opt_
    PUSHORT ValueByteLength,
    _Inout_opt_
    PUNICODE_STRING Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryUnicodeString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_opt_
    PUSHORT ValueByteLength,
    _Inout_opt_
    PUNICODE_STRING Value
    )
{
    return ((PFN_WDFREGISTRYQUERYUNICODESTRING) WdfFunctions[WdfRegistryQueryUnicodeStringTableIndex])(WdfDriverGlobals, Key, ValueName, ValueByteLength, Value);
}

//
// WDF Function: WdfRegistryQueryString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYSTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    )
{
    return ((PFN_WDFREGISTRYQUERYSTRING) WdfFunctions[WdfRegistryQueryStringTableIndex])(WdfDriverGlobals, Key, ValueName, String);
}

//
// WDF Function: WdfRegistryQueryULong
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYQUERYULONG)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_
    PULONG Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryQueryULong(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _Out_
    PULONG Value
    )
{
    return ((PFN_WDFREGISTRYQUERYULONG) WdfFunctions[WdfRegistryQueryULongTableIndex])(WdfDriverGlobals, Key, ValueName, Value);
}

//
// WDF Function: WdfRegistryAssignValue
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNVALUE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    ULONG ValueLength,
    _In_reads_( ValueLength)
    PVOID Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignValue(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    ULONG ValueLength,
    _In_reads_( ValueLength)
    PVOID Value
    )
{
    return ((PFN_WDFREGISTRYASSIGNVALUE) WdfFunctions[WdfRegistryAssignValueTableIndex])(WdfDriverGlobals, Key, ValueName, ValueType, ValueLength, Value);
}

//
// WDF Function: WdfRegistryAssignMemory
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNMEMORY)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    WDFMEMORY Memory,
    _In_opt_
    PWDFMEMORY_OFFSET MemoryOffsets
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignMemory(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG ValueType,
    _In_
    WDFMEMORY Memory,
    _In_opt_
    PWDFMEMORY_OFFSET MemoryOffsets
    )
{
    return ((PFN_WDFREGISTRYASSIGNMEMORY) WdfFunctions[WdfRegistryAssignMemoryTableIndex])(WdfDriverGlobals, Key, ValueName, ValueType, Memory, MemoryOffsets);
}

//
// WDF Function: WdfRegistryAssignMultiString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNMULTISTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFCOLLECTION StringsCollection
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignMultiString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFCOLLECTION StringsCollection
    )
{
    return ((PFN_WDFREGISTRYASSIGNMULTISTRING) WdfFunctions[WdfRegistryAssignMultiStringTableIndex])(WdfDriverGlobals, Key, ValueName, StringsCollection);
}

//
// WDF Function: WdfRegistryAssignUnicodeString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNUNICODESTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    PCUNICODE_STRING Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignUnicodeString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    PCUNICODE_STRING Value
    )
{
    return ((PFN_WDFREGISTRYASSIGNUNICODESTRING) WdfFunctions[WdfRegistryAssignUnicodeStringTableIndex])(WdfDriverGlobals, Key, ValueName, Value);
}

//
// WDF Function: WdfRegistryAssignString
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNSTRING)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignString(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    WDFSTRING String
    )
{
    return ((PFN_WDFREGISTRYASSIGNSTRING) WdfFunctions[WdfRegistryAssignStringTableIndex])(WdfDriverGlobals, Key, ValueName, String);
}

//
// WDF Function: WdfRegistryAssignULong
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFREGISTRYASSIGNULONG)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG Value
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfRegistryAssignULong(
    _In_
    WDFKEY Key,
    _In_
    PCUNICODE_STRING ValueName,
    _In_
    ULONG Value
    )
{
    return ((PFN_WDFREGISTRYASSIGNULONG) WdfFunctions[WdfRegistryAssignULongTableIndex])(WdfDriverGlobals, Key, ValueName, Value);
}



#endif // (NTDDI_VERSION >= NTDDI_WIN2K)


#endif // _WDFREGISTRY_1_9_H_
