#if 000//TODO-PORT

#include "stdafx.h"
#include "ProcessDump/MemoryThresholds.h"
#include "DirectoryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(MemoryThresholdUnitTests)
	{
		TEST_METHOD(MemoryThresholdInterpretNameValue)
		{
			// Init CDirectoryManager, prevent full setup
			CDirectoryManager::MockInstance(NULL);

			CMemoryThresholds mt(GetTickCount());
			mt.Start();

			// Good name/values
			Assert::IsTrue(mt.InterpretNameValue(L"Warning_Threshold", L"12345"));
			Assert::IsTrue(mt.InterpretNameValue(L"Reboot_Threshold", L"12345"));

			// Bad name
			Assert::IsFalse(mt.InterpretNameValue(L"RReboot_Threshold", L"12345"));


			// Good name/bad values
			Assert::IsFalse(mt.InterpretNameValue(L"Warning_Threshold", L"ONETWO"));
			Assert::IsFalse(mt.InterpretNameValue(L"Reboot_Threshold", L"ONETWO"));
			Assert::IsFalse(mt.InterpretNameValue(L"Warning_Threshold", L"ON2345"));
			Assert::IsFalse(mt.InterpretNameValue(L"Reboot_Threshold", L"ONE33333"));
		}
	};
}

#endif //TODO-PORT