/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
*/
#pragma once
#if defined(_WIN32)
#include "WexDebug.h"

namespace WEX { namespace Common
{
    class PreserveLastError
    {
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
    };
} /*namespace Common*/ } /*namespace WEX*/
#endif // #if defined(_WIN32)