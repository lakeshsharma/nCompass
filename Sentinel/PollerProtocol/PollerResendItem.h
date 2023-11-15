#pragma once
#include "PollerMessage.h"

enum ResendMessageType
{
	PBT_MESSAGE,
	TICKET_MESSAGE,
	NONE
};

class CPollerResendItem
{
public:
	CPollerResendItem();
public:
	~CPollerResendItem(void);

public:
	void SetContent(CPollerMessage *msg, ResendMessageType);
	CPollerMessage *GetContent(void);

	time_t GetTimeStamp(void);
	time_t GetTimeoutTime(void);
	void ResetTime(void);
	void SetResendPeriod(int);
	ResendMessageType GetType(void);

private:
	CPollerMessage*		m_pollerMsg;
	time_t	m_tTimeStamp;
	int			m_nResendPeriod;
	ResendMessageType m_rType;
	
};
