// Copyright (C) Microsoft Corporation. All rights reserved.

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _NETPOWEROFFLOAD_2_2_H_
#define _NETPOWEROFFLOAD_2_2_H_

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



typedef struct _NET_POWER_OFFLOAD_ARP_PARAMETERS
{
    ULONG Size;
    ULONG Id;
    NET_IPV4_ADDRESS RemoteIPv4Address;
    NET_IPV4_ADDRESS HostIPv4Address;
    NET_ADAPTER_LINK_LAYER_ADDRESS LinkLayerAddress;
} NET_POWER_OFFLOAD_ARP_PARAMETERS;

inline
void
NET_POWER_OFFLOAD_ARP_PARAMETERS_INIT(
    _Out_ NET_POWER_OFFLOAD_ARP_PARAMETERS * Parameters
)
{
    RtlZeroMemory(Parameters, sizeof(*Parameters));
    Parameters->Size = sizeof(*Parameters);
}

typedef struct _NET_POWER_OFFLOAD_NS_PARAMETERS
{
    ULONG Size;
    ULONG Id;
    NET_IPV6_ADDRESS RemoteIPv6Address;
    NET_IPV6_ADDRESS SolicitedNodeIPv6Address;
    NET_IPV6_ADDRESS TargetIPv6Addresses[2];
    NET_ADAPTER_LINK_LAYER_ADDRESS LinkLayerAddress;
} NET_POWER_OFFLOAD_NS_PARAMETERS;

inline
void
NET_POWER_OFFLOAD_NS_PARAMETERS_INIT(
    _Out_ NET_POWER_OFFLOAD_NS_PARAMETERS * Parameters
)
{
    RtlZeroMemory(Parameters, sizeof(*Parameters));
    Parameters->Size = sizeof(*Parameters);
}

typedef enum _NET_POWER_OFFLOAD_TYPE {
    NetPowerOffloadTypeArp = 1,
    NetPowerOffloadTypeNS,
} NET_POWER_OFFLOAD_TYPE;


//
// NET Function: NetPowerOffloadGetType
//
typedef
_IRQL_requires_(PASSIVE_LEVEL)
WDFAPI
NET_POWER_OFFLOAD_TYPE
(NTAPI *PFN_NETPOWEROFFLOADGETTYPE)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPOWEROFFLOAD PowerOffload
    );

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
NET_POWER_OFFLOAD_TYPE
NetPowerOffloadGetType(
    _In_
    NETPOWEROFFLOAD PowerOffload
    )
{
    return ((PFN_NETPOWEROFFLOADGETTYPE) NetFunctions[NetPowerOffloadGetTypeTableIndex])(NetDriverGlobals, PowerOffload);
}

//
// NET Function: NetPowerOffloadGetAdapter
//
typedef
_IRQL_requires_(PASSIVE_LEVEL)
WDFAPI
NETADAPTER
(NTAPI *PFN_NETPOWEROFFLOADGETADAPTER)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPOWEROFFLOAD PowerOffload
    );

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
NETADAPTER
NetPowerOffloadGetAdapter(
    _In_
    NETPOWEROFFLOAD PowerOffload
    )
{
    return ((PFN_NETPOWEROFFLOADGETADAPTER) NetFunctions[NetPowerOffloadGetAdapterTableIndex])(NetDriverGlobals, PowerOffload);
}

//
// NET Function: NetPowerOffloadGetArpParameters
//
typedef
_IRQL_requires_(PASSIVE_LEVEL)
WDFAPI
void
(NTAPI *PFN_NETPOWEROFFLOADGETARPPARAMETERS)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPOWEROFFLOAD PowerOffload,
    _Inout_
    NET_POWER_OFFLOAD_ARP_PARAMETERS* Parameters
    );

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
void
NetPowerOffloadGetArpParameters(
    _In_
    NETPOWEROFFLOAD PowerOffload,
    _Inout_
    NET_POWER_OFFLOAD_ARP_PARAMETERS* Parameters
    )
{
    ((PFN_NETPOWEROFFLOADGETARPPARAMETERS) NetFunctions[NetPowerOffloadGetArpParametersTableIndex])(NetDriverGlobals, PowerOffload, Parameters);
}

//
// NET Function: NetPowerOffloadGetNSParameters
//
typedef
_IRQL_requires_(PASSIVE_LEVEL)
WDFAPI
void
(NTAPI *PFN_NETPOWEROFFLOADGETNSPARAMETERS)(
    _In_
    PNET_DRIVER_GLOBALS DriverGlobals,
    _In_
    NETPOWEROFFLOAD PowerOffload,
    _Inout_
    NET_POWER_OFFLOAD_NS_PARAMETERS* Parameters
    );

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
void
NetPowerOffloadGetNSParameters(
    _In_
    NETPOWEROFFLOAD PowerOffload,
    _Inout_
    NET_POWER_OFFLOAD_NS_PARAMETERS* Parameters
    )
{
    ((PFN_NETPOWEROFFLOADGETNSPARAMETERS) NetFunctions[NetPowerOffloadGetNSParametersTableIndex])(NetDriverGlobals, PowerOffload, Parameters);
}



WDF_EXTERN_C_END

#endif // _NETPOWEROFFLOAD_2_2_H_

