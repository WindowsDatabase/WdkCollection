//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary> API and data types for using data in data driven native tests </summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "TE.Common.h"

#include "WexDebug.h"

#if !defined(_WIN32)
#pragma push_macro("_In_z_")
#pragma push_macro("_Outptr_")
#if !defined(_In_z_)
#define _In_z_
#endif
#if !defined(_Outptr_)
#define _Outptr_
#endif
#endif // #if !defined(_WIN32)

#if defined(_WIN32)
// Forward define 'IStream'
typedef interface IStream IStream;
#endif // #if defined(_WIN32)

namespace WEX { namespace Common
{
    class NoThrowString;
    class String;
}/* namespace Common */}/* namespace WEX */

namespace WEX { namespace TestExecution
{
    class TestDataArrayPrivate;

    /// <summary>
    ///  A helper class to work with arrays in data driven testing
    /// </summary>
    /// \code
    /// Example:
    ///
    ///  TestDataArray<int> sizes;
    ///  if (SUCCEEDED(TestData::TryGetValue(L"size", sizes)))
    ///  {
    ///      size_t count = sizes.GetSize();
    ///      for (size_t i = 0; i < count; ++i)
    ///      {
    ///          Log::Comment(String().Format(L"Size[%d] retrieved was %d", i, sizes[i]));
    ///      }
    ///  }
    /// \endcode
    template <typename T>
    class TECOMMON_API TestDataArray final
    {
        friend class TestDataArrayPrivate;

    public:
        TestDataArray();
        ~TestDataArray();
        T& operator[](size_t index);
#if defined(__clang__)
#pragma clang diagnostic push
        // We have to return a const size_t in order to preserve the decorated name of the export.
        // This will be fixed once we refactor this code to avoid decorated C++ exports.
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#endif
        const size_t GetSize() const;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        const T& operator[](size_t index) const;

    private:
        void Allocate(size_t elements);
        TestDataArray(const TestDataArray<T>& other);
        TestDataArray<T>& operator=(TestDataArray<T>& other);
        void Free();
        T* Detach();

        /// \internal
        /// we need a way to get an address because operator& is overloaded
        TestDataArray<T>* This() {return this;}

        size_t m_size;
        T* m_p;
    };

    namespace Private
    {
        /// \internal
        /// WEX::TestExecution::Private::TestData must be a templatized *class* so that we can dll export explicit 
        /// instantiations.
        template <typename T>
        class TECOMMON_API TestData final
        {
        public:
            /// Tries to get the requested value in the requested format
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, T& result);
            /// TestDataArray specific TestData implementation
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, TestDataArray<T>& results);

            #ifdef TECOMMON_EXPORTS
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, T& result);
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, TestDataArray<T>& results);
            #endif
        };

#if defined(_WIN32)
        class TECOMMON_API XmlTestData final
        {
        public:
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, _Outptr_ IStream** result);

            #ifdef TECOMMON_EXPORTS
            static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, _Outptr_ IStream** result);
            #endif
        };
#endif // #if defined(_WIN32)

        template <typename T1, typename T2>
        class AreSameType
        {
        public:
            static const bool value = false;
        };

        template <typename T1>
        class AreSameType<T1, T1>
        {
        public:
            static const bool value = true;
        };
    }

    /// <summary>
    ///  Data driven test data retrival class
    /// </summary>
    /// \internal
    /// WEX::TestExecution::TestData provides templatized *methods* to allow compiler type inference, and simply routes 
    /// the call to the matching WEX::TestExecution::Private::TestData<T> instantiation.
    class TestData
    {
    public:

        /// <summary>
        ///  Data driven test data retrival API for basic types
        /// </summary>
        template <typename T>
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, T& result)
        {
            static_assert((Private::AreSameType<int, T>::value || Private::AreSameType<size_t, T>::value || Private::AreSameType<bool, T>::value ||
                Private::AreSameType<float, T>::value || Private::AreSameType<double, T>::value || Private::AreSameType<unsigned long, T>::value ||
#if defined(_WIN32)
                Private::AreSameType<__int64, T>::value || Private::AreSameType<unsigned __int64, T>::value || 
#endif // #if defined(_WIN32)
                Private::AreSameType<unsigned int, T>::value || Private::AreSameType<WEX::Common::String, T>::value ||
                Private::AreSameType<WEX::Common::NoThrowString, T>::value || Private::AreSameType<long, T>::value),
                "Attempt to use TestData::TryGetValue with an unsupported type");
            return Private::TestData<T>::TryGetValue(pszString, result);
        }

#if defined(_WIN32)
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, _Outptr_ IStream** result)
        {
            return Private::XmlTestData::TryGetValue(pszString, result);
        }
#endif // #if defined(_WIN32)

        /// <summary>
        ///  Data driven test data retrival API for array types
        /// </summary>
        template<typename T>
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const wchar_t* pszString, TestDataArray<T>& results)
        {
            static_assert((Private::AreSameType<int, T>::value || Private::AreSameType<size_t, T>::value || Private::AreSameType<bool, T>::value ||
                Private::AreSameType<float, T>::value || Private::AreSameType<double, T>::value || Private::AreSameType<unsigned long, T>::value ||
#if defined(_WIN32)
                Private::AreSameType<__int64, T>::value || Private::AreSameType<unsigned __int64, T>::value ||
#endif // #if defined(_WIN32)
                Private::AreSameType<unsigned int, T>::value || Private::AreSameType<WEX::Common::String, T>::value ||
                Private::AreSameType<WEX::Common::NoThrowString, T>::value || Private::AreSameType<long, T>::value),
                "Attempt to use TestData::TryGetValue with an unsupported type for TestDataArray");
            return Private::TestData<T>::TryGetValue(pszString, results);
        }

        #ifdef TECOMMON_EXPORTS
        template <typename T>
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, T& result)
        {
            static_assert((Private::AreSameType<int, T>::value || Private::AreSameType<size_t, T>::value || Private::AreSameType<bool, T>::value ||
                Private::AreSameType<float, T>::value || Private::AreSameType<double, T>::value || Private::AreSameType<unsigned long, T>::value ||
#if defined(_WIN32)
                Private::AreSameType<__int64, T>::value || Private::AreSameType<unsigned __int64, T>::value ||
#endif // #if defined(_WIN32)
                Private::AreSameType<unsigned int, T>::value || Private::AreSameType<WEX::Common::String, T>::value ||
                Private::AreSameType<WEX::Common::NoThrowString, T>::value || Private::AreSameType<long, T>::value),
                "Attempt to use TestData::TryGetValue with an unsupported type");
            return Private::TestData<T>::TryGetValue(pszString, result);
        }

#if defined(_WIN32)
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, _Outptr_ IStream** result)
        {
            return Private::XmlTestData::TryGetValue(pszString, result);
        }
#endif // #if defined(_WIN32)

        /// <summary>
        ///  Data driven test data retrival API for array types
        /// </summary>
        template<typename T>
        static HRESULT TAEF_STDCALL TryGetValue(_In_z_ const __wchar_t* pszString, TestDataArray<T>& results)
        {
            static_assert((Private::AreSameType<int, T>::value || Private::AreSameType<size_t, T>::value || Private::AreSameType<bool, T>::value ||
                Private::AreSameType<float, T>::value || Private::AreSameType<double, T>::value || Private::AreSameType<unsigned long, T>::value ||
#if defined(_WIN32)
                Private::AreSameType<__int64, T>::value || Private::AreSameType<unsigned __int64, T>::value ||
#endif // #if defined(_WIN32)
                Private::AreSameType<unsigned int, T>::value || Private::AreSameType<WEX::Common::String, T>::value ||
                Private::AreSameType<WEX::Common::NoThrowString, T>::value || Private::AreSameType<long, T>::value),
                "Attempt to use TestData::TryGetValue with an unsupported type for TestDataArray");
            return Private::TestData<T>::TryGetValue(pszString, results);
        }
        #endif
    };
}/* namespace TestExecution */}/* namespace WEX */

#if !defined(_WIN32)
#pragma pop_macro("_In_z_")
#pragma pop_macro("_Outptr_")
#endif // #if !defined(_WIN32)
