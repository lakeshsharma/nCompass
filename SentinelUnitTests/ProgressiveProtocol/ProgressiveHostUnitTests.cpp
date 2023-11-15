#include "stdafx.h"
#include "UnitTestingUtils.h"
#include "ProgressiveProtocol/ProgressiveHost.h"
#include "Utilities.h"
#include "gtest/gtest.h"

#define LINT_PTR size_t
#define L_LPCTSTR const char*

// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, #result)

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, #result)

namespace SentinelNativeUnitTests
{
    class ProgressiveHost : public ::testing::Test {
        protected:
			virtual void SetUp() {
			}
			virtual void TearDown() 
			{
			}
    		// Default expected result(s) for CProgressiveHost.
    		static const std::string m_nameExpected;
    		static const bool m_hasAddressExpected = false;
    		static const CIPAddress m_ipAddressExpected;
    		static const LINT_PTR m_currentAddressIndexExpected = 0;
    		static const CIPAddress* const m_ipAddressesExpected;
    		static const LINT_PTR m_ipAddressCountExpected = 0;
    		static const DWORD m_hostPortExpected = 0;
    		static const std::string m_urlExpected;
    		static const CIPAddress m_defaultIPAddress;

    		void AssertEqual(const CProgressiveHost &obj,
    			const std::string& nameExpected,
    			bool hasAddressExpected,
    			const CIPAddress& ipAddressExpected,
    			LINT_PTR currentAddressIndexExpected,
    			const CIPAddress ipAddressesExpected[],
    			LINT_PTR ipAddressCountExpected,
    			DWORD hostPortExpected,
    			const std::string& urlExpected,
    			int testIndex,
    			L_LPCTSTR functionName,
    			int lineNumber,
    			const int* extraIndex = NULL)
    		{
    			AssertResult2h(obj.GetName(), nameExpected);
    			AssertResult2h(obj.HasAddress(), hasAddressExpected);
    			AssertResult2h(obj.GetIPAddress(), ipAddressExpected);
    			AssertResult2h((int)obj.GetCurrentAddressIndex(), (int)currentAddressIndexExpected);

    			AssertResult2h((int)obj.GetIPAddressCount(), (int)ipAddressCountExpected);

    			AssertResult2h(obj.GetIPAddress(-1), m_defaultIPAddress);
    			for (LINT_PTR index(0); index < ipAddressCountExpected; ++index)
    			{
    				std::string resultName;
    				resultName = FormatString("obj.GetIPAddress(index=%d)", (int)index);
    				AssertResult3h(static_cast<CIPAddress>(obj.GetIPAddress(index)), ipAddressesExpected[index], resultName.c_str());
                }
    			AssertResult2h(obj.GetIPAddress(obj.GetIPAddressCount()), m_defaultIPAddress);

    			AssertResult2h(obj.GetHostPort(), hostPortExpected);
    			AssertResult2h(obj.GetURL(), urlExpected);
    		}
    };

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// Use a "default" CProgressiveHost instance with an empty host name and port zero.
		/// </summary>
		TEST_F(ProgressiveHost, CProgressiveHost_HostNameDefaultTest)
		{
			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CProgressiveHost obj("", 0);

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// </summary>
		TEST_F(ProgressiveHost, CProgressiveHost_HostNameTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				L_LPCTSTR const m_hostName;
				const int m_hostPort;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const std::string m_nameExpected;
				const bool m_hasAddressExpected;
				const TIPAddressStringPort m_ipAddressExpected;
				const LINT_PTR m_ipAddressCountExpected;
				const int m_hostPortExpected;
				const std::string m_urlExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
                { "invalid.", -1, "invalid.", false, { "0.0.0.0", 0 }, 0, -1, "" },
				{ "localhost.", -1, "localhost.", true, { "127.0.0.1", -1 }, 1, -1, "ssl://127.0.0.1:-1?wireFormat=stomp" },

				{ "invalid.", 0, "invalid.", false, { "0.0.0.0", 0 }, 0, 0, "" },
				{ "localhost.", 0, "localhost.", true, { "127.0.0.1", 0 }, 1, 0, "ssl://127.0.0.1:0?wireFormat=stomp" },

				{ "invalid.", 1, "invalid.", false, { "0.0.0.0", 0 }, 0, 1, "" },
				{ "localhost.", 1, "localhost.", true, { "127.0.0.1", 1 }, 1, 1, "ssl://127.0.0.1:1?wireFormat=stomp" },

				{ "invalid.", WORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX, "" },
				{ "localhost.", WORD_MAX, "localhost.", true, { "127.0.0.1", WORD_MAX }, 1, WORD_MAX, "ssl://127.0.0.1:65535?wireFormat=stomp" },

				{ "invalid.", WORD_MAX + 1, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX + 1, "" },
				{ "localhost.", WORD_MAX + 1, "localhost.", true, { "127.0.0.1", WORD_MAX + 1 }, 1, WORD_MAX + 1, "ssl://127.0.0.1:65536?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MAX, "localhost.", true, { "127.0.0.1", INT_MAX }, 1, INT_MAX, "ssl://127.0.0.1:2147483647?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MIN, "localhost.", true, { "127.0.0.1", INT_MIN }, 1, INT_MIN, "ssl://127.0.0.1:-2147483648?wireFormat=stomp" },

				{ "invalid.", (int)DWORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, (int)DWORD_MAX, "" },
				{ "localhost.", (int)DWORD_MAX, "localhost.", true, { "127.0.0.1", (int)DWORD_MAX }, 1, (int)DWORD_MAX, "ssl://127.0.0.1:-1?wireFormat=stomp" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Calculate ipAddressExpected.
				CIPAddress ipAddressExpected;
				ipAddressExpected.SetIP(TEST_DATA[testIndex].m_ipAddressExpected.m_ipAddressString);
				ipAddressExpected.SetPort(TEST_DATA[testIndex].m_ipAddressExpected.m_port);

				// Perform operation(s) to be tested.
				const CProgressiveHost obj(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
				
				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					&ipAddressExpected,
					TEST_DATA[testIndex].m_ipAddressCountExpected,
					TEST_DATA[testIndex].m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use a "default" CProgressiveHost instance with an empty IP address array.
		/// </summary>
		TEST_F(ProgressiveHost, CProgressiveHost_IPAddressArrayDefaultTest)
		{
			// Setup.
			const int testIndex(0);
			CIPAddressArray ipAddressArray;

			// Perform operation(s) to be tested.
			const CProgressiveHost obj(ipAddressArray);

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use the same IP address and port for each element of the IP address array so the elements are in a known "order" even after they have been randomized.
		/// </summary>
		TEST_F(ProgressiveHost, CProgressiveHost_IPAddressArrayOrderedTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort* const m_ipAddressStringPort;
				const int m_ipAddressCount;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const bool m_hasAddressExpected;
				const std::string m_urlExpected;
			} TestDataType;

			TIPAddressStringPort addr1[] = {
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr2[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr3[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, "" },
				{ addr1, _countof(addr1), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr2, _countof(addr2), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr3, _countof(addr3), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
				CIPAddressArray ipAddressArray;
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
					ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
					ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
						TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				}

				const CIPAddress &ipAddressExpected(
					TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

				// Perform operation(s) to be tested.
				const CProgressiveHost obj(ipAddressArray);

				// Test(s).
				AssertEqual(obj,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete[] ipAddressesExpected;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// Use a "default" CProgressiveHost instance with an empty host name and port zero.
		/// </summary>
		TEST_F(ProgressiveHost, CopyConstructor_HostNameDefaultTest)
		{
			// Setup.
			const int testIndex(0);
			const CProgressiveHost progHost("", 0);

			// Perform operation(s) to be tested.
			const CProgressiveHost obj(progHost);

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// </summary>
		TEST_F(ProgressiveHost, CopyConstructor_HostNameTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				L_LPCTSTR const m_hostName;
				const int m_hostPort;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const std::string m_nameExpected;
				const bool m_hasAddressExpected;
				const TIPAddressStringPort m_ipAddressExpected;
				const LINT_PTR m_ipAddressCountExpected;
				const int m_hostPortExpected;
				const std::string m_urlExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ "invalid.", -1, "invalid.", false, { "0.0.0.0", 0 }, 0, -1, "" },
				{ "localhost.", -1, "localhost.", true, { "127.0.0.1", -1 }, 1, -1, "ssl://127.0.0.1:-1?wireFormat=stomp" },

				{ "invalid.", 0, "invalid.", false, { "0.0.0.0", 0 }, 0, 0, "" },
				{ "localhost.", 0, "localhost.", true, { "127.0.0.1", 0 }, 1, 0, "ssl://127.0.0.1:0?wireFormat=stomp" },

				{ "invalid.", 1, "invalid.", false, { "0.0.0.0", 0 }, 0, 1, "" },
				{ "localhost.", 1, "localhost.", true, { "127.0.0.1", 1 }, 1, 1, "ssl://127.0.0.1:1?wireFormat=stomp" },

				{ "invalid.", WORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX, "" },
				{ "localhost.", WORD_MAX, "localhost.", true, { "127.0.0.1", WORD_MAX }, 1, WORD_MAX, "ssl://127.0.0.1:65535?wireFormat=stomp" },

				{ "invalid.", WORD_MAX + 1, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX + 1, "" },
				{ "localhost.", WORD_MAX + 1, "localhost.", true, { "127.0.0.1", WORD_MAX + 1 }, 1, WORD_MAX + 1, "ssl://127.0.0.1:65536?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MAX, "localhost.", true, { "127.0.0.1", INT_MAX }, 1, INT_MAX, "ssl://127.0.0.1:2147483647?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MIN, "localhost.", true, { "127.0.0.1", INT_MIN }, 1, INT_MIN, "ssl://127.0.0.1:-2147483648?wireFormat=stomp" },

				{ "invalid.", (int)DWORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, (int)DWORD_MAX, "" },
				{ "localhost.", (int)DWORD_MAX, "localhost.", true, { "127.0.0.1", (int)DWORD_MAX }, 1, (int)DWORD_MAX, "ssl://127.0.0.1:-1?wireFormat=stomp" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Calculate ipAddressExpected.
				CIPAddress ipAddressExpected;
				ipAddressExpected.SetIP(TEST_DATA[testIndex].m_ipAddressExpected.m_ipAddressString);
				ipAddressExpected.SetPort(TEST_DATA[testIndex].m_ipAddressExpected.m_port);

				// Setup. Construct the source CProgressiveHost instance.
				const CProgressiveHost progHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);

				// Perform operation(s) to be tested.
				const CProgressiveHost obj(progHost);

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					&ipAddressExpected,
					TEST_DATA[testIndex].m_ipAddressCountExpected,
					TEST_DATA[testIndex].m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use a "default" CProgressiveHost instance with an empty IP address array.
		/// </summary>
		TEST_F(ProgressiveHost, CopyConstructor_IPAddressArrayDefaultTest)
		{
			// Setup.
			const int testIndex(0);
			CIPAddressArray ipAddressArray;
			const CProgressiveHost progHost(ipAddressArray);

			// Perform operation(s) to be tested.
			const CProgressiveHost obj(progHost);

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use the same IP address and port for each element of the IP address array so the elements are in a known "order" even after they have been randomized.
		/// </summary>
		TEST_F(ProgressiveHost, CopyConstructor_IPAddressArrayOrderedTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort* const m_ipAddressStringPort;
				const int m_ipAddressCount;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const bool m_hasAddressExpected;
				const std::string m_urlExpected;
			} TestDataType;

			TIPAddressStringPort addr1[] = {
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr2[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr3[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, "" },
				{ addr1, _countof(addr1), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr2, _countof(addr2), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr3, _countof(addr3), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
				CIPAddressArray ipAddressArray;
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
					ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
					ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
						TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				}

				const CIPAddress &ipAddressExpected(
					TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

				// Setup. Construct the source CProgressiveHost instance.
				const CProgressiveHost progHost(ipAddressArray);

				// Perform operation(s) to be tested.
				const CProgressiveHost obj(progHost);

				// Test(s).
				AssertEqual(obj,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete[] ipAddressesExpected;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use source CProgressiveHost instances with randomized lists of IP addresses.
		/// </summary>
		TEST_F(ProgressiveHost, CopyConstructor_RandomTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort* const m_ipAddressStringPort;
				const int m_ipAddressCount;

				// Expected result(s).
				// IP addresses and URL are calculated at runtime.
				const bool m_hasAddressExpected;
			} TestDataType;

			TIPAddressStringPort addr1[] = {
				{ "1.2.3.40", 12340 },
			};

			TIPAddressStringPort addr2[] = {
				{ "1.2.3.41", 12341 },
				{ "1.2.3.42", 12342 },
				{ "1.2.3.43", 12343 },
			};

			TIPAddressStringPort addr3[] = {
				{ "1.2.3.44", 12344 },
				{ "1.2.3.45", 12345 },
				{ "1.2.3.46", 12346 },
				{ "1.2.3.47", 12347 },
				{ "1.2.3.48", 12348 },
			};

			TIPAddressStringPort addr4[] = {
				{ "1.2.3.44", 12344 },
				{ "0.0.0.0", 0 },
				{ "1.2.3.45", 12345 },
				{ "0.0.0.0", 0 },
				{ "0.0.0.0", 0 },
				{ "1.2.3.46", 12346 },
				{ "123.234.56.78", 0x12345678 },
				{ "123.234.56.78", 0x12345678 },
				{ "1.2.3.47", 12347 },
				{ "255.255.255.255", 1234567890 },
				{ "255.255.255.255", 1234567890 },
				{ "1.2.3.48", 12348 },
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false },
				{ addr1, _countof(addr1), true },
				{ addr2, _countof(addr2), true },
				{ addr3, _countof(addr3), true },
				{ addr4, _countof(addr4), true },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
				CIPAddressArray ipAddressArray;
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
					ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
					ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
						TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				}
				const CProgressiveHost progHost(ipAddressArray);

				// Reorder ipAddressesExpected as needed.
				// Reorder our trusted list, instead of overwriting our list with elements from an instance under test.
				bool reorderOkay(true);
				AssertResult2((int)progHost.GetIPAddressCount(), TEST_DATA[testIndex].m_ipAddressCount);
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					CIPAddress hostIPAddress = progHost.GetIPAddress(index);
					if (ipAddressesExpected[index] != hostIPAddress)
					{
						reorderOkay = false;
						for (int swapIndex(index + 1); swapIndex < TEST_DATA[testIndex].m_ipAddressCount; ++swapIndex)
						{
							if (ipAddressesExpected[swapIndex] == hostIPAddress)
							{
								// Found an element to swap.
								CIPAddress temp = ipAddressesExpected[index];
								ipAddressesExpected[index] = ipAddressesExpected[swapIndex];
								ipAddressesExpected[swapIndex] = temp;
								reorderOkay = true;
							}
						}
					}
				}
				AssertResult2(reorderOkay, true);

				const CIPAddress &ipAddressExpected(
					TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

				std::ostringstream stringStream;
				if (TEST_DATA[testIndex].m_ipAddressCount > 0)
				{
					BYTE address[4];
					ipAddressExpected.GetIP(address, _countof(address));
					stringStream << "ssl://" <<
						(unsigned)address[0] << '.' << (unsigned)address[1] << '.' << (unsigned)address[2] << '.' << (unsigned)address[3] <<
						':' << ipAddressExpected.GetPort() << "?wireFormat=stomp";
				}
				const std::string urlExpected(stringStream.str());

				// Perform operation(s) to be tested.
				const CProgressiveHost obj(progHost);

				// Test(s).
				AssertEqual(obj,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					urlExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete[] ipAddressesExpected;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost& operator=(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// Use a "default" CProgressiveHost instance with an empty host name and port zero.
		/// </summary>
		TEST_F(ProgressiveHost, AssignmentOperator_HostNameDefaultTest)
		{
			// Setup.
			const int testIndex(0);
			const CProgressiveHost progHost("", 0);
			CProgressiveHost obj("localhost.", -1234543210); // Set the target instance to something different.

			// Perform operation(s) to be tested.
			obj = progHost;

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost& operator=(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(L_LPCTSTR hostName, DWORD hostPort)
		/// </summary>
		TEST_F(ProgressiveHost, AssignmentOperator_HostNameTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				L_LPCTSTR const m_hostName;
				const int m_hostPort;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const std::string m_nameExpected;
				const bool m_hasAddressExpected;
				const TIPAddressStringPort m_ipAddressExpected;
				const LINT_PTR m_ipAddressCountExpected;
				const int m_hostPortExpected;
				const std::string m_urlExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ "invalid.", -1, "invalid.", false, { "0.0.0.0", 0 }, 0, -1, "" },
				/*{ "localhost.", -1, "localhost.", true, { "127.0.0.1", -1 }, 1, -1, "ssl://127.0.0.1:-1?wireFormat=stomp" },

				{ "invalid.", 0, "invalid.", false, { "0.0.0.0", 0 }, 0, 0, "" },
				{ "localhost.", 0, "localhost.", true, { "127.0.0.1", 0 }, 1, 0, "ssl://127.0.0.1:0?wireFormat=stomp" },

				{ "invalid.", 1, "invalid.", false, { "0.0.0.0", 0 }, 0, 1, "" },
				{ "localhost.", 1, "localhost.", true, { "127.0.0.1", 1 }, 1, 1, "ssl://127.0.0.1:1?wireFormat=stomp" },

				{ "invalid.", WORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX, "" },
				{ "localhost.", WORD_MAX, "localhost.", true, { "127.0.0.1", WORD_MAX }, 1, WORD_MAX, "ssl://127.0.0.1:65535?wireFormat=stomp" },

				{ "invalid.", WORD_MAX + 1, "invalid.", false, { "0.0.0.0", 0 }, 0, WORD_MAX + 1, "" },
				{ "localhost.", WORD_MAX + 1, "localhost.", true, { "127.0.0.1", WORD_MAX + 1 }, 1, WORD_MAX + 1, "ssl://127.0.0.1:65536?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MAX, "localhost.", true, { "127.0.0.1", INT_MAX }, 1, INT_MAX, "ssl://127.0.0.1:2147483647?wireFormat=stomp" },

				{ "invalid.", INT_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, INT_MAX, "" },
				{ "localhost.", INT_MIN, "localhost.", true, { "127.0.0.1", INT_MIN }, 1, INT_MIN, "ssl://127.0.0.1:-2147483648?wireFormat=stomp" },

				{ "invalid.", (int)DWORD_MAX, "invalid.", false, { "0.0.0.0", 0 }, 0, (int)DWORD_MAX, "" },
				{ "localhost.", (int)DWORD_MAX, "localhost.", true, { "127.0.0.1", (int)DWORD_MAX }, 1, (int)DWORD_MAX, "ssl://127.0.0.1:-1?wireFormat=stomp" },
                */
			};

			// Setup. Perform each test with the same instance.
			CProgressiveHost objA("localhost.", -1234543210); // Set the target instance to something different.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Calculate ipAddressExpected.
				CIPAddress ipAddressExpected;
				ipAddressExpected.SetIP(TEST_DATA[testIndex].m_ipAddressExpected.m_ipAddressString);
				ipAddressExpected.SetPort(TEST_DATA[testIndex].m_ipAddressExpected.m_port);

				// Setup. Construct the source CProgressiveHost instance.
				const CProgressiveHost progHost(TEST_DATA[testIndex].m_hostName, TEST_DATA[testIndex].m_hostPort);
				CProgressiveHost objB("localhost.", -1234543210); // Set the target instance to something different.

				// Perform operation(s) to be tested.
				objA = progHost;
				objB = progHost;

				// Test(s).
				AssertEqual(objA,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					&ipAddressExpected,
					TEST_DATA[testIndex].m_ipAddressCountExpected,
					TEST_DATA[testIndex].m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					TEST_DATA[testIndex].m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					&ipAddressExpected,
					TEST_DATA[testIndex].m_ipAddressCountExpected,
					TEST_DATA[testIndex].m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost& operator=(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use a "default" CProgressiveHost instance with an empty IP address array.
		/// </summary>
		TEST_F(ProgressiveHost, AssignmentOperator_IPAddressArrayDefaultTest)
		{
			// Setup.
			const int testIndex(0);
			CIPAddressArray ipAddressArray;

			// Setup. Construct the source CProgressiveHost instance.
			const CProgressiveHost progHost(ipAddressArray);
			CProgressiveHost obj("localhost.", -1234543210); // Set the target instance to something different.

			// Perform operation(s) to be tested.
			obj = progHost;

			// Test(s).
			AssertEqual(obj,
				m_nameExpected,
				m_hasAddressExpected,
				m_ipAddressExpected,
				m_currentAddressIndexExpected,
				m_ipAddressesExpected,
				m_ipAddressCountExpected,
				m_hostPortExpected,
				m_urlExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost& operator=(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use the same IP address and port for each element of the IP address array so the elements are in a known "order" even after they have been randomized.
		/// </summary>
		TEST_F(ProgressiveHost, AssignmentOperator_IPAddressArrayOrderedTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort* const m_ipAddressStringPort;
				const int m_ipAddressCount;

				// Expected result(s).
				// IP addresses are calculated at runtime.
				const bool m_hasAddressExpected;
				const std::string m_urlExpected;
			} TestDataType;

			TIPAddressStringPort addr1[] = {
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr2[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			TIPAddressStringPort addr3[] = {
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
				{ "1.2.3.4", 12345 },
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, "" },
				{ addr1, _countof(addr1), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr2, _countof(addr2), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
				{ addr3, _countof(addr3), true, "ssl://1.2.3.4:12345?wireFormat=stomp" },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveHost objA("localhost.", -1234543210); // Set the target instance to something different.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
				CIPAddressArray ipAddressArray;
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
					ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
					ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
						TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				}

				const CIPAddress &ipAddressExpected(
					TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

				// Setup. Construct the source CProgressiveHost instance.
				const CProgressiveHost progHost(ipAddressArray);
				CProgressiveHost objB("localhost.", -1234543210); // Set the target instance to something different.

				// Perform operation(s) to be tested.
				objA = progHost;
				objB = progHost;

				// Test(s).
				AssertEqual(objA,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					TEST_DATA[testIndex].m_urlExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete[] ipAddressesExpected;
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveHost::CProgressiveHost& operator=(const CProgressiveHost &rhs)
		///     CProgressiveHost::CProgressiveHost(CIPAddressArray &addressArray)
		/// Use source CProgressiveHost instances with randomized lists of IP addresses.
		/// </summary>
		TEST_F(ProgressiveHost, AssignmentOperator_RandomTest)
		{
			typedef struct {
				L_LPCTSTR const m_ipAddressString;
				int m_port;
			} TIPAddressStringPort;

			typedef struct
			{
				// Input(s).
				const TIPAddressStringPort* const m_ipAddressStringPort;
				const int m_ipAddressCount;

				// Expected result(s).
				// IP addresses and URL are calculated at runtime.
				const bool m_hasAddressExpected;
			} TestDataType;

			TIPAddressStringPort addr1[] = {
				{ "1.2.3.40", 12340 },
			};

			TIPAddressStringPort addr2[] = {
				{ "1.2.3.41", 12341 },
				{ "1.2.3.42", 12342 },
				{ "1.2.3.43", 12343 },
			};

			TIPAddressStringPort addr3[] = {
				{ "1.2.3.44", 12344 },
				{ "1.2.3.45", 12345 },
				{ "1.2.3.46", 12346 },
				{ "1.2.3.47", 12347 },
				{ "1.2.3.48", 12348 },
			};

			TIPAddressStringPort addr4[] = {
				{ "1.2.3.44", 12344 },
				{ "0.0.0.0", 0 },
				{ "1.2.3.45", 12345 },
				{ "0.0.0.0", 0 },
				{ "0.0.0.0", 0 },
				{ "1.2.3.46", 12346 },
				{ "123.234.56.78", 0x12345678 },
				{ "123.234.56.78", 0x12345678 },
				{ "1.2.3.47", 12347 },
				{ "255.255.255.255", 1234567890 },
				{ "255.255.255.255", 1234567890 },
				{ "1.2.3.48", 12348 },
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false },
				{ addr1, _countof(addr1), true },
				{ addr2, _countof(addr2), true },
				{ addr3, _countof(addr3), true },
				{ addr4, _countof(addr4), true },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveHost objA("localhost.", -1234543210); // Set the target instance to something different.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				CIPAddress* const ipAddressesExpected(new CIPAddress[TEST_DATA[testIndex].m_ipAddressCount]);
				CIPAddressArray ipAddressArray;
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					ipAddressesExpected[index].SetIP(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString);
					ipAddressesExpected[index].SetPort(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
					ipAddressArray.Add(TEST_DATA[testIndex].m_ipAddressStringPort[index].m_ipAddressString,
						TEST_DATA[testIndex].m_ipAddressStringPort[index].m_port);
				}
				const CProgressiveHost progHost(ipAddressArray);
				CProgressiveHost objB("localhost.", -1234543210); // Set the target instance to something different.

				// Reorder ipAddressesExpected as needed.
				// Reorder our trusted list, instead of overwriting our list with elements from an instance under test.
				bool reorderOkay(true);
				AssertResult2((int)progHost.GetIPAddressCount(), TEST_DATA[testIndex].m_ipAddressCount);
				for (int index(0); index < TEST_DATA[testIndex].m_ipAddressCount; ++index)
				{
					CIPAddress hostIPAddress = progHost.GetIPAddress(index);
					if (ipAddressesExpected[index] != hostIPAddress)
					{
						reorderOkay = false;
						for (int swapIndex(index + 1); swapIndex < TEST_DATA[testIndex].m_ipAddressCount; ++swapIndex)
						{
							if (ipAddressesExpected[swapIndex] == hostIPAddress)
							{
								// Found an element to swap.
								CIPAddress temp = ipAddressesExpected[index];
								ipAddressesExpected[index] = ipAddressesExpected[swapIndex];
								ipAddressesExpected[swapIndex] = temp;
								reorderOkay = true;
							}
						}
					}
				}
				AssertResult2(reorderOkay, true);

				const CIPAddress &ipAddressExpected(
					TEST_DATA[testIndex].m_ipAddressCount > 0 ? ipAddressesExpected[0] : m_defaultIPAddress);

				std::ostringstream stringStream;
				if (TEST_DATA[testIndex].m_ipAddressCount > 0)
				{
					BYTE address[4];
					ipAddressExpected.GetIP(address, _countof(address));
					stringStream << "ssl://" <<
						(unsigned)address[0] << '.' << (unsigned)address[1] << '.' << (unsigned)address[2] << '.' << (unsigned)address[3] <<
						':' << ipAddressExpected.GetPort() << "?wireFormat=stomp";
				}
				const std::string urlExpected(stringStream.str());

				// Perform operation(s) to be tested.
				objA = progHost;
				objB = progHost;

				// Test(s).
				AssertEqual(objA,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					urlExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_nameExpected,
					TEST_DATA[testIndex].m_hasAddressExpected,
					ipAddressExpected,
					m_currentAddressIndexExpected,
					ipAddressesExpected,
					TEST_DATA[testIndex].m_ipAddressCount,
					m_hostPortExpected,
					urlExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Cleanup.
				delete[] ipAddressesExpected;
			}
		}


	const std::string ProgressiveHost::m_nameExpected;
	const CIPAddress ProgressiveHost::m_ipAddressExpected;
	const CIPAddress* const ProgressiveHost::m_ipAddressesExpected(NULL);
	const std::string ProgressiveHost::m_urlExpected;

	const CIPAddress ProgressiveHost::m_defaultIPAddress;
}

