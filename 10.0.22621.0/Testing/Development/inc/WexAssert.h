//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>WexCommon assert implementation.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#if defined(_WIN32)
#include "WexDebug.h"

#if !defined(WEX_ASSERT)
# if defined(NDEBUG)

#  define WEX_ASSERT(___condition, ___message)

# else

#  define WEX_ASSERT(___condition, ___message) (void)( \
        (!!(___condition)) || \
        (WEX::Common::Debug::Assert(false, WIDEN(#___condition), (___message), __WFILE__, __WFUNCTION__, __LINE__), 0) \
    )

# endif /* NDEBUG */
#endif /* WEX_ASSERT */

#endif // #if defined(_WIN32)
