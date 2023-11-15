#include "stdafx.h"
#include "MessagePrioritizer.h"
#include "ProgressiveProtocol/ProgressiveWinnerPicked.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

	class MessagePrioritizerUnitTests : public ::testing::Test
	{
	public:
	};
	
		TEST(MessagePrioritizerUnitTests, GetMessageFlushTypeUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			for (unsigned int index(0); index < NUMBER_OF_INT_MSG; ++index)
			{
				InternalMsgType msgType((InternalMsgType)index);
				MessageFlushType flushType(NoFlushType);
				if (messagePrioritizer.IsPBTMessage(msgType))
				{
					flushType = PBFlushType;
				}
				else if (messagePrioritizer.IsPromotionMessage(msgType))
				{
					flushType = PromotionFlushType;
				}
				else if (messagePrioritizer.IsJackpotMessage(msgType))
				{
					flushType = JackpotFlushType;
				}
				else if (messagePrioritizer.IsPteFlushtype(msgType))
				{
					flushType = PteFlushType;
				}
				else if (messagePrioritizer.IsEmployeeFlushtype(msgType))
				{
					flushType = EmployeeFlushType;
				}
				ASSERT_TRUE(flushType == messagePrioritizer.GetMessageFlushType(msgType));
			}
		}

		TEST(MessagePrioritizerUnitTests, IsFlusherMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			//Tests
			ASSERT_TRUE(messagePrioritizer.IsFlusherMessage(INT_MSG_PB_COMPLETE));

			ASSERT_TRUE(messagePrioritizer.IsFlusherMessage(INT_MSG_PROGRESSIVE_WINNER_PICKED));

			ASSERT_FALSE(messagePrioritizer.IsFlusherMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, PNW_END_SCREEN));

			ASSERT_FALSE(messagePrioritizer.IsFlusherMessage(INT_MSG_JACKPOT_ANNOUNCEMENT));

			ASSERT_FALSE(messagePrioritizer.IsFlusherMessage(INT_MSG_JACKPOT_ANNOUNCEMENT, CARD_IN_PROMOTION));

			ASSERT_FALSE(messagePrioritizer.IsFlusherMessage(INT_MSG_JACKPOT_HIT));

			ASSERT_TRUE(messagePrioritizer.IsFlusherMessage(INT_MSG_DISPLAY_PTE_RESULTS));

			ASSERT_FALSE(messagePrioritizer.IsFlusherMessage(INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN));
		}

		TEST(MessagePrioritizerUnitTests, IsPBTMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			for (unsigned int index(0); index < NUMBER_OF_INT_MSG; ++index)
			{
				InternalMsgType msgType((InternalMsgType)index);

				if (INT_MSG_CLEAR_PBT == msgType ||
					INT_MSG_PB_CHECK_DENOM == msgType ||
					INT_MSG_PB_COMPLETE == msgType ||
					INT_MSG_PB_NCEPOUT_FAIL == msgType ||
					INT_MSG_AUTO_TRANSFER_CANCELLED == msgType ||
					INT_MSG_PB_INFO == msgType ||
					INT_MSG_SHOW_OTHER_BUTTON_ERROR == msgType ||
                    INT_MSG_PB_PIN_RESET_RESULT == msgType ||
					INT_MSG_NEW_PIN_REQUIRED == msgType)
				{
					ASSERT_TRUE(messagePrioritizer.IsPBTMessage(msgType));
				}
				else
				{
					ASSERT_FALSE(messagePrioritizer.IsPBTMessage(msgType));
				}
			}
		}

		TEST(MessagePrioritizerUnitTests, IsPromotionMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			//Tests

			for (unsigned int index(0); index < NUMBER_OF_INT_MSG; ++index)
			{
				InternalMsgType msgType((InternalMsgType)index);

				if (INT_MSG_CASINO_CHALLENGE_END == msgType ||
					INT_MSG_CASINO_CHALLENGE_TRIGGER == msgType ||
					INT_MSG_CASINO_CHALLENGE_INCREMENT == msgType ||
					INT_MSG_CASINO_CHALLENGE_AWARD == msgType ||
					INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END == msgType ||
					INT_MSG_SPLASHDOWN_START == msgType ||
					INT_MSG_SPLASHDOWN_AWARD == msgType ||
					INT_MSG_SPLASHDOWN_END == msgType ||
					INT_MSG_SPLASHDOWN_WINNER_END == msgType ||
					INT_MSG_RICOCHET_TRIGGER == msgType ||
					INT_MSG_RICOCHET_AWARD == msgType ||
					INT_MSG_JACKPOT_ANNOUNCEMENT == msgType)
				{
					ASSERT_TRUE(messagePrioritizer.IsPromotionMessage(msgType));
				}
				else
				{
					ASSERT_FALSE(messagePrioritizer.IsPromotionMessage(msgType));
				}
			}
		}

		TEST(MessagePrioritizerUnitTests, IsJackpotMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;


			for (unsigned int index(0); index < NUMBER_OF_INT_MSG; ++index)
			{
				InternalMsgType msgType((InternalMsgType)index);

				if (INT_MSG_PROGRESSIVE_WINNER_PICK == msgType ||
					INT_MSG_PROGRESSIVE_WINNER_PICKED == msgType ||
					INT_MSG_PROGRESSIVE_SCHEDULE_END == msgType)
				{
					ASSERT_TRUE(messagePrioritizer.IsJackpotMessage(msgType));
				}
				else
				{
					ASSERT_FALSE(messagePrioritizer.IsJackpotMessage(msgType));
				}
			}
		}

		TEST(MessagePrioritizerUnitTests, IsPteFlushtypeUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			//Tests
			for (unsigned int index(0); index < NUMBER_OF_INT_MSG; ++index)
			{
				InternalMsgType msgType((InternalMsgType)index);

				if (INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN == msgType ||
					INT_MSG_DISPLAY_PTE_ANTICIPATION == msgType ||
					INT_MSG_DISPLAY_PTE_LEADERBOARD == msgType ||
					INT_MSG_DISPLAY_PTE_LEVELUP == msgType ||
					INT_MSG_DISPLAY_PTE_RESULTS == msgType)
				{
					ASSERT_TRUE(messagePrioritizer.IsPteFlushtype(msgType));
				}
				else
				{
					ASSERT_FALSE(messagePrioritizer.IsPteFlushtype(msgType));
				}
			}
		}

		TEST(MessagePrioritizerUnitTests, FindMessagePriorityUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;
			MessagePriorityInfo priorityInfo;

			//Tests
			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SPLASHDOWN_START, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 10000);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SPLASHDOWN_AWARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_RICOCHET_TRIGGER, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_RICOCHET_AWARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CASINO_CHALLENGE_INCREMENT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == 1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CASINO_CHALLENGE_END, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CASINO_CHALLENGE_AWARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CASINO_CHALLENGE_FIVE_MINUTE_END, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CASINO_CHALLENGE_TRIGGER, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SPLASHDOWN_WINNER_END, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SPLASHDOWN_END, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_SMPROMO);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_SMPROMOTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PROGRESSIVE_JACKPOT_ANNOUNCEMENT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTS);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTSTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PROGRESSIVE_TOTALIZER_ANNOUNCEMENT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTS);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_ANNOUNCEMENTSTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PROGRESSIVE_WINNER_PICK, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_WINNERANTICIPATION);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_WINNERANTICIPATION);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_WINNERANTICIPATIONTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PROGRESSIVE_CLAIMABLE_AWARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_MASSMARKETING);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_MASSMARKETING);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_MASSMARKETINGTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_CLEAR_PBT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == 0);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_CHECK_DENOM, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_NEW_PIN_REQUIRED, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_COMPLETE, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_NCEPOUT_FAIL, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_AUTO_TRANSFER_CANCELLED, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_PIN_RESET_RESULT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);			

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_INFO, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PB_NCEPOUT_FAIL, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SHOW_OTHER_BUTTON_ERROR, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PROCESS_BUTTON_PRESS, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DOLLAR_BUTTON_SUCCESS, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DOLLAR_BUTTON_ERROR, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SHOW_CREDIT_TRANSFER_ENTRY_SCREEN, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SHOW_NON_CREDIT_TRANSFER_ENTRY_SCREEN, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_ACCOUNT_BALANCE, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATEDTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_BAD_CARD_READ, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == -1);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_UJP, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_UJP_INVALID_PIN, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_AUTOCARDOUT, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATED - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_PLAYERINITIATEDTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_PLAY_CONGRATS, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_CONGRATS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_CONGRATS);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_CONGRATSTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SHOW_ANNIVERSARY, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_ANNIVERSARY);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_ANNIVERSARY);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_ANNIVERSARYTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_SHOW_BIRTHDAY, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_BIRTHDAY);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_BIRTHDAY);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYTIME);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_BIRTHDAYTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_BANNED_PLAYER, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 2);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_PBTSCREENS - 1);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == 0);

			ASSERT_FALSE(messagePrioritizer.FindMessagePriority(INT_MSG_ALLOW_TICKET_PRINTED, priorityInfo));

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_ANTICIPATION, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATION);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATION);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ANTICIPATIONTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_AWARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_WINNERMESSAGE);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_WINNERMESSAGE);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_WINNERTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_ENDCOUNTDOWN, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOON);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOON);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_ENDINGSOONTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_LEADERBOARD, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARD);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARD);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEADERBOARDTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_LEVELUP, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKED);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKED);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_LEVELUNLOCKEDTTL);

			ASSERT_TRUE(messagePrioritizer.FindMessagePriority(INT_MSG_DISPLAY_PTE_RESULTS, priorityInfo));
			ASSERT_TRUE(priorityInfo.persistPriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTS);
			ASSERT_TRUE(priorityInfo.messagePriority == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTS);
			ASSERT_TRUE(priorityInfo.persistType == PERSIST_CONTENT_WINDOW);
			ASSERT_TRUE(priorityInfo.queueStarve == POPUPDISPLAYUNLIMITED);
			ASSERT_FALSE(priorityInfo.interactive);
			ASSERT_TRUE(priorityInfo.ttl == CQueuingConfig::DEFAULT_PRIORITY_INVITED_GROUPS_FINALRESULTSTTL);
		}		

		TEST(MessagePrioritizerUnitTests, PrioritizeMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			CInternalMessage *msg = new CInternalMessage(INT_MSG_CARDED_SESSION_IN_PROGRESS);
			int priority(0);
			CMessagePersist* mp(messagePrioritizer.PrioritizeMessage(msg, priority));

			ASSERT_TRUE(mp->GetPriority() == INTERACTIVECONTENT);
			ASSERT_FALSE(mp->IsFlusher());
			ASSERT_TRUE(mp->isNewStarvation());

			delete mp;
			delete msg;
		}

		TEST(MessagePrioritizerUnitTests, PrioritizeMessageFlusherUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PB_COMPLETE);
			int priority(0);
			CMessagePersist* mp(messagePrioritizer.PrioritizeMessage(msg, priority));

			ASSERT_TRUE(mp->GetPriority() == INTERACTIVECONTENT);
			ASSERT_TRUE(mp->IsFlusher());
			ASSERT_TRUE(mp->isNewStarvation());

			delete mp;
			delete msg;
		}

		TEST(MessagePrioritizerUnitTests, PrioritizeProgressivePrizeMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_PRIZE);
			int priority(0);
			CMessagePersist* mp(messagePrioritizer.PrioritizeMessage(msg, priority));

			ASSERT_TRUE(mp->GetPriority() == INTERACTIVECONTENT);
			ASSERT_TRUE(4000 == priority);
			ASSERT_FALSE(mp->IsFlusher());
			ASSERT_TRUE(mp->isNewStarvation());

			delete mp;
			delete msg;
		}

		TEST(MessagePrioritizerUnitTests, PrioritizePromoGameMessageUnitTest)
		{
			//Setup
			CMessagePrioritizer messagePrioritizer;

			CInternalMessage *msg = new CInternalMessage(INT_MSG_PROGRESSIVE_PROMO_GAME);
			int priority(0);
			CMessagePersist* mp(messagePrioritizer.PrioritizeMessage(msg, priority));

			ASSERT_TRUE(mp->GetPriority() == INTERACTIVECONTENT);
			ASSERT_TRUE(4000 == priority);
			ASSERT_FALSE(mp->IsFlusher());
			ASSERT_TRUE(mp->isNewStarvation());

			delete mp;
			delete msg;
		}
