//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Common definitions for the Te.Common.dll.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef TECOMMON_EXPORTS
# ifndef TECOMMON_API
#  define TECOMMON_API __declspec(dllexport)
# endif
# ifndef TECOMMON_EXPORT_TEMPLATE
#  define TECOMMON_EXPORT_TEMPLATE
# endif
#else
# ifdef TECOMMON_UNIT_TESTING
#  ifndef TECOMMON_API
#   define TECOMMON_API
#  endif
#  ifndef TECOMMON_EXPORT_TEMPLATE
#   define TECOMMON_EXPORT_TEMPLATE
#  endif
# else
#  ifndef TECOMMON_API
#   ifdef _WIN32
#    define TECOMMON_API __declspec(dllimport)
#   else
#    define TECOMMON_API
#   endif
#  endif
#  ifndef TECOMMON_EXPORT_TEMPLATE
#   define TECOMMON_EXPORT_TEMPLATE extern
#  endif
# endif
#endif

#if defined(_WIN32)
#define TAEF_STDCALL __stdcall
#define TAEF_THISCALL __thiscall
#else
#define TAEF_STDCALL
#define TAEF_THISCALL
#endif

#ifdef UNIT_TESTING
# define UNIT_TEST_CLASS(__class) __if_exists(__class) { friend class __class; }
#else
# define UNIT_TEST_CLASS(__class)
#endif