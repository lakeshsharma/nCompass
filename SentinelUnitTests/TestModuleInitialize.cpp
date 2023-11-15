#include "stdafx.h"
#include "TestModuleInitialize.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	int CTestModuleEnvironment::userDllInitRefCount = 0;

	CTestModuleEnvironment::~CTestModuleEnvironment()
	{
	}

	void CTestModuleEnvironment::SetUp()
	{
		if (0 == userDllInitRefCount)
		{
			if (S7LITE_DLL_Init() == S7DLL_STATUS_OK)
			{
				++userDllInitRefCount;
			}
		}
		else
		{
			++userDllInitRefCount;
		}
	}

	void CTestModuleEnvironment::TearDown()
	{
		if (--userDllInitRefCount == 0)
		{
			S7LITE_DLL_DeInit();
		}
	}
}
