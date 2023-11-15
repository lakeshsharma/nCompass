#include "stdafx.h"
#include "LogString.h"

// Sentinel includes.
#include "Logger.h"

using namespace std;

void LogString(const std::string &pIdentifier, LPCSTR pFormat, ...)
{
	if (!pIdentifier.empty() &&
		pFormat != nullptr && *pFormat != 0)
	{
		va_list args;
		va_start(args, pFormat);
		LogStringV(pIdentifier, pFormat, args);
		va_end(args);
	}
}

void LogStringV(const std::string &pIdentifier, LPCSTR pFormat, va_list args)
{
	if (!pIdentifier.empty() &&
		pFormat != nullptr && *pFormat != 0)
	{
		// Determine the number of characters needed to format the arguments.
		va_list args_copy;
		va_copy(args_copy, args);
		int charsNeeded = vsnprintf(nullptr, 0, pFormat, args_copy);
		va_end(args_copy);

		if (charsNeeded > 0)
		{
			// Format the arguments.
			int charsAllocated = charsNeeded + 1; // Allocate an extra character for the trailing nul character.
			std::unique_ptr<char[]> formatted(new char[charsAllocated]);
			vsnprintf(&formatted[0], charsAllocated, pFormat, args);
			formatted[charsAllocated - 1] = 0; // Ensure that a terminating nul character is present.
			LogStringNoFormat(pIdentifier, formatted.get());
		};
	}
}

void LogStringNoFormat(const std::string &pIdentifier, LPCSTR pLogString)
{
	if (!pIdentifier.empty() &&
		pLogString != nullptr && *pLogString != 0)
	{
#ifndef _UNIT_TEST // TODO: Consider using a CSmartDataLogger instance but not logging to disk.
		CSmartDataLogger::Instance().LogString(pIdentifier, pLogString);
#endif // _UNIT_TEST.
	}
}

void LogStringNoFormat(const std::string &pIdentifier, const std::string &pLogString)
{
	LogStringNoFormat(pIdentifier, pLogString.c_str());
}

void LogStringNoFormat(const std::string &pIdentifier,const std::wstring &pLogString)
{
	LogStringNoFormat(pIdentifier, pLogString);
}

void LogData(const std::string &pIdentifier, BYTE code, const void *pData, long dataLength)
{
	if (!pIdentifier.empty() && pData != nullptr && dataLength > 0)
	{
#ifndef _UNIT_TEST // TODO: Consider using a CSmartDataLogger instance but not logging to disk.
		CSmartDataLogger::Instance().LogData(pIdentifier, code, pData, dataLength);
#endif // _UNIT_TEST.
	}
}
void LogS5DLLStatus(S7_Result status, LPCTSTR format, ...)
{
	va_list args;
	va_start(args, format);
	LogS5DLLStatus(status, format, args);
	va_end(args);
}

void LogS5DLLStatus(S7_Result status, LPCTSTR format, va_list argList)
{
	string logStr;
	logStr = FormatStringV(format, argList);

	if (S7DLL_STATUS_WARNING == status)
	{
		logStr += " -> WARNING resulted";
	}
	else if (S7DLL_STATUS_ERROR == status)
	{
		logStr += " -> ERROR resulted";
		//LogStringNoFormat(ERROR_LOG, logStr);
	}
	else if (S7DLL_STATUS_OK == status)
	{
		logStr += " -> SUCCESS resulted";
	}
	else
	{
		logStr += " -> UNKNOWN resulted";
	}

	LogStringNoFormat(SENTINEL_API_LOG, logStr);
}
