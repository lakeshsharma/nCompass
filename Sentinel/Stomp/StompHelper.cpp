#include "stdafx.h"

#include "StompHelper.h"
#include "EnumMap.h"

using namespace std;

namespace Stomp
{
	static const wchar_t* versionStringsW[] =
	{
		L"1.0",
		L"1.1",
		L"1.2"
	};

	const wchar_t* GetVersionStrW(Version version)
	{
		return (size_t) version < _countof(versionStringsW) ? versionStringsW[(size_t) version] : L"";
	}

	static const char* versionStrings[] =
	{
		"1.0",
		"1.1",
		"1.2"
	};

	const char* GetVersionStr(Version version)
	{
		return (size_t) version < _countof(versionStrings) ? versionStrings[(size_t) version] : "";
	}

	Version GetVersion(const char* pVersion)
	{
		if (!pVersion)
			return V1_0;

		while (*pVersion == ' ' || *pVersion == '\t')
			pVersion++;

		Version version = V1_0;

		while (*pVersion)
		{
			const char* pStart = pVersion;
			while (*pVersion && *pVersion != ',')
				pVersion++;

			size_t len = pVersion - pStart;
			for (size_t i = (size_t) V1_0; i < _countof(versionStrings); i++)
			{
				if (strncmp(pStart, versionStrings[i], len) == 0)
				{
					if (i > (size_t) version)
						version = (Version) i;

					break;
				}
			}

			if (*pVersion)
				pVersion++;
		}

		return version;
	}

	///////////////////////////////////////////////////////////////////////////

	namespace RecvCommand
	{
		static EnumMap<Type>::EnumPair types[] =
		{
			{ Connected, "CONNECTED" },
			{ Message  , "MESSAGE"	 },
			{ Receipt  , "RECEIPT"	 },
			{ Error    , "ERROR"	 }
		};

		EnumMap<Type> emap(types);
	}

	namespace Header
	{
		// Connected
		const char* pVersion		= "version";
		const char* pHeartBeat		= "heart-beat";
		const char* pSession		= "session";
		const char* pServer			= "server";

		// Message
		const char* pDestination	= "destination";
		const char* pSubscription	= "subscription";
		const char* pContentLength	= "content-length";
		const char* pContentType	= "content-type";
		const char* pMessageId		= "message-id";

		// Receipt
		const char* pReceiptId		= "receipt-id";

		// Error (pHdrContentLen and pHdrContentType are also optional)
		const char* pMessage		= "message";

		static EnumMap<Type>::EnumPair types[] =
		{
			{ ContentLength, pContentLength	},
			{ ContentType,	 pContentType	},
			{ Destination,	 pDestination	},	
			{ HeartBeat,	 pHeartBeat		},
			{ Message,		 pMessage		},
			{ MessageId,	 pMessageId		},
			{ ReceiptId,	 pReceiptId		},		
			{ Server,		 pServer		},
			{ Session,		 pSession		},
			{ Subscription,	 pSubscription	},	
			{ Version,		 pVersion		}
		};

		EnumMap<Type> emap(types);
	}

	namespace SendCommand
	{
		// This doesn't use the EnumMap since the client will never parse these frames.
		static const char* Str[] =
		{
			"CONNECT",
			"DISCONNECT",
			"SUBSCRIBE",
			"UNSUBSCRIBE",
			"SEND",
			"ACK"
		};

		const char* At(Type command, bool throwOnError)
		{
			if ((size_t) command >= _countof(Str))
			{
				if (throwOnError)
					throw StompException(InvalidCommand);

				return nullptr;
			}

			return Str[(size_t) command];
		}

		const char* AtEx(Type command)
		{
			return At(command, true);
		}
	};
}

