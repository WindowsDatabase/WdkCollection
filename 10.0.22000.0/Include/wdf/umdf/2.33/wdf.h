/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Wdf.h

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

#ifndef _Dispatch_type_
#include <driverspecs.h>
#endif

__user_code

//
// Fix for value-less UMDF_DRIVER identifier in UMDF 1.x.
//
#if defined(UMDF_DRIVER)
#undef UMDF_DRIVER
#endif
#define UMDF_DRIVER (2)

#include <winioctl.h>
#include <ntstatus.h>

#pragma warning( disable: 4201 )    // nonstandard extension used : nameless struct/union
#pragma warning( disable: 4214 )    // nonstandard extension used : bit field types other than int

#include "WudfWdm.h"

#ifdef WDF_UMDF2_DRIVER_INTERNAL
#include <WdfUtilityUm.h>
#endif

//
// Rename WdfFunctions to match version number. Build issues relating to
// unresolved externals of WdfFunctions or WdfFunctions_XXXXX indicate
// multiple WDF versions are being included. Ensure WDF version of all input
// binaries match to resolve.
//

#define WdfFunctions WdfFunctions_02033

typedef VOID (*WDFFUNC) (VOID);
extern const WDFFUNC *WdfFunctions;

// Basic definitions
#include "wdftypes.h"
#include "wdfglobals.h"
#include "wdffuncenum.h"
#include "wdfstatus.h"
#include "wdfassert.h"
#include "wdfverifier.h"

// generic object
#include "wdfobject.h"

// Synchronization
#include "wdfsync.h"

#include "wdfcore.h"

#include "wdfdriver.h"

// Objects
#include "WdfQueryInterface.h"
#include "wdfmemory.h"
#include "wdfchildlist.h"
#include "wdffileobject.h"
#include "wdfdevice.h"
#include "wdfcollection.h"

#include "wdftimer.h"
#include "wdfworkitem.h"
#include "wdfinterrupt.h"
#include "wdfresource.h"

// I/O
#include "wdfrequest.h"
#include "wdfiotarget.h"
#include "wdfio.h"

// particular device types
#include "wdffdo.h"
#include "wdfpdo.h"
#include "wdfcontrol.h"

#include "WdfWMI.h"

#include "wdfstring.h"
#include "wdfregistry.h"

#include "wdfbugcodes.h"
#include "wdfroletypes.h"
#include "wdfhwaccess.h"

#include "wdfcompanion.h"

__declspec(dllexport)
__control_entrypoint(DllExport)
NTSTATUS
FxDriverEntryUm(
    _In_ PVOID LoaderInterface,
    _In_ PVOID Context,
    _In_ PVOID DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    );
WDF_EXTERN_C_END

