/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    UcmTcpciCx.h

Abstract:

    Main header file for UCM (USB Connector Manager) TCPCI (Type-C Port Controller Interface) class
    extension client.

Environment:

    Kernel-mode.

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _UCMTCPCICX_H_
#define _UCMTCPCICX_H_

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



typedef VOID (*UCMTCPCIFUNC) (VOID);
extern UCMTCPCIFUNC UcmtcpciFunctions[];

#include "UcmTcpciGlobals.h"
#include "UcmTcpciFuncEnum.h"
#include "UcmTcpciSpec.h"
#include "UcmTcpciDevice.h"
#include "UcmTcpciPortController.h"
#include "UcmTcpciPortControllerRequests.h"





WDF_EXTERN_C_END

#endif // _UCMTCPCICX_H_

