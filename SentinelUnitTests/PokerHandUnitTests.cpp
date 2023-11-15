#include "stdafx.h"
#include "PokerHand.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PokerHandTests : public ::testing::Test
	{
	public:
	};

		TEST(PokerHandTests, PokerHand_ctor_nullptr)
		{
			PokerHand pokerHand;
			ASSERT_FALSE(pokerHand.IsValid());
		}

		TEST(PokerHandTests, PokerHand_ctor_data)
		{
			static byte cards[PokerHand::CardsInHand] = { 0x01, 0x23, 0x45, 0x56, 0x78 };

			PokerHand pokerHand(cards);

			ASSERT_TRUE(pokerHand.IsValid());
			for (int i = 0; i < PokerHand::CardsInHand; i++)
				ASSERT_EQ(cards[i], pokerHand[i]) << AssertMsg::Format(i, nullptr);
		}

		TEST(PokerHandTests, PokerHand_Set_nullptr)
		{
			PokerHand pokerHand;

			pokerHand.Set(nullptr);
			ASSERT_FALSE(pokerHand.IsValid());
		}

		TEST(PokerHandTests, PokerHand_Set)
		{
			static byte cards[PokerHand::CardsInHand] = { 0x01, 0x23, 0x45, 0x56, 0x78 };

			PokerHand pokerHand;

			pokerHand.Set(cards);
			ASSERT_TRUE(pokerHand.IsValid());
			for (int i = 0; i < PokerHand::CardsInHand; i++)
				ASSERT_EQ(cards[i], pokerHand[i]) << AssertMsg::Format(i, nullptr);
		}

		TEST(PokerHandTests, PokerHand_ToOneLinkString)
		{
			struct
			{
				int line;
				byte cards[PokerHand::CardsInHand];
				const char* pExpected;
			}
			static data[] =
			{
				{ __LINE__, { 0x00, 0x00, 0x00, 0x00, 0x00 }, "2S 2S 2S 2S 2S" },
				{ __LINE__, { 0x01, 0x23, 0x4d, 0x5e, 0xff }, "3S 5H WW ?? ??" }
			};

			for (size_t i = 0; i < _countof(data); i++)
			{
				PokerHand pokerHand(data[i].cards);
				string s = pokerHand.ToOneLinkString();
				ASSERT_STREQ(data[i].pExpected, s.c_str()) << AssertMsg::Format(data[i].line, nullptr);

			}
		}
}
