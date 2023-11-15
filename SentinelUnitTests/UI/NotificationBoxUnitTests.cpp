//****NOTE****
//***While porting please note of changes in NotificationBox done for Kestrel project.***

#include "stdafx.h"
#include "UI/NotificationBox.h"

#include "Config/ConfigExtData.h"
#include "GlobalDefs.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"
#include "UnitTestingUtils.h"
#include "gtest/gtest.h"

using namespace std;

namespace SentinelUnitTests
{
	class NotificationBoxUnitTests : public ::testing::Test
	{
		static const string TOPIC_NOTIFICATIONBOX;
		static const string MOBILE_CONNECT_TOPIC;

		// Default expected result(s).
		static const DWORD m_defaultPlayerTrackingDurationExpected = 3000;

		static const DWORD m_playerTrackingDurationExpected = m_defaultPlayerTrackingDurationExpected;
		static const bool m_checkTimedEventsExpected = false;

	public:
		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}

		void GetText_DefaultTest()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;

			// Perform operation(s) to be tested.
			CNotificationBox obj;
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");

			// Verify
			ASSERT_EQ(response, expectedResponse);
		}

		void SetDownloadPendingTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedDownloadPendingResponse;
			expectedDownloadPendingResponse = R"({"NotificationText":"download_pending",)";
			expectedDownloadPendingResponse += R"("NotificationTextArgs":[],)";
			expectedDownloadPendingResponse += R"("NotificationTextPriority":"2",)";
			expectedDownloadPendingResponse += R"("AlertText":"",)";
			expectedDownloadPendingResponse += R"("AlertTextArgs":[],)";
			expectedDownloadPendingResponse += R"("AlertTextPriority":"",)";
			expectedDownloadPendingResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedDownloadPendingResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.SetDownloadPending(true));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedDownloadPendingResponse);

			ASSERT_FALSE(obj.SetDownloadPending(true));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedDownloadPendingResponse);

			ASSERT_TRUE(obj.SetDownloadPending(false));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetDownloadPending(false));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		void SetSystemBusyTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedSystemBusyResponse;
			expectedSystemBusyResponse = R"({"NotificationText":"system_busy",)";
			expectedSystemBusyResponse += R"("NotificationTextArgs":[],)";
			expectedSystemBusyResponse += R"("NotificationTextPriority":"2",)";
			expectedSystemBusyResponse += R"("AlertText":"",)";
			expectedSystemBusyResponse += R"("AlertTextArgs":[],)";
			expectedSystemBusyResponse += R"("AlertTextPriority":"",)";
			expectedSystemBusyResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedSystemBusyResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.SetNotificationState(NotificationState::SYSTEM_BUSY));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedSystemBusyResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::SYSTEM_BUSY));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedSystemBusyResponse);

			ASSERT_TRUE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		void SetBadCardReadTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedBadCardReadResponse;
			expectedBadCardReadResponse = R"({"NotificationText":"bad_card_read",)";
			expectedBadCardReadResponse += R"("NotificationTextArgs":[],)";
			expectedBadCardReadResponse += R"("NotificationTextPriority":"2",)";
			expectedBadCardReadResponse += R"("AlertText":"",)";
			expectedBadCardReadResponse += R"("AlertTextArgs":[],)";
			expectedBadCardReadResponse += R"("AlertTextPriority":"",)";
			expectedBadCardReadResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedBadCardReadResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.SetNotificationState(NotificationState::BAD_CARD_READ));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedBadCardReadResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::BAD_CARD_READ));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedBadCardReadResponse);

			ASSERT_TRUE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		void SetVirtualPlayerTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedVirtualPlayerResponse;
			expectedVirtualPlayerResponse = R"({"NotificationText":"virtual_session",)";
			expectedVirtualPlayerResponse += R"("NotificationTextArgs":[],)";
			expectedVirtualPlayerResponse += R"("NotificationTextPriority":"3",)";
			expectedVirtualPlayerResponse += R"("AlertText":"",)";
			expectedVirtualPlayerResponse += R"("AlertTextArgs":[],)";
			expectedVirtualPlayerResponse += R"("AlertTextPriority":"",)";
			expectedVirtualPlayerResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedVirtualPlayerResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.SetNotificationState(NotificationState::VIRTUAL_PLAYER));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedVirtualPlayerResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::VIRTUAL_PLAYER));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedVirtualPlayerResponse);

			ASSERT_TRUE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		void SetIneligiblePlayerTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedIneligiblePlayerResponse =
				"{\"NotificationText\":\"\",\"NotificationTextArgs\":[],"
				"\"NotificationTextPriority\":\"\",\"AlertText\":\"\","
				"\"AlertTextArgs\":[],\"AlertTextPriority\":\"\","
				"\"ShowAlertUntilNewNotification\":false,"
				"\"TextToggleIntervalInMilliSecs\":3000}";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			// below changed from TRUE to FALSE - text is blank now so it does not
			// change away from its default (also blank), so false is returned.
			ASSERT_FALSE(obj.SetNotificationState(NotificationState::INELIGIBLE_PLAYER));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIneligiblePlayerResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::INELIGIBLE_PLAYER));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIneligiblePlayerResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		void KeepNotificationBoxUpFront_Test()
		{
			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);

			// Perform operation(s) and test(s).
			obj.SetNotificationState(NotificationState::UNCARDED);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::SYSTEM_BUSY);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::BAD_CARD_READ);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::VIRTUAL_PLAYER);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::INELIGIBLE_PLAYER);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::PLAYER_IN_PLAY);
			ASSERT_FALSE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::GUEST_WELCOME);
			ASSERT_TRUE(obj.KeepNotificationBoxUpFront());

			obj.SetNotificationState(NotificationState::PLAYER_WELCOME);
			ASSERT_TRUE(obj.KeepNotificationBoxUpFront());
		}

		void SetMaxCardLimitExceededTest()
		{
			// Setup.
			string expectedIdleResponse;
			expectedIdleResponse = R"({"NotificationText":"",)";
			expectedIdleResponse += R"("NotificationTextArgs":[],)";
			expectedIdleResponse += R"("NotificationTextPriority":"",)";
			expectedIdleResponse += R"("AlertText":"",)";
			expectedIdleResponse += R"("AlertTextArgs":[],)";
			expectedIdleResponse += R"("AlertTextPriority":"",)";
			expectedIdleResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedIdleResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string expectedMaxCardInResponse;
			expectedMaxCardInResponse = R"({"NotificationText":"max_card_in_exceeded",)";
			expectedMaxCardInResponse += R"("NotificationTextArgs":[],)";
			expectedMaxCardInResponse += R"("NotificationTextPriority":"2",)";
			expectedMaxCardInResponse += R"("AlertText":"",)";
			expectedMaxCardInResponse += R"("AlertTextArgs":[],)";
			expectedMaxCardInResponse += R"("AlertTextPriority":"",)";
			expectedMaxCardInResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedMaxCardInResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedMaxCardInResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedMaxCardInResponse);

			ASSERT_TRUE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);

			ASSERT_FALSE(obj.SetNotificationState(NotificationState::IDLE));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedIdleResponse);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetNotificationState()
		///     CNotificationBox::IsPlayerPresent()
		/// Test setting with one parameter.
		/// </summary>
		void IsPlayerPresentTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.

			// Setup.
			CNotificationBox obj(operatingSystem);

			// Perform operation(s) and test(s).
			obj.SetNotificationState(NotificationState::UNCARDED);
			ASSERT_TRUE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::SYSTEM_BUSY);
			ASSERT_FALSE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::BAD_CARD_READ);
			ASSERT_FALSE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::VIRTUAL_PLAYER);
			ASSERT_FALSE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::INELIGIBLE_PLAYER);
			ASSERT_FALSE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED);
			ASSERT_FALSE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::GUEST_WELCOME);
			ASSERT_TRUE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::PLAYER_WELCOME);
			ASSERT_TRUE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState::PLAYER_IN_PLAY);
			ASSERT_TRUE(obj.IsPlayerPresent());

			obj.SetNotificationState(NotificationState(-1));
			ASSERT_FALSE(obj.IsPlayerPresent());
		}

		void AddAlertTest_MeterSnapshotInProgress()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"meter_snapshot_in_progress",)";
			expectedResponse += R"("AlertTextArgs":["40"],)";
			expectedResponse += R"("AlertTextPriority":"1",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS, 40));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_RebootInProgress()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"reboot_in_progress",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"1",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_REBOOT_IN_PROGRESS));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_RebootedMemoryError()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"rebooted_memory_error",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"1",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_REBOOTED_MEMORY_ERROR));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_RebootedMemoryOK()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"rebooted_memory_ok",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"1",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_REBOOTED_MEMORY_OK));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_ProgressiveDown()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"progressive_down",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"3",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_PROGRESSIVE_DOWN));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_ProgressiveIneligibility()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"progressive_ineligibility",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"3",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_PROGRESSIVE_INELIGIBILITY));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_SystemBusy()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"system_busy",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"3",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_SYSTEM_BUSY));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void AddAlertTest_SystemBusy_UncardedSession()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"insert_card",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"3",)";
			expectedResponse += R"("AlertText":"system_busy",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"3",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;
			obj.SetNotificationState(NotificationState::UNCARDED);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.AddAlert(Alerts::ALERT_SYSTEM_BUSY));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void RemoveAlertTest_1()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;
			obj.AddAlert(Alerts::ALERT_PROGRESSIVE_DOWN);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.RemoveAlert(Alerts::ALERT_PROGRESSIVE_DOWN));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void RemoveAlertTest_2()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"insert_card",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"3",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;
			obj.SetNotificationState(NotificationState::UNCARDED);
			obj.AddAlert(Alerts::ALERT_PROGRESSIVE_DOWN);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.RemoveAlert(Alerts::ALERT_PROGRESSIVE_DOWN));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void RemoveAlertTest_3()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"insert_card",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"3",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":3000})";
			string response;
			CNotificationBox obj;
			obj.SetNotificationState(NotificationState::UNCARDED);
			obj.AddAlert(Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS, 20);

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.RemoveAlert(Alerts::ALERT_METER_SNAPSHOT_IN_PROGRESS));
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void SetTextToggleIntervalTest_1()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":5000})";
			string response;
			CNotificationBox obj;

			// Perform operation(s) and test(s).
			obj.SetTextToggleInterval(5000);
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void SetTextToggleIntervalTest_2()
		{
			// Setup.
			string expectedResponse;
			expectedResponse = R"({"NotificationText":"insert_card",)";
			expectedResponse += R"("NotificationTextArgs":[],)";
			expectedResponse += R"("NotificationTextPriority":"3",)";
			expectedResponse += R"("AlertText":"",)";
			expectedResponse += R"("AlertTextArgs":[],)";
			expectedResponse += R"("AlertTextPriority":"",)";
			expectedResponse += R"("ShowAlertUntilNewNotification":false,)";
			expectedResponse += R"("TextToggleIntervalInMilliSecs":5000})";
			string response;
			CNotificationBox obj;
			obj.SetNotificationState(NotificationState::UNCARDED);

			// Perform operation(s) and test(s).
			obj.SetTextToggleInterval(5000);
			response = obj.GetSubscriptionDataResponse(TOPIC_NOTIFICATIONBOX, "");
			ASSERT_EQ(response, expectedResponse);
		}

		void Mobile_ResposeTest()
		{
			// Setup.
			string expectedResponse = "{\"IsMobileConnectEnabled\":false,\"State\":4,\"Code\":\"\"}";

			string response;

			// Perform operation(s) to be tested.
			CNotificationBox obj;
			response = obj.GetSubscriptionDataResponse(MOBILE_CONNECT_TOPIC, "");

			// Verify
			ASSERT_EQ(response, expectedResponse);
		}

		void CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_ok()
		{
			const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.

			COperatingSystemTest operatingSystem; // So we can control the tick count.

			operatingSystem.m_tickCount32 = 5000;

			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileLoadBalancingInProgress = true;
			obj.m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount() - MOBILE_LOAD_BALANCING_TIME_CHECK;
			obj.m_notificationState = NotificationState::IDLE;
			obj.m_advanceStateAfterGuestText = true;
			obj.m_startTickCount = 0;
			obj.m_guestWelcomeText.push_back("Test");
			CConfig config(true, "123");
			config.SetNotificationForPlayerTrackingTime(0);
			CUIConfig uiconfig(config, true);
			obj.SetUIConfig(uiconfig);
			obj.m_isSubscriptionDataSent = true;
			obj.AddAlert(ALERT_REBOOTED_MEMORY_OK);
			ASSERT_TRUE(obj.CheckIfNotificationTextHasChanged());
		}

		void CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_MemErr()
		{
			const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.

			COperatingSystemTest operatingSystem; // So we can control the tick count.

			operatingSystem.m_tickCount32 = 5000;

			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileLoadBalancingInProgress = true;
			obj.m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount() - MOBILE_LOAD_BALANCING_TIME_CHECK;
			obj.m_notificationState = NotificationState::IDLE;
			obj.m_advanceStateAfterGuestText = true;
			obj.m_startTickCount = 0;
			obj.m_guestWelcomeText.push_back("Test");
			CConfig config(true, "123");
			config.SetNotificationForPlayerTrackingTime(0);
			CUIConfig uiconfig(config, true);
			obj.SetUIConfig(uiconfig);
			obj.m_isSubscriptionDataSent = true;
			obj.AddAlert(ALERT_REBOOTED_MEMORY_ERROR);
			ASSERT_TRUE(obj.CheckIfNotificationTextHasChanged());
		}

		void CheckIfNotificationTextHasChangedTest_GuestWelcome()
		{
			const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.

			COperatingSystemTest operatingSystem; // So we can control the tick count.

			operatingSystem.m_tickCount32 = 5000;

			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileLoadBalancingInProgress = true;
			obj.m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount() - MOBILE_LOAD_BALANCING_TIME_CHECK;
			obj.m_notificationState = NotificationState::GUEST_WELCOME;
			obj.m_advanceStateAfterGuestText = true;
			obj.m_startTickCount = 0;
			obj.m_guestWelcomeText.push_back("Test");
			CConfig config(true, "123");
			config.SetNotificationForPlayerTrackingTime(0);
			CUIConfig uiconfig(config, true);
			obj.SetUIConfig(uiconfig);
			obj.AddAlert(ALERT_REBOOTED_MEMORY_OK);

			ASSERT_FALSE(obj.CheckIfNotificationTextHasChanged());
		}

		void CheckIfNotificationTextHasChangedTest_PlayerWelcome()
		{
			const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.

			COperatingSystemTest operatingSystem; // So we can control the tick count.

			operatingSystem.m_tickCount32 = 5000;

			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileLoadBalancingInProgress = true;
			obj.m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount() - MOBILE_LOAD_BALANCING_TIME_CHECK;
			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_advanceStateAfterGuestText = true;
			obj.m_startTickCount = 10;
			obj.m_playerWelcomeText.push_back("Test");
			CConfig config(true, "123");
			//config.SetNotificationForPlayerTrackingTime(0);
			CUIConfig uiconfig(config, true);
			obj.SetUIConfig(uiconfig);
			obj.AddAlert(ALERT_REBOOTED_MEMORY_ERROR);

			ASSERT_FALSE(obj.CheckIfNotificationTextHasChanged());
		}

		void CheckIfNotificationTextHasChangedTest_IneligablePlayer()
		{
			const DWORD MOBILE_LOAD_BALANCING_TIME_CHECK = 2500;//In milliseconds.

			COperatingSystemTest operatingSystem; // So we can control the tick count.

			operatingSystem.m_tickCount32 = 100;

			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileLoadBalancingInProgress = true;
			obj.m_mobileLoadBalancingInProgressStarted = CUtilities::GetTickCount() - MOBILE_LOAD_BALANCING_TIME_CHECK;
			obj.m_notificationState = NotificationState::INELIGIBLE_PLAYER;
			obj.m_advanceStateAfterGuestText = true;
			obj.m_startTickCount = 0;
			CConfig config(true, "123");
			config.SetNotificationForPlayerTrackingTime(0);
			CUIConfig uiconfig(config, true);
			obj.SetUIConfig(uiconfig);
			obj.SetRFID(true);
			obj.SetEmployeeCardFlag(false);
			obj.m_notificationStartTickCount = 10;
			obj.AddAlert(ALERT_SYSTEM_BUSY);
			obj.m_isSubscriptionDataSent = true;

			ASSERT_FALSE(obj.CheckIfNotificationTextHasChanged());
		}

		void SetNotificationStateByScreenIdTest_INT_MSG_BAD_CARD_READ()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			ASSERT_TRUE(obj.SetNotificationStateByScreenId(INT_MSG_BAD_CARD_READ, false));
		}

		void SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_BANNED_PLAYER()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			// banned player status is now blank, so false is returned here
			ASSERT_FALSE(obj.SetNotificationStateByScreenId(INT_MSG_DISPLAY_BANNED_PLAYER, false));
		}

		void SetNotificationStateByScreenIdTest_INT_MSG_CARDED_SESSION_IN_PROGRESS()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			ASSERT_TRUE(obj.SetNotificationStateByScreenId(INT_MSG_CARDED_SESSION_IN_PROGRESS, false));
		}

		void SetNotificationStateByScreenIdTest_INT_MSG_DOWNLOAD_CANCELED()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			ASSERT_FALSE(obj.SetNotificationStateByScreenId(INT_MSG_DOWNLOAD_CANCELED, false));
		}

		void SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_DOWNLOAD_PENDING()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			ASSERT_TRUE(obj.SetNotificationStateByScreenId(INT_MSG_DISPLAY_DOWNLOAD_PENDING, false));
		}

		void SetGuestWelcomeTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			std::vector<std::string> guestWelcomeText;
			guestWelcomeText.push_back("one");
			guestWelcomeText.push_back("two");
			guestWelcomeText.push_back("three");
			std::vector<std::string> inPlayTextArgs;
			inPlayTextArgs.push_back("1");
			inPlayTextArgs.push_back("2");
			inPlayTextArgs.push_back("3");

			ASSERT_TRUE(obj.SetGuestWelcome(guestWelcomeText, true, "In Play", inPlayTextArgs));
		}

		void SetPlayerWelcomeTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			std::vector<std::string> playerWelcomeText;
			playerWelcomeText.push_back("one");
			playerWelcomeText.push_back("two");
			playerWelcomeText.push_back("three");

			std::vector<std::string> inPlayTextArgs;
			inPlayTextArgs.push_back("1");
			inPlayTextArgs.push_back("2");
			inPlayTextArgs.push_back("3");

			std::vector<std::vector<std::string>> playerWelcomeArgs;
			std::vector<std::string> playerWelcomeArg;
			playerWelcomeArg.push_back("one");
			playerWelcomeArgs.push_back(playerWelcomeArg);

			ASSERT_TRUE(obj.SetPlayerWelcome(playerWelcomeText, playerWelcomeArgs, "In Play", inPlayTextArgs));
		}

		void SetInPlayTextTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			std::vector<std::string> inPlayTextArgs;
			inPlayTextArgs.push_back("1");
			inPlayTextArgs.push_back("2");
			inPlayTextArgs.push_back("3");

			std::vector<std::vector<std::string>> playerWelcomeArgs;
			std::vector<std::string> playerWelcomeArg;
			playerWelcomeArg.push_back("one");
			playerWelcomeArgs.push_back(playerWelcomeArg);

			ASSERT_FALSE(obj.SetInPlayText("In Play", inPlayTextArgs));
		}

		void Set4FieldPlayerTextTest_IPDT_TOTAL_SESSION()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(true);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(true);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		}

		void Set4FieldPlayerTextTest_IPDT_TOTALCASH_SESSIONCASH()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(true);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(true);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		};

		void Set4FieldPlayerTextTest_IPDT_COUNTDOWN_SESSION()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(true);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(true);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		};

		void Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSION()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(true);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(true);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		};

		void Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSION_noPlayerName()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(false);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(true);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		};

		void Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSIONPTS()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CPlayer player;
			CConfig config(true, "1.2.3");

			config.SetDisplayPlayerName(true);
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(false);
			config.SetDisplayGoodLuckAndSession(false);
			config.SetDisplayTotalAndSessionPtsNum(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(false);
			config.SetDisplayGoodLuckAndSessionPts(true);

			CEGMConfig egmConfig(MEMORY_NONE);
			player.ConfigSettings(config, egmConfig);

			CPlayerInfo playerInfo;
			playerInfo.SetPointsAndCountdown(100, 0);
			player.SetPlayerInfo(&playerInfo);

			obj.m_notificationState = NotificationState::PLAYER_WELCOME;
			obj.m_playerWelcomeText.push_back("Welcome Player");
			std::vector<std::string> args;
			args.push_back("Arg");
			obj.m_playerWelcomeTextArgs.push_back(args);

			ASSERT_TRUE(obj.Set4FieldPlayerText(&player));
			ASSERT_EQ(obj.m_text, "Welcome Player");

			player.SetPlayerInfo(nullptr);
		};


		void SetMobileEnableFlagTest_true()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			obj.SetMobileEnableFlag(true);

			ASSERT_TRUE(obj.IsMobileEnabled());
		}

		void SetMobileEnableFlagTest_false()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);
			obj.m_mobileEnabled = true;

			obj.SetMobileEnableFlag(false);

			ASSERT_FALSE(obj.IsMobileEnabled());
		}

		void WaitingForMobileConnectionTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			obj.SetMobileEnableFlag(false);

			ASSERT_FALSE(obj.WaitingForMobileConnection());
		}

		void GetTextTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CNotificationTextConfig notificationTextConfig;
			obj.m_text = "TEST";

			ASSERT_EQ(obj.GetText(), "TEST");

		}

		void GetNotificationStateTest()
		{
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			// Setup.
			CNotificationBox obj(operatingSystem);

			CNotificationTextConfig notificationTextConfig;
			obj.m_notificationState = NotificationState::INELIGIBLE_PLAYER;

			ASSERT_EQ(obj.GetNotificationState(), NotificationState::INELIGIBLE_PLAYER);

		}
	};

	const string NotificationBoxUnitTests::TOPIC_NOTIFICATIONBOX = "NotificationBox";
	const string NotificationBoxUnitTests::MOBILE_CONNECT_TOPIC = "MobileConnectSubscription";

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::GetText()
    /// </summary>
    TEST_F(NotificationBoxUnitTests, GetText_DefaultTest)
    {
        GetText_DefaultTest();
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::SetDownloadPending()
    /// </summary>
    TEST_F(NotificationBoxUnitTests, SetDownloadPendingTest)
    {
        SetDownloadPendingTest();
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::SetNotificationState(SYSTEM_BUSY)
    /// </summary>
    TEST_F(NotificationBoxUnitTests, SetSystemBusyTest)
    {
       SetSystemBusyTest();
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::SetNotificationState(BAD_CARD_READ)
    /// </summary>
    TEST_F(NotificationBoxUnitTests, SetBadCardReadTest)
    {
        SetBadCardReadTest();
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::SetNotificationState(VIRTUAL_PLAYER)
    /// </summary>
    TEST_F(NotificationBoxUnitTests, SetVirtualPlayerTest)
    {
        SetVirtualPlayerTest();
    }

    /// <summary>
    /// Code coverage and functional test for:
    ///     CNotificationBox::CNotificationBox()
    ///     CNotificationBox::SetNotificationState(INELIGIBLE_PLAYER)
    /// </summary>
    TEST_F(NotificationBoxUnitTests, SetIneligiblePlayerTest)
    {
        SetIneligiblePlayerTest();
    }

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	///     CNotificationBox::SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED)
	///     CNotificationBox::KeepNotificationBoxUpFront()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, KeepNotificationBoxUpFront_Test)
	{
		KeepNotificationBoxUpFront_Test();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	///     CNotificationBox::SetNotificationState(NotificationState::MAX_CARD_LIMIT_EXCEEDED)
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetMaxCardLimitExceededTest)
	{
		SetMaxCardLimitExceededTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetNotificationState()
	///     CNotificationBox::IsPlayerPresent()
	/// Test setting with one parameter.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, IsPlayerPresentTest)
	{
		IsPlayerPresentTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Mobile_ResposeTest)
	{
		Mobile_ResposeTest();
	}

	

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_ok)
	{
		CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_ok();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_MemErr)
	{
		CheckIfNotificationTextHasChangedTest_Idle_RebootAlert_MemErr();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, CheckIfNotificationTextHasChangedTest_GuestWelcome)
	{
		CheckIfNotificationTextHasChangedTest_GuestWelcome(); 
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, CheckIfNotificationTextHasChangedTest_PlayerWelcome)
	{
		CheckIfNotificationTextHasChangedTest_PlayerWelcome();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, CheckIfNotificationTextHasChangedTest_IneligablePlayer)
	{
		CheckIfNotificationTextHasChangedTest_IneligablePlayer();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest_INT_MSG_BAD_CARD_READ)
	{
		SetNotificationStateByScreenIdTest_INT_MSG_BAD_CARD_READ();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_BANNED_PLAYER)
	{
		SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_BANNED_PLAYER();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest_INT_MSG_CARDED_SESSION_IN_PROGRESS)
	{
		SetNotificationStateByScreenIdTest_INT_MSG_CARDED_SESSION_IN_PROGRESS();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest_INT_MSG_DOWNLOAD_CANCELED)
	{
		SetNotificationStateByScreenIdTest_INT_MSG_DOWNLOAD_CANCELED();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_DOWNLOAD_PENDING)
	{
		SetNotificationStateByScreenIdTest_INT_MSG_DISPLAY_DOWNLOAD_PENDING();
	}
	
	/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::CNotificationBox()
		/// </summary>
	TEST_F(NotificationBoxUnitTests, SetGuestWelcomeTest)
	{
		SetGuestWelcomeTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetPlayerWelcomeTest)
	{
		SetPlayerWelcomeTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetInPlayTextTest)
	{
		SetInPlayTextTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSIONPTS)
	{
		Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSIONPTS();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSION)
	{
		Set4FieldPlayerTextTest_IPDT_GOODLUCK_SESSION();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Set4FieldPlayerTextTest_IPDT_COUNTDOWN_SESSION)
	{
		Set4FieldPlayerTextTest_IPDT_COUNTDOWN_SESSION();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Set4FieldPlayerTextTest_IPDT_TOTALCASH_SESSIONCASH)
	{
		Set4FieldPlayerTextTest_IPDT_TOTALCASH_SESSIONCASH();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, Set4FieldPlayerTextTest_IPDT_TOTAL_SESSION)
	{
		Set4FieldPlayerTextTest_IPDT_TOTAL_SESSION();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetMobileEnableFlagTest_false)
	{
		SetMobileEnableFlagTest_false();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetMobileEnableFlagTest_true)
	{
		SetMobileEnableFlagTest_true();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, WaitingForMobileConnectionTest)
	{
		WaitingForMobileConnectionTest();
	}
	
	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, GetTextTest)
	{
		GetTextTest();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::CNotificationBox()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, GetNotificationStateTest)
	{
		GetNotificationStateTest();
	}
	
	
#if 000  // TODO 
	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetPlayerTrackingDuration()
	/// Test setting a range of values.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetPlayerTrackingTextDataTest)
	{
		// Data.
		const stringArray playerWelcomeText; // Empty.

		CXmlReaderTest testReader(CUnitTestingXmlData::GetStringData(), &m_testIndex);
		auto resultData = [&](LPCTSTR data) { return string(data != NULL ? data : _T("")); };

		// Setup.  Use the same object for each test.
		COperatingSystemTest operatingSystemA; // So we can control the tick count.
		CNotificationBox objA(operatingSystemA);
		objA.SetPlayerWelcome(playerWelcomeText, NULL);

		for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
		{
			// Get the test data.
			string testData(testReader.GetNextAttributeAsString(CUnitTestingXmlData::TestData_ValueName));

			// Perform operation(s) and test(s).
			objA.SetInPlayText(testData, testData);
			AssertResult2(objA.GetText(), resultData(testData));

			// Setup.  Use a different object for each test sequence.
			COperatingSystemTest operatingSystemB; // So we can control the tick count.
			CNotificationBox objB(operatingSystemB);
			objB.SetPlayerWelcome(playerWelcomeText, NULL);

			// Perform operation(s) and test(s).
			objB.SetInPlayText(testData, testData);
			AssertResult2(objB.GetText(), resultData(testData));
		}

		Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
		testReader.AssertEof(); // Assert that we read all of the data.
	}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlaySingleLineText()
		/// Test setting a range of values.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetInPlaySingleLineTextDataTest)
		{
			// Data.
			string inPlayText1(_T("1a"));
			string inPlayText2(_T("22a"));
			vector<string> playerWelcomeText;

			CXmlReaderTest testReader(CUnitTestingXmlData::GetStringData(), &m_testIndex);
			auto resultData = [](LPCTSTR data, LPCTSTR dataA, LPCTSTR dataB) { return string(data != NULL && *data != 0 ? dataA : dataB); };
			auto resultData1 = [&](LPCTSTR data, LPCTSTR dataB) { return resultData(data, data, dataB); };

			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystemA; // So we can control the tick count.
			CNotificationBox objA(operatingSystemA);
			objA.SetPlayerWelcome(playerWelcomeText, NULL);
			objA.SetInPlayText(inPlayText1, inPlayText2);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				string testData(testReader.GetNextAttributeAsString(CUnitTestingXmlData::TestData_ValueName));

				// Perform operation(s) and test(s).
				objA.SetInPlaySingleLineText(testData);
				AssertResult2(objA.GetText(), resultData1(testData, inPlayText1));

				// Setup.  Use a different object for each test sequence.
				COperatingSystemTest operatingSystemB; // So we can control the tick count.
				CNotificationBox objB(operatingSystemB);
				objB.SetPlayerWelcome(playerWelcomeText, NULL);
				objB.SetInPlayText(inPlayText1, inPlayText2);

				// Perform operation(s) and test(s).
				objB.SetInPlaySingleLineText(testData);
				AssertResult2(objB.GetText(), resultData1(testData, inPlayText1));
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}



		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetUIConfig()
		/// Test setting a range of values for the promotion duration.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetPromotionDurationTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(), &m_testIndex);
			WORD defaultResult(CConfigExtData::DEFAULT_NOTIFICATION_SHOW_PROMOTION);
			WORD minimumResult(CConfigExtData::MINIMUM_NOTIFICATION_SHOW_PROMOTION);
			WORD maximumResult(CConfigExtData::MAXIMUM_NOTIFICATION_SHOW_PROMOTION);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			// Setup.  Use the same object for each test.
			CNotificationBox objA;

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				CConfig config;
				const CEGMConfig egmConfig(MEMORY_NONE);
				config.SetNotificationForShowPromotionTime(testData);
				CUIConfig taskBarConfig(config);

				// Perform operation(s) and test(s).
				objA.SetUIConfig(taskBarConfig);
				AssertResult2(objA.m_taskBarConfig.GetNotificationForShowPromotionTimeInSeconds(), expectedResult(testData));
				AssertResult2(objA.m_taskBarConfig.GetNotificationForShowPromotionTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);

				// Setup.  Use a different object for each test sequence.
				CNotificationBox objB;
				objB.SetUIConfig(taskBarConfig);
				AssertResult2(objB.m_taskBarConfig.GetNotificationForShowPromotionTimeInSeconds(), expectedResult(testData));
				AssertResult2(objB.m_taskBarConfig.GetNotificationForShowPromotionTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}



		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetUIConfig()
		/// Test setting a range of values for the player tracking duration.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetPlayerTrackingDurationTest)
		{
			// Data.
			CXmlReaderTest testReader(CUnitTestingXmlData::GetWordData(), &m_testIndex);
			WORD defaultResult(CConfigExtData::DEFAULT_NOTIFICATION_PLAYER_TRACKING);
			WORD minimumResult(CConfigExtData::MINIMUM_NOTIFICATION_PLAYER_TRACKING);
			WORD maximumResult(CConfigExtData::MAXIMUM_NOTIFICATION_PLAYER_TRACKING);
			auto expectedResult = [=](WORD data) { return minimumResult <= data && data <= maximumResult ? data : defaultResult; };

			// Setup.  Use the same object for each test.
			CNotificationBox objA;

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				WORD testData(testReader.GetNextAttributeAsWORD(CUnitTestingXmlData::TestData_ValueName));

				CConfig config;
				config.SetNotificationForPlayerTrackingTime(testData);
				CUIConfig taskBarConfig(config);

				// Perform operation(s) and test(s).
				objA.SetUIConfig(taskBarConfig);
				AssertResult2(objA.m_taskBarConfig.GetNotificationForPlayerTrackingTimeInSeconds(), expectedResult(testData));
				AssertResult2(objA.m_taskBarConfig.GetNotificationForPlayerTrackingTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);

				// Setup.  Use a different object for each test sequence.
				CNotificationBox objB;
				objB.SetUIConfig(taskBarConfig);
				AssertResult2(objB.m_taskBarConfig.GetNotificationForPlayerTrackingTimeInSeconds(), expectedResult(testData));
				AssertResult2(objB.m_taskBarConfig.GetNotificationForPlayerTrackingTimeInMilliseconds(), expectedResult(testData) * (DWORD)1000);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetUIConfig()
		///     CNotificationBox::AddPlayerMessage()
		///     CNotificationBox::CheckTimedEvents(interactiveButtonExpired, msgType)
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetPlayerTrackingDuration1_AddPlayerMessageTest)
		{
			// Data.
			vector<string> playerWelcomeText;
			string inPlaySingleLineText(_T("ss11tttt"));
			string playerMessage(_T("Congratulations, player!"));
			const DWORD playerTrackingDuration = 2 * m_playerTrackingDurationExpected;
			bool interactiveButtonExpired;
			InternalMsgType msgType;
			CInternalMessage* msg(NULL);

			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			CConfig config;
			config.SetNotificationForPlayerTrackingTime((WORD)(playerTrackingDuration / 1000));
			CUIConfig taskBarConfig(config);
			obj.SetUIConfig(taskBarConfig);
			obj.SetPlayerWelcome(playerWelcomeText, inPlaySingleLineText);
			AssertResult2(obj.GetText(), inPlaySingleLineText);

			// Perform operation(s) and test(s).
			operatingSystem.m_tickCount32 = 0;
			obj.AddPlayerMessage(playerMessage);
			AssertResult2(obj.GetText(), playerMessage);
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage);

			operatingSystem.m_tickCount32 = playerTrackingDuration - 1;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage);

			operatingSystem.m_tickCount32 = playerTrackingDuration;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
			AssertResult2(obj.GetText(), inPlaySingleLineText);

			operatingSystem.m_tickCount32 = 3 * playerTrackingDuration;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), inPlaySingleLineText);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetUIConfig()
		///     CNotificationBox::AddPlayerMessage()
		///     CNotificationBox::CheckTimedEvents(interactiveButtonExpired, msgType, msg)
		/// Test expiration with a range of tick count values.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetPlayerTrackingDurationData_AddPlayerMessageTest)
		{
			// Data.
			vector<string> playerWelcomeText;
			string inPlaySingleLineText(_T("ss11tttt"));
			string playerMessage(_T("Congratulations, player!"));
			const DWORD playerTrackingDuration = 2 * m_playerTrackingDurationExpected;
			CXmlReaderTest testReader(CUnitTestingXmlData::GetDwordData(), &m_testIndex);
			bool interactiveButtonExpired;
			InternalMsgType msgType;
			CInternalMessage* msg(NULL);

			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			CConfig config;
			config.SetNotificationForPlayerTrackingTime((WORD)(playerTrackingDuration / 1000));
			CUIConfig taskBarConfig(config);
			obj.SetUIConfig(taskBarConfig);
			obj.SetPlayerWelcome(playerWelcomeText, inPlaySingleLineText);
			AssertResult2(obj.GetText(), inPlaySingleLineText);

			for (; testReader.MoveToNextElement(CUnitTestingXmlData::TestData_DataElementName); m_testIndex++)
			{
				// Get the test data.
				DWORD testData(testReader.GetNextAttributeAsDWORD(CUnitTestingXmlData::TestData_ValueName));

				// Perform operation(s) and test(s).
				operatingSystem.m_tickCount32 = testData;
				bool added(false);
				obj.AddPlayerMessage(playerMessage);
				AssertResult2(obj.GetText(), playerMessage);
				AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
				AssertResult2(obj.GetText(), playerMessage);

				operatingSystem.m_tickCount32 = testData + playerTrackingDuration - 1;
				AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
				AssertResult2(obj.GetText(), playerMessage);

				operatingSystem.m_tickCount32 = testData + playerTrackingDuration;
				AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
				AssertResult2(obj.GetText(), inPlaySingleLineText);

				operatingSystem.m_tickCount32 = testData + 3 * playerTrackingDuration;
				AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
				AssertResult2(obj.GetText(), inPlaySingleLineText);
			}

			Assert::IsTrue(m_testIndex > 0, _T("m_testIndex > 0")); // Assert that the data contained at least one test.
			testReader.AssertEof(); // Assert that we read all of the data.
		}
        	
		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::CNotificationBox()
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with default text state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_DefaultNullTest)
		{
			// Setup.
			CNotificationBox obj;

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(NULL);

			// Test(s).
			AssertResult2(obj.GetText(), m_idleTextExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetDownloadPending()
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with download pending text state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_DownloadPendingNullTest)
		{
			// Setup.
			CNotificationBox obj;
			obj.SetDownloadPending(true);
			AssertResult2(obj.GetText(), m_downloadPendingTextExpected);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(NULL);

			// Test(s).
			AssertResult2(obj.GetText(), m_downloadPendingTextExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_PlayerTextNullTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(NULL);

			// Test(s).
			AssertResult2(obj.GetText(), inPlayText1a);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_DefaultPlayerTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			CPlayer player;

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), m_inPlayTextExpected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// Test with CPlayer::GetInPlayDisplayType() == IPDT_TOTAL_SESSION.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_TotalSessionTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;
			const string text1Expected(_T("0 total points earned."));
			const string text2Expected(_T("0 points earned this session"));

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Get a default config object and set as needed.
			CUnitTestingUtils::SetNvramInvalid();
			CConfig config;
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayTotalAndSessionPtsNum(true);

			CPlayer player(config, egmConfig);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), text1Expected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// Test with CPlayer::GetInPlayDisplayType() == IPDT_TOTALCASH_SESSIONCASH.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_TotalCashSessionCashTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;
			const string text1Expected(_T("$0.00 total points earned."));
			const string text2Expected(_T("$0.00 earned this session."));

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Get a default config object and set as needed.
			CUnitTestingUtils::SetNvramInvalid();
			CConfig config;
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayTotalPtsSessionPtsAsCash(true);

			CPlayer player(config, egmConfig);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), text1Expected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// Test with CPlayer::GetInPlayDisplayType() == IPDT_COUNTDOWN_SESSION.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_CountdownSessionTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;
			const DWORD currentCountdown(100);
			const string text1Expected(_T("100 countdown to earn next point"));
			const string text2Expected(_T("0 points earned this session"));

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Get a default config object and set as needed.
			CUnitTestingUtils::SetNvramInvalid();
			CConfig config;
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayCountdownAndSession(true);

			CPlayer player(config, egmConfig);
			player.ResetSession(0); // Set session points to 0.
			player.SetCurrentCountdown(currentCountdown); // Override value obtained from NVRAM via CPlayer::BuildYourself().

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), text1Expected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// Test with CPlayer::GetInPlayDisplayType() == IPDT_GOODLUCK_SESSION.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_GoodLuckSessionTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;
			const string text1Expected(_T("Good Luck!"));
			const string text2Expected(_T("$0.00 earned this session."));

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Get a default config object and set as needed.
			CUnitTestingUtils::SetNvramInvalid();
			CConfig config;
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayGoodLuckAndSession(true);

			CPlayer player(config, egmConfig);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), text1Expected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetInPlayText(inPlayText1, inPlayText2)
		///     CNotificationBox::Set4FieldPlayerText()
		///     CNotificationBox::GetText()
		/// Test with player tracking already in play text state.
		/// Test with CPlayer::GetInPlayDisplayType() == IPDT_GOODLUCK_SESSIONPTS.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, Set4FieldPlayerText_GoodLuckSessionPointsTest)
		{
			// Data.
			const string inPlayText1a(_T("1a"));
			const string inPlayText2a(_T("22a"));
			vector<string> playerWelcomeText;
			const string text1Expected(_T("Good Luck!"));
			const string text2Expected(_T("0 points earned this session"));

			// Setup.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);
			obj.SetPlayerWelcome(playerWelcomeText, NULL);
			obj.SetInPlayText(inPlayText1a, inPlayText2a);
			AssertResult2(obj.GetText(), inPlayText1a);

			// Get a default config object and set as needed.
			CUnitTestingUtils::SetNvramInvalid();
			CConfig config;
			const CEGMConfig egmConfig(MEMORY_NONE);
			config.BuildYourself();
			config.SetDisplayPlayerPtsTotal(false);
			config.SetDisplayGoodLuckAndSessionPts(true);

			CPlayer player(config, egmConfig);

			// Perform operation(s) to be tested.
			obj.Set4FieldPlayerText(&player);

			// Test(s).
			AssertResult2(obj.GetText(), text1Expected);
		}

		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetGuestWelcome()
		///     CNotificationBox::CheckTimedEvents(interactiveButtonExpired, msgType, msg)
		/// Add two different player messages (at the same time) while in the PLAYER_IN_PLAY state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetGuestWelcomeTest)
		{
			// Data.
			vector<string> playerWelcomeText;
			string inPlaySingleLineText(_T("ss11tttt"));
			string playerMessage1(_T("Good card"));
			string playerMessage2(_T("Hahaha"));
			bool interactiveButtonExpired;
			InternalMsgType msgType;
			CInternalMessage* msg(NULL);

			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);

			stringArray textArray;
			textArray.Add(playerMessage1);
			textArray.Add(playerMessage2);

			AssertResult2(obj.SetGuestWelcome(textArray, true, inPlaySingleLineText), true);
			AssertResult2(obj.GetText(), playerMessage1);

			// Perform operation(s) and test(s).
			operatingSystem.m_tickCount32 = 0;
			bool added(false);
			operatingSystem.m_tickCount32 = m_playerTrackingDurationExpected - 1;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage1);

			operatingSystem.m_tickCount32 = m_playerTrackingDurationExpected;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
			AssertResult2(obj.GetText(), playerMessage2);

			operatingSystem.m_tickCount32 = 2 * m_playerTrackingDurationExpected - 1;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage2);

			operatingSystem.m_tickCount32 = 2 * m_playerTrackingDurationExpected;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
			AssertResult2(obj.GetText(), inPlaySingleLineText);
		}
		
		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetPlayerWelcome()
		///     CNotificationBox::CheckTimedEvents(interactiveButtonExpired, msgType, msg)
		/// Add two different player messages (at the same time) while in the PLAYER_IN_PLAY state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetPlayerWelcomeTest)
		{
			// Data.
			vector<string> playerWelcomeText;
			string inPlaySingleLineText(_T("ss11tttt"));
			string playerMessage1(_T("Welcome"));
			string playerMessage2(_T("How are you?"));
			bool interactiveButtonExpired;
			InternalMsgType msgType;
			CInternalMessage* msg(NULL);

			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);

			stringArray textArray;
			textArray.Add(playerMessage1);
			textArray.Add(playerMessage2);

			AssertResult2(obj.SetPlayerWelcome(textArray, inPlaySingleLineText), true);
			AssertResult2(obj.GetText(), playerMessage1);

			// Perform operation(s) and test(s).
			operatingSystem.m_tickCount32 = 0;
			bool added(false);
			operatingSystem.m_tickCount32 = m_playerTrackingDurationExpected - 1;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage1);

			operatingSystem.m_tickCount32 = m_playerTrackingDurationExpected;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
			AssertResult2(obj.GetText(), playerMessage2);

			operatingSystem.m_tickCount32 = 2 * m_playerTrackingDurationExpected - 1;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), m_checkTimedEventsExpected);
			AssertResult2(obj.GetText(), playerMessage2);

			operatingSystem.m_tickCount32 = 2 * m_playerTrackingDurationExpected;
			AssertResult2(obj.CheckTimedEvents(interactiveButtonExpired, msgType, msg), true);
			AssertResult2(obj.GetText(), inPlaySingleLineText);
		}

		
		/// <summary>
		/// Code coverage and functional test for:
		///     CNotificationBox::SetNotificationStateByScreenId()
		///     CNotificationBox::CheckTimedEvents(interactiveButtonExpired, msgType, msg)
		/// Add two different player messages (at the same time) while in the PLAYER_IN_PLAY state.
		/// </summary>
		TEST_F(NotificationBoxUnitTests, SetNotificationStateByScreenIdTest)
		{
			// Setup.  Use the same object for each test.
			COperatingSystemTest operatingSystem; // So we can control the tick count.
			CNotificationBox obj(operatingSystem);			

			// Perform operation(s) and test(s).
			AssertResult2(obj.SetNotificationStateByScreenId(screenPlayerBadCard), false);
			AssertResult2(obj.SetNotificationStateByScreenId(screenBannedFromPlayMessage), false);
			AssertResult2(obj.SetNotificationStateByScreenId(screenPlayerCardedSessionInProgress), false);
		}
#endif //TODO-PORT

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_MeterSnapshotInProgress)
	{
		AddAlertTest_MeterSnapshotInProgress();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_RebootInProgress)
	{
		AddAlertTest_RebootInProgress();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_RebootedMemoryError)
	{
		AddAlertTest_RebootedMemoryError();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_RebootedMemoryOK)
	{
		AddAlertTest_RebootedMemoryOK();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_ProgressiveDown)
	{
		AddAlertTest_ProgressiveDown();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_ProgressiveIneligibility)
	{
		AddAlertTest_ProgressiveIneligibility();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_SystemBusy)
	{
		AddAlertTest_SystemBusy();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetNotificationState()
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// Testing for system busy alert along with uncarded session.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, AddAlertTest_SystemBusy_UncardedSession)
	{
		AddAlertTest_SystemBusy_UncardedSession();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::RemoveAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// Testing for progressive down alert.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, RemoveAlertTest_1)
	{
		RemoveAlertTest_1();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetNotificationState()
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::RemoveAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// Testing for progressive down alert along with uncarded session.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, RemoveAlertTest_2)
	{
		RemoveAlertTest_2();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetNotificationState()
	///     CNotificationBox::AddAlert()
	///     CNotificationBox::RemoveAlert()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// Testing for meter snapshot in progress alert along with uncarded session.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, RemoveAlertTest_3)
	{
		RemoveAlertTest_3();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetTextToggleInterval()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetTextToggleIntervalTest_1)
	{
		SetTextToggleIntervalTest_1();
	}

	/// <summary>
	/// Code coverage and functional test for:
	///     CNotificationBox::SetNotificationState()
	///     CNotificationBox::SetTextToggleInterval()
	///     CNotificationBox::GetSubscriptionDataResponse()
	/// Testing for setting text toggle interval along with uncarded session.
	/// </summary>
	TEST_F(NotificationBoxUnitTests, SetTextToggleIntervalTest_2)
	{
		SetTextToggleIntervalTest_2();
	}
}
