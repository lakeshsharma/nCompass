
#include "stdafx.h"
#include "Diagnostics.h"

// Sentinel includes.
#include "ProcessDump/WebDiagnostics.h"
#include "Util/ByteArrayEx.h"
#include "UnitTestingUtils.h"
#include "Utilities.h"

#include "gtest/gtest.h"


// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, NULL, __TFUNCTION__, __LINE__, NULL);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a class initialization method.
#define AssertResult3i(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, NULL, __TFUNCTION__, __LINE__, NULL);
#define AssertResult2i(result, expected) AssertResult3i(result, expected, _T(#result))

using namespace std;

namespace SentinelUnitTests
{
	static const int WAITFORLOOP = 10;
	static const int SAFETY = 20000;
	
	struct ThreadData
	{
		enum Sentinel
		{
			RUNNING = 0,
			DONE
		};
	
		ThreadData() : stop(false), sentinel(RUNNING) {}
	
		bool stop;
		Sentinel sentinel;
	};

	class CThread
	{
	private:
		std::unique_ptr<std::thread> m_pThread;
		ThreadData *m_pData;
		std::string m_threadName;

	public:
		CThread()
			:  m_pThread(nullptr)
			,  m_pData(nullptr)
			,  m_threadName("Nested")
		{	
		}

		~CThread()
		{
			StopThread();
		}

		void StartThread()
		{
			m_pThread.reset(new std::thread(_thread, this));
		}	
		
		void StopThread()
		{
			// Tell thread to stop

			if (m_pThread.get() != nullptr)
			{
				m_pThread->join();
				delete m_pThread.release();

			}
            else
            {
                return;
            }

		}

		static UINT AFX_CDECL _thread(CThread* pClass)
		{
			if (pClass)
			{
				pClass->NestedThread();
			}
			return 0;
		}

		UINT AFX_CDECL TestThread()
		{
			m_pData = new ThreadData();
		
			while (!m_pData->stop)
			{
				sleep(WAITFORLOOP);
			}
		
			m_pData->sentinel = ThreadData::DONE;
		
			return 0;
		}
		
		UINT AFX_CDECL NestedThread()
		{
			return TestThread();
		}

		std::thread::id GetThreadId()
		{
			return m_pThread->get_id();
		}

		std::string GetThreadName()
		{
			return m_threadName;
		}
	};
	
	class DiagnosticsUnitTests : public ::testing::Test
	{
	private:
		CThread m_thread;

	public:
		virtual void SetUp()
		{
			CDiagnostics::Instance().ClearReboot();
			CDiagnostics::CleanupInstance();
		}

		virtual void TearDown()
		{
		}

		void TestCallstackToBuffer(string name, char* buffer, size_t bufferSize)
		{
			DWORD lastError = ERROR_SUCCESS;
			size_t remaining = CDiagnostics::ReportThreadCallstackToBuffer(name, buffer, bufferSize);
			if (-1 == remaining)
			{
				string msg;
				m_thread.StopThread();
				FAIL() << "Error resulted getting callstack to buffer.";
			}
		}
		
		void GetCallstackFromFile(const string& testFile, string& written, string& code, string& address)
		{
		
			std::ifstream file;
			file.open(testFile, std::ifstream::in);
			
			if (!file.eof() && !file.fail())
			{
				string line;
				while (std::getline(file, line))
				{
					written = line;
					 std::size_t pos = line.find("ERROR: ", 7);
				  if (pos!=std::string::npos)
					{
						code = line.substr (pos);
					}
					else
					{
						pos = line.find("at ");
						if (pos!=std::string::npos)
						{
							address = line.substr (0, pos + 3);
						}
					}
				}
		
				file.close();
		
				// Make sure we got last line
				if (!line.empty())
				{
					written = line;
				}
			}
		}
		


	};

	
	TEST_F(DiagnosticsUnitTests, DiagnosticsSetGetRebootTiltCodeTest)
	{
		TiltCode expected = tltRebootAsap;
		TiltCode original = tltUnknown;
		TiltCode result = tltUnknown;
	
		// Get and set in different scopes so we read NVRAM\EEPROM and not local members
		{
			original = CDiagnostics::Instance().GetRebootTiltCode();
			CDiagnostics::Instance().ClearReboot();
			CDiagnostics::Instance().PersistReboot(expected, "TEST");
		}
	
		{
			result = CDiagnostics::Instance().GetRebootTiltCode();
		}
	
		// Restore code before we begin testing
		CDiagnostics::Instance().ClearReboot();
		CDiagnostics::Instance().PersistReboot(original, "TEST");
		TiltCode restored = CDiagnostics::Instance().GetRebootTiltCode();
		ASSERT_EQ(original , restored) << "Unable to restore Reboot Tilt Code back to original.";
	
		ASSERT_EQ(result , expected) << "Reboot Tilt code set did not return as expected.";
	}

	/// <summary>
	/// Code coverage and functional test for:
	/// 	CDiagnostics::GetFileUpdateCount()
	/// Expect that no threads change the value during the test.
	/// </summary>
	TEST_F(DiagnosticsUnitTests, DiagnosticsGetFileUpdateTickCount_NoChangeTest)
	{
		const DWORD SLEEP_ASYNC_EVENTS_SECONDS(1); // Allow asynchronous events to occur.
	
		// Perform operation(s) to be tested.
		DWORD fileUpdateCount0(CDiagnostics::GetFileUpdateCount());
		sleep(SLEEP_ASYNC_EVENTS_SECONDS);
		DWORD fileUpdateCount1(CDiagnostics::GetFileUpdateCount());
		sleep(SLEEP_ASYNC_EVENTS_SECONDS);
		DWORD fileUpdateCount2(CDiagnostics::GetFileUpdateCount());
	
		ASSERT_TRUE(fileUpdateCount0 == fileUpdateCount1) << "fileUpdateCount did change. 0 1";
		ASSERT_TRUE(fileUpdateCount0 == fileUpdateCount2) << "fileUpdateCount did change. 0 2";
	}
	
	TEST_F(DiagnosticsUnitTests, DiagnosticsGetFileUpdateTickCount_ReportThreadCallstackToFileTest)
	{
	
		string testFile =  "/tmp/notcrashed.txt";
		// Perform operation(s) to be tested.
		DWORD fileUpdateCount0(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportThreadCallstackToFile(testFile, "test", "", "");
		DWORD fileUpdateCount1(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportThreadCallstackToFile(testFile, "test", "", "");
		DWORD fileUpdateCount2(CDiagnostics::GetFileUpdateCount());
	
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount1) << "fileUpdateCount did not change. 0 1";
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount2) << "fileUpdateCount did not change. 0 2";
		ASSERT_TRUE(fileUpdateCount1 != fileUpdateCount2) << "fileUpdateCount did not change. 1 2";
	}

	TEST_F(DiagnosticsUnitTests, DiagnosticsGetFileUpdateTickCount_ReportRebootToFileTest)
	{
		string testFile =  "/tmp/notcrashed.txt";
		// Perform operation(s) to be tested.
		DWORD fileUpdateCount0(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportRebootToFile(testFile, tltUnknown, "");
		DWORD fileUpdateCount1(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportRebootToFile(testFile, tltUnknown, "");
		DWORD fileUpdateCount2(CDiagnostics::GetFileUpdateCount());
	
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount1) << "fileUpdateCount did not change. 0 1";
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount2) << "fileUpdateCount did not change. 0 2";
		ASSERT_TRUE(fileUpdateCount1 != fileUpdateCount2) << "fileUpdateCount did not change. 1 2";
	}
	
	TEST_F(DiagnosticsUnitTests, DiagnosticsGetFileUpdateTickCount_ReportPBTTicketDiagnosticToFileTest)
	{
		string testFile =  "/tmp/notcrashed.txt";
		// Perform operation(s) to be tested.
		DWORD fileUpdateCount0(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportPBTTicketDiagnosticToFile(testFile, "", "", tltUnknown);
		DWORD fileUpdateCount1(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportPBTTicketDiagnosticToFile(testFile, "", "", tltUnknown);
		DWORD fileUpdateCount2(CDiagnostics::GetFileUpdateCount());
	
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount1) << "fileUpdateCount did not change. 0 1";
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount2) << "fileUpdateCount did not change. 0 2";
		ASSERT_TRUE(fileUpdateCount1 != fileUpdateCount2) << "fileUpdateCount did not change. 1 2";
	}
	
	
	/// <summary>
	/// Functional test for:
	/// 	CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile();
	/// Test that the file update count is incremented.
	/// </summary>
	TEST_F(DiagnosticsUnitTests, DiagnosticsGetFileUpdateTickCount_ReportBadPollerProtocolSendPacketToTextFileTest)
	{
		// Data.
		LPCTSTR timeStamp(_T("10/15/2015 09:23:45 AM"));
		string testFile =  "/tmp/notcrashed.txt";
		
		// Perform operation(s) to be tested.
		DWORD fileUpdateCount0(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(testFile, timeStamp, tltUnknown, _T(""), nullptr, 0);
		DWORD fileUpdateCount1(CDiagnostics::GetFileUpdateCount());
		CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(testFile, timeStamp, tltUnknown, _T(""), nullptr, 0);
		DWORD fileUpdateCount2(CDiagnostics::GetFileUpdateCount());
	
		// Perform test(s).
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount1) << "fileUpdateCount did not change. 0 1";
		ASSERT_TRUE(fileUpdateCount0 != fileUpdateCount2) << "fileUpdateCount did not change. 0 2";
		ASSERT_TRUE(fileUpdateCount1 != fileUpdateCount2) << "fileUpdateCount did not change. 1 2";
	}
	
	void SetupTestFileName(string filename, string &directory, string &pathname)
	{
		directory = "/tmp";
		pathname = directory + _T('/') + filename;
	
	}
	
	/// <summary>
	/// Functional test for:
	/// 	CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile();
	/// Test file contents after writing to a file that is not present.
	/// </summary>
	TEST_F(DiagnosticsUnitTests, DiagnosticsReportBadPollerProtocolSendPacketToTextFileTest)
	{
		// Data.
		string filename(_T("test.txt"));
		LPCTSTR timeStamp(_T("10/15/2015 09:23:45 AM"));
		TiltCode tiltCode(tltPollerProtocolCheckPacket);
		LPCTSTR reason(_T("bad format"));
		string expected = FormatString("\r@@%s - Bad Poller Protocol packet created/sent. tiltCode=%u reason=%s\r",
			static_cast<LPCTSTR>(timeStamp),
			(unsigned)tiltCode,
			static_cast<LPCTSTR>(reason));
	
		// Setup.
		string directory;
		string pathname;
        SetupTestFileName(filename, directory, pathname);
	
		// Perform operation(s) to be tested.
		int bytesWritten = CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(pathname, timeStamp, tiltCode, reason, nullptr, 0);
	
		// Perform test(s).
		string written;
		CUnitTestingUtils::GetCStringFromTextFile(pathname.c_str(), written);
		ASSERT_TRUE(!CUtilities::DeleteFile(pathname.c_str())) << "Error file not cleaned up.";
	
		ASSERT_EQ(expected, written);
		ASSERT_EQ((int)expected.size(), bytesWritten-2) << "bytesWritten.";
	}
	
	/// <summary>
	/// Functional test for:
	/// 	CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile();
	/// Test file contents after writing to a file that is not present and then writing to the file again.
	/// </summary>
	TEST_F(DiagnosticsUnitTests, DiagnosticsReportBadPollerProtocolSendPacketToTextFileTest2)
	{
		// Data.
		string filename(_T("test.txt"));
		LPCTSTR timeStamp1(_T("10/15/2015 01:23:45 PM"));
		LPCTSTR timeStamp2(_T("10/15/2015 02:23:45 PM"));
		TiltCode tiltCode1(tltPollerProtocolCreatePacket);
		TiltCode tiltCode2(tltPollerProtocolCheckPacket);
		LPCTSTR reason1(_T("bad format"));
		LPCTSTR reason2(_T("bad meter value"));
  		string expected = FormatString(_T("\r@@%s - Bad Poller Protocol packet created/sent. tiltCode=%u reason=%s\r"),
			static_cast<LPCTSTR>(timeStamp1),
			(unsigned)tiltCode1,
			static_cast<LPCTSTR>(reason1));
		expected += FormatString(_T("\r@@%s - Bad Poller Protocol packet created/sent. tiltCode=%u reason=%s\r"),
			static_cast<LPCTSTR>(timeStamp2),
			(unsigned)tiltCode2,
			static_cast<LPCTSTR>(reason2));
	
		// Setup.
		string directory;
        string pathname;
        SetupTestFileName(filename, directory, pathname);
	
		// Perform operation(s) to be tested.
		int bytesWritten = CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(pathname, timeStamp1, tiltCode1, reason1, nullptr, 0);
		bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(pathname, timeStamp2, tiltCode2, reason2, nullptr, 0);
	
		// Perform test(s).
		string contents;
		CUnitTestingUtils::GetCStringFromTextFile(pathname.c_str(), contents);
		ASSERT_TRUE(!
					CUtilities::DeleteFile(pathname.c_str())) << "Error file not cleaned up.";
	
		ASSERT_EQ(expected, contents);
		ASSERT_EQ((int)expected.size(), bytesWritten-4) << "bytesWritten.";
	}
	
#if 0		
		void TestBuffer(const char* threadName, const char* buffer)
		{
			string prefix = FormatString("Callstack = [%s], ", buffer);
		
			string callstack = buffer;
		
			{
				string msg(prefix);
				msg += "not written to buffer.";
				Assert::IsTrue(lstrlen(threadName) + 2 < callstack.GetLength(), msg);
			}
		
			{
				string msg(prefix);
				msg.AppendFormat(L"not ended with '0', last char = %c.", callstack.GetAt(callstack.GetLength() - 3));
				Assert::IsTrue(L'0' == callstack.GetAt(callstack.GetLength() - 3), msg);
			}
		
			{
				string msg(prefix);
				msg.AppendFormat(L"not started with thread name, name = %s.", threadName);
				Assert::IsTrue(0 == callstack.Find(threadName), msg);


		}

		TEST_METHOD DiagnosticsReportThreadCallstackToBuffer1Test)
     	{
		CThread thread;
		thread.StartThread();
	
		char buffer[CALLSTACK_BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));
	
			size_t remaining = CDiagnostics::ReportThreadCallstackToBuffer(thread.GetThreadName(), buffer, _countof(buffer));
			if (-1 == remaining)
			{
				FAIL() << "ReportThreadCallstackToBuffer errored getting buffer.";
			}

	
		thread.StopThread();
	}



	public:
		


		TEST_METHOD(DiagnosticsReportThreadCallstackToBuffer1Test)
		{
			ThreadData threadData;

			CWinThread* thread = AfxBeginThread(NestedThread, &threadData);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			CUtilities::SuspendThread(threadId, NULL, false);

			const LPCWSTR THREADNAME = L"nested";
			wchar_t buffer[CALLSTACK_BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			CONTEXT context = { CONTEXT_CONTROL };
			if (::GetThreadContext(threadHandle, &context))
			{
				size_t remaining = CDiagnostics::ReportThreadCallstackToBuffer(THREADNAME, threadHandle, &context, buffer, _countof(buffer));
				if (-1 == remaining)
				{
					CString msg;
					msg.AppendFormat(L"Error resulted getting callstack to buffer, id = %d.", threadId);
					StopThread(threadData, threadHandle, 1);
					Assert::Fail(L"ReportThreadCallstackToBuffer errored getting buffer.");
				}
			}
			else
			{
				CString msg;
				msg.AppendFormat(L"Unable to get thread context, id = %d, error = %d.", threadId, GetLastError());
				StopThread(threadData, threadHandle, 1);
				Assert::Fail(msg);
			}

			CUtilities::ResumeThread(threadId, NULL, false);
			StopThread(threadData, threadHandle, 0);

			TestBuffer(THREADNAME, buffer);
		}

		TEST_METHOD(DiagnosticsReportThreadCallstackToBuffer2Test)
		{
			ThreadData threadData;

			CWinThread* thread = AfxBeginThread(NestedThread, &threadData);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			const LPCWSTR THREADNAME = L"nested";
			wchar_t buffer[CALLSTACK_BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			DWORD lastError = ERROR_SUCCESS;
			size_t remaining = CDiagnostics::ReportThreadCallstackToBuffer(THREADNAME, threadHandle, buffer, _countof(buffer), &lastError, false);
			if (-1 == remaining)
			{
				CString msg;
				msg.AppendFormat(L"Error resulted getting callstack to buffer, id = %d, error = %d.", threadId, lastError);
				StopThread(threadData, threadHandle, 1);
				Assert::Fail(L"ReportThreadCallstackToBuffer errored getting buffer.");
			}

			StopThread(threadData, threadHandle, 0);

			TestBuffer(THREADNAME, buffer);
		}

		void TestCallstackWritten(const wchar_t* buffer, int bytesWritten, const CString& written)
		{
			CString expected(buffer);

			// Remove trailing whitespace such as 'return's
			expected = expected.TrimRight();

			if (lstrlen(buffer) > bytesWritten || 0 != written.Compare(expected))
			{
				CString msg;
				msg.AppendFormat(L"Callstack not written to file, expected = %s, written = %s.", expected, written);
				Assert::Fail(msg);
			}
		}

		TEST_METHOD(DiagnosticsReportThreadCallstackToFileNoCrashTest)
		{
			ThreadData threadData;
			std::unique_ptr<std::thread> thread =  std::unique_ptr<std::thread>(new std::thread(NestedThread, &threadData));

			CWinThread* thread = AfxBeginThread(NestedThread, &threadData);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			const LPCWSTR THREADNAME = L"nested";
			wchar_t buffer[CALLSTACK_BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			TestCallstackToBuffer(THREADNAME, threadId, threadHandle, threadData, buffer, _countof(buffer));

			StopThread(threadData, threadHandle, 0);

			CString testDir;

			CUnitTestingUtils::GetTestDir(L"Test", testDir);

			CString testFile = testDir + L"\\notcrashed.txt";

			::CreateDirectory(testDir, NULL);

			int bytesWritten = CDiagnostics::ReportThreadCallstackToFile(testFile, L"test", 0, NULL, buffer);

			CString written;
			CString errorCode;
			CString address;

			GetCallstackFromFile(testFile, written, errorCode, address);

			bool testRemoved = true;
			if (!::DeleteFile(testFile))
			{
				testRemoved = (ERROR_FILE_NOT_FOUND == GetLastError());
			}

			::RemoveDirectory(testDir);

			TestCallstackWritten(buffer, bytesWritten, written);
			Assert::IsTrue(errorCode.IsEmpty(), CString(L"Error code written when not passed, error = ") + errorCode);
			Assert::IsTrue(address.IsEmpty(), CString(L"Address written when not passed, address = ") + address);
			Assert::IsTrue(testRemoved, L"Error file not cleaned up");
		}

		TEST_METHOD(DiagnosticsReportThreadCallstackToFileWithErrorTest)
		{
			ThreadData threadData;

			CWinThread* thread = AfxBeginThread(NestedThread, &threadData);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			const LPCWSTR THREADNAME = L"nested";
			wchar_t buffer[CALLSTACK_BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			TestCallstackToBuffer(THREADNAME, threadId, threadHandle, threadData, buffer, _countof(buffer));

			StopThread(threadData, threadHandle, 0);

			CString testDir;

			CUnitTestingUtils::GetTestDir(L"Test", testDir);

			CString testFile = testDir + L"\\notcrashed.txt";

			::CreateDirectory(testDir, NULL);

			int bytesWritten = CDiagnostics::ReportThreadCallstackToFile(testFile, L"test", 0x1, NULL, buffer);

			CString written;
			CString errorCode;
			CString address;

			GetCallstackFromFile(testFile, written, errorCode, address);

			bool testRemoved = true;
			if (!::DeleteFile(testFile))
			{
				testRemoved = (ERROR_FILE_NOT_FOUND == GetLastError());
			}

			::RemoveDirectory(testDir);

			TestCallstackWritten(buffer, bytesWritten, written);

			Assert::IsFalse(errorCode.IsEmpty(), L"No error code written when passed");
			Assert::IsTrue(0 == errorCode.Compare(L"1"), CString(L"Wrong error code written, error = ") + errorCode);
			Assert::IsTrue(address.IsEmpty(), CString(L"Address written when not passed, address = ") + address);
			Assert::IsTrue(testRemoved, L"Error file not cleaned up");
		}

		TEST_METHOD(DiagnosticsReportThreadCallstackToFileWithAddressTest)
		{
			ThreadData threadData;

			CWinThread* thread = AfxBeginThread(NestedThread, &threadData);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			const LPCWSTR THREADNAME = L"nested";
			wchar_t buffer[CALLSTACK_BUFFER_SIZE];
			memset(buffer, 0, sizeof(buffer));

			TestCallstackToBuffer(THREADNAME, threadId, threadHandle, threadData, buffer, _countof(buffer));

			StopThread(threadData, threadHandle, 0);

			CString testDir;

			CUnitTestingUtils::GetTestDir(L"Test", testDir);

			CString testFile = testDir + L"\\notcrashed.txt";

			::CreateDirectory(testDir, NULL);

			int bytesWritten = CDiagnostics::ReportThreadCallstackToFile(testFile, L"test", 0, (PVOID)0x1, buffer);

			CString written;
			CString errorCode;
			CString address;

			GetCallstackFromFile(testFile, written, errorCode, address);

			bool testRemoved = true;
			if (!::DeleteFile(testFile))
			{
				testRemoved = (ERROR_FILE_NOT_FOUND == GetLastError());
			}

			::RemoveDirectory(testDir);

			TestCallstackWritten(buffer, bytesWritten, written);

			Assert::IsFalse(address.IsEmpty(), L"No error code written when passed.");
			Assert::IsTrue(0 == address.Compare(L"1"), CString(L"Wrong address written, address = ") + address);
			Assert::IsTrue(errorCode.IsEmpty(), CString(L"Error code written when not passed, error = ") + errorCode);
			Assert::IsTrue(testRemoved, L"Error file not cleaned up.");
		}

		/// <summary>
		/// Functional test for:
		///     CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile();
		/// Test file contents after writing to a file that is not present.
		/// </summary>
		TEST_METHOD(DiagnosticsReportBadPollerProtocolSendPacketToLogFileTest)
		{
			// Data.
			const LPCTSTR filename(_T("test.sdf"));
			const BYTE message1[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10 };
			const BYTE code(DATA_WRITE | DATA_ETHERNET);

			CByteArrayEx expected;
			expected.Append(code);
			expected.Append((WORD)sizeof(message1));
			expected.Append(message1, sizeof(message1));

			// Setup.
			CString directory;
			CString pathname(SetupTestFileName(filename, directory));

			// Perform operation(s) to be tested.
			int bytesWritten = CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, message1, (WORD)sizeof(message1));

			// Perform test(s).
			CArray<BYTE> contents;
			CUnitTestingUtils::GetByteArrayFromFile(pathname, contents);
			Assert::AreEqual(true, CUnitTestingUtils::RemoveFileAndDirectory(directory, filename), L"Error file not cleaned up.");

			Assert::AreEqual(expected.GetCount(), contents.GetCount(), L"Contents");
			Assert::AreEqual(0, memcmp(expected.GetData(), contents.GetData(), expected.GetCount()), L"Contents");
			Assert::AreEqual(expected.GetCount(), bytesWritten, L"bytesWritten.");
		}

		/// <summary>
		/// Functional test for:
		///     CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile();
		/// Test file contents after writing to a file that is not present and then writing again.
		/// </summary>
		TEST_METHOD(DiagnosticsReportBadPollerProtocolSendPacketToLogFileTest3)
		{
			// Data.
			const LPCTSTR filename(_T("test.sdf"));
			const BYTE message1[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10 };
			const BYTE *message2(nullptr);
			const WORD sizeofMessage2(0);
			const BYTE message3[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20 };
			const BYTE code(DATA_WRITE | DATA_ETHERNET);

			CByteArrayEx expected;
			expected.Append(code);
			expected.Append((WORD)sizeof(message1));
			expected.Append(message1, sizeof(message1));
			expected.Append(code);
			expected.Append((WORD)sizeofMessage2);
			expected.Append(message2, sizeofMessage2);
			expected.Append(code);
			expected.Append((WORD)sizeof(message3));
			expected.Append(message3, sizeof(message3));

			// Setup.
			CString directory;
			CString pathname(SetupTestFileName(filename, directory));

			// Perform operation(s) to be tested.
			int bytesWritten = CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, message1, (WORD)sizeof(message1));
			bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, nullptr, 0);
			bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, message3, (WORD)sizeof(message3));

			// Perform test(s).
			CArray<BYTE> contents;
			CUnitTestingUtils::GetByteArrayFromFile(pathname, contents);
			Assert::AreEqual(true, CUnitTestingUtils::RemoveFileAndDirectory(directory, filename), L"Error file not cleaned up.");

			Assert::AreEqual(expected.GetCount(), contents.GetCount(), L"GetCount");
			Assert::AreEqual(0, memcmp(expected.GetData(), contents.GetData(), expected.GetCount()), L"Contents");
			Assert::AreEqual(expected.GetCount(), bytesWritten, L"bytesWritten.");
		}

		/// <summary>
		/// Functional test for:
		/// Test for correct contant value(s).
		/// </summary>
		TEST_METHOD(DiagnosticsMaxLogFileSize)
		{
			Assert::AreEqual((__int64)16 * 1024, CDiagnostics::MAX_LOG_FILE_SIZE, L"MAX_LOG_FILE_SIZE");
		}

		/// <summary>
		/// Functional test for:
		///     CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile();
		/// Test that file size is under maximum file size after writing.
		/// </summary>
		TEST_METHOD(DiagnosticsReportBadPollerProtocolSendPacketToLogFile_UnderMaxSizeTest)
		{
			// Data.
			const LPCTSTR filename(_T("test.sdf"));
			const WORD messageLength(1024);
			const int numReports((int)(CDiagnostics::MAX_LOG_FILE_SIZE / messageLength) - 1);

			CByteArray message;
			message.SetSize(messageLength);
			memset(message.GetData(), 0xdd, messageLength);

			// Setup.
			CString directory;
			CString pathname(SetupTestFileName(filename, directory));

			// Perform operation(s) to be tested.
			int bytesWritten(0);
			for (int num(0); num < numReports; ++num)
			{
				bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, message.GetData(), (WORD)message.GetCount());
			}

			// Perform test(s).
			Assert::IsTrue(bytesWritten < CDiagnostics::MAX_LOG_FILE_SIZE, L"bytesWritten");
			Assert::AreEqual((ULONGLONG)bytesWritten, CUnitTestingUtils::GetFileSize(pathname), L"GetFileSize");
			Assert::AreEqual(true, CUnitTestingUtils::RemoveFileAndDirectory(directory, filename), L"Error file not cleaned up.");
		}

		/// <summary>
		/// Functional test for:
		///     CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile();
		/// Test that file size is over maximum file size after writing.
		/// </summary>
		TEST_METHOD(DiagnosticsReportReportBadPollerProtocolSendPacketToLogFile_OverMaxSizeTest)
		{
			// Data.
			const LPCTSTR filename(_T("test.sdf"));
			const WORD messageLength(1024);
			const int numReports((int)(CDiagnostics::MAX_LOG_FILE_SIZE / messageLength));

			CByteArray message;
			message.SetSize(messageLength);
			memset(message.GetData(), 0xdd, messageLength);

			// Setup.
			CString directory;
			CString pathname(SetupTestFileName(filename, directory));

			// Perform operation(s) to be tested.
			int bytesWritten(0);
			for (int num(0); num < numReports; ++num)
			{
				bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname, message.GetData(), (WORD)message.GetCount());
			}

			// Perform test(s).
			Assert::IsTrue(bytesWritten > CDiagnostics::MAX_LOG_FILE_SIZE, L"bytesWritten");
			Assert::AreEqual((ULONGLONG)bytesWritten, CUnitTestingUtils::GetFileSize(pathname), L"GetFileSize");
			Assert::AreEqual(true, CUnitTestingUtils::RemoveFileAndDirectory(directory, filename), L"Error file not cleaned up.");
		}

		/// <summary>
		/// Functional test for:
		///     CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile();
		/// Test that file size is reset before last writing last report.
		/// In other words, test that the file contains only the last report.
		/// </summary>
		TEST_METHOD(DiagnosticsReportBadPollerProtocolSendPacketToLogFile_ResetSizeTest)
		{
			// Data.
			const LPCTSTR filename(_T("test.sdf"));
			const WORD messageLength(1024);
			const BYTE code(DATA_WRITE | DATA_ETHERNET);
			const ULONGLONG expectedFileSize(sizeof(BYTE) + sizeof(messageLength) + messageLength); // Code, messageLength, plus message.
			const int numReports((int)(CDiagnostics::MAX_LOG_FILE_SIZE / messageLength) + 1);

			CByteArray message;
			message.SetSize(messageLength);

			// Setup.
			CString directory;
			CString pathname(SetupTestFileName(filename, directory));

			// Perform operation(s) to be tested.
			int bytesWritten(0);
			for (int num(0); num < numReports; ++num)
			{
				memset(message.GetData(), num, message.GetCount());
				bytesWritten += CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(pathname,
					message.GetData(), 
					(WORD)message.GetCount());
			}

			// Perform test(s).
			Assert::IsTrue(bytesWritten > CDiagnostics::MAX_LOG_FILE_SIZE, L"bytesWritten");
			Assert::IsTrue(expectedFileSize < CDiagnostics::MAX_LOG_FILE_SIZE, L"expectedFileSize");
			Assert::AreEqual(expectedFileSize, CUnitTestingUtils::GetFileSize(pathname), L"GetFileSize");

			CByteArrayEx expected;
			expected.Append(code);
			expected.Append((WORD)message.GetCount());
			expected.Append(message.GetData(), message.GetCount());
			CArray<BYTE> contents;
			CUnitTestingUtils::GetByteArrayFromFile(pathname, contents);
			Assert::AreEqual(expected.GetCount(), contents.GetCount(), L"Contents");
			Assert::AreEqual(0, memcmp(expected.GetData(), contents.GetData(), expected.GetCount()), L"Contents");

			Assert::AreEqual(true, CUnitTestingUtils::RemoveFileAndDirectory(directory, filename), L"Error file not cleaned up.");
		}

		// Provide a file with 3 lines.
		class CMemFileTestError1 : public CMemFile
		{
		public:
			CMemFileTestError1(
				LPCTSTR fileNameExpected,
				UINT nOpenFlagsExpected) :
				m_nOpenFlags(0),
				m_openCount(0),
				m_closeCount(0),
				m_fileNameExpected(fileNameExpected),
				m_nOpenFlagsExpected(nOpenFlagsExpected)
			{
				// Initialize file contents.
				Write(m_fileContentsExpected, m_fileLengthExpected);
				SeekToBegin();
			}

			virtual ~CMemFileTestError1()
			{
			}

			virtual BOOL Open(
				LPCTSTR lpszFileName,
				UINT nOpenFlags,
				CFileException* pError = NULL
				)
			{
				BOOL retval(FALSE);

				m_fileName = lpszFileName;
				m_nOpenFlags = nOpenFlags;
				++m_openCount;

				if (m_fileNameExpected == lpszFileName &&
					m_nOpenFlagsExpected == nOpenFlags)
				{
					retval = TRUE;
				}

				return retval;
			}

			virtual void Close()
			{
				++m_closeCount;
			}

			CString m_fileName;
			UINT m_nOpenFlags;
			DWORD m_openCount;
			DWORD m_closeCount;
			CString m_fileNameExpected;
			UINT m_nOpenFlagsExpected;

			const static size_t m_fileLengthExpected = 54;
			const static BYTE m_fileContentsExpected[m_fileLengthExpected + 1];
		};

		class CMemFileTest : public CMemFile
		{
		public:
			CMemFileTest(
				LPCTSTR fileNameExpected,
				UINT nOpenFlagsExpected,
				const BYTE* fileContentsExpected,
				UINT fileLengthExpected) :
				m_nOpenFlags(0),
				m_openCount(0),
				m_closeCount(0),
				m_fileNameExpected(fileNameExpected),
				m_nOpenFlagsExpected(nOpenFlagsExpected)
			{
				// Initialize file contents.
				Write(fileContentsExpected, fileLengthExpected);
				SeekToBegin();
			}

			virtual ~CMemFileTest()
			{
			}

			virtual BOOL Open(
				LPCTSTR lpszFileName,
				UINT nOpenFlags,
				CFileException* pError = NULL
				)
			{
				BOOL retval(FALSE);

				m_fileName = lpszFileName;
				m_nOpenFlags = nOpenFlags;
				++m_openCount;

				if (m_fileNameExpected == lpszFileName &&
					m_nOpenFlagsExpected == nOpenFlags)
				{
					retval = TRUE;
				}

				return retval;
			}

			virtual void Close()
			{
				++m_closeCount;
			}

			CString m_fileName;
			UINT m_nOpenFlags;
			DWORD m_openCount;
			DWORD m_closeCount;
			CString m_fileNameExpected;
			UINT m_nOpenFlagsExpected;
		};

		/// <summary>
		/// Code coverage and functional test for:
		///     CDiagnostics::ReadEndOfFile()
		/// Test with Open failure due to non-existent filename.
		/// </summary>
		TEST_METHOD(DiagnosticsReadEndOfFile_BadFileNameTest)
		{
			// Input(s).
			CString errorFileName(_T("A:") _T("\\ErrorFile") _T("\\Error.txt"));

			// Expected result(s).
			const UINT bufferLengthExpected(0);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {
			};

			// Setup.
			CString errorFileNameThatExists(_T("E:") _T("\\ErrorFile") _T("\\Error.txt"));
			CMemFileTestError1 file(errorFileNameThatExists, (CFile::modeRead | CFile::shareDenyNone));
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			const UINT bufferSize(53);
			BYTE bufferResult[bufferSize];
			UINT bytesReadResult(12345);
			ULONGLONG fileSizeResult(1234567);
			bool returnResult = CDiagnostics::ReadEndOfFile(file, errorFileName, bufferResult, bufferSize, bytesReadResult, fileSizeResult);

			// Test(s).
			Assert::IsTrue(file.m_fileName == errorFileName, _T("m_fileName"));
			Assert::IsTrue(file.m_nOpenFlags == (CFile::modeRead | CFile::shareDenyNone), _T("m_nOpenFlags"));
			Assert::IsTrue(file.m_openCount == 1, _T("m_openCount"));
			Assert::IsTrue(file.m_closeCount == 0, _T("m_closeCount")); // Expect no Close() if Open fails.

			Assert::IsTrue(returnResult == false, _T("returnResult"));
			Assert::IsTrue(fileSizeResult == bufferLengthExpected, _T("fileSizeResult"));
			Assert::IsTrue(bytesReadResult == bufferLengthExpected, _T("bufferLengthResult"));
			Assert::IsTrue(0 == memcmp(bufferResult, bufferExpected, bufferLengthExpected), _T("memcmp"));

			// Verify that file is unchanged.
			Assert::IsTrue(CUnitTestingUtils::IsEqual(file, CMemFileTestError1::m_fileContentsExpected, CMemFileTestError1::m_fileLengthExpected), _T("LoadErrorFile() IsEqual"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CDiagnostics::ReadEndOfFile()
		/// Test return value for successful open and read 0 bytes.
		/// </summary>
		TEST_METHOD(DiagnosticsReadEndOfFile_EmptyFileTest)
		{
			// Input(s).
			CString errorFileName(_T("E:") _T("\\ErrorFile") _T("\\Error.txt"));

			// Expected result(s).
			const UINT bufferLengthExpected(0);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {
			};

			// Setup.
			CMemFileTest file(errorFileName, (CFile::modeRead | CFile::shareDenyNone), bufferExpected, bufferLengthExpected);
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			const UINT bufferSize(53);
			BYTE bufferResult[bufferSize];
			UINT bytesReadResult(12345);
			ULONGLONG fileSizeResult(1234567);
			bool returnResult = CDiagnostics::ReadEndOfFile(file, errorFileName, bufferResult, bufferSize, bytesReadResult, fileSizeResult);

			// Test(s).
			Assert::IsTrue(file.m_fileName == errorFileName, _T("m_fileName"));
			Assert::IsTrue(file.m_nOpenFlags == (CFile::modeRead | CFile::shareDenyNone), _T("m_nOpenFlags"));
			Assert::IsTrue(file.m_openCount == 1, _T("m_openCount"));
			Assert::IsTrue(file.m_closeCount == 1, _T("m_closeCount")); // Expect no Close() if Open fails.

			Assert::IsTrue(returnResult == true, _T("returnResult"));
			Assert::IsTrue(fileSizeResult == bufferLengthExpected, _T("fileSizeResult"));
			Assert::IsTrue(bytesReadResult == bufferLengthExpected, _T("bufferLengthResult"));
			Assert::IsTrue(0 == memcmp(bufferResult, bufferExpected, bufferLengthExpected), _T("memcmp"));

			// Verify that file is unchanged.
			Assert::IsTrue(CUnitTestingUtils::IsEqual(file, bufferExpected, bufferLengthExpected), _T("LoadErrorFile() IsEqual"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CDiagnostics::ReadEndOfFile()
		/// Test with target buffer size (53) slightly smaller than error file.
		/// </summary>
		TEST_METHOD(DiagnosticsReadEndOfFile_LessThanTest)
		{
			// Inputs.
			CString errorFileName(_T("E:") _T("\\ErrorFile") _T("\\Error.txt"));

			// Expected results.
			const UINT bufferLengthExpected(53);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {
				// "E" // Begining of file that was skipped.
				"rror File Line 1\n"
				"Error File Line 2\n"
				"Error File Line 3\n"
			};

			// Setup.
			CMemFileTestError1 file(errorFileName, (CFile::modeRead | CFile::shareDenyNone));
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			const UINT bufferSize(53);
			BYTE bufferResult[bufferSize];
			UINT bytesReadResult(12345);
			ULONGLONG fileSizeResult(1234567);
			bool returnResult = CDiagnostics::ReadEndOfFile(file, errorFileName, bufferResult, bufferSize, bytesReadResult, fileSizeResult);

			// Test(s).
			Assert::IsTrue(file.m_fileName == errorFileName, _T("m_fileName"));
			Assert::IsTrue(file.m_nOpenFlags == (CFile::modeRead | CFile::shareDenyNone), _T("m_nOpenFlags"));
			Assert::IsTrue(file.m_openCount == 1, _T("m_openCount"));
			Assert::IsTrue(file.m_closeCount == 1, _T("m_closeCount"));

			Assert::IsTrue(returnResult == true, _T("returnResult"));
			Assert::IsTrue(fileSizeResult == bufferLengthExpected + 1, _T("fileSizeResult"));
			Assert::IsTrue(bytesReadResult == bufferLengthExpected, _T("bufferLengthResult"));
			Assert::IsTrue(0 == memcmp(bufferResult, bufferExpected, bufferLengthExpected), _T("memcmp"));

			// Verify that file is unchanged.
			Assert::IsTrue(CUnitTestingUtils::IsEqual(file, CMemFileTestError1::m_fileContentsExpected, CMemFileTestError1::m_fileLengthExpected), _T("LoadErrorFile() IsEqual"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CDiagnostics::ReadEndOfFile()
		/// Test with target buffer size (54) sames size as error file.
		/// </summary>
		TEST_METHOD(DiagnosticsReadEndOfFile_EqualToTest)
		{
			// Inputs.
			CString errorFileName(_T("E:") _T("\\ErrorFile") _T("\\Error.txt"));

			// Expected results.
			const UINT bufferLengthExpected(54);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {
				"Error File Line 1\n"
				"Error File Line 2\n"
				"Error File Line 3\n"
			};

			// Setup.
			CMemFileTestError1 file(errorFileName, (CFile::modeRead | CFile::shareDenyNone));
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			const UINT bufferSize(54);
			BYTE bufferResult[bufferSize];
			UINT bytesReadResult(12345);
			ULONGLONG fileSizeResult(1234567);
			bool returnResult = CDiagnostics::ReadEndOfFile(file, errorFileName, bufferResult, bufferSize, bytesReadResult, fileSizeResult);

			// Test(s).
			Assert::IsTrue(file.m_fileName == errorFileName, _T("m_fileName"));
			Assert::IsTrue(file.m_nOpenFlags == (CFile::modeRead | CFile::shareDenyNone), _T("m_nOpenFlags"));
			Assert::IsTrue(file.m_openCount == 1, _T("m_openCount"));
			Assert::IsTrue(file.m_closeCount == 1, _T("m_closeCount"));

			Assert::IsTrue(returnResult == true, _T("returnResult"));
			Assert::IsTrue(fileSizeResult == bufferLengthExpected, _T("fileSizeResult"));
			Assert::IsTrue(bytesReadResult == bufferLengthExpected, _T("bufferLengthResult"));
			Assert::IsTrue(0 == memcmp(bufferResult, bufferExpected, bufferLengthExpected), _T("memcmp"));

			// Verify that file is unchanged.
			Assert::IsTrue(CUnitTestingUtils::IsEqual(file, CMemFileTestError1::m_fileContentsExpected, CMemFileTestError1::m_fileLengthExpected), _T("LoadErrorFile() IsEqual"));
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CDiagnostics::ReadEndOfFile()
		/// Test with target buffer size (55) slightly larger than error file.
		/// </summary>
		TEST_METHOD(DiagnosticsReadEndOfFile_GreaterThanTest)
		{
			// Inputs.
			CString errorFileName(_T("E:") _T("\\ErrorFile") _T("\\Error.txt"));

			// Expected results.
			const UINT bufferLengthExpected(54);
			const BYTE bufferExpected[bufferLengthExpected + 1] = {
				"Error File Line 1\n"
				"Error File Line 2\n"
				"Error File Line 3\n"
			};

			// Setup.
			CMemFileTestError1 file(errorFileName, (CFile::modeRead | CFile::shareDenyNone));
			CWebDiagnostics wb;

			// Perform operation(s) to be tested.
			const UINT bufferSize(55);
			BYTE bufferResult[bufferSize];
			UINT bytesReadResult(12345);
			ULONGLONG fileSizeResult(1234567);
			bool returnResult = CDiagnostics::ReadEndOfFile(file, errorFileName, bufferResult, bufferSize, bytesReadResult, fileSizeResult);

			// Test(s).
			Assert::IsTrue(file.m_fileName == errorFileName, _T("m_fileName"));
			Assert::IsTrue(file.m_nOpenFlags == (CFile::modeRead | CFile::shareDenyNone), _T("m_nOpenFlags"));
			Assert::IsTrue(file.m_openCount == 1, _T("m_openCount"));
			Assert::IsTrue(file.m_closeCount == 1, _T("m_closeCount"));

			Assert::IsTrue(returnResult == true, _T("returnResult"));
			Assert::IsTrue(fileSizeResult == bufferLengthExpected, _T("fileSizeResult"));
			Assert::IsTrue(bytesReadResult == bufferLengthExpected, _T("bufferLengthResult"));
			Assert::IsTrue(0 == memcmp(bufferResult, bufferExpected, bufferLengthExpected), _T("memcmp"));

			// Verify that file is unchanged.
			Assert::IsTrue(CUnitTestingUtils::IsEqual(file, CMemFileTestError1::m_fileContentsExpected, CMemFileTestError1::m_fileLengthExpected), _T("LoadErrorFile() IsEqual"));
		}
	};

	// Declarations and const/default values for TEST_CLASS static members.
	const BYTE DiagnosticsUnitTests::CMemFileTestError1::m_fileContentsExpected[DiagnosticsUnitTests::CMemFileTestError1::m_fileLengthExpected + 1] = {
		"Error File Line 1\n"
		"Error File Line 2\n"
		"Error File Line 3\n"
#endif //TODO-PORT
}


