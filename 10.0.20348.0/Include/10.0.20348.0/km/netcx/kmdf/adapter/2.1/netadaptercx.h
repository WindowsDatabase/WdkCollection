// Copyright (C) Microsoft Corporation. All rights reserved.

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _NETADAPTERCX_2_1_H_
#define _NETADAPTERCX_2_1_H_

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




#ifndef WDFAPI
#error include wdf.h before netadaptercx.h
#endif

#ifdef NETCX_ADAPTER_2
#error NETCX_ADAPTER_2 previously defined. NETCX_ macro prefix is reserved
#endif


#define NETCX_ADAPTER_2
#include <ndis.h>
#undef NETCX_ADAPTER_2


#define NETCX_ADAPTER_2
#include <net/extension.h>
#include <net/fragment.h>
#include <net/packet.h>
#include <net/ring.h>
#include <net/ringcollection.h>
#undef NETCX_ADAPTER_2

#include <netadaptercxtypes.h>

#ifndef NETCX_ADAPTER_PREVIEW
#include <netfuncenum.h>
#endif


#include <netadapterpacket.h>


#include <nettxqueue.h>
#include <netrxqueue.h>

#include <netdevice.h>
#include <netadapter.h>

#include <netpoweroffload.h>
#include <netpoweroffloadlist.h>
#include <netwakesource.h>
#include <netwakesourcelist.h>

#include <netadapteroffload.h>
#include <netadaptertxdemux.h>

#include <netexecutioncontext.h>


#include <netconfiguration.h>


WDF_EXTERN_C_END

#endif // _NETADAPTERCX_2_1_H_

