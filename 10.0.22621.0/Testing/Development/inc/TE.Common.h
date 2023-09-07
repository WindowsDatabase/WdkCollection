//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Common definitions for the Te.Common.dll.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "WexTypes.h"

#if !defined(TECOMMON_API)
# if defined(_WIN32)
#  define TECOMMON_API __declspec(dllimport)
# else
#  define TECOMMON_API __attribute__ ((visibility ("default")))
# endif
#endif

#if defined(UNIT_TESTING)
# define UNIT_TEST_CLASS(__class) __if_exists(__class) { friend class __class; }
#else
# define UNIT_TEST_CLASS(__class)
#endif
