#include "stdafx.h"
#include "WebDiagnostics.h"

// Sentinel includes.
#include "Hardware/CardReaderLayer.h"
#include "Util/Path.h"

using namespace std;

void CWebDiagnostics::UpdateCardReader(
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CMCRWrapper* pMcr)
{
	if (pMcr != nullptr)
	{
		string pathname;
		pathname = Path::AddSlash(CDirectoryManager::Instance().HtmlFilePath().c_str()) + CardReaderInfoFilename;
		ofstream os;
		os.open(pathname, ios_base::out | ios_base::binary | ios_base::trunc);
		if (!os)
		{
			LogString(GENERAL, "Error (%d) opening: %s", CUtilities::GetLastError(), (LPCTSTR) pathname.c_str());
		}
		else
		{
			SaveCardReaderData(os, commonInfo, pMcr);
		}
	}
}

void CWebDiagnostics::WriteHtmlHeader(
	ostream& os,
	int refresh,
	const char* pTitle, 
	uint32_t flags, 
	const char* pCustomHeadElement,
	const char* pCustomStyle)
{
	os <<	"<html>"
			"<head>"
				"<meta charset=\"utf-8\" http-equiv=\"refresh\" content=\"" << refresh << "\" />" 
				"<meta http-equiv=\"cache-control\" content=\"no-store\" />";

	if (pTitle != nullptr)
	{
		os <<	"<title>" << pTitle << "</title>";
	}

	if (pCustomHeadElement != nullptr)
	{
		os <<	pCustomHeadElement;
	}

	if ((flags & UseStyles) != 0 || pCustomStyle != nullptr)
	{
		os <<	"<style>";
		if ((flags & UseTableStyle) != 0)
		{
			os <<	"table{"
						"border-width: 1px;"
						"border-style: solid;"
						"border-collapse: collapse;"
					"}"
					"td,th{"
						"border-width: 1px;"
						"border-style: solid;"
						"padding: 3px 7px 2px 7px;"
					"}"
					"th#t1{"
						"border-width: 0px;"
						"border-style: hidden solid solid hidden;"
					"}";
		}

		if ((flags & UseRgbColorStyle) != 0)
		{
			os <<	"r1{color:red}"
					"g1{color:green}"
					"b1{color:blue}";
		}

		if (pCustomStyle != nullptr)
		{
			os <<	pCustomStyle;
		}

		os <<	"</style>";
	}

	os <<	"</head>"
			"<body>";
}

void CWebDiagnostics::WriteHtmlClose(std::ostream& os)
{
	os << "</body>";
}

void CWebDiagnostics::SaveCardReaderData(
	std::ostream& os,
	const CWebDiagnosticsCommonInfo &commonInfo,
	const CMCRWrapper* pMcr)
{
	WriteHtmlHeader(os, CardreaderInfoRefreshSeconds, "Card Reader Info", UseTableStyle | UseRgbColorStyle);
	commonInfo.WriteHtml(os);

	if (pMcr != nullptr)
	{
		pMcr->WriteHtml(os);
	}

	WriteHtmlClose(os);
}

static const char* pColorTableHdr1 =
	"<table>"
	"<tr>"
		"<th id=\"t1\" colspan=\"4\">";

static const char* pColorTableHdr2 =
	"</th>"
		"<th colspan=\"2\"><r1>Red 1</r1></th>"
		"<th colspan=\"2\"><g1>Green 1</g1></th>"
		"<th colspan=\"2\"><b1>Blue 1</b1></th>"
		"<th colspan=\"2\"><r1>Red 2</r1></th>"
		"<th colspan=\"2\"><g1>Green 2</g1></th>"
		"<th colspan=\"2\"><b1>Blue 2</b1></th>"
	"</tr>"
	"<tr>"
		"<th width=\"70\" align=\"left\">";

static const char* pColorTableHdr3 =
		"</th>"
		"<th width=\"100\" align=\"left\">Color</th>"
		"<th>ID</th>"
		"<th>Rate</th>"
		"<th><r1>Low</r1></th>"
		"<th><r1>High</r1></th>"
		"<th><g1>Low</g1></th>"
		"<th><g1>High</g1></th>"
		"<th><b1>Low</b1></th>"
		"<th><b1>High</b1></th>"
		"<th><r1>Low</r1></th>"
		"<th><r1>High</r1></th>"
		"<th><g1>Low</g1></th>"
		"<th><g1>High</g1></th>"
		"<th><b1>Low</b1></th>"
		"<th><b1>High</b1></th>"
	"</tr>";

static const char* pColorTableClose =
	"</table>";

static void WriteColorTableHdr(ostream& os, const char* pTableName, const char* pColumn1Hdr)
{
	os << pColorTableHdr1 << pTableName << pColorTableHdr2 << pColumn1Hdr << pColorTableHdr3;
}

static void WriteColorTableClose(ostream& os)
{
	os << pColorTableClose;
}

void CMCRWrapper::WriteHtml(ostream& os) const
{
	os <<
		"Card Reader Info:"
		"<div style=\"margin-left:6px;\">"
		"Current Bezel Style: " << CardStyleEnumMap.At(m_currentBezelStyle, "Invalid", false) << "<br>"
		"Current Rank Id: " << (int) m_currentRankId << "<br>"
		"Card is Inserted: " << CWebDiagnostics::GetBoolStringA(m_bInserted) << "<br>"
		"Track1 Data is Available: " << CWebDiagnostics::GetBoolStringA(m_track1DataIsAvailable) << "<br>"
		"Track2 Data is Available: " << CWebDiagnostics::GetBoolStringA(m_track2DataIsAvailable) << "<br>"
		"RFID Data is Available: " << CWebDiagnostics::GetBoolStringA(m_rfidDataIsAvailable) << "<br>"
		"Error Opening: " << CWebDiagnostics::GetBoolStringA(m_bErrorOpening) << "<br>";

		//byte m_universePropCodes[SIZE_OF_VALID_CARDS_PREFIX][SIZE_OF_VALID_PROP_CODE];

	os <<
		"Reconnect Error Count: " << m_reconnectErrorCount << "<br>"
		"Insert Remove Error Reported: " << CWebDiagnostics::GetBoolStringA(m_insertRemoveErrorReported) << "<br>"
		"Find Card Reader Failed Reported: " << CWebDiagnostics::GetBoolStringA(m_findCardReaderFailedReported) << "<br>"
		"Error Opening MCR Driver Reported: " << CWebDiagnostics::GetBoolStringA(m_errorOpeningMCRDriverReported) << "<br>"
		"Could Not Set Bezel Color Reported: " << CWebDiagnostics::GetBoolStringA(m_couldNotSetBezelColorReported) << "<br>"
		"</div>";

	m_cardReaderLayer.WriteHtml(os);
}

void CCardReaderLayer::WriteHtml(ostream& os) const
{
	unique_lock<recursive_mutex> cs(m_cardReaderConfigCriticalSection);
	shared_ptr<CardReaderConfig> cardReaderConfig = m_cardReaderConfig;
	shared_ptr<CardReader> cardReader = m_cardReader;
	cs.unlock();

	os << "<br>";
	if (cardReader)
	{
		cardReader->WriteHtml(os, m_cardReadersPathname);
	}
	else
	{
		os << "Current Card Reader: <r1>Invalid</r1><br>";
	}

	os << "<br>";
	if (cardReaderConfig)
	{
		cardReaderConfig->WriteHtml(os);
	}
	else
	{
		os << "Card Reader Colors: <r1>Invalid</r1><br>";
	}
}

void CardReaderConfig::WriteHtml(ostream& os) const
{
	os << "Card Reader Colors:<br>"
		"<div style=\"margin-left:6px;\">"
		"CardReaderConfig.xml File: " << m_pathname << "<br>"
		"<br>";

	WriteColorTableHdr(os, "Card Type Colors", "Type");

	for (size_t i = 0; i < m_cardTypeColors.size(); i++)
	{
		m_cardTypeColors[i]->WriteHtml(os, CardReaderCardType::emap.At((CardReaderCardType::Type) i, "Unknown", false));
	}

	WriteColorTableClose(os);

	if (m_rankColors.size() > 0)
	{
		os << "<br>";
		WriteColorTableHdr(os, "Rank Colors", "Rank");

		for_each(m_rankColors.cbegin(), m_rankColors.cend(),
			[&](const pair<int, shared_ptr<CardReaderColor>>& color)
		{
			stringstream s;
			s << color.first;

			color.second->WriteHtml(os, s.str().c_str());
		});
	}

	WriteColorTableClose(os);
	
	os << "</div>";
}

void CardReaderColor::rgb::WriteHtml(ostream& os) const
{
	os << "<td><r1>" << (unsigned) r.lsb << "</r1></td>"
		  "<td><r1>" << (unsigned) r.msb << "</r1></td>"
		  "<td><g1>" << (unsigned) g.lsb << "</g1></td>"
		  "<td><g1>" << (unsigned) g.msb << "</g1></td>"
		  "<td><b1>" << (unsigned) b.lsb << "</b1></td>"
		  "<td><b1>" << (unsigned) b.msb << "</b1></td>";
}

void CardReaderColor::WriteHtml(ostream& os, const char* pType) const
{
	os << "<tr>"
		"<td>" << pType << "</td>"
		"<td>" << name << "</td>"
		"<td>" << id << "</td>"
		"<td>" << (unsigned) rate << "</td>";

	color1.WriteHtml(os);
	color2.WriteHtml(os);

	os << "</tr>";
}

void CardReader::WriteHtml(ostream& os, const std::string& pathname) const
{
	os << "Current Card Reader:"
		"<div style=\"margin-left:6px;\">" <<
			"CardReaders.xml File: " << pathname << "<br>"
			"Type: " << CardReaderType::emap.At(m_type, "Unknown", false) << "<br>"
			"Vendor: " << m_vendor << "<br>"
			"Vendor ID: 0x" << std::hex << m_vendorId << std::dec << "<br>"
			"Product ID: 0x" << std::hex << m_productId << std::dec << "<br>"
		"</div>";
}
