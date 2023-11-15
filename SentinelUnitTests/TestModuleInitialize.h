#pragma once
#include "gtest/gtest.h"

namespace SentinelNativeUnitTests
{
	class CTestModuleEnvironment : public ::testing::Environment
	{
	public:
		virtual ~CTestModuleEnvironment();
		virtual void SetUp();
		virtual void TearDown();

	private:
		static int userDllInitRefCount;
	};
}
