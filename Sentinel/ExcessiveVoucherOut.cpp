#include "stdafx.h"
#include "ExcessiveVoucherOut.h"
#include "Utilities.h"
#include "Logging/LogString.h"

using namespace std;

CExcessiveVoucherOut::CExcessiveVoucherOut() :
m_globalExcessiveVoucherOutEnabledFlag(false),
m_excessiveVoucherOutThreshold(0),
m_totalBillInThreshold(0),
m_coinInThreshold(0),
m_coinOutThreshold(0),
m_currentBillInPennies(0),
m_currentCoinInPennies(0),
m_currentCoinOutPennies(0)
{	
}

CExcessiveVoucherOut::~CExcessiveVoucherOut()
{
}

string CExcessiveVoucherOut::GetHtmlString() const
{
	string html;

	html.append(_T("Excessive Voucher Out<br>"));
	html.append(FormatString(_T("Global Excessive Voucher Out Enabled Flag: %s<br>"), m_globalExcessiveVoucherOutEnabledFlag ? "Yes" : "No"));
	html.append(FormatString(_T("Excessive Voucher Out Threshold (pennies): %u<br>"), m_excessiveVoucherOutThreshold));
	html.append(FormatString(_T("Total Bill In Threshold (pennies): %u<br>"), m_totalBillInThreshold));
	html.append(FormatString(_T("Coin In Threshold (pennies): %u<br>"), m_coinInThreshold));
	html.append(FormatString(_T("Coin Out Threshold (pennies): %u<br>"), m_coinOutThreshold));
	html.append(FormatString(_T("Current Bill In (pennies): %u<br>"), m_currentBillInPennies));
	html.append(FormatString(_T("Current Coin In (pennies): %u<br>"), m_currentCoinInPennies));
	html.append(FormatString(_T("Current Coin Out (pennies): %u<br><br>"), m_currentCoinOutPennies));

	return html;
}

void CExcessiveVoucherOut::SetGlobalExcessiveVoucherOutEnabledFlag(bool enabled)
{
	m_globalExcessiveVoucherOutEnabledFlag = enabled;
}

void CExcessiveVoucherOut::SetExcessiveVoucherOutThreshold(DWORD threshold)
{
	m_excessiveVoucherOutThreshold = threshold;
}

void CExcessiveVoucherOut::SetTotalBillInThreshold(DWORD threshold)
{
	m_totalBillInThreshold = threshold;
}

void CExcessiveVoucherOut::SetCoinInThreshold(DWORD threshold)
{
	m_coinInThreshold = threshold;
}

void CExcessiveVoucherOut::SetCoinOutThreshold(DWORD threshold)
{
	m_coinOutThreshold = threshold;
}

void CExcessiveVoucherOut::ProcessBillIn(DWORD billInPennies)
{
	m_currentBillInPennies += billInPennies;
}

void CExcessiveVoucherOut::ProcessCoinIn(DWORD coinInPennies)
{
	if (m_currentBillInPennies > 0)
	{
		m_currentCoinInPennies += coinInPennies;
		if (m_currentCoinInPennies >= m_coinInThreshold)
		{
			ResetTrackingValues();
		}
	}
}

void CExcessiveVoucherOut::ProcessCoinOut(DWORD coinOutPennies)
{
	if (m_currentBillInPennies > 0)
	{
		m_currentCoinOutPennies += coinOutPennies;

		if (m_currentCoinOutPennies >= m_coinOutThreshold)
		{
			ResetTrackingValues();
		}
	}
}

bool CExcessiveVoucherOut::ProcessVoucherOut(DWORD amountInPennies, DWORD& currentBillInPennies)
{
	bool sendExcessiveTicketOut(false);

	LogString(GENERAL, _T("Excessive Voucher Out - ProcessVoucherOut m_globalExcessiveVoucherOutEnabledFlag=%d m_currentBillInPennies=%u"),
		(int)m_globalExcessiveVoucherOutEnabledFlag, m_currentBillInPennies);

	if (m_globalExcessiveVoucherOutEnabledFlag && m_currentBillInPennies > 0)
	{
		bool excessiveVoucherOut(amountInPennies >= m_excessiveVoucherOutThreshold);
		bool billInIsOverThreshold(m_currentBillInPennies >= m_totalBillInThreshold);
		bool coinInIsLessThanThreshold(m_currentCoinInPennies < m_coinInThreshold);
		bool coinOutIsLessThanThreshold(m_currentCoinOutPennies < m_coinOutThreshold);
		if (excessiveVoucherOut && billInIsOverThreshold && coinInIsLessThanThreshold && coinOutIsLessThanThreshold)
		{
			sendExcessiveTicketOut = true;
			currentBillInPennies = m_currentBillInPennies;
		}
		LogString(GENERAL, _T("Excessive Voucher Out - ProcessVoucherOut amountInPennies=%u m_excessiveVoucherOutThreshold=%u m_totalBillInThreshold=%u m_coinInThreshold=%u m_coinOutThreshold=%u sendExcessiveTicketOut=%d"),
			amountInPennies, m_excessiveVoucherOutThreshold, m_totalBillInThreshold, m_coinInThreshold, m_coinOutThreshold, (int)sendExcessiveTicketOut);
	}

	ResetTrackingValues();

	return sendExcessiveTicketOut;
}

void CExcessiveVoucherOut::ResetTrackingValues()
{
	m_currentBillInPennies = 0;
	m_currentCoinInPennies = 0;
	m_currentCoinOutPennies = 0;
}

void CExcessiveVoucherOut::GameCreditsBecameZero()
{
	if (m_currentBillInPennies > 0)
	{
		LogString(GENERAL, _T("Excessive Voucher Out - Game credits became zero, resetting tracking values."));
		ResetTrackingValues();
	}
}