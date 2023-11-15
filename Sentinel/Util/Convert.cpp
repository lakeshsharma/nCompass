#include "stdafx.h"

#include "Convert.h"

using namespace std;

namespace Convert
{
	static wstring_convert<codecvt_utf8<wchar_t>> wstrToUtf8converter;

	string WStrToUtf8(const wchar_t* pStr)
	{
		return pStr != nullptr ? wstrToUtf8converter.to_bytes(pStr) : string();
	}

	string WStrToUtf8(const wstring& str)
	{
		return wstrToUtf8converter.to_bytes(str.c_str());
	}

	string WStrToUtf8(const wchar_t* pStr, size_t length)
	{
		return pStr != nullptr && length != 0 ? wstrToUtf8converter.to_bytes(wstring(pStr, (size_t) length)) : string();
	}

	wstring Utf8ToWStr(const char* pStr)
	{
		return pStr != nullptr ? wstrToUtf8converter.from_bytes(pStr) : wstring();
	}

	wstring Utf8ToWStr(const std::string& str)
	{
		return wstrToUtf8converter.from_bytes(str.c_str());
	}

	wstring Utf8ToWStr(const char* pStr, size_t length)
	{
		return pStr != nullptr ? wstrToUtf8converter.from_bytes(string(pStr, length)) : wstring();
	}

	string Utf8ToString(const char* pStr)
	{		
		wstring wstr = Utf8ToWStr(pStr);
		
		using convert_type = std::codecvt_utf8<wchar_t>;
		wstring_convert<convert_type, wchar_t> converter;
		
		return converter.to_bytes( wstr );
    }

#ifdef _MFC_VER
	CStringW Utf8ToCString(const char* pStr)
	{
		wstring string = Utf8ToWStr(pStr);
		return CString(string.c_str(), string.size());
	}

	CStringW Utf8ToCString(const std::string& str)
	{
		wstring string = Utf8ToWStr(str);
		return CString(string.c_str(), string.size());
	}

	CStringW Utf8ToCString(const char* pStr, size_t length)
	{
		wstring string = Utf8ToWStr(pStr, length);
		return CString(string.c_str(), string.size());
	}
#endif

	static uint64_t ToUint64(const char* pStr, const char** pEnd, bool& err, uint64_t maxValue)
	{
		uint64_t val = 0;
		err = false;

		if (*pStr == 0)
		{
			err = true;
		}
		else
		{
			uint32_t lastDigit = maxValue % BaseTen;
			maxValue /= BaseTen;

			while (*pStr)
			{
				if (val <= maxValue && isdigit(*pStr))
				{
					uint32_t chVal = (int) (*pStr - '0');
					if (val == maxValue && chVal > lastDigit)
					{
						err = true;
						break;
					}

					val *= BaseTen;
					val += chVal;
				}
				else
				{
					err = true;
					break;
				}

				pStr++;
			}
		}

		if (pEnd)
			*pEnd = pStr;

		return val;
	}

	bool ToUint64(const char* pStr, uint64_t& val, const char** pEnd)
	{
		bool err = true;

		if (pStr)
		{
			while (*pStr > 0 && *pStr <= ' ')
				pStr++;

			val = ToUint64(pStr, pEnd, err, UINT64_MAX);
		}
		else
		{
			val = 0;
		}

		return !err;
	}

	bool ToInt64(const char* pStr, int64_t& val, const char** pEnd)
	{
		bool err = true;

		if (pStr)
		{
			while (*pStr > 0 && *pStr <= ' ')
				pStr++;

			if (*pStr == '-')
			{
				val = -(int64_t) ToUint64(++pStr, pEnd, err, ((uint64_t) INT64_MAX) + 1);
			}
			else
			{
				val = (int64_t) ToUint64(pStr, pEnd, err, INT64_MAX);
			}
		}
		else
		{
			val = 0;
		}

		return !err;
	}

	static uint32_t _ToUint32(const char* pStr, const char** pEnd, bool& err, uint32_t maxValue)
	{
		uint32_t val = 0;
		err = false;

		if (*pStr == 0)
		{
			err = true;
		}
		else
		{
			uint32_t lastDigit = maxValue % BaseTen;
			maxValue /= BaseTen;

			while (*pStr)
			{
				if (val <= maxValue && isdigit(*pStr))
				{
					uint32_t chVal = (int) (*pStr - '0');
					if (val == maxValue && chVal > lastDigit)
					{
						err = true;
						break;
					}

					val *= BaseTen;
					val += chVal;
				}
				else
				{
					err = true;
					break;
				}

				pStr++;
			}
		}

		if (pEnd)
			*pEnd = pStr;

		return val;
	}

	bool ToUint32(const char* pStr, uint32_t& val, const char** pEnd)
	{
		bool err = true;

		if (pStr)
		{
			while (*pStr > 0 && *pStr <= ' ')
				pStr++;

			val = _ToUint32(pStr, pEnd, err, UINT32_MAX);
		}
		else
		{
			val = 0;
		}

		return !err;
	}

	bool ToInt32(const char* pStr, int32_t& val, const char** pEnd)
	{
		bool err = true;

		if (pStr)
		{
			while (*pStr > 0 && *pStr <= ' ')
				pStr++;

			if (*pStr == '-')
			{
				val = -(int) _ToUint32(++pStr, pEnd, err, ((uint32_t) INT32_MAX) + 1);
			}
			else
			{
				val = (int) _ToUint32(pStr, pEnd, err, INT32_MAX);
			}
		}
		else
		{
			val = 0;
		}

		return !err;
	}
}

