#if 000//TODO-PORT
#include "stdafx.h"
#include "ProcessDump/ProcessHandler.h"
#include "TestUtil/AssertMsg.h"
#include "TestUtil/TestUtil.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	// This must remain POD
	struct ProcessInfo : public PROCESS_INFORMATION
	{
		ProcessInfo()
		{
			ZeroMemory(this, sizeof(PROCESS_INFORMATION));
		}

		~ProcessInfo()
		{
			CloseHandle(hProcess);
			CloseHandle(hThread);
		}
	};

	static unique_ptr<CMutex> nativeUnitTestsMutex;
	static unique_ptr<ProcessInfo> processInfo;

	TEST_CLASS(ProcessHandlerProcessTests)
	{
		TEST_METHOD_INITIALIZE(ProcessHandlerProcessTests_initialize)
		{
			nativeUnitTestsMutex.reset(new CMutex(TRUE, L"Global\\SentinelNativeUnitTests"));
		}

		TEST_METHOD_CLEANUP(ProcessHandlerProcessTests_cleanup)
		{
			nativeUnitTestsMutex.reset();
			if ((bool) processInfo && processInfo->hProcess)
			{
				if (WaitForSingleObject(processInfo->hProcess, 100) == WAIT_TIMEOUT)
				{
				}
			}
			processInfo.reset();
		}

		static void StartProcessHandlerTestApp()
		{
			CString testAppPathName(TestUtil::GetNativeUnitTestPath() + L"ProcessHandlerTestApp.exe");

			STARTUPINFO startupInfo;
			ZeroMemory(&startupInfo, sizeof(startupInfo));
			startupInfo.cb = sizeof(startupInfo);
			startupInfo.wShowWindow = SW_HIDE;

			BOOL result = CreateProcess(testAppPathName, nullptr, nullptr, nullptr, FALSE, HIGH_PRIORITY_CLASS, nullptr, nullptr, &startupInfo, processInfo.get());
			Assert::IsTrue(result, AssertMsg::FormatOnly(L"CreateProcess (%u): " + testAppPathName, GetLastError()));
		}

		TEST_METHOD(ProcessHandlerProcessTests_WaitForProcess)
		{
			processInfo.reset(new ProcessInfo);
			StartProcessHandlerTestApp();
			Sleep(15);

			CProcessHandler processHandler;
			bool result = processHandler.WaitForProcess(L"ProcessHandlerTestApp.exe", 2);
			Assert::IsTrue(result, L"processHandler.WaitForProcess");
		}
	};
}
#endif //TODO-PORT