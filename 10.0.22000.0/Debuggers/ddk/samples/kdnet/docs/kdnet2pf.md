# **Problem:**

When enabling kernel debugging on a NIC, the kernel debugging support
(KDNIC.sys\<-\>KDNET\<-\>KDNET\<-\>Ext. module) takes over the physical
device to provide both a kernel debugging and network connection on the
box. This works fine on consumer low bandwidth NICs (1-10Gbps), but on
high throughput devices that support 10-40+ Gbps the kernel debugging
extensibility modules that talk to the hardware generally cannot keep up
with the amount of traffic that comes from Windows networking stack.
This will result in a continuous network degradation that eventually
will render the entire system to be unresponsive.

# **KDNET + Multiple PF goals:**

-   The rationale behind the Multiple PF (2-PF) functionality is to
    add/assign a new PF to the original PCI network port (e.g.
    Bus.dev.**fun0.0**).

-   The new added PF (e.g. bus.dev.fun0.1) will be used only by KDNET to
    route Debugger packets to/from the target.

-   The original PF will be used by the Windows inbox NIC driver to
    route the Windows networking packets (TCP/IP) .

-   This way both drivers can work in parallel w/o interfering with each
    other work (please see below pics, side by side performance
    numbers).

-   This functionality will need to be provided by the NIC firmware.

-   Both drivers will run over the partitioned PCI configuration space:

-   Windows Inbox driver will run out of the original network port at
    bus.dev.**fun0.0**

-   KDNET-KDNET-Ext. module will run out of the added PF at
    bus.dev.**fun0.1**,

              This way ensures that the Windows inbox NIC driver does
not get impacted by sharing the NIC with KDNET.

-   This functionality would be easily configured.

> User mode tool (e.g. kdnet.exe) configures the 2-PF feature via the
> Windows Inbox driver by adding specific IOCTL codes to add/remove
> KDNET PF.

# **Multiple PFs feature design requirements:**

1.  The KDNET 2-PF feature needs to work for all current KD scenarios
    whether it is the pre-NT OS (e.g. Boot Manager, OS loader,
    WinResume, hyper-v, etc.), NT OS, or Windows Desktop.

2.  Rebooting the system will be required when adding a new PF for a
    device results in a change needed to the BCD configuration for
    debugging settings. This means that the configuration for an
    additional PF must be persistent across boots.

3.  The KDNET 2-PF should be used only by the debugger to ensure that
    there is not any other Windows/UEFI ethernet driver
    accessing/running from the PCI 2-PF location when the debugger owns
    the debug device (the 2-PF location is pointed by
    dbgsettings::busparams).

4.  Windows or UEFI Ethernet drivers cannot run out of the added KDNET
    2-PF even when KDNET is not enabled in the system.

5.  The 2-PF feature should support a dynamic mechanism for
    adding/enabling and removing/disabling the functionality on the
    current NIC.

6.  The Windows miniport drivers will implement the 2-PF feature via
    servicing the following NDIS OIDs:

> OID_KDNET_ENUMERATE_PFS -- Enumerates PFs on the current bus.dev.fun
> (BDF), where the miniport driver is running.
>
> OID_KDNET_ADD_PF - Adds a PF to the current BDF, where the miniport
> driver is running.
>
> OID_KDNET_REMOVE_PF -- Removes the added PF, from the passed in BDF.
>
> OID_KDNET_QUERY_PF_INFORMATION - Queries PF information data from the
> passed in BDF.

The OIDs and their structures are defined in ntddndis.h and kdnetpf.h
files that are released with the public WDK.

Please see the details below on Input/Output parameters for each OID
(described in the kdnetpf.h file).

7.  KD should be configured via the KDNET 2-PF feature on NICS where
    multiple PF feature is available, and the NIC enables 2-PF
    functionality by following all above requirements.

# **KDNET configurations:**

![](./kdnet2pfpic.png?raw=true)

# **KDNET Multiple PF Interface for Windows NIC Drivers**

-   Miniport drivers will need to implement the handling of the
    following NDIS OIDs:

    -   OID_KDNET_ENUMERATE_PFS

    -   OID_KDNET_ADD_PF

    -   OID_KDNET_REMOVE_PF

    -   OID_KDNET_QUERY_PF_INFORMATION

-   These OIDs and structures are populated in the ntddndis.h and
    kdnetpf.h files in the public WDK release on this path:

    -   e.g. \<public WDK root directory\>:\\ddk\\inc\\ndis

    -   These files also are populated by installing the Windows SDK,
        and can be found on \<\>:\\Program Files (x86)\\Windows
        Kits\\10\\Include\\10.0.21301.0\\shared

-   The client tool (kdnet.exe) will use a private NDIS IOCTL to route
    the KDNET 2-PF NDIS OIDs to the miniport drivers.

**The Multiple PF feature is operated by using these NDIS OIDs.**

1.  Enumerate PFs on the miniport BDF primary port (OID:
    ***OID_KDNET_ENUMERATE_PFS***, see below definition):

-   This returns a list of all BDFs associated to the given primary port
    from where the miniport driver is running from -- port is
    represented by the bus.dev.fun (BDF).

-   The list of PFs will be returned to the client via a NDIS Query
    operation.

-   The ***OID_KDNET_ENUMERATE_PFS*** OID is associated with the
    **NDIS_KDNET_ENUMERATE_PFS** structure.

-   The ***OID_KDNET_ENUMERATE_PFS*** driver handler will return a
    buffer containing the PFs list with each PF element described by the
    type **NDIS_KDNET_PF_ENUM_ELEMENT**.

> The PfNumber field contains the PF Function Number (e.g.
> bus.dev.**fun**)
>
> The PfState field contains the PF state possible values- each element
> type described by **NDIS_KDNET_PF_STATE** enum.
>
> NDIS_KDNET_PF_STATE::NdisKdNetPfStatePrimary - This is a primary PF
> and it's usually used only by the miniport driver.
>
> NDIS_KDNET_PF_STATE::NdisKdnetPfStateEnabled - This is an added PF,
> and it's currently used by KDNET.
>
> NDIS_KDNET_PF_STATE::NdisKdnetPfStateConfigured -- This is an added
> PF, but it's is currently only added/configured,
>
> But it's not been used by KDNET yet, since FW NIC has not been
> resetted.

-   If the PF list output buffer size is not large enough to allocate
    the actual PFs list, then the OID handler needs to return:
    E_NOT_SUFFICIENT_BUFFER error return value, together with the
    required buffer size, so the client tool can allocate the required
    size buffer, and then the client can make another call with the
    correct buffer size allocated.

2.  Add PCI PF to the miniport BDF primary port (OID:
    ***OID_KDNET_ADD_PF*** see below definition)

-   Add a PF to the miniport primary port -- port is represented by the
    BDF.

-   The newly added PF will be returned to the client via a NDIS Query
    operation.

-   The ***OID_KDNET_ADD_PF*** OID is associated with the
    **NDIS_KDNET_ADD_PF** structure.

-   The ***OID_KDNET_ADD_PF*** driver handler will return an ULONG
    containing the *added* PF function number.

> ***Note***: The added PF can be used exclusively by KDNET, so Windows
> NIC drivers are rigged to expressly \*NOT\* run on an added PF, so
> this also applies when KDNET is \*NOT\* enabled on the system and the
> PF has been added to the port.

3.  Remove PCI PF (OID: ***OID_KDNET_REMOVE_PF***, see below definition)

-   Remove a PF from the ***given port*** - port is represented
    by the BDF.

-   The ***OID_KDNET_REMOVE_PF*** OID is associated with the
    **NDIS_KDNET_REMOVE_PF** structure

-   The ***OID_KDNET_REMOVE_PF*** OID has an input BDF port and returns
    an ULONG containing the *removed* PF function number via a NDIS
    Method operation.

-   This function will succeed only on the PFs that has been added via
    using the ***OID_KDNET_ADD_PF*** OID.

4.  Query PCI PF information (OID: ***OID_KDNET_QUERY_PF_INFORMATION***,
    see below definition)

-   This OID code allows querying specific PF data on a ***given
    port*** -- port is represented by the BDF.

-   The requested PF information will be returned to the client via a
    NDIS Method operation.

-   The ***OID_KDNET_QUERY_PF_INFORMATION*** OID is associated with the
    **NDIS_KDNET_QUERY_PF_INFORMATION** structure.

-   The ***OID_KDNET_QUERY_PF_INFORMATION*** OID has an input BDF port
    and returns a buffer containing the following data:

    -   MAC Address: Network address of the assigned new KDNET PF if
        there is any.

    -   Usage Tag: Describes the entity that owns the PF port -- it
        contains a constant value described by
        **NDIS_KDNET_PF_USAGE_TAG** enum

    -   Maximum Number of PFs: Contains an ULONG with the maximum number
        of PFs that can be added to the given BDF.

    -   Device ID: Contains the device ID associated to the given BDF
        port. This is required for cases where the NIC FW assigns a new
        device ID to the new added KDNET PF port.

# **NDIS OIDs for KDNET on 2-PF:**

-   ***Ntddndis.h*** file defines the OIDs:

```cpp
#if (NDIS_SUPPORT_NDIS686)

 //

 // Optional OIDs to handle network multiple PF feature.

 //

#define OID_KDNET_ENUMERATE_PFS 0x00020222

#define OID_KDNET_ADD_PF 0x00020223

#define OID_KDNET_REMOVE_PF 0x00020224

#define OID_KDNET_QUERY_PF_INFORMATION 0x00020225

#endif // (NDIS_SUPPORT_NDIS686)
```

-   ***Kdnetpf.h*** file describes the type and structures
    associated with the NDIS OIDs:

```cpp
#if (NDIS_SUPPORT_NDIS686)

 //
 // Used to query/add/remove Physical function on a network port.
 // These structures are used by these OIDs:
 // OID_KDNET_ENUMERATE_PFS
 // OID_KDNET_ADD_PF
 // OID_KDNET_REMOVE_PF
 // OID_KDNET_QUERY_PF_INFORMATION
 // These OIDs handle PFs that are primary intended to be used by KDNET.
 //

 //
 // PCI location of the port to query
 //

 typedef struct _NDIS_KDNET_BDF

 {

 ULONG SegmentNumber;

 ULONG BusNumber;

 ULONG DeviceNumber;

 ULONG FunctionNumber;

 ULONG Reserved;

 } NDIS_KDNET_BDF, *PNDIS_KDNET_PCI_BDF;

 //
 // PF supported states.
 //

 typedef enum _NDIS_KDNET_PF_STATE
 {
 NdisKdNetPfStatePrimary = 0x0,
 NdisKdnetPfStateEnabled = 0x1,
 NdisKdnetPfStateConfigured = 0x2,
 } NDIS_KDNET_PF_STATE,*PNDIS_KDNET_PF_STATE;

 //
 // PF Usage Tag
 // Used to indicate the entity that owns the PF.
 // Used by the query NdisKdnetQueryUsageTag.
 //

typedef enum _NDIS_KDNET_PF_USAGE_TAG
{
 NdisKdnetPfUsageUnknown = 0x0,
 NdisKdnetPfUsageKdModule = 0x1,
} NDIS_KDNET_PF_USAGE_TAG,*PNDIS_KDNET_PF_USAGE_TAG;

//
// PF element array structure
//

typedef struct _NDIS_KDNET_PF_ENUM_ELEMENT
{
NDIS_OBJECT_HEADER Header;

//
// PF value (e.g. if \<bus.dev.fun\>, then PF value = fun)
//

ULONG PfNumber;

//
// The PF state value (defined by NDIS_KDNET_PF_STATE)
//

NDIS_KDNET_PF_STATE PfState;

} NDIS_KDNET_PF_ENUM_ELEMENT, *PNDIS_KDNET_PF_ENUM_ELEMENT;

#define NDIS_KDNET_PF_ENUM_ELEMENT_REVISION_1 1
#define NDIS_SIZEOF_KDNET_PF_ENUM_ELEMENT_REVISION_1 \\
RTL_SIZEOF_THROUGH_FIELD(NDIS_KDNET_PF_ENUM_ELEMENT, PfState)

//
// This structure describes the data required to enumerate the list of PF
// Used by OID_KDNET_ENUMERATE_PFS.
//

typedef struct _NDIS_KDNET_ENUMERATE_PFS
{
NDIS_OBJECT_HEADER Header;

//
// The size of each element is the sizeof(NDIS_KDNET_PF_ENUM_ELEMENT)
//

ULONG ElementSize;

//
// The number of elements in the returned array
//

ULONG NumberOfElements;

//
// Offset value to the first element of the returned array.
// Each array element is defined by NDIS_KDNET_PF_ENUM_ELEMENT.
//

ULONG OffsetToFirstElement;

} NDIS_KDNET_ENUMERATE_PFS, *PNDIS_KDNET_ENUMERATE_PFS;

#define NDIS_KDNET_ENUMERATE_PFS_REVISION_1 1
#define NDIS_SIZEOF_KDNET_ENUMERATE_PFS_REVISION_1 \
RTL_SIZEOF_THROUGH_FIELD(NDIS_KDNET_ENUMERATE_PFS,OffsetToFirstElement)

//
// This structure indicates the data required to add a PF to the BDF port.
// Used by OID_KDNET_ADD_PF.
//

typedef struct _NDIS_KDNET_ADD_PF
{

NDIS_OBJECT_HEADER Header;

//
// One element containing the added PF port number
//

ULONG AddedFunctionNumber;

} NDIS_KDNET_ADD_PF, *PNDIS_KDNET_ADD_PF;

#define NDIS_KDNET_ADD_PF_REVISION_1 1
#define NDIS_SIZEOF_KDNET_ADD_PF_REVISION_1 \
RTL_SIZEOF_THROUGH_FIELD(NDIS_KDNET_ADD_PF, AddedFunctionNumber)

//
// This structure indicates the data required to remove a PF from the BDF port.
// Used by OID_KDNET_REMOVE_PF.
//

typedef struct _NDIS_KDNET_REMOVE_PF
{

NDIS_OBJECT_HEADER Header;

//
// PCI location that points to the PF that needs to be removed
//

NDIS_KDNET_BDF Bdf;

//
// One element containing the removed PF port
//

ULONG FunctionNumber;

} NDIS_KDNET_REMOVE_PF, *PNDIS_KDNET_REMOVE_PF;

#define NDIS_KDNET_REMOVE_PF_REVISION_1 1
#define NDIS_SIZEOF_KDNET_REMOVE_PF_REVISION_1 \
RTL_SIZEOF_THROUGH_FIELD(NDIS_KDNET_REMOVE_PF, FunctionNumber)

//
// This structure describes the data required to query the PF management data
// Used by OID_KDNET_QUERY_PF_INFORMATION
//

typedef struct _NDIS_KDNET_QUERY_PF_INFORMATION
{
NDIS_OBJECT_HEADER Header;

//
// PF PCI location to query for
//

NDIS_KDNET_BDF Bdf;

//
// PF assigned MAC address
//

UCHAR NetworkAdddress[6];

//
// PF Usage tag described by NDIS_KDNET_PF_USAGE_TAG
//

ULONG UsageTag;

//
// Maximum number of Pfs that can be associated to the Primary BDF.
//

ULONG MaximumNumberOfSupportedPfs;

//
// KDNET PF device ID (Used if there is a new added PF and
// the FW assigns a new DeviceID to the added KDNET PF)
//

ULONG DeviceId;

} NDIS_KDNET_QUERY_PF_INFORMATION, *PNDIS_KDNET_QUERY_PF_INFORMATION;

#define NDIS_KDNET_QUERY_PF_INFORMATION_REVISION_1 1

#define NDIS_SIZEOF_KDNET_QUERY_PF_INFORMATION_REVISION_1 \
RTL_SIZEOF_THROUGH_FIELD(NDIS_KDNET_QUERY_PF_INFORMATION, DeviceId)

#endif // (NDIS_SUPPORT_NDIS686)
```