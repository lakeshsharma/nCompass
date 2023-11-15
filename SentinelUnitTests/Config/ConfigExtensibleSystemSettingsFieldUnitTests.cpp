
using namespace std;

#include "gtest/gtest.h"
#include "stdafx.h"
#include "../Sentinel/Utilities.h"
#include "Config/ConfigExtensibleSystemSettingsField.h"

// Unit testing includes.
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class ConfigExtensibleSystemSettingsFieldUnitTests : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
	}

	// Creates buffer for CConfigExtensibleSystemSettingsField with the given key-value pair.
	BYTE* CreateBuffer(const char key[], const char value[], int &bufferLength)
	{
		const WORD totalKeyCount = 1;

		bufferLength = sizeof(totalKeyCount) + 1 + strlen(key) + 1 + strlen(value);
		BYTE *buffer = new BYTE[bufferLength]();
		BYTE *ptr = buffer;

		// write TotalKeyCount
		*ptr = totalKeyCount;

		// write KeyLength
		ptr += sizeof(totalKeyCount);
		*ptr = (BYTE)strlen(key);

		// write Key
		ptr += sizeof(BYTE);
		memcpy(ptr, key, strlen(key));

		// write ValueLength
		ptr += strlen(key);
		*ptr = (BYTE)strlen(value);

		// write Value
		ptr += sizeof(BYTE);
		memcpy(ptr, value, strlen(value));

		return buffer;
	}

public:
	void AutoUploadCredits_Enabled()
	{
		//setup
		char key[] = "AutoUploadCredits";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isAutoUploadCredits = obj.AutoUploadCredits();
		ASSERT_TRUE(isAutoUploadCredits) << "AutoUploadCredits is not enabled.";

		// cleanup
		delete buffer;
	}

	void AutoUploadCredits_Disabled()
	{
		//setup
		char key[] = "AutoUploadCredits";
		char value[] = "no";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isAutoUploadCredits = obj.AutoUploadCredits();
		ASSERT_FALSE(isAutoUploadCredits) << "AutoUploadCredits is not disabled.";

		// cleanup
		delete buffer;
	}

	void CashoutToHost_Enabled()
	{
		//setup
		char key[] = "CashoutToPBT";
		char value[] = "true";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isCashoutToHost = obj.CashoutToHost();
		ASSERT_TRUE(isCashoutToHost) << "CashoutToHost is not enabled.";

		// cleanup
		delete buffer;
	}

	void CashoutToHost_Disabled()
	{
		//setup
		char key[] = "CashoutToPBT";
		char value[] = "false";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isCashoutToHost = obj.CashoutToHost();
		ASSERT_FALSE(isCashoutToHost) << "CashoutToHost is not disabled.";

		// cleanup
		delete buffer;
	}

	void UTJTaxWithhold_Enabled()
	{
		//setup
		char key[] = "UTJTaxWithhold";
		char value[] = "1";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isUTJTaxWithhold = obj.UJPTaxWithholding();
		ASSERT_TRUE(isUTJTaxWithhold) << "UTJTaxWithhold is not enabled.";

		// cleanup
		delete buffer;
	}

	void UTJTaxWithhold_Disabled()
	{
		//setup
		char key[] = "UTJTaxWithhold";
		char value[] = "0";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isUTJTaxWithhold = obj.UJPTaxWithholding();
		ASSERT_FALSE(isUTJTaxWithhold) << "UTJTaxWithhold is not disabled.";

		// cleanup
		delete buffer;
	}

	void FedTaxWithholdPercent()
	{
		//setup
		char key[] = "FedTaxWithholdPercent";
		char value[] = "7.5";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		float FedTaxWithholdPercent = obj.GetFederalTaxWithholdingPercent();
		ASSERT_EQ(FedTaxWithholdPercent, 7.5) << "FedTaxWithholdPercent is incorrect.";

		// cleanup
		delete buffer;
	}

	void StateTaxWithholdPercent()
	{
		//setup
		char key[] = "StateTaxWithholdPercent";
		char value[] = "4.5";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		float StateTaxWithholdPercent = obj.GetStateTaxWithholdingPercent();
		ASSERT_EQ(StateTaxWithholdPercent, 4.5) << "StateTaxWithholdPercent is incorrect.";

		// cleanup
		delete buffer;
	}

	void PollerLoadBalanceTime_Valid()
	{
		//setup
		char key[] = "PollerLoadBalanceTime";
		char value[] = "04:30";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		BYTE hour, minute;
		obj.GetPollerLoadBalanceTime(hour, minute);
		ASSERT_TRUE(hour == 4) << "Hour is not correct.";
		ASSERT_TRUE(minute == 30) << "Minute is not correct.";

		// cleanup
		delete buffer;
	}

	void PollerLoadBalanceTime_InvalidFormat()
	{
		//setup
		char key[] = "PollerLoadBalanceTime";
		char value[] = "4:30";  // invalid format
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		BYTE hour, minute;
		obj.GetPollerLoadBalanceTime(hour, minute);
		ASSERT_TRUE(hour == 0xff) << "Hour is not correct.";
		ASSERT_TRUE(minute == 0xff) << "Minute is not correct.";

		// cleanup
		delete buffer;
	}

	void PollerLoadBalanceTime_InvalidData()
	{
		//setup
		char key[] = "PollerLoadBalanceTime";
		char value[] = "24:30";  // invalid data
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		BYTE hour, minute;
		obj.GetPollerLoadBalanceTime(hour, minute);
		ASSERT_TRUE(hour == 0xff) << "Hour is not correct.";
		ASSERT_TRUE(minute == 0xff) << "Minute is not correct.";

		// cleanup
		delete buffer;
	}

	void PollerReconnect_Enabled()
	{
		//setup
		char key[] = "Reconnect";
		char value[] = "1";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isPollerReconnect = obj.PollerReconnect();
		ASSERT_TRUE(isPollerReconnect) << "Reconnect is not enabled.";

		// cleanup
		delete buffer;
	}

	void PollerReconnect_Disabled()
	{
		//setup
		char key[] = "Reconnect";
		char value[] = "0";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isPollerReconnect = obj.PollerReconnect();
		ASSERT_FALSE(isPollerReconnect) << "Reconnect is not disabled.";

		// cleanup
		delete buffer;
	}
	
	void MaintenanceMode_Enabled()
	{
		//setup
		char key[] = "MaintenanceMode";
		char value[] = "1";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isMaintenanceMode = obj.MaintenanceMode();
		ASSERT_TRUE(isMaintenanceMode) << "MaintenanceMode is not enabled.";

		// cleanup
		delete buffer;
	}

	void MaintenanceMode_Disabled()
	{
		//setup
		char key[] = "MaintenanceMode";
		char value[] = "0";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool isMaintenanceMode = obj.MaintenanceMode();
		ASSERT_FALSE(isMaintenanceMode) << "MaintenanceMode is not disabled.";

		// cleanup
		delete buffer;
	}	

	void CombineZipLogs_Enabled()
	{
		//setup
		char key[] = "CombineZipLogs";
		char value[] = "1";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool combineZipLogs = obj.CombineZipLogs();
		ASSERT_TRUE(combineZipLogs) << "CombineZipLogs is not enabled.";

		// cleanup
		delete buffer;
	}

	void CombineZipLogs_Disabled()
	{
		//setup
		char key[] = "CombineZipLogs";
		char value[] = "0";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool combineZipLogs = obj.CombineZipLogs();
		ASSERT_FALSE(combineZipLogs) << "CombineZipLogs is not disabled.";

		// cleanup
		delete buffer;
	}

	void ValidationCheck_EmptyKeyName()
	{
		//setup
		char key[] = "";
		char value[] = "10";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.AutoUploadCredits()) << "AutoUploadCredits";
		ASSERT_FALSE(obj.CashoutToHost()) << "CashoutToHost";
		ASSERT_FALSE(obj.UJPTaxWithholding()) << "UJPTaxWithholding";
		ASSERT_EQ(obj.GetFederalTaxWithholdingPercent(), 0.0) << "GetFederalTaxWithholdingPercent";
		ASSERT_EQ(obj.GetStateTaxWithholdingPercent(), 0.0) << "GetStateTaxWithholdingPercent";
		ASSERT_FALSE(obj.PollerReconnect()) << "PollerReconnect";
		ASSERT_FALSE(obj.MaintenanceMode()) << "MaintenanceMode";
		BYTE hour = 0;
		BYTE minute = 0;
		obj.GetPollerLoadBalanceTime(hour, minute);
		ASSERT_EQ(hour, 0xFF) << "PollerLoadBalancingTime - hour";
		ASSERT_EQ(minute, 0xFF) << "PollerLoadBalancingTime - minute";

		// cleanup
		delete buffer;
	}

	void ValidationCheck_EmptyValue()
	{
		//setup
		char key[] = "MaintenanceMode";
		char value[] = "";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.MaintenanceMode()) << "MaintenanceMode";

		// cleanup
		delete buffer;
	}

	void ValidationCheck_InvalidArgument()
	{
		//setup
		char key[] = "FedTaxWithholdPercent";
		char value[] = "abc";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		float FedTaxWithholdPercent = obj.GetFederalTaxWithholdingPercent();
		ASSERT_EQ(FedTaxWithholdPercent, 0.0) << "FedTaxWithholdPercent is incorrect.";

		// cleanup
		delete buffer;
	}

	void ValidationCheck_OutOfRange()
	{
		//setup
		char key[] = "FedTaxWithholdPercent";
		char value[] = "3.502823e+38";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		float FedTaxWithholdPercent = obj.GetFederalTaxWithholdingPercent();
		ASSERT_EQ(FedTaxWithholdPercent, 0.0) << "FedTaxWithholdPercent is incorrect.";

		// cleanup
		delete buffer;
	}

	void AutoUploadDelaySeconds()
	{
		//setup
		char key[] = "AutoUploadDelaySeconds";
		char value[] = "5";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(5 == obj.GetAutoUploadDelaySeconds()) << "AutoUploadDelaySeconds";

		// cleanup
		delete buffer;
	}

	void GetSocialDistanceCountdownDelay()
	{
		//setup
		char key[] = "SocialDistanceCountdownDelay";
		char value[] = "47";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(47 == obj.GetSocialDistanceCountdownDelay()) << "SocialDistanceCountdownDelay";

		// cleanup
		delete buffer;
	}

	
	void AutoCardOutSecToCountdownCredits()
	{
		//setup
		char key[] = "AutoCardOutSecToCountdownCredits";
		char value[] = "30";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(30 == obj.GetAutoCardOutSecToCountdownCredits()) << "AutoCardOutSecToCountdownCredits";

		// cleanup
		delete buffer;
	}

	void AutoCardOutSecToCountdownNoCrdts()
	{
		//setup
		char key[] = "AutoCardOutSecToCountdownNoCrdts";
		char value[] = "29";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(29 == obj.GetAutoCardOutSecToCountdownNoCrdts()) << "AutoCardOutSecToCountdownNoCrdts";

		// cleanup
		delete buffer;
	}

	void AutoCardOutCountdownSeconds()
	{
		//setup
		char key[] = "AutoCardOutCountdownSeconds";
		char value[] = "28";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(28 == obj.GetAutoCardOutCountdownSeconds()) << "AutoCardOutCountdownSeconds";

		// cleanup
		delete buffer;
	}

	void AutoCardOutSecToAlert()
	{
		//setup
		char key[] = "AutoCardOutSecToAlert";
		char value[] = "27";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(27 == obj.GetAutoCardOutSecToAlert()) << "AutoCardOutSecToAlert";

		// cleanup
		delete buffer;
	}

	void NumberOfSubgamesMismatchCheck()
	{
		//setup
		char key[] = "NumberOfSubgamesMismatchCheck";
		char value[] = "50";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(50 == obj.GetNumberOfSubgamesMismatchCheck()) << "NumberOfSubgamesMismatchCheck";

		// cleanup
		delete buffer;
	}

	void GetExtensibleSettingsStr()
	{
		//setup
		const string extensibleSettingsStr("<b>Extensible System Settings:</b><br>AutoUploadCredits:Yes<br>");

		char key[] = "AutoUploadCredits";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(extensibleSettingsStr == obj.GetExtensibleSettingsStr());

		// cleanup
		delete buffer;
	}

	void GetCountdownDisplayIntervalSeconds()
	{
		//setup
		char key[] = "CountdownDisplayIntervalInSec";
		char value[] = "5";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(5 == obj.GetCountdownDisplayIntervalSeconds()) << "GetCountdownDisplayIntervalSeconds";

		// cleanup
		delete buffer;
	}

	void GetPointsCountdownControlledByUniverse_Enabled()
	{
		//setup
		char key[] = "CountdownControlByUniverse";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.GetPointsCountdownControlledByUniverse());

		// cleanup
		delete buffer;
	}

	void GetPointsCountdownControlledByUniverse_Disabled()
	{
		//setup
		char key[] = "CountdownControlByUniverse";
		char value[] = "No";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.GetPointsCountdownControlledByUniverse());

		// cleanup
		delete buffer;
	}

	void TicketInResend_Enabled()
	{
		//setup
		char key[] = "TicketInResend";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.GetTicketInResend());

		// cleanup
		delete buffer;
	}

	void TicketInResend_Disabled()
	{
		//setup
		char key[] = "TicketInResend";
		char value[] = "No";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.GetTicketInResend());

		// cleanup
		delete buffer;
	}

	void nConnectURL()
	{
		//setup
		char key[] = "nConnectUrl";
		char value[] = "https://SampleUrl";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetnConnectUrl(), value);

		// cleanup
		delete buffer;
	}

	void nConnectToken()
	{
		//setup
		char key[] = "nConnectToken";
		char value[] = "SampleToken";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetnConnectToken(), value);

		// cleanup
		delete buffer;
	}

	void AvatarURL()
	{
		//setup
		char key[] = "AvatarUrl";
		char value[] = "https://SampleUrl";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetAvatarUrl(), value);

		// cleanup
		delete buffer;
	}

	void AvatarToken()
	{
		//setup
		char key[] = "AvatarToken";
		char value[] = "SampleToken";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetAvatarToken(), value);

		// cleanup
		delete buffer;
	}

	void RequestServiceURL()
	{
		//setup
		char key[] = "RequestServiceUrl";
		char value[] = "https://SampleUrl";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetRequestServiceUrl(), value);

		// cleanup
		delete buffer;
	}

	void RequestServiceToken()
	{
		//setup
		char key[] = "RequestServiceToken";
		char value[] = "SampleToken";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetRequestServiceToken(), value);

		// cleanup
		delete buffer;
	}

	void DrinksOnTapURL()
	{
		//setup
		char key[] = "DrinksOnTapUrl";
		char value[] = "https://SampleUrl";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetDrinksOnTapUrl(), value);

		// cleanup
		delete buffer;
	}

	void DrinksOnTapToken()
	{
		//setup
		char key[] = "DrinksOnTapToken";
		char value[] = "SampleToken";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetDrinksOnTapToken(), value);

		// cleanup
		delete buffer;
	}

	void SocialDistancingURL()
	{
		//setup
		char key[] = "SocialDistanceUrl";
		char value[] = "https://SampleUrl";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetSocialDistanceUrl(), value);

		// cleanup
		delete buffer;
	}

	void SocialDistancingToken()
	{
		//setup
		char key[] = "SocialDistanceToken";
		char value[] = "SampleToken";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetSocialDistanceToken(), value);

		// cleanup
		delete buffer;
	}

	void GetArchiveLogDescriptionStr()
	{
		//setup
		char key[] = "ArchiveLogDescription";
		char value[] = "SampleLogs";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_EQ(obj.GetArchiveLogDescriptionStr(), value);

		// cleanup
		delete buffer;
	}

	void GetMeterRolloverSyncEnabled_Enabled()
	{
		//setup
		char key[] = "MeterRolloverSyncEnabled";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.GetMeterRolloverSyncEnabled());

		// cleanup
		delete buffer;
	}

	void GetMeterRolloverSyncEnabled_Disabled()
	{
		//setup
		char key[] = "MeterRolloverSyncEnabled";
		char value[] = "No";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.GetMeterRolloverSyncEnabled());

		// cleanup
		delete buffer;
	}

	void MeterRolloverTime_Valid()
	{
		//setup
		char key[] = "MeterRolloverTime";
		char value[] = "04:30:59";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool rolloverTimeSycnEnabled;
		BYTE hour, minute, second;
		obj.GetMeterRolloverTime(rolloverTimeSycnEnabled, hour, minute, second);
		cout << "MeterRolloverTime_Valid hour=" << (DWORD)hour << " minute=" << (DWORD)minute << " second=" << (DWORD)second << endl;
		ASSERT_TRUE(hour == 4) << "Hour is not correct.";
		ASSERT_TRUE(minute == 30) << "Minute is not correct.";
		ASSERT_TRUE(second == 59) << "Second is not correct.";

		// cleanup
		delete buffer;
	}

	void MeterRolloverTime_InvalidFormat()
	{
		//setup
		char key[] = "MeterRolloverTime";
		char value[] = "4:30:59";  // invalid format
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool rolloverTimeSycnEnabled;
		BYTE hour, minute, second;
		obj.GetMeterRolloverTime(rolloverTimeSycnEnabled, hour, minute, second);
		ASSERT_TRUE(hour == 0xff) << "Hour is not correct.";
		ASSERT_TRUE(minute == 0xff) << "Minute is not correct.";
		ASSERT_TRUE(second == 0xff) << "Second is not correct.";

		// cleanup
		delete buffer;
	}

	void MeterRolloverTime_InvalidData()
	{
		//setup
		char key[] = "MeterRolloverTime";
		char value[] = "24:30:59";  // invalid data
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool rolloverTimeSycnEnabled;
		BYTE hour, minute, second;
		obj.GetMeterRolloverTime(rolloverTimeSycnEnabled, hour, minute, second);
		ASSERT_TRUE(hour == 0xff) << "Hour is not correct.";
		ASSERT_TRUE(minute == 0xff) << "Minute is not correct.";
		ASSERT_TRUE(second == 0xff) << "Second is not correct.";

		// cleanup
		delete buffer;
	}

	void GetRestartUIAfterxDays()
	{
		//setup
		char key[] = "RestartUIAfterxDays";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRestartUIAfterxDays()) << "RestartUIAfterxDays";

		// cleanup
		delete buffer;
	}

	
	void GetRebootASAPThreshold()
	{
		//setup
		char key[] = "RebootAsapThresholdInMb";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRebootAsapThresholdInMb()) << "RebootAsapThresholdInMb";

		// cleanup
		delete buffer;
	}

	void GetRebootWarningThreshold()
	{
		//setup
		char key[] = "RebootWarningThresholdMb";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRebootWarningThresholdMb()) << "RebootWarningThresholdMb";

		// cleanup
		delete buffer;
	}

	void GetRebootUiASAPThreshold()
	{
		//setup
		char key[] = "RestartUIAsapThresholdMb";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRestartUIAsapThresholdMb()) << "RestartUIAsapThresholdMb";

		// cleanup
		delete buffer;
	}

	void GetRebootUiAfterDropThreshold()
	{
		//setup
		char key[] = "RestartUIAfterDropThresholdMb";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRestartUIAfterDropThresholdMb()) << "RestartUIAfterDropThresholdMb";

		// cleanup
		delete buffer;
	}

	void GetRebootUiDelayHours()
	{
		//setup
		char key[] = "RestartUIDelayHours";
		char value[] = "17";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(17 == obj.GetRestartUIDelayHours()) << "RestartUIDelayHours";

		// cleanup
		delete buffer;
	}

	void GetSASReadTimeOutMultiplier()
	{
		//setup
		char key[] = "SASReadTimeOutMilliseconds";
		char value[] = "3100";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(31 == obj.GetSASReadTimeOutMultiplier()) << "SASReadTimeOutMilliseconds";

		// cleanup
		delete buffer;
	}

	void GetSocialDistanceIgnoreDirtyOn()
	{
		//setup
		char key[] = "SocialDistanceIgnoreDirty";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.GetSocialDistanceIgnoreDirty());

		// cleanup
		delete buffer;
	}

	void GetSocialDistanceIgnoreDirtyOff()
	{
		//setup
		char key[] = "SocialDistanceIgnoreDirty";
		char value[] = "No";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.GetSocialDistanceIgnoreDirty());

		// cleanup
		delete buffer;
	}

	void GetSocialDistanceManualCleanEnabled()
	{
		//setup
		char key[] = "SocialDistanceManualCleanEnabled";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.GetSocialDistanceManualCleanEnabled());

		// cleanup
		delete buffer;
	}

	void GetSocialDistanceManualCleanDisabled()
	{
		//setup
		char key[] = "SocialDistanceManualCleanEnabled";
		char value[] = "No";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.GetSocialDistanceManualCleanEnabled());

		// cleanup
		delete buffer;
	}

	void UseSDSCardFormat_Enabled()
	{
		//setup
		char key[] = "SDSCARDFULLSUPPORT";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_TRUE(obj.UseSDSCardFormat());

		// cleanup
		delete buffer;
	}
	void UseSDSCardFormat_Disabled()
	{
		//setup
		char key[] = "SDSCARDFULLSUPPORT";
		char value[] = "No";
		int bufferLength = 0;
		BYTE* buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		ASSERT_FALSE(obj.UseSDSCardFormat());

		// cleanup
		delete buffer;
	}

	void EnableLocationServices_Enabled()
	{
		//setup
		char key[] = "EnableLocationServices";
		char value[] = "Yes";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool enableLocationServices = obj.GetEnableLocationServices();
		ASSERT_TRUE(enableLocationServices) << "EnableLocationService is enabled.";

		// cleanup
		delete buffer;
	}

	void EnableLocationServices_Disabled()
	{
		//setup
		char key[] = "EnableLocationServices";
		char value[] = "No";
		int bufferLength = 0;
		BYTE *buffer = CreateBuffer(key, value, bufferLength);

		// perform operation
		CConfigExtensibleSystemSettingsField obj(buffer, bufferLength);

		// verify
		bool enableLocationServices = obj.GetEnableLocationServices();
		ASSERT_FALSE(enableLocationServices) << "EnableLocationService is not enabled.";

		// cleanup
		delete buffer;
	}	
};

/// <summary>
/// Functional test for consuming auto upload credits using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoUploadCredits_Enabled)
{
	AutoUploadCredits_Enabled();
}

/// <summary>
/// Functional test for consuming auto upload credits using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoUploadCredits_Disabled)
{
	AutoUploadCredits_Disabled();
}

/// <summary>
/// Functional test for consuming cashout to host using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, CashoutToHost_Enabled)
{
	CashoutToHost_Enabled();
}

/// <summary>
/// Functional test for consuming cashout to host using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, CashoutToHost_Disabled)
{
	CashoutToHost_Disabled();
}

/// <summary>
/// Functional test for consuming UTJ tax withhold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, UTJTaxWithhold_Enabled)
{
	UTJTaxWithhold_Enabled();
}

/// <summary>
/// Functional test for consuming UTJ tax withhold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, UTJTaxWithhold_Disabled)
{
	UTJTaxWithhold_Disabled();
}

/// <summary>
/// Functional test for consuming fed tax withhold percentage using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, FedTaxWithholdPercent)
{
	FedTaxWithholdPercent();
}

/// <summary>
/// Functional test for consuming state tax withhold percentage using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, StateTaxWithholdPercent)
{
	StateTaxWithholdPercent();
}

/// <summary>
/// Functional test for consuming poller load balancing time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, PollerLoadBalanceTime_Valid)
{
	PollerLoadBalanceTime_Valid();
}

/// <summary>
/// Functional test for consuming poller load balancing time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, PollerLoadBalanceTime_InvalidFormat)
{
	PollerLoadBalanceTime_InvalidFormat();
}

/// <summary>
/// Functional test for consuming poller load balancing time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, PollerLoadBalanceTime_InvalidData)
{
	PollerLoadBalanceTime_InvalidData();
}

/// <summary>
/// Functional test for consuming poller reconnect setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, PollerReconnect_Enabled)
{
	PollerReconnect_Enabled();
}

/// <summary>
/// Functional test for consuming poller reconnect setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, PollerReconnect_Disabled)
{
	PollerReconnect_Disabled();
}

/// <summary>
/// Functional test for consuming maintenance mode setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, MaintenanceMode_Enabled)
{
	MaintenanceMode_Enabled();
}

/// <summary>
/// Functional test for consuming maintenance mode setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, MaintenanceMode_Disabled)
{
	MaintenanceMode_Disabled();
}

/// <summary>
/// Functional test for consuming combine zip logs setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, CombineZipLogs_Enabled)
{
	CombineZipLogs_Enabled();
}

/// <summary>
/// Functional test for consuming combine zip logs setting using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, CombineZipLogs_Disabled)
{
	CombineZipLogs_Disabled();
}

/// <summary>
/// Validation Check for CConfigExtensibleSystemSettingsField instance when empty key name is passed.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, ValidationCheck_EmptyKeyName)
{
	ValidationCheck_EmptyKeyName();
}

/// <summary>
/// Validation Check for CConfigExtensibleSystemSettingsField instance when empty value is passed.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, ValidationCheck_EmptyValue)
{
	ValidationCheck_EmptyValue();
}

/// <summary>
/// Validation Check for CConfigExtensibleSystemSettingsField instance when invalid argument is passed as value.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, ValidationCheck_InvalidArgument)
{
	ValidationCheck_InvalidArgument();
}

/// <summary>
/// Validation Check for CConfigExtensibleSystemSettingsField instance when out of range value is passed.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, ValidationCheck_OutOfRange)
{
	ValidationCheck_OutOfRange();
}

/// <summary>
/// Functional test for consuming AutoUploadDelaySeconds using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoUploadDelaySeconds)
{
	AutoUploadDelaySeconds();
}

/// <summary>
/// Functional test for consuming AutoCardOutSecToCountdownCredits using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoCardOutSecToCountdownCredits)
{
	AutoCardOutSecToCountdownCredits();
}

/// <summary>
/// Functional test for consumingAutoCardOutSecToCountdownNoCrdts using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoCardOutSecToCountdownNoCrdts)
{
	AutoCardOutSecToCountdownNoCrdts();
}

/// <summary>
/// Functional test for consuming AutoCardOutCountdownSeconds using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoCardOutCountdownSeconds)
{
	AutoCardOutCountdownSeconds();
}

/// <summary>
/// Functional test for consuming AutoCardOutSecToAlert using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AutoCardOutSecToAlert)
{
	AutoCardOutSecToAlert();
}

/// <summary>
/// Functional test for consuming NumberOfSubgamesMismatchCheck using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, NumberOfSubgamesMismatchCheck)
{
	NumberOfSubgamesMismatchCheck();
}

/// <summary>
/// Functional test for GetExtensibleSettingsStr using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetExtensibleSettingsStrTest)
{
	GetExtensibleSettingsStr();
}

/// <summary>
/// Functional test for GetCountdownDisplayIntervalSeconds using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetCountdownDisplayIntervalSeconds)
{
	GetCountdownDisplayIntervalSeconds();
}

/// <summary>
/// Functional test for GetPointsCountdownControlledByUniverse using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetPointsCountdownControlledByUniverse_Enabled)
{
	GetPointsCountdownControlledByUniverse_Enabled();
}

/// <summary>
/// Functional test for GetPointsCountdownControlledByUniverse using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetPointsCountdownControlledByUniverse_Disabled)
{
	GetPointsCountdownControlledByUniverse_Disabled();
}

/// <summary>
/// Functional test for GetArchiveLogDescriptionStr using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetArchiveLogDescriptionStr)
{
	GetArchiveLogDescriptionStr();
}

/// <summary>
/// Functional test for GetMeterRolloverSyncEnabled using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetMeterRolloverSyncEnabled_Enabled)
{
	GetMeterRolloverSyncEnabled_Enabled();
}

/// <summary>
/// Functional test for GetMeterRolloverSyncEnabled using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetMeterRolloverSyncEnabled_Disabled)
{
	GetMeterRolloverSyncEnabled_Disabled();
}

/// <summary>
/// Functional test for consuming meter rollover time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, MeterRolloverTime_Valid)
{
	MeterRolloverTime_Valid();
}

/// <summary>
/// Functional test for consuming meter rollover time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, MeterRolloverTime_InvalidFormat)
{
	MeterRolloverTime_InvalidFormat();
}

/// <summary>
/// Functional test for consuming meter rollover time using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, MeterRolloverTime_InvalidData)
{
	MeterRolloverTime_InvalidData();
}

/// <summary>
/// Functional test for consuming RestartUIAfterxDays using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRestartUIAfterxDays)
{
	GetRestartUIAfterxDays();
}

/// <summary>
/// Functional test for consuming reboot ASAP threshold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRebootASAPThreshold)
{
	GetRebootASAPThreshold();
}

/// <summary>
/// Functional test for consuming reboot warning threshold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRebootWarningThreshold)
{
	GetRebootWarningThreshold();
}

/// <summary>
/// Functional test for consuming reboot UI ASAP threshold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRebootUiASAPThreshold)
{
	GetRebootUiASAPThreshold();
}

/// <summary>
/// Functional test for consuming reboot UI after drop threshold using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRebootUiAfterDropThreshold)
{
	GetRebootUiAfterDropThreshold();
}

/// <summary>
/// Functional test for consuming reboot UI delay hours using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetRebootUiDelayHours)
{
	GetRebootUiDelayHours();
}

/// <summary>
/// Functional test for consuming TicketInResend using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, TicketInResend_Enabled)
{
	TicketInResend_Enabled();
}

/// <summary>
/// Functional test for consuming TicketInResend using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, TicketInResend_Disabled)
{
	TicketInResend_Disabled();
}

/// <summary>
/// Functional test for consuming nConnect URL using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, nConnectURL)
{
	nConnectURL();
}

/// <summary>
/// Functional test for consuming nConnect token using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, nConnectToken)
{
	nConnectToken();
}

/// <summary>
/// Functional test for consuming avatar URL using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AvatarURL)
{
	AvatarURL();
}

/// <summary>
/// Functional test for consuming avatar token using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, AvatarToken)
{
	AvatarToken();
}

/// <summary>
/// Functional test for consuming request service URL using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, RequestServiceURL)
{
	RequestServiceURL();
}

/// <summary>
/// Functional test for consuming request service token using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, RequestServiceToken)
{
	RequestServiceToken();
}

/// <summary>
/// Functional test for consuming drinks on tap URL using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, DrinksOnTapURL)
{
	DrinksOnTapURL();
}

/// <summary>
/// Functional test for consuming drinks on tap token using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, DrinksOnTapToken)
{
	DrinksOnTapToken();
}

/// <summary>
/// Functional test for consuming SASReadTimeOutMilliseconds using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSASReadTimeOutMultiplier)
{
	GetSASReadTimeOutMultiplier();
}

/// <summary>
/// Functional test for consuming social distance ignore dirty flag (on) using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSocialDistanceIgnoreDirtyOn)
{
	GetSocialDistanceIgnoreDirtyOn();
}

/// <summary>
/// Functional test for consuming social distance ignore dirty flag (off) using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSocialDistanceIgnoreDirtyOff)
{
	GetSocialDistanceIgnoreDirtyOff();
}

/// <summary>
/// Functional test for consuming social distance manual clean enabled flag using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSocialDistanceManualCleanEnabled)
{
	GetSocialDistanceManualCleanEnabled();
}

/// <summary>
/// Functional test for consuming social distance manual clean disabled flag using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSocialDistanceManualCleanDisabled)
{
	GetSocialDistanceManualCleanDisabled();
}

/// <summary>
/// Functional test for consuming social distance countdown delay using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, GetSocialDistanceCountdownDelay)
{
	GetSocialDistanceCountdownDelay();
}

/// <summary>
/// Functional test for consuming social distancing URL using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, SocialDistancingURL)
{
	SocialDistancingURL();
}

/// <summary>
/// Functional test for consuming social distancing token using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, SocialDistancingToken)
{
	SocialDistancingToken();
}

/// <summary>
/// Functional test for UseSDSCardFormat using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, UseSDSCardFormat_Enabled)
{
	UseSDSCardFormat_Enabled();
}

/// <summary>
/// Functional test for UseSDSCardFormat using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, UseSDSCardFormat_Disabled)
{
	UseSDSCardFormat_Disabled();
}

/// <summary>
/// Functional test for consuming enable location services using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, EnableLocationServices_Enabled)
{
	EnableLocationServices_Enabled();
}

/// <summary>
/// Functional test for consuming enable location services using CConfigExtensibleSystemSettingsField instance.
/// </summary>
TEST_F(ConfigExtensibleSystemSettingsFieldUnitTests, EnableLocationServices_Disabled)
{
	EnableLocationServices_Disabled();
}
