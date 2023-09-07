//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Common definitions for the Wex.Common.dll.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#if !defined(__WEX_COMMON_H__) && defined(_WIN32)
#define __WEX_COMMON_H__

#include <basetyps.h>

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

#define WEXCOMMON_STDCALL __stdcall

#if defined(UNIT_TESTING)
# define UNIT_TEST_CLASS(__class) __if_exists(__class) { friend class __class; }
#else
# define UNIT_TEST_CLASS(__class)
#endif

#endif // #if !defined(__WEX_COMMON_H__) && defined(_WIN32)
