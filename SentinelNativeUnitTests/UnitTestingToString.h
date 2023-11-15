#pragma once
#if 000//TODO-PORT
#include "GlobalDefs.h"
#include "Hardware/MemoryStorage.h"
#include "InternalMessage.h"
#include "MessagePersist.h"
#include "PersonalBanker.h"
#include "ProgressiveProtocol/EGMAward.h"
#include "ProgressiveProtocol/EGMAwardManager.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "ProgressiveProtocol/ProgressiveKeys.h"
#include "ProgressiveProtocol/ProgressiveLevelForward.h"
#include "PlayerSession.h"
#include "SASProtocol/LongPoll8EMessage.h"
#include "SASProtocol/SASPollMessage.h"
#include "TiltsDef.h"

// Below are templates used to create appropriate messages for AreEqual, AreSame, AreNotEqual and AreNotSame asserts.
// Could move nCompass-specific types to the nCompass headers where they are declared.
namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			// General types.
			template<> static std::wstring ToString<CString>(const CString &t) { std::wstringstream _s(t.GetString());	; return _s.str(); }
			
			template<> static std::wstring ToString<CTime>(const CTime &t) { RETURN_WIDE_STRING(std::wstring(t.Format(L"%c"))); }
			template<> static std::wstring ToString<CTimeSpan>(const CTimeSpan &t) { RETURN_WIDE_STRING(std::wstring(t.Format(L"%Dd %H:%M:%S"))); }
			template<> static std::wstring ToString<WORD>(const WORD &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<__int64>(const __int64 &t) { RETURN_WIDE_STRING(t); }

			template<> static std::wstring ToString<SYSTEMTIME>(const SYSTEMTIME &t) {
				std::wostringstream ss;
				ss << std::setw(4) << std::setfill(L'0') << std::internal << t.wYear;
				ss << L'-' << std::setw(2) << std::setfill(L'0') << std::internal << t.wMonth;
				ss << L'-' << std::setw(2) << std::setfill(L'0') << std::internal << t.wDay;
				ss << L' ' << std::setw(2) << std::setfill(L'0') << std::internal << t.wHour;
				ss << L':' << std::setw(2) << std::setfill(L'0') << std::internal << t.wMinute;
				ss << L':' << std::setw(2) << std::setfill(L'0') << std::internal << t.wSecond;
				ss << L'.' << std::setw(3) << std::setfill(L'0') << std::internal << t.wMilliseconds;
				ss << L" wDayOfWeek=" << t.wDayOfWeek;
				return ss.str();
			}

			// nCompass types.
			template<> static std::wstring ToString<AwardType::Type>(const AwardType::Type &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<CLongPoll8EMessage::PokerHandType>(const CLongPoll8EMessage::PokerHandType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<DenomCurrencyType>(const DenomCurrencyType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMAwardPaidType>(const EGMAwardPaidType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMAwardState>(const EGMAwardState &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMAwardTransferType>(const EGMAwardTransferType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMPollingSpeed>(const EGMPollingSpeed &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMProtocol>(const EGMProtocol &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<EGMProtocolState>(const EGMProtocolState &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<InsertTranslation>(const InsertTranslation &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<InternalMsgType>(const InternalMsgType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<MEMORY_TYPE>(const MEMORY_TYPE &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<MessagePersistType>(const MessagePersistType &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<MessageQueueId>(const MessageQueueId &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<MethodOfPayment::Type>(const MethodOfPayment::Type &t) { RETURN_WIDE_STRING((int)t); }
			template<> static std::wstring ToString<PersonalBankerAction>(const PersonalBankerAction &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<PersonalBankerState>(const PersonalBankerState &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<ProgressiveConnectionState>(const ProgressiveConnectionState &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<ProgressiveLevelType::Type>(const ProgressiveLevelType::Type &t) { RETURN_WIDE_STRING((int)t); }
			template<> static std::wstring ToString<SASMeterCode>(const SASMeterCode &t) { RETURN_WIDE_STRING(t); }
			template<> static std::wstring ToString<TiltCode>(const TiltCode &t) { RETURN_WIDE_STRING(t); }
		}
	}
}
#endif //TODO-PORT