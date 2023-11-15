#if 000//TODO-PORT
#include "stdafx.h"
#include "Jackpot.h"
#include "TestUtil.h"
#include "ProcessDump\WebDiagnosticsCommonInfo.h"

using namespace std;
using namespace TestUtil;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	static CWebDiagnosticsCommonInfoBase commonInfo(
		CTime(2015, 8, 28, 13, 0, 9),
		CTime(2015, 1, 27, 13, 14, 15),
		2976,
		55553,
		_T("1.50.0.20"),
		_T("1.50.1"));

	TEST_CLASS(WebDiagnosticsJackpotInfoTests)
	{
		TEST_METHOD(WebDiagnosticsJackpotInfo_Base)
		{
			CJackpot jackpot;

			jackpot.SetJackpotID(1);
			jackpot.SetJackpotType(JT_HANDPAY);
			jackpot.SetJackpotTransactionType(JTRANS_COMPOSITE);
			jackpot.SetJackpotAmount(2345);
			jackpot.SetProgressiveLevel(6);
			jackpot.SetCoinsPlayed(789);
			jackpot.SetKeyToCreditAvailable(true);
			jackpot.SetUnattendedState(CJackpot::UJPAuthorized);
			jackpot.SetUnattendedStateTimestamp(87654321);
			//jackpot.SetTransactionSent();
			byte cardID[] = { 0, 10, 127, 128, 255 };
			jackpot.SetCardID(cardID, _countof(cardID));
			jackpot.SetHandPayReset(false);

			// Verifying the HTML is a waste of time. What matters is that no exceptions
			// are thrown while creating it and it doesn't take too long to create it.

			stringstream ss;
			jackpot.WriteWebDiagnostics(commonInfo, ss);

			OutputDebugStringA("#### WebDiagnosticsJackpotInfo_Base #####\n");
			OutputDebugStringA(ss.str().c_str());
			OutputDebugStringA("\n#### WebDiagnosticsJackpotInfo_Base #####\n");
		}
	};
}
#endif //TODO-PORT