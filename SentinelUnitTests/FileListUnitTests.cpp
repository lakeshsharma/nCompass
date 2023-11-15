#include "stdafx.h"
#include "FileList.h"
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;


namespace SentinelUnitTests
{

    TEST(FileListUnitTests, FileListAdd)
    {
        CFileList fl;

        fl.Add("FileName1", 1234);

        ASSERT_TRUE(fl.GetCount() == 1);
    }

    TEST(FileListUnitTests,FileListAddRemove)
    {
        CFileList fl;

        fl.Add("FileName1", 1234);
        fl.Add("FileName2", 1233);

        ASSERT_TRUE(fl.GetCount() == 2);
        fl.Remove("FileName2");
        ASSERT_TRUE(fl.GetCount() == 1);
    }

	 TEST(FileListUnitTests,FileListForEach)
	 {
		 CFileList fl;

		 fl.Add("FileName 1", 1234);
		 fl.Add("FileName 2", 5678);
		 fl.Add("FileName 3", 3456);
		 fl.Add("", 1234);

		 int count = 0;
		 int checksumTotal = 0;
		 fl.ForEach([&](const std::string& filename, int checksum)
			{
				++count;
				checksumTotal += checksum;
			});
		 ASSERT_EQ(count, 4);
		 ASSERT_EQ(checksumTotal, 11602);
	 }

	 TEST(FileListUnitTests, FileListForEachEmpty)
	 {
			CFileList fl;

			int count = 0;
			fl.ForEach([&](const std::string& filename, int checksum)
			{
				++count;
			});
			ASSERT_EQ(count, 0);
	 }
}
