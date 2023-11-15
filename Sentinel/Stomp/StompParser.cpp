#include "stdafx.h"

#include "StompParser.h"
#include "StompHelper.h"
#include "EnumMap.h"

using namespace std;

namespace Stomp
{
	Parser::Parser()
	{
	}

	Parser::~Parser()
	{
	}

	Err Parser::OnCommand(RecvCommand::Type command)
	{
        UNREFERENCED_PARAMETER(&command);

		return Success;
	}

	Err Parser::OnHeader(char* pKey, size_t keyLen, char* pValue, size_t valueLen)
	{
        UNREFERENCED_PARAMETER(&pKey);
        UNREFERENCED_PARAMETER(&keyLen);
        UNREFERENCED_PARAMETER(&pValue);
        UNREFERENCED_PARAMETER(&valueLen);

		return Success;
	}

	Err Parser::OnBody(char* pBody, size_t bodyLen)
	{
        UNREFERENCED_PARAMETER(&pBody);
        UNREFERENCED_PARAMETER(&bodyLen);

		return Success;
	}

	///	<summary>
	///	Returns the length of end-of-line (EOL) at a character position.
	///	</summary>
	///	<returns>If on an EOL, the number of characters used else a -1 if
	/// at a NULL.
	/// </returns>
	///	<remarks>
	///	NO error checking is done on the pFrame pointer.
	///	
	///	For STOMP 1.0 and 1.1, an end of line is a \n (LF) only and a \r (CR) is considered a fatal error.
	///	STOMP 1.2 allows lines to end with \n or \r\n.
	///	</remarks>
	int Parser::GetEolLen(const char* pStr, Version version)
	{
		if (!*pStr)
			return -1;

		// A <LF> by itself is ALWAYS an EOL
		if (*pStr == '\n')
			return 1;

		if (*pStr == '\r')
		{
			if (version >= V1_2)
			{
				return pStr[1] == '\n' ? 2 : -1;
			}
			else
			{
				return -1;
			}
		}

		return 0;
	}

	/// <summary>
	/// Returns the command pointed to by the passed <paramref name="pStr"/>.
	/// </summary>
	/// <param name="pStr">A pointer to the possible command.</param>
	/// <param name="len">The length of the string.</param>
	/// <param name="version">The version to use to determine the command.</param>
	/// <returns>The command enumeration that is the equivalent to the passed string, or 
	/// Invalid if <paramref name="pStr"/> is nullptr, <paramref name="len"/> is zero or
	/// no command matches the string for this version of STOMP.</returns>
	///	<remarks>
	/// STOMP 1.0, 1.1 and 1.2 define only four frames that come from a server:
	/// CONNECTED, MESSAGE, RECEIPT AND ERROR.
	///	</remarks>
	RecvCommand::Type Parser::DetermineCommand(char*& pStr, Version version)
	{    
		if (pStr)
		{
			char* pCommand = pStr;

			while (*pStr >= 'A' && *pStr <= 'Z')
				pStr++;

			int eolLen = GetEolLen(pStr, version);
			if (eolLen > 0)
			{
				*pStr = 0;
				pStr += eolLen;
				return RecvCommand::emap.At(pCommand);
			}
		}
		return RecvCommand::Invalid;
	}

	Parser::ParseHeader::ParseHeader(Version version)
		: pKey(nullptr)
		, keyLen(0)
		, pVal(nullptr)
		, valLen(0)
		, version(version)
	{
	}

	Parser::ParseHeader::~ParseHeader()
	{
	}

	Parser::ParseHeaderConnected::ParseHeaderConnected(Version version)
		: ParseHeader(version)
	{
	}

	void Parser::ParseHeader::Reset(char* pFrame)
	{
		pKey = pFrame;
		keyLen = 0;

		pVal = (char *)"";
		valLen = 0;
	}

	Err Parser::ParseHeader::Parse(char*& pFrame)
	{
		Reset(pFrame);

		int eolLen;

		for (; *pFrame != ':'; pFrame++, keyLen++)
		{
			eolLen = GetEolLen(pFrame, version);
			if (eolLen)
				return InvalidHeaderKey;

			if (*pFrame == '\\')
			{
				do
				{
					// redundant the first time through
					if (*pFrame == '\\')
					{
						switch (*++pFrame)
						{
							case 'r':
								if (version < V1_2)
									return InvalidHeaderKey;

								pKey[keyLen++] = '\r';
								break;

							case 'n':
								pKey[keyLen++] = '\n';
								break;

							case 'c':
								pKey[keyLen++] = ':';
								break;

							case '\\':
								pKey[keyLen++] = '\\';
								break;

							default:
								return InvalidHeaderKey;
						}
						pFrame++;
					}
					else
					{
						pKey[keyLen++] = *pFrame++;
					}

					eolLen = GetEolLen(pFrame, version);
					if (eolLen)
						return InvalidHeaderKey;

				} while (*pFrame != ':');

				break;
			}
		}

		if (version < V1_2)
		{
			while (keyLen && (pKey[keyLen - 1] == ' ' || pKey[keyLen - 1] == '\t'))
				keyLen--;
		}

		if (!keyLen)
			return InvalidHeaderKey;

		pFrame++;

		// We now have a key and pFrame points to the value

		if (version < V1_2)
		{
			while (*pFrame == ' ' || *pFrame == '\t')
				pFrame++;
		}

		pVal = pFrame;

		for (;; pFrame++, valLen++)
		{
			eolLen = GetEolLen(pFrame, version);
			if (eolLen)
				break;

			if (*pFrame == '\\')
			{
				do
				{
					// redundant the first time through
					if (*pFrame == '\\')
					{
						switch (*++pFrame)
						{
							case 'r':
								if (version < V1_2)
									return InvalidHeaderValue;

								pVal[valLen++] = '\r';
								break;

							case 'n':
								pVal[valLen++] = '\n';
								break;

							case 'c':
								pVal[valLen++] = ':';
								break;

							case '\\':
								pVal[valLen++] = '\\';
								break;

							default:
								return InvalidHeaderValue;
						}
						pFrame++;
					}
					else
					{
						pVal[valLen++] = *pFrame++;
					}

					eolLen = GetEolLen(pFrame, version);
					if (eolLen)
						break;

				} while (*pFrame != ':');

				break;
			}
		}

		pKey[keyLen] = 0;
		pVal[valLen] = 0;

		pFrame += eolLen;

		return Success;
	}

	Err Parser::ParseHeaderConnected::Parse(char*& pFrame)
	{
		Reset(pFrame);

		int eolLen;

		for (; *pFrame != ':'; pFrame++)
		{
			eolLen = GetEolLen(pFrame, version);
			if (eolLen)
				return InvalidHeaderKey;
		}

		keyLen = pFrame++ - pKey;

		if (version < V1_2)
		{
			while (keyLen && (pKey[keyLen - 1] == ' ' || pKey[keyLen - 1] == '\t'))
				keyLen--;
		}

		if (!keyLen)
			return InvalidHeaderKey;

		// We now have a key and pFrame points to the value

		if (version < V1_2)
		{
			while (*pFrame == ' ' || *pFrame == '\t')
				pFrame++;
		}

		pVal = pFrame;

		for (;; pFrame++)
		{
			eolLen = GetEolLen(pFrame, version);
			if (eolLen)
				break;
		}

		valLen = pFrame - pVal;

		pKey[keyLen] = 0;
		pVal[valLen] = 0;

		pFrame += eolLen;

		return Success;
	}

	int Parser::GetContentLengthVal(const char* pStr)
	{
		if (!*pStr)
			return ContentLengthInvalid;

		size_t contentLen = 0;

		while (*pStr)
		{
			if (!isdigit(*pStr))
				return ContentLengthInvalid;

			const int BaseTen = 10;
			contentLen = (contentLen * BaseTen) + (*pStr++ - '0');
			if (contentLen > MaxBodyLen)
				return ContentLengthInvalid;
		}

		return contentLen;
	}

	bool Parser::VerifyEndOfHeader(const char* pEnd, Version version)
	{
		if (*pEnd-- == '\n')
		{
			// \n\n is always valid
			if (*pEnd == '\n')
				return true;

			if (version >= V1_2)
			{
				// \n\r\n is valid in Version 1.2
				if (*pEnd-- == '\r')
					return *pEnd == '\n';
			}
		}
		return false;
	}

	Err Parser::Parse(char* pFrame, size_t frameLen, Version version, bool headerOnly, int* pContentLen)
	{
		if (pContentLen)
			*pContentLen = ContentLengthMissing;

		if (!pFrame)
			return NullFrame;

		if (frameLen == 0 || !*pFrame)
			return ZeroLength;

		if (frameLen > MaxFrameLen)
			return InvalidLength;

		char* pEnd = &pFrame[frameLen - 1];
		if (headerOnly)
		{
			if (frameLen < MinHeaderLen || !VerifyEndOfHeader(pEnd, version))
				return InvalidHeader;
		}
		else
		{
			if (frameLen < MinFrameLen || *pEnd != 0)
				return InvalidFrame;
		}

		RecvCommand::Type command = DetermineCommand(pFrame, version);
		if (command == RecvCommand::Invalid)
			return InvalidCommand;

		Err err = OnCommand(command);
		if (err)
			return err;

		int eolLen = GetEolLen(pFrame, version);
		if (eolLen < 0)
			return InvalidHeader;

		if (eolLen == 0)
		{
			ParseHeader				parseHeaderBase(version);
			ParseHeaderConnected	parseHeaderConnected(version);

			ParseHeader& parseHeader(command == RecvCommand::Connected ? parseHeaderConnected : parseHeaderBase);

			do
			{
				err = parseHeader.Parse(pFrame);
				if (err)
					return err;

				if (pContentLen && strcmp(parseHeader.pKey, Header::pContentLength) == 0)
				{
					*pContentLen = GetContentLengthVal(parseHeader.pVal);
				}

				err = OnHeader(parseHeader.pKey, parseHeader.keyLen,
							   parseHeader.pVal, parseHeader.valLen);
				if (err)
					return err;

				eolLen = GetEolLen(pFrame, version);

			} while (eolLen == 0);

			if (eolLen < 0)
				return InvalidFrame;

			// pFrame should now be pointing at the EOL marking the
			// end of the headers.
		}

		pFrame += eolLen;
		return headerOnly || *pFrame == 0 ? Success : OnBody(pFrame, pEnd - pFrame);
	}

}


