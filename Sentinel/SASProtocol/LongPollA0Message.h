#pragma once
#include "SASPollMessage.h"

class CLongPollA0Message :
	public CSASPollMessage
{
public:
	CLongPollA0Message(byte gameAddress);
public:
	~CLongPollA0Message(void);

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetGameNumber(int gameNumber);

	bool m_bJackpotMultiplier;
	bool m_bAFTBonusAwards;
	bool m_bLegacyBonusAwards;
	bool m_bTournament;
	bool m_bValidationExtensions;
	byte m_cValidationStyle;
	bool m_bTicketRedemption;
	byte m_cMeterModelFlag;
	bool m_bTicketsToTotalDrop;
	bool m_bExtendedMeters;
	bool m_bComponentAuthentication;
	bool m_bAFT;
	bool m_bMultiDenomExtensions;
	bool m_bMaximumPollingRate; // 1 = 40 ms (only gauranteed to be set with SAS 6.01)
	bool m_bMultipleSASProgressiveWinReporting;

private:
		int m_gameNumber;

};
