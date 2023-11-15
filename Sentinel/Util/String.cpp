#include "stdafx.h"
#include "String.h"
#include "Convert.h"

#undef min
#undef max

using namespace std;

namespace String
{
	const char CR = '\r';
	const char LF = '\n';

	string Concat(const char* pStr1, const char* pStr2)
	{
		string result(pStr1 ? pStr1 : "");
		if (pStr2)
		{
			result += pStr2;
		}
		return result;
	}

	string Concat(const char* pStr1, const wchar_t* pStr2)
	{
		return Concat(pStr1, Convert::WStrToUtf8(pStr2).c_str());
	}

	string Concat(const wchar_t* pStr1, const char* pStr2)
	{
		return Concat(Convert::WStrToUtf8(pStr1).c_str(), pStr2);
	}

	string Concat(const wchar_t* pStr1, const wchar_t* pStr2)
	{
		return Concat(Convert::WStrToUtf8(pStr1).c_str(), Convert::WStrToUtf8(pStr2).c_str());
	}

	int GetEolLen(const char* pStr)
	{
		if (pStr)
		{
			if (pStr[0] == LF)
				return 1;

			if (pStr[0] == CR)
				return pStr[1] == LF ? 2 : 1;
		}
		return 0;
	}

	string& RemoveAll(string& str, const char* pFind)
	{
		if (!str.empty() && pFind != nullptr && *pFind != 0)
		{
			size_t findLen = strlen(pFind);

			for (string::size_type i = 0; (i = str.find(pFind, i, findLen)) != string::npos;)
			{
				str.erase(i, findLen);
			}
		}
		return str;
	}

	string& ReplaceAll(string& str, const char* pFind, const char* pReplace)
	{
		if (pReplace == nullptr || *pReplace == 0)
		{
			str = RemoveAll(str, pFind);
		}
		else if (!str.empty() && pFind != nullptr && *pFind != 0)
		{
			size_t findLen = strlen(pFind);
			size_t replaceLen = strlen(pReplace);
			
			for (string::size_type i = 0; (i = str.find(pFind, i, findLen)) != string::npos; i += replaceLen)
			{
				str.replace(i, findLen, pReplace, replaceLen);
			}
		}
		return str;
	}

	static char hexDigits[2][16] = 
	{
		{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' },
		{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
	};

	static const uint64_t LowestNibble = 0x000000000000000f;
	static const uint8_t LowNibble = 0x0f;
	static const int NibbleLen = 4;
	static const int CharactersPerByte = 2;

	string& AppendHex(uint64_t val, string& str, bool useUpperCase, int padLen)
	{
		if (padLen > MaxLen_Uint64HexStr)
		{
			padLen = MaxLen_Uint64HexStr;
		}

		if (val == 0)
		{
			if (padLen <= 0)
			{
				str += '0';
			}
			else
			{
				str.append(padLen, '0');
			}
		}
		else
		{
			char* pHexDigits = hexDigits[useUpperCase ? 1 : 0];
			char hexStr[MaxLen_Uint64HexStr];
			int offset = _countof(hexStr);

			do
			{
				hexStr[--offset] = pHexDigits[val & LowestNibble];
				val >>= NibbleLen;

			} while (val != 0);


			int padOffset = (int) _countof(hexStr) - padLen;
			while (offset > padOffset)
			{ 
				hexStr[--offset] = '0';
			}

			str.append(&hexStr[offset], _countof(hexStr) - offset);
		}

		return str;
	}

	string ToHex(uint64_t val, bool useUpperCase, int padLen)
	{
		string str;
		str.reserve(MaxLen_Uint64HexStr);
		AppendHex(val, str, useUpperCase, padLen);
		return str;
	}

	string& BinToHex(const void* _pData, size_t dataLen, string& str, bool useUpperCase, char separator)
	{
		if (_pData != nullptr && dataLen != 0)
		{
			str.reserve(dataLen * (CharactersPerByte + (separator == 0 ? 0 : 1)));
			char* pHexDigits = hexDigits[useUpperCase ? 1 : 0];

			const uint8_t* pData = (const uint8_t*) _pData;

			for (size_t i = 0; i < dataLen;)
			{
				str += pHexDigits[pData[i] >> NibbleLen];
				str += pHexDigits[pData[i++] & LowNibble];

				if (separator != 0 && i < dataLen)
				{
					str += separator;
				}
			}
		}
		return str;
	}

	string BinToHex(const void* _pData, size_t dataLen, bool useUpperCase, char separator)
	{
		string str;
		BinToHex(_pData, dataLen, str, useUpperCase, separator);
		return str;
	}

	char* BinToHex(const void* _pData, size_t dataLen, char* pBuffer, size_t bufferLen, bool useUpperCase, char separator)
	{
		if (pBuffer != nullptr && bufferLen > 0)
		{
			size_t dstIndex = 0;

			if (_pData != nullptr && dataLen != 0)
			{
				bufferLen--;

				char* pHexDigits = hexDigits[useUpperCase ? 1 : 0];
				const uint8_t* pData = (const uint8_t*) _pData;

				for (size_t srcIndex = 0; srcIndex < dataLen && dstIndex < bufferLen;)
				{
					pBuffer[dstIndex++] = pHexDigits[pData[srcIndex] >> NibbleLen];
					if (dstIndex < bufferLen)
					{
						pBuffer[dstIndex++] = pHexDigits[pData[srcIndex++] & LowNibble];

						if (separator != 0 && srcIndex < dataLen && dstIndex < bufferLen)
						{
							pBuffer[dstIndex++] = separator;
						}
					}
				}
			}

			pBuffer[dstIndex] = 0;
		}
		return pBuffer;
	}

	size_t SafeCopy(char* pDst, const char* pSrc, size_t len, bool fill, bool* pErr)
	{
		if (pErr != nullptr)
			*pErr = false;

		if (pDst && len > 0)
		{
			size_t copyLen = len - 1;

			if (pSrc != nullptr)
			{
				for (len = 0; len < copyLen && pSrc[len]; len++)
				{
					pDst[len] = pSrc[len];
				}

				if (len == copyLen && pErr != nullptr)
					*pErr = true;
			}
			else
			{
				len = 0;
			}

			if (fill)
			{
				memset(&pDst[len], 0, copyLen - len + 1);
			}
			else
			{
				pDst[len] = 0;
			}
		}
		else
		{
			len = 0;
		}
		return len;
	}

	size_t SafeCopy(wchar_t* pDst, const wchar_t* pSrc, size_t len, bool fill, bool* pErr)
	{
		if (pErr != nullptr)
			*pErr = false;

		if (pDst && len > 0)
		{
			size_t copyLen = len - 1;

			if (pSrc != nullptr)
			{
				for (len = 0; len < copyLen && pSrc[len]; len++)
				{
					pDst[len] = pSrc[len];
				}

				if (len == copyLen && pErr != nullptr)
					*pErr = true;
			}
			else
			{
				len = 0;
			}

			if (fill)
			{
				memset(&pDst[len], 0, (copyLen - len + 1) * sizeof(wchar_t));
			}
			else
			{
				pDst[len] = 0;
			}
		}
		else
		{
			len = 0;
		}
		return len;
	}
}
