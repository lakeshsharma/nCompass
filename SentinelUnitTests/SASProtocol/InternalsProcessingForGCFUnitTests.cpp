// Sentinel includes.
#include "InternalMessage.h"
#include "Utilities.h"
#include "SentinelMock.h"
#include "InternalsFrameworkMock.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"

#include "gtest/gtest.h"

using ::testing::Mock;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace SentinelNativeUnitTests
{
    class InternalsProcessingForGCFUnitTests :public ::testing::Test
	{
	protected:
		NiceMock<CSentinelMock> m_SentinelMock;
		CInternalsFrameworkMock *m_internalsMock;
		InternalsProcessingForGCFUnitTests() {}

		virtual void SetUp()
		{
			CDirectoryManager::MockInstance(NULL);
			m_internalsMock = new CInternalsFrameworkMock(&m_SentinelMock, CUtilities::GetCurrentTime(), false, "2.8");

			ON_CALL(m_SentinelMock, SendMessageToProtocol(_, _, _)).WillByDefault(Invoke(&m_SentinelMock, &CSentinelMock::SendMessageToProtocolMocked));
		}

		virtual void TearDown()
		{
			CDirectoryManager::MockInstanceCleanup();
		}

		void Validate_ProcessPBBalancesFromGCF_PBInfoWithError()
		{
			// Setup
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetErrorCode(AccessError);
			CConfig config(false, "2.8");
			m_internalsMock->ProcessPersonalBankerInfo(pbInfo, config, true, true);

			// GCF message to process by internals
			CInternalMessage *msg = new CInternalMessage(INT_MSG_SET_PB_BALANCES, (DWORD)100 /* cash on game */, (DWORD)100 /* promo on game */);
			msg->SetData((__int64)100 /* cash promo on game */, (__int64)true /* EFT mode */);
			msg->SetData((WORD)false /* can transfer off */, (WORD)false /* call to game failed */);

			// Perform operation(s) and test(s).
			m_internalsMock->ProcessMessageFromGCF(msg);
			ASSERT_TRUE(m_internalsMock->GetPBInfoCopy() == NULL) << "PBInfo is not NULL";
			ASSERT_FALSE(m_internalsMock->HasPBTransfer()) << "PBTransfer is not NULL ";

			// cleanup
			delete pbInfo;
		}

		void Validate_SendMCFundsTransferBalanceToPollerProtocol(bool isCEPOutEnabled)
		{
			// Setup CConfig instance
			CConfig config(false, "2.8");
			if (isCEPOutEnabled)
			{
				config.SetSenOption2Raw(config.GetSenOption2Raw() + SO2_CEP_OUT_ENABLED);
			}
			config.SetPBFromGameEnabled(true);
			config.SetPBTCashEnabled_Global(true);
			config.SetPartialWATOutEnabled(true);
			config.SetPartialWATOutEnabled_Global(true);

			// Setup CPersonalBankerInfo instance
			CPersonalBankerInfo *pbInfo = new CPersonalBankerInfo();
			pbInfo->SetErrorCode(AccessError);
			pbInfo->SetCashBalanceOnGame(1000);
			pbInfo->SetConfigData(config, false, false);

			// Update internals with CConfig & CPersonalBankerInfo instances.
			m_internalsMock->ProcessPersonalBankerInfo(pbInfo, config, true, false);

			// Set mobile PBT balances into CMobilePBTSessionInfo instance
			DWORD cashable = 101;
			DWORD cashPromo = 200;
			DWORD nonCashablePromo = 300;
			m_internalsMock->SetMobilePBTBalances(cashable, cashPromo, nonCashablePromo);

			// Perform operation(s) and test(s).
			m_internalsMock->SendMCFundsTransferBalanceToPollerProtocolMocked();

			// Verification
			CInternalMessage *msg = m_SentinelMock.GetInternalMsg();
			ASSERT_TRUE(msg != NULL) << "No message sent to Poller.";
			ASSERT_EQ(msg->GetMessageType(), INT_MSG_SET_PB_BALANCES) << "Message sent to Poller is not INT_MSG_SET_PB_BALANCES.";

			CFundsTransferBalanceEvent *pFundsTransferBalanceEvent = msg->GetFundsTransferBalanceEventData();
			ASSERT_TRUE(pFundsTransferBalanceEvent != NULL) << "CFundsTransferBalanceEvent instance is NULL in the message sent to Poller.";

			DWORD cashPromo1 = pFundsTransferBalanceEvent->GetCashable() - cashable;
			if (isCEPOutEnabled)
			{
				ASSERT_EQ(cashPromo1, cashPromo) << "Cash promo is not as excepted.";
			}
			else
			{
				ASSERT_TRUE(cashPromo1 == 0) << "Cash promo is not 0.";
			}

			// cleanup
			delete pbInfo;
		}
	
		void Validate_NonNCEPPlayed()
		{
			ASSERT_EQ(m_internalsMock->CalculateNonNCEPPlayedMocked(100, 90), 10);

			ASSERT_EQ(m_internalsMock->CalculateNonNCEPPlayedMocked(90, 100), 0);

			ASSERT_EQ(m_internalsMock->CalculateNonNCEPPlayedMocked(100, 100), 0);
		}
	};

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::ProcessPBBalancesFromGCF()
	/// Test ProcessPBBalancesFromGCF method with error in PBInfo object.
	/// </summary>
	TEST_F(InternalsProcessingForGCFUnitTests, ProcessPBBalancesFromGCF_PBInfoWithError)
	{
		Validate_ProcessPBBalancesFromGCF_PBInfoWithError();
	}

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::SendMCFundsTransferBalanceToPollerProtocol()
	/// Test SendMCFundsTransferBalanceToPollerProtocol method with CEPOutEnabled.
	/// </summary>
	TEST_F(InternalsProcessingForGCFUnitTests, SendMCFundsTransferBalanceToPollerProtocol_CEPOutEnabled)
	{
		Validate_SendMCFundsTransferBalanceToPollerProtocol(true);
	}

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::SendMCFundsTransferBalanceToPollerProtocol()
	/// Test SendMCFundsTransferBalanceToPollerProtocol method with CEPOutDisabled.
	/// </summary>
	TEST_F(InternalsProcessingForGCFUnitTests, SendMCFundsTransferBalanceToPollerProtocol_CEPOutDisabled)
	{
		Validate_SendMCFundsTransferBalanceToPollerProtocol(false);
	}

	/// <summary>
	/// Functional test for the scenario where coin in has rolled over but NCEP Played has not.
	/// NonNCEPPlayed should be set to 0 in this case.
	/// </summary>
	TEST_F(InternalsProcessingForGCFUnitTests, NonNCEPPlayedCalculation)
	{
		Validate_NonNCEPPlayed();
	}

}
