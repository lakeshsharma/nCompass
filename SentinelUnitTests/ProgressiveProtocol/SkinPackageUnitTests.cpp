#include "stdafx.h"
#include "ProgressiveProtocol/SkinPackage.h"
#include "Utilities.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

const std::string skinDownloadMessage("{\"Skin\":{\"Id\":101,\"Name\":\"silverskin\",\"SkinRevision\":5,\"Version\":\"1.1\",\"MD5\":\"w27ae08133f04a68441ed9908f20c5d2\",\"Resolution\":{\"Name\":\"nCompassLCD\",\"Width\":640,\"Height\":240},\"BaseDownloadUrl\":\"http://localhost/onelink\",\"LanguagePacks\":[{\"Id\":10,\"Name\":\"English\",\"Code\":\"en\",\"MD5\":\"w27ae08133f04a68441ed9908f20c5d2\"}],\"Files\":[{\"FileName\":\"samplefile1.png\",\"MD5\":\"d27ae08133f04a68441ed9908f20c5d2\"}]}}");

class SkinPackageTests : public ::testing::Test
{
};

TEST_F(SkinPackageTests, SkinPackageTest)
{
	// expected data
	DWORD id = 101;
	DWORD skinRevision = 5;
	std::string name = "silverskin";
	std::string version = "1.1";
	std::string baseUrl = "http://localhost/onelink";
	std::string skinUrl = "http://localhost/onelink/skins/101/silverskin.json";
	std::string skinChecksum = "w27ae08133f04a68441ed9908f20c5d2";

	std::string resolutionName = "nCompassLCD";
	DWORD resolutionWidth = 640;
	DWORD resolutionHeight = 240;
	// setup
	rapidjson::Document jsonDoc;
	jsonDoc.Parse(skinDownloadMessage.c_str()).HasParseError();

	// perform test
	CSkinPackage obj(jsonDoc);

	// verify
	ASSERT_TRUE(obj.IsValid()) << "CSkinPackage instance is invalid";
	ASSERT_EQ(obj.GetId(), id) << "Id is incorrect";
	ASSERT_EQ(obj.GetRevision(), skinRevision) << "SkinRevision is incorrect";
	ASSERT_EQ(obj.GetName(), name) << "Name is incorrect";
	ASSERT_EQ(obj.GetVersion(), version) << "Version is incorrect";
	ASSERT_EQ(obj.GetSkinUrl(), skinUrl) << "Skin URL is incorrect";
	ASSERT_EQ(obj.GetSkinChecksum(), skinChecksum) << "Skin checksum is incorrect";
	ASSERT_EQ(obj.GetBaseUrl(), baseUrl) << "Base URL is incorrect";

	SkinResolution resolution = obj.GetResolution();
	ASSERT_EQ(resolution.name, resolutionName) << "Resolution.name is incorrect";
	ASSERT_EQ(resolution.width, resolutionWidth) << "Resolution.width is incorrect";
	ASSERT_EQ(resolution.height, resolutionHeight) << "Resolution.height is incorrect";

	ASSERT_EQ(obj.GetLanguagePacks().size(), 1) << "obj.GetLanguagePacks().size() is incorrect";

	for (auto &languagePack : obj.GetLanguagePacks())
	{
		ASSERT_EQ(languagePack->id, 10) << "languagePack->id is incorrect";
		ASSERT_EQ(languagePack->name, "English") << "languagePack->name is incorrect";
		ASSERT_EQ(languagePack->code, "en") << "languagePack->code is incorrect";
		ASSERT_EQ(languagePack->checksum, "w27ae08133f04a68441ed9908f20c5d2") << "languagePack->checksum is incorrect";
		ASSERT_EQ(languagePack->url, "http://localhost/onelink/skins/101/en.json") << "languagePack->url is incorrect";
	}

	ASSERT_EQ(obj.GetFiles().size(), 1) << "obj.GetFiles().size() is incorrect";

	for (auto &file : obj.GetFiles())
	{
		ASSERT_EQ(file->filename, "samplefile1.png") << "file->filename is incorrect";
		ASSERT_EQ(file->checksum, "d27ae08133f04a68441ed9908f20c5d2") << "file->checksum is incorrect";
		ASSERT_EQ(file->url, "http://localhost/onelink/original/samplefile1.png") << "file->url is incorrect";
	}
}
