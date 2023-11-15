#include "stdafx.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(TestUtilTests)
	{
		TEST_METHOD(TestUtilTests_IsZeroes_true)
		{
			const int buffer[3] = { 0, 0, 0 };
			bool result = TestUtil::IsZeroes(buffer, sizeof(buffer));
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestUtilTests_IsZeroes_false)
		{
			const int buffer[3] = { 0, 0, 1 };
			bool result = TestUtil::IsZeroes(buffer, sizeof(buffer));
			Assert::IsFalse(result);
		}

		TEST_METHOD(TestUtilTests_IsZeroes_template_true)
		{
			const int buffer[3] = { 0, 0, 0 };
			bool result = TestUtil::IsZeroes(buffer);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestUtilTests_IsZeroes_template_false)
		{
			const int buffer[3] = { 0, 0, 1 };
			bool result = TestUtil::IsZeroes(buffer);
			Assert::IsFalse(result);
		}

		TEST_METHOD(TestUtilTests_AreBitEqual_equal)
		{
			WIN32_FIND_DATA find1;
			memset(&find1, 0, sizeof(find1));

			WIN32_FIND_DATA find2;
			memset(&find2, 0, sizeof(find2));

			bool result = TestUtil::AreBitEqual(find1, find2);
			Assert::IsTrue(result);
		}

		TEST_METHOD(TestUtilTests_AreBitEqual_notequal)
		{
			WIN32_FIND_DATA find1;
			memset(&find1, 0, sizeof(find1));

			WIN32_FIND_DATA find2;
			memset(&find2, 0, sizeof(find2));
			find2.nFileSizeLow = 1;

			bool result = TestUtil::AreBitEqual(find1, find2);
			Assert::IsFalse(result);
		}
	};
}
