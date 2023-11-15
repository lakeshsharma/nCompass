#include "stdafx.h"
#include "UI/SentinelDlgState.h"
#include "UI/ScreenDefs.h"
#include "gtest/gtest.h"

namespace SentinelUnitTests
{
	class SentinelDlgStateUnitTests : public ::testing::Test
	{
	public:
	};
	TEST_F(SentinelDlgStateUnitTests, SentinelDlgStateSetActiveSessionTest)
	{
		CSentinelDlgState ui_state;
		ASSERT_FALSE(ui_state.IsActiveSession());
		ui_state.SetActiveSession(true);
		ASSERT_TRUE(ui_state.IsActiveSession());
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelDlgState::CSentinelDlgState()
	///     CSentinelDlgState::GetEmployeeCardIn()
	/// Test default value.
	/// </summary>
	TEST_F(SentinelDlgStateUnitTests, SetEmployeeCardIn_DefaultTest)
	{
		// Data.
		bool expected(false);

		// Perform operation(s) and TEST_F(s).
		CSentinelDlgState state;
		ASSERT_EQ(expected, state.GetEmployeeCardIn());
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelDlgState::SetEmployeeCardIn()
	///     CSentinelDlgState::GetEmployeeCardIn()
	/// Test setting the value to true.
	/// </summary>
	TEST_F(SentinelDlgStateUnitTests, SetEmployeeCardIn_TrueTest)
	{
		// Data.
		bool expected(true);

		// Setup.
		CSentinelDlgState state;

		// Perform operation(s) and TEST_F(s).
		state.SetEmployeeCardIn(expected);
		ASSERT_EQ(expected, state.GetEmployeeCardIn());
		ASSERT_EQ(expected, state.GetEmployeeCardIn());
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CSentinelDlgState::SetTimeToClose()
	///     CSentinelDlgState::IsTimeToClose()
	/// </summary>
	TEST_F(SentinelDlgStateUnitTests, TimeToCloseFlagTest)
	{
		// Setup.
		CSentinelDlgState state;

		// Perform operation(s) and TEST_F(s).
		ASSERT_FALSE(state.IsTimeToClose());
		state.SetTimeToClose(false);
		ASSERT_FALSE(state.IsTimeToClose());
		state.SetTimeToClose(true);
		ASSERT_TRUE(state.IsTimeToClose());
	}

	TEST_F(SentinelDlgStateUnitTests, EmployeeCardEventSubscriptionTest)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		ASSERT_TRUE(INT_MSG_DISPLAY_EMPLOYEE_SCREEN == msg->GetMessageType());
		delete msg;

		
	}

	TEST_F(SentinelDlgStateUnitTests, SetCardEventCARD_MECHANIC_Test)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		string ExpectedSubscriptionData;

		sentinelDlgState.SetCardEventSubscription(CARD_MECHANIC, false, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"mechanic\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_MECHANIC, true, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"mechanic\",\"ShowApplyRevertScreen\":true,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_MECHANIC, true, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"mechanic\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_MECHANIC, false, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"mechanic\",\"ShowApplyRevertScreen\":false,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);
	}

	TEST_F(SentinelDlgStateUnitTests, SetCardEventCARD_GLOBAL_Test)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		string ExpectedSubscriptionData;

		sentinelDlgState.SetCardEventSubscription(CARD_GLOBAL, false, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"global\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_GLOBAL, true, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"global\",\"ShowApplyRevertScreen\":true,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_GLOBAL, true, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"global\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_GLOBAL, false, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"global\",\"ShowApplyRevertScreen\":false,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);
	}

	TEST_F(SentinelDlgStateUnitTests, SetCardEventCARD_FLOOR_Test)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		string ExpectedSubscriptionData;

		sentinelDlgState.SetCardEventSubscription(CARD_FLOOR, false, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"floor\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_FLOOR, true, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"floor\",\"ShowApplyRevertScreen\":true,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_FLOOR, true, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"floor\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_FLOOR, false, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"floor\",\"ShowApplyRevertScreen\":false,\"PinRequired\":true}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);
	}

	TEST_F(SentinelDlgStateUnitTests, SetCardEventCARD_METER_Test)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		string ExpectedSubscriptionData;

		sentinelDlgState.SetCardEventSubscription(CARD_METER, false, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"meter\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_METER, true, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"meter\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_METER, true, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"meter\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_METER, false, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"meter\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);
	}

	TEST_F(SentinelDlgStateUnitTests, SetCardEventCARD_CURRENCY_Test)
	{
		string subscriptionData("{\"jsonrpc\":\"2.0\", \"params\":{\"type\":\"EmployeeCardEvent\", \"where\":{\"id\":3147483647}}, \"method\":\"subscribe\", \"id\":\"69\"}");
		CSentinelDlgState sentinelDlgState;
		CInternalMessage* msg(sentinelDlgState.CreateAngularUIMessageForInternals(subscriptionData));
		string ExpectedSubscriptionData;

		sentinelDlgState.SetCardEventSubscription(CARD_CURRENCY, false, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"currency\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_CURRENCY, true, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"currency\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_CURRENCY, true, false);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"currency\",\"ShowApplyRevertScreen\":true,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);

		sentinelDlgState.SetCardEventSubscription(CARD_CURRENCY, false, true);
		sentinelDlgState.GetEmployeeCardEventSubscriptionData(subscriptionData);
		ExpectedSubscriptionData = "{\"jsonrpc\":\"2.0\",\"type\":\"EmployeeCardEvent\",\"id\":\"69\",\"result\":{\"EmployeeCardEvent\":{\"CardType\":\"currency\",\"ShowApplyRevertScreen\":false,\"PinRequired\":false}}}";
		ASSERT_TRUE(subscriptionData == ExpectedSubscriptionData);
	}
}
