
#include "stdafx.h"
#include "CheckSum.h"
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
		TEST(CheckSumUnitTest, CheckSumSimpleByteArrayUnitTest)
		{
			const int ARRAYSIZE = 1000;
			CCheckSum cs(true);
			byte bytes[ARRAYSIZE];

			memset(bytes, 1, ARRAYSIZE);
			cs.Add(bytes, ARRAYSIZE);

			ASSERT_TRUE(cs.Get() == ARRAYSIZE);


		}

		TEST(CheckSumUnitTest, CheckSumSimpleStringUnitTest)
		{
			CCheckSum cs(true);
			std::string str =  "12345"; // should add 49..53 which totals 255

			cs.Add(str);
			ASSERT_TRUE(cs.Get() == 255);
		}

}

