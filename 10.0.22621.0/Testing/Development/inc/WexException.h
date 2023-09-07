//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Base exception class; all exceptions thrown should either be this class or derive from it.</summary>
/// <remarks>You cannot throw this Exception directly - instead, you must use one of the Throw class methods.</remarks>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#if !defined(__WEXEXCEPTION_H__) && defined(_WIN32)
#define __WEXEXCEPTION_H__
#include "Wex.Common.h"

#include "Throw.h"
#include "WexString.h"

#include <specstrings.h>

namespace WEX { namespace Common
{
    const wchar_t c_szExceptionMessageUnavailable[] = L"Error retrieving exception message; possibly out of memory.";

    class WEXCOMMON_API Exception
    {
    // Only allow Throw to construct Exception objects
    friend void Throw::Private::ThrowImpl(HRESULT, const wchar_t*);

    public:
        Exception(const Exception& other); // Copy constructor
        virtual ~Exception(){}

        _Ret_z_ const unsigned short* Message() const // Returns the message (if any) that was passed into the Exception constructor 
        {
            if (m_message.IsValid())
            {
                return reinterpret_cast<const unsigned short*>(static_cast<const wchar_t*>(m_message));
            }
            else
            {
                return reinterpret_cast<const unsigned short*>(c_szExceptionMessageUnavailable);
            }
        }

        #pragma warning(suppress:28196) // The static analyzer is not taking the _Field_range_ annotation under consideration when enforcing _Ret_range_.
        // Returns the HRESULT that was passed into the Exception constructor.
        _Ret_range_(<,0) HRESULT ErrorCode() const
        {
            return m_hresult;
        }

    protected:
        // Don't allow "throw Exception()" -- use Throw::<method>() instead
        explicit Exception(_In_range_(<, 0) HRESULT errorCode)
            : m_hresult(errorCode)
        {}

        // Don't allow "throw Exception()" -- use Throw::<method>() instead
        Exception(_In_range_(<, 0) HRESULT errorCode, _In_opt_z_ const wchar_t* pszMessage)
            : m_hresult(errorCode)
            , m_message(pszMessage)
        {}

        // Don't allow "throw Exception()" -- use Throw::<method>() instead
        Exception(_In_range_(<, 0) HRESULT errorCode, const WEX::Common::String& message)
            : m_hresult(errorCode)
            , m_message(message)
        {}

        // wchar_t native type export
        #if defined(WEXCOMMON_FULL_BUILD)
        Exception(_In_range_(<, 0) HRESULT errorCode, _In_opt_z_ const __wchar_t* pszMessage)
            : m_hresult(errorCode)
            , m_message(reinterpret_cast<const wchar_t*>(pszMessage))
        {}
        #endif

    private:
        Exception& operator=(const Exception&) = delete;

        // Making 'operator new' private prevents heap allocation of Exception, and forces Exception instances
        // to be thrown by value.
        static void* operator new(size_t) = delete;

        // Making 'operator delete' private for consistency with 'operator new'.
        // This is implemented to prevent errors due to references from compiler-generated functions.
        static void operator delete(void*) {}

        _Field_range_(<,0) const HRESULT m_hresult;
        WEX::Common::NoThrowString m_message;
    };
}/* namespace Common */}/* namespace WEX */
#endif // #if !defined(__WEXEXCEPTION_H__) && defined(_WIN32)
