#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveConfigFile.h"

// Sentinel includes.
#include "ProcessDump/OperatingSystem.h"
#include "ProgressiveProtocol/ProgressiveConfig.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "UnitTestingXmlLite.h"
#include "DirectoryManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a test method using the member test index.
#define AssertResult3m(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2m(result, expected) AssertResult3m(result, expected, _T(#result))

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, _T(#result))

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, _T(#result))

	TEST_CLASS(ProgressiveConfigFile)
	{
		// Default input(s).
		static const LPCTSTR m_fileName;
		static const LPCTSTR m_fileName2;
		static const CFile::OpenFlags m_fileOpenFlags = CFile::modeRead;

		static const DWORD m_pingIntervalMinimum = 5000;
		static const DWORD m_pingIntervalMaximum = DWORD_MAX;
		static const DWORD m_pingIntervalDefault = 15000;
		static const DWORD m_pingIntervalNotDefault = m_pingIntervalDefault + 1; // A valid value that is not the default value.

		// Default expected result(s).
		static const bool m_hasConfigFileExpected = false;
		static const bool m_activeExpected = false;
		static const bool m_dnsLookupExpected = true;
		static const int m_endpointCountExpected = 2;
		static const CString m_endpointsExpected[m_endpointCountExpected];
		static const DWORD m_pingIntervalExpected = m_pingIntervalDefault;

		// Class member(s) used by test (data) methods.
		static const CStringW m_beginRootElement;
		static const CStringW m_endRootElement;
		static const CStringW m_dataElementName;
		int m_testIndex;

		// Test data attribute name(s).
		static const LPCWSTR TestData_ContentName;
		static const LPCWSTR TestData_ValueName;
		static const LPCWSTR TestData_ValidName;
		/// <summary>
		/// Initialize and/or test static class members.
		/// </summary>
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{

			// Test default value(s).
			Assert::IsTrue(m_pingIntervalMinimum == CProgressiveConfig::MINIMUM_PING_INTERVAL, _T("m_pingIntervalMinimum"));
			Assert::IsTrue(m_pingIntervalMaximum == CProgressiveConfig::MAXIMUM_PING_INTERVAL, _T("m_pingIntervalMaximum"));
			Assert::IsTrue(m_pingIntervalDefault == CProgressiveConfig::DEFAULT_PING_INTERVAL, _T("m_pingIntervalDefault"));

			Assert::IsTrue(m_pingIntervalNotDefault >= m_pingIntervalMinimum, _T("m_pingIntervalNotDefault >= m_pingIntervalMinimum"));
			Assert::IsTrue(m_pingIntervalNotDefault <= m_pingIntervalMaximum, _T("m_pingIntervalNotDefault <= m_pingIntervalMaximum"));
			Assert::IsTrue(m_pingIntervalNotDefault != m_pingIntervalDefault, _T("m_pingIntervalNotDefault != m_pingIntervalDefault"));
		}

		/// <summary>
		/// Cleanup static class member(s).
		/// </summary>
		TEST_CLASS_CLEANUP(ClassCleanup)
		{
			// Class cleanup code.
		}

		/// <summary>
		/// Initialize class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Test method initialization code.
			m_testIndex = 0;
		}

		/// <summary>
		/// Cleanup class member(s) before each test method is run.
		/// </summary>
		TEST_METHOD_CLEANUP(Cleanup)
		{
			// Test method cleanup code.
		}

		static DWORD GetPingIntervalExpected(bool valid, DWORD value)
		{
			DWORD valueExpected(m_pingIntervalDefault);

			if (valid)
			{
				if (m_pingIntervalMinimum <= value && value <= m_pingIntervalMaximum)
				{
					valueExpected = value;
				}
			}

			return valueExpected;
		}

		static CStringW DataElement(LPCWSTR attributesString)
		{
			CStringW dataElement(CStringW(L"<") + m_dataElementName + L" " + attributesString + L"/>");
			return dataElement;
		}

		void AssertEqual(const CProgressiveConfigFile &obj,
			bool hasConfigFileExpected,
			bool activeExpected,
			bool dnsLookupExpected,
			DWORD pingIntervalExpected,
			const CString endpointsExpected[],
			int endpointCountExpected,
			int testIndex,
			LPCTSTR functionName,
			int lineNumber,
			const int* extraIndex = NULL)
		{
			
			const CProgressiveConfig* configObj(obj.GetProgressiveConfigCopy());
			AssertResult2h(configObj->HasConfigFile(), hasConfigFileExpected);
			AssertResult2h(configObj->GetActive(), activeExpected);
			AssertResult2h(configObj->GetDNSLookup(), dnsLookupExpected);
			AssertResult2h(configObj->GetPingInterval(), pingIntervalExpected);

			AssertResult2h(configObj->GetEndpointCount(), endpointCountExpected);
			CString emptyString;
			AssertResult2h(configObj->GetEndpoint(endpointCountExpected), emptyString);

			for (int index = 0; index < endpointCountExpected; ++index)
			{
				CString resultName;
				resultName.Format(_T("obj.GetEndpoint(index=%d)"), index);
				AssertResult3h(configObj->GetEndpoint(index), endpointsExpected[index], resultName);
			}

			// Cleanup.
			delete configObj;
		}

		void AssertEqual(const CProgressiveConfig &obj,
			bool hasConfigFileExpected,
			bool activeExpected,
			bool dnsLookupExpected,
			DWORD pingIntervalExpected,
			const CString endpointsExpected[],
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
			CString emptyString;
			AssertResult2h(obj.GetEndpoint(endpointCountExpected), emptyString);

			for (int index = 0; index < endpointCountExpected; ++index)
			{
				CString resultName;
				resultName.Format(_T("obj.GetEndpoint(index=%d)"), index);
				AssertResult3h(obj.GetEndpoint(index), endpointsExpected[index], resultName);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::GetPingInterval()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		/// Code coverage:
		///     CProgressiveConfigFile::~CProgressiveConfigFile()
		/// </summary>
		TEST_METHOD(CProgressiveConfigFileTest)
		{
			// Setup.

			// Perform operation(s) to be tested.
			const CProgressiveConfigFile obj;

			// Test(s).
			AssertEqual(obj,
				m_hasConfigFileExpected,
				m_activeExpected,
				m_dnsLookupExpected,
				m_pingIntervalExpected,
				m_endpointsExpected,
				m_endpointCountExpected,
				0, __TFUNCTION__, __LINE__);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::GetActive()
		/// Test with fileName "D:\\SentinelData\\ProgressiveProtocolConfig.txt"
		/// </summary>
		TEST_METHOD(Start_ActiveTest)
		{
			CDirectoryManager::MockInstance();

			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that does not set RequestButtonEnabled.

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "Active=cd";
			const char content3a[] = "Active=";
			const char content3b[] = "Active= ";
			const char content3c[] = "Active=   ";

			// Content that is close or equivalent to "IgnoreUnmappedPackages=false\n".
			const char content4[] = "Active=0";
			const char content5[] = "Active=F";
			const char content6[] = "Active=False";
			const char content7[] = "Active=fals";
			const char content8[] = "Active=falsE";
			const char content9[] = "Active=falsex";
			const char content10[] = "Active=false";
			const char content11[] = " Active = false ";
			const char content12[] = "   Active   =   false   ";
			const char content13[] = "Active=false\n";
			const char content14[] = "Active=false\n" "Active=false\n";

			// Content that is close or equivalent to "Active=true\n".
			const char content15[] = "Active=1";
			const char content16[] = "Active=T";
			const char content17[] = "Active=True";
			const char content18[] = "Active=tru";
			const char content19[] = "Active=truE";
			const char content20[] = "Active=truex";
			const char content21[] = "Active=true";
			const char content22[] = " Active = true ";
			const char content23[] = "   Active   =   true   ";
			const char content24[] = "Active=true\n";
			const char content25[] = "Active=true\n" "Active=true\n";

			// Content that sets Active to one value and then to another.
			const char content26[] = "Active=false\n" "Active=true\n";
			const char content27[] = "Active=true\n" "Active=false\n";
			const char content28[] = "Active=false\n" "Active=tru\n";
			const char content29[] = "Active=true\n" "Active=fals\n";

			// Content that sets Active to false and then test values that are close or equivalent to "true".
			const char content30[] = "Active=false\n" "Active=1";
			const char content31[] = "Active=false\n" "Active=T";
			const char content32[] = "Active=false\n" "Active=True";
			const char content33[] = "Active=false\n" "Active=tru";
			const char content34[] = "Active=false\n" "Active=truE";
			const char content35[] = "Active=false\n" "Active=truex";
			const char content36[] = "Active=false\n" "Active=true";
			const char content37[] = "Active=false\n" " Active = true ";
			const char content38[] = "Active=false\n" "   Active   =   true   ";
			const char content39[] = "Active=false\n" "Active=true\n";

			// Content that sets Active to true and then test values that are close or equivalent to "false".
			const char content40[] = "Active=true\n" "Active=0";
			const char content41[] = "Active=true\n" "Active=F";
			const char content42[] = "Active=true\n" "Active=False";
			const char content43[] = "Active=true\n" "Active=fals";
			const char content44[] = "Active=true\n" "Active=falsE";
			const char content45[] = "Active=true\n" "Active=falsex";
			const char content46[] = "Active=true\n" "Active=false";
			const char content47[] = "Active=true\n" " Active = false ";
			const char content48[] = "Active=true\n" "   Active   =   false   ";
			const char content49[] = "Active=true\n" "Active=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3a)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3b)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3c)-1, false, false, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, false, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, true, false, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::RefreshConfig()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::GetActive()
		/// Test with fileName "D:\\SentinelData\\ProgressiveProtocolConfig.txt"
		/// </summary>
		TEST_METHOD(RefreshConfig_ActiveTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that does not set RequestButtonEnabled.

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "Active=cd";
			const char content3a[] = "Active=";
			const char content3b[] = "Active= ";
			const char content3c[] = "Active=   ";

			// Content that is close or equivalent to "IgnoreUnmappedPackages=false\n".
			const char content4[] = "Active=0";
			const char content5[] = "Active=F";
			const char content6[] = "Active=False";
			const char content7[] = "Active=fals";
			const char content8[] = "Active=falsE";
			const char content9[] = "Active=falsex";
			const char content10[] = "Active=false";
			const char content11[] = " Active = false ";
			const char content12[] = "   Active   =   false   ";
			const char content13[] = "Active=false\n";
			const char content14[] = "Active=false\n" "Active=false\n";

			// Content that is close or equivalent to "Active=true\n".
			const char content15[] = "Active=1";
			const char content16[] = "Active=T";
			const char content17[] = "Active=True";
			const char content18[] = "Active=tru";
			const char content19[] = "Active=truE";
			const char content20[] = "Active=truex";
			const char content21[] = "Active=true";
			const char content22[] = " Active = true ";
			const char content23[] = "   Active   =   true   ";
			const char content24[] = "Active=true\n";
			const char content25[] = "Active=true\n" "Active=true\n";

			// Content that sets Active to one value and then to another.
			const char content26[] = "Active=false\n" "Active=true\n";
			const char content27[] = "Active=true\n" "Active=false\n";
			const char content28[] = "Active=false\n" "Active=tru\n";
			const char content29[] = "Active=true\n" "Active=fals\n";

			// Content that sets Active to false and then test values that are close or equivalent to "true".
			const char content30[] = "Active=false\n" "Active=1";
			const char content31[] = "Active=false\n" "Active=T";
			const char content32[] = "Active=false\n" "Active=True";
			const char content33[] = "Active=false\n" "Active=tru";
			const char content34[] = "Active=false\n" "Active=truE";
			const char content35[] = "Active=false\n" "Active=truex";
			const char content36[] = "Active=false\n" "Active=true";
			const char content37[] = "Active=false\n" " Active = true ";
			const char content38[] = "Active=false\n" "   Active   =   true   ";
			const char content39[] = "Active=false\n" "Active=true\n";

			// Content that sets Active to true and then test values that are close or equivalent to "false".
			const char content40[] = "Active=true\n" "Active=0";
			const char content41[] = "Active=true\n" "Active=F";
			const char content42[] = "Active=true\n" "Active=False";
			const char content43[] = "Active=true\n" "Active=fals";
			const char content44[] = "Active=true\n" "Active=falsE";
			const char content45[] = "Active=true\n" "Active=falsex";
			const char content46[] = "Active=true\n" "Active=false";
			const char content47[] = "Active=true\n" " Active = false ";
			const char content48[] = "Active=true\n" "   Active   =   false   ";
			const char content49[] = "Active=true\n" "Active=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3a)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3b)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3c)-1, false, false, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, false, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, false, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content40), sizeof(content40)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content41), sizeof(content41)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content42), sizeof(content42)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content43), sizeof(content43)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content44), sizeof(content44)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content45), sizeof(content45)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content46), sizeof(content46)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content47), sizeof(content47)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content48), sizeof(content48)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content49), sizeof(content49)-1, true, false, 1 },
			};

			COperatingSystemTest operatingSystemA(fileName,
				nOpenFlags,
				NULL,
				0);
			DWORD totalFileCreateCountExpectedA(0);
			DWORD totalFileOpenCountExpectedA(0);
			DWORD totalFileFinalizeCountExpectedA(0);

			// Setup. Perform each test with the same instance.
			CProgressiveConfigFile objA(operatingSystemA);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int extraIndex = 0; extraIndex < 2; ++extraIndex)
				{
					// Extra input(s) -- vary for each set of testIndex loops.
					bool refresh = bool(extraIndex & 1);

					// Setup.
					// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
					COperatingSystemTest operatingSystemB(fileName,
						nOpenFlags,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileContent,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileLength);

					// Setup. Perform each test with a new object.
					CProgressiveConfigFile objB(operatingSystemB);
					objB.Start();

					// Verify setup.
					AssertEqual(objB,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_hasConfigFileExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// More setup.
					operatingSystemA.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);
					operatingSystemB.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);

					// Perform operation(s) to be tested.
					// See note above about the ASSERT for lpMemSource.
					objA.RefreshConfig(refresh);
					objB.RefreshConfig(refresh);

					// Test(s).
					AssertEqual(objA,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						TEST_DATA[testIndex].m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);
					AssertEqual(objB,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						TEST_DATA[testIndex].m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystemA.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemA.m_file1.m_openCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_closeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalizeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					AssertResult2x(operatingSystemB.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemB.m_file1.m_openCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_closeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalizeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					// Verify that the file was not changed.
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemA.m_file1.m_finalFileContents,
						operatingSystemA.m_file1.m_finalFileLength),
						_T("IsEqual - error"));
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemB.m_file1.m_finalFileContents,
						operatingSystemB.m_file1.m_finalFileLength),
						_T("IsEqual - error"));

					// Check for unexpected accesses.
					totalFileCreateCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileOpenCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileFinalizeCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;

					AssertResult2x(operatingSystemA.m_totalFileCreateCount, totalFileCreateCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileOpenCount, totalFileOpenCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileCloseCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_totalFileFinalizeCount, totalFileFinalizeCountExpectedA);

					AssertResult2x(operatingSystemB.m_totalFileCreateCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileOpenCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileCloseCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_totalFileFinalizeCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				}
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::GetDNSLookup()
		/// Test with fileName "C:\\ProgressiveProtocolConfig.txt"
		/// </summary>
		TEST_METHOD(Start_DNSLookup2Test)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("C:\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			// Expected result(s) -- independent of test iteration
			const DWORD totalFileCreateCountExpected(2); // Expect number of creates == opens == finalizes.

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_dnsLookupExpected;
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "DNSLookup=cd";
			const char content3a[] = "DNSLookup=";
			const char content3b[] = "DNSLookup= ";
			const char content3c[] = "DNSLookup=   ";

			// Content that is close or equivalent to "DNSLookup=false\n".
			const char content4[] = "DNSLookup=0";
			const char content5[] = "DNSLookup=F";
			const char content6[] = "DNSLookup=False";
			const char content7[] = "DNSLookup=fals";
			const char content8[] = "DNSLookup=falsE";
			const char content9[] = "DNSLookup=falsex";
			const char content10[] = "DNSLookup=false";
			const char content11[] = " DNSLookup = false ";
			const char content12[] = "   DNSLookup   =   false   ";
			const char content13[] = "DNSLookup=false\n";
			const char content14[] = "DNSLookup=false\n" "DNSLookup=false\n";

			// Content that is close or equivalent to "DNSLookup=true\n".
			const char content15[] = "DNSLookup=1";
			const char content16[] = "DNSLookup=T";
			const char content17[] = "DNSLookup=True";
			const char content18[] = "DNSLookup=tru";
			const char content19[] = "DNSLookup=truE";
			const char content20[] = "DNSLookup=truex";
			const char content21[] = "DNSLookup=true";
			const char content22[] = " DNSLookup = true ";
			const char content23[] = "   DNSLookup   =   true   ";
			const char content24[] = "DNSLookup=true\n";
			const char content25[] = "DNSLookup=true\n" "DNSLookup=true\n";

			// Content that sets DNSLookup to one value and then to another.
			const char content26[] = "DNSLookup=false\n" "DNSLookup=true\n";
			const char content27[] = "DNSLookup=true\n" "DNSLookup=false\n";
			const char content28[] = "DNSLookup=false\n" "DNSLookup=tru\n";
			const char content29[] = "DNSLookup=true\n" "DNSLookup=fals\n";

			// Content that sets DNSLookup to true and then test values that are close or equivalent to "false".
			const char content30[] = "DNSLookup=true\n" "DNSLookup=0";
			const char content31[] = "DNSLookup=true\n" "DNSLookup=F";
			const char content32[] = "DNSLookup=true\n" "DNSLookup=False";
			const char content33[] = "DNSLookup=true\n" "DNSLookup=fals";
			const char content34[] = "DNSLookup=true\n" "DNSLookup=falsE";
			const char content35[] = "DNSLookup=true\n" "DNSLookup=falsex";
			const char content36[] = "DNSLookup=true\n" "DNSLookup=false";
			const char content37[] = "DNSLookup=true\n" " DNSLookup = false ";
			const char content38[] = "DNSLookup=true\n" "   DNSLookup   =   false   ";
			const char content39[] = "DNSLookup=true\n" "DNSLookup=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, true },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3)-1, false, true },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, false },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, true },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, true, true },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, false },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, false },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::GetDNSLookup()
		/// Test with fileName "D:\\SentinelData\\ProgressiveProtocolConfig.txt"
		/// </summary>
		TEST_METHOD(Start_DNSLookupTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "DNSLookup=cd";
			const char content3a[] = "DNSLookup=";
			const char content3b[] = "DNSLookup= ";
			const char content3c[] = "DNSLookup=   ";

			// Content that is close or equivalent to "DNSLookup=false\n".
			const char content4[] = "DNSLookup=0";
			const char content5[] = "DNSLookup=F";
			const char content6[] = "DNSLookup=False";
			const char content7[] = "DNSLookup=fals";
			const char content8[] = "DNSLookup=falsE";
			const char content9[] = "DNSLookup=falsex";
			const char content10[] = "DNSLookup=false";
			const char content11[] = " DNSLookup = false ";
			const char content12[] = "   DNSLookup   =   false   ";
			const char content13[] = "DNSLookup=false\n";
			const char content14[] = "DNSLookup=false\n" "DNSLookup=false\n";

			// Content that is close or equivalent to "DNSLookup=true\n".
			const char content15[] = "DNSLookup=1";
			const char content16[] = "DNSLookup=T";
			const char content17[] = "DNSLookup=True";
			const char content18[] = "DNSLookup=tru";
			const char content19[] = "DNSLookup=truE";
			const char content20[] = "DNSLookup=truex";
			const char content21[] = "DNSLookup=true";
			const char content22[] = " DNSLookup = true ";
			const char content23[] = "   DNSLookup   =   true   ";
			const char content24[] = "DNSLookup=true\n";
			const char content25[] = "DNSLookup=true\n" "DNSLookup=true\n";

			// Content that sets DNSLookup to one value and then to another.
			const char content26[] = "DNSLookup=false\n" "DNSLookup=true\n";
			const char content27[] = "DNSLookup=true\n" "DNSLookup=false\n";
			const char content28[] = "DNSLookup=false\n" "DNSLookup=tru\n";
			const char content29[] = "DNSLookup=true\n" "DNSLookup=fals\n";

			// Content that sets DNSLookup to true and then test values that are close or equivalent to "false".
			const char content30[] = "DNSLookup=true\n" "DNSLookup=0";
			const char content31[] = "DNSLookup=true\n" "DNSLookup=F";
			const char content32[] = "DNSLookup=true\n" "DNSLookup=False";
			const char content33[] = "DNSLookup=true\n" "DNSLookup=fals";
			const char content34[] = "DNSLookup=true\n" "DNSLookup=falsE";
			const char content35[] = "DNSLookup=true\n" "DNSLookup=falsex";
			const char content36[] = "DNSLookup=true\n" "DNSLookup=false";
			const char content37[] = "DNSLookup=true\n" " DNSLookup = false ";
			const char content38[] = "DNSLookup=true\n" "   DNSLookup   =   false   ";
			const char content39[] = "DNSLookup=true\n" "DNSLookup=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3)-1, false, true, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, false, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, false, 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					m_pingIntervalExpected,
					m_endpointsExpected,
					m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::RefreshConfig()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::GetDNSLookup()
		/// Test with fileName "D:\\SentinelData\\ProgressiveProtocolConfig.txt"
		/// </summary>
		TEST_METHOD(RefreshConfig_DNSLookupTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";
			const char content2[] = "ab=cd";
			const char content3[] = "DNSLookup=cd";
			const char content3a[] = "DNSLookup=";
			const char content3b[] = "DNSLookup= ";
			const char content3c[] = "DNSLookup=   ";

			// Content that is close or equivalent to "DNSLookup=false\n".
			const char content4[] = "DNSLookup=0";
			const char content5[] = "DNSLookup=F";
			const char content6[] = "DNSLookup=False";
			const char content7[] = "DNSLookup=fals";
			const char content8[] = "DNSLookup=falsE";
			const char content9[] = "DNSLookup=falsex";
			const char content10[] = "DNSLookup=false";
			const char content11[] = " DNSLookup = false ";
			const char content12[] = "   DNSLookup   =   false   ";
			const char content13[] = "DNSLookup=false\n";
			const char content14[] = "DNSLookup=false\n" "DNSLookup=false\n";

			// Content that is close or equivalent to "DNSLookup=true\n".
			const char content15[] = "DNSLookup=1";
			const char content16[] = "DNSLookup=T";
			const char content17[] = "DNSLookup=True";
			const char content18[] = "DNSLookup=tru";
			const char content19[] = "DNSLookup=truE";
			const char content20[] = "DNSLookup=truex";
			const char content21[] = "DNSLookup=true";
			const char content22[] = " DNSLookup = true ";
			const char content23[] = "   DNSLookup   =   true   ";
			const char content24[] = "DNSLookup=true\n";
			const char content25[] = "DNSLookup=true\n" "DNSLookup=true\n";

			// Content that sets DNSLookup to one value and then to another.
			const char content26[] = "DNSLookup=false\n" "DNSLookup=true\n";
			const char content27[] = "DNSLookup=true\n" "DNSLookup=false\n";
			const char content28[] = "DNSLookup=false\n" "DNSLookup=tru\n";
			const char content29[] = "DNSLookup=true\n" "DNSLookup=fals\n";

			// Content that sets DNSLookup to true and then test values that are close or equivalent to "false".
			const char content30[] = "DNSLookup=true\n" "DNSLookup=0";
			const char content31[] = "DNSLookup=true\n" "DNSLookup=F";
			const char content32[] = "DNSLookup=true\n" "DNSLookup=False";
			const char content33[] = "DNSLookup=true\n" "DNSLookup=fals";
			const char content34[] = "DNSLookup=true\n" "DNSLookup=falsE";
			const char content35[] = "DNSLookup=true\n" "DNSLookup=falsex";
			const char content36[] = "DNSLookup=true\n" "DNSLookup=false";
			const char content37[] = "DNSLookup=true\n" " DNSLookup = false ";
			const char content38[] = "DNSLookup=true\n" "   DNSLookup   =   false   ";
			const char content39[] = "DNSLookup=true\n" "DNSLookup=false\n";

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3a), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3b), sizeof(content3)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content3c), sizeof(content3)-1, false, true, 2 },

				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, false, 1 },

				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content16), sizeof(content16)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content17), sizeof(content17)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content18), sizeof(content18)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content19), sizeof(content19)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content20), sizeof(content20)-1, false, true, 2 },
				{ reinterpret_cast<const BYTE*>(content21), sizeof(content21)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content22), sizeof(content22)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content23), sizeof(content23)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content24), sizeof(content24)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content25), sizeof(content25)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content26), sizeof(content26)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content27), sizeof(content27)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content28), sizeof(content28)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content29), sizeof(content29)-1, true, true, 1 },

				{ reinterpret_cast<const BYTE*>(content30), sizeof(content30)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content31), sizeof(content31)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content32), sizeof(content32)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content33), sizeof(content33)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content34), sizeof(content34)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content35), sizeof(content35)-1, true, true, 1 },
				{ reinterpret_cast<const BYTE*>(content36), sizeof(content36)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content37), sizeof(content37)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content38), sizeof(content38)-1, true, false, 1 },
				{ reinterpret_cast<const BYTE*>(content39), sizeof(content39)-1, true, false, 1 },
			};

			COperatingSystemTest operatingSystemA(fileName,
				nOpenFlags,
				NULL,
				0);
			DWORD totalFileCreateCountExpectedA(0);
			DWORD totalFileOpenCountExpectedA(0);
			DWORD totalFileFinalizeCountExpectedA(0);

			// Setup. Perform each test with the same instance.
			CProgressiveConfigFile objA(operatingSystemA);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int extraIndex = 0; extraIndex < 2; ++extraIndex)
				{
					// Extra input(s) -- vary for each set of testIndex loops.
					bool refresh = bool(extraIndex & 1);

					// Setup.
					// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
					COperatingSystemTest operatingSystemB(fileName,
						nOpenFlags,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileContent,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileLength);

					// Setup. Perform each test with a new object.
					CProgressiveConfigFile objB(operatingSystemB);
					objB.Start();

					// Verify setup.
					AssertEqual(objB,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_hasConfigFileExpected,
						m_activeExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// More setup.
					operatingSystemA.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);
					operatingSystemB.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);

					// Perform operation(s) to be tested.
					// See note above about the ASSERT for lpMemSource.
					objA.RefreshConfig(refresh);
					objB.RefreshConfig(refresh);

					// Test(s).
					AssertEqual(objA,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						m_activeExpected,
						TEST_DATA[testIndex].m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);
					AssertEqual(objB,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						m_activeExpected,
						TEST_DATA[testIndex].m_dnsLookupExpected,
						m_pingIntervalExpected,
						m_endpointsExpected,
						m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystemA.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemA.m_file1.m_openCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_closeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalizeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					AssertResult2x(operatingSystemB.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemB.m_file1.m_openCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_closeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalizeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					// Verify that the file was not changed.
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemA.m_file1.m_finalFileContents,
						operatingSystemA.m_file1.m_finalFileLength),
						_T("IsEqual - error"));
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemB.m_file1.m_finalFileContents,
						operatingSystemB.m_file1.m_finalFileLength),
						_T("IsEqual - error"));

					// Check for unexpected accesses.
					totalFileCreateCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileOpenCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileFinalizeCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;

					AssertResult2x(operatingSystemA.m_totalFileCreateCount, totalFileCreateCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileOpenCount, totalFileOpenCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileCloseCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_totalFileFinalizeCount, totalFileFinalizeCountExpectedA);

					AssertResult2x(operatingSystemB.m_totalFileCreateCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileOpenCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileCloseCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_totalFileFinalizeCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				}
			}
		}

		static CStringW PingIntervalTestData()
		{
			// Test data:
			// "content" is the content to be read from the configuration file.  Use XML numeric character references ("&#10;") for newlines.
			// "value" is the configuration value in the content, if the content contains a correctly formatted configuration value.
			// "valid" indicates if the content is expected to have a correctly formatted configuration value.
			//     "valid" may be true even if the value is not in the valid range for the configuration field.
			const CStringW testData =
				m_beginRootElement +

				// Content that isn't even close.
				DataElement(L"content='' value='0' valid='false'") +
				DataElement(L"content='ab' value='0' valid='false'") +
				DataElement(L"content='ab=cd' value='0' valid='false'") +
				DataElement(L"content='PingInterval=cd' value='0' valid='false'") +
				DataElement(L"content='PingInterval=' value='0' valid='false'") +
				DataElement(L"content='PingInterval= ' value='0' valid='false'") +
				DataElement(L"content='PingInterval=   ' value='0' valid='false'") +

				// Content that is close to being valid or are valid.
				DataElement(L"content='PingInterval=0x9999' value='9999' valid='false'") +
				DataElement(L"content='PingInterval=9999.' value='9999' valid='false'") +
				DataElement(L"content='PingInterval=9999.0' value='9999' valid='false'") +
				DataElement(L"content='PingInterval=9999e0' value='9999' valid='false'") +
				DataElement(L"content='PingInterval=+9999' value='9999' valid='true'") +
				DataElement(L"content='PingInterval=5001' value='5001' valid='true'") +
				DataElement(L"content=' PingInterval = 1002 ' value='1002' valid='true'") +
				DataElement(L"content='   PingInterval   =   1003   ' value='1003' valid='true'") +
				DataElement(L"content='PingInterval=1004&#10;' value='1004' valid='true'") +
				DataElement(L"content='PingInterval=1005&#10;PingInterval=1005&#10;' value='1005' valid='true'") +
				DataElement(L"content='   PingInterval   =   1006   &#10;   PingInterval   =   1007   &#10;' value='1007' valid='true'") +
				DataElement(L"content='   PingInterval   =   1008   &#10;   PingInterval   =   1009.   &#10;' value='1009' valid='true'") +
				DataElement(L"content='   PingInterval   =   1010   &#10;   PingInterval   =   1011 0   &#10;' value='1010' valid='true'") +
				DataElement(L"content='   PingInterval   =   1012   &#10;   PingInterval   =   1013   PingInterval   =   1014  &#10;' value='1014' valid='true'") +

				// Non-negative values that are valid DWORD values.
				DataElement(L"content='PingInterval=0' value='0' valid='true'") +
				DataElement(L"content='PingInterval=1' value='1' valid='true'") +
				DataElement(L"content='PingInterval=2' value='2' valid='true'") +
				DataElement(L"content='PingInterval=999' value='999' valid='true'") +
				DataElement(L"content='PingInterval=1000' value='1000' valid='true'") +
				DataElement(L"content='PingInterval=5001' value='5001' valid='true'") +
				DataElement(L"content='PingInterval=1234567890' value='1234567890' valid='true'") +
				DataElement(L"content='PingInterval=2147483647' value='2147483647' valid='true'") + // Maximum unsigned 31-bit value.
				DataElement(L"content='PingInterval=2147483648' value='2147483648' valid='true'") + // // One more than maximum unsigned 31-bit value.
				DataElement(L"content='PingInterval=4294967295' value='4294967295' valid='true'") + // Maximum unsigned 32-bit value.

				// Non-negative values that are not valid DWORD values.
				DataElement(L"content='PingInterval=4294967296' value='0' valid='false'") + // One more than maximum unsigned 32-bit value.
				DataElement(L"content='PingInterval=9999999999' value='0' valid='false'") +
				DataElement(L"content='PingInterval=18446744073709551615' value='0' valid='false'") + // Maximum unsigned 64-bit value.
				DataElement(L"content='PingInterval=99999999999999999999' value='0' valid='false'") +
				DataElement(L"content='PingInterval=9999999999999999999999999999999999999999' value='0' valid='false'") +

				// "Negative" values that are valid DWORD values.
				DataElement(L"content='PingInterval=-0' value='0' valid='true'") +
				DataElement(L"content='PingInterval=-1' value='4294967295' valid='true'") +
				DataElement(L"content='PingInterval=-2' value='4294967294' valid='true'") +
				DataElement(L"content='PingInterval=-999' value='4294966297' valid='true'") +
				DataElement(L"content='PingInterval=-1000' value='4294966296' valid='true'") +
				DataElement(L"content='PingInterval=-5001' value='4294962295' valid='true'") +
				DataElement(L"content='PingInterval=-1234567890' value='3060399406' valid='true'") +
				DataElement(L"content='PingInterval=-2147483647' value='2147483649' valid='true'") + // One more than minimum signed 31-bit value.
				DataElement(L"content='PingInterval=-2147483648' value='2147483648' valid='true'") + // Minimum signed 31-bit value.
				DataElement(L"content='PingInterval=-2147483649' value='2147483647' valid='true'") + // One less than maximum unsigned 31-bit value.
				DataElement(L"content='PingInterval=-4294966295' value='5001' valid='false'") + // Equivalent to 5001.
				DataElement(L"content='PingInterval=-4294966296' value='1000' valid='false'") + // Equivalent to 1000.
				DataElement(L"content='PingInterval=-4294966297' value='999' valid='false'") + // Equivalent to 999.
				DataElement(L"content='PingInterval=-4294967295' value='1' valid='false'") + // Maximum unsigned 32-bit value, negated.

				// Negative values that are not valid DWORD values.
				DataElement(L"content='PingInterval=-4294967296' value='0' valid='false'") + // One more than maximum unsigned 32-bit value, negated.
				DataElement(L"content='PingInterval=-9999999999' value='0' valid='false'") +
				DataElement(L"content='PingInterval=-18446744073709551615' value='0' valid='false'") + // Maximum unsigned 64-bit value.
				DataElement(L"content='PingInterval=-99999999999999999999' value='0' valid='false'") +
				DataElement(L"content='PingInterval=-9999999999999999999999999999999999999999' value='0' valid='false'") +

				// Content that sets one value and then maybe to another.
				DataElement(L"content='PingInterval=12345&#10;PingInterval=23456&#10;' value='23456' valid='true'") +
				DataElement(L"content='PingInterval=12346&#10;PingInterval=23457.&#10;' value='12346' valid='true'") +
				DataElement(L"content='PingInterval=12347&#10;PingInterval=23458 0&#10;' value='12347' valid='true'") +
				DataElement(L"content='PingInterval=12348&#10;PingInterval=23459&#10;' value='23459' valid='true'") +

				// More content.
				DataElement(L"content='PingInterval=01234' value='1234' valid='true'") + // Test that a value with a leading zero is not treated as a valid octal value.
				DataElement(L"content='PingInterval=09999' value='9999' valid='true'") + // Test that a value with a leading zero is not treated as an invalid octal value.

				m_endRootElement;

			return testData;
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretDWORD()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::GetPingInterval()
		/// </summary>
		TEST_METHOD(Start_PingIntervalTest)
		{
			CXmlReaderTest testReader(PingIntervalTestData(), &m_testIndex);

			for (; testReader.MoveToNextElement(m_dataElementName); ++m_testIndex)
			{
				// Setup. Get test data.
				CStringA content(testReader.GetNextAttributeAsString(TestData_ContentName));
				DWORD value(testReader.GetNextAttributeAsDWORD(TestData_ValueName));
				bool valid(testReader.GetNextAttributeAsBool(TestData_ValidName));
				DWORD pingIntervalExpected(GetPingIntervalExpected(valid, value));

				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(m_fileName,
					m_fileOpenFlags,
					reinterpret_cast<const BYTE *>(LPCSTR(content)),
					content.GetLength());
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				unique_ptr<CProgressiveConfig> progConfig(obj.GetProgressiveConfigCopy());
				AssertResult2m(progConfig->GetPingInterval(), pingIntervalExpected);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::RefreshConfig()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretDWORD()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::GetPingInterval()
		/// </summary>
		TEST_METHOD(RefreshConfig_PingIntervalTest)
		{
			CXmlReaderTest testReader(PingIntervalTestData(), &m_testIndex);
			CStringA lastContent;
			DWORD lastPingIntervalExpected(m_pingIntervalExpected);

			COperatingSystemTest operatingSystemA(m_fileName,
				m_fileOpenFlags,
				reinterpret_cast<const BYTE *>(LPCSTR(lastContent)),
				lastContent.GetLength());

			// Setup. Perform each test with the same instance.
			CProgressiveConfigFile objA(operatingSystemA);

			for (; testReader.MoveToNextElement(m_dataElementName); ++m_testIndex)
			{
				// Setup. Get test data.
				CStringA content(testReader.GetNextAttributeAsString(L"content"));
				DWORD value(testReader.GetNextAttributeAsDWORD(L"value"));
				bool valid(testReader.GetNextAttributeAsBool(L"valid"));
				DWORD pingIntervalExpected(GetPingIntervalExpected(valid, value));

				for (int extraIndex = 0; extraIndex < 2; ++extraIndex)
				{
					// Extra input(s) -- vary for each set of testIndex loops.
					bool refresh = bool(extraIndex & 1);

					// Setup.
					// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
					COperatingSystemTest operatingSystemB(m_fileName,
						m_fileOpenFlags,
						reinterpret_cast<const BYTE *>(LPCSTR(lastContent)),
						lastContent.GetLength());

					// Setup. Perform each test with a new object.
					CProgressiveConfigFile objB(operatingSystemB);
					objB.Start();

					// Verify setup.
					unique_ptr<CProgressiveConfig> progConfigS(objB.GetProgressiveConfigCopy());
					AssertResult2m(progConfigS->GetPingInterval(), lastPingIntervalExpected);

					// More setup.
					operatingSystemA.ChangeFileContents(reinterpret_cast<const BYTE *>(LPCSTR(content)),
						content.GetLength());
					operatingSystemB.ChangeFileContents(reinterpret_cast<const BYTE *>(LPCSTR(content)),
						content.GetLength());

					// Perform operation(s) to be tested.
					// See note above about the ASSERT for lpMemSource.
					objA.RefreshConfig(refresh);
					objB.RefreshConfig(refresh);

					// Test(s).
					unique_ptr<CProgressiveConfig> progConfigA(objA.GetProgressiveConfigCopy());
					unique_ptr<CProgressiveConfig> progConfigB(objB.GetProgressiveConfigCopy());
					AssertResult2m(progConfigA->GetPingInterval(), pingIntervalExpected);
					AssertResult2m(progConfigB->GetPingInterval(), pingIntervalExpected);
				}
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		/// </summary>
		TEST_METHOD(Start_AddEndpointTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";

			const char content2[] = "ab=cd";

			const char content3[] = "Endpoint=";
			const CString endpoints3[] = { _T(""), };

			const char content4[] = "Endpoint= ";
			const CString endpoints4[] = { _T(""), };

			const char content5[] = "Endpoint=   ";
			const CString endpoints5[] = { _T(""), };

			const char content6[] = "Endpoint=   ab";
			const CString endpoints6[] = { _T("ab"), };

			const char content7[] = "Endpoint=cd   ";
			const CString endpoints7[] = { _T("cd"), };

			const char content8[] = "Endpoint=   ef   ";
			const CString endpoints8[] = { _T("ef"), };

			const char content9[] = "Endpoint=   ef   gh   ";
			const CString endpoints9[] = { _T("ef   gh"), };

			const char content10[] = " Endpoint = abc \n" " Endpoint = def ";
			const CString endpoints10[] = { _T("abc"), _T("def"), };

			const char content11[] = "   Endpoint   =  ghi   \n" "   Endpoint   =   jkL   \n";
			const CString endpoints11[] = { _T("ghi"), _T("jkL"), };

			const char content12[] = "Endpoint=abc.defghi.com:12345\n" "Endpoint=tuvw.net:23456\n" "Endpoint=xyz.gov:34567\n";
			const CString endpoints12[] = { _T("abc.defghi.com:12345"), _T("tuvw.net:23456"), _T("xyz.gov:34567"), };

			// All displayable ASCII characters, with leading and imbedded space and imbedded quote.
			const char content13[] =
				"Endpoint=abc\n"
				"Endpoint= !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n"
				"Endpoint=def\n";
			const CString endpoints13[] = {
				_T("abc"),
				_T("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
				_T("def"), };

			// All displayable ASCII characters, with imbedded space and trailing quote.
			const char content14[] =
				"Endpoint=dddabc\n"
				"Endpoint=!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\"\n"
				"Endpoint=def\n";
			const CString endpoints14[] = {
				_T("dddabc"),
				_T("!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\""),
				_T("def"), };

			// 300 characters.
			const char content15[] =
				"Endpoint=mno\n"
				"Endpoint=" // Value follows.
				"d23456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789."
				"123456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789."
				"123456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789.\n"
				"Endpoint=pqr\n";
			const CString endpoints15[] = {
				_T("mno"),
				_T("d23456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.") // No comma.
				_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.") // No comma.
				_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789."),
				_T("pqr"),
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, true, endpoints3, _countof(endpoints3), 1 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, true, endpoints4, _countof(endpoints4), 1 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, true, endpoints5, _countof(endpoints5), 1 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, true, endpoints6, _countof(endpoints6), 1 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, true, endpoints7, _countof(endpoints7), 1 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, true, endpoints8, _countof(endpoints8), 1 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, true, endpoints9, _countof(endpoints9), 1 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, endpoints10, _countof(endpoints10), 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, endpoints11, _countof(endpoints11), 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, endpoints12, _countof(endpoints12), 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, endpoints13, _countof(endpoints13), 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, endpoints14, _countof(endpoints14), 1 },
				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, true, endpoints15, _countof(endpoints15), 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					m_activeExpected,
					m_dnsLookupExpected,
					m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpected,
					TEST_DATA[testIndex].m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::CProgressiveConfigFile()
		///     CProgressiveConfigFile::SetDefaults()
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::RefreshConfig()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		/// </summary>
		TEST_METHOD(RefreshConfig_AddEndpointTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			// Content that isn't even close.
			const char content1[] = "ab";

			const char content2[] = "ab=cd";

			const char content3[] = "Endpoint=";
			const CString endpoints3[] = { _T(""), };

			const char content4[] = "Endpoint= ";
			const CString endpoints4[] = { _T(""), };

			const char content5[] = "Endpoint=   ";
			const CString endpoints5[] = { _T(""), };

			const char content6[] = "Endpoint=   ab";
			const CString endpoints6[] = { _T("ab"), };

			const char content7[] = "Endpoint=cd   ";
			const CString endpoints7[] = { _T("cd"), };

			const char content8[] = "Endpoint=   ef   ";
			const CString endpoints8[] = { _T("ef"), };

			const char content9[] = "Endpoint=   ef   gh   ";
			const CString endpoints9[] = { _T("ef   gh"), };

			const char content10[] = " Endpoint = abc \n" " Endpoint = def ";
			const CString endpoints10[] = { _T("abc"), _T("def"), };

			const char content11[] = "   Endpoint   =  ghi   \n" "   Endpoint   =   jkL   \n";
			const CString endpoints11[] = { _T("ghi"), _T("jkL"), };

			const char content12[] = "Endpoint=abc.defghi.com:12345\n" "Endpoint=tuvw.net:23456\n" "Endpoint=xyz.gov:34567\n";
			const CString endpoints12[] = { _T("abc.defghi.com:12345"), _T("tuvw.net:23456"), _T("xyz.gov:34567"), };

			// All displayable ASCII characters, with leading and imbedded space and imbedded quote.
			const char content13[] =
				"Endpoint=abc\n"
				"Endpoint= !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n"
				"Endpoint=def\n";
			const CString endpoints13[] = {
				_T("abc"),
				_T("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
				_T("def"), };

			// All displayable ASCII characters, with imbedded space and trailing quote.
			const char content14[] =
				"Endpoint=dddabc\n"
				"Endpoint=!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\"\n"
				"Endpoint=def\n";
			const CString endpoints14[] = {
				_T("dddabc"),
				_T("!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\""),
				_T("def"), };

			// 300 characters.
			const char content15[] =
				"Endpoint=mno\n"
				"Endpoint=" // Value follows.
				"d23456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789."
				"123456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789."
				"123456789.123456789.123456789.123456789.123456789." "123456789.123456789.123456789.123456789.123456789.\n"
				"Endpoint=pqr\n";
			const CString endpoints15[] = {
				_T("mno"),
				_T("d23456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.") // No comma.
				_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.") // No comma.
				_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789."),
				_T("pqr"),
			};

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, false, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, true, endpoints3, _countof(endpoints3), 1 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, true, endpoints4, _countof(endpoints4), 1 },
				{ reinterpret_cast<const BYTE*>(content5), sizeof(content5)-1, true, endpoints5, _countof(endpoints5), 1 },
				{ reinterpret_cast<const BYTE*>(content6), sizeof(content6)-1, true, endpoints6, _countof(endpoints6), 1 },
				{ reinterpret_cast<const BYTE*>(content7), sizeof(content7)-1, true, endpoints7, _countof(endpoints7), 1 },
				{ reinterpret_cast<const BYTE*>(content8), sizeof(content8)-1, true, endpoints8, _countof(endpoints8), 1 },
				{ reinterpret_cast<const BYTE*>(content9), sizeof(content9)-1, true, endpoints9, _countof(endpoints9), 1 },
				{ reinterpret_cast<const BYTE*>(content10), sizeof(content10)-1, true, endpoints10, _countof(endpoints10), 1 },
				{ reinterpret_cast<const BYTE*>(content11), sizeof(content11)-1, true, endpoints11, _countof(endpoints11), 1 },
				{ reinterpret_cast<const BYTE*>(content12), sizeof(content12)-1, true, endpoints12, _countof(endpoints12), 1 },
				{ reinterpret_cast<const BYTE*>(content13), sizeof(content13)-1, true, endpoints13, _countof(endpoints13), 1 },
				{ reinterpret_cast<const BYTE*>(content14), sizeof(content14)-1, true, endpoints14, _countof(endpoints14), 1 },
				{ reinterpret_cast<const BYTE*>(content15), sizeof(content15)-1, true, endpoints15, _countof(endpoints15), 1 },
			};

			COperatingSystemTest operatingSystemA(fileName,
				nOpenFlags,
				NULL,
				0);
			DWORD totalFileCreateCountExpectedA(0);
			DWORD totalFileOpenCountExpectedA(0);
			DWORD totalFileFinalizeCountExpectedA(0);

			// Setup. Perform each test with the same instance.
			CProgressiveConfigFile objA(operatingSystemA);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int extraIndex = 0; extraIndex < 2; ++extraIndex)
				{
					// Extra input(s) -- vary for each set of testIndex loops.
					bool refresh = bool(extraIndex & 1);

					// Setup.
					// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
					COperatingSystemTest operatingSystemB(fileName,
						nOpenFlags,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileContent,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileLength);

					// Setup. Perform each test with a new object.
					CProgressiveConfigFile objB(operatingSystemB);
					objB.Start();

					// Verify setup.
					AssertEqual(objB,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_hasConfigFileExpected,
						m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_endpointsExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// More setup.
					operatingSystemA.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);
					operatingSystemB.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);

					// Perform operation(s) to be tested.
					// See note above about the ASSERT for lpMemSource.
					objA.RefreshConfig(refresh);
					objB.RefreshConfig(refresh);

					// Test(s).
					AssertEqual(objA,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						TEST_DATA[testIndex].m_endpointsExpected,
						TEST_DATA[testIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);
					AssertEqual(objB,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						m_activeExpected,
						m_dnsLookupExpected,
						m_pingIntervalExpected,
						TEST_DATA[testIndex].m_endpointsExpected,
						TEST_DATA[testIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystemA.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemA.m_file1.m_openCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_closeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalizeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					AssertResult2x(operatingSystemB.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemB.m_file1.m_openCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_closeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalizeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					// Verify that the file was not changed.
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemA.m_file1.m_finalFileContents,
						operatingSystemA.m_file1.m_finalFileLength),
						_T("IsEqual - error"));
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemB.m_file1.m_finalFileContents,
						operatingSystemB.m_file1.m_finalFileLength),
						_T("IsEqual - error"));

					// Check for unexpected accesses.
					totalFileCreateCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileOpenCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileFinalizeCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;

					AssertResult2x(operatingSystemA.m_totalFileCreateCount, totalFileCreateCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileOpenCount, totalFileOpenCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileCloseCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_totalFileFinalizeCount, totalFileFinalizeCountExpectedA);

					AssertResult2x(operatingSystemB.m_totalFileCreateCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileOpenCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileCloseCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_totalFileFinalizeCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				}
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CConfigFile::::InterpretDWORD()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::GetPingInterval()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		/// </summary>
		TEST_METHOD(StartTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			const char content1[] =
				"Active=false\n"
				"DNSLookup=false\n"
				"Endpoint=ef.s:12\n"
				"Endpoint=ghi.st:34\n"
				"PingInterval=6000\n";
			const CString endpoints1[] = {
				_T("ef.s:12"),
				_T("ghi.st:34"), };

			const char content2[] =
				"Active=false\n"
				"DNSLookup=true\n"
				"Endpoint=efghij.uvwx:12345\n"
				"Endpoint=ghi.st:3456\n"
				"Endpoint=j.v:3456\n"
				"PingInterval=7000\n";
			const CString endpoints2[] = {
				_T("efghij.uvwx:12345"),
				_T("ghi.st:3456"),
				_T("j.v:3456"), };

			const char content3[] =
				"Active=true\n"
				"DNSLookup=false\n"
				"Endpoint=efghijkLM.uvwx:12345\n"
				"Endpoint=ghix.st:3456\n"
				"Endpoint=j01.v:345\n"
				"Endpoint=jr.v:34\n"
				"PingInterval=8000\n";
			const CString endpoints3[] = {
				_T("efghijkLM.uvwx:12345"),
				_T("ghix.st:3456"),
				_T("j01.v:345"),
				_T("jr.v:34"), };

			const char content4[] =
				"Active=true\n"
				"DNSLookup=true\n"
				"Endpoint=efghijkLMuvwx:23456\n"
				"Endpoint=ghixmargo.st:34567\n"
				"Endpoint=j01.v:4567\n"
				"Endpoint=jrbobdavesamworthsmith.v:1\n"
				"Endpoint=ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210\n"
				"PingInterval=9000\n";
			const CString endpoints4[] = {
				_T("efghijkLMuvwx:23456"),
				_T("ghixmargo.st:34567"),
				_T("j01.v:4567"),
				_T("jrbobdavesamworthsmith.v:1"),
				_T("ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210"), };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, true, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, true, false, false, 6000, endpoints1, _countof(endpoints1), 1 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, true, false, true, 7000, endpoints2, _countof(endpoints2), 1 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, true, true, false, 8000, endpoints3, _countof(endpoints3), 1 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, true, true, true, 9000, endpoints4, _countof(endpoints4), 1 },
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.
				// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				// See note above about the ASSERT for lpMemSource.
				obj.Start();

				// Test(s).
				AssertEqual(obj,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpected,
					TEST_DATA[testIndex].m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
				AssertResult2(operatingSystem.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

				// Verify that the file was not changed.
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
					TEST_DATA[testIndex].m_fileLength,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("IsEqual - error"));

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, (DWORD)1);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, TEST_DATA[testIndex].m_totalFileCreateCountExpected);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfigFile::Start()
		///     CProgressiveConfigFile::RefreshConfig()
		///     CProgressiveConfigFile::InterpretNameValue()
		///     CConfigFile::::InterpretBool()
		///     CConfigFile::::InterpretDWORD()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::GetPingInterval()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		/// </summary>
		TEST_METHOD(RefreshConfigTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags(CFile::modeRead);

			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} TestDataType;

			const char content1[] =
				"Active=false\n"
				"DNSLookup=false\n"
				"Endpoint=ef.s:12\n"
				"Endpoint=ghi.st:34\n"
				"PingInterval=6000\n";
			const CString endpoints1[] = {
				_T("ef.s:12"),
				_T("ghi.st:34"), };

			const char content2[] =
				"Active=false\n"
				"DNSLookup=true\n"
				"Endpoint=efghij.uvwx:12345\n"
				"Endpoint=ghi.st:3456\n"
				"Endpoint=j.v:3456\n"
				"PingInterval=7000\n";
			const CString endpoints2[] = {
				_T("efghij.uvwx:12345"),
				_T("ghi.st:3456"),
				_T("j.v:3456"), };

			const char content3[] =
				"Active=true\n"
				"DNSLookup=false\n"
				"Endpoint=efghijkLM.uvwx:12345\n"
				"Endpoint=ghix.st:3456\n"
				"Endpoint=j01.v:345\n"
				"Endpoint=jr.v:34\n"
				"PingInterval=8000\n";
			const CString endpoints3[] = {
				_T("efghijkLM.uvwx:12345"),
				_T("ghix.st:3456"),
				_T("j01.v:345"),
				_T("jr.v:34"), };

			const char content4[] =
				"Active=true\n"
				"DNSLookup=true\n"
				"Endpoint=efghijkLMuvwx:23456\n"
				"Endpoint=ghixmargo.st:34567\n"
				"Endpoint=j01.v:4567\n"
				"Endpoint=jrbobdavesamworthsmith.v:1\n"
				"Endpoint=ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210\n"
				"PingInterval=9000\n";
			const CString endpoints4[] = {
				_T("efghijkLMuvwx:23456"),
				_T("ghixmargo.st:34567"),
				_T("j01.v:4567"),
				_T("jrbobdavesamworthsmith.v:1"),
				_T("ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210"), };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, false, false, true, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, true, false, false, 6000, endpoints1, _countof(endpoints1), 1 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, true, false, true, 7000, endpoints2, _countof(endpoints2), 1 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, true, true, false, 8000, endpoints3, _countof(endpoints3), 1 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, true, true, true, 9000, endpoints4, _countof(endpoints4), 1 },
			};

			COperatingSystemTest operatingSystemA(fileName,
				nOpenFlags,
				NULL,
				0);
			DWORD totalFileCreateCountExpectedA(0);
			DWORD totalFileOpenCountExpectedA(0);
			DWORD totalFileFinalizeCountExpectedA(0);

			// Setup. Perform each test with the same instance.
			CProgressiveConfigFile objA(operatingSystemA);

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int extraIndex = 0; extraIndex < 2; ++extraIndex)
				{
					// Extra input(s) -- vary for each set of testIndex loops.
					bool refresh = bool(extraIndex & 1);

					// Setup.
					// Note: The ASSERT for lpMemSource != NULL in CMemFile::Memcpy() on line 105 (and 108) can be safely ignored when the file size is 0.
					COperatingSystemTest operatingSystemB(fileName,
						nOpenFlags,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileContent,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_fileLength);

					// Setup. Perform each test with a new object.
					CProgressiveConfigFile objB(operatingSystemB);
					objB.Start();

					// Verify setup.
					AssertEqual(objB,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_hasConfigFileExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_activeExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_dnsLookupExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_pingIntervalExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_endpointsExpected,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// More setup.
					operatingSystemA.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);
					operatingSystemB.ChangeFileContents(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength);

					// Perform operation(s) to be tested.
					// See note above about the ASSERT for lpMemSource.
					objA.RefreshConfig(refresh);
					objB.RefreshConfig(refresh);

					// Test(s).
					AssertEqual(objA,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						TEST_DATA[testIndex].m_activeExpected,
						TEST_DATA[testIndex].m_dnsLookupExpected,
						TEST_DATA[testIndex].m_pingIntervalExpected,
						TEST_DATA[testIndex].m_endpointsExpected,
						TEST_DATA[testIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);
					AssertEqual(objB,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						TEST_DATA[testIndex].m_activeExpected,
						TEST_DATA[testIndex].m_dnsLookupExpected,
						TEST_DATA[testIndex].m_pingIntervalExpected,
						TEST_DATA[testIndex].m_endpointsExpected,
						TEST_DATA[testIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystemA.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemA.m_file1.m_openCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_closeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalizeCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemA.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					AssertResult2x(operatingSystemB.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystemB.m_file1.m_openCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_closeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalizeCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileGetLength, (ULONGLONG)TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLength, TEST_DATA[testIndex].m_fileLength);
					AssertResult2x(operatingSystemB.m_file1.m_finalFileLengthRead, TEST_DATA[testIndex].m_fileLength);

					// Verify that the file was not changed.
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemA.m_file1.m_finalFileContents,
						operatingSystemA.m_file1.m_finalFileLength),
						_T("IsEqual - error"));
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_fileContent,
						TEST_DATA[testIndex].m_fileLength,
						operatingSystemB.m_file1.m_finalFileContents,
						operatingSystemB.m_file1.m_finalFileLength),
						_T("IsEqual - error"));

					// Check for unexpected accesses.
					totalFileCreateCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileOpenCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;
					totalFileFinalizeCountExpectedA += TEST_DATA[testIndex].m_totalFileCreateCountExpected;

					AssertResult2x(operatingSystemA.m_totalFileCreateCount, totalFileCreateCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileOpenCount, totalFileOpenCountExpectedA);
					AssertResult2x(operatingSystemA.m_totalFileCloseCount, (DWORD)(2 * testIndex + extraIndex + 1));
					AssertResult2x(operatingSystemA.m_totalFileFinalizeCount, totalFileFinalizeCountExpectedA);

					AssertResult2x(operatingSystemB.m_totalFileCreateCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileOpenCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystemB.m_totalFileCloseCount, (DWORD)2);
					AssertResult2x(operatingSystemB.m_totalFileFinalizeCount,
						TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_totalFileCreateCountExpected +
						TEST_DATA[testIndex].m_totalFileCreateCountExpected);
				}
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfig::SetHasConfigFile()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::SetProgressiveConfig()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		///     CProgressiveConfig::GetPingInterval()
		/// </summary>
		TEST_METHOD(SetProgressiveConfigTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlags((CFile::OpenFlags)(CFile::modeCreate | CFile::modeWrite));

			LPCTSTR endpoint1(_T("abcde.xyz:12345"));
			LPCTSTR endpoint2(_T("123.234.56.78:23456"));

			// Expected result(s) -- independent of test iteration.
			const DWORD totalFileCreateCountExpected(1); // Expect number of creates == opens == finalizes.
			const int totalGetCurrentTimeCount(0);

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const CString* const m_endpoints;
				const int m_endpointCount;

				// Expected setup.
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;

				// Expected result(s).
				const BYTE* const m_bufferExpected;
				const UINT m_bufferLengthExpected;
			} TestDataType;

			CString endpoints1[] = {
				_T("abcde.xyz:12345"),
			};
			CString endpoints2[] = {
				_T("abcde.xyz:12345"),
				_T("   abcdefghijk.lmnop.xyz:1234567890   "),
			};
			CString endpoints3[] = {
				_T("abcde.xyz:12345"),
				_T("abcdefghijk.lmnop.xyz:1234567890"),
				_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 "),
			};

			const BYTE bufferExpected0[] = {
				"Active=false\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=onelinkdiscossl:61614\r\n"
				"Endpoint=onelinkdisco:61613\r\n"
				"PingInterval=15000\r\n"
			};
			const UINT bufferLengthExpected0(sizeof(bufferExpected0) - 1);

			const UINT bufferLengthExpected1(75);
			const BYTE bufferExpected1[bufferLengthExpected1 + 1] = {
				"Active=false\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"PingInterval=6000\r\n"
			};

			const UINT bufferLengthExpected2(130);
			const BYTE bufferExpected2[bufferLengthExpected2 + 1] = {
				"Active=true\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=   abcdefghijk.lmnop.xyz:1234567890   \r\n"
				"PingInterval=1234567890\r\n"
			};

			const UINT bufferLengthExpected3(262);
			const BYTE bufferExpected3[bufferLengthExpected3 + 1] = {
				"Active=true\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=abcdefghijk.lmnop.xyz:1234567890\r\n"
				"Endpoint= !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 \r\n"
				"PingInterval=4294967295\r\n"
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				bufferExpected2, bufferLengthExpected2,
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				bufferExpected3, bufferLengthExpected3,
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				bufferExpected2, bufferLengthExpected2,
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				bufferExpected3, bufferLengthExpected3,
				},
			};

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{

				// Setup CProgressiveConfig instance.
				CProgressiveConfig varProgConfig;
				varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
				varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}
				const CProgressiveConfig& progConfig(varProgConfig);

				// Verify setup.
				AssertEqual(progConfig,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpected,
					TEST_DATA[testIndex].m_endpointCountExpected,
					testIndex, __TFUNCTION__, __LINE__);

				// Setup. Perform each test with a new instance.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlags,
					NULL,
					0);
				CProgressiveConfigFile obj(operatingSystem);

				// Perform operation(s) to be tested.
				obj.SetProgressiveConfig(progConfig);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)1);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);

				// Verify that the output file is as expected.
				CString testIndexStr;
				testIndexStr.Format(_T(" testIndex=%d"), (int)testIndex);
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_bufferExpected,
					TEST_DATA[testIndex].m_bufferLengthExpected,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("SetProgressiveConfigTest() IsEqual") + testIndexStr);

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalGetCurrentTimeCount, totalGetCurrentTimeCount);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfig::SetHasConfigFile()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::SetProgressiveConfig()
		///     CProgressiveConfig::Start()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		///     CProgressiveConfig::GetPingInterval()
		/// Test that a second instance can read a file written by first instance.
		/// </summary>
		TEST_METHOD(SetProgressiveConfig_StartTest)
		{
			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlagsWrite((CFile::OpenFlags)(CFile::modeCreate | CFile::modeWrite));
			const CFile::OpenFlags nOpenFlagsRead(CFile::modeRead);

			LPCTSTR endpoint1(_T("abcde.xyz:12345"));
			LPCTSTR endpoint2(_T("123.234.56.78:23456"));

			// Expected result(s) -- independent of test iteration.
			const bool hasConfigFileExpectedB(true);
			const DWORD totalFileCreateCountExpected(2); // Expect number of creates == opens == finalizes.
			const int totalGetCurrentTimeCount(0);

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const CString* const m_endpoints;
				const int m_endpointCount;

				// Expected setup.
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpectedA;
				const int m_endpointCountExpectedA;
				const CString* const m_endpointsExpectedB;
				const int m_endpointCountExpectedB;

				// Expected result(s).
				const BYTE* const m_bufferExpected;
				const UINT m_bufferLengthExpected;
			} TestDataType;

			CString endpoints1[] = {
				_T("abcde.xyz:12345"),
			};
			CString endpoints2[] = {
				_T("abcde.xyz:12345"),
				_T("   abcdefghijk.lmnop.xyz:1234567890   "),
			};
			CString endpoints2B[] = {
				_T("abcde.xyz:12345"),
				_T("abcdefghijk.lmnop.xyz:1234567890"),
			};
			CString endpoints3[] = {
				_T("abcde.xyz:12345"),
				_T("abcdefghijk.lmnop.xyz:1234567890"),
				_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 "),
			};
			CString endpoints3B[] = {
				_T("abcde.xyz:12345"),
				_T("abcdefghijk.lmnop.xyz:1234567890"),
				_T("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890"),
			};

			const BYTE bufferExpected0[] = {
				"Active=false\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=onelinkdiscossl:61614\r\n"
				"Endpoint=onelinkdisco:61613\r\n"
				"PingInterval=15000\r\n"
			};
			const UINT bufferLengthExpected0(sizeof(bufferExpected0) - 1);

			const UINT bufferLengthExpected1(75);
			const BYTE bufferExpected1[bufferLengthExpected1 + 1] = {
				"Active=false\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"PingInterval=6000\r\n"
			};

			const UINT bufferLengthExpected2(130);
			const BYTE bufferExpected2[bufferLengthExpected2 + 1] = {
				"Active=true\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=   abcdefghijk.lmnop.xyz:1234567890   \r\n"
				"PingInterval=1234567890\r\n"
			};

			const UINT bufferLengthExpected3(262);
			const BYTE bufferExpected3[bufferLengthExpected3 + 1] = {
				"Active=true\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=abcdefghijk.lmnop.xyz:1234567890\r\n"
				"Endpoint= !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 \r\n"
				"PingInterval=4294967295\r\n"
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, endpoints1, _countof(endpoints1), endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, endpoints2, _countof(endpoints2), endpoints2B, _countof(endpoints2B),
				bufferExpected2, bufferLengthExpected2,
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, endpoints3, _countof(endpoints3), endpoints3B, _countof(endpoints3B),
				bufferExpected3, bufferLengthExpected3,
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, endpoints1, _countof(endpoints1), endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, endpoints2, _countof(endpoints2), endpoints2B, _countof(endpoints2B),
				bufferExpected2, bufferLengthExpected2,
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, endpoints3, _countof(endpoints3), endpoints3B, _countof(endpoints3B),
				bufferExpected3, bufferLengthExpected3,
				},
			};

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{

				// Setup CProgressiveConfig instance.
				CProgressiveConfig varProgConfig;
				varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
				varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
				varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
				varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
				for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
				{
					varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
				}
				const CProgressiveConfig& progConfig(varProgConfig);

				// Verify setup.
				AssertEqual(progConfig,
					TEST_DATA[testIndex].m_hasConfigFileExpected,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpectedA,
					TEST_DATA[testIndex].m_endpointCountExpectedA,
					testIndex, __TFUNCTION__, __LINE__);

				// Setup. Perform each test with a new instance.
				COperatingSystemTest operatingSystem(fileName,
					nOpenFlagsWrite,
					nOpenFlagsRead,
					NULL,
					0);
				CProgressiveConfigFile objA(operatingSystem);

				// Perform operation(s) to be tested.
				objA.SetProgressiveConfig(progConfig);
				CProgressiveConfigFile objB(operatingSystem);
				objB.Start();

				// Verify new instance.
				AssertEqual(objB,
					hasConfigFileExpectedB,
					TEST_DATA[testIndex].m_activeExpected,
					TEST_DATA[testIndex].m_dnsLookupExpected,
					TEST_DATA[testIndex].m_pingIntervalExpected,
					TEST_DATA[testIndex].m_endpointsExpectedB,
					TEST_DATA[testIndex].m_endpointCountExpectedB,
					testIndex, __TFUNCTION__, __LINE__);

				// Verify that the file was accessed as expected.
				AssertResult2(operatingSystem.m_file1.m_isOpen, false);
				AssertResult2(operatingSystem.m_file1.m_openCount, (DWORD)2);
				AssertResult2(operatingSystem.m_file1.m_closeCount, (DWORD)2);
				AssertResult2(operatingSystem.m_file1.m_finalizeCount, (DWORD)2);

				// Verify that the output file is as expected.
				CString testIndexStr;
				testIndexStr.Format(_T(" testIndex=%d"), (int)testIndex);
				Assert::IsTrue(
					CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_bufferExpected,
					TEST_DATA[testIndex].m_bufferLengthExpected,
					operatingSystem.m_file1.m_finalFileContents,
					operatingSystem.m_file1.m_finalFileLength),
					_T("SetProgressiveConfigTest() IsEqual") + testIndexStr);

				// Check for unexpected accesses.
				AssertResult2(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileCloseCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected);
				AssertResult2(operatingSystem.m_totalGetCurrentTimeCount, totalGetCurrentTimeCount);
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CProgressiveConfig::SetHasConfigFile()
		///     CProgressiveConfig::SetActive()
		///     CProgressiveConfig::SetDNSLookup()
		///     CProgressiveConfig::AddEndpoint()
		///     CProgressiveConfig::SetPingInterval()
		///     CProgressiveConfig::SetProgressiveConfig()
		///     CProgressiveConfig::RefreshConfig()
		///     CProgressiveConfig::GetActive()
		///     CProgressiveConfig::GetDNSLookup()
		///     CProgressiveConfig::GetEndpointCount()
		///     CProgressiveConfig::GetEndpoint()
		///     CProgressiveConfig::GetPingInterval()
		/// After an instance writes a config file, test that the instance can read from the file after it changes.
		/// </summary>
		TEST_METHOD(SetProgressiveConfig_RefreshConfigTest)
		{
			typedef struct
			{
				// Input(s).
				const BYTE* const m_fileContent;
				const UINT m_fileLength;

				// Expected result(s).
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;
				const DWORD m_totalFileCreateCountExpected; // Expect number of creates == opens == finalizes.
			} ExtraDataType;

			const char content1[] =
				"Active=false\n"
				"DNSLookup=false\n"
				"Endpoint=ef.s:12\n"
				"Endpoint=ghi.st:34\n"
				"PingInterval=6000\n";
			const CString endpoints1x[] = {
				_T("ef.s:12"),
				_T("ghi.st:34"), };

			const char content2[] =
				"Active=false\n"
				"DNSLookup=true\n"
				"Endpoint=efghij.uvwx:12345\n"
				"Endpoint=ghi.st:3456\n"
				"Endpoint=j.v:3456\n"
				"PingInterval=7000\n";
			const CString endpoints2x[] = {
				_T("efghij.uvwx:12345"),
				_T("ghi.st:3456"),
				_T("j.v:3456"), };

			const char content3[] =
				"Active=true\n"
				"DNSLookup=false\n"
				"Endpoint=efghijkLM.uvwx:12345\n"
				"Endpoint=ghix.st:3456\n"
				"Endpoint=j01.v:345\n"
				"Endpoint=jr.v:34\n"
				"PingInterval=8000\n";
			const CString endpoints3x[] = {
				_T("efghijkLM.uvwx:12345"),
				_T("ghix.st:3456"),
				_T("j01.v:345"),
				_T("jr.v:34"), };

			const char content4[] =
				"Active=true\n"
				"DNSLookup=true\n"
				"Endpoint=efghijkLMuvwx:23456\n"
				"Endpoint=ghixmargo.st:34567\n"
				"Endpoint=j01.v:4567\n"
				"Endpoint=jrbobdavesamworthsmith.v:1\n"
				"Endpoint=ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210\n"
				"PingInterval=9000\n";
			const CString endpoints4x[] = {
				_T("efghijkLMuvwx:23456"),
				_T("ghixmargo.st:34567"),
				_T("j01.v:4567"),
				_T("jrbobdavesamworthsmith.v:1"),
				_T("ab_cd-ef..ghijklmn~opqrstu.vwx.yz.1234.567890.v:98765432109876543210"), };

			const ExtraDataType EXTRA_DATA[] =
			{
				{ NULL, 0, false, false, true, 15000, this->m_endpointsExpected, this->m_endpointCountExpected, 2 },
				{ reinterpret_cast<const BYTE*>(content1), sizeof(content1)-1, true, false, false, 6000, endpoints1x, _countof(endpoints1x), 1 },
				{ reinterpret_cast<const BYTE*>(content2), sizeof(content2)-1, true, false, true, 7000, endpoints2x, _countof(endpoints2x), 1 },
				{ reinterpret_cast<const BYTE*>(content3), sizeof(content3)-1, true, true, false, 8000, endpoints3x, _countof(endpoints3x), 1 },
				{ reinterpret_cast<const BYTE*>(content4), sizeof(content4)-1, true, true, true, 9000, endpoints4x, _countof(endpoints4x), 1 },
			};



			// Input(s) -- independent of test iteration.
			const LPCTSTR fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
			const CFile::OpenFlags nOpenFlagsWrite((CFile::OpenFlags)(CFile::modeCreate | CFile::modeWrite));
			const CFile::OpenFlags nOpenFlagsRead(CFile::modeRead);

			LPCTSTR endpoint1(_T("abcde.xyz:12345"));
			LPCTSTR endpoint2(_T("123.234.56.78:23456"));

			// Expected result(s) -- independent of test iteration.
			const DWORD totalFileCreateCountExpected(1); // Expect number of creates == opens == finalizes.
			const int totalGetCurrentTimeCount(0);

			typedef struct
			{
				// Input(s).
				const bool m_hasConfigFile;
				const bool m_active;
				const bool m_dnsLookup;
				const DWORD m_pingInterval;
				const CString* const m_endpoints;
				const int m_endpointCount;

				// Expected setup.
				const bool m_hasConfigFileExpected;
				const bool m_activeExpected;
				const bool m_dnsLookupExpected;
				const DWORD m_pingIntervalExpected;
				const CString* const m_endpointsExpected;
				const int m_endpointCountExpected;

				// Expected result(s).
				const BYTE* const m_bufferExpected;
				const UINT m_bufferLengthExpected;
			} TestDataType;

			CString endpoints1[] = {
				_T("abcde.xyz:12345"),
			};
			CString endpoints2[] = {
				_T("abcde.xyz:12345"),
				_T("   abcdefghijk.lmnop.xyz:1234567890   "),
			};
			CString endpoints3[] = {
				_T("abcde.xyz:12345"),
				_T("abcdefghijk.lmnop.xyz:1234567890"),
				_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 "),
			};

			const BYTE bufferExpected0[] = {
				"Active=false\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=onelinkdiscossl:61614\r\n"
				"Endpoint=onelinkdisco:61613\r\n"
				"PingInterval=15000\r\n"
			};
			const UINT bufferLengthExpected0(sizeof(bufferExpected0) - 1);

			const UINT bufferLengthExpected1(75);
			const BYTE bufferExpected1[bufferLengthExpected1 + 1] = {
				"Active=false\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"PingInterval=6000\r\n"
			};

			const UINT bufferLengthExpected2(130);
			const BYTE bufferExpected2[bufferLengthExpected2 + 1] = {
				"Active=true\r\n"
				"DNSLookup=false\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=   abcdefghijk.lmnop.xyz:1234567890   \r\n"
				"PingInterval=1234567890\r\n"
			};

			const UINT bufferLengthExpected3(262);
			const BYTE bufferExpected3[bufferLengthExpected3 + 1] = {
				"Active=true\r\n"
				"DNSLookup=true\r\n"
				"Endpoint=abcde.xyz:12345\r\n"
				"Endpoint=abcdefghijk.lmnop.xyz:1234567890\r\n"
				"Endpoint= !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~:123456789012345678901234567890 \r\n"
				"PingInterval=4294967295\r\n"
			};

			const TestDataType TEST_DATA[] =
			{
				{ false, false, false, 0, NULL, 0,
				false, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ false, false, true, 6000, endpoints1, _countof(endpoints1),
				false, false, true, 6000, endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				false, true, false, 1234567890, endpoints2, _countof(endpoints2),
				bufferExpected2, bufferLengthExpected2,
				},

				{ false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				false, true, true, 4294967295, endpoints3, _countof(endpoints3),
				bufferExpected3, bufferLengthExpected3,
				},

				{ true, false, false, 0, NULL, 0,
				true, false, false, 15000, this->m_endpointsExpected, this->m_endpointCountExpected,
				bufferExpected0, bufferLengthExpected0,
				},

				{ true, false, true, 6000, endpoints1, _countof(endpoints1),
				true, false, true, 6000, endpoints1, _countof(endpoints1),
				bufferExpected1, bufferLengthExpected1,
				},

				{ true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				true, true, false, 1234567890, endpoints2, _countof(endpoints2),
				bufferExpected2, bufferLengthExpected2,
				},

				{ true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				true, true, true, 4294967295, endpoints3, _countof(endpoints3),
				bufferExpected3, bufferLengthExpected3,
				},
			};

			for (int testIndex(0); testIndex < _countof(TEST_DATA); ++testIndex)
			{
				for (int extraIndex(0); extraIndex < _countof(EXTRA_DATA); ++extraIndex)
				{
					// Setup CProgressiveConfig instance.
					CProgressiveConfig varProgConfig;
					varProgConfig.SetHasConfigFile(TEST_DATA[testIndex].m_hasConfigFile);
					varProgConfig.SetActive(TEST_DATA[testIndex].m_active);
					varProgConfig.SetDNSLookup(TEST_DATA[testIndex].m_dnsLookup);
					varProgConfig.SetPingInterval(TEST_DATA[testIndex].m_pingInterval);
					for (int index(0); index < TEST_DATA[testIndex].m_endpointCount; ++index)
					{
						varProgConfig.AddEndpoint(TEST_DATA[testIndex].m_endpoints[index]);
					}
					const CProgressiveConfig& progConfig(varProgConfig);

					// Verify setup.
					AssertEqual(progConfig,
						TEST_DATA[testIndex].m_hasConfigFileExpected,
						TEST_DATA[testIndex].m_activeExpected,
						TEST_DATA[testIndex].m_dnsLookupExpected,
						TEST_DATA[testIndex].m_pingIntervalExpected,
						TEST_DATA[testIndex].m_endpointsExpected,
						TEST_DATA[testIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Setup. Perform each test with a new instance.
					COperatingSystemTest operatingSystem(fileName,
						nOpenFlagsWrite,
						nOpenFlagsRead,
						NULL,
						0);
					CProgressiveConfigFile obj(operatingSystem);

					// Perform operation(s) to be tested.
					obj.SetProgressiveConfig(progConfig);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystem.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystem.m_file1.m_openCount, (DWORD)1);
					AssertResult2x(operatingSystem.m_file1.m_closeCount, (DWORD)1);
					AssertResult2x(operatingSystem.m_file1.m_finalizeCount, (DWORD)1);

					// Verify that the output file is as expected.
					CString testIndexStr;
					testIndexStr.Format(_T(" testIndex=%d/%d"), (int)testIndex, (int)extraIndex);
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(TEST_DATA[testIndex].m_bufferExpected,
						TEST_DATA[testIndex].m_bufferLengthExpected,
						operatingSystem.m_file1.m_finalFileContents,
						operatingSystem.m_file1.m_finalFileLength),
						_T("SetProgressiveConfigTest() IsEqual") + testIndexStr);

					// Check for unexpected accesses.
					AssertResult2x(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalFileCloseCount, totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalGetCurrentTimeCount, totalGetCurrentTimeCount);

					// More setup.
					operatingSystem.ChangeFileContents(EXTRA_DATA[extraIndex].m_fileContent,
						EXTRA_DATA[extraIndex].m_fileLength);

					// Perform more operation(s) to be tested.
					obj.RefreshConfig((bool)(extraIndex & 1));

					// Test(s).
					AssertEqual(obj,
						EXTRA_DATA[extraIndex].m_hasConfigFileExpected,
						EXTRA_DATA[extraIndex].m_activeExpected,
						EXTRA_DATA[extraIndex].m_dnsLookupExpected,
						EXTRA_DATA[extraIndex].m_pingIntervalExpected,
						EXTRA_DATA[extraIndex].m_endpointsExpected,
						EXTRA_DATA[extraIndex].m_endpointCountExpected,
						testIndex, __TFUNCTION__, __LINE__, &extraIndex);

					// Verify that the file was accessed as expected.
					AssertResult2x(operatingSystem.m_file1.m_isOpen, false);
					AssertResult2x(operatingSystem.m_file1.m_openCount, (DWORD)2);
					AssertResult2x(operatingSystem.m_file1.m_closeCount, (DWORD)2);
					AssertResult2x(operatingSystem.m_file1.m_finalizeCount, (DWORD)2);
					AssertResult2x(operatingSystem.m_file1.m_finalFileGetLength, (ULONGLONG)EXTRA_DATA[extraIndex].m_fileLength);
					AssertResult2x(operatingSystem.m_file1.m_finalFileLength, EXTRA_DATA[extraIndex].m_fileLength);
					AssertResult2x(operatingSystem.m_file1.m_finalFileLengthRead, EXTRA_DATA[extraIndex].m_fileLength);

					// Verify that the file was not changed.
					Assert::IsTrue(
						CUnitTestingUtils::IsEqual(EXTRA_DATA[extraIndex].m_fileContent,
						EXTRA_DATA[extraIndex].m_fileLength,
						operatingSystem.m_file1.m_finalFileContents,
						operatingSystem.m_file1.m_finalFileLength),
						_T("IsEqual - error") + testIndexStr);

					// Check for unexpected accesses.
					AssertResult2x(operatingSystem.m_totalFileCreateCount, totalFileCreateCountExpected +
						EXTRA_DATA[extraIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalFileOpenCount, totalFileCreateCountExpected +
						EXTRA_DATA[extraIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalFileCloseCount, totalFileCreateCountExpected + (DWORD)1);
					AssertResult2x(operatingSystem.m_totalFileFinalizeCount, totalFileCreateCountExpected +
						EXTRA_DATA[extraIndex].m_totalFileCreateCountExpected);
					AssertResult2x(operatingSystem.m_totalGetCurrentTimeCount, totalGetCurrentTimeCount);
				}
			}
		}
	};

	const LPCTSTR ProgressiveConfigFile::m_fileName(_T("D:\\SentinelData\\ProgressiveProtocolConfig.txt"));
	const LPCTSTR ProgressiveConfigFile::m_fileName2(_T("C:\\ProgressiveProtocolConfig.txt"));

	const CStringW ProgressiveConfigFile::m_beginRootElement(L"<?xml version='1.0' encoding='UTF-16' ?><root>");
	const CStringW ProgressiveConfigFile::m_endRootElement(L"</root>");
	const CStringW ProgressiveConfigFile::m_dataElementName(L"data");

	const LPCWSTR ProgressiveConfigFile::TestData_ContentName(L"content");
	const LPCWSTR ProgressiveConfigFile::TestData_ValueName(L"value");
	const LPCWSTR ProgressiveConfigFile::TestData_ValidName(L"valid");

	const CString ProgressiveConfigFile::m_endpointsExpected[] = { _T("onelinkdiscossl:61614"), _T("onelinkdisco:61613") };
}
#endif //TODO-PORT