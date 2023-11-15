#include "stdafx.h"
#include "../Sentinel/InternalMessage.h"

// Unit testing includes.
#include "ProgressiveProtocol/ProgressiveLevelTestHelpers.h"
#include "../Sentinel/ProgressiveProtocol/ProgressiveConfig.h"
#include "../Sentinel/ProgressiveProtocol/ProgressiveConnectionInfo.h"
#include "../Sentinel/ProgressiveProtocol/ProgressiveHost.h"
#include "../Sentinel/ProgressiveProtocol/ProgressiveLevelForward.h"
#include "UnitTestingUtils.h"

#include "gtest/gtest.h"

using namespace std;

#define AssertResult2i(result, expected) \
	AssertEqual(result, expected, _T(#result), testIndex, __TFUNCTION__, __LINE__);

#define AssertResult2(result, expected) \
	CUnitTestingUtils::AssertEqual(result, expected, _T(#result), testIndex, __TFUNCTION__, __LINE__);

    		// Default expected result(s).
static const WORD m_additionalWordDataExpected = 0;

class InternalMessage : public ::testing::Test
{
};


void AssertEqual(InternalMsgType result,
    InternalMsgType expected,
    LPCTSTR resultName,
    int testIndex,
    LPCTSTR functionName,
    int lineNumber,
    int* extraIndex = NULL)
{
    const bool success(result == expected);
    if (!success)
    {
        string extraIndexStr;
        if (extraIndex != NULL)
        {
            extraIndexStr = FormatString(_T("/%d"), (int)*extraIndex);
        }

        string failedMsg;
        failedMsg = FormatString(_T("%s=%d expected=%d testIndex=%d(%s) in %s(%d)"),
            static_cast<LPCTSTR>(resultName),
            (int)result,
            (int)expected,
            (int)testIndex,
            static_cast<LPCTSTR>(extraIndexStr.c_str()),
            static_cast<LPCTSTR>(functionName),
            (int)lineNumber);
        ASSERT_TRUE(false)<< failedMsg;
    }
}

/// <summary>
/// Test the implementation of:
///     CInternalMessage::SetMessageType().
///     CInternalMessage::GetMessageType().
/// </summary>
TEST_F(InternalMessage, SetMessageTypeTest)
{
    typedef struct
    {
        // Inputs.
        const InternalMsgType m_type;

        // Expected results.
        const InternalMsgType m_typeExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { (InternalMsgType)(-1), (InternalMsgType)(-1) },
        { (InternalMsgType)(0), (InternalMsgType)(0) },

        { INT_MSG_NONE, INT_MSG_NONE },
        { INT_MSG_ALLOW_TICKET_PRINTED, INT_MSG_ALLOW_TICKET_PRINTED },
        { INT_MSG_BUTTON_ACTIVITY, INT_MSG_BUTTON_ACTIVITY },
        { INT_MSG_FINISHED_GAMESLIST_FIRSTPASS, INT_MSG_FINISHED_GAMESLIST_FIRSTPASS },
        { INT_MSG_GET_PROGRESSIVE_GAMES_INFO, INT_MSG_GET_PROGRESSIVE_GAMES_INFO },
        { INT_MSG_PLAYER_GOODBYE, INT_MSG_PLAYER_GOODBYE },
        { INT_MSG_POINTS_OR_CASH_LABEL, INT_MSG_POINTS_OR_CASH_LABEL },
        { INT_MSG_SET_PROGRESSIVE_CONFIG, INT_MSG_SET_PROGRESSIVE_CONFIG },
        { INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO, INT_MSG_SET_PROGRESSIVE_CONNECTION_INFO },
        { INT_MSG_SET_PROGRESSIVE_GAMES_INFO, INT_MSG_SET_PROGRESSIVE_GAMES_INFO },
        { INT_MSG_SET_PROGRESSIVE_HOST, INT_MSG_SET_PROGRESSIVE_HOST },
        { INT_MSG_UPDATE_SAS_INFO, INT_MSG_UPDATE_SAS_INFO },

        { INT_SHOW_POINTS_VS_ACCOUNT_BALANCE, INT_SHOW_POINTS_VS_ACCOUNT_BALANCE },

        { (InternalMsgType)(INT_MAX - 1), (InternalMsgType)(INT_MAX - 1) },
        { (InternalMsgType)(INT_MAX), (InternalMsgType)(INT_MAX) },
        { (InternalMsgType)(INT_MIN), (InternalMsgType)(INT_MIN) },
        { (InternalMsgType)(INT_MIN + 1), (InternalMsgType)(INT_MIN + 1) },
        { (InternalMsgType)(UINT_MAX - 1), (InternalMsgType)(UINT_MAX - 1) },
        { (InternalMsgType)(UINT_MAX), (InternalMsgType)(UINT_MAX) },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CInternalMessage varMsg;

        // Perform operation(s) to be tested.
        varMsg.SetMessageType(TEST_DATA[testIndex].m_type);
        const CInternalMessage &msg(varMsg);
        InternalMsgType typeResult = msg.GetMessageType();

        // Test(s).
        AssertResult2i(typeResult, TEST_DATA[testIndex].m_typeExpected);

        // Cleanup.
    }
}

// #pragma region GetWordData
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetWord()
/// </summary>
TEST_F(InternalMessage, InitMembers_GetWordData_Default)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((WORD)0, msg.GetWordData())<< L"msg.GetWordData()";
}

/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetWordData(false)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetWordData_False)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((WORD)0, msg.GetWordData(false))<< L"msg.GetWordData(false)";
}
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetWordData(true)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetWordData_True)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((WORD)0, msg.GetWordData(true))<< L"msg.GetWordData(true)";
}

/// <summary>
/// Test the implementation of:
///     CInternalMessage::SetData(WORD, WORD).
///     CInternalMessage::GetWordData().
/// Test with default value for the additional WORD data.
/// </summary>
TEST_F(InternalMessage, SetDataWord1Test)
{
    typedef struct
    {
        // Inputs.
        const WORD m_wordData;

        // Expected results.
        const WORD m_wordDataExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { WORD(-1), WORD(-1) },
        { WORD(0), WORD(0) },
        { WORD(INT16_MAX - 1), WORD(INT16_MAX - 1) },
        { WORD(INT16_MAX), WORD(INT16_MAX) },
        { WORD(INT16_MIN), WORD(INT16_MIN) },
        { WORD(INT16_MIN + 1), WORD(INT16_MIN + 1) },
        { WORD_MAX - 1, WORD_MAX - 1 },
        { WORD_MAX, WORD_MAX },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CInternalMessage varMsg;

        // Perform operation(s) to be tested.
        varMsg.SetData(TEST_DATA[testIndex].m_wordData);
        const CInternalMessage &msg(varMsg);
        const WORD wordDataResult = msg.GetWordData(false);
        const WORD additionalWordDataResult = msg.GetWordData(true);

        // Test(s).
        AssertResult2(wordDataResult, TEST_DATA[testIndex].m_wordDataExpected);
        AssertResult2(additionalWordDataResult, m_additionalWordDataExpected);

        // Cleanup.
    }
}

/// <summary>
/// Test the implementation of:
///     CInternalMessage::SetData(WORD, WORD).
///     CInternalMessage::GetWordData().
/// Test with varying values for the additional WORD data.
/// </summary>
TEST_F(InternalMessage, SetDataWord2Test)
{
    typedef struct
    {
        // Inputs.
        const WORD m_wordData;

        // Expected results.
        const WORD m_wordDataExpected;
    } TestDataType;

    const TestDataType TEST_DATA[] =
    {
        { WORD(-1), WORD(-1) },
        { WORD(0), WORD(0) },
        { WORD(INT16_MAX - 1), WORD(INT16_MAX - 1) },
        { WORD(INT16_MAX), WORD(INT16_MAX) },
        { WORD(INT16_MIN), WORD(INT16_MIN) },
        { WORD(INT16_MIN + 1), WORD(INT16_MIN + 1) },
        { WORD_MAX - 1, WORD_MAX - 1 },
        { WORD_MAX, WORD_MAX },
    };

    for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
    {
        // Setup.
        CInternalMessage varMsg;

        // Perform operation(s) to be tested.
        varMsg.SetData(TEST_DATA[testIndex].m_wordData,
            TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_wordData);
        const CInternalMessage &msg(varMsg);
        const WORD wordDataResult = msg.GetWordData(false);
        const WORD additionalWordDataResult = msg.GetWordData(true);

        // Test(s).
        AssertResult2(wordDataResult, TEST_DATA[testIndex].m_wordDataExpected);
        AssertResult2(additionalWordDataResult, TEST_DATA[(testIndex + 1) % _countof(TEST_DATA)].m_wordDataExpected);

        // Cleanup.
    }
}
// #pragma endregion GetWordData

// #pragma region GetInt64Data
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetInt64Data()
/// </summary>
TEST_F(InternalMessage, InitMembers_GetInt64Data_Default)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((__int64)0, msg.GetInt64Data())<< L"msg.GetInt64Data()";
}

/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetInt64Data(false)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetInt64Data_False)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((__int64)0, msg.GetInt64Data(false))<< L"msg.GetInt64Data(false)";
}
// #pragma endregion GetWordData

// #pragma region GetDWordData
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDWordData()
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDWordData_Default)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((DWORD)0, msg.GetDWordData())<< L"msg.GetDWordData()";
}

/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDWordData(false)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDWordData_False)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((DWORD)0, msg.GetDWordData(false))<< L"msg.GetDWordData(false)";
}
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDWordData(true)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDWordData_True)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((DWORD)0, msg.GetDWordData(true))<< L"msg.GetDWordData(true)";
}
// #pragma endregion GetDWordData

// #pragma region GetDoubleData
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDoubleData()
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDoubleData_Default)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((double)0, msg.GetDoubleData())<< L"msg.GetDoubleData()";
}

/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDoubleData(false)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDoubleData_False)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((double)0, msg.GetDoubleData(false))<< L"msg.GetDoubleData(false)";
}
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetDoubleData(true)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetDoubleData_True)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((double)0, msg.GetDoubleData(true))<< L"msg.GetDoubleData(true)";
}
// #pragma endregion GetDoubleData

// #pragma region GetInt64Data
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetInt64Data(true)
/// </summary>
TEST_F(InternalMessage, InitMembers_GetInt64Data_True)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    ASSERT_EQ((__int64)0, msg.GetInt64Data(true))<< L"msg.GetInt64Data(true)";
}
// #pragma endregion GetInt64Data

// #pragma region GetBytesData
/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetBytesData()
/// </summary>
TEST_F(InternalMessage, InitMembers_GetBytesData)
{
    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_NONE);
    size_t size(12345);
    ASSERT_EQ(msg.GetBytesData(size), nullptr)<< L"msg.GetBytesData()";
    ASSERT_EQ((size_t)0, size)<< L"size";
}
// #pragma endregion GetBytesData

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveConfig returns a NULL pointer when a NULL pointer was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_ConstructorNullTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveConfig returns the same pointer as was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_ConstructorTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj(new CProgressiveConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::SetData(CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
///  Construct NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_SetData00Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj1(NULL);
    CProgressiveConfig* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::SetData(CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
///  Construct NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_SetData01Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj1(NULL);
    CProgressiveConfig* obj2(new CProgressiveConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::SetData(CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_SetData10Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj1(new CProgressiveConfig());
    CProgressiveConfig* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConfig*)
///     CInternalMessage::SetData(CProgressiveConfig*)
///     CInternalMessage::GetProgressiveConfig()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConfig_SetData11Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConfig* obj1(new CProgressiveConfig());
    CProgressiveConfig* obj2(new CProgressiveConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConfig(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}



/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveConnectionInfo returns a NULL pointer when a NULL pointer was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_ConstructorNullTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveConnectionInfo returns the same pointer as was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_ConstructorTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj(new CProgressiveConnectionInfo());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::SetData(CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
///  Construct NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_SetData00Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj1(NULL);
    CProgressiveConnectionInfo* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::SetData(CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
///  Construct NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_SetData01Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj1(NULL);
    CProgressiveConnectionInfo* obj2(new CProgressiveConnectionInfo());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::SetData(CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_SetData10Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj1(new CProgressiveConnectionInfo());
    CProgressiveConnectionInfo* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveConnectionInfo*)
///     CInternalMessage::SetData(CProgressiveConnectionInfo*)
///     CInternalMessage::GetProgressiveConnectionInfo()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveConnectionInfo_SetData11Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveConnectionInfo* obj1(new CProgressiveConnectionInfo());
    CProgressiveConnectionInfo* obj2(new CProgressiveConnectionInfo());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveConnectionInfo(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}

// #pragma region ProgressiveLevelsInfo
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevels_ConstructorTest)
{
    SentinelNativeUnitTests::ProgressiveLevelTestHelpers::_ICardConfig5 cardConfig;
	 rapidjson::Document jsonDoc;
    cardConfig.CreateJsonDoc(jsonDoc);

    ProgressiveLevelsInfo *obj1(new ProgressiveLevelsInfo(jsonDoc));
	//CProgressiveAnnouncementsInfo *obj2(new CProgressiveAnnouncementsInfo(jsonDoc));
	CProgressiveAnnouncementsInfo *obj2(nullptr);  // to correct
    
    CFirmwarePackage* obj3(new CFirmwarePackage(jsonDoc));
    CCampaignConfig* obj4(new CCampaignConfig(jsonDoc));
    CSkinConfig* obj5(new CSkinConfig(jsonDoc));

    CInternalMessage msg(INT_MSG_PROGRESSIVE_CONFIGURATION, obj1, obj2, obj3, obj4, obj5);

    ASSERT_EQ((void*) msg.GetProgressiveLevelsInfo(), (void*) obj1);
	ASSERT_EQ((void*) msg.GetProgressiveAnnouncementsInfo(), (void*)obj2);
}

TEST_F(InternalMessage, CInternalMessage_ProgressiveLevelsInfo_ConstructorNullTest)
{
    CInternalMessage msg(INT_MSG_PROGRESSIVE_CONFIGURATION, (ProgressiveLevelsInfo*) nullptr, (CProgressiveAnnouncementsInfo*) nullptr, (CFirmwarePackage*) nullptr, (CCampaignConfig*) nullptr, (CSkinConfig*) nullptr);
    ASSERT_EQ(msg.GetProgressiveLevelsInfo(), nullptr);
    ASSERT_EQ(msg.GetProgressiveAnnouncementsInfo(), nullptr);
}
// #pragma endregion ProgressiveLevelsInfo

// #pragma region CProgressiveLevelsForward
/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveLevelForward()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveData returns a NULL pointer when a NULL pointer was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_ConstructorNullTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveData()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveData returns the same pointer as was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_ConstructorTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj = new CProgressiveLevelForward(0, false);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::SetData(CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveData()
///  Construct NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_SetData00Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj1(NULL);
    CProgressiveLevelForward* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::SetData(CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveData()
///  Construct NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_SetData01Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj1(NULL);
    CProgressiveLevelForward* obj2 = new CProgressiveLevelForward(0, false);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::SetData(CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveData()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_SetData10Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj1 = new CProgressiveLevelForward(0, false);
    CProgressiveLevelForward* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveLevelForward*)
///     CInternalMessage::SetData(CProgressiveLevelForward*)
///     CInternalMessage::GetProgressiveData()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveLevelForward_SetData11Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveLevelForward* obj1 = new CProgressiveLevelForward(0, false);
    CProgressiveLevelForward* obj2 = new CProgressiveLevelForward(0, false);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveLevelForward(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}
// #pragma endregion CProgressiveLevelForward



/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveHost returns a NULL pointer when a NULL pointer was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_ConstructorNullTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetProgressiveHost returns the same pointer as was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_ConstructorTest)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj(new CProgressiveHost(_T(""), 0));

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::SetData(CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
///  Construct NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_SetData00Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj1(NULL);
    CProgressiveHost* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::SetData(CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
///  Construct NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_SetData01Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj1(NULL);
    CProgressiveHost* obj2(new CProgressiveHost(_T(""), 0));

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj2);

    // Cleanup.
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::SetData(CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_SetData10Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj1(new CProgressiveHost(_T(""), 0));
    CProgressiveHost* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CProgressiveHost*)
///     CInternalMessage::SetData(CProgressiveHost*)
///     CInternalMessage::GetProgressiveHost()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CProgressiveHost_SetData11Test)
{
    // Setup.
    const int testIndex(0);
    CProgressiveHost* obj1(new CProgressiveHost(_T(""), 0));
    CProgressiveHost* obj2(new CProgressiveHost(_T(""), 0));

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE, obj1);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj1);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetProgressiveHost(), (const void*)obj2);

    // Cleanup.
    delete obj1;
}



/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(INT_MSG_NONE)
///     CInternalMessage::SetMessageType()
///     CInternalMessage::GetMessageType()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the CInternalMessage constructor uses a NULL pointer for its CUIConfig instance, by default.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_UpdateUIConfigTest)
{
    // Setup.
    const int testIndex(0);
    CInternalMessage msg(INT_MSG_NONE);
    AssertResult2(msg.GetMessageType(), INT_MSG_NONE);

    // Perform operation(s) to be tested.
    msg.SetMessageType(INT_MSG_UPDATE_UI_CONFIG);

    // Test(s).
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);
}
/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(INT_MSG_NONE)
///     CInternalMessage::GetUIConfig()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the CInternalMessage constructor uses a NULL pointer for its CUIConfig instance, by default.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_NoneTest)
{
    // Setup.
    const int testIndex(0);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_NONE);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)NULL);
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetUIConfig()
///     CInternalMessage::GetMessageType()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the CInternalMessage constructor uses a NULL pointer for its CUIConfig instance, by default.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_DefaultTest)
{
    // Setup.
    const int testIndex(0);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)NULL);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::GetUIConfig()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetUIConfig returns a NULL pointer when a NULL pointer was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_ConstructorNullTest)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::GetUIConfig()
/// Code coverage for:
///     CInternalMessage::~CInternalMessage()
/// Test that the GetUIConfig returns the same pointer as was passed to the CInternalMessage constructor.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_ConstructorTest)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj(new CUIConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::SetData(CUIConfig*)
///     CInternalMessage::GetUIConfig()
///  Construct NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_SetData00Test)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj1(NULL);
    CUIConfig* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj1);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj1);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj2);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG)
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::SetData(CUIConfig*)
///     CInternalMessage::GetUIConfig()
///  Construct NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_SetData01Test)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj1(NULL);
    CUIConfig* obj2(new CUIConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj1);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj1);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj2);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::SetData(CUIConfig*)
///     CInternalMessage::GetUIConfig()
///  Construct non-NULL, set NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_SetData10Test)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj1(new CUIConfig());
    CUIConfig* obj2(NULL);

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj1);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj1);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj2);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Code coverage and functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType, CUIConfig*)
///     CInternalMessage::SetData(CUIConfig*)
///     CInternalMessage::GetUIConfig()
///  Construct non-NULL, set non-NULL.
/// </summary>
TEST_F(InternalMessage, CInternalMessage_CUIConfig_SetData11Test)
{
    // Setup.
    const int testIndex(0);
    CUIConfig* obj1(new CUIConfig());
    CUIConfig* obj2(new CUIConfig());

    // Perform operation(s) to be tested.
    CInternalMessage msg(INT_MSG_UPDATE_UI_CONFIG, obj1);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj1);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Perform operation(s) to be tested.
    msg.SetData(obj2);

    // Test(s).
    AssertResult2(msg.GetUIConfig(), (const void*)obj2);
    AssertResult2(msg.GetMessageType(), INT_MSG_UPDATE_UI_CONFIG);

    // Cleanup.
    delete obj1;
}

/// <summary>
/// Functional test for:
///     CInternalMessage::CInternalMessage(InternalMsgType)
///     CInternalMessage::GetMessageType()
/// </summary>
TEST_F(InternalMessage, CInternalMessage_SendProgressiveLevelForward)
{
    // Setup.
    const int testIndex(0);

    // Perform operation(s) and test(s).
    CInternalMessage msg(INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD);
    AssertResult2(msg.GetMessageType(), INT_MSG_SEND_PROGRESSIVE_LEVEL_FORWARD);
}