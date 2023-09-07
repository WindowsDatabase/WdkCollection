/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    NetRxQueue.h

Abstract:

    This is the interfaces for the NetRxQueue

Environment:

    kernel mode only

Revision History:

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _NETRXQUEUE_H_
#define _NETRXQUEUE_H_

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




#include <NetPacketQueue.h>

typedef struct NETRXQUEUE_INIT *PNETRXQUEUE_INIT;

typedef struct _NET_RXQUEUE_BUFFER_LAYOUT_HINT
{
    //
    // Minimum space that should be unused in
    // the start of the first fragment
    //
    ULONG MinimumBackfillSize;

    //
    // Ideal alignment for the start of the L3
    // header. The value is in the form N-1,
    // where N is the alignment.
    //
    ULONG L3HeaderAlignment;

} NET_RXQUEUE_BUFFER_LAYOUT_HINT, *PNET_RXQUEUE_BUFFER_LAYOUT_HINT;

//
// NET Function: NetRxQueueCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_NETRXQUEUECREATE)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _Inout_
    PNETRXQUEUE_INIT NetRxQueueInit,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RxQueueAttributes,
    _In_
    PNET_PACKET_QUEUE_CONFIG Configuration,
    _Out_
    NETPACKETQUEUE* RxQueue
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
NetRxQueueCreate(
    _Inout_
    PNETRXQUEUE_INIT NetRxQueueInit,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES RxQueueAttributes,
    _In_
    PNET_PACKET_QUEUE_CONFIG Configuration,
    _Out_
    NETPACKETQUEUE* RxQueue
    )
{
    return ((PFN_NETRXQUEUECREATE) NetFunctions[NetRxQueueCreateTableIndex])(NetDriverGlobals, NetRxQueueInit, RxQueueAttributes, Configuration, RxQueue);
}

//
// NET Function: NetRxQueueNotifyMoreReceivedPacketsAvailable
//
typedef
_IRQL_requires_max_(HIGH_LEVEL)
WDFAPI
VOID
(*PFN_NETRXQUEUENOTIFYMORERECEIVEDPACKETSAVAILABLE)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPACKETQUEUE RxQueue
    );

_IRQL_requires_max_(HIGH_LEVEL)
VOID
FORCEINLINE
NetRxQueueNotifyMoreReceivedPacketsAvailable(
    _In_
    NETPACKETQUEUE RxQueue
    )
{
    ((PFN_NETRXQUEUENOTIFYMORERECEIVEDPACKETSAVAILABLE) NetFunctions[NetRxQueueNotifyMoreReceivedPacketsAvailableTableIndex])(NetDriverGlobals, RxQueue);
}

//
// NET Function: NetRxQueueInitGetQueueId
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
ULONG
(*PFN_NETRXQUEUEINITGETQUEUEID)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    PNETRXQUEUE_INIT NetRxQueueInit
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
ULONG
FORCEINLINE
NetRxQueueInitGetQueueId(
    _In_
    PNETRXQUEUE_INIT NetRxQueueInit
    )
{
    return ((PFN_NETRXQUEUEINITGETQUEUEID) NetFunctions[NetRxQueueInitGetQueueIdTableIndex])(NetDriverGlobals, NetRxQueueInit);
}

//
// NET Function: NetRxQueueGetDatapathDescriptor
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PCNET_DATAPATH_DESCRIPTOR
(*PFN_NETRXQUEUEGETDATAPATHDESCRIPTOR)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPACKETQUEUE NetRxQueue
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
PCNET_DATAPATH_DESCRIPTOR
FORCEINLINE
NetRxQueueGetDatapathDescriptor(
    _In_
    NETPACKETQUEUE NetRxQueue
    )
{
    return ((PFN_NETRXQUEUEGETDATAPATHDESCRIPTOR) NetFunctions[NetRxQueueGetDatapathDescriptorTableIndex])(NetDriverGlobals, NetRxQueue);
}

//
// NET Function: NetRxQueueGetPacketContextToken
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
PNET_PACKET_CONTEXT_TOKEN
(*PFN_NETRXQUEUEGETPACKETCONTEXTTOKEN)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPACKETQUEUE NetRxQueue,
    _In_
    PCNET_CONTEXT_TYPE_INFO ContextTypeInfo
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
PNET_PACKET_CONTEXT_TOKEN
FORCEINLINE
NetRxQueueGetPacketContextToken(
    _In_
    NETPACKETQUEUE NetRxQueue,
    _In_
    PCNET_CONTEXT_TYPE_INFO ContextTypeInfo
    )
{
    return ((PFN_NETRXQUEUEGETPACKETCONTEXTTOKEN) NetFunctions[NetRxQueueGetPacketContextTokenTableIndex])(NetDriverGlobals, NetRxQueue, ContextTypeInfo);
}

//
// NET Function: NetRxQueueInitAddPacketContextAttributes
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_NETRXQUEUEINITADDPACKETCONTEXTATTRIBUTES)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _Inout_
    PNETRXQUEUE_INIT NetRxQueueInit,
    _In_
    PNET_PACKET_CONTEXT_ATTRIBUTES PacketContextAttributes
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
NetRxQueueInitAddPacketContextAttributes(
    _Inout_
    PNETRXQUEUE_INIT NetRxQueueInit,
    _In_
    PNET_PACKET_CONTEXT_ATTRIBUTES PacketContextAttributes
    )
{
    return ((PFN_NETRXQUEUEINITADDPACKETCONTEXTATTRIBUTES) NetFunctions[NetRxQueueInitAddPacketContextAttributesTableIndex])(NetDriverGlobals, NetRxQueueInit, PacketContextAttributes);
}

//
// NET Function: NetRxQueueGetPacketExtensionOffset
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
SIZE_T
(*PFN_NETRXQUEUEGETPACKETEXTENSIONOFFSET)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPACKETQUEUE NetRxQueue,
    _In_
    CONST PNET_PACKET_EXTENSION_QUERY ExtensionToGet
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
SIZE_T
FORCEINLINE
NetRxQueueGetPacketExtensionOffset(
    _In_
    NETPACKETQUEUE NetRxQueue,
    _In_
    CONST PNET_PACKET_EXTENSION_QUERY ExtensionToGet
    )
{
    return ((PFN_NETRXQUEUEGETPACKETEXTENSIONOFFSET) NetFunctions[NetRxQueueGetPacketExtensionOffsetTableIndex])(NetDriverGlobals, NetRxQueue, ExtensionToGet);
}




WDF_EXTERN_C_END

#endif // _NETRXQUEUE_H_

