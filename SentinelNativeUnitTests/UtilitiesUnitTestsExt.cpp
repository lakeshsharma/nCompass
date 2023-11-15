#if 000//TODO-PORT
#include "stdafx.h"
#include "Utilities.h"

#include "CppUnitTest.h"

#include "UnitTestingToString.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"
#include "Hardware\WatchDog.h"
#include "IPAddressArray.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(UtilitiesUnitTestsExt)
	{
		static const int StopTimeout = 100;
		static const int CountTimeout = 150;

		struct ThreadData
		{
			enum Sentinel
			{
				RUNNING = 0,
				DONE
			};

			ThreadData()
				: count(0)
				, sentinel(RUNNING)
				, stopEvent(FALSE, TRUE)
			{
			}

			ThreadData::Sentinel StopThread(HANDLE thread, int suspendCount, DWORD timeout = StopTimeout)
			{
				stopEvent.SetEvent();

				while (suspendCount-- > 0)
					::ResumeThread(thread);

				WaitForSingleObject(thread, timeout);

				return sentinel;
			}

			void IncCount()
			{
				countEvent.SetEvent();
			}

			bool WaitForCounted(DWORD timeout = CountTimeout)
			{
				HANDLE handles[2];
				handles[0] = stopEvent;
				handles[1] = countedEvent;

				return ::WaitForMultipleObjects(_countof(handles), handles, FALSE, timeout) == WAIT_OBJECT_0 + 1;
			}

			int count;
			Sentinel sentinel;
			CEvent stopEvent;
			CEvent countEvent;
			CEvent countedEvent;

		private:
			void ThreadMain()
			{
				HANDLE handles[2];
				handles[0] = stopEvent;
				handles[1] = countEvent;

				while (WaitForMultipleObjects(_countof(handles), handles, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)
				{
					count++;
					countedEvent.SetEvent();
				}

				sentinel = DONE;
			}

		public:
			static UINT AFX_CDECL TestThread(PVOID threadParam)
			{
				ThreadData *data = (ThreadData *)threadParam;
				data->ThreadMain();
				return 0;
			}
		};

	public:
		
		TEST_METHOD(UtilitiesGetTimeStampTest)
		{
			// Test normal behavior
			{
				wchar_t timeStamp[32] = L"\0";
				bool success = CUtilities::GetTimeStamp(timeStamp, _countof(timeStamp));
				Assert::IsTrue(success, L"GetTimeStamp returned failure when should work.");
				Assert::IsFalse(0 == lstrcmp(L"", timeStamp), L"GetTimeStamp returned empty string.");
			}

			// Test string too short
			{
				wchar_t timeStamp[2] = L"\0";
				bool success = CUtilities::GetTimeStamp(timeStamp, _countof(timeStamp));
				Assert::IsFalse(success, L"GetTimeStamp returned success when should fail.");
				Assert::IsTrue(0 == lstrcmp(L"", timeStamp), L"Too short string for GetTimeStamp did not work.");
			}
		}

		TEST_METHOD(UtilitiesReportLastErrorTest)
		{
			DWORD lastError = ERROR_SUCCESS;

			SetLastError(ERROR_ERRORS_ENCOUNTERED);
			CUtilities::ReportLastError(&lastError, false, NULL);
			Assert::IsTrue(ERROR_ERRORS_ENCOUNTERED == lastError, L"ReportLastError did not return error set.");

			SetLastError(ERROR_FILE_NOT_FOUND);
			CUtilities::ReportLastError(&lastError, false, NULL);
			Assert::IsTrue(ERROR_ERRORS_ENCOUNTERED == lastError, L"ReportLastError did not preserve first error found.");
		}

		TEST_METHOD(UtilitiesOpenThreadFromIdBadIdTest)
		{
			DWORD lastError = ERROR_SUCCESS;
			HANDLE handle = CUtilities::OpenThreadFromId(-1, THREAD_ALL_ACCESS, &lastError, false);
			Assert::IsTrue(INVALID_HANDLE_VALUE == handle, L"Return valid handle from invalid id.");
			Assert::IsFalse(ERROR_SUCCESS == lastError, L"Last Error not set from failed attempt to open thread.");
		}

		TEST_METHOD(UtilitiesOpenThreadFromIdTest)
		{
			ThreadData threadData;
			CWinThread* thread = AfxBeginThread(ThreadData::TestThread, &threadData, THREAD_PRIORITY_HIGHEST);
			DWORD threadId = thread->m_nThreadID;
			
			DWORD lastError = ERROR_SUCCESS;
			HANDLE handle = CUtilities::OpenThreadFromId(threadId, THREAD_ALL_ACCESS, &lastError, false);
			CloseHandle(handle);
			threadData.StopThread(thread->m_hThread, 0, 1000);

			Assert::AreNotEqual(INVALID_HANDLE_VALUE, handle, AssertMsg::FormatOnly(L"Unable to get thread handle, id = %d, error = %d.", threadId, lastError));
			Assert::AreEqual((int) ThreadData::DONE, (int) threadData.sentinel, AssertMsg::FormatOnly(L"Thread did not set DONE."));
		}

		int TestSuspendThread(int line, DWORD threadId, ThreadData& threadData, HANDLE handle, int expectedCount)
		{
			DWORD lastError = ERROR_SUCCESS;
			int threadCount = CUtilities::SuspendThread(threadId, &lastError, false);
			if (-1 == threadCount)
			{
				threadData.StopThread(handle, expectedCount);
				Assert::Fail(AssertMsg::Format(line, nullptr, L"Unable to suspend thread, id = %d, error = %d.", threadId, lastError));
			}

			if (expectedCount != threadCount)
			{
				threadData.StopThread(handle, expectedCount);
				Assert::Fail(AssertMsg::Format(line, nullptr, L"SuspendThread reports already suspended, id = %d, count = %d, expected = %d.", threadId, threadCount, expectedCount));
			}

			return threadCount;
		}

		int TestResumeThread(int line, DWORD threadId, ThreadData& threadData, HANDLE handle, int expectedCount)
		{
			DWORD lastError = ERROR_SUCCESS;
			int threadCount = CUtilities::ResumeThread(threadId, &lastError, false);
			if (-1 == threadCount)
			{
				threadData.StopThread(handle, expectedCount);
				Assert::Fail(AssertMsg::Format(line, nullptr, L"Unable to resume thread, id = %d, error = %d.", threadId, lastError));
			}

			if (expectedCount != threadCount)
			{
				threadData.StopThread(handle, expectedCount);
				Assert::Fail(AssertMsg::Format(line, nullptr, L"ResumeThread reports still suspended, id = %d, count = %d, expected = %d.", threadId, threadCount, expectedCount));
			}

			return threadCount;
		}

		TEST_METHOD(UtilitiesSuspendResumeThreadTest)
		{
			ThreadData threadData;

			CWinThread* thread = AfxBeginThread(ThreadData::TestThread, &threadData, THREAD_PRIORITY_HIGHEST);
			DWORD threadId = thread->m_nThreadID;
			HANDLE threadHandle = thread->m_hThread;

			// Test basic Suspend and Resume
			TestSuspendThread(__LINE__, threadId, threadData, threadHandle, 0);

			int lastCount = threadData.count;

			threadData.IncCount();
			threadData.WaitForCounted();
			if (lastCount != threadData.count)
			{
				threadData.StopThread(threadHandle, 1, StopTimeout);
				Assert::Fail(L"Thread not suspended.");
			}

			TestResumeThread(__LINE__, threadId, threadData, threadHandle, 1);

			threadData.WaitForCounted();
			if (lastCount >= threadData.count)
			{
				threadData.StopThread(threadHandle, 1, StopTimeout);
				Assert::Fail(L"Thread not resumed.");
			}

			// Test nested Suspend and Resume
			TestSuspendThread(__LINE__, threadId, threadData, threadHandle, 0);
			TestSuspendThread(__LINE__, threadId, threadData, threadHandle, 1);

			lastCount = threadData.count;

			threadData.IncCount();
			threadData.WaitForCounted();
			if (lastCount != threadData.count)
			{
				threadData.StopThread(threadHandle, 1, StopTimeout);
				Assert::Fail(L"Thread not suspended after two suspends.");
			}

			TestResumeThread(__LINE__, threadId, threadData, threadHandle, 2);

			threadData.WaitForCounted();
			if (lastCount != threadData.count)
			{
				threadData.StopThread(threadHandle, 1, StopTimeout);
				Assert::Fail(L"Suspended thread not suspended after only one resume.");
			}

			TestResumeThread(__LINE__, threadId, threadData, threadHandle, 1);

			threadData.WaitForCounted();
			if (lastCount >= threadData.count)
			{
				threadData.StopThread(threadHandle, 1, StopTimeout);
				Assert::Fail(L"Thread not resumed.");
			}

			threadData.StopThread(threadHandle, 1, StopTimeout);
		}

		TEST_METHOD(UtilitiesDNSLookupTest)
		{
			// Lookup ip address from an existing domain
			{
				CIPAddressArray addresses;
				LPCWSTR name = L"www.google.com";
				LONG error = CUtilities::DNSLookup(name, addresses);
				CString msg;
				msg.AppendFormat(L"Failed to find domain, %s, returned error, %d.", name, error);
				Assert::IsTrue(0 == error, msg);
				Assert::IsTrue(addresses.GetCount() > 0, L"No IP address found");
			}
			// Lookup ip address from a non-existent domain
			{
				CIPAddressArray addresses;
				LPCWSTR name = L"bad.bad.bad";
				LONG error = CUtilities::DNSLookup(name, addresses);
				CString msg;
				msg.AppendFormat(L"Found non-existent domain, %s.", name);
				Assert::IsTrue(0 != error, msg);
				Assert::IsTrue(addresses.GetCount() == 0, L"IP addresses found");
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::DNSLookup()
		// Use a domain name that should always resolve to an address per RFC 6761, if the implementation supports IPv4 addresses.
		/// </summary>
		TEST_METHOD(UtilitiesDNSLookupGoodTest)
		{
			// Lookup ip address from an existing domain
			CIPAddressArray addresses;
			LPCTSTR name = _T("localhost.");
			LONG error = CUtilities::DNSLookup(name, addresses);
			CString msg;
			msg.AppendFormat(L"Failed to find domain, %s, returned error, %d.", name, error);
			Assert::IsTrue(0 == error, msg);
			Assert::IsTrue(addresses.GetCount() > 0, L"No IP address found");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::DNSLookup()
		/// Use a domain name that should never resolve to an address per RFC 6761.
		/// </summary>
		TEST_METHOD(UtilitiesDNSLookupBadTest)
		{
			// Lookup ip address from an existing domain
			CIPAddressArray addresses;
			LPCTSTR name = _T("invalid.");
			LONG error = CUtilities::DNSLookup(name, addresses);
			CString msg;
			msg.AppendFormat(L"Found non-existent domain, %s.", name);
			Assert::IsTrue(0 != error, msg);
			Assert::IsTrue(addresses.GetCount() == 0, L"IP addresses found");
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::DNSLookup()
		/// Verify that input/out array is cleared on a lookup that fails.
		/// </summary>
		TEST_METHOD(UtilitiesDNSLookupClearGoodTest)
		{
			CIPAddress initialIPAddress;
			initialIPAddress.SetIP(_T("0.0.0.0")); // This address should NOT be returned by a valid DNS lookup.

			CIPAddressArray addresses;
			addresses.Add(initialIPAddress);
			Assert::IsTrue(1 == addresses.GetCount(), _T("1 == addresses.GetCount()")); // Verify setup.

			// Use a domain name that should always resolve to an address per RFC 6761, if the implementation supports IPv4 addresses.
			LPCTSTR name = _T("localhost.");
			LONG error = CUtilities::DNSLookup(name, addresses);
			CString msg;
			msg.AppendFormat(L"Failed to find domain, %s, returned error, %d.", name, error);
			Assert::IsTrue(0 == error, msg);
			Assert::IsTrue(addresses.GetCount() > 0, L"No IP address found");

			// Verify that the initial address in the array is no longer in the array.
			for (INT_PTR index(0); index < addresses.GetCount(); ++index)
			{
				Assert::IsTrue(initialIPAddress != addresses.GetAt(index), _T("initialIPAddress found"));
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CUtilities::DNSLookup()
		/// Verify that input/out array is cleared on a lookup that fails.
		/// </summary>
		TEST_METHOD(UtilitiesDNSLookupClearBadTest)
		{
			CIPAddressArray addresses;
			addresses.Add(_T("1.2.3.4"));
			Assert::IsTrue(1 == addresses.GetCount(), _T("1 == addresses.GetCount()")); // Verify setup.

			/// Use a domain name that should never resolve to an address per RFC 6761.
			LPCTSTR name = _T("invalid.");
			LONG error = CUtilities::DNSLookup(name, addresses);
			CString msg;
			msg.AppendFormat(L"Found non-existent domain, %s.", name);
			Assert::IsTrue(0 != error, msg);
			Assert::IsTrue(addresses.GetCount() == 0, L"IP addresses found or remain.");
		}


		TEST_METHOD(Utilities_MemCopySafe_ToOffset_BadOk)
		{
			bool ok = false;
			size_t dstBufferOffset = 2;

			byte dstBuffer[6];
			memset(dstBuffer, '0', sizeof(dstBuffer));

			CUtilities::MemCopySafe_ToOffset(&ok, dstBuffer, _countof(dstBuffer), &dstBufferOffset, "123", 3);
			Assert::IsFalse(ok, L"ok=false [ok]");
			Assert::AreEqual(2U, dstBufferOffset, L"ok=false [dstBufferOffset]");
			Assert::IsTrue(memcmp(dstBuffer, "000000", _countof(dstBuffer)) == 0, L"ok=false [dstBuffer]");
		}

		TEST_METHOD(Utilities_MemCopySafe_ToOffset)
		{
			struct
			{
				int			line;

				size_t		orgDstBufferOffset;
				const char* pSrc;
				size_t		srcLen;

				bool		expectedOk;
				size_t		expectedDstOffset;
				char		expectedBuffer[7];

				void Run(int test)
				{
					bool ok					= true;
					size_t dstBufferOffset	= orgDstBufferOffset;

					char dstBuffer[7] = "012345";

					CUtilities::MemCopySafe_ToOffset(&ok, (byte*)dstBuffer, 6, &dstBufferOffset, pSrc, srcLen);
					Assert::AreEqual(expectedOk, ok, AssertMsg::Format(line, L"Ok"));
					Assert::AreEqual(expectedDstOffset, dstBufferOffset, AssertMsg::Format(line, L"dstBufferOffset"));
					Assert::IsTrue(memcmp(dstBuffer, expectedBuffer, 6) == 0, AssertMsg::Format(line, L"dstBuffer"));
				}
			}
			static tests[] =
			{
				{ __LINE__, 0, "abcd", 3, true,  3, { "abc345" } },
				{ __LINE__, 1, "abcd", 3, true,  4, { "0abc45" } },
				{ __LINE__, 2, "abcd", 3, true,  5, { "01abc5" } },
				{ __LINE__, 3, "abcd", 3, true,  6, { "012abc" } },
				{ __LINE__, 4, "abcd", 3, false, 4, { "012345" } },
				{ __LINE__, SIZE_MAX, "ab", 3, false, SIZE_MAX, { "012345" } },
				{ __LINE__, 3, "a",    2, true,  5, { "012a""\0""5" } },
				{ __LINE__, 0, "abcdef",  6,  true, 6, { "abcdef" } },
				{ __LINE__, 0, "abcdefg", 7, false, 0, { "012345" } }
			};

			for (int test = 0; test < _countof(tests); test++)
			{
				tests[test].Run(test);
			}
		}

// #pragma region MemCopySafe_IncSourcePtr
		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with ok false.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_NotOk)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = false;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, sizeof(target), source, sourceEnd);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target length of 0.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_Length0NullPtrs)
		{
			// Data.
			const BYTE targetDefault(0x12);

			bool ok = true;
			BYTE target(targetDefault);
			const size_t targetSize(0);
			const byte sourceBuffer[1] = { 0xef };
			const byte *sourceEnd(sourceBuffer);
			const byte *source(nullptr);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, nullptr, targetSize, source, nullptr);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)nullptr, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target nullptr.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_TargetNullptr)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, nullptr, targetSize, source, sourceEnd);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a source nullptr.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_SourceNullptr)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(nullptr);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)nullptr, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a sourceEnd nullptr.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_SourceEndNullptr)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, nullptr);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target length of 0 and sourceEnd that is too small.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_Length0SourceEndTooSmall)
		{
			// Data.
			const BYTE targetDefault(0x12);

			bool ok = true;
			BYTE target(targetDefault);
			const size_t targetSize(0);
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + targetSize - 1);
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target length of 1 and sourceEnd that is too small.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_Length1SourceEndTooSmall)
		{
			// Data.
			const BYTE targetDefault(0x12);

			bool ok = true;
			BYTE target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + targetSize - 1);
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target length of 2 and sourceEnd that is too small.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_Length2SourceEndTooSmall)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + targetSize - 1);
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(false, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)sourceBuffer, (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target length of 0.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_GoodSameLength0)
		{
			// Data.
			const BYTE targetDefault(0x12);

			bool ok = true;
			BYTE target(targetDefault);
			const size_t targetSize(0);
			const byte sourceBuffer[1] = { 0xef };
			const byte *sourceEnd(sourceBuffer);
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual(targetDefault, target, L"target");
			Assert::AreEqual((void *)(sourceBuffer + targetSize), (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target size of 1 and source buffer is the same length.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_GoodSameLength1)
		{
			// Data.
			const BYTE targetDefault(0x12);

			bool ok = true;
			BYTE target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[1] = { 0xef };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual((BYTE)0xef, target, L"target");
			Assert::AreEqual((void *)(sourceBuffer + targetSize), (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target size of 1 when target memory is greater than the target size specified.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_GoodSameLength1Partial)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(1);
			const byte sourceBuffer[1] = { 0xef };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual((WORD)0x12ef, target, L"target");
			Assert::AreEqual((void *)(sourceBuffer + targetSize), (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target size of 2 and source buffer is the same length.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_GoodSameLength2)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[2] = { 0xef, 0xcd };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual((WORD)0xcdef, target, L"target");
			Assert::AreEqual((void *)(sourceBuffer + targetSize), (void *)source, L"source");
		}

		/// <summary>
		/// Functional test for:
		///     CUtilities::MemCopySafe_IncSourcePtr()
		/// Test with a target size of 2 and source buffer has extra length.
		/// </summary>
		TEST_METHOD(Utilities_MemCopySafe_IncSourcePtr_GoodExtraLength)
		{
			// Data.
			const WORD targetDefault(0x1234);

			bool ok = true;
			WORD target(targetDefault);
			const size_t targetSize(sizeof(target));
			const byte sourceBuffer[3] = { 0xef, 0xcd, 0xab };
			const byte *sourceEnd(sourceBuffer + sizeof(sourceBuffer));
			const byte *source(sourceBuffer);

			// Perform operation(s) and test(s).
			CUtilities::MemCopySafe_IncSourcePtr(ok, &target, targetSize, source, sourceEnd);
			Assert::AreEqual(true, ok, L"ok");
			Assert::AreEqual((WORD)0xcdef, target, L"target");
			Assert::AreEqual((void *)(sourceBuffer + targetSize), (void *)source, L"source");
		}
// #pragma endregion MemCopySafe_IncSourcePtr

		TEST_METHOD(Utilities_RectSetSize)
		{
			CRect rect(5, 10, 100, 500);
			SIZE  size = { 15, 12 };
			CUtilities::CRectSetSize(rect, size);

			Assert::AreEqual( 5l, rect.left,   L"CRectSetSize()[left]");
			Assert::AreEqual(10l, rect.top ,   L"CRectSetSize()[top]");
			Assert::AreEqual(20l, rect.right,  L"CRectSetSize()[right]");
			Assert::AreEqual(22l, rect.bottom, L"CRectSetSize()[bottom]");
		}

		TEST_METHOD(Utilities_RotateRectBack)
		{
			CRect rect(0, 0, 8, 6);
			CSize size(8, 6);

			CRect
			result = CUtilities::RotateRectBack(rect, size, DesktopOrientationNoChange);
			Assert::AreEqual(0l, result.left,   L"RotateRectBack(... DesktopOrientationNoChange).left");
			Assert::AreEqual(0l, result.top ,   L"RotateRectBack(... DesktopOrientationNoChange).top");
			Assert::AreEqual(8l, result.right,  L"RotateRectBack(... DesktopOrientationNoChange).right");
			Assert::AreEqual(6l, result.bottom, L"RotateRectBack(... DesktopOrientationNoChange).bottom");

			result = CUtilities::RotateRectBack(rect, size, DesktopOrientationDefault);
			Assert::AreEqual(0l, result.left,   L"RotateRectBack(... DesktopOrientationNoChange).left");
			Assert::AreEqual(0l, result.top,    L"RotateRectBack(... DesktopOrientationNoChange).top");
			Assert::AreEqual(8l, result.right,  L"RotateRectBack(... DesktopOrientationNoChange).right");
			Assert::AreEqual(6l, result.bottom, L"RotateRectBack(... DesktopOrientationNoChange).bottom");

			result = CUtilities::RotateRectBack(rect, size, DesktopOrientation90);
			Assert::AreEqual(0l,  result.left,   L"RotateRectBack(... DesktopOrientationNoChange).left");
			Assert::AreEqual(-2l, result.top,    L"RotateRectBack(... DesktopOrientationNoChange).top");
			Assert::AreEqual(6l,  result.right,  L"RotateRectBack(... DesktopOrientationNoChange).right");
			Assert::AreEqual(6l,  result.bottom, L"RotateRectBack(... DesktopOrientationNoChange).bottom");

			result = CUtilities::RotateRectBack(rect, size, DesktopOrientation180);
			Assert::AreEqual(0l,  result.left,   L"RotateRectBack(... DesktopOrientationNoChange).left");
			Assert::AreEqual(0l,  result.top,    L"RotateRectBack(... DesktopOrientationNoChange).top");
			Assert::AreEqual(8l,  result.right,  L"RotateRectBack(... DesktopOrientationNoChange).right");
			Assert::AreEqual(6l,  result.bottom, L"RotateRectBack(... DesktopOrientationNoChange).bottom");

			result = CUtilities::RotateRectBack(rect, size, DesktopOrientation270);
			Assert::AreEqual(2l,  result.left,   L"RotateRectBack(... DesktopOrientationNoChange).left");
			Assert::AreEqual(0l,  result.top,    L"RotateRectBack(... DesktopOrientationNoChange).top");
			Assert::AreEqual(8l,  result.right,  L"RotateRectBack(... DesktopOrientationNoChange).right");
			Assert::AreEqual(8l,  result.bottom, L"RotateRectBack(... DesktopOrientationNoChange).bottom");
		}

		TEST_METHOD(FormatLocalMilitaryDateTimeStringFromUTC_Test)
		{
			SYSTEMTIME sysTime;
			sysTime.wYear = 2015;
			sysTime.wMonth = 6;
			sysTime.wDayOfWeek = 1;
			sysTime.wDay = 1;
			sysTime.wHour = 12;
			sysTime.wMinute = 13;
			sysTime.wSecond = 14;
			sysTime.wMilliseconds = 15;

			CString timeStr = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(sysTime);
			Assert::AreEqual(L"06/01/2015 12:13:14", (LPCTSTR) timeStr);
		}

		TEST_METHOD(FormatLocalMilitaryDateTimeStringFromUTC_JAN1970_SYSTIME_Test)
		{
			SYSTEMTIME sysTime = JAN1970_SYSTIME;

			CString timeStr = CUtilities::FormatLocalMilitaryDateTimeStringFromUTC(sysTime);
			Assert::AreEqual(L"N/A", (LPCTSTR) timeStr);
		}
	};
}
#endif //TODO-PORT