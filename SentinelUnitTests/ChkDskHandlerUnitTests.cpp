
#include "stdafx.h"

#include "ChkDskHandler.h"
#include "UnitTestingUtils.h"
#include "DirectoryManager.h"
#include "Util/FileSys.h"


#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
		//CDirectoryManager::MockInstance(NULL);
		
		TEST(ChkDskHandlerUnitTests, ChkDskHandlerIsChkdskRequiredTest)
		{
			string testDir;

			CUnitTestingUtils::GetTestDirAndDrive("Test", testDir);

			string testSubDir = testDir + "/Sub";

			const unsigned DIRCOUNT = 1;
			string directories[DIRCOUNT];

			directories[0] = testDir;

			const unsigned SMDDIRCOUNT = 1;
			string smdDirectories[SMDDIRCOUNT];

			smdDirectories[0] = testDir;

			ChkDskHandler chkdsk(directories, DIRCOUNT);

			FileSys::CreateDirectory(testDir);
			FileSys::CreateDirectory(testSubDir);
			
  			bool requiredWhenNotEmpty = chkdsk.IsChkdskRequired();

			FileSys::RemoveDirectory(testSubDir);
			
			bool notRequiredWhenEmtpy = chkdsk.IsChkdskRequired();

			FileSys::RemoveDirectory(testDir);
			
			bool notRequiredWhenNotExists = chkdsk.IsChkdskRequired();

			ASSERT_TRUE(requiredWhenNotEmpty) << "chkdsk not found required when test directory not empty";
			ASSERT_FALSE(notRequiredWhenEmtpy) << "chkdsk found required when test directory is empty";
			ASSERT_FALSE(notRequiredWhenNotExists) << "chkdsk found required when test directory does not exists";
		}
		
		       
}

