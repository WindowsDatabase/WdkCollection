//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Common definitions for the Wex.Common.dll.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#ifndef __WEX_COMMON_H__
#define __WEX_COMMON_H__

#if defined(_WIN32)
#include <basetyps.h>
#endif // #if defined(_WIN32)

#if defined(WEXCOMMON_EXPORTS)
# define WEXCOMMON_API __declspec(dllexport)
# define EXPIMP_TEMPLATE
#elif defined(WEXCOMMON_UNIT_TESTING)
# define WEXCOMMON_API
# define EXPIMP_TEMPLATE
#elif defined(WEXCOMMON_STATIC)
# define WEXCOMMON_API
# define EXPIMP_TEMPLATE
#elif defined(_WIN32)
# define WEXCOMMON_API __declspec(dllimport)
# define EXPIMP_TEMPLATE extern
#else
# define WEXCOMMON_API
# define EXPIMP_TEMPLATE
#endif

#if defined(_WIN32)
# define WEXCOMMON_STDCALL __stdcall
#else
# define WEXCOMMON_STDCALL
#endif

#if defined(UNIT_TESTING)
# define UNIT_TEST_CLASS(__class) __if_exists(__class) { friend class __class; }
#else
# define UNIT_TEST_CLASS(__class)
#endif

#endif // __WEX_COMMON_H__
