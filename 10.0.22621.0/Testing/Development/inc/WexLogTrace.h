//----------------------------------------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "LogTestResults.h"

// Allow anyone who has defined a LogTrace, or Trace macro to compile with this header file included
#pragma push_macro("LogTrace")
#undef LogTrace
#pragma push_macro("Trace")
#undef Trace

// Just including <stdint.h> and using intptr_t/uintptr_t caused build breaks for some existing users.
// Work around this for now by defining our own pointer-sized ints for MSVC++.
#if defined(_MSC_VER)
# ifdef _WIN64
#  define WEX_INTPTR __int64
#  define WEX_UINTPTR unsigned __int64
# else
#  define WEX_INTPTR int
#  define WEX_UINTPTR unsigned int
# endif
#else
# include <stdint.h>
# define WEX_INTPTR intptr_t
# define WEX_UINTPTR uintptr_t
#endif

#if defined(__linux__)
# define WEX_LOGGER_NO_WCHAR_API
#endif

#pragma warning(push)
// Disable "Prefer enum class over enum".
#pragma warning(disable:26812)
// Disable "copy constructor was implicitly defined as deleted" and "assignment operator was implicitly defined as deleted".
// It is intentional that the derived classes have implicitly deleted copy constructors and assignment operators.
#pragma warning(disable:4625 4626)
// Disable "Local declaration of 'Error' hides previous declaration at line '77' of '...\winrt\asyncinfo.h'."
// This is a false positive.
#pragma warning(disable:6244)

namespace WEX { namespace Logging
{
    namespace LogTraceAttachment
    {
        enum Attachment : WEX_UINTPTR
        {
            None = 0x0,
            ScreenCapture = 0x1,
            MiniDump = 0x2,
        };

        inline Attachment operator|(Attachment p1, Attachment p2)
        {
            return static_cast<Attachment>(static_cast<WEX_UINTPTR>(p1) | static_cast<WEX_UINTPTR>(p2));
        }
        inline Attachment& operator|=(Attachment& p1, Attachment p2)
        {
            return p1 = p1 | p2;
        }
    }/* namespace LogTraceAttachment */

    // When using this, it is recommended that you use it all in one statement. Here is an example:
    //
    //   Log::Trace(ErrorTrace{L"Message"}.WithContext(L"MyTestLib").WithXmlAnnotation(L"<CustomDataForLogFile />"));
    //
    // The strings are not copied by the trace classes; so if you use temporary strings and don't use the trace class right away,
    // you may end up reading freed memory. Using the trace class on the same line as the Log::Trace call avoids the issue.
    //
    // You can also add source info to a trace like this:
    //
    //   Log::Trace(ErrorTrace{L"Message"}.WithSourceInfo(WEX_LOGGER_CURRENT_SOURCE_INFO));
    //
    // WEX_LOGGER_CURRENT_SOURCE_INFO expands to the current file, function, and line number, but you can also pass the parameters manually.
    class LogTrace
    {
        friend class LogTraceFriend;
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        LogTrace&& WithContext(const wchar_t* pszContext) && noexcept
        {
            m_pszContext = pszContext;
            return static_cast<LogTrace&&>(*this);
        }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        LogTrace&& WithContext(const char* pszContext) && noexcept
        {
            m_pszContext = pszContext;
            // Set flags to indicate the context is UTF-8.
            m_traceTypeAndFlags |= 0x00020080;
            return static_cast<LogTrace&&>(*this);
        }
#if defined(__cpp_char8_t)
        LogTrace&& WithContext(const char8_t* pszContext) && noexcept
        {
            m_pszContext = pszContext;
            // Set flags to indicate the context is UTF-8.
            m_traceTypeAndFlags |= 0x00020080;
            return static_cast<LogTrace&&>(*this);
        }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
# define WEX_LOGGER_CURRENT_SOURCE_INFO __FILEW__, __FUNCTIONW__, __LINE__
        LogTrace&& WithSourceInfo(const wchar_t* pszFile, const wchar_t* pszFunction, int line) && noexcept
        {
            m_pszFile = pszFile;
            m_pszFunction = pszFunction;
            m_line = static_cast<WEX_INTPTR>(line);
            return static_cast<LogTrace&&>(*this);
        }
#else
# define WEX_LOGGER_CURRENT_SOURCE_INFO __FILE__, __FUNCTION__, __LINE__
#endif
        LogTrace&& WithSourceInfo(const char* pszFile, const char* pszFunction, int line) && noexcept
        {
            m_pszFile = pszFile;
            m_pszFunction = pszFunction;
            m_line = static_cast<WEX_INTPTR>(line);
            // Set flags to indicate m_pszFile and m_pszFunction are UTF-8.
            m_traceTypeAndFlags |= 0x00040080;
            return static_cast<LogTrace&&>(*this);
        }

#if !defined(WEX_LOGGER_NO_WCHAR_API)
        // The XML annotation is added to the WTT log file for this trace. It can contain any contents, but it should be valid
        // XML or the log file will be corrupted.
        LogTrace&& WithXmlAnnotation(const wchar_t* pszXmlAnnotation) && noexcept
        {
            m_pszXmlAnnotation = pszXmlAnnotation;
            return static_cast<LogTrace&&>(*this);
        }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        // The XML annotation is added to the WTT log file for this trace. It can contain any contents, but it should be valid
        // XML or the log file will be corrupted.
        LogTrace&& WithXmlAnnotation(const char* pszXmlAnnotation) && noexcept
        {
            m_pszXmlAnnotation = pszXmlAnnotation;
            // Set flags to indicate the XML annotation is UTF-8.
            m_traceTypeAndFlags |= 0x00080080;
            return static_cast<LogTrace&&>(*this);
        }
#if defined(__cpp_char8_t)
        LogTrace&& WithXmlAnnotation(const char8_t* pszXmlAnnotation) && noexcept
        {
            m_pszXmlAnnotation = pszXmlAnnotation;
            // Set flags to indicate the XML annotation is UTF-8.
            m_traceTypeAndFlags |= 0x00080080;
            return static_cast<LogTrace&&>(*this);
        }
#endif // #if defined(__cpp_char8_t)

        // You can attach a screenshot and/or minidump to any trace using this method.
        LogTrace&& WithAttachment(::WEX::Logging::LogTraceAttachment::Attachment attachment) && noexcept
        {
            m_traceTypeAndFlags |= static_cast<WEX_UINTPTR>(attachment) << 8;
            return static_cast<LogTrace&&>(*this);
        }

        // Prevent copying of trace classes.
        LogTrace(const LogTrace&) = delete;
        LogTrace& operator=(const LogTrace&) = delete;
        // Prevent allocation of trace classes.
        static void* operator new(size_t) = delete;
        static void operator delete(void*) = delete;

    protected:
        enum TraceType : WEX_UINTPTR
        {
            Assert,
            Error,
            Warning,
            Comment,
            StartGroup,
            EndGroup,
            KnownBug,
            FileName,
            FileBuffer,
            Property,
            Xml,
            Result,
            LightweightStartGroup,
            LightweightEndGroup,

            TraceTypeCount
        };

#if !defined(WEX_LOGGER_NO_WCHAR_API)
        LogTrace(TraceType traceType, const wchar_t* pszMessage, WEX_UINTPTR traceTypeSpecificIntegralValue = 0,
                const void* pTraceTypeSpecificPointerValue = nullptr) noexcept
            : m_traceTypeAndFlags{static_cast<WEX_UINTPTR>(traceType)}
            , m_pszMessage{pszMessage}
            , m_traceTypeSpecificIntegralValue{traceTypeSpecificIntegralValue}
            , m_pTraceTypeSpecificPointerValue{pTraceTypeSpecificPointerValue}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)

        LogTrace(TraceType traceType, const char* pszMessage, WEX_UINTPTR traceTypeSpecificIntegralValue = 0,
                const void* pTraceTypeSpecificPointerValue = nullptr) noexcept
            // Set flags to indicate the message is UTF-8.
            : m_traceTypeAndFlags{static_cast<WEX_UINTPTR>(traceType) | 0x00010080}
            , m_pszMessage{pszMessage}
            , m_traceTypeSpecificIntegralValue{traceTypeSpecificIntegralValue}
            , m_pTraceTypeSpecificPointerValue{pTraceTypeSpecificPointerValue}
        {}
#if defined(__cpp_char8_t)
        LogTrace(TraceType traceType, const char8_t* pszMessage, WEX_UINTPTR traceTypeSpecificIntegralValue = 0,
            const void* pTraceTypeSpecificPointerValue = nullptr) noexcept
            // Set flags to indicate the message is UTF-8.
            : m_traceTypeAndFlags{static_cast<WEX_UINTPTR>(traceType) | 0x00010080}
            , m_pszMessage{pszMessage}
            , m_traceTypeSpecificIntegralValue{traceTypeSpecificIntegralValue}
            , m_pTraceTypeSpecificPointerValue{pTraceTypeSpecificPointerValue}
        {}
#endif // #if defined(__cpp_char8_t)

    private:
        // All members are the same size to avoid having inconsistent padding across DLLs.

        // m_traceTypeAndFlags:
        // Bits 0-6: The TraceType enum value.
        // Bit 7: This is set if any of bits 10 through 31 are set.
        //  (Bit 7 causes old Wex.Logger versions to fail even though they don't verify that reserved bits are zero.)
        // Bit 8: Attach a screen capture to the log trace.
        // Bit 9: Attach a minidump to the log trace.
        // Bits 10-15: Reserved. Must be zero.
        // Bit 16: Set: m_pszMessage is UTF-8. Clear: m_pszMessage is UTF-16.
        //  Bit 16 also applies to m_pTraceTypeSpecificPointerValue if that is used for a string.
        // Bit 17: Set: m_pszContext is UTF-8. Clear: m_pszContext is UTF-16.
        // Bit 18: Set: m_pszFile and m_pszFunction are UTF-8. Clear: m_pszFile and m_pszFunction are UTF-16.
        // Bit 19: Set: m_pszXmlAnnotation is UTF-8. Clear: m_pszXmlAnnotation is UTF-16.
        // Bits 20-31: Reserved. Must be zero.
        // Bits 32-63: Ignored. (Unavailable on 32-bit builds.)
        WEX_UINTPTR m_traceTypeAndFlags;
        const void* const m_pszMessage;
        const WEX_UINTPTR m_traceTypeSpecificIntegralValue;
        const void* const m_pTraceTypeSpecificPointerValue;

        const void* m_pszContext = nullptr;
        const void* m_pszFile = nullptr;
        const void* m_pszFunction = nullptr;
        WEX_INTPTR m_line = -1;
        const void* m_pszXmlAnnotation = nullptr;
    };
    
    class AssertTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit AssertTrace(const wchar_t* pszAssert) noexcept
            : LogTrace{Assert, pszAssert}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit AssertTrace(const char* pszAssert) noexcept
            : LogTrace{Assert, pszAssert}
        {}
#if defined(__cpp_char8_t)
        explicit AssertTrace(const char8_t* pszAssert) noexcept
            : LogTrace{Assert, pszAssert}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class BugTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        BugTrace(const wchar_t* pszBugDatabase, int bugId) noexcept
            : LogTrace{KnownBug, pszBugDatabase, static_cast<WEX_UINTPTR>(bugId)}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        BugTrace(const char* pszBugDatabase, int bugId) noexcept
            : LogTrace{KnownBug, pszBugDatabase, static_cast<WEX_UINTPTR>(bugId)}
        {}
#if defined(__cpp_char8_t)
        BugTrace(const char8_t* pszBugDatabase, int bugId) noexcept
            : LogTrace{KnownBug, pszBugDatabase, static_cast<WEX_UINTPTR>(bugId)}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class CommentTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit CommentTrace(const wchar_t* pszComment) noexcept
            : LogTrace{Comment, pszComment}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit CommentTrace(const char* pszComment) noexcept
            : LogTrace{Comment, pszComment}
        {}
#if defined(__cpp_char8_t)
        explicit CommentTrace(const char8_t* pszComment) noexcept
            : LogTrace{Comment, pszComment}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class EndGroupTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit EndGroupTrace(const wchar_t* pszGroupName) noexcept
            : LogTrace{EndGroup, pszGroupName}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit EndGroupTrace(const char* pszGroupName) noexcept
            : LogTrace{EndGroup, pszGroupName}
        {}
#if defined(__cpp_char8_t)
        explicit EndGroupTrace(const char8_t* pszGroupName) noexcept
            : LogTrace{EndGroup, pszGroupName}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class ErrorTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit ErrorTrace(const wchar_t* pszError) noexcept
            : LogTrace{Error, pszError}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit ErrorTrace(const char* pszError) noexcept
            : LogTrace{Error, pszError}
        {}
#if defined(__cpp_char8_t)
        explicit ErrorTrace(const char8_t* pszError) noexcept
            : LogTrace{Error, pszError}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class FileTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit FileTrace(const wchar_t* pszFilePath) noexcept
            : LogTrace{FileName, pszFilePath}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit FileTrace(const char* pszFilePath) noexcept
            : LogTrace{FileName, pszFilePath}
        {}
#if defined(__cpp_char8_t)
        explicit FileTrace(const char8_t* pszFilePath) noexcept
            : LogTrace{FileName, pszFilePath}
        {}
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
        FileTrace(const wchar_t* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
            : LogTrace{FileBuffer, pszFileName, static_cast<WEX_UINTPTR>(bufferSize), pFileBuffer}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        FileTrace(const char* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
            : LogTrace{FileBuffer, pszFileName, static_cast<WEX_UINTPTR>(bufferSize), pFileBuffer}
        {}
#if defined(__cpp_char8_t)
        FileTrace(const char8_t* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
            : LogTrace{FileBuffer, pszFileName, static_cast<WEX_UINTPTR>(bufferSize), pFileBuffer}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class PropertyTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        PropertyTrace(const wchar_t* pszName, const wchar_t* pszValue) noexcept
            : LogTrace{Property, pszName, 0, pszValue}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        PropertyTrace(const char* pszName, const char* pszValue) noexcept
            : LogTrace{Property, pszName, 0, pszValue}
        {}
#if defined(__cpp_char8_t)
        PropertyTrace(const char8_t* pszName, const char8_t* pszValue) noexcept
            : LogTrace{Property, pszName, 0, pszValue}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class ResultTrace : public LogTrace
    {
    public:
        explicit ResultTrace(TestResults::Result testResult) noexcept
            : LogTrace{Result, static_cast<const char*>(nullptr), static_cast<WEX_UINTPTR>(testResult)}
        {}

#if !defined(WEX_LOGGER_NO_WCHAR_API)
        ResultTrace(TestResults::Result testResult, const wchar_t* pszComment) noexcept
            : LogTrace{Result, pszComment, static_cast<WEX_UINTPTR>(testResult)}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        ResultTrace(TestResults::Result testResult, const char* pszComment) noexcept
            : LogTrace{Result, pszComment, static_cast<WEX_UINTPTR>(testResult)}
        {}
#if defined(__cpp_char8_t)
        ResultTrace(TestResults::Result testResult, const char8_t* pszComment) noexcept
            : LogTrace{Result, pszComment, static_cast<WEX_UINTPTR>(testResult)}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class StartGroupTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit StartGroupTrace(const wchar_t* pszGroupName) noexcept
            : LogTrace{StartGroup, pszGroupName}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit StartGroupTrace(const char* pszGroupName) noexcept
            : LogTrace{StartGroup, pszGroupName}
        {}
#if defined(__cpp_char8_t)
        explicit StartGroupTrace(const char8_t* pszGroupName) noexcept
            : LogTrace{StartGroup, pszGroupName}
        {}
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
        StartGroupTrace(const wchar_t* pszGroupName, TestResults::Result defaultTestResult) noexcept
            : LogTrace{StartGroup, pszGroupName, static_cast<WEX_UINTPTR>(defaultTestResult)}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        StartGroupTrace(const char* pszGroupName, TestResults::Result defaultTestResult) noexcept
            : LogTrace{StartGroup, pszGroupName, static_cast<WEX_UINTPTR>(defaultTestResult)}
        {}
#if defined(__cpp_char8_t)
        StartGroupTrace(const char8_t* pszGroupName, TestResults::Result defaultTestResult) noexcept
            : LogTrace{StartGroup, pszGroupName, static_cast<WEX_UINTPTR>(defaultTestResult)}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class WarningTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit WarningTrace(const wchar_t* pszWarning) noexcept
            : LogTrace{Warning, pszWarning}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit WarningTrace(const char* pszWarning) noexcept
            : LogTrace{Warning, pszWarning}
        {}
#if defined(__cpp_char8_t)
        explicit WarningTrace(const char8_t* pszWarning) noexcept
            : LogTrace{Warning, pszWarning}
        {}
#endif // #if defined(__cpp_char8_t)
    };

    class XmlTrace : public LogTrace
    {
    public:
#if !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit XmlTrace(const wchar_t* pszXml) noexcept
            // XML traces don't use the message field.
            : LogTrace{Xml, static_cast<const wchar_t*>(nullptr), 0, pszXml}
        {}
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
        explicit XmlTrace(const char* pszXml) noexcept
            // XML traces don't use the message field.
            : LogTrace{Xml, static_cast<const char*>(nullptr), 0, pszXml}
        {}
#if defined(__cpp_char8_t)
        explicit XmlTrace(const char8_t* pszXml) noexcept
            // XML traces don't use the message field.
            : LogTrace{Xml, static_cast<const char8_t*>(nullptr), 0, pszXml}
        {}
#endif // #if defined(__cpp_char8_t)
    };
}/* namespace Logging */}/* namespace WEX */

#pragma warning(pop)

#pragma pop_macro("Trace")
#pragma pop_macro("LogTrace")
