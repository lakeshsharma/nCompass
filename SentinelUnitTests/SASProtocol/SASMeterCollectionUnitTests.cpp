#include "stdafx.h"
#include "SASProtocol/SASMeterCollection.h"
#include "SASProtocol/SASSerialWrapperMock.h"
#include "SASProtocol/LongPoll10Message.h"

#include "GlobalDefs.h"
#include "Utilities.h"
#include "gtest/gtest.h"

using ::testing::NiceMock;

namespace SentinelNativeUnitTests
{
	class SASMeterCollectionUnitTests : public ::testing::Test
	{
protected:		
		CMeterStepdown m_mainGameMeterStepdown;
		CMeterStepdown m_subGameMeterStepdown;
		CSASConsumer *cons = NULL;
		CQueueList *gcf = NULL;
		CQueueList *in = NULL;
	
	void PopulateMeterStepdowns(void)
	{

		// Populate main game meter stepdown map here
		m_mainGameMeterStepdown.AddMeterItem(mCD, 0x6F, 0x2F, 0x1C, 0x19);	// Coin Drop
		//m_mainGameMeterStepdown.AddMeterItem(mSDr,	0x1C,	0,		0,		0 );	// Slot Door
		m_mainGameMeterStepdown.AddMeterItem(mActualSlotDr, 0x1C, 0, 0, 0);	// Slot Door
		m_mainGameMeterStepdown.AddMeterItem(mD1, 0x6F, 0x2F, 0x1e, 0x31);		//{Ones}
		m_mainGameMeterStepdown.AddMeterItem(mD2, 0x6F, 0x2F, 0x32, 0);		//{Twos}
		m_mainGameMeterStepdown.AddMeterItem(mD5, 0x6F, 0x2F, 0x1e, 0x33);		//{Fives}
		m_mainGameMeterStepdown.AddMeterItem(mD10, 0x6F, 0x2F, 0x1e, 0x34);		//{Tens}
		m_mainGameMeterStepdown.AddMeterItem(mD20, 0x6F, 0x2F, 0x1e, 0x35);		//{Twenties}
		m_mainGameMeterStepdown.AddMeterItem(mD50, 0x6F, 0x2F, 0x1e, 0x36);		//{Fifties}
		m_mainGameMeterStepdown.AddMeterItem(mD100, 0x6F, 0x2F, 0x1e, 0x37);		//{Hundreds}
		m_mainGameMeterStepdown.AddMeterItem(mCrd, 0x6F, 0x2F, 0x1A, 0);	// Credit Meter
		m_mainGameMeterStepdown.AddMeterItem(mTCi, 0x2A, 0, 0, 0);	// True Coin In
		m_mainGameMeterStepdown.AddMeterItem(mTCo, 0x2B, 0, 0, 0);	// True Coin Out
		m_mainGameMeterStepdown.AddMeterItem(mBD, 0x6F, 0x2F, 0x46, 0);	// Bill drop credits
		m_mainGameMeterStepdown.AddMeterItem(mTHP, 0x6F, 0x2F, 0, 0);	// Total Hand Pay
		m_mainGameMeterStepdown.AddMeterItem(mActualCD, 0x6F, 0x2F, 0, 0);	// Actual coin drop
		m_mainGameMeterStepdown.AddMeterItem(gCI, 0x6F, 0x2F, 0x1C, 0x19);	// Coin in
		m_mainGameMeterStepdown.AddMeterItem(gCO, 0x6F, 0x2F, 0x1C, 0x19);	// Coin out
		m_mainGameMeterStepdown.AddMeterItem(gGS, 0x6F, 0x2F, 0x1C, 0x19);	// Games played
		m_mainGameMeterStepdown.AddMeterItem(gGW, 0x6F, 0x2F, 0x1C, 0);	// Games won
		m_mainGameMeterStepdown.AddMeterItem(gCC, 0x6F, 0x2F, 0x10, 0);		// Total canceled credits
		m_mainGameMeterStepdown.AddMeterItem(gBW, 0x9A, 0, 0, 0);		// Bonus Won (Legacy Bonus meter)
		m_mainGameMeterStepdown.AddMeterItem(gJP, 0x6F, 0x2F, 0x1C, 0x19);	// Jackpot
		m_mainGameMeterStepdown.AddMeterItem(mHCC, 0x6F, 0x2F, 0x2D, 0);	// Hand-paid canceled credits
		m_mainGameMeterStepdown.AddMeterItem(mD200, 0x6F, 0x2F, 0x3A, 0);		//{Two Hundreds}
		m_mainGameMeterStepdown.AddMeterItem(mD500, 0x6F, 0x2F, 0x38, 0);		//{Five Hundreds}
		m_mainGameMeterStepdown.AddMeterItem(mD1000, 0x6F, 0x2F, 0x39, 0);		//{One Thousands}
		m_mainGameMeterStepdown.AddMeterItem(mTValueCI, 0x6F, 0x2F, 0, 0);	// Physical Coin In
		m_mainGameMeterStepdown.AddMeterItem(mTValueCO, 0x6F, 0x2F, 0, 0);	// Physical Coin Out
		m_mainGameMeterStepdown.AddMeterItem(mGameWTPP, 0x6F, 0x2F, 0, 0);	// Weighted Theoretical Payback Percentage
		m_mainGameMeterStepdown.AddMeterItem(mTotalDrop, 0x6F, 0x2F, 0x1C, 0);	// Total Drop
		m_mainGameMeterStepdown.AddMeterItem(mBonusMach, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mBonusAtt, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPrgPayAtt, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPrgPayMac, 0x6F, 0x2F, 0, 0);
		// Ticketing meters
		m_mainGameMeterStepdown.AddMeterItem(mTVal, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mTO, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCPromoOutVal, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCPromoOutCnt, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mHPCshRcptVal, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mHPCshRcptCnt, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mHPWinRcptCnt, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mHPWinRcptVal, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCoupV, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mVchrTktDrop, 0x6F, 0x2F, 00, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCoup, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCTktPV, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCTktPN, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCTktCV, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mCTktCN, 0x6F, 0x2F, 0, 0);
		// PBT Meters
		m_mainGameMeterStepdown.AddMeterItem(mPbAR2G, 0x6F, 0x2F, 0x1d, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAN2G, 0x6F, 0x2F, 0x1d, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAC2H, 0x6F, 0x2F, 0x1d, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAC2G, 0x6F, 0x2F, 0x1d, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAR2H, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAN2H, 0x6F, 0x2F, 0, 0);
		//m_mainGameMeterStepdown.AddMeterItem(mPtR,	0x1d,	0,	0,	0);		
		//m_mainGameMeterStepdown.AddMeterItem(mPBn,	0x1d,	0,	0,	0);		
		m_mainGameMeterStepdown.AddMeterItem(mRstrPlayd, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mURstrPlayd, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mPbAD2G, 0x6F, 0x2F, 0, 0);
		m_mainGameMeterStepdown.AddMeterItem(mNcepCredits, 0x6F, 0x2F, 0x27, 0);


		// Populate sub game meter stepdown map here
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_GAMES_PLAYED, 0x6F, 0x2F, 0x52, 0);	// Coin in
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_COIN_IN, 0x6F, 0x2F, 0x52, 0);	// Games played
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_COIN_OUT, 0x6F, 0x2F, 0, 0);	// Games won
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_JACKPOT, 0x6F, 0x2F, 0x52, 0);	// Coin Out
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_GAMES_WON, 0x6F, 0x2F, 0, 0);	// Machine Pay Progressive payout
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_PROG, 0x6F, 0x2F, 0, 0);	// External Bonusing machine payout
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_ATT_PAID_EXT_BONUS, 0x6F, 0x2F, 0x52, 0);	// Jackpot
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_MACH_PAID_PROG, 0x6F, 0x2F, 0, 0);	// Attendant Pay Progressive payout
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_MACH_PAID_EXT_BONUS, 0x6F, 0x2F, 0, 0);	// External Bonusing Attendant payout
		m_subGameMeterStepdown.AddMeterItem(SUBGAME_METER_WTPP, 0x6F, 0x2F, 0, 0);	// Weighted Theoretical Payback Percentage
	}

	virtual void SetUp()
	{
		cons = new CSASConsumer();
		gcf = new CQueueList("");
		in = new CQueueList("");

		PopulateMeterStepdowns();
	}

	virtual void TearDown()
	{
		delete cons;
		delete gcf;
		delete in;
	}

	};

	TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionHighPorioirityMetersSuccessful)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());

			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);

			int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();
				m->SetMeter(gCI, 100);
				m->SetMeter(gCO, 101);
				m->SetMeter(mCTktPV, 102); // Promo Ticket In
				m->SetMeter(mPbAR2G, 103); // PBT NCEP In
				m->SetMeter(mCPromoOutVal, 104); // Promo Ticket Out
				m->SetMeter(mPbAR2H, 105); // PBT NCEP Out
				m->SetMeter(mNcepCredits, 106); // NCEP Credits on game
				m->SetMeter(mRstrPlayd, 107); // NCEP Played
				m->SetMeter(mCrd, 108); // Credit Meter
				m->SetMeter(gJP, 109); // Jackpot
				m->SetMeter(gGS, 110); // Games played
				m->SetMeter(mBonusMach, 111); // Machine-paid bonus for final bonus award m.
				m->SetMeter(mBonusAtt, 112); // Machine-paid bonus for final bonus award m.
				m->SetMeter(gBW, 113); //Legacy Bonus meter
				msg->SetReturnValue(SASPOLL_SUCCESS);
				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

			EXPECT_TRUE(acquiredMeterCount == 14);
			EXPECT_TRUE(meters->GetMeterByNumber(gCI)->GetValue() == 100);
			EXPECT_TRUE(meters->GetMeterByNumber(gCO)->GetValue() == 101);
			EXPECT_TRUE(meters->GetMeterByNumber(mCTktPV)->GetValue() == 102);
			EXPECT_TRUE(meters->GetMeterByNumber(mPbAR2G)->GetValue() == 103);
			EXPECT_TRUE(meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == 104);
			EXPECT_TRUE(meters->GetMeterByNumber(mPbAR2H)->GetValue() == 105);
			EXPECT_TRUE(meters->GetMeterByNumber(mNcepCredits)->GetValue() == 106);
			EXPECT_TRUE(meters->GetMeterByNumber(mRstrPlayd)->GetValue() == 107);
			EXPECT_TRUE(meters->GetMeterByNumber(mCrd)->GetValue() == 108);
			EXPECT_TRUE(meters->GetMeterByNumber(gJP)->GetValue() == 109);
			EXPECT_TRUE(meters->GetMeterByNumber(gGS)->GetValue() == 110);
			EXPECT_TRUE(meters->GetMeterByNumber(mBonusMach)->GetValue() == 111);
			EXPECT_TRUE(meters->GetMeterByNumber(mBonusAtt)->GetValue() == 112);
			EXPECT_TRUE(meters->GetMeterByNumber(gBW)->GetValue() == 113);

		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionHighPorioirityMetersNack)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);

			int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();
				m->SetMeter(gCI, 100);
				m->SetMeter(gCO, 101);
				m->SetMeter(mCTktPV, 102); // Promo Ticket In
				m->SetMeter(mPbAR2G, 103); // PBT NCEP In
				m->SetMeter(mCPromoOutVal, 104); // Promo Ticket Out
				m->SetMeter(mPbAR2H, 105); // PBT NCEP Out
				m->SetMeter(mNcepCredits, 106); // NCEP Credits on game
				m->SetMeter(mRstrPlayd, 107); // NCEP Played
				m->SetMeter(mCrd, 108); // Credit Meter
				m->SetMeter(gJP, 109); // Jackpot
				m->SetMeter(gGS, 110); // Games played
				m->SetMeter(mBonusMach, 111); // Machine-paid bonus for final bonus award m.
				m->SetMeter(mBonusAtt, 112); // Machine-paid bonus for final bonus award m.
				m->SetMeter(gBW, 113); //Legacy Bonus meter
				msg->SetReturnValue(SASPOLL_NACK);
				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

			EXPECT_TRUE(acquiredMeterCount == 0);

		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionHighPorioirityMetersSomeSuccessful)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);

			int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();
				m->SetMeter(gCI, 100);
				m->SetMeter(gCO, 101);
				m->SetMeter(mCTktPV, 102); // Promo Ticket In
				m->SetMeter(mPbAR2G, 103); // PBT NCEP In
				m->SetMeter(mCrd, 108); // Credit Meter
				m->SetMeter(gJP, 109); // Jackpot
				m->SetMeter(gGS, 110); // Games played
				m->SetMeter(mBonusMach, 111); // Machine-paid bonus for final bonus award m.
				m->SetMeter(mBonusAtt, 112); // Machine-paid bonus for final bonus award m.
				m->SetMeter(gBW, 113); //Legacy Bonus meter
				msg->SetReturnValue(SASPOLL_SUCCESS);
				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

			EXPECT_TRUE(acquiredMeterCount == 10);
			EXPECT_TRUE(meters->GetMeterByNumber(gCI)->GetValue() == 100);
			EXPECT_TRUE(meters->GetMeterByNumber(gCO)->GetValue() == 101);
			EXPECT_TRUE(meters->GetMeterByNumber(mCTktPV)->GetValue() == 102);
			EXPECT_TRUE(meters->GetMeterByNumber(mPbAR2G)->GetValue() == 103);
			EXPECT_TRUE(meters->GetMeterByNumber(mCPromoOutVal)->GetValue() == INVALID_METER_VALUE);
			EXPECT_TRUE(meters->GetMeterByNumber(mPbAR2H)->GetValue() == INVALID_METER_VALUE);
			EXPECT_TRUE(meters->GetMeterByNumber(mNcepCredits)->GetValue() == INVALID_METER_VALUE);
			EXPECT_TRUE(meters->GetMeterByNumber(mRstrPlayd)->GetValue() == INVALID_METER_VALUE);
			EXPECT_TRUE(meters->GetMeterByNumber(mCrd)->GetValue() == 108);
			EXPECT_TRUE(meters->GetMeterByNumber(gJP)->GetValue() == 109);
			EXPECT_TRUE(meters->GetMeterByNumber(gGS)->GetValue() == 110);
			EXPECT_TRUE(meters->GetMeterByNumber(mBonusMach)->GetValue() == 111);
			EXPECT_TRUE(meters->GetMeterByNumber(mBonusAtt)->GetValue() == 112);
			EXPECT_TRUE(meters->GetMeterByNumber(gBW)->GetValue() == 113);
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionHighPorioirityPollCount)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);
			int pollCount = 0;
			int acquiredMeterCount = collect.CollectMeters([&pollCount](CSASPollMessage *msg, int priority)
			{

				// Ananymous function to send to sas
				pollCount++;
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();
				for (int mi = 0; mi < m->GetMeterCount(); mi++)
				{
					m->GetMeterByIndex(mi)->SetValue(100);
				}
				msg->SetReturnValue(SASPOLL_SUCCESS);

				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

			EXPECT_TRUE(acquiredMeterCount == 14);
			EXPECT_TRUE(pollCount == 3);
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionHighPorioirityPollCountSuccessfulPollsNoMeters)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);
			int pollCount = 0;
			int acquiredMeterCount = collect.CollectMeters([&pollCount](CSASPollMessage *msg, int priority)
			{

				// Ananymous function to send to sas
				pollCount++;
				msg->SetReturnValue(SASPOLL_SUCCESS);

				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

			EXPECT_TRUE(pollCount == 14);
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionStepdownFailedPoll)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			sas.SetGameComStatus(true);
			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in

			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);

			for (int i = 0; i < 3; i++)
			{
				CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);
				int acquiredMeterCount = collect.CollectMeters([](CSASPollMessage *msg, int priority)
				{
					// Ananymous function to send to sas, in this case poll failed
					msg->SetReturnValue(SASPOLL_NACK);
					return;
				},
					[](CInternalMessage *msg, int priority)
				{
					// Anonymous function to send to internals
					return;
				});

				EXPECT_TRUE(acquiredMeterCount == 0);
			}

			// Look for step back long poll
			CSASMeterCollection collectLast(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &m_mainGameMeterStepdown, false);
			collect.CollectMeters([](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas, check long poll for fallback value
				EXPECT_TRUE(msg->GetPollValue() == 0x2f);
				msg->SetReturnValue(SASPOLL_SUCCESS);
				return;
			},
				[](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				delete msg;
				return;
			});

		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionNonSnapshotCheckInternalsCall)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			bool isSnapshot = false;
			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, isSnapshot, false, &m_mainGameMeterStepdown, false);

			int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();

				m->SetMeter(gCI, 100);
				m->SetMeter(gCO, 101);
				m->SetMeter(mCTktPV, 102); // Promo Ticket In
				m->SetMeter(mPbAR2G, 103); // PBT NCEP In
				m->SetMeter(mCPromoOutVal, 104); // Promo Ticket Out
				m->SetMeter(mPbAR2H, 105); // PBT NCEP Out
				m->SetMeter(mNcepCredits, 106); // NCEP Credits on game
				m->SetMeter(mRstrPlayd, 107); // NCEP Played
				m->SetMeter(mCrd, 108); // Credit Meter
				m->SetMeter(gJP, 109); // Jackpot
				m->SetMeter(gGS, 110); // Games played
				m->SetMeter(mBonusMach, 111); // Machine-paid bonus for final bonus award m.
				m->SetMeter(mBonusAtt, 112); // Machine-paid bonus for final bonus award m.
				m->SetMeter(gBW, 113); //Legacy Bonus meter
				msg->SetReturnValue(SASPOLL_SUCCESS);
				return;
			},
				[this](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals, Should not be called when not in snapshot
				EXPECT_TRUE(false);
				delete msg;
				return;
			});
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionSnapshotCheckInternalsCall)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			int lastMeters = -1;
			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gCI); // Coin in
			meters->SetMeter(mCTktPV); // Promo Ticket In
			meters->SetMeter(mPbAR2G); // PBT NCEP In
			meters->SetMeter(mCPromoOutVal); // Promo Ticket Out
			meters->SetMeter(mPbAR2H); // PBT NCEP Out
			meters->SetMeter(mNcepCredits); // NCEP Credits on game
			meters->SetMeter(mRstrPlayd); // NCEP Played
			meters->SetMeter(gCO); // Coin out
			meters->SetMeter(mCrd); // Credit Meter
			meters->SetMeter(gJP); // Jackpot
			meters->SetMeter(gGS); // Games played
			meters->SetMeter(mBonusMach); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(mBonusAtt); // Machine-paid bonus for final bonus award meters.
			meters->SetMeter(gBW); //Legacy Bonus meter

			bool isSnapshot = true;
			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, isSnapshot, false, &m_mainGameMeterStepdown, false);
			bool firstCall = true;
			int acquiredMeterCount = collect.CollectMeters([this](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				CMetersLongPoll *lp(dynamic_cast<CMetersLongPoll *>(msg));
				EXPECT_TRUE(lp != nullptr);
				CMeters *m = lp->GetMeters();
				m->SetMeter(gCI, 100);
				m->SetMeter(gCO, 101);
				m->SetMeter(mCTktPV, 102); // Promo Ticket In
				m->SetMeter(mPbAR2G, 103); // PBT NCEP In
				m->SetMeter(mCPromoOutVal, 104); // Promo Ticket Out
				m->SetMeter(mPbAR2H, 105); // PBT NCEP Out
				m->SetMeter(mNcepCredits, 106); // NCEP Credits on game
				m->SetMeter(mRstrPlayd, 107); // NCEP Played
				m->SetMeter(mCrd, 108); // Credit Meter
				m->SetMeter(gJP, 109); // Jackpot
				m->SetMeter(gGS, 110); // Games played
				m->SetMeter(mBonusMach, 111); // Machine-paid bonus for final bonus award m.
				m->SetMeter(mBonusAtt, 112); // Machine-paid bonus for final bonus award m.
				m->SetMeter(gBW, 113); //Legacy Bonus meter
				msg->SetReturnValue(SASPOLL_SUCCESS);
				return;
			},
				[&firstCall, &lastMeters](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals, first call 0 games, > 0 after incrementing each call
				if (firstCall)
				{
					EXPECT_TRUE(msg->GetWordData() == 0);
					firstCall = false;
				}
				else
					EXPECT_TRUE(msg->GetWordData() > 0);

				EXPECT_TRUE((int)msg->GetWordData() > lastMeters);
				lastMeters = msg->GetWordData();
				delete msg;
				return;
			});
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionIsIndividualBillLPTrue)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			
			// Custom stepdowns
			CMeterStepdown mainGameMeterStepdown;
			mainGameMeterStepdown.AddMeterItem(mD1, 0x31, 0x00, 0, 0);		//{Ones}
			mainGameMeterStepdown.AddMeterItem(mD2, 0x32, 0, 0, 0);		//{Twos}
			mainGameMeterStepdown.AddMeterItem(mD5, 0x33, 0, 0, 0);		//{Fives}
			mainGameMeterStepdown.AddMeterItem(mD10, 0x34, 0, 0, 0);		//{Tens}
			mainGameMeterStepdown.AddMeterItem(mD20, 0x35, 0, 0, 0);		//{Twenties}
			mainGameMeterStepdown.AddMeterItem(mD50, 0x36, 0, 0, 0);		//{Fifties}
			mainGameMeterStepdown.AddMeterItem(mD100, 0x37, 0, 0, 0);		//{Hundreds}


			CSASInterface sas(mockSerial, cons, gcf, in);
			sas.SetGameComStatus(true);

			int lastMeters = -1;
			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(mD1); // 
			meters->SetMeter(mD10); // 
			meters->SetMeter(mD20); // 
			meters->SetMeter(mD100); // 
			meters->SetMeter(mD1000); // 
			meters->SetMeter(mD5); // 
			meters->SetMeter(mD50); // 
			meters->SetMeter(mD500); // 

			bool isSnapshot = true;
			int nCalls = 0;
		
			CSASMeterCollection collect1(&sas, meters.get(), EGM_GAME_NUMBER, 1, false, false, &mainGameMeterStepdown, false); 
			int acquiredMeterCount = collect1.CollectMeters([&nCalls](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas, in this case poll failed
				msg->SetReturnValue(SASPOLL_NACK);
				nCalls++;
				return;
			},
				[](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals
				return;
			});

			// Should have been more than 3 attempts
			EXPECT_TRUE(nCalls > 3);
		}

		TEST_F(SASMeterCollectionUnitTests, SASMeterCollectionTryOnce)
		{
			NiceMock<CSASSerialWrapperMock> *mockSerial(new NiceMock<CSASSerialWrapperMock>());
			CSASInterface sas(mockSerial, cons, gcf, in);

			int lastMeters = -1;
			unique_ptr<CMeters> meters(new CMeters(MEMORY_NONE));
			meters->SetMeter(gBW); // Coin in
			int nCalls = 0;
			bool isSnapshot = true;
			CSASMeterCollection collect(&sas, meters.get(), EGM_GAME_NUMBER, 1, isSnapshot, true, &m_mainGameMeterStepdown, false);
			bool firstCall = true;
			int acquiredMeterCount = collect.CollectMeters([&nCalls](CSASPollMessage *msg, int priority)
			{
				// Ananymous function to send to sas
				msg->SetReturnValue(SASPOLL_NACK);
				nCalls++;
				return;
			},
				[](CInternalMessage *msg, int priority)
			{
				// Anonymous function to send to internals, first call 0 games, > 0 after incrementing each call
				delete msg;
				return;
			});

			EXPECT_TRUE(nCalls == 1);
		}
}
