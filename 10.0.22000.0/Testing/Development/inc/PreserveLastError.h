/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
*/
#pragma once
#include "Wex.Common.h"
#include "WexDebug.h"

namespace WEX { namespace Common
{
    class PreserveLastError
    {
#if defined(_WIN32)
    public:
        PreserveLastError()
        : m_lastError(Debug::GetLastError())
        {
        }

        ~PreserveLastError()
        {
            Debug::SetLastError(m_lastError);
        }

    private:
        unsigned long m_lastError;
#endif // #if defined(_WIN32)
    };
} /*namespace Common*/ } /*namespace WEX*/