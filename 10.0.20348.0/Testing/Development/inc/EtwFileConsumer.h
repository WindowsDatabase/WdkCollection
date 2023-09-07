/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
* Consumer of Etw events logged in a *.etl file 
*/
#pragma once
#include "EtwProcessor.h"

#include "EtwConsumer.h"

// Suppress warning 4481: nonstandard extension used: override specifier 'override'
#pragma warning(push)
#pragma warning(disable: 4481)

namespace Etw { namespace Processor
{
    class EtwFileConsumer : public EtwConsumer
    {
    public:
        EtwFileConsumer(const wchar_t* pszFileName)
        {
            Initialize(pszFileName);
        }

        ETWPROCESSOR_API ~EtwFileConsumer();

        ETWPROCESSOR_API virtual HRESULT OpenTraceW() override;
        
    private:
        EtwFileConsumer(const EtwFileConsumer&) = delete;
        EtwFileConsumer& operator=(const EtwFileConsumer&) = delete;

        ETWPROCESSOR_API void Initialize(const wchar_t* pszFileName);
#if defined(ETWPROCESSOR_FULL_BUILD)
        ETWPROCESSOR_API void Initialize(const __wchar_t* pszFileName);
#endif

        struct EtwFileImpl;
        EtwFileImpl* m_pImpl = nullptr;
    };
}/* namespace Processor */}/* namespace Etw */

#pragma warning(pop)
