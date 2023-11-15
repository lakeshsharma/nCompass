#include "stdafx.h"
#include "Jackpot.h"
#include "gtest/gtest.h"

class JackpotUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

};

TEST_F(JackpotUnitTests, JackpotNVRAMDefault)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NVRAM);

	CJackpot *copy = new CJackpot(MEMORY_NVRAM, true);
	ASSERT_TRUE(copy->GetJackpotAmount() == 0);
	ASSERT_TRUE(copy->GetJackpotType() == JT_NONE);
	ASSERT_TRUE(copy->GetJackpotTransactionType() == JTRANS_COMPOSITE);
	ASSERT_TRUE(copy->GetProgressiveLevel() == 0);
	ASSERT_TRUE(copy->GetCoinsPlayed() == 0);

	delete jackpot;
	delete copy;
}

TEST_F(JackpotUnitTests, JackpotNVRAMNonDefault)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NVRAM);

	jackpot->SetJackpotAmount(1000);
	jackpot->SetJackpotType(JT_CANCEL_CREDIT);
	jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_HOPPER_PAID);
	jackpot->SetProgressiveLevel(1);
	jackpot->SetCoinsPlayed(10);

	CJackpot *copy = new CJackpot(MEMORY_NVRAM, true);
	ASSERT_TRUE(copy->GetJackpotAmount() == 1000);
	ASSERT_TRUE(copy->GetJackpotType() == JT_CANCEL_CREDIT);
	ASSERT_TRUE(copy->GetJackpotTransactionType() == JTRANS_PROGRESSIVE_HOPPER_PAID);
	ASSERT_TRUE(copy->GetProgressiveLevel() == 1);
	ASSERT_TRUE(copy->GetCoinsPlayed() == 10);

	delete jackpot;
	delete copy;
}

TEST_F(JackpotUnitTests, JackpotNVRAMOverwriteWithDefault)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NVRAM);

	jackpot->SetJackpotAmount(1000);
	jackpot->SetJackpotType(JT_CANCEL_CREDIT);
	jackpot->SetJackpotTransactionType(JTRANS_PROGRESSIVE_HOPPER_PAID);
	jackpot->SetProgressiveLevel(1);
	jackpot->SetCoinsPlayed(10);

	CJackpot *copy = new CJackpot(MEMORY_NVRAM, true);
	ASSERT_TRUE(copy->GetJackpotAmount() == 1000);
	ASSERT_TRUE(copy->GetJackpotType() == JT_CANCEL_CREDIT);
	ASSERT_TRUE(copy->GetJackpotTransactionType() == JTRANS_PROGRESSIVE_HOPPER_PAID);
	ASSERT_TRUE(copy->GetProgressiveLevel() == 1);
	ASSERT_TRUE(copy->GetCoinsPlayed() == 10);

	delete jackpot;
	delete copy;

	// Create new default jackpot
	jackpot = new CJackpot(MEMORY_NVRAM);

	copy = new CJackpot(MEMORY_NVRAM, true);
	ASSERT_TRUE(copy->GetJackpotAmount() == 0);
	ASSERT_TRUE(copy->GetJackpotType() == JT_NONE);
	ASSERT_TRUE(copy->GetJackpotTransactionType() == JTRANS_COMPOSITE);
	ASSERT_TRUE(copy->GetProgressiveLevel() == 0);
	ASSERT_TRUE(copy->GetCoinsPlayed() == 0);

	delete jackpot;
	delete copy;
}

TEST_F(JackpotUnitTests, JackpoTaxWithholdingConfigTest)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NONE);

	jackpot->SetTaxWithholdingConfig(true, 0, 0);
	ASSERT_FALSE(jackpot->IsTaxWithholdingEnabled());
	jackpot->SetTaxWithholdingConfig(true, 1, 0);
	ASSERT_TRUE(jackpot->IsTaxWithholdingEnabled());
	jackpot->SetTaxWithholdingConfig(true, 0, 0);
	ASSERT_FALSE(jackpot->IsTaxWithholdingEnabled());
	jackpot->SetTaxWithholdingConfig(true, 0, 1);
	ASSERT_TRUE(jackpot->IsTaxWithholdingEnabled());
	jackpot->SetTaxWithholdingConfig(true, 0, 0);
	ASSERT_FALSE(jackpot->IsTaxWithholdingEnabled());	

	delete jackpot;
}

TEST_F(JackpotUnitTests, GetJackpotStatusTest)
{
	CJackpot *jackpot = new CJackpot(MEMORY_NONE);

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCreditAndRemoteReset);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusSuccessKeyToCreditRemoteReset == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCreditReset);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusSuccessKeyToCredit == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPFailed);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusFailed == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceled);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceled == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledHandpayReset);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledHandpayReset == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledHandpayReset);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledHandpayReset == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledHandpayReset);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledHandpayReset == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledReboot);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledReboot == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledVirtualPlayer);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledVirtualPlayer == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledTimedOut);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledTimedOut == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledTaskbarButtonPress);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledTaskbarButtonPress == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledCloseMediaWindow);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledCloseMediaWindow == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledAccountLocked);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledAccountLocked == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledPlayerIdMismatch);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledPlayerIdMismatch == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledInsufficientInfo);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledInsufficientInfo == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledEgmNotCapable);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledEgmNotCapable == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledAboveLimit);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledAboveLimit == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPCanceledCallHost);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceledCallHost == jackpot->GetJackpotStatus());

	jackpot->SetUnattendedState(UnattendedJackpotState::UJPNone);
	ASSERT_TRUE(UnattendedJackpotStatus::UJPStatusCanceled == jackpot->GetJackpotStatus());	

	delete jackpot;
}
