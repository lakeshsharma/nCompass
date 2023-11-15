#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveConfig.h"

// Sentinel includes.
#include "ProcessDump/OperatingSystem.h"

// Unit testing includes.
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"
#include "sstream"

using namespace std;

namespace SentinelNativeUnitTests
{
	// AssertResult macros for calling CUnitTestingUtils from a test method.
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
	class ProgressiveConfig : public ::testing::Test
	{
	public:
		// Default expected result(s).
		static const bool m_hasConfigFileExpected = false;
		static const bool m_activeExpected = false;
		static const bool m_dnsLookupExpected = true;
		static const int m_endpointCountExpected = 2;
		static const string m_endpointsExpected[m_endpointCountExpected];
		static const DWORD m_pingIntervalExpected = 15000;

		void AssertEqual(const CProgressiveConfig &obj,
			bool hasConfigFileExpected,
			bool activeExpected,
			bool dnsLookupExpected,
			DWORD pingIntervalExpected,
			const string endpointsExpected[],
			int endpointCountExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			AssertResult2h(obj.HasConfigFile(), hasConfigFileExpected);
			AssertResult2h(obj.GetActive(), activeExpected);
			AssertResult2h(obj.GetDNSLookup(), dnsLookupExpected);
			AssertResult2h(obj.GetPingInterval(), pingIntervalExpected);

			AssertResult2h(obj.GetEndpointCount(), endpointCountExpected);
			string emptyString;
			AssertResult2h(obj.GetEndpoint(endpointCountExpected), emptyString);

			for (int index = 0; index < endpointCountExpected; ++index)
			{
				stringstream resultName;
				resultName << "obj.GetEndpoint" << "(index=" << index << ")";
				std::string result = resultName.str();
				AssertResult3h(obj.GetEndpoint(index), endpointsExpected[index], result.c_str());
			}
		}

		void f_ProgressiveConfigTest(void)
		{
			// Setup.
			const int testIndex(0);

			// Perform operation(s) to be tested.
			const CProgressiveConfig obj;

			// Test(s).
			AssertEqual(obj,
				m_hasConfigFileExpected,
				m_activeExpected,
				m_dnsLookupExpected,
				m_pingIntervalExpected,
				m_endpointsExpected,
				m_endpointCountExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}


		void f_SetHasConfigFileTest(void)
		{
			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ true, true },
				{ false, false },
				{ false, false },
				{ true, true },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConfig objA;
			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				objA.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				objB.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);

				// Test(s).
				AssertEqual(objA,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetActiveTest(void)
		{
			typedef struct
			{
				// Input(s).
				const bool m_active;

				// Expected result(s).
				const bool m_activeExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ true, true },
				{ false, false },
				{ false, false },
				{ true, true },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConfig objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				objA.SetActive(TEST_DATA[testIndex].m_active);
				objB.SetActive(TEST_DATA[testIndex].m_active);

				// Test(s).
				AssertEqual(objA,
					m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetDNSLookupTest(void)
		{
			typedef struct
			{
				// Input(s).
				const bool m_dnsLookup;

				// Expected result(s).
				const bool m_dnsLookupExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ true, true },
				{ true, true },
				{ false, false },
				{ false, false },
				{ true, true },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConfig objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				objA.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				objB.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);

				// Test(s).
				AssertEqual(objA,
					m_hasConfigFileExpected,
					m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_hasConfigFileExpected,
					m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetPingIntervalTest(void)
		{
			typedef struct
			{
				// Input(s).
				const DWORD m_pingInterval;

				// Expected result(s).
				const DWORD m_pingIntervalExpectedA;
				const DWORD m_pingIntervalExpectedB;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ (DWORD)-1, (DWORD)-1, (DWORD)-1 },
				{ 0, (DWORD)-1, 15000 },
				{ 1, (DWORD)-1, 15000 },
				{ 2, (DWORD)-1, 15000 },
				{ 999, (DWORD)-1, 15000 },
				{ 5000, 5000, 5000 },
				{ 5001, 5001, 5001 },
				{ 1234567890, 1234567890, 1234567890 },
				{ INT_MAX - 1, INT_MAX - 1, INT_MAX - 1 },
				{ INT_MAX, INT_MAX, INT_MAX },

				{ (DWORD)INT_MIN, (DWORD)INT_MIN, (DWORD)INT_MIN },
				{ (DWORD)(INT_MIN + 1), (DWORD)(INT_MIN + 1), (DWORD)(INT_MIN + 1) },
				{ (DWORD)-1234567890, (DWORD)-1234567890, (DWORD)-1234567890 },
				{ DWORD_MAX - 1, DWORD_MAX - 1, DWORD_MAX - 1 },
				{ DWORD_MAX, DWORD_MAX, DWORD_MAX },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConfig objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				objA.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				objB.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);

				// Test(s).
				AssertEqual(objA,
					m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedB,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_AddEndpointTest(void)
		{
			typedef struct
			{
				// Input(s).
				const string* const m_endpoints;
				const int m_endpointCount;

				// Expected result(s).
				const string* const m_endpointsExpectedA;
				const int m_endpointCountExpectedA;
				const string* const m_endpointsExpectedB;
				const int m_endpointCountExpectedB;
			} TestDataType;

			const string endpoints0ExpectedA[] = { "onelinkdiscossl:61614", "onelinkdisco:61613" };

			const string endpoints1[] = { "", };
			const string endpoints1ExpectedA[] = { "" };

			const string endpoints2[] = { " ", };
			const string endpoints2ExpectedA[] = { "", " ", };

			const string endpoints3[] = { " abc ", "def", };
			const string endpoints3ExpectedA[] = { "", " ", " abc ", "def", };

			const string endpoints4[] = { "abc.defghi.com:12345", "tuvw.net:23456", "xyz.gov:34567", };
			const string endpoints4ExpectedA[] = {
				"", " ", " abc ", "def",
				"abc.defghi.com:12345", "tuvw.net:23456", "xyz.gov:34567",
			};

			// 300 characters.
			const string endpoints5[] = {
				"d23456789.123456789.123456789.123456789.123456789." ,"123456789.123456789.123456789.123456789.123456789." // No comma.
				"123456789.123456789.123456789.123456789.123456789.", "123456789.123456789.123456789.123456789.123456789." // No comma.
				"123456789.123456789.123456789.123456789.123456789.", "123456789.123456789.123456789.123456789.123456789.",
			};
			const string endpoints5ExpectedA[] = {
				""," ", " abc ", "def",
				"abc.defghi.com:12345", "tuvw.net:23456", "xyz.gov:34567",
				"d23456789.123456789.123456789.123456789.123456789.", "123456789.123456789.123456789.123456789.123456789." // No comma.
				"123456789.123456789.123456789.123456789.123456789.", "123456789.123456789.123456789.123456789.123456789." // No comma.
				"123456789.123456789.123456789.123456789.123456789.", "123456789.123456789.123456789.123456789.123456789.",
			};

			const TestDataType TEST_DATA[] =
			{
				{ endpoints1, 0, endpoints0ExpectedA, _countof(endpoints0ExpectedA), endpoints0ExpectedA, _countof(endpoints0ExpectedA) },
				{ endpoints1, _countof(endpoints1), endpoints1ExpectedA, _countof(endpoints1ExpectedA), endpoints1, _countof(endpoints1) },
				{ endpoints2, _countof(endpoints2), endpoints2ExpectedA, _countof(endpoints2ExpectedA), endpoints2, _countof(endpoints2) },
				{ endpoints3, _countof(endpoints3), endpoints3ExpectedA, _countof(endpoints3ExpectedA), endpoints3, _countof(endpoints3) },
				{ endpoints4, _countof(endpoints4), endpoints4ExpectedA, _countof(endpoints4ExpectedA), endpoints4, _countof(endpoints4) },
				{ endpoints5, _countof(endpoints5), endpoints5ExpectedA, _countof(endpoints5ExpectedA), endpoints5, _countof(endpoints5) },
			};

			// Setup. Perform each test with the same instance.
			CProgressiveConfig objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new object.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				for (int index = 0; index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					objA.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
					objB.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}

				// Test(s).
				AssertEqual(objA,
					m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpectedB,
					TEST_DATA[testIndex].m_endpointCountExpectedB,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetGet1Test(void)
		{
			// Input(s).
			LPCTSTR endpoint1("abcde.xyz:12345");
			LPCTSTR endpoint2("123.234.56.78:23456");

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const string* const m_endpoints;
				const int m_endpointCount;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const string* const m_endpointsExpected;
				const int m_endpointCountExpected;

			} TestDataType;

			string endpoints1[] = {
				"abcde.xyz:12345",
			};
			string endpoints2[] = {
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			string endpoints3[] = {
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, endpoints1, _countof(endpoints1),
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, endpoints1, _countof(endpoints1),
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				},
			};

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new instance.
				CProgressiveConfig varObj;

				// Perform operation(s) to be tested.
				varObj.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				varObj.SetActive(TEST_DATA[testIndex].m_active);
				varObj.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				varObj.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					varObj.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}
				const CProgressiveConfig& obj(varObj);

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpected,
					TEST_DATA[testIndex].m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetGet2Test(void)
		{
			// Input(s).
			LPCTSTR endpoint1("abcde.xyz:12345");
			LPCTSTR endpoint2("123.234.56.78:23456");

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const string* const m_endpoints;
				const int m_endpointCount;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpectedA;
				const DWORD m_pingIntervalExpectedB;
				const string* const m_endpointsExpectedA;
				const int m_endpointCountExpectedA;
				const string* const m_endpointsExpectedB;
				const int m_endpointCountExpectedB;

			} TestDataType;

			string endpoints1[] = {
				"abcde.xyz:12345",
			};
			string endpoints2[] = {
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			string endpoints3[] = {
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const string endpoints1ExpectedA[] = {
				"abcde.xyz:12345",
			};
			const string endpoints2ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints3ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints4ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints5ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345"
			};
			const string endpoints6ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints7ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, 6000, endpoints1ExpectedA, _countof(endpoints1ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, 1234567890, endpoints2ExpectedA, _countof(endpoints2ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, 4294967295, endpoints3ExpectedA, _countof(endpoints3ExpectedA), endpoints3, _countof(endpoints3),
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 4294967295, 15000, endpoints4ExpectedA, _countof(endpoints4ExpectedA), this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, 6000, endpoints5ExpectedA, _countof(endpoints5ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, 1234567890, endpoints6ExpectedA, _countof(endpoints6ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, 4294967295, endpoints7ExpectedA, _countof(endpoints7ExpectedA), endpoints3, _countof(endpoints3),
				},
			};

			// Setup. Perform each test with the same (pre-existing) instance.
			CProgressiveConfig varObjA;

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new instance.
				CProgressiveConfig varObjB;

				// Perform operation(s) to be tested.
				varObjA.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				varObjA.SetActive(TEST_DATA[testIndex].m_active);
				varObjA.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				varObjA.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					varObjA.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}
				const CProgressiveConfig& objA(varObjA);

				varObjB.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				varObjB.SetActive(TEST_DATA[testIndex].m_active);
				varObjB.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				varObjB.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					varObjB.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}
				const CProgressiveConfig& objB(varObjB);

				// Test(s).
				AssertEqual(objA,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedB,
					TEST_DATA[testIndex].m_endpointsExpectedB,
					TEST_DATA[testIndex].m_endpointCountExpectedB,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetGet2Test_Copy(void)
		{
			// Input(s).
			LPCTSTR endpoint1("abcde.xyz:12345");
			LPCTSTR endpoint2("123.234.56.78:23456");

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const string* const m_endpoints;
				const int m_endpointCount;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpectedA;
				const DWORD m_pingIntervalExpectedB;
				const string* const m_endpointsExpectedA;
				const int m_endpointCountExpectedA;
				const string* const m_endpointsExpectedB;
				const int m_endpointCountExpectedB;

			} TestDataType;

			string endpoints1[] = {
				"abcde.xyz:12345",
			};
			string endpoints2[] = {
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			string endpoints3[] = {
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const string endpoints1ExpectedA[] = {
				"abcde.xyz:12345",
			};
			const string endpoints2ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints3ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints4ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints5ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345"
			};
			const string endpoints6ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints7ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, 6000, endpoints1ExpectedA, _countof(endpoints1ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, 1234567890, endpoints2ExpectedA, _countof(endpoints2ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, 4294967295, endpoints3ExpectedA, _countof(endpoints3ExpectedA), endpoints3, _countof(endpoints3),
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 4294967295, 15000, endpoints4ExpectedA, _countof(endpoints4ExpectedA), this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, 6000, endpoints5ExpectedA, _countof(endpoints5ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, 1234567890, endpoints6ExpectedA, _countof(endpoints6ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, 4294967295, endpoints7ExpectedA, _countof(endpoints7ExpectedA), endpoints3, _countof(endpoints3),
				},
			};

			// Setup. Perform each test with the same (pre-existing) instance.
			CProgressiveConfig objA;

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new instance.
				std::unique_ptr<CProgressiveConfig> objB;

				// Perform operation(s) to be tested.
				objA.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				objA.SetActive(TEST_DATA[testIndex].m_active);
				objA.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				objA.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					if (index == TEST_DATA[testIndex].m_endpointCount - 1)
					{
						// Last loop iteration.
						objB.reset(new CProgressiveConfig(objA));
						objB->AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);

					}
					objA.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}

				// If no loop interations then assign objA to objB.
				if (nullptr == objB.get())
				{
					objB.reset(new CProgressiveConfig(objA));
				}

				// Test(s).  Expect objB endpoints to be the same as objA endpoints.
				AssertEqual(objA,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(*objB,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}

		void f_SetGet2Test_Assignment(void)
		{
			// Input(s).
			LPCTSTR endpoint1("abcde.xyz:12345");
			LPCTSTR endpoint2("123.234.56.78:23456");

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const string* const m_endpoints;
				const int m_endpointCount;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpectedA;
				const DWORD m_pingIntervalExpectedB;
				const string* const m_endpointsExpectedA;
				const int m_endpointCountExpectedA;
				const string* const m_endpointsExpectedB;
				const int m_endpointCountExpectedB;

			} TestDataType;

			string endpoints1[] = {
				"abcde.xyz:12345",
			};
			string endpoints2[] = {
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			string endpoints3[] = {
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const string endpoints1ExpectedA[] = {
				"abcde.xyz:12345",
			};
			const string endpoints2ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints3ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints4ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};
			const string endpoints5ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345"
			};
			const string endpoints6ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
			};
			const string endpoints7ExpectedA[] = {
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
				"abcde.xyz:12345",
				"abcde.xyz:12345",
				"   abcdefghijk.lmnop.xyz:1234567890   ",
				"abcde.xyz:12345",
				"abcdefghijk.lmnop.xyz:1234567890",
				" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 ",
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, 6000, endpoints1ExpectedA, _countof(endpoints1ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, 1234567890, endpoints2ExpectedA, _countof(endpoints2ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, 4294967295, endpoints3ExpectedA, _countof(endpoints3ExpectedA), endpoints3, _countof(endpoints3),
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 4294967295, 15000, endpoints4ExpectedA, _countof(endpoints4ExpectedA), this->m_endpointsExpected, this->m_endpointCountExpected,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, 6000, endpoints5ExpectedA, _countof(endpoints5ExpectedA), endpoints1, _countof(endpoints1),
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, 1234567890, endpoints6ExpectedA, _countof(endpoints6ExpectedA), endpoints2, _countof(endpoints2),
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, 4294967295, endpoints7ExpectedA, _countof(endpoints7ExpectedA), endpoints3, _countof(endpoints3),
				},
			};

			// Setup. Perform each test with the same (pre-existing) instance.
			CProgressiveConfig objA;

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup. Perform each test with a new instance.
				CProgressiveConfig objB;

				// Perform operation(s) to be tested.
				objA.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				objA.SetActive(TEST_DATA[testIndex].m_active);
				objA.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				objA.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					if (index == TEST_DATA[testIndex].m_endpointCount - 1)
					{
						// Last loop iteration.
						objB = objA;
						objB.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);

					}
					objA.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}

				// If no endpoint count loop interations then assign objA to objB.
				if (0 == TEST_DATA[testIndex].m_endpointCount)
				{
					objB = objA;
				}

				// Test(s).  Expect objB endpoints to be the same as objA endpoints.
				AssertEqual(objA,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
				AssertEqual(objB,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpectedA,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);
			}
		}
	};

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::CProgressiveConfig()
	///     CProgressiveConfig::GetActive()
	///     CProgressiveConfig::GetDNSLookup()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetPingInterval()
	/// Code coverage:
	///     CProgressiveConfig::~CProgressiveConfig()
	/// </summary>
	TEST_F(ProgressiveConfig, ProgressiveConfigTest)
	{
		f_ProgressiveConfigTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetHasConfigFile()
	///     CProgressiveConfig::HasConfigFile()
	/// </summary>
	TEST_F(ProgressiveConfig, SetHasConfigFileTest)
	{
		f_SetHasConfigFileTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetActive()
	///     CProgressiveConfig::GetActive()
	/// </summary>
	TEST_F(ProgressiveConfig, SetActiveTest)
	{
		f_SetActiveTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetDNSLookup()
	///     CProgressiveConfig::GetDNSLookup()
	/// </summary>
	TEST_F(ProgressiveConfig, SetDNSLookupTest)
	{
		f_SetDNSLookupTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetPingInterval()
	///     CProgressiveConfig::GetPingInterval()
	/// </summary>
	TEST_F(ProgressiveConfig, SetPingIntervalTest)
	{
		f_SetPingIntervalTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::AddEndpoint()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetEndpoint()
	/// </summary>
	TEST_F(ProgressiveConfig, AddEndpointTest)
	{
		f_AddEndpointTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetHasConfigFile()
	///     CProgressiveConfig::HasConfigFile()
	///     CProgressiveConfig::SetActive()
	///     CProgressiveConfig::GetActive()
	///     CProgressiveConfig::SetDNSLookup()
	///     CProgressiveConfig::GetDNSLookup()
	///     CProgressiveConfig::AddEndpoint()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetEndpoint()
	///     CProgressiveConfig::SetPingInterval()
	///     CProgressiveConfig::GetPingInterval()
	/// Test with a new instance for each test iteration.
	/// </summary>
	TEST_F(ProgressiveConfig, SetGet1Test)
	{
		f_SetGet1Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetHasConfigFile()
	///     CProgressiveConfig::HasConfigFile()
	///     CProgressiveConfig::SetActive()
	///     CProgressiveConfig::GetActive()
	///     CProgressiveConfig::SetDNSLookup()
	///     CProgressiveConfig::GetDNSLookup()
	///     CProgressiveConfig::AddEndpoint()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetEndpoint()
	///     CProgressiveConfig::SetPingInterval()
	///     CProgressiveConfig::GetPingInterval()
	/// Test with both existing and new instances.
	/// </summary>
	TEST_F(ProgressiveConfig, SetGet2Test)
	{
		f_SetGet2Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetHasConfigFile()
	///     CProgressiveConfig::HasConfigFile()
	///     CProgressiveConfig::SetActive()
	///     CProgressiveConfig::GetActive()
	///     CProgressiveConfig::SetDNSLookup()
	///     CProgressiveConfig::GetDNSLookup()
	///     CProgressiveConfig::AddEndpoint()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetEndpoint()
	///     CProgressiveConfig::SetPingInterval()
	///     CProgressiveConfig::GetPingInterval()
	/// Test with both existing and new instances, using a copy constructor before setting the last endpoint.
	/// This test is for the m_haveDefaultEndpoints member.
	/// </summary>
	TEST_F(ProgressiveConfig, SetGet2Test_Copy)
	{
		f_SetGet2Test_Copy();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CProgressiveConfig::SetHasConfigFile()
	///     CProgressiveConfig::HasConfigFile()
	///     CProgressiveConfig::SetActive()
	///     CProgressiveConfig::GetActive()
	///     CProgressiveConfig::SetDNSLookup()
	///     CProgressiveConfig::GetDNSLookup()
	///     CProgressiveConfig::AddEndpoint()
	///     CProgressiveConfig::GetEndpointCount()
	///     CProgressiveConfig::GetEndpoint()
	///     CProgressiveConfig::SetPingInterval()
	///     CProgressiveConfig::GetPingInterval()
	/// Test with both existing and new instances, using an assigment operator before setting the last endpoint.
	/// This test is for the m_haveDefaultEndpoints member.
	/// </summary>
	TEST_F(ProgressiveConfig, SetGet2Test_Assignment)
	{
		f_SetGet2Test_Assignment();
	}
	const string ProgressiveConfig::m_endpointsExpected[] = { _T("onelinkdiscossl:61614"), _T("onelinkdisco:61613") };
}
