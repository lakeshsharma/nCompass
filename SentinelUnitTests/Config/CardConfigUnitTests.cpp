#include "stdafx.h"
#include "Config/CardConfig.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#if 000//TODO-PORT
#include "UnitTestingXmlLite.h"
#endif
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
	// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, #result)

#define AssertResult5(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult4(result, resultLength, expected, expectedLength) AssertResult5(result, resultLength, expected, expectedLength, #result)

// AssertResult macros for calling CUnitTestingUtils from a test method using the member test index.
#define AssertResult3m(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2m(result, expected) AssertResult3m(result, expected, #result)

// AssertResult macros for calling CUnitTestingUtils from a helper method.
#define AssertResult3h(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult2h(result, expected) AssertResult3h(result, expected, #result)

#define AssertResult5h(result, resultLength, expected, expectedLength, resultName) \
	CUnitTestingUtils::AssertEqual(result, resultLength, expected, expectedLength, resultName, testIndex, functionName, lineNumber, extraIndex);
#define AssertResult4h(result, resultLength, expected, expectedLength) AssertResult5h(result, resultLength, expected, expectedLength, #result)

// AssertResult macro for calling CUnitTestingUtils (with an extra index) from a test method.
#define AssertResult3x(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, testIndex, __TFUNCTION__, __LINE__, &extraIndex);
#define AssertResult2x(result, expected) AssertResult3x(result, expected, #result)

	static const BYTE m_commIDExpected = 0;
	static const bool m_rcclCardsEnabledExpected = false;
	static const bool m_nclCardsEnabledExpected = false;
	static const vector<BYTE> m_playerCardPrefixesExpected;
	static const int m_prefixCountExpected = 0;
	static const BYTE m_globalCardPrefixExpected = 0;
	static const BYTE m_mechanicCardPrefixExpected = 0;
	static const BYTE m_floorCardPrefixExpected = 0;
	static const BYTE m_tempCardPrefixExpected = 0;
	static const bool m_universeEnabledExpected = false;
	static const bool m_mixedModeEnabledExpected = false;
	static const vector<BYTE> m_universeCardPrefixExpected;
	static const int m_universeCardPrefixCountExpected = 0;
	static const vector<WORD> m_universePropCodesExpected;
	static const int m_universePropCodesCountExpected = 0;
	static const BYTE m_cardReaderTrackNumberExpected = 2;
	static const BYTE m_rcclCardPlayerPrefixExpected = 0;
	static const BYTE m_useSDSCardFormatExpected = 0;

	// Class member(s) used by test (data) methods.
	static const string m_beginRootElement("<?xml version='1.0' encoding='UTF-16' ?><root>");
	static const string m_endRootElement("</root>");
	static const string m_dataElementName("data");
	int testIndex;

	// Test data attribute name(s).
	static const LPCWSTR TestData_RcclCardsEnabledName(L"rccl");
	static const LPCWSTR TestData_NclCardsEnabledName(L"ncl");
	static const LPCWSTR TestData_UniverseEnabledName(L"universe");
	static const LPCWSTR TestData_MixedModeEnabledName(L"mixed");
	static const LPCWSTR TestData_CardReaderTrackNumberName(L"track");
	static const LPCWSTR TestData_CardReaderTrackNumberExpectedName(L"trackExpected");

	/// <summary>
	/// Initialize class member(s) before each test method is run.
	/// </summary>
	TEST(CardConfig, Initialize)
	{
		// Test method initialization code.
		testIndex = 0;
	}

	/// <summary>
	/// Cleanup class member(s) before each test method is run.
	/// </summary>
	TEST(CardConfig, Cleanup)
	{
		// Test method cleanup code.
	}

#if 000//TODO-PORT
	static stringW DataElement(LPCWSTR attributesString)
	{
		stringW dataElement(CStringW(L"<") + m_dataElementName + L" " + attributesString + L"/>");
		return dataElement;
	}
#endif

	void AssertEqual(const CCardConfig &obj,
		BYTE commIDExpected,
		bool rcclCardsEnabledExpected,
		bool nclCardsEnabledExpected,
		const vector<BYTE> &playerCardPrefixesExpected,
		int playerCardPrefixCountExpected,
		BYTE globalCardPrefixExpected,
		BYTE mechanicCardPrefixExpected,
		BYTE floorCardPrefixExpected,
		BYTE tempCardPrefixExpected,
		bool universeEnabledExpected,
		bool mixedModeEnabledExpected,
		const vector<BYTE> &universeCardPrefixExpected,
		int universeCardPrefixCountExpected,
		const vector<WORD> &universePropCodesExpected,
		int universePropCodesCountExpected,
		BYTE cardReaderTrackNumberExpected,
		BYTE rcclCardPlayerPrefixExpected,
		BYTE useSDSCardFormatExpected,
		int testIndex,
		LPCTSTR functionName,
		int lineNumber,
		const int* extraIndex = NULL)
	{
		AssertResult2h(obj.GetCommID(), commIDExpected);
		AssertResult2h(obj.RcclCardsEnabled(), rcclCardsEnabledExpected);
		AssertResult2h(obj.NclCardsEnabled(), nclCardsEnabledExpected);

		{
			const vector<BYTE> &playerCardPrefixes(obj.GetPlayerCardPrefixes());
			int playerCardPrefixCount = playerCardPrefixes.size();
			AssertResult2h(playerCardPrefixCount, (int)obj.GetPlayerCardPrefixes().size());
		}

		AssertResult2h(obj.GetGlobalCardPrefix(), globalCardPrefixExpected);
		AssertResult2h(obj.GetMechanicCardPrefix(), mechanicCardPrefixExpected);
		AssertResult2h(obj.GetFloorCardPrefix(), floorCardPrefixExpected);
		AssertResult2h(obj.GetTempCardPrefix(), tempCardPrefixExpected);
		AssertResult2h(obj.GetUniverseEnabled(), universeEnabledExpected);
		AssertResult2h(obj.GetMixedModeEnabled(), mixedModeEnabledExpected);

		{
			const vector<BYTE> &universeCardPrefix(obj.GetUniverseCardPrefix());
			int universeCardPrefixCount = universeCardPrefix.size();
			AssertResult2h(universeCardPrefixCount, (int)obj.GetUniverseCardPrefix().size());
		}

		{
			const vector<WORD> &universePropCodes(obj.GetUniversePropCodes());
			int universePropCodesCount = universePropCodes.size();
			AssertResult2h(universePropCodesCount, (int)obj.GetUniversePropCodes().size() );
			ASSERT_TRUE(2 == SIZE_OF_VALID_PROP_CODE) << "2 == SIZE_OF_VALID_PROP_CODE";
		}

		AssertResult2h(obj.CardReaderTrackNumber(), cardReaderTrackNumberExpected);
		AssertResult2h(obj.GetRcclCardPlayerPrefix(), rcclCardPlayerPrefixExpected);
		AssertResult2h(obj.GetUseSDSCardFormat(), useSDSCardFormatExpected);
	}

	/// <summary>
	/// Functional test for:
	///     CCardConfig::INVALID_CARD_PREFIX
	/// </summary>
	TEST(CardConfig, InvalidCardPrefixTest)
	{
		AssertResult2m(CCardConfig::INVALID_CARD_PREFIX, (BYTE)0);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::GetCommID()
	///     CCardConfig::RcclCardsEnabled()
	///     CCardConfig::NclCardsEnabled()
	///     CCardConfig::GetPlayerCardPrefixes()
	///     CCardConfig::GetGlobalCardPrefix()
	///     CCardConfig::GetMechanicCardPrefix()
	///     CCardConfig::GetFloorCardPrefix()
	///     CCardConfig::GetTempCardPrefix()
	///     CCardConfig::GetUniverseEnabled()
	///     CCardConfig::GetMixedModeEnabled()
	///     CCardConfig::GetUniverseCardPrefix()
	///     CCardConfig::GetUniversePropCodes()
	///     CCardConfig::CardReaderTrackNumber()
	///     CCardConfig::GetRcclCardPlayerPrefix()
	/// Code coverage:
	///     CCardConfig::~CCardConfig()
	/// </summary>
	TEST(CardConfig, CardConfigTest)
	{
		// Setup.
		const int testIndex(0);

		// Perform operation(s) to be tested.
		const CCardConfig obj;

		// Test(s).
		AssertEqual(obj,
			m_commIDExpected,
			m_rcclCardsEnabledExpected,
			m_nclCardsEnabledExpected,
			m_playerCardPrefixesExpected,
			m_prefixCountExpected,
			m_globalCardPrefixExpected,
			m_mechanicCardPrefixExpected,
			m_floorCardPrefixExpected,
			m_tempCardPrefixExpected,
			m_universeEnabledExpected,
			m_mixedModeEnabledExpected,
			m_universeCardPrefixExpected,
			m_universeCardPrefixCountExpected,
			m_universePropCodesExpected,
			m_universePropCodesCountExpected,
			m_cardReaderTrackNumberExpected,
			m_rcclCardPlayerPrefixExpected,
			m_useSDSCardFormatExpected,
			testIndex, __TFUNCTION__, __LINE__);
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::Setup()
	/// Test using default values.
	/// </summary>
	TEST(CardConfig, Setup_DefaultTest)
	{
		// Setup.
		const int testIndex(0);
		CCardConfig varObj;

		CConfig varConfig(false, "");
		varConfig.SetCommID(m_commIDExpected);
		varConfig.SetRcclCardsEnabled(m_rcclCardsEnabledExpected);
		varConfig.SetNclCardsEnabled(m_nclCardsEnabledExpected);
		varConfig.SetPlayerCardPrefixes(m_playerCardPrefixesExpected);
		varConfig.SetGlobalCardPrefix(m_globalCardPrefixExpected);
		varConfig.SetMechanicCardPrefix(m_mechanicCardPrefixExpected);
		varConfig.SetFloorCardPrefix(m_floorCardPrefixExpected);
		varConfig.SetTempCardPrefix(m_commIDExpected);
		varConfig.SetUniverseEnabled(m_universeEnabledExpected);
		varConfig.SetMixedModeEnabled(m_mixedModeEnabledExpected);
		varConfig.SetUniversePrefixes(m_universeCardPrefixExpected);
		varConfig.SetUniversePropCodes(m_universePropCodesExpected);
		varConfig.SetCardReaderTrackNumber(m_cardReaderTrackNumberExpected);
		const CConfig& config(varConfig);

		// Perform operation(s) to be tested.
		varObj.Setup(config);
		const CCardConfig& obj(varObj);

		// Test(s).
		AssertEqual(obj,
			m_commIDExpected,
			m_rcclCardsEnabledExpected,
			m_nclCardsEnabledExpected,
			m_playerCardPrefixesExpected,
			m_prefixCountExpected,
			m_globalCardPrefixExpected,
			m_mechanicCardPrefixExpected,
			m_floorCardPrefixExpected,
			m_tempCardPrefixExpected,
			m_universeEnabledExpected,
			m_mixedModeEnabledExpected,
			m_universeCardPrefixExpected,
			m_universeCardPrefixCountExpected,
			m_universePropCodesExpected,
			m_universePropCodesCountExpected,
			m_cardReaderTrackNumberExpected,
			m_rcclCardPlayerPrefixExpected,
			m_useSDSCardFormatExpected,
			testIndex, __TFUNCTION__, __LINE__);
	}

#if 000//TODO-PORT
	static stringW SetupTestData()
	{
		// Test data:
		// "rccl" is the test value for SetRcclCardsEnabled() and RcclCardsEnabled().
		// "ncl" is the test value for SetNclCardsEnabled() and NclCardsEnabled().
		// "universe" is the test value for SetUniverseEnabled() and GetUniverseEnabled().
		// "mixed" is the test value for SetMixedModeEnabled() and GetMixedModeEnabled().
		// "track" is the test value for SetCardReaderTrackNumber().
		// "trackExpected" is the test value for CardReaderTrackNumber().
		const stringW testData =
			m_beginRootElement +

			// Test with all bool values false.
			DataElement(L"rccl='false' ncl='false' universe='false' mixed='false' track='255' trackExpected='2'") +

			// Test with all bool values false, except for one.
			DataElement(L"rccl='false' ncl='false' universe='false' mixed='true' track='1' trackExpected='1'") +
			DataElement(L"rccl='false' ncl='false' universe='true' mixed='false' track='0' trackExpected='2'") +
			DataElement(L"rccl='false' ncl='true' universe='false' mixed='false' track='2' trackExpected='2'") +
			DataElement(L"rccl='true' ncl='false' universe='false' mixed='false' track='1' trackExpected='1'") +

			// Test with all bool values true, except for one.
			DataElement(L"rccl='true' ncl='true' universe='true' mixed='false' track='3' trackExpected='2'") +
			DataElement(L"rccl='true' ncl='true' universe='false' mixed='true' track='4' trackExpected='2'") +
			DataElement(L"rccl='false' ncl='false' universe='true' mixed='true' track='1' trackExpected='1'") +
			DataElement(L"rccl='true' ncl='true' universe='true' mixed='true' track='5' trackExpected='2'") +

			// Test with all bool values true.
			DataElement(L"rccl='true' ncl='true' universe='true' mixed='true' track='0' trackExpected='2'") +

			m_endRootElement;

		return testData;
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::Setup()
	/// Test using different permutations of the boolean values and card reader track number.
	/// </summary>
	TEST(CardConfig, SetupTest)
	{
		// Expected result(s).
		const BYTE commIDExpected(11);
		const BYTE globalCardPrefixExpected(12);
		const BYTE mechanicCardPrefixExpected(13);
		const BYTE floorCardPrefixExpected(14);
		const BYTE tempCardPrefixExpected(15);
		static const BYTE cardReaderTrackNumberExpected = 16;

		static const BYTE playerCardPrefixesExpected[] = { 71, 72, 73, 74, 75 };
		static const int prefixCountExpected(_countof(playerCardPrefixesExpected));
		static const BYTE rcclCardPlayerPrefixExpected(playerCardPrefixesExpected[0]);

		static const BYTE universeCardPrefixExpected[] = { 81, 82, 83, 84, 85, 86, 87 };
		static const BYTE universePropCodesExpected[] = { 91, 92, 93, 94, 95, 96 };
		static const int universePropCodesCountExpected(_countof(playerCardPrefixesExpected) / 2);
		static const int universeCardPrefixCountExpected(universePropCodesCountExpected);
		ASSERT_TRUE(2 == SIZE_OF_VALID_PROP_CODE) << "2 == SIZE_OF_VALID_PROP_CODE";

		CXmlReaderTest testReader(SetupTestData(), &testIndex);

		for (; testReader.MoveToNextElement(m_dataElementName); ++testIndex)
		{
			// Setup. Get test data.
			const bool rcclCardsEnabled(testReader.GetNextAttributeAsBool(TestData_RcclCardsEnabledName));
			const bool nclCardsEnabled(testReader.GetNextAttributeAsBool(TestData_NclCardsEnabledName));
			const bool universeEnabled(testReader.GetNextAttributeAsBool(TestData_UniverseEnabledName));
			const bool mixedModeEnabled(testReader.GetNextAttributeAsBool(TestData_MixedModeEnabledName));
			const BYTE cardReaderTrackNumber(testReader.GetNextAttributeAsBYTE(TestData_CardReaderTrackNumberName));
			const BYTE cardReaderTrackNumberExpected(testReader.GetNextAttributeAsBYTE(TestData_CardReaderTrackNumberExpectedName));

			// Setup.
			CCardConfig varObj;

			CConfig varConfig(false, "");
			varConfig.SetCommID(commIDExpected);
			varConfig.SetRcclCardsEnabled(rcclCardsEnabled);
			varConfig.SetNclCardsEnabled(nclCardsEnabled);
			varConfig.SetPlayerCardPrefixes(playerCardPrefixesExpected, prefixCountExpected);
			varConfig.SetGlobalCardPrefix(globalCardPrefixExpected);
			varConfig.SetMechanicCardPrefix(mechanicCardPrefixExpected);
			varConfig.SetFloorCardPrefix(floorCardPrefixExpected);
			varConfig.SetTempCardPrefix(tempCardPrefixExpected);
			varConfig.SetUniverseEnabled(universeEnabled);
			varConfig.SetMixedModeEnabled(mixedModeEnabled);
			varConfig.SetUniversePrefixes(universeCardPrefixExpected, universeCardPrefixCountExpected);
			varConfig.SetUniversePropCodes(universePropCodesExpected, universePropCodesCountExpected);
			varConfig.SetCardReaderTrackNumber(cardReaderTrackNumber);
			const CConfig& config(varConfig);

			// Perform operation(s) to be tested.
			varObj.Setup(config);
			const CCardConfig& obj(varObj);

			// Test(s).
			AssertEqual(obj,
				commIDExpected,
				rcclCardsEnabled,
				nclCardsEnabled,
				playerCardPrefixesExpected,
				prefixCountExpected,
				globalCardPrefixExpected,
				mechanicCardPrefixExpected,
				floorCardPrefixExpected,
				tempCardPrefixExpected,
				universeEnabled,
				mixedModeEnabled,
				universeCardPrefixExpected,
				universeCardPrefixCountExpected,
				universePropCodesExpected,
				universePropCodesCountExpected,
				cardReaderTrackNumberExpected,
				rcclCardPlayerPrefixExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		ASSERT_TRUE(testIndex > 0) << "testIndex > 0"; // Assert that the data contained at least one test.
		testReader.AssertEof(); // Assert that we read all of the data.
	}
#endif

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetCommID()
	///     CCardConfig::GetCommID()
	/// </summary>
	TEST(CardConfig, SetCommIDTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE m_commID;

			// Expected results.
			const BYTE m_commIDExpected;
			const BYTE m_oldCommIDExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

			{ 0, 0, (BYTE)-1 },
			{ 1, 1, 0 },
			{ 2, 2, 1 },

			{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 2 },
			{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
			{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
			{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
			{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
			{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			BYTE m_oldCommIDResultA(objA.SetCommID(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_commID));
			AssertResult2(m_oldCommIDResultA, m_commIDExpected);
			AssertEqual(objA,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			BYTE oldCommIDResultA = objA.SetCommID(TEST_DATA[testIndex].m_commID);

			CCardConfig objB;
			BYTE oldCommIDResultB(objB.SetCommID(TEST_DATA[testIndex].m_commID));

			// Test(s).
			AssertResult2(oldCommIDResultA, TEST_DATA[testIndex].m_oldCommIDExpected);
			AssertEqual(objA,
				TEST_DATA[testIndex].m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldCommIDResultB, m_commIDExpected);
			AssertEqual(objB,
				TEST_DATA[testIndex].m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetRcclCardsEnabled()
	///     CCardConfig::RcclCardsEnabled()
	/// </summary>
	TEST(CardConfig, SetRcclCardsEnabledTest)
	{
		typedef struct
		{
			// Inputs.
			const bool m_rcclCardsEnabled;

			// Expected results.
			const bool m_rcclCardsEnabledExpected;
			const bool m_oldRcclCardsEnabledExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ true, true, false },
			{ true, true, true },
			{ false, false, true },
			{ false, false, false },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			bool oldRcclCardsEnabledResultA(objA.SetRcclCardsEnabled(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_rcclCardsEnabled));
			AssertResult2(oldRcclCardsEnabledResultA, m_rcclCardsEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			bool oldRcclCardsEnabledResultA = objA.SetRcclCardsEnabled(TEST_DATA[testIndex].m_rcclCardsEnabled);

			CCardConfig objB;
			bool oldRcclCardsEnabledResultB(objB.SetRcclCardsEnabled(TEST_DATA[testIndex].m_rcclCardsEnabled));

			// Test(s).
			AssertResult2(oldRcclCardsEnabledResultA, TEST_DATA[testIndex].m_oldRcclCardsEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				TEST_DATA[testIndex].m_rcclCardsEnabled,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldRcclCardsEnabledResultB, m_rcclCardsEnabledExpected);
			AssertEqual(objB,
				m_commIDExpected,
				TEST_DATA[testIndex].m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetNclCardsEnabled()
	///     CCardConfig::NclCardsEnabled()
	/// </summary>
	TEST(CardConfig, SetNclCardsEnabledTest)
	{
		typedef struct
		{
			// Inputs.
			const bool m_nclCardsEnabled;

			// Expected results.
			const bool m_nclCardsEnabledExpected;
			const bool m_oldNclCardsEnabledExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ true, true, false },
			{ true, true, true },
			{ false, false, true },
			{ false, false, false },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			bool oldNclCardsEnabledResultA(objA.SetNclCardsEnabled(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_nclCardsEnabled));
			AssertResult2(oldNclCardsEnabledResultA, m_nclCardsEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			bool oldNclCardsEnabledResultA = objA.SetNclCardsEnabled(TEST_DATA[testIndex].m_nclCardsEnabled);

			CCardConfig objB;
			bool oldNclCardsEnabledResultB(objB.SetNclCardsEnabled(TEST_DATA[testIndex].m_nclCardsEnabled));

			// Test(s).
			AssertResult2(oldNclCardsEnabledResultA, TEST_DATA[testIndex].m_oldNclCardsEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				TEST_DATA[testIndex].m_nclCardsEnabled,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldNclCardsEnabledResultB, m_nclCardsEnabledExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				TEST_DATA[testIndex].m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

#if 000//TODO-PORT
	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::SetPlayerCardPrefixes()
	///     CCardConfig::GetUniverseCardPrefix()
	/// </summary>
	TEST(CardConfig, SetPlayerCardPrefixesTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE* const m_playerCardPrefixes;
			const int m_prefixCount;

			// Expected results.
			const BYTE* const m_playerCardPrefixesExpected;
			const int m_prefixCountExpected;
			const BYTE m_rcclCardPlayerPrefixExpected;
		} TestDataType;

		const BYTE in2[] = { 0x00 };
		const BYTE in3[] = { 0x7f };
		const BYTE in4[] = { 0x80 };
		const BYTE in5[] = { 0xff };
		const BYTE in6[] = { 0x12, 0x34 };
		const BYTE in7[] = { 0x23, 0x45, 0x67 };
		const BYTE in8[] = { 0x00, 0x7f, 0x80, 0xff };
		const BYTE in9[] = {
			0x99, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e };
		const BYTE in10[] = {
			0xaa, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

		const BYTE in11[] = {
			0xbb, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };
		const BYTE out11[] = {
			0xbb, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const BYTE in12[] = {
			0xcc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21 };
		const BYTE out12[] = {
			0xcc, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const BYTE in13[] = {
			0xdd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21, 0x22 };
		const BYTE out13[] = {
			0xdd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const TestDataType TEST_DATA[] =
		{
			{ NULL, 0, NULL, 0, 0 },
			{ NULL, 33, NULL, 0, 0 },
			{ NULL, -33, NULL, 0, 0 },
			{ in2, -33, NULL, 0, 0 },
			{ in2, (int)sizeof(in2), in2, sizeof(in2), in2[0] },
			{ in3, (int)sizeof(in3), in3, sizeof(in3), in3[0] },
			{ in4, (int)sizeof(in4), in4, sizeof(in4), in4[0] },
			{ in5, (int)sizeof(in5), in5, sizeof(in5), in5[0] },
			{ in6, (int)sizeof(in6), in6, sizeof(in6), in6[0] },
			{ in7, (int)sizeof(in7), in7, sizeof(in7), in7[0] },
			{ in8, (int)sizeof(in8), in8, sizeof(in8), in8[0] },
			{ in9, (int)sizeof(in9), in9, sizeof(in9), in9[0] },
			{ in10, (int)sizeof(in10), in10, sizeof(in10), in10[0] },
			{ in11, (int)sizeof(in11), out11, sizeof(out11), in11[0] },
			{ in12, (int)sizeof(in12), out12, sizeof(out12), in12[0] },
			{ in13, (int)sizeof(in13), out13, sizeof(out13), in13[0] },
		};

		// Setup.
		CCardConfig objA;

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			objA.SetPlayerCardPrefixes(TEST_DATA[testIndex].m_playerCardPrefixes, TEST_DATA[testIndex].m_prefixCount);

			CCardConfig objB;
			objB.SetPlayerCardPrefixes(TEST_DATA[testIndex].m_playerCardPrefixes, TEST_DATA[testIndex].m_prefixCount);

			// Test(s).
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				TEST_DATA[testIndex].m_playerCardPrefixesExpected,
				TEST_DATA[testIndex].m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				TEST_DATA[testIndex].m_rcclCardPlayerPrefixExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				TEST_DATA[testIndex].m_playerCardPrefixesExpected,
				TEST_DATA[testIndex].m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				TEST_DATA[testIndex].m_rcclCardPlayerPrefixExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}
#endif

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetGlobalCardPrefix()
	///     CCardConfig::GetGlobalCardPrefix()
	/// </summary>
	TEST(CardConfig, SetGlobalCardPrefixTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE m_globalCardPrefix;

			// Expected results.
			const BYTE m_globalCardPrefixExpected;
			const BYTE m_oldGlobalCardPrefixExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

			{ 0, 0, (BYTE)-1 },
			{ 1, 1, 0 },
			{ 2, 2, 1 },

			{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 2 },
			{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
			{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
			{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
			{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
			{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			BYTE m_oldGlobalCardPrefixResultA(objA.SetGlobalCardPrefix(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_globalCardPrefix));
			AssertResult2(m_oldGlobalCardPrefixResultA, m_globalCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			BYTE oldGlobalCardPrefixResultA = objA.SetGlobalCardPrefix(TEST_DATA[testIndex].m_globalCardPrefix);

			CCardConfig objB;
			BYTE oldGlobalCardPrefixResultB(objB.SetGlobalCardPrefix(TEST_DATA[testIndex].m_globalCardPrefix));

			// Test(s).
			AssertResult2(oldGlobalCardPrefixResultA, TEST_DATA[testIndex].m_oldGlobalCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				TEST_DATA[testIndex].m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldGlobalCardPrefixResultB, m_globalCardPrefixExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				TEST_DATA[testIndex].m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetMechanicCardPrefix()
	///     CCardConfig::GetMechanicCardPrefix()
	/// </summary>
	TEST(CardConfig, SetMechanicCardPrefixTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE m_mechanicCardPrefix;

			// Expected results.
			const BYTE m_mechanicCardPrefixExpected;
			const BYTE m_oldMechanicCardPrefixExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

			{ 0, 0, (BYTE)-1 },
			{ 1, 1, 0 },
			{ 2, 2, 1 },

			{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 2 },
			{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
			{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
			{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
			{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
			{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			BYTE m_oldMechanicCardPrefixResultA(objA.SetMechanicCardPrefix(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_mechanicCardPrefix));
			AssertResult2(m_oldMechanicCardPrefixResultA, m_mechanicCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			BYTE oldMechanicCardPrefixResultA = objA.SetMechanicCardPrefix(TEST_DATA[testIndex].m_mechanicCardPrefix);

			CCardConfig objB;
			BYTE oldMechanicCardPrefixResultB(objB.SetMechanicCardPrefix(TEST_DATA[testIndex].m_mechanicCardPrefix));

			// Test(s).
			AssertResult2(oldMechanicCardPrefixResultA, TEST_DATA[testIndex].m_oldMechanicCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				TEST_DATA[testIndex].m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldMechanicCardPrefixResultB, m_mechanicCardPrefixExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				TEST_DATA[testIndex].m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetFloorCardPrefix()
	///     CCardConfig::GetFloorCardPrefix()
	/// </summary>
	TEST(CardConfig, SetFloorCardPrefixTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE m_floorCardPrefix;

			// Expected results.
			const BYTE m_floorCardPrefixExpected;
			const BYTE m_oldFloorCardPrefixExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

			{ 0, 0, (BYTE)-1 },
			{ 1, 1, 0 },
			{ 2, 2, 1 },

			{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 2 },
			{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
			{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
			{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
			{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
			{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			BYTE m_oldFloorCardPrefixResultA(objA.SetFloorCardPrefix(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_floorCardPrefix));
			AssertResult2(m_oldFloorCardPrefixResultA, m_floorCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			BYTE oldFloorCardPrefixResultA = objA.SetFloorCardPrefix(TEST_DATA[testIndex].m_floorCardPrefix);

			CCardConfig objB;
			BYTE oldFloorCardPrefixResultB(objB.SetFloorCardPrefix(TEST_DATA[testIndex].m_floorCardPrefix));

			// Test(s).
			AssertResult2(oldFloorCardPrefixResultA, TEST_DATA[testIndex].m_oldFloorCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				TEST_DATA[testIndex].m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldFloorCardPrefixResultB, m_floorCardPrefixExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				TEST_DATA[testIndex].m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::CCardConfig()
	///     CCardConfig::SetTempCardPrefix()
	///     CCardConfig::GetTempCardPrefix()
	/// </summary>
	TEST(CardConfig, SetTempCardPrefixTest)
	{
		typedef struct
		{
			// Inputs.
			const BYTE m_tempCardPrefix;

			// Expected results.
			const BYTE m_tempCardPrefixExpected;
			const BYTE m_oldTempCardPrefixExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

			{ 0, 0, (BYTE)-1 },
			{ 1, 1, 0 },
			{ 2, 2, 1 },

			{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 2 },
			{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
			{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
			{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
			{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
			{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			BYTE m_oldTempCardPrefixResultA(objA.SetTempCardPrefix(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_tempCardPrefix));
			AssertResult2(m_oldTempCardPrefixResultA, m_tempCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			BYTE oldTempCardPrefixResultA = objA.SetTempCardPrefix(TEST_DATA[testIndex].m_tempCardPrefix);

			CCardConfig objB;
			BYTE oldTempCardPrefixResultB(objB.SetTempCardPrefix(TEST_DATA[testIndex].m_tempCardPrefix));

			// Test(s).
			AssertResult2(oldTempCardPrefixResultA, TEST_DATA[testIndex].m_oldTempCardPrefixExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				TEST_DATA[testIndex].m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldTempCardPrefixResultB, m_tempCardPrefixExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				TEST_DATA[testIndex].m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	/// CCardConfig::CCardConfig()
	/// CCardConfig::SetUniverseEnabled()
	/// CCardConfig::GetUniverseEnabled()
	/// </summary>
	TEST(CardConfig, SetUniverseEnabledTest)
	{
		typedef struct
		{
			// Inputs.
			const bool m_universeEnabled;

			// Expected results.
			const bool m_universeEnabledExpected;
			const bool m_oldUniverseEnabledExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ true, true, false },
			{ true, true, true },
			{ false, false, true },
			{ false, false, false },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			bool oldUniverseEnabledResultA(objA.SetUniverseEnabled(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_universeEnabled));
			AssertResult2(oldUniverseEnabledResultA, m_universeEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			bool oldUniverseEnabledResultA = objA.SetUniverseEnabled(TEST_DATA[testIndex].m_universeEnabled);

			CCardConfig objB;
			bool oldUniverseEnabledResultB(objB.SetUniverseEnabled(TEST_DATA[testIndex].m_universeEnabled));

			// Test(s).
			AssertResult2(oldUniverseEnabledResultA, TEST_DATA[testIndex].m_oldUniverseEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				TEST_DATA[testIndex].m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldUniverseEnabledResultB, m_universeEnabledExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				TEST_DATA[testIndex].m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

	/// <summary>
	/// Code coverage and functional test for:
	/// CCardConfig::CCardConfig()
	/// CCardConfig::SetMixedModeEnabled()
	/// CCardConfig::GetMixedModeEnabled()
	/// </summary>
	TEST(CardConfig, SetMixedModeEnabledTest)
	{
		typedef struct
		{
			// Inputs.
			const bool m_mixedModeEnabled;

			// Expected results.
			const bool m_mixedModeEnabledExpected;
			const bool m_oldMixedModeEnabledExpected;
		} TestDataType;

		const TestDataType TEST_DATA[] =
		{
			{ true, true, false },
			{ true, true, true },
			{ false, false, true },
			{ false, false, false },
		};

		// Setup.
		CCardConfig objA;
		{
			const int testIndex(0);
			bool oldMixedModeEnabledResultA(objA.SetMixedModeEnabled(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_mixedModeEnabled));
			AssertResult2(oldMixedModeEnabledResultA, m_mixedModeEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			bool oldMixedModeEnabledResultA = objA.SetMixedModeEnabled(TEST_DATA[testIndex].m_mixedModeEnabled);

			CCardConfig objB;
			bool oldMixedModeEnabledResultB(objB.SetMixedModeEnabled(TEST_DATA[testIndex].m_mixedModeEnabled));

			// Test(s).
			AssertResult2(oldMixedModeEnabledResultA, TEST_DATA[testIndex].m_oldMixedModeEnabledExpected);
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				TEST_DATA[testIndex].m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertResult2(oldMixedModeEnabledResultB, m_mixedModeEnabledExpected);
			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				TEST_DATA[testIndex].m_mixedModeEnabledExpected,
				m_universeCardPrefixExpected,
				m_universeCardPrefixCountExpected,
				m_universePropCodesExpected,
				m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				m_useSDSCardFormatExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}

#if 000//TODO-PORT
	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::SetUniversePrefixes()
	///     CCardConfig::GetUniverseCardPrefix()
	///     CCardConfig::GetUniversePropCodes()
	/// </summary>
	TEST(CardConfig, SetUniversePrefixesTest)
	{
		// Expected result(s).
		BYTE universePropCodesExpected[] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0
		};
		ASSERT_TRUE(sizeof(universePropCodesExpected) == SIZE_OF_VALID_CARDS_PREFIX * SIZE_OF_VALID_PROP_CODE) << "sizeof(universePropCodesExpected)";

		typedef struct
		{
			// Inputs.
			const BYTE* const m_universePrefixes;
			const int m_prefixCount;

			// Expected results.
			const BYTE* const m_universeCardPrefixExpected;
			const int m_universeCardPrefixCountExpected;
			const int m_universePropCodesCountExpected;
		} TestDataType;

		const BYTE in2[] = { 0x00 };
		const BYTE in3[] = { 0x7f };
		const BYTE in4[] = { 0x80 };
		const BYTE in5[] = { 0xff };
		const BYTE in6[] = { 0x12, 0x34 };
		const BYTE in7[] = { 0x23, 0x45, 0x67 };
		const BYTE in8[] = { 0x00, 0x7f, 0x80, 0xff };
		const BYTE in9[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e };
		const BYTE in10[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

		const BYTE in11[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };
		const BYTE out11[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const BYTE in12[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21 };
		const BYTE out12[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const BYTE in13[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20, 0x21, 0x22 };
		const BYTE out13[] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
			0x20 };

		const TestDataType TEST_DATA[] =
		{
			{ NULL, 0, NULL, 0, 0 },
			{ NULL, 33, NULL, 0, 0 },
			{ NULL, -33, NULL, 0, 0 },
			{ in2, -33, NULL, 0, 0 },
			{ in2, (int)sizeof(in2), in2, sizeof(in2), sizeof(in2) },
			{ in3, (int)sizeof(in3), in3, sizeof(in3), sizeof(in3) },
			{ in4, (int)sizeof(in4), in4, sizeof(in4), sizeof(in4) },
			{ in5, (int)sizeof(in5), in5, sizeof(in5), sizeof(in5) },
			{ in6, (int)sizeof(in6), in6, sizeof(in6), sizeof(in6) },
			{ in7, (int)sizeof(in7), in7, sizeof(in7), sizeof(in7) },
			{ in8, (int)sizeof(in8), in8, sizeof(in8), sizeof(in8) },
			{ in9, (int)sizeof(in9), in9, sizeof(in9), sizeof(in9) },
			{ in10, (int)sizeof(in10), in10, sizeof(in10), sizeof(in10) },
			{ in11, (int)sizeof(in11), out11, sizeof(out11), sizeof(out11) },
			{ in12, (int)sizeof(in12), out12, sizeof(out12), sizeof(out12) },
			{ in13, (int)sizeof(in13), out13, sizeof(out13), sizeof(out13) },
		};

		// Setup.
		CCardConfig objA;

		for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
		{
			// Perform operation(s) to be tested.
			objA.SetUniversePrefixes(TEST_DATA[testIndex].m_universePrefixes, TEST_DATA[testIndex].m_prefixCount);

			CCardConfig objB;
			objB.SetUniversePrefixes(TEST_DATA[testIndex].m_universePrefixes, TEST_DATA[testIndex].m_prefixCount);

			// Test(s).
			AssertEqual(objA,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				TEST_DATA[testIndex].m_universeCardPrefixExpected,
				TEST_DATA[testIndex].m_universeCardPrefixCountExpected,
				universePropCodesExpected,
				TEST_DATA[testIndex].m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				testIndex, __TFUNCTION__, __LINE__);

			AssertEqual(objB,
				m_commIDExpected,
				m_rcclCardsEnabledExpected,
				m_nclCardsEnabledExpected,
				m_playerCardPrefixesExpected,
				m_prefixCountExpected,
				m_globalCardPrefixExpected,
				m_mechanicCardPrefixExpected,
				m_floorCardPrefixExpected,
				m_tempCardPrefixExpected,
				m_universeEnabledExpected,
				m_mixedModeEnabledExpected,
				TEST_DATA[testIndex].m_universeCardPrefixExpected,
				TEST_DATA[testIndex].m_universeCardPrefixCountExpected,
				universePropCodesExpected,
				TEST_DATA[testIndex].m_universePropCodesCountExpected,
				m_cardReaderTrackNumberExpected,
				m_rcclCardPlayerPrefixExpected,
				testIndex, __TFUNCTION__, __LINE__);
		}
	}


	/// <summary>
	/// Code coverage and functional test for:
	///     CCardConfig::SetUniversePropCodes()
	///     CCardConfig::GetUniversePropCodes()
	///     CCardConfig::GetUniverseCardPrefix()
	/// </summary>
	TEST(CardConfig, SetUniversePropCodesTest)
	{
		// Expected result(s).
		BYTE universeCardPrefixExpected[] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0
		};
		ASSERT_TRUE(sizeof(universeCardPrefixExpected) == SIZE_OF_VALID_CARDS_PREFIX) << "sizeof(universeCardPrefixExpected)";

		typedef struct
		{
			// Inputs.
			const BYTE* const m_universePropCodes;
			const int m_propCodeCount;

			// Expected results.
			{ buffer2, sizeof(buffer2), sizeof(buffer2), buffer2
				const BYTE* const m_universePropCodesExpected;
			const int m_universePropCodesCountExpected;
			const int m_universeCardPrefixCountExpected;
			} TestDataType;

			const BYTE in2[] = { 0x00, 0x00 };
			const BYTE in3[] = { 0x7f, 0x7f };
			const BYTE in4[] = { 0x80, 0x80 };
			const BYTE in5[] = { 0xff, 0xff };
			const BYTE in6[] = { 0x12, 0x34, 0x56, 0x78 };
			const BYTE in7[] = { 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
			const BYTE in8[] = { 0x00, 0x00, 0x7f, 0x7f, 0x80, 0x80, 0xff, 0xff };
			const BYTE in9[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
			};
			const BYTE in10[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f };

			const BYTE in11[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41 };
			{ buffer2, sizeof(buffer2), sizeof(buffer2), buffer2
				const BYTE out11[] = {
					0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
					0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
					0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
					0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
					0x40, 0x41 };

			const BYTE in12[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43 };
			const BYTE out12[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41 };

			const BYTE in13[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41, 0x42, 0x43, 0x44, 0x45 };
			const BYTE out13[] = {
				0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
				0x40, 0x41 };

			const TestDataType TEST_DATA[] =
			{
				{ NULL, 0, NULL, 0, 0 },
				{ NULL, 33, NULL, 0, 0 },
				{ NULL, -33, NULL, 0, 0 },
				{ in2, -33, NULL, 0, 0 },
				{ in2, (int)sizeof(in2) / SIZE_OF_VALID_PROP_CODE, in2, sizeof(in2) / SIZE_OF_VALID_PROP_CODE, sizeof(in2) / SIZE_OF_VALID_PROP_CODE },
				{ in3, (int)sizeof(in3) / SIZE_OF_VALID_PROP_CODE, in3, sizeof(in3) / SIZE_OF_VALID_PROP_CODE, sizeof(in3) / SIZE_OF_VALID_PROP_CODE },
				{ in4, (int)sizeof(in4) / SIZE_OF_VALID_PROP_CODE, in4, sizeof(in4) / SIZE_OF_VALID_PROP_CODE, sizeof(in4) / SIZE_OF_VALID_PROP_CODE },
				{ in5, (int)sizeof(in5) / SIZE_OF_VALID_PROP_CODE, in5, sizeof(in5) / SIZE_OF_VALID_PROP_CODE, sizeof(in5) / SIZE_OF_VALID_PROP_CODE },
				{ in6, (int)sizeof(in6) / SIZE_OF_VALID_PROP_CODE, in6, sizeof(in6) / SIZE_OF_VALID_PROP_CODE, sizeof(in6) / SIZE_OF_VALID_PROP_CODE },
				{ in7, (int)sizeof(in7) / SIZE_OF_VALID_PROP_CODE, in7, sizeof(in7) / SIZE_OF_VALID_PROP_CODE, sizeof(in7) / SIZE_OF_VALID_PROP_CODE },
				{ in8, (int)sizeof(in8) / SIZE_OF_VALID_PROP_CODE, in8, sizeof(in8) / SIZE_OF_VALID_PROP_CODE, sizeof(in8) / SIZE_OF_VALID_PROP_CODE },
				{ in9, (int)sizeof(in9) / SIZE_OF_VALID_PROP_CODE, in9, sizeof(in9) / SIZE_OF_VALID_PROP_CODE, sizeof(in9) / SIZE_OF_VALID_PROP_CODE },
				{ in10, (int)sizeof(in10) / SIZE_OF_VALID_PROP_CODE, in10, sizeof(in10) / SIZE_OF_VALID_PROP_CODE, sizeof(in10) / SIZE_OF_VALID_PROP_CODE },
				{ in11, (int)sizeof(in11) / SIZE_OF_VALID_PROP_CODE, out11, sizeof(out11) / SIZE_OF_VALID_PROP_CODE, sizeof(out11) / SIZE_OF_VALID_PROP_CODE },
				{ in12, (int)sizeof(in12) / SIZE_OF_VALID_PROP_CODE, out12, sizeof(out12) / SIZE_OF_VALID_PROP_CODE, sizeof(out12) / SIZE_OF_VALID_PROP_CODE },
				{ in13, (int)sizeof(in13) / SIZE_OF_VALID_PROP_CODE, out13, sizeof(out13) / SIZE_OF_VALID_PROP_CODE, sizeof(out13) / SIZE_OF_VALID_PROP_CODE },
			};
			ASSERT_TRUE(2 == SIZE_OF_VALID_PROP_CODE) << "2 == SIZE_OF_VALID_PROP_CODE";

			// Setup.
			CCardConfig objA;

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Perform operation(s) to be tested.
				objA.SetUniversePropCodes(TEST_DATA[testIndex].m_universePropCodes, TEST_DATA[testIndex].m_propCodeCount);

				CCardConfig objB;
				objB.SetUniversePropCodes(TEST_DATA[testIndex].m_universePropCodes, TEST_DATA[testIndex].m_propCodeCount);

				// Test(s).
				AssertEqual(objA,
					m_commIDExpected,
					m_rcclCardsEnabledExpected,
					m_nclCardsEnabledExpected,
					m_playerCardPrefixesExpected,
					m_prefixCountExpected,
					m_globalCardPrefixExpected,
					m_mechanicCardPrefixExpected,
					m_floorCardPrefixExpected,
					m_tempCardPrefixExpected,
					m_universeEnabledExpected,
					m_mixedModeEnabledExpected,
					universeCardPrefixExpected,
					TEST_DATA[testIndex].m_universeCardPrefixCountExpected,
					TEST_DATA[testIndex].m_universePropCodesExpected,
					TEST_DATA[testIndex].m_universePropCodesCountExpected,
					m_cardReaderTrackNumberExpected,
					m_rcclCardPlayerPrefixExpected,
					testIndex, __TFUNCTION__, __LINE__);

				AssertEqual(objB,
					m_commIDExpected,
					m_rcclCardsEnabledExpected,
					m_nclCardsEnabledExpected,
					m_playerCardPrefixesExpected,
					m_prefixCountExpected,
					m_globalCardPrefixExpected,
					m_mechanicCardPrefixExpected,
					m_floorCardPrefixExpected,
					m_tempCardPrefixExpected,
					m_universeEnabledExpected,
					m_mixedModeEnabledExpected,
					universeCardPrefixExpected,
					TEST_DATA[testIndex].m_universeCardPrefixCountExpected,
					TEST_DATA[testIndex].m_universePropCodesExpected,
					TEST_DATA[testIndex].m_universePropCodesCountExpected,
					m_cardReaderTrackNumberExpected,
					m_rcclCardPlayerPrefixExpected,
					testIndex, __TFUNCTION__, __LINE__);
			}
			}

#endif//TODO-PORT
			/// <summary>
			/// Code coverage and functional test for:
			///     CCardConfig::CCardConfig()
			///     CCardConfig::SetCardReaderTrackNumber()
			///     CCardConfig::GetCardReaderTrackNumber()
			/// </summary>
			TEST(CardConfig, SetCardReaderTrackNumberTest)
			{
				typedef struct
				{
					// Inputs.
					const BYTE m_cardReaderTrackNumber;

					// Expected results.
					const BYTE m_cardReaderTrackNumberExpected;
					const BYTE m_oldCardReaderTrackNumberExpected;
				} TestDataType;

				const TestDataType TEST_DATA[] =
				{
					{ (BYTE)-1, (BYTE)-1, UCHAR_MAX },

					{ 0, 0, (BYTE)-1 },
					{ 1, 1, 0 },
					{ 2, 2, 1 },
					{ 3, 3, 2 },
					{ 4, 4, 3 },
					{ 5, 5, 4 },

					{ (BYTE)(CHAR_MAX - 1), (BYTE)(CHAR_MAX - 1), 5 },
					{ CHAR_MAX, CHAR_MAX, (BYTE)(CHAR_MAX - 1) },
					{ (BYTE)CHAR_MIN, (BYTE)CHAR_MIN, CHAR_MAX },
					{ (BYTE)(CHAR_MIN + 1), (BYTE)(CHAR_MIN + 1), (BYTE)CHAR_MIN },
					{ (BYTE)(UCHAR_MAX - 1), (BYTE)(UCHAR_MAX - 1), (BYTE)(CHAR_MIN + 1) },
					{ UCHAR_MAX, UCHAR_MAX, (BYTE)(UCHAR_MAX - 1) },
				};

				// Setup.
				CCardConfig objA;
				{
					const int testIndex(0);
					BYTE m_oldCardReaderTrackNumberResultA(objA.SetCardReaderTrackNumber(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_cardReaderTrackNumber));
					AssertResult2(m_oldCardReaderTrackNumberResultA, m_cardReaderTrackNumberExpected);
					AssertEqual(objA,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						m_useSDSCardFormatExpected,
						testIndex, __TFUNCTION__, __LINE__);
				}

				for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
				{
					// Perform operation(s) to be tested.
					BYTE oldCardReaderTrackNumberResultA = objA.SetCardReaderTrackNumber(TEST_DATA[testIndex].m_cardReaderTrackNumber);

					CCardConfig objB;
					BYTE oldCardReaderTrackNumberResultB(objB.SetCardReaderTrackNumber(TEST_DATA[testIndex].m_cardReaderTrackNumber));

					// Test(s).
					AssertResult2(oldCardReaderTrackNumberResultA, TEST_DATA[testIndex].m_oldCardReaderTrackNumberExpected);
					AssertEqual(objA,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						TEST_DATA[testIndex].m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						m_useSDSCardFormatExpected,
						testIndex, __TFUNCTION__, __LINE__);

					AssertResult2(oldCardReaderTrackNumberResultB, m_cardReaderTrackNumberExpected);
					AssertEqual(objB,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						TEST_DATA[testIndex].m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						m_useSDSCardFormatExpected,
						testIndex, __TFUNCTION__, __LINE__);
				}
			}


			/// <summary>
			/// Code coverage and functional test for:
			///     CCardConfig::CCardConfig()
			///     CCardConfig::SetUseSDSCardFormat()
			///     CCardConfig::GetUseSDSCardFormat()
			/// </summary>
			TEST(CardConfig, SetUseSDSCardFormatTest)
			{
				typedef struct
				{
					// Inputs.
					const bool m_useSDSCardFormat;

					// Expected results.
					const bool m_useSDSCardFormatExpected;
					const bool m_oldUseSDSCardFormatExpected;
				} TestDataType;

				const TestDataType TEST_DATA[] =
				{
					{ true, true, false },
					{ true, true, true },
					{ false, false, true },
					{ false, false, false },
				};

				// Setup.
				CCardConfig objA;
				{
					const int testIndex(0);
					bool oldUseSDSCardFormatResultA(objA.SetUseSDSCardFormat(TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_useSDSCardFormat));
					AssertResult2(oldUseSDSCardFormatResultA, m_useSDSCardFormatExpected);
					AssertEqual(objA,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						TEST_DATA[(0 + _countof(TEST_DATA) - 1) % _countof(TEST_DATA)].m_useSDSCardFormatExpected,
						testIndex, __TFUNCTION__, __LINE__);
				}

				for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
				{
					// Perform operation(s) to be tested.
					bool oldUseSDSCardFormatResultA = objA.SetUseSDSCardFormat(TEST_DATA[testIndex].m_useSDSCardFormat);

					CCardConfig objB;
					bool oldUseSDSCardFormatResultB(objB.SetUseSDSCardFormat(TEST_DATA[testIndex].m_useSDSCardFormat));

					// Test(s).
					AssertResult2(oldUseSDSCardFormatResultA, TEST_DATA[testIndex].m_oldUseSDSCardFormatExpected);
					AssertEqual(objA,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						TEST_DATA[testIndex].m_useSDSCardFormat,
						testIndex, __TFUNCTION__, __LINE__);

					AssertResult2(oldUseSDSCardFormatResultB, m_useSDSCardFormatExpected);
					AssertEqual(objB,
						m_commIDExpected,
						m_rcclCardsEnabledExpected,
						m_nclCardsEnabledExpected,
						m_playerCardPrefixesExpected,
						m_prefixCountExpected,
						m_globalCardPrefixExpected,
						m_mechanicCardPrefixExpected,
						m_floorCardPrefixExpected,
						m_tempCardPrefixExpected,
						m_universeEnabledExpected,
						m_mixedModeEnabledExpected,
						m_universeCardPrefixExpected,
						m_universeCardPrefixCountExpected,
						m_universePropCodesExpected,
						m_universePropCodesCountExpected,
						m_cardReaderTrackNumberExpected,
						m_rcclCardPlayerPrefixExpected,
						TEST_DATA[testIndex].m_useSDSCardFormatExpected,
						testIndex, __TFUNCTION__, __LINE__);
				}
			}
		}
