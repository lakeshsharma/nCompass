#if 000//TODO-PORT
#include "stdafx.h"
#include "s5dll_api.h"
#include "..\VirtualLiteDLL\S5_DLLNamedPipe.h"
#include "TestUtil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// The code inside all test methods except TEST_CLASS_INITIALIZE(_Initialize)
// must be wrapped in:
// if (m_virtualVersion == VERSION_ONE)
// {
//
// }
//

#define VERSION_MASK	0xffff0000U
#define VERSION_ONE		0x00010000U

/*
BIG NOTE:

Some of the tests in this file test named pipes to the VirtualLiteDll.
Due to how the named pipes are implemented in that DLL, if the client 
pipe handles are closed without a "Quit:" message being sent and flushed,
the pipe handles will remain busy and only one of the named pipe tests
will pass. The application works in production because the client isn't
disconnecting and reconnecting throughout the life of nCompass. This is
why there are pairs of OpenFrom/ToServerNamedPipe tests.

However, by having this unit test, if the named pipe code is changed, the
new algorithm can be tested.
*/

namespace SentinelNativeUnitTests
{
	#define S5DLL_VIRTUAL_LITE_VERSION 0x0001f7faU // this is hard coded in userdll_Lite.cpp
	TEST_CLASS(VirtualLiteDllTests)
	{
	private:
		static const DWORD m_fileNotFoundSleepTicks = 50;

		static UINT m_virtualVersion;
		static HANDLE m_hToServerPipe;
		static HANDLE m_hFromServerPipe;

	public:
		TEST_CLASS_INITIALIZE(_Initialize)
		{
			// The VirtualLite DLL is globally initialized in TestUtil.cpp:
			// TEST_MODULE_INITIALIZE(UserDllInitForTesting)

			m_virtualVersion = 0;
			m_hToServerPipe = INVALID_HANDLE_VALUE;
			m_hFromServerPipe = INVALID_HANDLE_VALUE;

			UINT version = 0;
			S5_Result result = S5_DLL_GetDLLVersion(version);
			if (result == S5DLL_STATUS_OK && version == S5DLL_VIRTUAL_LITE_VERSION)
			{
				m_virtualVersion = version & VERSION_MASK;
			}
		}

		TEST_CLASS_CLEANUP(_Cleanup)
		{
			// The VirtualLite DLL is globally deinitialized in TestUtil.cpp:
			// TEST_MODULE_CLEANUP(UserDllDeinitForTesting). This is here for
			// debugging reasons.

			ClosePipes(m_hToServerPipe, m_hFromServerPipe);
			m_hToServerPipe = INVALID_HANDLE_VALUE;
			m_hFromServerPipe = INVALID_HANDLE_VALUE;

			if (m_virtualVersion == VERSION_ONE)
			{

			}
		}

		TEST_METHOD_INITIALIZE(MethodInit)
		{
			ClosePipes(m_hToServerPipe, m_hFromServerPipe);
			m_hToServerPipe = INVALID_HANDLE_VALUE;
			m_hFromServerPipe = INVALID_HANDLE_VALUE;
		}

		TEST_METHOD_CLEANUP(MethodCleanup)
		{
			ClosePipes(m_hToServerPipe, m_hFromServerPipe);
			m_hToServerPipe = INVALID_HANDLE_VALUE;
			m_hFromServerPipe = INVALID_HANDLE_VALUE;
		}

		// This test is redundent but is included for completeness
		TEST_METHOD(GetDLLVersion)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				UINT version = 0;
				S5_Result result = S5_DLL_GetDLLVersion(version);

				Assert::AreEqual((int)S5DLL_STATUS_OK, (int)result, L"result");
				Assert::AreEqual(S5DLL_VIRTUAL_LITE_VERSION, version, L"version");
			}
		}

		static HANDLE _OpenToServerNamedPipe()
		{
			wchar_t pipeName[MAX_PATH];
			_snwprintf_s(pipeName, _countof(pipeName), _TRUNCATE, L"\\\\.\\pipe\\%s", S5_DLLReadNamedPipeName);

			HANDLE hPipe = INVALID_HANDLE_VALUE;
			DWORD startTick = GetTickCount();
			int tries(0);
			LPCWSTR lastDesc(L"N/A");
			DWORD lastError(ERROR_SUCCESS);

			do
			{
				++tries;
				hPipe = CreateFile(pipeName, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
				if (hPipe != INVALID_HANDLE_VALUE)
				{
					DWORD mode = PIPE_READMODE_MESSAGE; 
					if (!SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr))
					{
						DWORD pipeModeLastError = GetLastError();
						BOOL closeResult = CloseHandle(hPipe);
						Assert::AreNotEqual(FALSE, closeResult,
							AssertMsg::Format(__LINE__, nullptr, L"TO: CloseHandle closeResult lastError=%u pipeModeLastError=%u",
							(unsigned)GetLastError(), (unsigned)pipeModeLastError));
						hPipe = INVALID_HANDLE_VALUE;
						Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"TO: SetNamedPipeHandleState pipeModeLastError=%u",
							(unsigned)pipeModeLastError));
					}
					break;
				}

				lastDesc = L"CreateFile";
				lastError = GetLastError();
				if (lastError != ERROR_PIPE_BUSY && lastError != ERROR_FILE_NOT_FOUND)
				{
					Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"TO: %s lastError=%u",
						static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
					break;
				}

				DWORD start = GetTickCount();
				if (WaitNamedPipe(pipeName, NMPWAIT_USE_DEFAULT_WAIT) == 0)
				{
					lastDesc = L"WaitNamedPipe";
					lastError = GetLastError();
					if (ERROR_FILE_NOT_FOUND == lastError)
					{
						Sleep(m_fileNotFoundSleepTicks);
					}
					else if (lastError != ERROR_SEM_TIMEOUT)
					{
						Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"TO: %s lastError=%u",
							static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
						break;
					}
				}

			} while (GetTickCount() - startTick < 1000 || tries < 20);

			if (INVALID_HANDLE_VALUE == hPipe)
			{
				DWORD ticks(GetTickCount() - startTick);
				Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"TO: hPipe Exit tries=%d ticks=%u lastDesc=%s lastError=%u",
					(int)tries, (unsigned)ticks, static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
			}
			return hPipe;
		}

		static HANDLE _OpenFromServerNamedPipe()
		{
			wchar_t pipeName[MAX_PATH];
			_snwprintf_s(pipeName, _countof(pipeName), _TRUNCATE, L"\\\\.\\pipe\\%s", S5_DLLWriteNamedPipeName);

			HANDLE hPipe = INVALID_HANDLE_VALUE;
			DWORD startTick = GetTickCount();
			int tries(0);
			LPCWSTR lastDesc(L"N/A");
			DWORD lastError(ERROR_SUCCESS);

			do
			{
				++tries;
				hPipe = CreateFile(pipeName, GENERIC_READ | FILE_WRITE_ATTRIBUTES, 0, nullptr, OPEN_EXISTING, 0, nullptr);
				if (hPipe != INVALID_HANDLE_VALUE)
				{
					DWORD mode = PIPE_READMODE_MESSAGE; 
					if (!SetNamedPipeHandleState(hPipe, &mode, nullptr, nullptr))
					{
						DWORD pipeModeLastError = GetLastError();
						BOOL closeResult = CloseHandle(hPipe);
						Assert::AreNotEqual(FALSE, closeResult,
							AssertMsg::Format(__LINE__, nullptr, L"FROM: CloseHandle closeResult lastError=%u pipeModeLastError=%u",
								(unsigned)GetLastError(), (unsigned)pipeModeLastError));
						hPipe = INVALID_HANDLE_VALUE;
						Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"FROM: SetNamedPipeHandleState pipeModeLastError=%u",
							(unsigned)pipeModeLastError));
					}
					break;
				}

				lastDesc = L"CreateFile";
				lastError = GetLastError();
				if (lastError != ERROR_PIPE_BUSY && lastError != ERROR_FILE_NOT_FOUND)
				{
					Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"FROM: %s lastError=%u",
						static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
					break;
				}

				if (!WaitNamedPipe(pipeName, NMPWAIT_USE_DEFAULT_WAIT))
				{
					lastDesc = L"WaitNamedPipe";
					lastError = GetLastError();
					if (ERROR_FILE_NOT_FOUND == lastError)
					{
						Sleep(m_fileNotFoundSleepTicks);
					}
					else if (lastError != ERROR_SEM_TIMEOUT)
					{
						Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"FROM: %s lastError=%u",
							static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
						break;
					}
				}

			} while (GetTickCount() - startTick < 1000 || tries < 20);

			if (INVALID_HANDLE_VALUE == hPipe)
			{
				DWORD ticks(GetTickCount() - startTick);
				Assert::Fail(AssertMsg::Format(__LINE__, nullptr, L"FROM: hPipe Exit tries=%d ticks=%u lastDesc=%s lastError=%u",
					(int)tries, (unsigned)ticks, static_cast<LPCWSTR>(lastDesc), (unsigned)lastError));
			}
			return hPipe;
		}

		/*
		ConnectNamedPipe on the server.
		CreateFile on the client
		communication
		CloseHandle on the client
		DisonnectNamedPipe on the server
		*/

		static void ClosePipes(HANDLE hToServerPipe, HANDLE hFromServerPipe)
		{
			if (hToServerPipe != INVALID_HANDLE_VALUE)
			{
				DWORD written = 0;
				BOOL writeResult = WriteFile(hToServerPipe, LITE_DLL_CMD_QUIT, LITE_DLL_CMD_QUIT_LEN, &written, nullptr);
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"CLOSE: WriteFile writeResult lastError=%u", (unsigned)GetLastError()));

				BOOL flushResult = FlushFileBuffers(hToServerPipe);
				Assert::AreNotEqual(FALSE,
					flushResult,
					AssertMsg::Format(__LINE__, nullptr, L"CLOSE: FlushFileBuffers flushResult lastError=%u", (unsigned)GetLastError()));

				BOOL toCloseResult = CloseHandle(hToServerPipe);
				Assert::AreNotEqual(FALSE,
					toCloseResult,
					AssertMsg::Format(__LINE__, nullptr, L"CLOSE: CloseHandle toCloseResult lastError=%u", (unsigned)GetLastError()));
			}

			if (hFromServerPipe != INVALID_HANDLE_VALUE)
			{
				BOOL fromCloseResult = CloseHandle(hFromServerPipe);
				Assert::AreNotEqual(FALSE,
					fromCloseResult,
					AssertMsg::Format(__LINE__, nullptr, L"CLOSE: CloseHandle fromCloseResult lastError=%u", (unsigned)GetLastError()));
			}
		}

		TEST_METHOD(OpenToServerNamedPipe)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");
			}
		}

		TEST_METHOD(OpenToServerNamedPipe2)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");
			}
		}

		TEST_METHOD(OpenFromServerNamedPipe)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");

				m_hFromServerPipe = _OpenFromServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hFromServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hFromServerPipe");
			}
		}

		TEST_METHOD(OpenFromServerNamedPipe2)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");

				m_hFromServerPipe = _OpenFromServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hFromServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hFromServerPipe");
			}
		}

		TEST_METHOD(RoundTripNamedPipe)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");

				m_hFromServerPipe = _OpenFromServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hFromServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hFromServerPipe");

				const char* pStringToWrite = "Test Message";

				DWORD written = 0;
				BOOL writeResult = WriteFile(m_hToServerPipe, pStringToWrite, strlen(pStringToWrite) + 1, &written, nullptr);
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result lastError=%u", (unsigned)GetLastError()));

				char buffer[256];

				DWORD read = 0;
				BOOL readResult = ReadFile(m_hFromServerPipe, buffer, sizeof(buffer), &read, nullptr);
				Assert::AreNotEqual(FALSE,
					readResult,
					AssertMsg::Format(__LINE__, nullptr, L"ReadFile result lastError=%u", (unsigned)GetLastError()));

				const char* pExpectedString = "Log:Unsupported command <Test Message>\r\n";

				Assert::AreEqual(pExpectedString, buffer, false, L"ReadFile buffer");
			}
		}

		TEST_METHOD(GetLoggingState)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");

				m_hFromServerPipe = _OpenFromServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hFromServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hFromServerPipe");

				DWORD written = 0;
				BOOL writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_IS_FILE_LOGGING, LITE_DLL_CMD_IS_FILE_LOGGING_LEN, &written, nullptr);
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result lastError=%u", (unsigned)GetLastError()));

				char buffer[256];

				DWORD read = 0;
				BOOL readResult = ReadFile(m_hFromServerPipe, buffer, sizeof(buffer), &read, nullptr);
				Assert::AreNotEqual(FALSE,
					readResult,
					AssertMsg::Format(__LINE__, nullptr, L"ReadFile result lastError=%u", (unsigned)GetLastError()));

				const char* pExpectedEnabled = LITE_DLL_CMD_IS_FILE_LOGGING "enabled\n";
				const char* pExpectedDisabled = LITE_DLL_CMD_IS_FILE_LOGGING "disabled\n";

				if (strcmp(pExpectedEnabled, buffer) != 0 &&
					strcmp(pExpectedDisabled, buffer) != 0)
				{
					Assert::Fail(AssertMsg::FormatOnly(L"ReadFile buffer returned %S", buffer));
				}
			}
		}

		TEST_METHOD(SetLoggingState)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				m_hToServerPipe = _OpenToServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hToServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hToServerPipe");

				m_hFromServerPipe = _OpenFromServerNamedPipe();
				Assert::AreNotEqual((LONG_PTR) m_hFromServerPipe, (LONG_PTR) INVALID_HANDLE_VALUE, L"m_hFromServerPipe");

				// Determine if logging is already enabled

				DWORD written = 0;
				BOOL writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_IS_FILE_LOGGING, LITE_DLL_CMD_IS_FILE_LOGGING_LEN, &written, nullptr);
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result lastError=%u", (unsigned)GetLastError()));

				char buffer[256];

				DWORD read = 0;
				BOOL readResult = ReadFile(m_hFromServerPipe, buffer, sizeof(buffer), &read, nullptr);
				Assert::AreNotEqual(FALSE,
					readResult,
					AssertMsg::Format(__LINE__, nullptr, L"ReadFile result lastError=%u", (unsigned)GetLastError()));

				const char* pExpectedDisabled = LITE_DLL_CMD_IS_FILE_LOGGING "disabled\n";
				const char* pExpectedEnabled = LITE_DLL_CMD_IS_FILE_LOGGING "enabled\n";

				bool loggingStartedEnabled = strcmp(pExpectedEnabled, buffer) == 0;

				if (loggingStartedEnabled)
				{
					writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_DISABLE_FILE_LOGGING, LITE_DLL_CMD_DISABLE_FILE_LOGGING_LEN, &written, nullptr);
				}
				else
				{
					writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_ENABLE_FILE_LOGGING, LITE_DLL_CMD_ENABLE_FILE_LOGGING_LEN, &written, nullptr);
				}
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result (1) lastError=%u", (unsigned)GetLastError()));

				writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_IS_FILE_LOGGING, LITE_DLL_CMD_IS_FILE_LOGGING_LEN, &written, nullptr);
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result (2) lastError=%u", (unsigned)GetLastError()));

				readResult = ReadFile(m_hFromServerPipe, buffer, sizeof(buffer), &read, nullptr);
				Assert::AreNotEqual(FALSE,
					readResult,
					AssertMsg::Format(__LINE__, nullptr, L"ReadFile result (2) lastError=%u", (unsigned)GetLastError()));

				if (loggingStartedEnabled)
				{
					if (strcmp(pExpectedDisabled, buffer) != 0)
					{
						Assert::Fail(AssertMsg::FormatOnly(L"ReadFile buffer returned %S", buffer));
					}
				}
				else
				{
					if (strcmp(pExpectedEnabled, buffer) != 0)
					{
						Assert::Fail(AssertMsg::FormatOnly(L"ReadFile buffer returned %S", buffer));
					}
				}

				if (loggingStartedEnabled)
				{
					writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_ENABLE_FILE_LOGGING, LITE_DLL_CMD_ENABLE_FILE_LOGGING_LEN, &written, nullptr);
				}
				else
				{
					writeResult = WriteFile(m_hToServerPipe, LITE_DLL_CMD_DISABLE_FILE_LOGGING, LITE_DLL_CMD_DISABLE_FILE_LOGGING_LEN, &written, nullptr);
				}
				Assert::AreNotEqual(FALSE,
					writeResult,
					AssertMsg::Format(__LINE__, nullptr, L"WriteFile result (3) lastError=%u", (unsigned)GetLastError()));
			}
		}

		TEST_METHOD(SetVOFormat)
		{
			if (m_virtualVersion == VERSION_ONE)
			{
				eVO_FORMAT orgFormat;
				S5_Result result = S5_VEB_GetVOFormat(orgFormat);
				Assert::AreEqual((int)S5DLL_STATUS_OK, (int)result, L"S5_VEB_GetVOFormat");

				eVO_FORMAT newFormat = orgFormat == EE_VO_FORMAT_BOTH ? EE_VO_FORMAT_AUTO : EE_VO_FORMAT_BOTH;

				result = S5_VEB_SetVOFormat(newFormat);
				Assert::AreEqual((int)S5DLL_STATUS_OK, (int)result, L"S5_VEB_SetVOFormat");

				eVO_FORMAT format;
				result = S5_VEB_GetVOFormat(format);
				Assert::AreEqual((int)S5DLL_STATUS_OK, (int)result, L"S5_VEB_GetVOFormat");
				Assert::AreEqual((int)newFormat, (int)format, L"S5_VEB_GetVOFormat");

				result = S5_VEB_SetVOFormat(orgFormat);
				Assert::AreEqual((int)S5DLL_STATUS_OK, (int)result, L"S5_VEB_SetVOFormat");
			}
		}
	};

	UINT VirtualLiteDllTests::m_virtualVersion = 0;
	HANDLE VirtualLiteDllTests::m_hToServerPipe(INVALID_HANDLE_VALUE);
	HANDLE VirtualLiteDllTests::m_hFromServerPipe(INVALID_HANDLE_VALUE);
}
#endif //TODO-PORT