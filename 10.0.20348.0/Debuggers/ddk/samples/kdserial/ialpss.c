/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ialpss.c

Abstract:

    This module contains support for the Intel Low Power Sub System UART.

--*/

// ------------------------------------------------------------------- Includes

#include "common.h"
#include "kdcom.h"

// ---------------------------------------------------------------- Definitions

//
// Software reset register offset.
//

#define IALPSS_UART_RESET_REGISTER_OFFSET 0x204

//
// Reset register value.
//

#define IALPSS_UART_REG_VAL_ASSERT_RESET 0x0
#define IALPSS_UART_REG_VAL_DEASSERT_RESET 0x3
#define IALPSS_UART_RESET_MASK 0x3

#define IALPSS_CONFIG_SPACE_OFFSET 0x1000
#define IALPSS_PMCSR_OFFSET 0x84

// ------------------------------------------------------------ Data Structures

typedef enum {
    PowerStateD0,
    PowerStateD3
} CONTROLLER_POWER_STATE, *PCONTROLLER_POWER_STATE;

#pragma warning(push)
#pragma warning(disable:4214) // bit field types other than int
#pragma warning(disable:4201) // nameless struct/union

//
// The following struct is adapted from minkernel\ntos\inc\pci_x.h.
//

typedef union _PCI_PMCSR {
    struct {
        USHORT      PowerState:2;
        USHORT      Rsvd1:1;
        USHORT      NoSoftReset:1;
        USHORT      Rsvd2:4;
        USHORT      PMEEnable:1;
        USHORT      DataSelect:4;
        USHORT      DataScale:2;
        USHORT      PMEStatus:1;
    };
    USHORT AsUSHORT;
} PCI_PMCSR, *PPCI_PMCSR;

#pragma warning(pop)

// -------------------------------------------------------------------- Globals

//
// Address of the UART's Power Management Control/Status Register.
//

PUSHORT IaLpssPmcsr = NULL;

// ----------------------------------------------------------------- Prototypes

BOOLEAN
Uart16550InitializePortCommon (
    _In_opt_ _Null_terminated_ PCHAR LoadOptions,
    _Inout_ PCPPORT Port,
    BOOLEAN MemoryMapped,
    UCHAR AccessSize,
    UCHAR BitWidth
    );

BOOLEAN
Uart16550SetBaud (
    _Inout_ PCPPORT Port,
    ULONG Rate
    );

UART_STATUS
Uart16550GetByte (
    _Inout_ PCPPORT Port,
    _Out_ PUCHAR Byte
    );

UART_STATUS
Uart16550PutByte (
    _Inout_ PCPPORT Port,
    UCHAR Byte,
    BOOLEAN BusyWait
    );

BOOLEAN
Uart16550RxReady (
    _Inout_ PCPPORT Port
    );

// ------------------------------------------------------------------ Functions

BOOLEAN
IaLpssInitializePort (
    _In_opt_ _Null_terminated_ PCHAR LoadOptions,
    _Inout_ PCPPORT Port,
    BOOLEAN MemoryMapped,
    UCHAR AccessSize,
    UCHAR BitWidth
    )

/*++

Routine Description:

    This routine performs the initialization of an Intel LPSS UART.

Arguments:

    LoadOptions - Optional load option string.

    Port - Supplies a pointer to a CPPORT object which will be filled in as
        part of the port initialization.

    MemoryMapped - Unused.

    AccessSize - Unused.

    BitWidth - Unused.

Return Value:

    TRUE if the port has been successfully initialized, FALSE otherwise.

--*/

{

    BOOLEAN Success;

    UNREFERENCED_PARAMETER(AccessSize);
    UNREFERENCED_PARAMETER(BitWidth);
    UNREFERENCED_PARAMETER(MemoryMapped);

    Port->Flags = 0;
    Success = Uart16550InitializePortCommon(LoadOptions,
                                            Port,
                                            TRUE,
                                            AcpiGenericAccessSizeByte,
                                            8);

    if (Success != FALSE) {
        IaLpssPmcsr = (PUSHORT)(Port->Address +
                                IALPSS_CONFIG_SPACE_OFFSET +
                                IALPSS_PMCSR_OFFSET);
    }

    return Success;
}

UART_STATUS
IaLpssReadPmcsr (
    _Out_ PPCI_PMCSR Pmcsr
    )

/*++

Routine Description:

    This routine reads the IALPSS PMCSR from hardware.

Arguments:

    Pmcsr - Supplies a pointer to the PMCSR structure to be filled in.

Return Value:

    UART_STATUS code.

--*/

{

    if ((IaLpssPmcsr == NULL) || (Pmcsr == NULL)) {
        return UartError;
    }

    Pmcsr->AsUSHORT = READ_REGISTER_USHORT(IaLpssPmcsr);
    return UartSuccess;
}

UART_STATUS
IaLpssWritePmcsr (
    _In_ PPCI_PMCSR Pmcsr
    )

/*++

Routine Description:

    This routine writes the IALPSS UART's PMCSR.

Arguments:

    Pmcsr - Supplies a pointer to the PMCSR structure to be written.

Return Value:

    UART_STATUS code.

--*/

{

    if ((IaLpssPmcsr == NULL) || (Pmcsr == NULL)) {
        return UartError;
    }

    WRITE_REGISTER_USHORT(IaLpssPmcsr, Pmcsr->AsUSHORT);
    return UartSuccess;
}

UART_STATUS
IaLpssPciSetPower (
    _In_ CONTROLLER_POWER_STATE PowerState
    )

/*++

Routine Description:

    This routine transitions the IALPSS UART to the D0 or D3 power state via
    its PMCSR.

Arguments:

    PowerState - Supplies the desired power state.

Return Value:

    UART_STATUS code.

--*/

{

    UART_STATUS Status;
    PCI_PMCSR Pmcsr;

    Status = UartSuccess;
    if (PowerState == PowerStateD0) {

        //
        // Get current PMCSR.
        //

        Status = IaLpssReadPmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        if (Pmcsr.PowerState == 0) {
            Status = UartError;
            goto IaLpssPciSetPowerEnd;
        }

        //
        // Update and write new PMCSR value.
        //

        Pmcsr.PowerState = 0;
        Pmcsr.PMEStatus = 1;

        Status = IaLpssWritePmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        //
        // Read back PMCSR and check if the write was successful.
        //

        Status = IaLpssReadPmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        if (Pmcsr.PowerState != 0) {
            Status = UartError;
            goto IaLpssPciSetPowerEnd;
        }

    } else if (PowerState == PowerStateD3) {

        //
        // Get current PMCSR.
        //

        Status = IaLpssReadPmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        if (Pmcsr.PowerState == 3) {
            Status = UartError;
            goto IaLpssPciSetPowerEnd;
        }

        //
        // Update and write new PMCSR value.
        //

        Pmcsr.PowerState = 3;
        Pmcsr.PMEStatus = 0;

        Status = IaLpssWritePmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        //
        // Read back PMCSR and check if the write was successful.
        //

        Status = IaLpssReadPmcsr(&Pmcsr);
        if (Status != UartSuccess) {
            goto IaLpssPciSetPowerEnd;
        }

        if (Pmcsr.PowerState != 3) {
            Status = UartError;
            goto IaLpssPciSetPowerEnd;
        }

    } else {

        //
        // Invalid power state.
        //

        Status = UartError;
        goto IaLpssPciSetPowerEnd;
    }

IaLpssPciSetPowerEnd:
    return Status;

}

ULONG
IaLpssReadResetRegister (
    _In_ PCPPORT Port
    )

/*++

Routine Description:

    This routine reads from the software reset register.

Arguments:

    Port - Supplies a pointer to a CPPORT object.

Return Value:

    The value of the software reset register.

--*/

{

    PULONG ResetReg;

    ResetReg = (PULONG)(Port->Address + IALPSS_UART_RESET_REGISTER_OFFSET);
    return READ_REGISTER_ULONG(ResetReg);
}

VOID
IaLpssWriteResetRegister (
    _In_ PCPPORT Port,
    _In_ ULONG Value
    )

/*++

Routine Description:

    This routine writes to the software reset register.

Arguments:

    Port - Supplies a pointer to a CPPORT object.

    Value - Supplies the value to be written to software reset register.

Return Value:

    None.

--*/

{

    PULONG ResetReg;

    ResetReg = (PULONG)(Port->Address + IALPSS_UART_RESET_REGISTER_OFFSET);
    WRITE_REGISTER_ULONG(ResetReg, Value);
}

UART_STATUS
IaLpssSetPowerD3 (
    _In_ PCPPORT Port
    )

/*++

Routine Description:

    This routine performs the steps needed to safely transition the UART to the
    low-power state D3.

Arguments:

    Port - Supplies a pointer to a CPPORT object.

Return Value:

    UartSuccess on successful transition to D3.
    UartError on failure.
    UartNotReady if the port is not initalized.

--*/

{

    UCHAR Mcr;
    ULONG ResetValue;
    UART_STATUS Status;

    if ((Port == NULL) || (Port->Address == NULL)) {
        Status = UartNotReady;
        goto IaLpssSetPowerD3End;
    }

    //
    // Check to ensure reset is de-asserted first.
    //

    ResetValue = IaLpssReadResetRegister(Port) & IALPSS_UART_RESET_MASK;
    if (ResetValue != IALPSS_UART_REG_VAL_DEASSERT_RESET) {
        Status = UartError;
        goto IaLpssSetPowerD3End;
    }

    //
    // De-assert aux output 2.
    //

    Mcr = Port->Read(Port, COM_MCR);
    Port->Write(Port, COM_MCR, Mcr & ~SERIAL_MCR_OUT2);

    //
    // Disable the FIFO.
    //

    Port->Write(Port, COM_FCR, 0);

    //
    // Assert reset.
    //

    IaLpssWriteResetRegister(Port, IALPSS_UART_REG_VAL_ASSERT_RESET);

    //
    // Transition to D3.
    //

    Status = IaLpssPciSetPower(PowerStateD3);
    if (Status != UartSuccess) {
        goto IaLpssSetPowerD3End;
    }

IaLpssSetPowerD3End:
    return Status;
}

UART_STATUS
IaLpssSetPowerD0 (
    _In_ PCPPORT Port
    )

/*++

Routine Description:

    This routine performs the steps needed to safely transition the UART to the
    working state D0.

Arguments:

    Port - Supplies a pointer to a CPPORT object.

Return Value:

    UartSuccess on successful transition to D0.
    UartError on failure.
    UartNotReady if the port is not initalized.

--*/

{

    ULONG ResetValue;
    UART_STATUS Status;
    BOOLEAN Success;

    if ((Port == NULL) || (Port->Address == NULL)) {
        Status = UartNotReady;
        goto IaLpssSetPowerD0End;
    }

    //
    // Transition to D0.
    //

    Status = IaLpssPciSetPower(PowerStateD0);
    if (Status != UartSuccess) {
        goto IaLpssSetPowerD0End;
    }

    //
    // Check to ensure reset is asserted.
    //

    ResetValue = IaLpssReadResetRegister(Port) & IALPSS_UART_RESET_MASK;
    if (ResetValue != IALPSS_UART_REG_VAL_ASSERT_RESET) {
        Status = UartError;
        goto IaLpssSetPowerD0End;
    }

    //
    // De-assert reset, PPPR register.
    //

    IaLpssWriteResetRegister(Port, IALPSS_UART_REG_VAL_DEASSERT_RESET);

    //
    // Re-initialize UART.
    //

    Port->Flags = 0;
    Success = Uart16550InitializePortCommon(NULL,
                                            Port,
                                            TRUE,
                                            AcpiGenericAccessSizeByte,
                                            8);

    if (Success != TRUE) {
        Status = UartError;
        goto IaLpssSetPowerD0End;
    }

    Status = UartSuccess;

IaLpssSetPowerD0End:
    return Status;
}

// -------------------------------------------------------------------- Globals

UART_HARDWARE_DRIVER IaLpssHardwareDriver = {
    IaLpssInitializePort,
    Uart16550SetBaud,
    Uart16550GetByte,
    Uart16550PutByte,
    Uart16550RxReady,
    IaLpssSetPowerD0,
    IaLpssSetPowerD3
};
