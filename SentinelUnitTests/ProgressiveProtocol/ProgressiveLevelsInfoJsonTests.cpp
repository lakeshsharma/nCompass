#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol/ProgressiveConnection.h"
#include "ProgressiveLevelTestHelpers.h"
#include "AssertMsg.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	struct
	{
		int line;
		const char* pJson;
		DWORD trueGameNumbers[32];
		DWORD falseGameNumbers[32];
	}
	static jsonStrings[] =
	{
		{
			__LINE__,
			"{\"Games\":["
			"{\"GameNumber\":9,\"Denomination\":0.05,\"MaxBetCredits\":5,\"TheoPercentage\":99.17,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":2,\"Denomination\":0.05,\"MaxBetCredits\":5,\"TheoPercentage\":99.54,\"GameName\":\"Jacks Or Better\",\"Paytable\":\"\",\"QueryCards\":false,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":46,\"Denomination\":0.05,\"MaxBetCredits\":8,\"TheoPercentage\":94.99,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":24,\"Denomination\":0.05,\"MaxBetCredits\":5,\"TheoPercentage\":98.98,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":68,\"Denomination\":0.05,\"MaxBetCredits\":10,\"TheoPercentage\":95.36,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":false,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":52,\"Denomination\":0.05,\"MaxBetCredits\":8,\"TheoPercentage\":94.35,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":73,\"Denomination\":0.05,\"MaxBetCredits\":80,\"TheoPercentage\":96.22,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":62,\"Denomination\":0.05,\"MaxBetCredits\":80,\"TheoPercentage\":94.99,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":false,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":57,\"Denomination\":0.05,\"MaxBetCredits\":8,\"TheoPercentage\":95.25,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":31,\"Denomination\":0.05,\"MaxBetCredits\":5,\"TheoPercentage\":99.73,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":83,\"Denomination\":0.05,\"MaxBetCredits\":80,\"TheoPercentage\":96.16,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":false,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]},"
			"{\"GameNumber\":16,\"Denomination\":0.05,\"MaxBetCredits\":5,\"TheoPercentage\":99.11,\"GameName\":\"Game King - N/A\",\"Paytable\":\"\",\"QueryCards\":true,\"Levels\":[{\"LevelId\":24093,\"LevelSequence\":0}]}],"
			"\"Levels\":[{\"Id\":24093,\"Version\":128,\"Type\":\"Bonus\",\"Channel\":{\"Destination\":\"@239.1.1.1:61613\",\"MulticastDestination\":\"stomp://239.1.1.1:61613\"},"
			"\"Info\":\"\",\"Name\":\"JoePoker\",\"MediaMinSeconds\":2,\"MediaMaxSeconds\":10}],"
			"\"ProtocolType\":\"SAS\",\"ProgressiveGroupId\":1,\"PaymentMethod\":\"SasLegacyBonus\",\"UseAFTLockOnTransfers\":false,\"MeterBankLevel\":false,\"MeterMachineOnly\":false,\"ContentHttpUrl\":\"http://10.138.98.10:8080/onelink/content/original/\","
			"\"Playlist\":{\"Name\":\"SLOT_PLAYLIST\",\"Items\":["
			"{\"Content\":{\"FileName\":\"640X720_poly_attract.mp4\",\"MD5\":\"91cc7eef0f2abb033f0c8e6cb56cdbb9\"},\"Enabled\":true,\"BeginDate\":\"2015-06-16T00:00:00.000-0700\",\"BeginTime\":\"00:00\",\"EndTime\":\"00:00\",\"ActiveSunday\":true,\"ActiveMonday\":true,\"ActiveTuesday\":true,\"ActiveWednesday\":true,\"ActiveThursday\":true,\"ActiveFriday\":true,\"ActiveSaturday\":true,\"SequenceId\":1,\"AudioAdjustment\":1.000000,\"Mute\":false,\"Image\":false,\"Duration\":10000},"
			"{\"Content\":{\"FileName\":\"640X720_poly_ldrbrd.mp4\",\"MD5\":\"38fd23e9f2a3803b73d593bf48aed9c9\"},\"Enabled\":true,\"BeginDate\":\"2015-06-16T00:00:00.000-0700\",\"BeginTime\":\"00:00\",\"EndTime\":\"00:00\",\"ActiveSunday\":true,\"ActiveMonday\":true,\"ActiveTuesday\":true,\"ActiveWednesday\":true,\"ActiveThursday\":true,\"ActiveFriday\":true,\"ActiveSaturday\":true,\"SequenceId\":2,\"AudioAdjustment\":1.000000,\"Mute\":false,\"Image\":false,\"Duration\":10000}],"
			"\"Shuffle\":false,\"Description\":\"\"},\"DeviceId\":23985,\"DeviceConfigVersion\":0,\"DeviceAddress\":\"@device.ICARD.0060E053E4F0\",\"EventChannel\":{\"Destination\":\"@239.3.0.1:61613\",\"MulticastDestination\":\"stomp://239.3.0.1:61613\"},\"AssetNumber\":\"55554\",\"BankName\":\"Default Bank\",\"TimeZone\":\"America/Los_Angeles\",\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1434580743596,\"MessageId\":\"518\",\"OneLinkSystemId\":\"ceae0730-bbef-46ec-9cf1-292cab5fd4ca\"}",
			{ 9, 46, 24, 52, 73, 57, 31, 16, 0 },
			{ 2, 68, 62, 83, 0 }
		},
		{
			__LINE__,
			"{\"Games\":["
				"{\"GameNumber\":1,\"Denomination\":0.01,\"MaxBetCredits\":250,\"TheoPercentage\":87.82,\"GameName\":\"Aristocrat - N/A\",\"QueryCards\":false},"
				"{\"GameNumber\":2,\"Denomination\":0.01,\"MaxBetCredits\":250,\"TheoPercentage\":87.82,\"GameName\":\"Aristocrat - N/A\",\"QueryCards\":false},"
				"{\"GameNumber\":3,\"Denomination\":0.01,\"MaxBetCredits\":250,\"TheoPercentage\":87.82,\"GameName\":\"Aristocrat - N/A\",\"QueryCards\":false}],"
			"\"ProtocolType\":\"SAS\",\"ProgressiveGroupId\":0,\"PaymentMethod\":\"SasAFTBonus\",\"UseAFTLockOnTransfers\":false,\"MeterBankLevel\":false,\"MeterMachineOnly\":false,\"ContentHttpUrl\":\"http://10.138.98.10:8080/onelink/content/original/\","
			"\"Playlist\":{\"Name\":\"SLOT_PLAYLIST\",\"Items\":["
				"{\"Content\":{\"FileName\":\"640X720_poly_attract.mp4\",\"MD5\":\"91cc7eef0f2abb033f0c8e6cb56cdbb9\"},\"Enabled\":true,\"BeginDate\":\"2015-06-16T00:00:00.000-0700\",\"BeginTime\":\"00:00\",\"EndTime\":\"00:00\",\"ActiveSunday\":true,\"ActiveMonday\":true,\"ActiveTuesday\":true,\"ActiveWednesday\":true,\"ActiveThursday\":true,\"ActiveFriday\":true,\"ActiveSaturday\":true,\"SequenceId\":1,\"AudioAdjustment\":1.000000,\"Mute\":false,\"Image\":false,\"Duration\":10000},"
				"{\"Content\":{\"FileName\":\"640X720_poly_ldrbrd.mp4\",\"MD5\":\"38fd23e9f2a3803b73d593bf48aed9c9\"},\"Enabled\":true,\"BeginDate\":\"2015-06-16T00:00:00.000-0700\",\"BeginTime\":\"00:00\",\"EndTime\":\"00:00\",\"ActiveSunday\":true,\"ActiveMonday\":true,\"ActiveTuesday\":true,\"ActiveWednesday\":true,\"ActiveThursday\":true,\"ActiveFriday\":true,\"ActiveSaturday\":true,\"SequenceId\":2,\"AudioAdjustment\":1.000000,\"Mute\":false,\"Image\":false,\"Duration\":10000},"
				"{\"Content\":{\"FileName\":\"F\\u0026B_lower_third010114.jpg\",\"MD5\":\"08c250c392910c0ffee0cd53a2822025\"},\"Enabled\":true,\"BeginDate\":\"2015-06-18T00:00:00.000-0700\",\"BeginTime\":\"00:00\",\"EndTime\":\"00:00\",\"ActiveSunday\":true,\"ActiveMonday\":true,\"ActiveTuesday\":true,\"ActiveWednesday\":true,\"ActiveThursday\":true,\"ActiveFriday\":true,\"ActiveSaturday\":true,\"SequenceId\":3,\"AudioAdjustment\":1.000000,\"Mute\":false,\"Image\":true,\"Duration\":10000}],"
			"\"Shuffle\":false,\"Description\":\"\"},\"DeviceId\":24039,\"DeviceConfigVersion\":0,\"DeviceAddress\":\"@device.ICARD.206FEC00414F\",\"EventChannel\":{\"Destination\":\"@239.3.0.1:61613\",\"MulticastDestination\":\"stomp://239.3.0.1:61613\"},\"AssetNumber\":\"55553\",\"BankName\":\"Default Bank\",\"TimeZone\":\"America/Los_Angeles\",\"SourceDeviceId\":1,\"SourceDeviceConfigVersion\":0,\"SourceAsset\":\"\",\"Timestamp\":1434670855697,\"MessageId\":\"554\",\"OneLinkSystemId\":\"ceae0730-bbef-46ec-9cf1-292cab5fd4ca\"}",
			{ 0 },
			{ 1, 2, 3, 0 }
		}
	};

		TEST(ProgressiveLevelsInfoJsonTests, ProgressiveLevelsInfo_jsontests)
		{
			for (size_t i = 0; i < _countof(jsonStrings); i++)
			{
				try
				{
					rapidjson::Document jsonDoc;
					bool result = jsonDoc.Parse(jsonStrings[i].pJson).HasParseError();
					ASSERT_FALSE(result) << AssertMsg::Format(jsonStrings[i].line,
						L"Parse", L"jsonStrings[%u]: %S", i, rapidjson::GetParseError_En(jsonDoc.GetParseError()));

					ProgressiveLevelsInfo levels(jsonDoc);

					auto queryCards = levels.GetQueryPokerCardsSet();

					size_t count = 0;
					for (; jsonStrings[i].trueGameNumbers[count]; count++)
					{
						auto it = queryCards.find(jsonStrings[i].trueGameNumbers[count]);
						bool result = it != queryCards.end();
						ASSERT_TRUE(result) << AssertMsg::Format(jsonStrings[i].line,
							L"QueryCards=true", L"jsonStrings[%u]: Game Number: %u", i, jsonStrings[i].trueGameNumbers[count]);
					}

					ASSERT_EQ(count, queryCards.size()) << AssertMsg::Format(jsonStrings[i].line, L"queryCards.size()", L"jsonStrings[%u]", i);

					for (count = 0; jsonStrings[i].falseGameNumbers[count]; count++)
					{
						auto it = queryCards.find(jsonStrings[i].falseGameNumbers[count]);
						bool result = it == queryCards.end();
						ASSERT_TRUE(result) << AssertMsg::Format(jsonStrings[i].line,
							L"QueryCards=false", L"jsonStrings[%u]: Game Number: %u", i, jsonStrings[i].falseGameNumbers[count]);
					}
				}
				catch (SentinelException& e)
				{
					ASSERT_TRUE(false) << AssertMsg::Format(jsonStrings[i].line, L"CProgressiveLevels ctor", L"jsonStrings[%u]: exception: %S", i, e.what());
				}
			}
		}
}
