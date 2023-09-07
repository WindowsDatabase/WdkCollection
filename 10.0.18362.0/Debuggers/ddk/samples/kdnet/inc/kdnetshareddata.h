/*++

Copyright (c) Microsoft Corporation

Module Name:

    kdnetshareddata.h

Abstract:

    Defines the public interface for the kdnet data that is shared with its
    extensibility modules.

Author:

    Joe Ballantyne (joeball) 2-18-2012

--*/

#pragma once

#define MAC_ADDRESS_SIZE 6
#define UNDI_DEFAULT_HARDWARE_CONTEXT_SIZE ((512 + 10) * 1024)

#if defined(_AMD64_)
#define MAX_HARDWARE_CONTEXT_SIZE (160*1024*1024)
#else
#define MAX_HARDWARE_CONTEXT_SIZE (16*1024*1024)
#endif

//
// TRANSMIT_ASYNC indicates that the packet should be sent asynchronously
// if possible, so the KDNET extensibility module will not wait for the
// hardware to send the packet before returning from SendTxPacket.  Support
// for this flag is optional, but can significantly improve performance.
//

#define TRANSMIT_ASYNC 0x80000000

//
// TRANSMIT_HANDLE indicates that this handle is for a transmit packet not for
// a receive packet.  Support for this flag is required.
//

#define TRANSMIT_HANDLE 0x40000000

//
// TRANSMIT_LAST indicates that this is the last packet of the current set of
// packets being sent by kdnic, and can be optionally used by the KDNET
// extensibility module to optimize send performance by allowing it to perform
// certain hardware operations for a set of packets instead of on each one.
// This flag is informational, and use of this flag is optional.
//

#define TRANSMIT_LAST 0x20000000

//
// HANDLE_FLAGS is the full set of possible flags that can be set in a handle.
// Use of this value to clear handle flags to obtain the packet index is
// absolutely required so that any newly added flags get properly removed
// from handles.
//

#define HANDLE_FLAGS (TRANSMIT_ASYNC | TRANSMIT_HANDLE | TRANSMIT_LAST)

//
// The following flags are set by the KDNET extensibility module in the Flags
// field of the KDNET_SHARED_DATA structure to indicate support for optional or
// extended functionality.
//

#define KDX_LOCAL_LINK_ONLY 0x1
#define KDX_VALID_FLAGS (KDX_LOCAL_LINK_ONLY)

typedef struct _KDNET_SHARED_DATA
{
    PVOID Hardware;
    PDEBUG_DEVICE_DESCRIPTOR Device;
    PUCHAR TargetMacAddress;
    ULONG LinkSpeed;
    ULONG LinkDuplex;
    PUCHAR LinkState;
    ULONG SerialBaudRate;
    ULONG Flags;
    UCHAR RestartKdnet;
    UCHAR Reserved[3];
} KDNET_SHARED_DATA, *PKDNET_SHARED_DATA;

