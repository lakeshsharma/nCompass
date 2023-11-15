#pragma once
#include "PollerMessage.h"
#include "../Config/ConfigFields.h"
#include "../Config/ConfigField.h"
#include "../Config/ConfigDenomField.h"
#include "../Config/ConfigCasinoNumField.h"
#include "../Config/ConfigCardTimeoutField.h"
#include "../Config/ConfigCasinoNameField.h"
#include "../Config/ConfigCommIDField.h"
#include "../Config/ConfigFloorLockField.h"
#include "../Config/ConfigLossLimitField.h"
#include "../Config/ConfigOtherPrefixesField.h"
#include "../Config/ConfigPlayerPrefixField.h"
#include "../Config/ConfigPlayerTrackingField.h"
#include "../Config/ConfigPointsPerDollarField.h"
#include "../Config/ConfigSenOption1Field.h"
#include "../Config/ConfigSenOption2Field.h"
#include "../Config/ConfigSlotNumberField.h"
#include "../Config/ConfigTicketingField.h"
#include "../Config/ConfigVirtualSessionField.h"
#include "../Config/ConfigPBTField.h"
#include "../Config/ConfigCashTransferTrackingField.h"
#include "../Config/ConfigTimezoneField.h"
#include "../Config/ConfigSlotMastRevisionField.h"
#include "../Config/ConfigAllowFloorLockoutField.h"
#include "../Config/ConfigPointsForPromoField.h"
#include "../Config/ConfigUpdateMMTField.h"
#include "../Config/ConfigUniverseEnabledField.h"
#include "../Config/ConfigUniverseCardPrefixField.h"
#include "../Config/ConfigUniversePropCodesField.h"
#include "../Config/ConfigFeatureFlagField.h"
#include "../Config/ConfigNclEnabledCardTrackField.h"
#include "../Config/ConfigSessionPointsTextField.h"
#include "../Config/ConfigBalanceRetrievalTextField.h"
#include "../Config/ConfigPointsOrCashLabelField.h"
#include "../Config/ConfigBalanceRetrievalTextField.h"
#include "../Config/ConfigPointsOrCashLabelField.h"
#include "../Config/ConfigShowPointsVsAccountBalanceField.h"
#include "../Config/ConfigDailyPointsDisplaySuffixField.h"
#include "../Config/ConfigTierPointsLabelField.h"
#include "../Config/ConfigPlayerProfileButtonField.h"
#include "../Config/ConfigMGMDField.h"
#include "../Config/ConfigExcessiveVoucherOutEnabledField.h"
#include "../Config/ConfigExcessiveVoucherOutThresholdField.h"
#include "../Config/ConfigPartialWATOutEnabledField.h"
#include "../Config/ConfigExtensibleSystemSettingsField.h"

class CPollerConfigRespMessage :
	public CPollerMessage
{
public:
	/// <summary>
	/// Constructs a poller config response message class
	/// </summary>
	/// <param name="hdr">IN - The Poller header for this message.</param>
	/// <param name="dataPtr">IN - A pointer to the message data.</param>
	CPollerConfigRespMessage(const PollerMessageHeader &hdr, const byte *dataPtr);

	/// <summary>
	/// Destructor
	/// </summary>
	~CPollerConfigRespMessage( void );


	/// <summary>
	/// //This will return all the fields in the config message.
	/// </summary>
	/// <returns>A pointer to the CConfigFields object.</returns>
	CConfigFields * GetFields();

private:
	CConfigField * GetField(FieldTypes type, byte * buffer, int length);
	bool IsNclEnabledCardTrackFieldSupported();
};
