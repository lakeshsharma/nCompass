#include "stdafx.h"
#include "AssertMsg.h"

using namespace std;

namespace AssertMsg
{
	const wchar_t* FormatV(wchar_t* pBuffer, size_t bufferLen, int lineNum, const wchar_t* pFieldName, const wchar_t* pFormat, va_list args)
	{
		if (!pBuffer || bufferLen < 8)
			return L"";

		pBuffer[0] = 0;

		int len = 0;

		if (lineNum >= 0 && pFieldName)
		{
			len = swprintf(pBuffer, bufferLen, L"Line %d: %s", lineNum, pFieldName);
		}
		else if (lineNum < 0 && pFieldName)
		{
			len = swprintf(pBuffer, bufferLen, L"%s", pFieldName);
		}
		else if (lineNum >= 0 && !pFieldName)
		{
			len = swprintf(pBuffer, bufferLen, L"Line %d", lineNum);
		}

		if (pFormat && len >= 0)
		{
			if (len > 0 && (size_t) len < bufferLen - 3)
			{
				pBuffer[len++] = L':';
				pBuffer[len++] = L' ';
				pBuffer[len] = 0;

				vswprintf(&pBuffer[len], bufferLen - len, pFormat, args);
			}
			else if (len == 0)
			{
				vswprintf(&pBuffer[len], bufferLen - len, pFormat, args);
			}
		}

		return pBuffer;
	}

	const wchar_t* Format(wchar_t* pBuffer, size_t bufferLen, int lineNum, const wchar_t* pFieldName, const wchar_t* pFormat, ...)
	{
		const wchar_t* pReturn;

		if (pFormat && *pFormat)
		{
			va_list args;
			va_start(args, pFormat);
			pReturn = FormatV(pBuffer, bufferLen, lineNum, pFieldName, pFormat, args);
			va_end(args);
		}
		else
		{
			pReturn = FormatV(pBuffer, bufferLen, lineNum, pFieldName, nullptr, nullptr);
		}

		return pReturn;
	}

	const wchar_t* FormatV(int lineNum, const wchar_t* pFieldName, const wchar_t* pFormat, va_list args)
	{
		static wchar_t assertMsg[512];
		return FormatV(assertMsg, _countof(assertMsg), lineNum, pFieldName, pFormat, args);
	}

	const wchar_t* Format(int line, const wchar_t* pField, const wchar_t* pFormat, ...)
	{
		const wchar_t* pReturn;

		if (pFormat && *pFormat)
		{
			va_list args;
			va_start(args, pFormat);
			pReturn = FormatV(line, pField, pFormat, args);
			va_end(args);
		}
		else
		{
			pReturn = FormatV(line, pField, nullptr, nullptr);
		}

		return pReturn;
	}

	const wchar_t* FormatOnly(const wchar_t* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		const wchar_t* pResult = FormatV(-1, nullptr, pFormat, args);
		va_end(args);
		return pResult;
	}

	const char* FormatV(char* pBuffer, size_t bufferLen, const char* pFormat, va_list args)
	{
		const char *pResult("");
		if (pBuffer != nullptr && bufferLen > 0)
		{
			vsnprintf(pBuffer, bufferLen, pFormat, args);
			pResult = pBuffer;
		}

		return pResult;
	}

	const char* FormatOnly(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		static char assertMsg[AssertMsgBufferLen];
		const char *pResult = FormatV(assertMsg, _countof(assertMsg), pFormat, args);
		va_end(args);
		return pResult;
	}
}
