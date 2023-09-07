/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    miniUart.c

Abstract:

    Broadcom BCM2835 (Raspberry Pi) mini UART serial support.

--*/


#include <ntddk.h>
#include "kdcom.h"
#include "HardwareLayer.h"

#define AUX_MU_IO_REG       0x40        // Data register
#define AUX_MU_IER_REG      0x44        // Interrupt Enable register
#define AUX_MU_LCR_REG      0x4C        // Line Control register
#define AUX_MU_STAT_REG     0x64        // Line status register

#define AUX_MU_IER_TXE      0x00000001  // TX FIFO empty interrupt
#define AUX_MU_IER_RXNE     0x00000002  // RX FIFO not empty interrupt

//
// 8bit mode:
// There is BCM2635 datasheet errata: 
// http://elinux.org/BCM2835_datasheet_errata.
// To get 8 bits we need to set the value of 
// 0x03, not 0x01.
//

#define AUX_MU_LCR_8BIT     0x00000003

#define AUX_MU_STAT_RXNE    0x00000001 // RX FIFO not empty
#define AUX_MU_STAT_TXNF    0x00000002 // TX FIFO not full

VOID
Bcm2835Reinitialize(
    _In_ PCPPORT PortPtr,
    _In_ const ULONG BaudRate
    );

BOOLEAN
Bcm2835InitializePort (
    _In_opt_ _Null_terminated_ PCHAR LoadOptions,
    _In_ PDEBUG_PORT DebugPortPtr
    )

/*++

Routine Description:

    We do not touch the serial port since it is pre-configured by UEFI.
    We cannot set the baud rate since we do not have the UART clock value.
    Moreover with Rpi2/3 UEFI sets the baud rate to 921600 BPS, while BcdEdit
    limits it to 115200 BPS.

Arguments:

    LoadOptions - Supplies a pointer to the load options string.

    DebugPortPtr - Supplies debug port structure.

--*/

{

    UNREFERENCED_PARAMETER(LoadOptions);

    Bcm2835Reinitialize(&DebugPortPtr->Port, DebugPortPtr->Port.Baud);
    return TRUE;
}

VOID
Bcm2835Reinitialize(
    _In_ PCPPORT PortPtr,
    _In_ const ULONG BaudRate
    )

/*++

Routine Description:

    Re-initialize the port. 
    We cannot set the baud rate since we do not have the UART clock value.
    Moreover with Rpi2/3 UEFI sets the baud rate to 921600 BPS, while BcdEdit
    limits it to 115200 BPS.

Arguments:

    PortPtr - Address of CPPORT object

    BaudRate - The desired baud rate in BPS

--*/

{

    ULONG interruptEnableReg;

    UNREFERENCED_PARAMETER(BaudRate);
    
    //
    // Disable interrupts
    //

    interruptEnableReg = 
        READ_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_IER_REG));

    interruptEnableReg &= ~(AUX_MU_IER_TXE | AUX_MU_IER_RXNE);
    WRITE_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_IER_REG),
                         interruptEnableReg);

    //
    // Set 8 bit mode
    //

    WRITE_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_LCR_REG),
                         AUX_MU_LCR_8BIT);

    //
    // Mini UART RX/TX are always enabled!
    //

    return;
}

VOID
Bcm2835PutByte (
    _In_ PCPPORT PortPtr,
    const UCHAR Byte,
    _Inout_ const BOOLEAN *CompDbgPortsPresentPtr
    )

/*++

    Routine Description:

        Write a byte out to the specified com port.

    Arguments:

        PortPtr - Address of CPPORT object

        Byte - data to emit

        CompDbgPortsPresentPtr - Address of a BOOLEAN value set to
            whether the debugger ports are present (TRUE) or not (FALSE).

--*/

{

    ULONG statusReg;

    if (*CompDbgPortsPresentPtr == FALSE) {
        return;
    }

    //
    //  Wait for FIFO to be not full.
    //

    do {
        statusReg = 
            READ_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_STAT_REG));

    } while ( (statusReg & AUX_MU_STAT_TXNF) == 0);

    //
    // Send the byte
    //

    WRITE_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_IO_REG),
                         (ULONG)Byte);

    return;
}

_Success_(return == CP_GET_SUCCESS)
USHORT
Bcm2835GetByte (
    _In_ PCPPORT PortPtr,
    _Out_ PUCHAR BytePtr,
    _Inout_ BOOLEAN *CompDbgPortsPresentPtr
    )

/*++

    Routine Description:

        Fetch a byte and return it.

    Arguments:

        PortPtr - address of port object that describes HW port

        BytePtr - address of variable to hold the result

        CompDbgPortsPresentPtr - Address of a BOOLEAN value set to
            whether the debugger ports are present (TRUE) or not (FALSE).

    Return Value:

        CP_GET_SUCCESS if data returned.

        CP_GET_NODATA if no data available, but no error.

--*/

{

    ULONG dataReg;
    ULONG statusReg;

    //
    // Check to make sure the CPPORT we were passed has been initialized.
    // (The only time it won't be initialized is when the kernel debugger
    // is disabled, in which case we just return.)
    //

    if (PortPtr->Address == NULL) {
        return CP_GET_NODATA;
    }

    if (*CompDbgPortsPresentPtr == FALSE) {
        Bcm2835Reinitialize(PortPtr, PortPtr->Baud);
        *CompDbgPortsPresentPtr = TRUE;
    }

    //
    // Get FIFO status.
    //

    statusReg = 
        READ_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_STAT_REG));

    //
    // Is at least one character available?
    //

    if ((statusReg & AUX_MU_STAT_RXNE) == 0) {
        return CP_GET_NODATA;
    }

    //
    // Fetch the data byte
    //

    dataReg = READ_REGISTER_ULONG((PULONG)(PortPtr->Address + AUX_MU_IO_REG));
    *BytePtr = (UCHAR)(dataReg & (ULONG)0xFF);

    return CP_GET_SUCCESS;
}


KD_HARDWARE_LAYER_DRIVER Bcm2835HardwareLayerDriver = {
    Bcm2835InitializePort,
    Bcm2835PutByte,
    Bcm2835GetByte,
    0, // no transformation while reading or writing to register
};
