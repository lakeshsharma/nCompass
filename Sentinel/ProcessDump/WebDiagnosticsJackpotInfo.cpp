#include "stdafx.h"
#include "Jackpot.h"

// Sentinel includes.
#include "Util/Path.h"
#include "WebDiagnostics.h"
#include "EnumMap.h"

using namespace std;

void CJackpot::UpdateWebDiagnostics(const CWebDiagnosticsCommonInfoBase& commonInfo, const CConfig& config) const
{
	string pathname;
	pathname = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + CWebDiagnostics::JackpotInfoFilename;

	ofstream os;
	os.open(pathname, ios_base::out | ios_base::binary | ios_base::trunc);
	if (!os)
	{
		LogString(GENERAL, "Error (%u) opening: %s", CUtilities::GetLastError(), (LPCTSTR) pathname.c_str());
	}
	else
	{
		WriteWebDiagnostics(commonInfo, os, config);
	}
}

static EnumMap<JackpotType>::EnumPair types[] =
{
	{ JT_NONE			, "None"			},
	{ JT_HANDPAY		, "Hand Pay"		},
	{ JT_CANCEL_CREDIT	, "Cancel Credit"	},
	{ JT_TOPAWARD		, "Top Award"		},
	{ JT_EXTERNALBONUS	, "External Bonus"	},
};

EnumSparseMap<JackpotType> CJackpot::m_typeEmap(types);

static EnumMap<JackpotTransactionType>::EnumPair transactionTypes[] =
{
	{ JTRANS_COMPOSITE					, "Composite"				 },
	{ JTRANS_PROGRESSIVE_HOPPER_PAID	, "Progressive Hopper Paid"  },
	{ JTRANS_PROGRESSIVE_SCANNER_ONLY	, "Progressive Scanner Only" },
	{ JTRANS_NONE						, "None"					 },
};

EnumMap<JackpotTransactionType> CJackpot::m_transactionTypeEmap(transactionTypes);

void CJackpot::WriteWebDiagnostics(const CWebDiagnosticsCommonInfoBase& commonInfo, ostream& os, const CConfig& config) const
{
	os.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		CWebDiagnostics::WriteHtmlHeader(os, CWebDiagnostics::JackpotInfoRefreshSeconds, "Jackpot Info");
		commonInfo.WriteHtml(os);

		const char* pUnknown = "Unknown";

		os << "Jackpot ID: " << m_jackpotID << "<br>"
			"Jackpot Type: " << m_typeEmap.At(m_jpType, pUnknown) << "<br>"
			"Jackpot Transaction Type: " << m_transactionTypeEmap.At(m_transType, pUnknown) << "<br>"
			"Jackpot Amount In Pennies: " << m_jackpotAmount << "<br>"
			"Progressive Level: " << (unsigned) m_progressiveLevel << "<br>"
			"Coins Played: " << m_coinsPlayed << "<br>"
			"Key to Credit Available: " << CWebDiagnostics::TrueFalseStr(m_keyToCreditAvailable) << "<br>"
			"Unattended State: " << GetUnattendedStateStr() << "<br>"
			"Unattended State Tick: " << m_unattendedStateTimestamp << "<br>"
			"Transaction Sent: " << CWebDiagnostics::TrueFalseStr(m_jackpotTransactionSent) << "<br>"
			"Card ID: ";

		os << setfill('0') << hex;

		for (int i = 0; i < CARD_ID_LEN; i++)
		{
			os << setw(2) << (unsigned) m_cardID[i];
		}

		os << setfill(' ') << dec << "<br>"
			"Hand Pay Reset: " << CWebDiagnostics::TrueFalseStr(m_handpayReset) << "<br>"
			"Federal Tax Withheld In Pennies: " << m_federalTaxWithholding << "<br>"
			"Federal Tax Overridden: " << CWebDiagnostics::TrueFalseStr(m_federalTaxOverridden) << "<br>"
			"State Tax Withheld In Pennies: " << m_stateTaxWithholding << "<br>"
			"State Tax Overridden: " << CWebDiagnostics::TrueFalseStr(m_stateTaxOverridden) << "<br><br>"

			"Tax Withholding Enabled: " << CWebDiagnostics::TrueFalseStr(config.UJPTaxWithholding()) << "<br>"
			"Federal Tax Withholding Percent: " << config.GetFederalTaxWithholdingPercent() << "<br>"
			"State Tax Withholding Percent: " << config.GetStateTaxWithholdingPercent() << "<br>"
			"</body>"
			"</html>";

	}
    catch (std::ios_base::failure &e)
	{
        LogString(GENERAL, "Exception: CJackpot::WriteWebDiagnostics:%d: %s",e.code().value(), e.what());
	}
}
