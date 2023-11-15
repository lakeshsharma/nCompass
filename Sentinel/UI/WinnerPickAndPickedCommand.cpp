#include "WinnerPickAndPickedCommand.h"
#include "../ProgressiveProtocol/ProgressiveKeys.h"

using namespace std;
using namespace AwardType;
using namespace MethodOfPayment;

CWinnerPickAndPickedCommand::CWinnerPickAndPickedCommand(DWORD displayDuration, 
	const string& assetTag,
	const string& dynamicContentPath,
	const string& levelName,
	const string& awardAmount,
	const string& prizeDescription,
	const string& paymentMethod,
	const string& awardType,
	const string& externalAwardName,
	const string& externalAwardValue,
	const string& externalAwardCurrency) :
	CFullScreenAttractorCommand(displayDuration, assetTag, dynamicContentPath)
	,m_levelName(levelName)
    ,m_awardAmount(awardAmount)
    ,m_prizeDescription(prizeDescription)
    ,m_paymentMethod(paymentMethod)
    ,m_awardType(awardType)
    ,m_externalAwardName(externalAwardName)
    ,m_externalAwardValue(externalAwardValue)
    ,m_externalAwardCurrency(externalAwardCurrency)
{
	if (pPrize == m_awardType || pOffer == m_awardType || pExtendedBucket == m_awardType)
	{
		m_assetTag.append(m_awardType);
	}
}

CWinnerPickAndPickedCommand::~CWinnerPickAndPickedCommand()
{
}

void CWinnerPickAndPickedCommand::AddDynamicTags(JsonWriter &writer) const
{
	writer.Key("dynamicTags");
	writer.StartArray();

	writer.StartObject();
	writer.Key("name");		writer.String("LevelName");
	writer.Key("value");	writer.String(m_levelName);
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("PaymentMethod");
	writer.Key("value");	writer.String(m_paymentMethod);
	writer.EndObject();
	
	writer.StartObject();
	writer.Key("name");		writer.String("AwardCurrency");
	writer.Key("value");	writer.String(GetAwardCurrency());
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("AwardDescription");
	writer.Key("value");	writer.String(GetAwardDescription());
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("PaymentModePrefix");
	writer.Key("value");	writer.String(GetPaymentModePrefix());
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("PaymentMode");
	writer.Key("value");	writer.String(GetPaymentMode());
	writer.EndObject();

	writer.StartObject();
	writer.Key("name");		writer.String("PaymentBucket");
	writer.Key("value");	writer.String(m_externalAwardName);
	writer.EndObject();

	writer.EndArray();
}

string CWinnerPickAndPickedCommand::GetAwardDescription() const
{
	string awardAmount(m_awardAmount);

	if (pExtendedBucket == m_awardType)
	{
		awardAmount = m_externalAwardValue;
	}
	else if(pOffer == m_awardType)
	{
		awardAmount = m_externalAwardName;
	}
	else if (pPrize == m_awardType)
	{
		awardAmount = m_prizeDescription;
	}
	
	return CUtilities::ConvertStringToDecimalIfNecessary(awardAmount, 2);
}

std::string CWinnerPickAndPickedCommand::GetAwardCurrency() const
{
	//Tell the Angular to use the skin JSON value
	string awardCurrrency("AwardCurrency");
	
	if (pExtendedBucket == m_awardType)
	{
		awardCurrrency = m_externalAwardCurrency;
	}
	else if (pOffer == m_awardType || pPrize == m_awardType)
	{// offer or prize related scenarios
		awardCurrrency.clear();
	}

	return awardCurrrency;
}

string CWinnerPickAndPickedCommand::GetPaymentMode() const
{
	string paymentMode;

	if (m_paymentMethod == pPayToGame && m_awardType == pCashableCredits)
	{
		paymentMode = "CashableCreditsToGame";
	}
	else if (m_paymentMethod == pPayToGame && m_awardType == pFreeplay) 
	{
		paymentMode = "FreePlayToGame";
	}
	else if (m_paymentMethod == pPayToGame && m_awardType == pRestrictedCredits) 
	{
		paymentMode = "RestrictedCreditsToGame";
	}
	else if (m_paymentMethod == pPayToGameWithHandPay && m_awardType == pCashableCredits) 
	{
		paymentMode = "CashableCreditsHandpayToGame";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pCashableCredits) 
	{
		paymentMode = "CashableCreditsToAccount";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pComps) 
	{
		paymentMode = "CompsToAccount";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pFreeplay) 
	{
		paymentMode = "FreePlayToAccount";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pPoints) 
	{
		paymentMode = "PointsToAccount";
	}
	else if (m_paymentMethod == pPayToGame && m_awardType == pPrize)
	{
		paymentMode = "PrizeToAccount";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pOffer) 
	{
		paymentMode = "OfferToAccount";
	}
	else if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pExtendedBucket) 
	{
		paymentMode = "ExtendedBucketToAccount";
	}

	return paymentMode;
}

string CWinnerPickAndPickedCommand::GetPaymentModePrefix() const
{
	string paymentModePrefix;

	if (m_paymentMethod == pPayToPlayerAccount && m_awardType == pExtendedBucket)
	{
		paymentModePrefix = "ExtendedBucketToAccountPrefix";
	}

	return paymentModePrefix;
}