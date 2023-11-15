#if 000//TODO-PORT
#include "stdafx.h"
#include "UI/ContentPackageInfo.h"

// Unit testing includes.
#include "UnitTestingUtils.h"

// AssertResult macro(s) for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(ContentPackageInfo)
	{
		/// <summary>
		/// Code coverage and functional test for:
		///     CContentPackageInfo::SetID().
		///     CContentPackageInfo::GetID().
		/// </summary>
		TEST_METHOD(SetIDTest)
		{
			typedef struct
			{
				// Inputs.
				const int m_id;

				// Expected results.
				const int m_idExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ -1, -1 },
				{ 0, 0 },
				{ 1, 1 },
				{ 222, 222 },
				{ 1234567890, 1234567890 },
				{ INT_MAX - 1, INT_MAX - 1 },
				{ INT_MAX, INT_MAX },
				{ INT_MIN, INT_MIN },
				{ INT_MIN + 1, INT_MIN + 1 },
				{ (int)(UINT_MAX - 1), (int)(UINT_MAX - 1) },
				{ (int)(UINT_MAX), (int)(UINT_MAX) },
			};

			// Setup.
			CContentPackageInfo varObj0; /// Test values using the same CContentPackageInfo instance.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				varObj0.SetID(TEST_DATA[testIndex].m_id);
				const CContentPackageInfo &obj0(varObj0);

				// Test(s).
				AssertResult2(obj0.GetID(), TEST_DATA[testIndex].m_idExpected);

				// More setup.
				CContentPackageInfo varObj2; /// With a default instance, test setting a first value and then a second value.

				// Perform more operation(s) to be tested.
				varObj2.SetID(TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_id);
				const CContentPackageInfo &obj2a(varObj2);

				// Test(s) prior operation(s).
				AssertResult2(obj2a.GetID(), TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_idExpected);

				// Perform more operation(s) to be tested.
				varObj2.SetID(TEST_DATA[testIndex].m_id);
				const CContentPackageInfo &obj2b(varObj2);

				// Test(s) prior operation(s).
				AssertResult2(obj2b.GetID(), TEST_DATA[testIndex].m_idExpected);

				// Cleanup.
			}
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CContentPackageInfo::SetPackageName()
		///     CContentPackageInfo::GetPackageName()
		/// </summary>
		TEST_METHOD(SetPackageNameTest)
		{
			typedef struct
			{
				// Input(s).
				const LPCTSTR m_packageName;

				// Expected result(s).
				const CString m_packageNameExpected;
			} TestDataType;

			const TestDataType TEST_DATA[] =
			{
				{ _T(""), _T("") },
				{ _T(" "), _T(" ") },
				{ _T("*"), _T("*") },
				{ _T("a"), _T("a") },
				{ _T("1"), _T("1") },
				{ _T("~"), _T("~") },
				{ _T("0000000000"), _T("0000000000") },
				{ _T("FFFFFFFFFF"), _T("FFFFFFFFFF") },
				{ _T("Alpha Beta"), _T("Alpha Beta") },
				{ _T(" Zeta Omega   "), _T(" Zeta Omega   ") },
				
				// All displayable ASCII characters, with imbedded quote.
				{ _T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
					_T(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
				},

				// All displayable ASCII characters, with trailing quote.
				{ _T(" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\""),
					_T(" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\"")
				},

				// 300 characters.
				{
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789."),
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
					_T("123456789.123456789.123456789.123456789.123456789.") _T("123456789.123456789.123456789.123456789.123456789.")
				},
			};

			// Setup.
			CContentPackageInfo varObj0; /// Test values using the same CContentPackageInfo instance.

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				varObj0.SetPackageName(TEST_DATA[testIndex].m_packageName);
				const CContentPackageInfo &obj0(varObj0);

				// Test(s).
				AssertResult2(obj0.GetPackageName(), TEST_DATA[testIndex].m_packageNameExpected);

				// More setup.
				CContentPackageInfo varObj2; /// With a default instance, test setting a first value and then a second value.

				// Perform more operation(s) to be tested.
				varObj2.SetPackageName(TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_packageName);
				const CContentPackageInfo &obj2a(varObj2);

				// Test(s) prior operation(s).
				AssertResult2(obj2a.GetPackageName(), TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_packageNameExpected);

				// Perform more operation(s) to be tested.
				varObj2.SetPackageName(TEST_DATA[testIndex].m_packageName);
				const CContentPackageInfo &obj2b(varObj2);

				// Test(s) prior operation(s).
				AssertResult2(obj2b.GetPackageName(), TEST_DATA[testIndex].m_packageNameExpected);

				// Cleanup.
			}
		}
	};
}
#endif //TODO-PORT