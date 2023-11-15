#include "stdafx.h"
#include "ProgressiveProtocol/ProgressiveLevelManager.h"
#include "ProgressiveLevelTestHelpers.h"
#include "AssertMsg.h"

using namespace std;

#include "gtest/gtest.h"

using namespace SentinelNativeUnitTests::ProgressiveLevelTestHelpers;

namespace SentinelNativeUnitTests
{
    class ProgressiveLevelManagerTests2 : public ::testing::Test {
        protected:
			virtual void SetUp() {
			}
			virtual void TearDown() 
			{
			}
            void test_ProgressiveLevelManager_multicastActive_DefaultJson();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_nonstale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_stale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_nonstale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_stale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_nonstale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_stale();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardTrue();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardFalse();
            void test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward();
            void test_MulticastActive_PublicGetLevelForward_Default();
            void test_MulticastActive_PublicGetLevelForward_TrueSame_FalseSame_TrueSame();

    };
	TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_multicastActive_DefaultJson)
    {
        test_ProgressiveLevelManager_multicastActive_DefaultJson();
    }
    void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_multicastActive_DefaultJson()
		{
			const DWORD configTickCount = 10000;

			_ICardConfig5 cardConfig;
			shared_ptr<ProgressiveLevelManager> levelManager = cardConfig.CreateLevelManager(configTickCount);

			ASSERT_TRUE(levelManager->GetMulticastActive());
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::CProgressiveLevels(DWORD receivedTickCount = 0, const SYSTEMTIME* pReceivedTime = nullptr)
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test with non-stale Standard level.
		/// </summary>
		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_nonstale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount		 = configTickCount;
			levelManager.m_forwardedTickCount	 = configTickCount;

            LevelData data =
				//TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
				{ __LINE__, configTickCount, 1, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
			levelManager.AddLevel(tempObj);

			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + 1, false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_nonstale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_nonstale();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test with stale Standard level.
		/// </summary>
		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_stale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount		 = configTickCount;
			levelManager.m_forwardedTickCount	 = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                { __LINE__, configTickCount, 1, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
			levelManager.AddLevel(tempObj);

			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + (CProgressiveLevelForward::MaxInterval * 2), false, multicaseActiveChanged));
			ASSERT_FALSE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_TRUE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_stale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standard_stale();
		}

		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_nonstale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::Bonus  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);
            
			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + 1, false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_nonstale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_nonstale();
		}

		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_stale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

			LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::Bonus  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);

			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + (CProgressiveLevelForward::MaxInterval * 2), false, multicaseActiveChanged));
			ASSERT_FALSE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_TRUE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_stale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_bonus_stale();
		}

		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_nonstale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::AccountingOnly  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);
            
			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + 1, false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_nonstale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_nonstale();
		}

		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_stale()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::AccountingOnly  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);
            
			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount + (CProgressiveLevelForward::MaxInterval * 2), false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_stale)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_accountingOnly_stale();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test with an non-stale Standard level and a non-stale Standard level.
		/// </summary>
		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardTrue()
		{
			const DWORD configTickCount = 10000;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);
            
            LevelData data1 =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 2, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj1 = data1.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj1);
            
			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount, false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";

		}
		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardTrue)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardTrue();
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test with an non-stale Standard level and a stale Standard level.
		/// </summary>
		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardFalse()
		{
			const DWORD configTickCount	   = 10000;
			const DWORD tickCountBroadcast = configTickCount + (CProgressiveLevelForward::MaxInterval * 2);
			const DWORD tickCountTest	   = tickCountBroadcast;

			ProgressiveLevelManager levelManager;
			levelManager.m_updatedTickCount = configTickCount;
			levelManager.m_forwardedTickCount = configTickCount;

            LevelData data =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 1, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj = data.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj);
            LevelData data1 =
                //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                {  __LINE__, configTickCount, 2, ProgressiveLevelType::Standard  ,""	 , "", 0, {}, 0 };
            shared_ptr<CProgressiveLevel> tempObj1 = data1.CreateProgressiveLevel();
            levelManager.AddLevel(tempObj1);
            
			{
				CProgressiveLevelBroadcast broadcast(tickCountBroadcast);
				broadcast.AddLevel(1, 12345, INVALID_VALUE, INVALID_VALUE, "Z");
				levelManager.ProcessLevelBroadcast(&broadcast);
			}

			bool multicaseActiveChanged;
			unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(configTickCount, false, multicaseActiveChanged));
			ASSERT_TRUE(levelManager.GetMulticastActive()) << L"GetMulticastActive()";
			ASSERT_FALSE(multicaseActiveChanged) << L"multicaseActiveChanged";
		}

		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardFalse)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward_standardFalse();
		}

		void ProgressiveLevelManagerTests2::test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward()
		{
			const DWORD configTickCount = 10000;

			const DWORD tickCountBroadcast = configTickCount + (CProgressiveLevelForward::MaxInterval * 2);
			const DWORD tickCountTest = tickCountBroadcast;

			struct
			{
				int line;
				ProgressiveLevelType::Type type1;
				ProgressiveLevelType::Type type2;
				DWORD addBroadcastTickCount;
				DWORD addForwardTickCount;
				bool expectedMulticaseActive;
				bool expectedMulticaseActiveChanged;
			}
			data[] =
			{
				{ __LINE__, ProgressiveLevelType::Standard, ProgressiveLevelType::Standard, 0, 0, true, false },
				{ __LINE__, ProgressiveLevelType::Standard, ProgressiveLevelType::Standard, CProgressiveLevelForward::MinInterval, 0, false, true },
				{ __LINE__, ProgressiveLevelType::Standard, ProgressiveLevelType::Standard, CProgressiveLevelForward::MinInterval, CProgressiveLevelForward::MinInterval, false, true },
				{ __LINE__, ProgressiveLevelType::Standard, ProgressiveLevelType::Standard, CProgressiveLevelForward::MaxInterval, 0, false, true },
				{ __LINE__, ProgressiveLevelType::Standard, ProgressiveLevelType::Standard, CProgressiveLevelForward::MaxInterval, CProgressiveLevelForward::MaxInterval, false, true },
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				DWORD tickCount = configTickCount;

				ProgressiveLevelManager levelManager;
				levelManager.m_updatedTickCount   = tickCount;
				levelManager.m_forwardedTickCount = tickCount;

				//levelManager.AddLevel(LevelData{ __LINE__, configTickCount, 1, data[i].type1 ,""	 , "", 0, {}, 0 }.CreateProgressiveLevel());
				//levelManager.AddLevel(LevelData{ __LINE__, configTickCount, 2, data[i].type2 ,""	 , "", 0, {}, 0 }.CreateProgressiveLevel());
                LevelData data2 =
                    //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                    {  __LINE__, configTickCount, 1, data[i].type1   ,""	 , "", 0, {}, 0 };
                shared_ptr<CProgressiveLevel> tempObj2 = data2.CreateProgressiveLevel();
                levelManager.AddLevel(tempObj2);
                LevelData data1 =
                    //TickCount, id, type, pName, pUrl, numSasLevels, sasLevels[3], expectedNumSasLevels;
                    {  __LINE__, configTickCount, 2, data[i].type2  ,""	 , "", 0, {}, 0 };
                shared_ptr<CProgressiveLevel> tempObj1 = data1.CreateProgressiveLevel();
                levelManager.AddLevel(tempObj1);
                
				tickCount += data[i].addBroadcastTickCount;

				{
					CProgressiveLevelBroadcast broadcast(tickCountBroadcast);
					broadcast.AddLevel(1, 12345, INVALID_VALUE, INVALID_VALUE, nullptr);
					levelManager.ProcessLevelBroadcast(&broadcast);
				}

				tickCount += data[i].addForwardTickCount;

				bool multicaseActiveChanged;
				unique_ptr<CProgressiveLevelForward> levelForward(levelManager.GetLevelForward(tickCount, false, multicaseActiveChanged));
				ASSERT_EQ(data[i].expectedMulticaseActive, levelManager.GetMulticastActive()) << AssertMsg::Format(data[i].line, L"GetMulticastActive()");
				ASSERT_EQ(data[i].expectedMulticaseActiveChanged, multicaseActiveChanged) << AssertMsg::Format(data[i].line, L"multicaseActiveChanged");
			}
		}

		TEST_F(ProgressiveLevelManagerTests2, ProgressiveLevelManager_GetMulticastActive_GetLevelForward)
		{
			test_ProgressiveLevelManager_GetMulticastActive_GetLevelForward();
		}
// #pragma endregion CProgressiveLevels::MulticastActive_PrivateGetLevelForward

#if 000//DELETED CLASS
// #pragma region CProgressiveLevels::MulticastActive_PublicGetLevelForward
		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::CProgressiveLevels(DWORD receivedTickCount = 0, const SYSTEMTIME* pReceivedTime = nullptr)
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test default value(s).
		/// Test with and without calls to the private GetLevelForward() method.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests2, MulticastActive_PublicGetLevelForward_Default)
		{
			// Data.
			const DWORD createdTickCount(10000);
			const DWORD updatedTickCount(createdTickCount); // Last update.

			// Perform operation(s) and test(s).
			CProgressiveLevels levels(createdTickCount);

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() != nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() == nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount + CProgressiveLevelForward::MaxInterval * 2, multicastActive));
				AssertResult2(levelForward.get() != nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount + CProgressiveLevelForward::MaxInterval * 2, multicastActive));
				AssertResult2(levelForward.get() == nullptr, true);
				AssertResult2(multicastActive, true);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevels::GetLevelForward(DWORD, CProgressiveLevelForward &)
		/// Test with a bonus level non-stale on the first two calls, then stale on the middle two calls, then non-stale on the last two calls.
		/// Test with and without calls to the private GetLevelForward() method.
		/// </summary>
		TEST_F(ProgressiveLevelManagerTests2, MulticastActive_PublicGetLevelForward_TrueSame_FalseSame_TrueSame)
		{
			// Data.
			const DWORD createdTickCount(10000);
			const DWORD updatedTickCount1(createdTickCount); // First update.
			const DWORD updatedTickCount2(createdTickCount + CProgressiveLevelForward::MaxInterval * 3); // Second update.

			// Setup.
			CProgressiveLevels levels(createdTickCount);
			levels.AddLevel(createdTickCount, 1, ProgressiveLevelType::Bonus, "", "");

			// Perform operation(s) and test(s).
			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount1 + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() != nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount1 + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() == nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount1 + CProgressiveLevelForward::MaxInterval * 2, multicastActive));
				AssertResult2(levelForward.get() != nullptr, true);
				AssertResult2(multicastActive, false);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount1 + CProgressiveLevelForward::MaxInterval * 2, multicastActive));
				AssertResult2(levelForward.get() == nullptr, true);
				AssertResult2(multicastActive, false);
			}

			CProgressiveLevelBroadcast broadcast(updatedTickCount2);
			broadcast.AddLevel(1, 12345, "");
			levels.ProcessLevelBroadcast(&broadcast);

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount2 + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() != nullptr, true);
				AssertResult2(multicastActive, true);
			}

			{
				bool multicastActive;
				unique_ptr<CProgressiveLevelForward> levelForward(
					levels.GetLevelForward(updatedTickCount2 + CProgressiveLevelForward::StrobeInterval, multicastActive));
				AssertResult2(levelForward.get() == nullptr, true);
				AssertResult2(multicastActive, true);
			}

		}
#endif
// #pragma endregion CProgressiveLevels::MulticastActive_PrivateGetLevelForward
}
