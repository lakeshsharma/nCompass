#if 000//TODO-PORT

#include "stdafx.h"
#include "DisplayManager.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(DisplayManagerUnitTests)
	{
		TEST_METHOD(DisplayManager_IsServiceWindowInDisplayUnitTest)
		{
			CDisplayManager::Instance().m_serviceWindowState = SWS_HIDE;
			Assert::IsFalse(CDisplayManager::Instance().IsServiceWindowInDisplay());
			CDisplayManager::Instance().m_serviceWindowState = SWS_MEDIA_WINDOW;
			Assert::IsTrue(CDisplayManager::Instance().IsServiceWindowInDisplay());
			CDisplayManager::Instance().m_serviceWindowState = SWS_SHOW;
			Assert::IsTrue(CDisplayManager::Instance().IsServiceWindowInDisplay());
		}
	};
}

#endif //TODO-PORT
