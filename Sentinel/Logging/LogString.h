#pragma once
// The intention of this header is to decouple logging from SentinelApp. Many files
// include Sentinel.h just for logging, which creates a large set of cross
// dependencies. In light of this, this header shall not include any unnecessary files.
#include "LoggerTypes.h"
#include <stdafx.h>

// In the future, SentinelApp could call these functions to initialize/deinitialize
// logging and fully decouple logging from SentinelApp.

///	<summary>
/// Initializes the logging subsytem.
///	</summary>
///	<returns><b>true</b> if logging subsystem successfully initialized, else <b>false</b>.</returns>
//bool LogInit();

///	<summary>
/// Deinitializes the logging subsytem.
///	</summary>
//void LogDeinit();

///	<summary>
/// Formats and logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pFormat">Format specification of string to log.</param>
void LogString(LPCTSTR pIdentifier, const wchar_t* pFormat, ...);

///	<summary>
/// Formats and logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pFormat">Format specification of string to log.</param>
void LogString(const std::string &pIdentifier, LPCSTR pFormat, ...)
	__attribute__((format(printf, 2, 3)));

///	<summary>
/// Formats and logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pFormat">Format specification of string to log.</param>
///	<param name="args">Pointer to list of arguments.</param>
void LogStringV(LPCTSTR pIdentifier, const wchar_t* pFormat, va_list args);

///	<summary>
/// Formats and logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pFormat">Format specification of string to log.</param>
///	<param name="args">Pointer to list of arguments.</param>
void LogStringV(const std::string &pIdentifier, LPCSTR pFormat, va_list args);

///	<summary>
/// Logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pLogString">The string to log.</param>
void LogStringNoFormat(LPCTSTR pIdentifier,const std::wstring & pLogString);

///	<summary>
/// Logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pLogString">The string to log.</param>
void LogStringNoFormat(const std::string &pIdentifier, LPCSTR pLogString);

///	<summary>
/// Logs a string
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="pLogString">The string to log.</param>
void LogStringNoFormat(const std::string &pIdentifier, const std::string &pLogString);

///	<summary>
/// Logs data
///	</summary>
///	<param name="pIdentifier">The logging category to log under (if enabled).</param>
///	<param name="code">The code to log.</param>
///	<param name="pData">IN - The data to log.</param>
///	<param name="dataLength">The length of the data to log, in bytes.</param>
void LogData(const std::string &pIdentifier, BYTE code, const void *pData, long dataLength);

/// <summary>
/// Logs the results of S5 API calls, if the result is not S5_OKAY.
/// </summary>
/// <param name="result">IN - The result.</param>
/// <param name="format">IN - The format for the log string.</param>
void LogS5DLLStatus(S7_Result status, LPCTSTR format, ...);

/// <summary>
/// Logs the result of an S5 API call, if the result is not S5DLL_STATUS_OK.
/// </summary>
/// <param name="result">IN - The result status.</param>
/// <param name="format">IN - The format for the log string.</param>
/// <param name="argList">IN - Variable argument info for "format".</param>
void LogS5DLLStatus(S7_Result status, LPCTSTR format, va_list argList);

