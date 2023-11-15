
#include "stdafx.h"
#include "UI/AudioInfo.h"


#include "gtest/gtest.h"


const std::string audioFileName = "click.wav";

namespace SentinelNativeUnitTests
{
		TEST(AudioInfoUnitTests, AudioInfoSetGetUnitTest)
		{
			
			CAudioInfo *info = new CAudioInfo();
			info->SetAudioFilename(audioFileName);
			ASSERT_TRUE(info->GetAudioFilename() == audioFileName);

			info->SetAudioFilename("");
			ASSERT_TRUE(info->GetAudioFilename() == "");

			delete info;
		}

		TEST(AudioInfoUnitTests, AudioInfoCopyConstructorUnitTest)
		{
			CAudioInfo *info = new CAudioInfo();
			info->SetAudioFilename(audioFileName);
			CAudioInfo *copy = new CAudioInfo(*info);

			ASSERT_TRUE(copy->GetAudioFilename() == audioFileName);

			CAudioInfo copy2 = *copy;
			ASSERT_TRUE(copy2.GetAudioFilename() == audioFileName);

			delete info;
			delete copy;
		}
}


