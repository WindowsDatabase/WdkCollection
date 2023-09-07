//----------------------------------------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#if defined(_WIN32)
#include <basetyps.h>
#endif // #if defined(_WIN32)

#if defined(_MSC_VER) && _MSC_VER < 1910
# error Unsupported version of Visual Studio. Please use VS 2017 or later.
#endif

#if defined(WEXLOGGER_UNIT_TESTING)
# define WEXLOGGER_API
#elif defined(_WIN32)
# if defined(WEXLOGGER_EXPORTS)
#  define WEXLOGGER_API __declspec(dllexport)
# else
#  define WEXLOGGER_API __declspec(dllimport)
# endif
#else
# define WEXLOGGER_API __attribute__ ((visibility ("default")))
#endif

#if defined(_WIN32)
# define WEXLOGGER_STDCALL __stdcall
#else
# define WEXLOGGER_STDCALL
#endif

#if defined(UNIT_TESTING)
# define UNIT_TEST_CLASS(__class) __if_exists(__class) { friend class __class; }
#else
# define UNIT_TEST_CLASS(__class)
#endif
