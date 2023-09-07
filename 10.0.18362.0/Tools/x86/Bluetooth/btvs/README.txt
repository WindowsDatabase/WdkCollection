Bluetooth Virtual Sniffer (btvs.exe)

The Bluetooth Virtual Sniffer allows the user to view live traces in the
Frontline Protocol Analysis System or in the Ellisys Bluetooth Analyzer.

# Command Line Options

    btvs.exe [-Mode Frontline|Ellisys] [-Address 127.0.0.1] [-Port 24352]

        Mode        Optionally specify whether btvs.exe should generate traces
                        for Frontline or Ellisys. (Default: Frontline)

        Address     (Ellisys mode only) Specifies the IP address of the machine
                        running Ellisys Bluetooth Analyzer. (Default: 127.0.0.1)

        Port        (Ellisys mode only) Specifies the UDP listen port of the
                        Ellisys Bluetooth Analyzer injection API. (Default: 24352)



# Frontline Protocol Analysis System operation

Prerequisites:
1. Install Frontline Protocol Analysis System (http://www.fte.com/) on the
    machine to be traced.

Usage:
1. Run btvs.exe on the same machine. No command line options are necessary. This
    launches the Frontline Protocol Analysis System.
2. Click the `Start Capture` button (Red button on the tool bar).
3. Click `View->Frame Display` to show HCI traces.


# Ellisys Bluetooth Analyzer operation

Prerequisites:
1. Install the Ellisys Bluetooth Analyzer (http://www.ellisys.com/) software.
    Note: This does not need to be installed on the machine to be traced.
    Note: An Ellisys Bluetooth protocol analyzer device may be required for
        tracing and saving traces.
2. In `Tools->Options` in the Ellisys Bluetooth Analyzer, enable HCI injection
    services on the Injection API tab.
3. Configure recording options in `Record->Recording options` in the Ellisys
    Bluetooth Analyzer. If only HCI traces are desired, uncheck all of the
    options under `Wireless Capture`.

Usage:
1. Start Ellisys Bluetooth Analyzer.
2. Select the `HCI Overview (injection)` overview tab.
3. Click `Record`.
4. Run btvs.exe in Ellisys mode on the machine to be traced:
    btvs.exe -Mode Ellisys
    a. Optionally, if the Ellisys Bluetooth Analyzer is running on a different
        machine, or if the listen port was changed, provide the Address or Port
        on the command line (see Command Line Options, above).