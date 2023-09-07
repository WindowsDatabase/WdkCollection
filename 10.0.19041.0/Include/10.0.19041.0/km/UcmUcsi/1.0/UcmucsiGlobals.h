/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    UcmUcsiGlobals.h

Abstract:

    UCM-UCSI global definitions.

Environment:

    Kernel-mode.

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _UCMUCSIGLOBALS_H_
#define _UCMUCSIGLOBALS_H_

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



typedef struct _UCMUCSI_DRIVER_GLOBALS {
    ULONG Reserved;
} UCMUCSI_DRIVER_GLOBALS, *PUCMUCSI_DRIVER_GLOBALS;




WDF_EXTERN_C_END

#endif // _UCMUCSIGLOBALS_H_

