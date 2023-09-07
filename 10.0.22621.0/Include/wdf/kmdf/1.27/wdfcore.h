/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name:

    wdfcore.h

Abstract:

    This is the main driver framework.

Environment:

    kernel mode only

Revision History:

--*/

//
// NOTE: This header is generated by stubwork.  Please make any 
//       modifications to the corresponding template files 
//       (.x or .y) and use stubwork to regenerate the header
//

#ifndef _WDFCORE_H_
#define _WDFCORE_H_

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



#if (NTDDI_VERSION >= NTDDI_WIN2K)



//
// Time conversion related macros
//
//                                                      to    to     to
//                                                      us    ms     sec
#define WDF_TIMEOUT_TO_SEC              ((LONGLONG) 1 * 10 * 1000 * 1000)
#define WDF_TIMEOUT_TO_MS               ((LONGLONG) 1 * 10 * 1000)
#define WDF_TIMEOUT_TO_US               ((LONGLONG) 1 * 10)
LONGLONG
FORCEINLINE
WDF_REL_TIMEOUT_IN_SEC(
    _In_ ULONGLONG Time
    )
{
    return Time * -1 * WDF_TIMEOUT_TO_SEC;
}

LONGLONG
FORCEINLINE
WDF_ABS_TIMEOUT_IN_SEC(
    _In_ ULONGLONG Time
    )
{
    return Time *  1 * WDF_TIMEOUT_TO_SEC;
}

LONGLONG
FORCEINLINE
WDF_REL_TIMEOUT_IN_MS(
    _In_ ULONGLONG Time
    )
{
    return Time * -1 * WDF_TIMEOUT_TO_MS;
}

LONGLONG
FORCEINLINE
WDF_ABS_TIMEOUT_IN_MS(
    _In_ ULONGLONG Time
    )
{
    return Time *  1 * WDF_TIMEOUT_TO_MS;
}

LONGLONG
FORCEINLINE
WDF_REL_TIMEOUT_IN_US(
    _In_ ULONGLONG Time
    )
{
    return Time * -1 * WDF_TIMEOUT_TO_US;
}

LONGLONG
FORCEINLINE
WDF_ABS_TIMEOUT_IN_US(
    _In_ ULONGLONG Time
    )
{
    return Time *  1 * WDF_TIMEOUT_TO_US;
}

//
// Rounding functions
//
size_t
FORCEINLINE
WDF_ALIGN_SIZE_DOWN(
    _In_ size_t Length,
    _In_ size_t AlignTo
    )
{
    return Length & ~(AlignTo - 1);
}

size_t
FORCEINLINE
WDF_ALIGN_SIZE_UP(
    _In_ size_t Length,
    _In_ size_t AlignTo
    )
{
    return WDF_ALIGN_SIZE_DOWN(Length + AlignTo - 1, AlignTo);
}


//
// Pointer math
//
#define WDF_PTR_ADD_OFFSET_TYPE(_ptr, _offset, _type) \
    ((_type) (((PUCHAR) (_ptr)) + (_offset)))

#define WDF_PTR_ADD_OFFSET(_ptr, _offset) \
        WDF_PTR_ADD_OFFSET_TYPE(_ptr, _offset, PVOID)

#define WDF_PTR_GET_OFFSET(_base, _addr) \
        (size_t) (((PUCHAR) _addr) - ((PUCHAR) _base))

#if (OSVER(NTDDI_VERSION) == NTDDI_WIN2K)
//
// These definitions are necessary for building under a Win2K Environment.
//
#ifndef DECLARE_UNICODE_STRING_SIZE
#define DECLARE_UNICODE_STRING_SIZE(_var, _size) \
WCHAR _var ## _buffer[_size]; \
UNICODE_STRING _var = { 0, _size * sizeof(WCHAR) , _var ## _buffer }
#endif

#undef DECLARE_CONST_UNICODE_STRING
#define DECLARE_CONST_UNICODE_STRING(_variablename, _string)      \
const WCHAR _variablename ## _buffer[] = _string;                 \
__pragma(warning(suppress:4204)) __pragma(warning(suppress:4221)) \
const UNICODE_STRING _variablename = { sizeof(_string) - sizeof(WCHAR), sizeof(_string), (PWSTR) _variablename ## _buffer }

#endif // (OSVER(NTDDI_VERSION) == NTDDI_WIN2K)



#endif // (NTDDI_VERSION >= NTDDI_WIN2K)


WDF_EXTERN_C_END

#endif // _WDFCORE_H_

