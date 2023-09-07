//----------------------------------------------------------------------------------------------------------------------
/// \file
/// <summary>Public logging interface for C++ tests.</summary>
// Copyright (c) Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Wex.Logger.h"

#include "LogTestResults.h"
#include "WexLogTrace.h"
#include "WexTypes.h"

// Allow anyone who has defined an Assert, Log, LogTrace, or Trace macro to compile with this header file included
#pragma push_macro("Assert")
#undef Assert
#pragma push_macro("Log")
#undef Log
#pragma push_macro("LogTrace")
#undef LogTrace
#pragma push_macro("Trace")
#undef Trace

namespace WEX { namespace Logging { namespace Private
{
    extern "C" WEXLOGGER_API void WEXLOGGER_STDCALL Log_Trace(LogTrace&& trace) noexcept;
    extern "C" WEXLOGGER_API void WEXLOGGER_STDCALL Log_MiniDump() noexcept;
}/* namespace Private */}/* namespace Logging */}/* namespace WEX */

namespace WEX { namespace Logging { namespace Log
{
    /// <summary>
    /// Log a trace
    /// </summary>
    /// <param name="trace"> </param>
    inline void Trace(LogTrace&& trace) noexcept
    {
        Private::Log_Trace(static_cast<LogTrace&&>(trace));
    }

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    inline void Assert(const wchar_t* pszAssert) noexcept
    {
        Trace(AssertTrace{pszAssert});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    inline void Assert(const char* pszAssert) noexcept
    {
        Trace(AssertTrace{pszAssert});
    }
#if defined(__cpp_char8_t)
    inline void Assert(const char8_t* pszAssert) noexcept
    {
        Trace(AssertTrace{pszAssert});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszContext"> </param>
    inline void Assert(const wchar_t* pszAssert, const wchar_t* pszContext) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszContext"> </param>
    inline void Assert(const char* pszAssert, const char* pszContext) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Assert(const char8_t* pszAssert, const char8_t* pszContext) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Assert(const wchar_t* pszAssert, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Assert(const char* pszAssert, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Assert(const char8_t* pszAssert, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Assert(const wchar_t* pszAssert, const wchar_t* pszContext, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test assert
    /// </summary>
    /// <param name="pszAssert"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Assert(const char* pszAssert, const char* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Assert(const char8_t* pszAssert, const char8_t* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(AssertTrace{pszAssert}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a known bug number
    /// </summary>
    /// <param name="pszBugDatabase"> </param>
    /// <param name="bugId"> </param>
    inline void Bug(const wchar_t* pszBugDatabase, int bugId) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a known bug number
    /// </summary>
    /// <param name="pszBugDatabase"> </param>
    /// <param name="bugId"> </param>
    inline void Bug(const char* pszBugDatabase, int bugId) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId});
    }
#if defined(__cpp_char8_t)
    inline void Bug(const char8_t* pszBugDatabase, int bugId) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a known bug number
    /// </summary>
    /// <param name="pszBugDatabase"> </param>
    /// <param name="bugId"> </param>
    /// <param name="pszContext"> </param>
    inline void Bug(const wchar_t* pszBugDatabase, int bugId, const wchar_t* pszContext) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a known bug number
    /// </summary>
    /// <param name="pszBugDatabase"> </param>
    /// <param name="bugId"> </param>
    /// <param name="pszContext"> </param>
    inline void Bug(const char* pszBugDatabase, int bugId, const char* pszContext) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Bug(const char8_t* pszBugDatabase, int bugId, const char8_t* pszContext) noexcept
    {
        Trace(BugTrace{pszBugDatabase, bugId}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test comment
    /// </summary>
    /// <param name="pszComment"> </param>
    inline void Comment(const wchar_t* pszComment) noexcept
    {
        Trace(CommentTrace{pszComment});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test comment
    /// </summary>
    /// <param name="pszComment"> </param>
    inline void Comment(const char* pszComment) noexcept
    {
        Trace(CommentTrace{pszComment});
    }
#if defined(__cpp_char8_t)
    inline void Comment(const char8_t* pszComment) noexcept
    {
        Trace(CommentTrace{pszComment});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test comment
    /// </summary>
    /// <param name="pszComment"> </param>
    /// <param name="pszContext"> </param>
    inline void Comment(const wchar_t* pszComment, const wchar_t* pszContext) noexcept
    {
        Trace(CommentTrace{pszComment}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test comment
    /// </summary>
    /// <param name="pszComment"> </param>
    /// <param name="pszContext"> </param>
    inline void Comment(const char* pszComment, const char* pszContext) noexcept
    {
        Trace(CommentTrace{pszComment}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Comment(const char8_t* pszComment, const char8_t* pszContext) noexcept
    {
        Trace(CommentTrace{pszComment}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the end of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    inline void EndGroup(const wchar_t* pszGroupName) noexcept
    {
        Trace(EndGroupTrace{pszGroupName});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the end of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    inline void EndGroup(const char* pszGroupName) noexcept
    {
        Trace(EndGroupTrace{pszGroupName});
    }
#if defined(__cpp_char8_t)
    inline void EndGroup(const char8_t* pszGroupName) noexcept
    {
        Trace(EndGroupTrace{pszGroupName});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the end of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    inline void EndGroup(const wchar_t* pszGroupName, const wchar_t* pszContext) noexcept
    {
        Trace(EndGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the end of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    inline void EndGroup(const char* pszGroupName, const char* pszContext) noexcept
    {
        Trace(EndGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void EndGroup(const char8_t* pszGroupName, const char8_t* pszContext) noexcept
    {
        Trace(EndGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    inline void Error(const wchar_t* pszError) noexcept
    {
        Trace(ErrorTrace{pszError});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    inline void Error(const char* pszError) noexcept
    {
        Trace(ErrorTrace{pszError});
    }
#if defined(__cpp_char8_t)
    inline void Error(const char8_t* pszError) noexcept
    {
        Trace(ErrorTrace{pszError});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszContext"> </param>
    inline void Error(const wchar_t* pszError, const wchar_t* pszContext) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszContext"> </param>
    inline void Error(const char* pszError, const char* pszContext) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Error(const char8_t* pszError, const char8_t* pszContext) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Error(const wchar_t* pszError, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Error(const char* pszError, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Error(const char8_t* pszError, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Error(const wchar_t* pszError, const wchar_t* pszContext, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test error
    /// </summary>
    /// <param name="pszError"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Error(const char* pszError, const char* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Error(const char8_t* pszError, const char8_t* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(ErrorTrace{pszError}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file to be saved to disk
    /// </summary>
    /// <param name="pszFilePath"> </param>
    inline void File(const wchar_t* pszFilePath) noexcept
    {
        Trace(FileTrace{pszFilePath});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file to be saved to disk
    /// </summary>
    /// <param name="pszFilePath"> </param>
    inline void File(const char* pszFilePath) noexcept
    {
        Trace(FileTrace{pszFilePath});
    }
#if defined(__cpp_char8_t)
    inline void File(const char8_t* pszFilePath) noexcept
    {
        Trace(FileTrace{pszFilePath});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file to be saved to disk
    /// </summary>
    /// <param name="pszFilePath"> </param>
    /// <param name="pszContext"> </param>
    inline void File(const wchar_t* pszFilePath, const wchar_t* pszContext) noexcept
    {
        Trace(FileTrace{pszFilePath}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file to be saved to disk
    /// </summary>
    /// <param name="pszFilePath"> </param>
    /// <param name="pszContext"> </param>
    inline void File(const char* pszFilePath, const char* pszContext) noexcept
    {
        Trace(FileTrace{pszFilePath}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void File(const char8_t* pszFilePath, const char8_t* pszContext) noexcept
    {
        Trace(FileTrace{pszFilePath}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file (in byte array form) to be saved to disk
    /// </summary>
    /// <param name="pszFileName"> </param>
    /// <param name="pFileBuffer"> </param>
    /// <param name="bufferSize"> </param>
    inline void File(const wchar_t* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file (in byte array form) to be saved to disk
    /// </summary>
    /// <param name="pszFileName"> </param>
    /// <param name="pFileBuffer"> </param>
    /// <param name="bufferSize"> </param>
    inline void File(const char* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize});
    }
#if defined(__cpp_char8_t)
    inline void File(const char8_t* pszFileName, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file (in byte array form) to be saved
    /// </summary>
    /// <param name="pszFileName"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pFileBuffer"> </param>
    /// <param name="bufferSize"> </param>
    inline void File(const wchar_t* pszFileName, const wchar_t* pszContext, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test file (in byte array form) to be saved
    /// </summary>
    /// <param name="pszFileName"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pFileBuffer"> </param>
    /// <param name="bufferSize"> </param>
    inline void File(const char* pszFileName, const char* pszContext, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void File(const char8_t* pszFileName, const char8_t* pszContext, const unsigned char* pFileBuffer, size_t bufferSize) noexcept
    {
        Trace(FileTrace{pszFileName, pFileBuffer, bufferSize}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a name/value property pair the value can be in xml format
    /// </summary>
    /// <param name="pszName"> </param>
    /// <param name="pszValue"> </param>
    inline void Property(const wchar_t* pszName, const wchar_t* pszValue) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a name/value property pair the value can be in xml format
    /// </summary>
    /// <param name="pszName"> </param>
    /// <param name="pszValue"> </param>
    inline void Property(const char* pszName, const char* pszValue) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue});
    }
#if defined(__cpp_char8_t)
    inline void Property(const char8_t* pszName, const char8_t* pszValue) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a name/value property pair; the value can be in xml format
    /// </summary>
    /// <param name="pszName"> </param>
    /// <param name="pszValue"> </param>
    /// <param name="pszContext"> </param>
    inline void Property(const wchar_t* pszName, const wchar_t* pszValue, const wchar_t* pszContext) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a name/value property pair; the value can be in xml format
    /// </summary>
    /// <param name="pszName"> </param>
    /// <param name="pszValue"> </param>
    /// <param name="pszContext"> </param>
    inline void Property(const char* pszName, const char* pszValue, const char* pszContext) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Property(const char8_t* pszName, const char8_t* pszValue, const char8_t* pszContext) noexcept
    {
        Trace(PropertyTrace{pszName, pszValue}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

    /// <summary>
    /// Log a test result
    /// </summary>
    /// <param name="testResult"> </param>
    inline void Result(TestResults::Result testResult) noexcept
    {
        Trace(ResultTrace{testResult});
    }

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test result
    /// </summary>
    /// <param name="testResult"> </param>
    /// <param name="pszComment"> </param>
    inline void Result(TestResults::Result testResult, const wchar_t* pszComment) noexcept
    {
        Trace(ResultTrace{testResult, pszComment});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test result
    /// </summary>
    /// <param name="testResult"> </param>
    /// <param name="pszComment"> </param>
    inline void Result(TestResults::Result testResult, const char* pszComment) noexcept
    {
        Trace(ResultTrace{testResult, pszComment});
    }
#if defined(__cpp_char8_t)
    inline void Result(TestResults::Result testResult, const char8_t* pszComment) noexcept
    {
        Trace(ResultTrace{testResult, pszComment});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test result
    /// </summary>
    /// <param name="testResult"> </param>
    /// <param name="pszComment"> </param>
    /// <param name="pszContext"> </param>
    inline void Result(TestResults::Result testResult, const wchar_t* pszComment, const wchar_t* pszContext) noexcept
    {
        Trace(ResultTrace{testResult, pszComment}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test result
    /// </summary>
    /// <param name="testResult"> </param>
    /// <param name="pszComment"> </param>
    /// <param name="pszContext"> </param>
    inline void Result(TestResults::Result testResult, const char* pszComment, const char* pszContext) noexcept
    {
        Trace(ResultTrace{testResult, pszComment}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Result(TestResults::Result testResult, const char8_t* pszComment, const char8_t* pszContext) noexcept
    {
        Trace(ResultTrace{testResult, pszComment}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    inline void StartGroup(const wchar_t* pszGroupName) noexcept
    {
        Trace(StartGroupTrace{pszGroupName});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    inline void StartGroup(const char* pszGroupName) noexcept
    {
        Trace(StartGroupTrace{pszGroupName});
    }
#if defined(__cpp_char8_t)
    inline void StartGroup(const char8_t* pszGroupName) noexcept
    {
        Trace(StartGroupTrace{pszGroupName});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test; also sets the default test result
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="defaultTestResult"> </param>
    inline void StartGroup(const wchar_t* pszGroupName, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test; also sets the default test result
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="defaultTestResult"> </param>
    inline void StartGroup(const char* pszGroupName, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult});
    }
#if defined(__cpp_char8_t)
    inline void StartGroup(const char8_t* pszGroupName, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    inline void StartGroup(const wchar_t* pszGroupName, const wchar_t* pszContext) noexcept
    {
        Trace(StartGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    inline void StartGroup(const char* pszGroupName, const char* pszContext) noexcept
    {
        Trace(StartGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void StartGroup(const char8_t* pszGroupName, const char8_t* pszContext) noexcept
    {
        Trace(StartGroupTrace{pszGroupName}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test; also sets the default test result
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="defaultTestResult"> </param>
    inline void StartGroup(const wchar_t* pszGroupName, const wchar_t* pszContext, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log the start of a group of tests, or of a specific test; also sets the default test result
    /// </summary>
    /// <param name="pszGroupName"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="defaultTestResult"> </param>
    inline void StartGroup(const char* pszGroupName, const char* pszContext, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void StartGroup(const char8_t* pszGroupName, const char8_t* pszContext, TestResults::Result defaultTestResult) noexcept
    {
        Trace(StartGroupTrace{pszGroupName, defaultTestResult}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    inline void Warning(const wchar_t* pszWarning) noexcept
    {
        Trace(WarningTrace{pszWarning});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    inline void Warning(const char* pszWarning) noexcept
    {
        Trace(WarningTrace{pszWarning});
    }
#if defined(__cpp_char8_t)
    inline void Warning(const char8_t* pszWarning) noexcept
    {
        Trace(WarningTrace{pszWarning});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszContext"> </param>
    inline void Warning(const wchar_t* pszWarning, const wchar_t* pszContext) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszContext"> </param>
    inline void Warning(const char* pszWarning, const char* pszContext) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Warning(const char8_t* pszWarning, const char8_t* pszContext) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Warning(const wchar_t* pszWarning, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Warning(const char* pszWarning, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Warning(const char8_t* pszWarning, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Warning(const wchar_t* pszWarning, const wchar_t* pszContext, const wchar_t* pszFile, const wchar_t* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log a test warning
    /// </summary>
    /// <param name="pszWarning"> </param>
    /// <param name="pszContext"> </param>
    /// <param name="pszFile"> </param>
    /// <param name="pszFunction"> </param>
    /// <param name="line"> </param>
    inline void Warning(const char* pszWarning, const char* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#if defined(__cpp_char8_t)
    inline void Warning(const char8_t* pszWarning, const char8_t* pszContext, const char* pszFile, const char* pszFunction, int line) noexcept
    {
        Trace(WarningTrace{pszWarning}.WithContext(pszContext).WithSourceInfo(pszFile, pszFunction, line));
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log xml data; no check is made to verify that it is well-formed
    /// </summary>
    /// <param name="pszXml"> </param>
    inline void Xml(const wchar_t* pszXml) noexcept
    {
        Trace(XmlTrace{pszXml});
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log xml data; no check is made to verify that it is well-formed
    /// </summary>
    /// <param name="pszXml"> </param>
    inline void Xml(const char* pszXml) noexcept
    {
        Trace(XmlTrace{pszXml});
    }
#if defined(__cpp_char8_t)
    inline void Xml(const char8_t* pszXml) noexcept
    {
        Trace(XmlTrace{pszXml});
    }
#endif // #if defined(__cpp_char8_t)

#if !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log xml data; no check is made to verify that it is well-formed
    /// </summary>
    /// <param name="pszXml"> </param>
    /// <param name="pszContext"> </param>
    inline void Xml(const wchar_t* pszXml, const wchar_t* pszContext) noexcept
    {
        Trace(XmlTrace{pszXml}.WithContext(pszContext));
    }
#endif // !defined(WEX_LOGGER_NO_WCHAR_API)
    /// <summary>
    /// Log xml data; no check is made to verify that it is well-formed
    /// </summary>
    /// <param name="pszXml"> </param>
    /// <param name="pszContext"> </param>
    inline void Xml(const char* pszXml, const char* pszContext) noexcept
    {
        Trace(XmlTrace{pszXml}.WithContext(pszContext));
    }
#if defined(__cpp_char8_t)
    inline void Xml(const char8_t* pszXml, const char8_t* pszContext) noexcept
    {
        Trace(XmlTrace{pszXml}.WithContext(pszContext));
    }
#endif // #if defined(__cpp_char8_t)

#if defined(_WIN32)
    /// <summary>
    /// Log the current process mini dump
    /// </summary>
    inline void MiniDump() noexcept
    {
        Private::Log_MiniDump();
    }
#endif // _WIN32
}/* namespace Log */}/* namespace Logging */}/* namespace WEX */

#pragma pop_macro("Trace")
#pragma pop_macro("LogTrace")
#pragma pop_macro("Log")
#pragma pop_macro("Assert")
