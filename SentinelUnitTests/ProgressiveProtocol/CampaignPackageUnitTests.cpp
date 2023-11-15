#include "stdafx.h"
#include "ProgressiveProtocol/CampaignPackage.h"
#include "Utilities.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

const std::string campaignDownloadMessage1("{\"BaseDownloadUrl\":\"http://localhost\",\"Campaign\":{\"Id\":122,\"Name\":\"MayCampaign\",\"Version\":5,\"CampaignMediaPackages\":[{\"Name\":\"MayCampaignLCDPkg\",\"Resolution\":{\"Height\":240,\"RefreshRate\":0,\"Rotation\":0,\"Width\":640},\"CampaignEventAssignments\":[{\"Event\":\"Anniversary\",\"Id\":1228532777,\"Playlist\":{\"Name\":\"AnniversaryPlaylist\",\"Items\":[{\"ActiveFriday\":false,\"ActiveMonday\":true,\"ActiveSaturday\":true,\"ActiveSunday\":false,\"ActiveThursday\":false,\"ActiveTuesday\":false,\"ActiveWednesday\":false,\"BeginDate\":\"2023-05-16T18:13:55.938-0700\",\"BeginTime\":\"00:00\",\"Content\":{\"FileName\":\"anniversary.png\",\"MD5\":\"f3f4cd1f04eecc81fac708c379cfcf8a\"},\"Duration\":10000,\"Enabled\":true,\"EndDate\":\"2023-06-16T18:13:55.938-0700\",\"EndTime\":\"00:00\",\"Image\":true,\"Mute\":false,\"SequenceId\":1}]}}]}]},\"SourceAsset\":\"\",\"SourceDeviceConfigVersion\":0,\"SourceDeviceId\":1,\"Timestamp\":1684286035939}");

class CampaignPackageTests : public ::testing::Test
{
};

TEST_F(CampaignPackageTests, CampaignPackageTest)
{
	// expected data
	std::string name("MayCampaign");
	std::string baseUrl("http://localhost");
	std::string versionStr("");
	DWORD version = 5;
	DWORD id = 122;

	// setup
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(campaignDownloadMessage1.c_str()).HasParseError();

	// perform test
	CCampaignPackage obj(jsonDoc);

	// verify
	ASSERT_EQ(obj.GetName(), name) << "Name is incorrect";
	ASSERT_EQ(obj.GetBaseUrl(), baseUrl) << "Base URL is incorrect";
	ASSERT_EQ(obj.GetId(), id) << "Id is incorrect";
	ASSERT_EQ(obj.GetCampaignInfo().GetName(), name) << "CampaignInfo.name is incorrect";
	ASSERT_EQ(obj.GetCampaignInfo().GetId(), id) << "CampaignInfo.id is incorrect";
	ASSERT_EQ(obj.GetCampaignInfo().GetVersion(), versionStr) << "CampaignInfo.version is incorrect";

	ASSERT_EQ(obj.GetCampaignMediaPackages().size(), 1) << "obj.GetCampaignMediaPackages().size() is incorrect";

	for (auto &campaignMediaPackage : obj.GetCampaignMediaPackages())
	{
		ASSERT_EQ(campaignMediaPackage->name, "MayCampaignLCDPkg") << "campaignMediaPackage->name is incorrect";
		ASSERT_EQ(campaignMediaPackage->width, 640) << "campaignMediaPackage->width is incorrect";
		ASSERT_EQ(campaignMediaPackage->height, 240) << "campaignMediaPackage->height is incorrect";

		ASSERT_EQ(campaignMediaPackage->events.size(), 1) << "campaignMediaPackage->events.size() is incorrect";

		for (auto &campEvent : campaignMediaPackage->events)
		{
			ASSERT_EQ(campEvent->name, "Anniversary") << "campEvent->name is incorrect";
			ASSERT_EQ(campEvent->playlist.name, "AnniversaryPlaylist") << "campEvent->playlist.name is incorrect";

			ASSERT_EQ(campEvent->playlist.items.size(), 1) << "campEvent->playlist.items.size() is incorrect";

			for (auto &item : campEvent->playlist.items)
			{
				ASSERT_TRUE(item->isEnabled) << "item->isEnabled is incorrect";
				ASSERT_TRUE(item->isImage) << "item->isImage is incorrect";
				ASSERT_FALSE(item->isMute) << "item->isMute is incorrect";

				ASSERT_EQ(item->filename, "anniversary.png") << "item->filename is incorrect";
				ASSERT_EQ(item->url, "http://localhost/anniversary.png") << "item->url is incorrect";
				ASSERT_EQ(item->checksum, "f3f4cd1f04eecc81fac708c379cfcf8a") << "item->checksum is incorrect";

				ASSERT_EQ(item->beginDate, "2023-05-16T18:13:55.938-0700") << "item->beginDate is incorrect";
				ASSERT_EQ(item->beginTime, "00:00") << "item->beginTime is incorrect";
				ASSERT_EQ(item->endDate, "2023-06-16T18:13:55.938-0700") << "item->endDate is incorrect";
				ASSERT_EQ(item->endTime, "00:00") << "item->endTime is incorrect";

				ASSERT_EQ(item->duration, 10000) << "item->duration is incorrect";
				ASSERT_EQ(item->sequenceId, 1) << "item->sequenceId is incorrect";

				ASSERT_FALSE(item->activeDays & CAMPAIGN_ACTIVE_SUNDAY) << "activeSunday is incorrect";
				ASSERT_TRUE(item->activeDays & CAMPAIGN_ACTIVE_MONDAY) << "activeMonday is incorrect";
				ASSERT_FALSE(item->activeDays & CAMPAIGN_ACTIVE_TUESDAY) << "activeTuesday is incorrect";
				ASSERT_FALSE(item->activeDays & CAMPAIGN_ACTIVE_WEDNESDAY) << "activeWednesday is incorrect";
				ASSERT_FALSE(item->activeDays & CAMPAIGN_ACTIVE_THURSDAY) << "activeThursday is incorrect";
				ASSERT_FALSE(item->activeDays & CAMPAIGN_ACTIVE_FRIDAY) << "activeFriday is incorrect";
				ASSERT_TRUE(item->activeDays & CAMPAIGN_ACTIVE_SATURDAY) << "activeSaturday is incorrect";
			}
		}
	}
}
