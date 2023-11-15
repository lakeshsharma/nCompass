// Sentinel includes.
#include "InternalMessage.h"
#include "Utilities.h"
#include "SentinelMock.h"
#include "InternalsFrameworkMock.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
    class InternalsProcessingForUiUnitTests :public ::testing::Test
	{
	protected:
		CSentinelMock m_SentinelMock;
		CInternalsFrameworkMock *m_internalsMock;
		InternalsProcessingForUiUnitTests() {}

		virtual void SetUp()
		{
			CDirectoryManager::MockInstance(NULL);
			m_internalsMock = new CInternalsFrameworkMock(&m_SentinelMock, CUtilities::GetCurrentTime(), false, "2.8");
		}

		virtual void TearDown()
		{
			CDirectoryManager::MockInstanceCleanup();
		}

		void PrepareData_ProcessUJPCheckPinFromUI(UnattendedJackpotState::Type state)
		{
			CJackpot *pJackpot = new CJackpot(MEMORY_NVRAM, true);
			pJackpot->SetUnattendedState(state);
			m_internalsMock->SetJackpot(pJackpot);

			const unsigned char* cardID = reinterpret_cast<const unsigned char*>("12345678");
			CCard *pCard = new CCard(0, 0);
			pCard->SetCardInfo(CardType::CARD_PLAYER, cardID, 9);
			CEGMConfig egmConfig(MEMORY_NONE);
			CConfig config(false, "2.8");
			CPlayer *pPlayer = new CPlayer(pCard, NULL, config, egmConfig, 0, false, NULL);
			m_internalsMock->SetPlayer(pPlayer);
		}

		void CleanData_ProcessUJPCheckPinFromUI()
		{
			CJackpot *pJackpot = m_internalsMock->GetJackpot();
			CPlayer *pPlayer = m_internalsMock->GetPlayer();

			m_internalsMock->SetJackpot(NULL);
			m_internalsMock->SetPlayer(NULL);

			delete pJackpot;
			delete pPlayer;
		}

		void Validate_ProcessUJPCheckPinFromUI_ValidInputs()
		{
			// Setup.
			PrepareData_ProcessUJPCheckPinFromUI(UnattendedJackpotState::UJPPinEntry);

			// UI message to process by internals
			CInternalMessage *msg = new CInternalMessage(INT_MSG_UJP_CHECK_PIN);
			msg->SetData("1234");
			msg->SetData((WORD)true);

			// Perform operation(s) and test(s).
			m_internalsMock->ProcessMessageFromUI(msg);
			ASSERT_EQ(m_internalsMock->GetJackpot()->GetUnattendedState(), UnattendedJackpotState::UJPPinSent);

			// cleanup
			CleanData_ProcessUJPCheckPinFromUI();
		}

		void Validate_ProcessUJPCheckPinFromUI_InvalidJackpotState()
		{
			// Setup.
			PrepareData_ProcessUJPCheckPinFromUI(UnattendedJackpotState::UJPNone);

			// UI message to process by internals
			CInternalMessage *msg = new CInternalMessage(INT_MSG_UJP_CHECK_PIN);
			msg->SetData("1234");
			msg->SetData((WORD)true);

			// Perform operation(s) and test(s).
			m_internalsMock->ProcessMessageFromUI(msg);
			ASSERT_EQ(m_internalsMock->GetJackpot()->GetUnattendedState(), UnattendedJackpotState::UJPNone);

			// cleanup
			CleanData_ProcessUJPCheckPinFromUI();
		}

		void Validate_ProcessUJPCheckPinFromUI_InvalidPinLength()
		{
			// Setup.
			PrepareData_ProcessUJPCheckPinFromUI(UnattendedJackpotState::UJPPinEntry);

			// UI message to process by internals
			CInternalMessage *msg = new CInternalMessage(INT_MSG_UJP_CHECK_PIN);
			msg->SetData("1234");
			msg->SetData((WORD)false);

			// Perform operation(s) and test(s).
			m_internalsMock->ProcessMessageFromUI(msg);
			ASSERT_EQ(m_internalsMock->GetJackpot()->GetUnattendedState(), UnattendedJackpotState::UJPPinEntry);

			// cleanup
			CleanData_ProcessUJPCheckPinFromUI();
		}
	};

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::ProcessUJPCheckPinFromUI()
	/// Test for jackpot state after ProcessUJPCheckPinFromUI() call with valid inputs.
	/// </summary>
	TEST_F(InternalsProcessingForUiUnitTests, ProcessUJPCheckPinFromUI_ValidInputs)
	{
		Validate_ProcessUJPCheckPinFromUI_ValidInputs();
	}

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::ProcessUJPCheckPinFromUI()
	/// Test for jackpot state after ProcessUJPCheckPinFromUI() call with invalid jackpot state.
	/// </summary>
	TEST_F(InternalsProcessingForUiUnitTests, ProcessUJPCheckPinFromUI_InvalidJackpotState)
	{
		Validate_ProcessUJPCheckPinFromUI_InvalidJackpotState();
	}

	/// <summary>
	/// Functional test for:
	///     CInternalsFramework::ProcessUJPCheckPinFromUI()
	/// Test for jackpot state after ProcessUJPCheckPinFromUI() call with invalid pin length.
	/// </summary>
	TEST_F(InternalsProcessingForUiUnitTests, ProcessUJPCheckPinFromUI_InvalidPinLength)
	{
		Validate_ProcessUJPCheckPinFromUI_InvalidPinLength();
	}
}	
