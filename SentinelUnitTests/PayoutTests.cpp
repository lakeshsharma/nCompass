#include "stdafx.h"
#include "Payout.h"
#include <Jackpot.h>

// Unit testing includes.
#include "gtest/gtest.h"
#include "AssertMsg.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PayoutTests : public ::testing::Test
	{
	public:
	};

		TEST(PayoutTests, Payout_ctor_Jackpot_AttPdJpHp)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_HANDPAY);
			jackpot->SetJackpotTransactionType(JTRANS_COMPOSITE);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AttendantPaidJackpot);
			delete jackpot;
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AttPdJpTA)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_TOPAWARD);
			jackpot->SetJackpotTransactionType(JTRANS_COMPOSITE);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AttendantPaidJackpot);
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AttPdProgComp)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_NONE);
			jackpot->SetJackpotTransactionType(JTRANS_COMPOSITE);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AttendantPaidProgressiveJackpot);
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AttPdProgHoppPaid)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_NONE);
			jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_HOPPER_PAID);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AttendantPaidProgressiveJackpot);
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AttPdProgScanner)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_NONE);
			jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_SCANNER_ONLY);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AttendantPaidProgressiveJackpot);
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AllJackpot)
		{
			CJackpot* jackpot = new CJackpot();
			jackpot->SetJackpotAmount(2);
			jackpot->SetJackpotType(JT_EXTERNALBONUS);
			jackpot->SetJackpotTransactionType(JTRANS_COMPOSITE);
			CPayout payout(jackpot);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AllJackpots);
		}

		TEST(PayoutTests, Payout_ctor_Jackpot_AllJackpotBonus)
		{
			CPayout payout(2, AllJackpots);

			ASSERT_TRUE(payout.GetJackpotAmount() == 2);
			ASSERT_TRUE(payout.GetJackpotType() == AllJackpots);
		}
}
