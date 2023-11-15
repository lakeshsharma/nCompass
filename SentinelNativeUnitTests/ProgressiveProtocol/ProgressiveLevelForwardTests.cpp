#if 000//TODO-PORT
#include "stdafx.h"
#include "ProgressiveProtocol\ProgressiveLevelForward.h"
#include "ProgressiveProtocol\ProgressiveLevel.h"
#include "ProgressiveProtocol\ProgressiveLevelsInfo.h"
#include "ProgressiveProtocol\ProgressiveLevelManager.h"
#include "ProgressiveLevelTestHelpers.h"

// Unit testing includes.
#include "UnitTestingXmlData.h"
#include "UnitTestingXmlLite.h"
#include "UnitTestingUtils.h"
#include "TestUtil.h"
#include "Util/FileTime.h"

// AssertResult macros for calling CUnitTestingUtils from a test method.
#define AssertResult3(result, expected, resultName) \
	CUnitTestingUtils::AssertEqual(result, expected, resultName, m_testIndex, __TFUNCTION__, __LINE__);
#define AssertResult2(result, expected) AssertResult3(result, expected, _T(#result))

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace SentinelNativeUnitTests
{
	using namespace ProgressiveLevelTestHelpers;

	static const char* pJson =
		"{"
		"\"SessionId\":470,"
		"\"EligibilityDetermined\":true,"
		"\"SourceDeviceId\":1,"
		"\"SourceDeviceConfigVersion\":0,"
		"\"SourceAsset\":\"\","
		"\"Timestamp\":1433466405284"
		"}";

	/// <summary>
	/// Test CProgressiveLevelForward.
	/// </summary>
	TEST_CLASS(ProgressiveLevelForwardTests)
	{
		// Class member variable(s).
		int m_testIndex;

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

		TEST_METHOD(ProgressiveLevelForward_Constructor)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
			Assert::IsTrue(data.cend() == data.cbegin(), L"num levels");
		}

		TEST_METHOD(ProgressiveLevelForward_Constructor2)
		{
			const int groupId = 3;
			const bool useAftLockOnTransfers = false;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
			Assert::IsTrue(data.cend() == data.cbegin(), L"num levels");
		}

		TEST_METHOD(ProgressiveLevelForward_CopyConstructor)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			CProgressiveLevelForward data2(data);

			Assert::AreEqual(groupId, data2.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data2.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
			Assert::IsTrue(data.cend() == data.cbegin(), L"num levels");
		}

		TEST_METHOD(ProgressiveLevelForward_Assignment)
		{
			const int progressiveGroupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(progressiveGroupId, useAftLockOnTransfers);

			const int progressiveGroupId2 = 3;
			const bool useAftLockOnTransfers2 = false;
			CProgressiveLevelForward data2(progressiveGroupId2, useAftLockOnTransfers2);
			data2 = data;

			Assert::AreEqual(progressiveGroupId, data2.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data2.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
			Assert::IsTrue(data2.cend() == data2.cbegin(), L"num levels");
		}

		struct TestData
		{
			BYTE sasLevel;
			__int64 amount;
			ProgressiveLevelType::Type type;
			DWORD forwardedTickCount;
			bool expectedResult;

			void Verify(const CProgressiveLevelForward::Level& level)
			{
				Assert::IsTrue(expectedResult, L"expectedResult");
				Assert::AreEqual(sasLevel, level.sasLevel, L"SAS Level");
				Assert::AreEqual((long) amount, (long) level.amount, L"amount");
				Assert::AreEqual((int) type, (int) level.type, L"type");
			}
		};

		TEST_METHOD(ProgressiveLevelForward_AddLevels)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1,  0,         ProgressiveLevelType::Standard,       123, true  },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus,          456, true  },
				{ 32, 10,        ProgressiveLevelType::AccountingOnly, 789, true  },
				{ 33, WORD_MAX,  ProgressiveLevelType::Standard,       159, false }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			Assert::IsFalse(data.cend() == data.cbegin(), L"num levels");

			size_t test = 0;

			for_each(data.cbegin(), data.cend(),
				[&](const CProgressiveLevelForward::Level& level)
			{
				tests[test++].Verify(level);
			});

			Assert::AreEqual(3, (int) test, L"num levels 2");
		}

		TEST_METHOD(ProgressiveLevelForward_CopyConstructor_Full)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward dataOrg(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1,  0,         ProgressiveLevelType::Standard,       135, true  },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus,          246, true  },
				{ 32, 10,        ProgressiveLevelType::AccountingOnly, 975, true  }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				dataOrg.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			CProgressiveLevelForward data(dataOrg);
			
			Assert::IsFalse(data.cend() == data.cbegin(), L"num levels");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			size_t test = 0;

			for_each(data.cbegin(), data.cend(),
				[&](const CProgressiveLevelForward::Level& level)
			{
				tests[test++].Verify(level);
			});

			Assert::AreEqual(3, (int) test, L"num levels 2");
		}

		TEST_METHOD(ProgressiveLevelForward_AccountingOnly)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1,  0,  ProgressiveLevelType::AccountingOnly, 1234567, true },
				{ 32, 10, ProgressiveLevelType::AccountingOnly, 7654321, true }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			size_t test = 0;

			for_each(data.cbegin(), data.cend(),
				[&](const CProgressiveLevelForward::Level& level)
			{
				tests[test++].Verify(level);
			});

			Assert::AreEqual(2, (int) test, L"num levels");
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::Clear()
		/// Test Clear() with no levels ever present.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_DefaultClear)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			// Perform operation(s) and test(s).
			data.Clear();

			Assert::AreEqual((size_t)0, data.GetNumLevels(), L"Clear: GetNumLevels()");
			Assert::IsTrue(data.cend() == data.cbegin(), L"Clear: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::Clear()
		/// Test Clear() after adding levels.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevelsClear)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus, 456, true },
				{ 31, 10, ProgressiveLevelType::AccountingOnly, 789, true },
				{ 32, WORD_MAX, ProgressiveLevelType::Standard, 159, false }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			data.Clear();

			Assert::AreEqual((size_t)0, data.GetNumLevels(), L"Clear: GetNumLevels()");
			Assert::IsTrue(data.cend() == data.cbegin(), L"Clear: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::Clear()
		/// Test Clear() twice after adding levels.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevelsClear2)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus, 456, true },
				{ 31, 10, ProgressiveLevelType::AccountingOnly, 789, true },
				{ 32, WORD_MAX, ProgressiveLevelType::Standard, 159, false }
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			data.Clear();
			data.Clear();

			Assert::AreEqual((size_t)0, data.GetNumLevels(), L"Clear: GetNumLevels()");
			Assert::IsTrue(data.cend() == data.cbegin(), L"Clear: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");
		}

		struct LevelData
		{
			__int64 levelId;
			ProgressiveLevelType::Type type;
			string url;
			BYTE numSasLevels;
			BYTE sasLevels[2];
		};

		struct ForwardData
		{
			__int64 amount;
			DWORD updatedTickCount;
			ProgressiveLevelType::Type type;
			BYTE sasLevel;
		};

		struct StaleData
		{
			__int64 levelId;
			unsigned staleCountToday;
			unsigned staleMillisecondsToday;
			unsigned staleCountYesterday;
			unsigned staleMillisecondsYesterday;
		};

		struct LevelsData
		{
			ProgressiveLevelManager levels;

			LevelsData(DWORD receivedTickCount, int progressiveGroupId, bool useAftOnTransfers)
			{
				levels.m_updatedTickCount = receivedTickCount;
			}

			void AddLevel(int64_t levelId, ProgressiveLevelType::Type type, LPCSTR pUrl, BYTE numSasLevels, BYTE sasLevel1, BYTE sasLevel2)
			{
				shared_ptr<CProgressiveLevel> level(new CProgressiveLevel);
				level->m_id = levelId;
				level->m_type = type;
				level->m_name = to_wstring(levelId);
				level->m_url = Convert::Utf8ToWStr(pUrl);
				if (numSasLevels > 0)
				{
					level->m_sasLevels.push_back(sasLevel1);
					if (numSasLevels > 1)
					{
						level->m_sasLevels.push_back(sasLevel2);
					}
				}
				levels.AddLevel(level);
			}

			void TestForward(int line, DWORD tickCount, const ForwardData* pForwardExpected, size_t forwardExpectedCount, const SYSTEMTIME& baseTime)
			{
				SYSTEMTIME localTime = FileTime::Add(baseTime, tickCount);

				bool multicastActiveChanged;
				CProgressiveLevelForward* pForward = levels.GetLevelForward(tickCount, false, multicastActiveChanged, &localTime);

				if (pForwardExpected == nullptr)
				{
					Assert::IsNull(pForward, AssertMsg::Format(line, L"GetLevelForward()", L"TickCount: %u", tickCount));
				}
				else
				{
					Assert::IsNotNull(pForward, AssertMsg::Format(line, L"GetLevelForward()", L"TickCount: %u", tickCount));

					Assert::AreEqual((int) forwardExpectedCount, (int) pForward->GetNumLevels(), AssertMsg::Format(line, L"GetNumLevels()", L"TickCount: %u", tickCount));

					if (forwardExpectedCount != 0)
					{
						Assert::AreEqual(levels.GetProgressiveGroupId(), pForward->GetProgressiveGroupId(), AssertMsg::Format(line, L"GetProgressiveGroupId()", L"TickCount: %u", tickCount));
						Assert::AreEqual(levels.UseAftLockOnTransfers(), pForward->UseAftLockOnTransfers(), AssertMsg::Format(line, L"UseAftLockOnTransfers()", L"TickCount: %u", tickCount));

						auto forwardLevelIt = pForward->cbegin();
						for (size_t i = 0; i < forwardExpectedCount; i++)
						{
							Assert::AreEqual((uint64_t) pForwardExpected[i].amount, (uint64_t) forwardLevelIt->amount, AssertMsg::Format(line, nullptr, L"[i].amount: TickCount: %u", i, tickCount));
							Assert::AreEqual((BYTE) pForwardExpected[i].type, (BYTE) forwardLevelIt->type, AssertMsg::Format(line, nullptr, L"[i].type: TickCount: %u", i, tickCount));
							Assert::AreEqual(pForwardExpected[i].sasLevel, forwardLevelIt->sasLevel, AssertMsg::Format(line, nullptr, L"[i].sasLevel: TickCount: %u", i, tickCount));

							++forwardLevelIt;
						}
					}
				}
			}

			void ReceiveBroadcast(DWORD tickCount, __int64 levelId, __int64 amount, LPCSTR pName = nullptr)
			{
				CProgressiveLevelBroadcast broadcast(tickCount);
				broadcast.AddLevel(levelId, amount, to_wstring(levelId).c_str());
				levels.ProcessLevelBroadcast(&broadcast);
			}

			void VerifyStaleData(int line, const StaleData* pStaleDataExpected, size_t staleDataCount, DWORD tickCount, const SYSTEMTIME& baseTime)
			{
				SYSTEMTIME localTime = FileTime::Add(baseTime, tickCount);

				map<__int64, const CProgressiveLevel*> staleLevels;

				for (const auto& level : levels.m_levels)
				{
					if (level.second->m_state == CProgressiveLevel::Stale)
					{
						staleLevels[level.first] = level.second.get();
					}
				}

				for (size_t i = 0; i < staleDataCount; i++)
				{
					auto it = staleLevels.find(pStaleDataExpected->levelId);

					CountHistory::Data today;
					CountHistory::Data yesterday;
					it->second->m_staleCount.GetDataNoRoll(&today, &yesterday, &localTime);

					Assert::IsFalse(it == staleLevels.end(), AssertMsg::Format(line, L"VerifyStaleData", L"levelId %llu was not stale", pStaleDataExpected->levelId));
					Assert::AreEqual(pStaleDataExpected->staleCountToday, today.GetCount(),
						AssertMsg::Format(line, L"VerifyStaleData", L"levelId %llu staleCount::today", pStaleDataExpected->levelId));
					Assert::AreEqual(pStaleDataExpected->staleMillisecondsToday, today.GetMilliseconds(),
						AssertMsg::Format(line, L"VerifyStaleData", L"levelId %llu staleMillseconds::today", pStaleDataExpected->levelId));
					Assert::AreEqual(pStaleDataExpected->staleCountYesterday, yesterday.GetCount(),
						AssertMsg::Format(line, L"VerifyStaleData", L"levelId %llu staleCount::yesterday", pStaleDataExpected->levelId));
					Assert::AreEqual(pStaleDataExpected->staleMillisecondsYesterday, yesterday.GetMilliseconds(),
						AssertMsg::Format(line, L"VerifyStaleData", L"levelId %llu staleMillseconds::yesterday", pStaleDataExpected->levelId));
				}
			}
		};

		TEST_METHOD(ProgressiveLevels_ProcessForward_NoSessionIdMatch)
		{
			// Data.
			const bool sessionActive(true);
			const int sessionId(470);
			static DWORD receivedTickCount = 0;
			const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(pJson).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");
			CProgressiveBonusingAvailable bonusingAvailable(sessionId - 1, jsonDoc);

			LevelsData levels(receivedTickCount, 5, true);
			levels.levels.ProcessBonusingAvailable(&bonusingAvailable, nullptr);
			levels.levels.SetSessionActiveInfo(sessionActive, sessionId);

			levels.AddLevel(1, ProgressiveLevelType::Standard,	     "url:1", 2, 3, 31);
			levels.AddLevel(2, ProgressiveLevelType::Standard,	     "url:2", 1, 1,  0);
			levels.AddLevel(3, ProgressiveLevelType::Bonus,		     "url:1", 1, 0,  0);
			levels.AddLevel(4, ProgressiveLevelType::AccountingOnly, "url:3", 1, 9,  0);

			levels.TestForward(__LINE__,    0, nullptr, 0, localTime);
			levels.TestForward(__LINE__,  500, nullptr, 0, localTime);
			levels.TestForward(__LINE__, 1000, nullptr, 0, localTime);

			levels.ReceiveBroadcast(1000, 4, 0);

			{
				const ForwardData forwardExpected[] = 
				{
					{ 0,  1000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 1500, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.TestForward(__LINE__, 2000, nullptr, 0, localTime);

			levels.ReceiveBroadcast(2000, 1, 55);

			{
				const ForwardData forwardExpected[] = 
				{
					{ 55, 2000, ProgressiveLevelType::Standard,		  3  },
					{ 55, 2000, ProgressiveLevelType::Standard,		  31 },
					{ 0,  1000, ProgressiveLevelType::AccountingOnly, 9  }
				};
				levels.TestForward(__LINE__, 3000, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.ReceiveBroadcast(4000, 1, 111);
			levels.ReceiveBroadcast(4000, 2, 222);
			levels.ReceiveBroadcast(4000, 3, 333);
			levels.ReceiveBroadcast(4000, 4, 444);

			{
				const ForwardData forwardExpected[] = 
				{
					{ 111, 4000, ProgressiveLevelType::Standard,	   3 },
					{ 111, 4000, ProgressiveLevelType::Standard,	  31 },
					{ 222, 4000, ProgressiveLevelType::Standard,	   1 },
					{ 444, 4000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 5000, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.ReceiveBroadcast(8000, 2, 202);
			levels.ReceiveBroadcast(8000, 3, 303);
			levels.ReceiveBroadcast(8000, 4, 404);

			{
				const ForwardData forwardExpected[] = 
				{
					{ 111, 4000, ProgressiveLevelType::Standard,	   3 },
					{ 111, 4000, ProgressiveLevelType::Standard,	  31 },
					{ 202, 8000, ProgressiveLevelType::Standard,	   1 },
					{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 8000 + 2000, forwardExpected, _countof(forwardExpected), localTime);
			}

			{
				// "forwardExpected" is not needed if CProgressiveLevelForward::MaxInterval == 6000.
				//const ForwardData forwardExpected[] = 
				//{
				//	{ 111, 4000, ProgressiveLevelType::Standard,	   3 },
				//	{ 111, 4000, ProgressiveLevelType::Standard,	  31 },
				//	{ 202, 8000, ProgressiveLevelType::Standard,	   1 },
				//	{ 303, 8000, ProgressiveLevelType::Bonus,		   0 },
				//	{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				//};

				// Test for nullptr forward when no time has elapsed since the last non-nullptr forward.
				Assert::AreEqual((DWORD)8000 + 2000, 4000 + CProgressiveLevelForward::MaxInterval);
				levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval, nullptr, 0, localTime);

			}

			levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval + 1, nullptr, 0, localTime);

			{
				const ForwardData forwardExpected[] = 
				{
					{ 202, 8000, ProgressiveLevelType::Standard,	   1 },
					{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval +  CProgressiveLevelForward::StrobeInterval + 1, forwardExpected, _countof(forwardExpected), localTime);
			}

			{
				DWORD tickCount = CProgressiveLevelForward::OneHour * 2;
				const ForwardData forwardExpected[] = 
				{
					{ 0, 0, ProgressiveLevelType::Invalid, 0 }
				};

				levels.TestForward(__LINE__, tickCount, forwardExpected, 0, localTime);
				const StaleData staleDataExpected[] = 
				{
					{ 1, 1, 7187999, 0, 0 },
					{ 2, 1,       0, 0, 0 },
					{ 3, 1, 7187999, 0, 0 },
					{ 4, 1,       0, 0, 0 }
				};
				levels.VerifyStaleData(__LINE__, staleDataExpected, _countof(staleDataExpected), tickCount, localTime);
			}

			{
				DWORD tickCount = CProgressiveLevelForward::OneHour * 4;
				const ForwardData forwardExpected[] = 
				{
					{ 0, 0, ProgressiveLevelType::Invalid, 0 }
				};
				levels.TestForward(__LINE__, tickCount, forwardExpected, 0, localTime);
				const StaleData staleDataExpected[] = 
				{
					{ 1, 1, 14387999, 0, 0 },
					{ 2, 1,        0, 0, 0 },
					{ 3, 1, 14387999, 0, 0 },
					{ 4, 1,        0, 0, 0 },
				};
				levels.VerifyStaleData(__LINE__, staleDataExpected, _countof(staleDataExpected), tickCount, localTime);
			}
		}

		TEST_METHOD(ProgressiveLevels_ProcessForward_SessionIdMatch)
		{
			// Data.
			const bool sessionActive(true);
			const int sessionId(470);
			static DWORD receivedTickCount = 0;
			const SYSTEMTIME localTime = { 2015, 7, 5, 10, 12, 0, 0, 0 };

			rapidjson::Document jsonDoc;
			if (jsonDoc.Parse(pJson).HasParseError())
				throw JsonException(jsonDoc.GetParseError(), nullptr, L"Parse");
			CProgressiveBonusingAvailable bonusingAvailable(sessionId, jsonDoc);

			LevelsData levels(receivedTickCount, 5, true);
			levels.levels.ProcessBonusingAvailable(&bonusingAvailable, nullptr);
			levels.levels.SetSessionActiveInfo(sessionActive, sessionId);

			levels.AddLevel(1, ProgressiveLevelType::Standard, "url:1", 2, 3, 31);
			levels.AddLevel(2, ProgressiveLevelType::Standard, "url:2", 1, 1, 0);
			levels.AddLevel(3, ProgressiveLevelType::Bonus, "url:1", 1, 0, 0);
			levels.AddLevel(4, ProgressiveLevelType::AccountingOnly, "url:3", 1, 9, 0);

			levels.TestForward(__LINE__, 0, nullptr, 0, localTime);
			levels.TestForward(__LINE__, 500, nullptr, 0, localTime);
			levels.TestForward(__LINE__, 1000, nullptr, 0, localTime);

			levels.ReceiveBroadcast(1000, 4, 0);

			{
				const ForwardData forwardExpected[] =
				{
					{ 0, 1000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 1500, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.TestForward(__LINE__, 2000, nullptr, 0, localTime);

			levels.ReceiveBroadcast(2000, 1, 55);

			{
				const ForwardData forwardExpected[] =
				{
					{ 55, 2000, ProgressiveLevelType::Standard, 3 },
					{ 55, 2000, ProgressiveLevelType::Standard, 31 },
					{ 0, 1000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 3000, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.ReceiveBroadcast(4000, 1, 111);
			levels.ReceiveBroadcast(4000, 2, 222);
			levels.ReceiveBroadcast(4000, 3, 333);
			levels.ReceiveBroadcast(4000, 4, 444);

			{
				const ForwardData forwardExpected[] =
				{
					{ 111, 4000, ProgressiveLevelType::Standard, 3 },
					{ 111, 4000, ProgressiveLevelType::Standard, 31 },
					{ 222, 4000, ProgressiveLevelType::Standard, 1 },
					{ 333, 8000, ProgressiveLevelType::Bonus, 0 },
					{ 444, 4000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 5000, forwardExpected, _countof(forwardExpected), localTime);
			}

			levels.ReceiveBroadcast(8000, 2, 202);
			levels.ReceiveBroadcast(8000, 3, 303);
			levels.ReceiveBroadcast(8000, 4, 404);

			{
				const ForwardData forwardExpected[] =
				{
					{ 111, 4000, ProgressiveLevelType::Standard, 3 },
					{ 111, 4000, ProgressiveLevelType::Standard, 31 },
					{ 202, 8000, ProgressiveLevelType::Standard, 1 },
					{ 303, 8000, ProgressiveLevelType::Bonus, 0 },
					{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 8000 + 2000, forwardExpected, _countof(forwardExpected), localTime);
			}

			{
				// "forwardExpected" is not needed if CProgressiveLevelForward::MaxInterval == 6000.
				//const ForwardData forwardExpected[] = 
				//{
				//	{ 111, 4000, ProgressiveLevelType::Standard,	   3 },
				//	{ 111, 4000, ProgressiveLevelType::Standard,	  31 },
				//	{ 202, 8000, ProgressiveLevelType::Standard,	   1 },
				//	{ 303, 8000, ProgressiveLevelType::Bonus,		   0 },
				//	{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				//};

				// Test for nullptr forward when no time has elapsed since the last non-nullptr forward.
				Assert::AreEqual((DWORD)8000 + 2000, 4000 + CProgressiveLevelForward::MaxInterval);
				levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval, nullptr, 0, localTime);

			}

			levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval + 1, nullptr, 0, localTime);

			{
				const ForwardData forwardExpected[] =
				{
					{ 202, 8000, ProgressiveLevelType::Standard, 1 },
					{ 303, 8000, ProgressiveLevelType::Bonus, 0 },
					{ 404, 8000, ProgressiveLevelType::AccountingOnly, 9 }
				};
				levels.TestForward(__LINE__, 4000 + CProgressiveLevelForward::MaxInterval + CProgressiveLevelForward::StrobeInterval + 1, forwardExpected, _countof(forwardExpected), localTime);
			}

			{
				DWORD tickCount = CProgressiveLevelForward::OneHour * 2;
				const ForwardData forwardExpected[] =
				{
					{ 0, 0, ProgressiveLevelType::Invalid, 0 }
				};

				levels.TestForward(__LINE__, tickCount, forwardExpected, 0, localTime);
				const StaleData staleDataExpected[] =
				{
					{ 1, 1, 7187999, 0, 0 },
					{ 2, 1, 0, 0, 0 },
					{ 3, 1, 7187999, 0, 0 },
					{ 4, 1, 0, 0, 0 }
				};
				levels.VerifyStaleData(__LINE__, staleDataExpected, _countof(staleDataExpected), tickCount, localTime);
			}

			{
				DWORD tickCount = CProgressiveLevelForward::OneHour * 4;
				const ForwardData forwardExpected[] =
				{
					{ 0, 0, ProgressiveLevelType::Invalid, 0 }
				};
				levels.TestForward(__LINE__, tickCount, forwardExpected, 0, localTime);
				const StaleData staleDataExpected[] =
				{
					{ 1, 1, 14387999, 0, 0 },
					{ 2, 1, 0, 0, 0 },
					{ 3, 1, 14387999, 0, 0 },
					{ 4, 1, 0, 0, 0 },
				};
				levels.VerifyStaleData(__LINE__, staleDataExpected, _countof(staleDataExpected), tickCount, localTime);
			}
		}

// #pragma region ProgressiveLevelForward_LevelStandard
		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(BYTE, ...)
		/// Test Standard SAS constructor for default eligible value.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_LevelStandard_Eligible)
		{
			// Perform operation(s) and test(s).
			const CProgressiveLevelForward::Level level(0, 0, ProgressiveLevelType::Standard, 0);
			AssertResult2(level.eligible, true);
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(BYTE, ...)
		/// Test Standard SAS constructor for default showOdometer value.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_LevelStandard_ShowOdometer)
		{
			// Perform operation(s) and test(s).
			const CProgressiveLevelForward::Level level(0, 0, ProgressiveLevelType::Standard, 0);
			AssertResult2(level.showOdometer, true);
		}
// #pragma endregion ProgressiveLevelForward_LevelStandard

// #pragma region ProgressiveLevelForward_Level_Bonus
		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various levelId values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_LevelId)
		{
			typedef __int64 TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(testValue, _T(""), false, false, false, 0, ProgressiveLevelType::Bonus, 0);
				AssertResult2(level.levelId, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various levelName values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_LevelName)
		{
			typedef CString TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetStringData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, testValue, false, false, false, 0, ProgressiveLevelType::Bonus, 0);
				AssertResult2(level.levelName, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various eligible values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_Eligible)
		{
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), testValue, false, false, 0, ProgressiveLevelType::Bonus, 0);
				AssertResult2(level.eligible, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various eligible values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_ShowOdometer)
		{
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), false, testValue, false, 0, ProgressiveLevelType::Bonus, 0);
				AssertResult2(level.showOdometer, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various amount values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_Amount)
		{
			typedef __int64 TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), false, false, false, testValue, ProgressiveLevelType::Bonus, 0);
				AssertResult2(level.amount, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various type values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_TypeVarious)
		{
			typedef int TestDataType;
			const TestDataType thresholdA((int)ProgressiveLevelType::Invalid);
			const TestDataType thresholdB((int)ProgressiveLevelType::PalcomX);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(thresholdA, thresholdB), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testData;
				testReader.GetNextAttribute(testData, CUnitTestingXmlData::TestData_ValueName);
				ProgressiveLevelType::Type testValue((ProgressiveLevelType::Type)testData);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), false, false, false, 0, testValue, 0);
				AssertResult2(level.type, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

			/// <summary>
			/// Functional test for:
			///     CProgressiveLevelForward::Level::Level(__int64, ...)
			/// Test with various type values.
			/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_TypeRange)
		{
			for (m_testIndex = (int)ProgressiveLevelType::Invalid - 2; m_testIndex <= (int)ProgressiveLevelType::PalcomX + 5; ++m_testIndex)
			{
				// Data.
				ProgressiveLevelType::Type testValue((ProgressiveLevelType::Type)m_testIndex);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), false, false, false, testValue, testValue, 0);
				AssertResult2(level.type, testValue);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::Level::Level(__int64, ...)
		/// Test with various tickCount values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_Level_UpdatedTickCount)
		{
			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Perform operation(s) and test(s).
				const CProgressiveLevelForward::Level level(0, _T(""), false, false, false, 0, ProgressiveLevelType::Bonus, testValue);
				AssertResult2(level.updatedTickCount, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion ProgressiveLevelForward_Level_Bonus

// #pragma region ProgressiveLevelForward_AddLevel_Bonus
		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various levelId values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_LevelId)
		{
			typedef __int64 TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(testValue, _T(""), false, false, false, 0, ProgressiveLevelType::Bonus, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->levelId, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various levelName values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_LevelName)
		{
			typedef CString TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetStringData(), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, testValue, false, false, false, 0, ProgressiveLevelType::Bonus, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->levelName, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various eligible values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_Eligible)
		{
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), testValue, false, false, 0, ProgressiveLevelType::Bonus, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->eligible, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various eligible values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_ShowOdometer)
		{
			typedef bool TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), false, testValue, false, 0, ProgressiveLevelType::Bonus, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->showOdometer, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various amount values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_Amount)
		{
			typedef __int64 TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), false, false, false, testValue, ProgressiveLevelType::Bonus, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->amount, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various type values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_TypeVarious)
		{
			typedef int TestDataType;
			const TestDataType thresholdA((int)ProgressiveLevelType::Invalid);
			const TestDataType thresholdB((int)ProgressiveLevelType::PalcomX);
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData(thresholdA, thresholdB), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testData;
				testReader.GetNextAttribute(testData, CUnitTestingXmlData::TestData_ValueName);
				ProgressiveLevelType::Type testValue((ProgressiveLevelType::Type)testData);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), false, false, false, 0, testValue, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->type, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various type values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_TypeRange)
		{
			for (m_testIndex = (int)ProgressiveLevelType::Invalid - 2; m_testIndex <= (int)ProgressiveLevelType::PalcomX + 5; ++m_testIndex)
			{
				// Data.
				ProgressiveLevelType::Type testValue((ProgressiveLevelType::Type)m_testIndex);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), false, false, false, 0, testValue, 0);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->type, testValue);
			}
		}

		/// <summary>
		/// Functional test for:
		///     CProgressiveLevelForward::AddLevel(__int64, ...)
		/// Test with various tickCount values.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_AddLevel_UpdatedTickCount)
		{
			typedef DWORD TestDataType;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetData((TestDataType)0), &m_testIndex);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); ++m_testIndex)
			{
				// Get the test data.
				TestDataType testValue;
				testReader.GetNextAttribute(testValue, CUnitTestingXmlData::TestData_ValueName);

				// Setup.
				CProgressiveLevelForward levelForward(0, false);
				AssertResult2(levelForward.cbegin() == levelForward.cend(), true); // Verify setup.

				// Perform operation(s) and test(s).
				levelForward.AddLevel(0, _T(""), false, false, false, 0, ProgressiveLevelType::Bonus, testValue);
				auto it = levelForward.cbegin();
				AssertResult2(it != levelForward.cend(), true);
				AssertResult2(it->updatedTickCount, testValue);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
// #pragma endregion ProgressiveLevelForward_AddLevelsBonus

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::EraseLevel()
		/// Test EraseLevel() after adding one level.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_EraseLevel1of1)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			auto it = data.cbegin();
			it = data.EraseLevel(it);

			Assert::AreEqual((size_t)0, data.GetNumLevels(), L"EraseLevel: GetNumLevels()");
			Assert::IsTrue(data.cend() == data.cbegin(), L"EraseLevel: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::IsTrue(it == data.cbegin(), L"EraseLevel: it cbegin");
			Assert::IsTrue(it == data.cend(), L"EraseLevel: it cend");
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::EraseLevel()
		/// Test EraseLevel() on the first level after adding two levels.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_EraseLevel1of2)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus, 456, true },
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			auto it = data.cbegin();
			it = data.EraseLevel(it);

			Assert::AreEqual((size_t)1, data.GetNumLevels(), L"EraseLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"EraseLevel: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::IsTrue(it == data.cbegin(), L"EraseLevel: it cbegin");
			Assert::IsFalse(it == data.cend(), L"EraseLevel: it cend");
			tests[1].Verify(*data.cbegin());
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::EraseLevel()
		/// Test EraseLevel() on the second level after adding two levels.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_EraseLevel2of2)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus, 456, true },
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			auto it = data.cbegin();
			++it;
			it = data.EraseLevel(it);

			Assert::AreEqual((size_t)1, data.GetNumLevels(), L"EraseLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"EraseLevel: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::IsFalse(it == data.cbegin(), L"EraseLevel: it cbegin");
			Assert::IsTrue(it == data.cend(), L"EraseLevel: it cend");
			tests[0].Verify(*data.cbegin());
		}

		/// <summary>
		/// Test:
		///     CProgressiveLevelForward::EraseLevel()
		/// Test EraseLevel() on the second level after adding three levels.
		/// </summary>
		TEST_METHOD(ProgressiveLevelForward_EraseLevel2of3)
		{
			const int groupId = 2;
			const bool useAftLockOnTransfers = true;

			CProgressiveLevelForward data(groupId, useAftLockOnTransfers);

			TestData tests[] =
			{
				{ 1, 0, ProgressiveLevelType::Standard, 123, true },
				{ 15, INT64_MAX, ProgressiveLevelType::Bonus, 456, true },
				{ 31, 10, ProgressiveLevelType::AccountingOnly, 789, true },
			};

			for (size_t test = 0; test < _countof(tests); test++)
			{
				data.AddLevel(tests[test].sasLevel, tests[test].amount, tests[test].type, tests[test].forwardedTickCount);
			}

			// Verify setup.
			Assert::AreEqual(_countof(tests), data.GetNumLevels(), L"AddLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"AddLevel: cend cbegin");

			// Perform operation(s) and test(s).
			auto it = data.cbegin();
			++it;
			it = data.EraseLevel(it);

			Assert::AreEqual((size_t)2, data.GetNumLevels(), L"EraseLevel: GetNumLevels()");
			Assert::IsFalse(data.cend() == data.cbegin(), L"EraseLevel: cend cbegin");
			Assert::AreEqual(groupId, data.GetProgressiveGroupId(), L"GetProgressiveGroupId");
			Assert::AreEqual(useAftLockOnTransfers, data.UseAftLockOnTransfers(), L"UseAftLockOnTransfers");

			Assert::IsFalse(it == data.cbegin(), L"EraseLevel: it cbegin");
			Assert::IsTrue(it == data.cbegin() + 1, L"EraseLevel: it cbegin() + 1");
			Assert::IsFalse(it == data.cend(), L"EraseLevel: it cend");
			tests[0].Verify(data.cbegin()[0]);
			tests[2].Verify(data.cbegin()[1]);
		}

		#define _MIN_INTERVAL			CProgressiveLevelForward::MinInterval
		#define _MIN_INTERVAL_UNDER		(_MIN_INTERVAL - 1)
		#define _MIN_INTERVAL_OVER		(_MIN_INTERVAL + 1)

		#define _STROBE_INTERVAL		CProgressiveLevelForward::StrobeInterval
		#define _STROBE_INTERVAL_UNDER	(_STROBE_INTERVAL - 1)
		#define _STROBE_INTERVAL_OVER	(_STROBE_INTERVAL + 1)

		#define _MAX_INTERVAL			CProgressiveLevelForward::MaxInterval
		#define _MAX_INTERVAL_UNDER		(_MAX_INTERVAL - 1)
		#define _MAX_INTERVAL_OVER		(_MAX_INTERVAL + 1)

		#define _DELETE_INTERVAL		CProgressiveLevelForward::DeleteInterval
		#define _DELETE_INTERVAL_UNDER	(_DELETE_INTERVAL - 1)
		#define _DELETE_INTERVAL_OVER	(_DELETE_INTERVAL + 1)

		struct TestData_ProcessForward
		{
			int line;
			ProgressiveLevelType::Type type;
			CProgressiveLevel::State state;
			BYTE numSasLevels;
			BYTE sasLevels[3];
			DWORD updatedTickCount;
			DWORD forwardedTickCount;
			DWORD tickCount;

			struct Expected
			{
				bool result;
				size_t numLevels;
				BYTE sasLevels[3];
				CProgressiveLevel::State state;
			};
			Expected expected;

			void Run(LPCTSTR pTest)
			{
				CProgressiveLevel progressiveLevel;
				progressiveLevel.m_id = 1;
				progressiveLevel.m_name = _T("Name");
				progressiveLevel.m_url = _T("Url");
				progressiveLevel.m_value = 1234;
				progressiveLevel.m_state = state;
				progressiveLevel.m_updatedTickCount = updatedTickCount;

				for (BYTE i = 0; i < numSasLevels; i++)
				{
					progressiveLevel.m_sasLevels.push_back(sasLevels[i]);
				}

				CProgressiveLevelForward progressiveLevelForward(1, true);

				bool result = progressiveLevel.ProcessForward(tickCount, &progressiveLevelForward);
				Assert::AreEqual(expected.result, result, AssertMsg::Format(line, L"result", pTest));
				Assert::AreEqual((int) expected.state, (int) progressiveLevel.m_state, AssertMsg::Format(line, L"m_state", pTest));
				Assert::AreEqual((int) expected.numLevels, (int) progressiveLevelForward.GetNumLevels(), AssertMsg::Format(line, L"GetNumLevels()", pTest));

				size_t curLevel = 0;
				for_each(progressiveLevelForward.cbegin(), progressiveLevelForward.cend(),
					[&](const CProgressiveLevelForward::Level& level)
				{
					Assert::AreEqual((int) type, (int) level.type, AssertMsg::Format(line, nullptr, L"level.type", pTest));
					Assert::AreEqual((uint64_t) 1234, (uint64_t) level.amount, AssertMsg::Format(line, nullptr, L"level.amount", pTest));
					Assert::AreEqual(expected.sasLevels[curLevel], level.sasLevel, AssertMsg::Format(line, nullptr, L"sasLevel[%u]: %s", curLevel, pTest));
					curLevel++;
				});
			}
		};

		TEST_METHOD(ProgressiveLevel_ProcessForward_Configured)
		{
			TestData_ProcessForward tests[] = 
			{
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MIN_INTERVAL_UNDER, 	{  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MIN_INTERVAL, 			{  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MIN_INTERVAL_OVER, 	{  true, 0, {}, CProgressiveLevel::Configured } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _STROBE_INTERVAL_UNDER, {  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _STROBE_INTERVAL,		{  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _STROBE_INTERVAL_OVER, 	{  true, 0, {}, CProgressiveLevel::Configured } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MAX_INTERVAL_UNDER, 	{  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MAX_INTERVAL, 			{  true, 0, {}, CProgressiveLevel::Configured } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _MAX_INTERVAL_OVER, 	{ false, 0, {}, CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _DELETE_INTERVAL_UNDER, { false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _DELETE_INTERVAL, 		{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Configured,	1, { 1 }, 0, 0, _DELETE_INTERVAL_OVER, 	{ false, 0, {}, CProgressiveLevel::Stale } }
			};

			for (size_t test = 0; test < _countof(tests); test++)
				tests[test].Run(L"Configured");
		}

		TEST_METHOD(ProgressiveLevel_ProcessForward_Updated)
		{
			TestData_ProcessForward tests[] = 
			{
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _MIN_INTERVAL_UNDER, 	{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 2, { 1, 3 }, 0, 0, _MIN_INTERVAL, 		{ true,  2, { 1, 3 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _MIN_INTERVAL_OVER, 	{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _STROBE_INTERVAL_UNDER,	{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _STROBE_INTERVAL,		{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _STROBE_INTERVAL_OVER, 	{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _MAX_INTERVAL_UNDER, 	{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _MAX_INTERVAL, 			{ true,  1, { 1 },    CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _MAX_INTERVAL_OVER, 	{ false, 0, { },      CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _DELETE_INTERVAL_UNDER,	{ false, 0, { },      CProgressiveLevel::Stale } },
				// The following two should never happen, but are here to test the state machine
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _DELETE_INTERVAL, 		{ false, 0, { },      CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Updated, 1, { 1 }, 0, 0, _DELETE_INTERVAL_OVER, 	{ false, 0, { },      CProgressiveLevel::Stale } },
			};

			for (size_t test = 0; test < _countof(tests); test++)
				tests[test].Run(L"Updated");
		}

		TEST_METHOD(ProgressiveLevel_ProcessForward_Forwarded)
		{
			TestData_ProcessForward tests[] = 
			{
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MIN_INTERVAL_UNDER, 	  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MIN_INTERVAL, 		  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MIN_INTERVAL_OVER, 	  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _STROBE_INTERVAL_UNDER, { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _STROBE_INTERVAL,		  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _STROBE_INTERVAL_OVER,  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MAX_INTERVAL_UNDER, 	  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MAX_INTERVAL, 		  { true,  1, { 1 }, CProgressiveLevel::Forwarded } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _MAX_INTERVAL_OVER, 	  { false, 0, {}, CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _DELETE_INTERVAL_UNDER, { false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _DELETE_INTERVAL, 	  { false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Forwarded, 1, { 1 }, 0, 0, _DELETE_INTERVAL_OVER,  { false, 0, {}, CProgressiveLevel::Stale } }
			};

			for (size_t test = 0; test < _countof(tests); test++)
				tests[test].Run(L"Forwarded");
		}

		TEST_METHOD(ProgressiveLevel_ProcessForward_Stale)
		{
			TestData_ProcessForward tests[] = 
			{
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MIN_INTERVAL_UNDER, 		{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MIN_INTERVAL, 			{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MIN_INTERVAL_OVER, 		{ false, 0, {}, CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _STROBE_INTERVAL_UNDER,	{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _STROBE_INTERVAL,			{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _STROBE_INTERVAL_OVER, 	{ false, 0, {}, CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MAX_INTERVAL_UNDER, 		{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MAX_INTERVAL, 			{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _MAX_INTERVAL_OVER, 		{ false, 0, {}, CProgressiveLevel::Stale } },

				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _DELETE_INTERVAL_UNDER,	{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _DELETE_INTERVAL, 		{ false, 0, {}, CProgressiveLevel::Stale } },
				{ __LINE__, ProgressiveLevelType::Standard, CProgressiveLevel::Stale, 1, { 1 }, 0, 0, _DELETE_INTERVAL_OVER, 	{ false, 0, {}, CProgressiveLevel::Stale } }
			};

			for (size_t test = 0; test < _countof(tests); test++)
				tests[test].Run(L"Stale");
		}
	};
}
#endif //TODO-PORT