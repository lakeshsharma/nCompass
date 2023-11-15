#include "stdafx.h"
#include "ExcessiveVoucherOut.h"
#include "PollerProtocol/PollerGenericTransactionMessage.h"
#include "Config/ConfigExcessiveVoucherOutEnabledField.h"
#include "Config/ConfigExcessiveVoucherOutThresholdField.h"
#include "gtest/gtest.h"

#include "UnitTestingOperatingSystem.h"

using namespace std;

class ExcessiveVoucherOutTests : public ::testing::Test{};
		
TEST_F(ExcessiveVoucherOutTests, ExcessiveVoucherOutConstructorTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	ASSERT_TRUE(excessiveVoucherOut.m_excessiveVoucherOutThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_totalBillInThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinInThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinOutThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_FALSE(excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag);
}

TEST_F(ExcessiveVoucherOutTests, ThresholdValuesAndGlobalExcessiveVoucherOutEnabledFlagTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	ASSERT_TRUE(excessiveVoucherOut.m_excessiveVoucherOutThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_totalBillInThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinInThreshold == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinOutThreshold == 0);
	ASSERT_FALSE(excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag);
	excessiveVoucherOut.SetGlobalExcessiveVoucherOutEnabledFlag(true);
	excessiveVoucherOut.SetExcessiveVoucherOutThreshold(1);
	excessiveVoucherOut.SetTotalBillInThreshold(2);
	excessiveVoucherOut.SetCoinInThreshold(3);
	excessiveVoucherOut.SetCoinOutThreshold(4);
	ASSERT_TRUE(excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag);
	ASSERT_TRUE(excessiveVoucherOut.m_excessiveVoucherOutThreshold == 1);
	ASSERT_TRUE(excessiveVoucherOut.m_totalBillInThreshold == 2);
	ASSERT_TRUE(excessiveVoucherOut.m_coinInThreshold == 3);
	ASSERT_TRUE(excessiveVoucherOut.m_coinOutThreshold == 4);
}

TEST_F(ExcessiveVoucherOutTests, GameCreditsBecameZeroTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag = true;
	excessiveVoucherOut.m_excessiveVoucherOutThreshold = 1;
	excessiveVoucherOut.m_totalBillInThreshold = 2;
	excessiveVoucherOut.m_coinInThreshold = 3;
	excessiveVoucherOut.m_coinOutThreshold = 4;
	excessiveVoucherOut.m_currentBillInPennies = 0;
	excessiveVoucherOut.m_currentCoinInPennies = 6;
	excessiveVoucherOut.m_currentCoinOutPennies = 7;
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 6);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 7);
	excessiveVoucherOut.GameCreditsBecameZero();
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 6);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 7);
	excessiveVoucherOut.m_currentBillInPennies = 1;
	excessiveVoucherOut.GameCreditsBecameZero();
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
}

TEST_F(ExcessiveVoucherOutTests, ProcessBillInTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	excessiveVoucherOut.ProcessBillIn(100);
	excessiveVoucherOut.ProcessBillIn(300);
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 400);
}

TEST_F(ExcessiveVoucherOutTests, ProcessCoinInTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	excessiveVoucherOut.SetCoinInThreshold(1000);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinInThreshold == 1000);
	excessiveVoucherOut.ProcessCoinIn(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	excessiveVoucherOut.ProcessBillIn(100);
	excessiveVoucherOut.ProcessCoinIn(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 100);
	excessiveVoucherOut.ProcessCoinIn(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 200);
	excessiveVoucherOut.ProcessCoinIn(300);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 500);
	excessiveVoucherOut.ProcessCoinIn(499);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 999);
	excessiveVoucherOut.ProcessCoinIn(1);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
}

TEST_F(ExcessiveVoucherOutTests, ProcessCoinOutTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	excessiveVoucherOut.SetCoinOutThreshold(1000);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_coinOutThreshold == 1000);
	excessiveVoucherOut.ProcessCoinOut(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	excessiveVoucherOut.ProcessBillIn(100);
	excessiveVoucherOut.ProcessCoinOut(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 100);
	excessiveVoucherOut.ProcessCoinOut(100);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 200);
	excessiveVoucherOut.ProcessCoinOut(300);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 500);
	excessiveVoucherOut.ProcessCoinOut(499);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 999);
	excessiveVoucherOut.ProcessCoinOut(1);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
}

TEST_F(ExcessiveVoucherOutTests, ProcessVoucherOutTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag = true;
	excessiveVoucherOut.m_excessiveVoucherOutThreshold = 900;
	excessiveVoucherOut.m_totalBillInThreshold = 1000;
	excessiveVoucherOut.m_coinInThreshold = 500;
	excessiveVoucherOut.m_coinOutThreshold = 500;
	excessiveVoucherOut.m_currentBillInPennies = 0;
	excessiveVoucherOut.m_currentCoinInPennies = 0;
	excessiveVoucherOut.m_currentCoinOutPennies = 0;

	DWORD amountInPennies(0), currentBillInPennies(0);
	ASSERT_FALSE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 0);
	ASSERT_TRUE(currentBillInPennies == 0);
	
	excessiveVoucherOut.m_currentBillInPennies = 1000;
	ASSERT_FALSE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 0);
	ASSERT_TRUE(currentBillInPennies == 0);
	
	amountInPennies = 900;
	ASSERT_FALSE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 900);
	ASSERT_TRUE(currentBillInPennies == 0);

	excessiveVoucherOut.m_currentBillInPennies = 1000;
	excessiveVoucherOut.m_currentCoinInPennies = 500;
	ASSERT_FALSE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 900);
	ASSERT_TRUE(currentBillInPennies == 0);

	excessiveVoucherOut.m_currentBillInPennies = 1000;
	excessiveVoucherOut.m_currentCoinOutPennies = 500;
	ASSERT_FALSE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 900);
	ASSERT_TRUE(currentBillInPennies == 0);

	excessiveVoucherOut.m_currentBillInPennies = 1000;
	excessiveVoucherOut.m_currentCoinInPennies = 499;
	excessiveVoucherOut.m_currentCoinOutPennies = 499;
	ASSERT_TRUE(excessiveVoucherOut.ProcessVoucherOut(amountInPennies, currentBillInPennies));
	ASSERT_TRUE(excessiveVoucherOut.m_currentBillInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinInPennies == 0);
	ASSERT_TRUE(excessiveVoucherOut.m_currentCoinOutPennies == 0);
	ASSERT_TRUE(amountInPennies == 900);
	ASSERT_TRUE(currentBillInPennies == 1000);
}

TEST_F(ExcessiveVoucherOutTests, HtmlStringTest)
{
	CExcessiveVoucherOut excessiveVoucherOut;
	excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag = true;
	excessiveVoucherOut.m_excessiveVoucherOutThreshold = 1;
	excessiveVoucherOut.m_totalBillInThreshold = 2;
	excessiveVoucherOut.m_coinInThreshold = 3;
	excessiveVoucherOut.m_coinOutThreshold = 4;
	excessiveVoucherOut.m_currentBillInPennies = 5;
	excessiveVoucherOut.m_currentCoinInPennies = 6;
	excessiveVoucherOut.m_currentCoinOutPennies = 7;

	string html1("Excessive Voucher Out<br>Global Excessive Voucher Out Enabled Flag: Yes<br>Excessive Voucher Out Threshold (pennies): 1<br>Total Bill In Threshold (pennies): 2<br>Coin In Threshold (pennies): 3<br>Coin Out Threshold (pennies): 4<br>Current Bill In (pennies): 5<br>Current Coin In (pennies): 6<br>Current Coin Out (pennies): 7<br><br>");
	ASSERT_TRUE(excessiveVoucherOut.GetHtmlString() == html1);

	excessiveVoucherOut.m_globalExcessiveVoucherOutEnabledFlag = false;
	string html2("Excessive Voucher Out<br>Global Excessive Voucher Out Enabled Flag: No<br>Excessive Voucher Out Threshold (pennies): 1<br>Total Bill In Threshold (pennies): 2<br>Coin In Threshold (pennies): 3<br>Coin Out Threshold (pennies): 4<br>Current Bill In (pennies): 5<br>Current Coin In (pennies): 6<br>Current Coin Out (pennies): 7<br><br>");
	ASSERT_TRUE(excessiveVoucherOut.GetHtmlString() == html2);
}

TEST_F(ExcessiveVoucherOutTests, PollerGenericTransactionMessageTests)
{
	// Setup for CPollerGenericTransactionMessage.
	CMeters *meters = (new CMeters(MEMORY_NONE));
	byte transCode = EXCESSIVE_TICKET_OUT;
	const byte * cardID = reinterpret_cast<const byte*>("1234567890");

	//SETUP
	CPollerGenericTransactionMessage pollerMessage(meters, transCode, 123, cardID, 1, 2, 3);

	//TEST
	ASSERT_TRUE(pollerMessage.m_uniqueId == 123);
	ASSERT_TRUE(pollerMessage.m_meterCount == 0);
	ASSERT_TRUE(pollerMessage.m_transCode == (byte)EXCESSIVE_TICKET_OUT);
	ASSERT_TRUE(pollerMessage.m_sessionPoints == 1);
	ASSERT_TRUE(pollerMessage.m_residualValue == 2);
	ASSERT_TRUE(pollerMessage.m_bonusPoints == 3);
	ASSERT_TRUE(pollerMessage.m_dataCount == 3);
	ASSERT_FALSE(pollerMessage.m_bBufferIsSet);
	pollerMessage.SetReadWriteBuffer();
	ASSERT_TRUE(pollerMessage.MessageHeader.MessageID == POLLER_MSG_TRANSACTION);
	ASSERT_TRUE(pollerMessage.m_bBufferIsSet);
	ASSERT_TRUE(pollerMessage.MessageHeader.DataLength == 62);

	//CLEANUP
	delete meters;
}


TEST_F(ExcessiveVoucherOutTests, ConfigExcessiveVoucherOutEnabledFieldTests)
{
	CConfigExcessiveVoucherOutEnabledField config;
	ASSERT_FALSE(config.IsEnabled());
	config.SetEnabledFlag(true);
	ASSERT_TRUE(config.IsEnabled());
}

TEST_F(ExcessiveVoucherOutTests, ConfigExcessiveVoucherOutThresholdFieldTests)
{
	CConfigExcessiveVoucherOutThresholdField excessiveVoucherOutThresholdField(EXCESSIVE_VOUCHER_OUT_THRESHOLD);
	const int lenght(2);
	const byte buffer1[] = { 0xff, 0x7f };//32767
	const byte buffer2[] = { 0xff, 0x6f };//28671
	const byte buffer3[] = { 0xff, 0x5f };//24575
	const byte buffer4[] = { 0xff, 0x4f };//20479

	excessiveVoucherOutThresholdField.SetThreshold(buffer1, lenght);
	ASSERT_TRUE(excessiveVoucherOutThresholdField.GetType() == EXCESSIVE_VOUCHER_OUT_THRESHOLD);
	ASSERT_TRUE(excessiveVoucherOutThresholdField.GetThreshold() == 32767);

	CConfigExcessiveVoucherOutThresholdField totalBillInThresholdField(TOTAL_BILL_IN_THRESHOLD);
	totalBillInThresholdField.SetThreshold(buffer2, lenght);
	ASSERT_TRUE(totalBillInThresholdField.GetType() == TOTAL_BILL_IN_THRESHOLD);
	ASSERT_TRUE(totalBillInThresholdField.GetThreshold() == 28671);

	CConfigExcessiveVoucherOutThresholdField coinInThresholdField(COIN_IN_THRESHOLD);
	coinInThresholdField.SetThreshold(buffer3, lenght);
	ASSERT_TRUE(coinInThresholdField.GetType() == COIN_IN_THRESHOLD);
	ASSERT_TRUE(coinInThresholdField.GetThreshold() == 24575);

	CConfigExcessiveVoucherOutThresholdField coinOutThresholdField(COIN_OUT_THRESHOLD);
	
	coinOutThresholdField.SetThreshold(buffer4, lenght);
	ASSERT_TRUE(coinOutThresholdField.GetType() == COIN_OUT_THRESHOLD);
	ASSERT_TRUE(coinOutThresholdField.GetThreshold() == 20479);
}