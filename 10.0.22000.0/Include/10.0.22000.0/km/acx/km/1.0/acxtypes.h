/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    AcxTypes.h

Abstract:

    This module contains contains the C driver frameworks basic types.

Environment:

    kernel mode only

Revision History:


--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _ACXTYPES_H_
#define _ACXTYPES_H_

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



#define ACXAPI



typedef PVOID ACXCONTEXT;

//
// Forward declare structures needed later header files
//
typedef struct ACXCIRCUIT_INIT *PACXCIRCUIT_INIT;
typedef struct ACXSTREAM_INIT  *PACXSTREAM_INIT;
typedef struct ACXFACTORYCIRCUIT_INIT *PACXFACTORYCIRCUIT_INIT;

#ifndef PCGUID
/**
 *  Pointer to a constant form of GUID
 */
typedef const GUID *PCGUID;

#endif
//
// Declare handles and types
//
typedef WDFOBJECT ACXOBJECT;

DECLARE_HANDLE( ACXMANAGER );
WDF_DECLARE_CUSTOM_TYPE( ACXMANAGER );

DECLARE_HANDLE( ACXOBJECTBAG );
WDF_DECLARE_CUSTOM_TYPE( ACXOBJECTBAG );

DECLARE_HANDLE( ACXEVENT );
WDF_DECLARE_CUSTOM_TYPE( ACXEVENT );

DECLARE_HANDLE( ACXEVENTDATA );
WDF_DECLARE_CUSTOM_TYPE( ACXEVENTDATA );

DECLARE_HANDLE( ACXPNPEVENT );
WDF_DECLARE_CUSTOM_TYPE( ACXPNPEVENT );

DECLARE_HANDLE( ACXCIRCUIT );
WDF_DECLARE_CUSTOM_TYPE( ACXCIRCUIT );

DECLARE_HANDLE( ACXPIN );
WDF_DECLARE_CUSTOM_TYPE( ACXPIN );

DECLARE_HANDLE( ACXELEMENT );
WDF_DECLARE_CUSTOM_TYPE( ACXELEMENT );

DECLARE_HANDLE( ACXAUDIOENGINE );
WDF_DECLARE_CUSTOM_TYPE( ACXAUDIOENGINE );

DECLARE_HANDLE( ACXSTREAMAUDIOENGINE );
WDF_DECLARE_CUSTOM_TYPE( ACXSTREAMAUDIOENGINE );

DECLARE_HANDLE( ACXKEYWORDSPOTTER );
WDF_DECLARE_CUSTOM_TYPE( ACXKEYWORDSPOTTER );

DECLARE_HANDLE( ACXVOLUME );
WDF_DECLARE_CUSTOM_TYPE( ACXVOLUME );

DECLARE_HANDLE( ACXMUTE );
WDF_DECLARE_CUSTOM_TYPE( ACXMUTE );

DECLARE_HANDLE( ACXJACK );
WDF_DECLARE_CUSTOM_TYPE( ACXJACK );

DECLARE_HANDLE( ACXMICARRAYGEOMETRY );
WDF_DECLARE_CUSTOM_TYPE( ACXMICARRAYGEOMETRY );

DECLARE_HANDLE( ACXPEAKMETER );
WDF_DECLARE_CUSTOM_TYPE( ACXPEAKMETER );

DECLARE_HANDLE( ACXSTREAM );
WDF_DECLARE_CUSTOM_TYPE( ACXSTREAM );

DECLARE_HANDLE( ACXDATAFORMAT );
WDF_DECLARE_CUSTOM_TYPE( ACXDATAFORMAT );

DECLARE_HANDLE( ACXDATAFORMATLIST );
WDF_DECLARE_CUSTOM_TYPE( ACXDATAFORMATLIST );

DECLARE_HANDLE( ACXTARGETCIRCUIT );
WDF_DECLARE_CUSTOM_TYPE( ACXTARGETCIRCUIT );

DECLARE_HANDLE( ACXTARGETPIN );
WDF_DECLARE_CUSTOM_TYPE( ACXTARGETPIN );

DECLARE_HANDLE( ACXTARGETELEMENT );
WDF_DECLARE_CUSTOM_TYPE( ACXTARGETELEMENT );

DECLARE_HANDLE( ACXTARGETSTREAM );
WDF_DECLARE_CUSTOM_TYPE( ACXTARGETSTREAM );

DECLARE_HANDLE( ACXTARGETFACTORYCIRCUIT );
WDF_DECLARE_CUSTOM_TYPE( ACXTARGETFACTORYCIRCUIT );

DECLARE_HANDLE( ACXSTREAMBRIDGE );
WDF_DECLARE_CUSTOM_TYPE( ACXSTREAMBRIDGE );

DECLARE_HANDLE( ACXCOMPOSITE );
WDF_DECLARE_CUSTOM_TYPE( ACXCOMPOSITE );

DECLARE_HANDLE( ACXCOMPOSITEFACTORY );
WDF_DECLARE_CUSTOM_TYPE( ACXCOMPOSITEFACTORY );

DECLARE_HANDLE( ACXFACTORYCIRCUIT );
WDF_DECLARE_CUSTOM_TYPE( ACXFACTORYCIRCUIT );

DECLARE_HANDLE( ACXCIRCUITMANAGER );
WDF_DECLARE_CUSTOM_TYPE( ACXCIRCUITMANAGER );

DECLARE_HANDLE( ACXCOMPOSITETEMPLATE );
WDF_DECLARE_CUSTOM_TYPE( ACXCOMPOSITETEMPLATE );

DECLARE_HANDLE( ACXCIRCUITTEMPLATE );
WDF_DECLARE_CUSTOM_TYPE( ACXCIRCUITTEMPLATE );

DECLARE_HANDLE( ACXAUDIOMODULE );
WDF_DECLARE_CUSTOM_TYPE( ACXAUDIOMODULE );





WDF_EXTERN_C_END

#endif // _ACXTYPES_H_

