// Copyright (C) Microsoft Corporation. All rights reserved.

Steps to deploy test serial kernel debug transport:
1) Build project (outputs kdserial.dll)
2) Rename kdserial.dll to kdcom.dll
3) Copy kdcom.dll to C:\windows\system32\kdcom.dll on target machine (Advised
    to create a backup of original kdcom.dll before overwriting it)
4) Run "bcdedit /dbgsettings serial debugport:<port> baudrate:<rate>" on target
5) Run "bcdedit /debug on" on target
6) Run "bcdedit /bootdebug on" on target
7) Run "bcdedit /testsigning yes" on target
8) Reboot target & connect to boot debugger. This step is required for the new
    kdcom.dll to be loaded without a valid Microsoft signature. If the boot
    debugger is not enabled & connected when kdcom.dll is being loaded, winload
    will fail to load it.
    NOTE: the custom kdcom.dll will not be used for the boot debugger transport
9) Continue through winload & connect to the kernel debugger via the custom
    kdcom.dll transport.
