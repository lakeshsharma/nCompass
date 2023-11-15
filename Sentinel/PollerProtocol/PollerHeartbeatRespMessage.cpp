#include "stdafx.h"
#include "PollerHeartbeatRespMessage.h"

CPollerHeartbeatRespMessage::CPollerHeartbeatRespMessage(WORD  packetVersion)
{
	m_packetVersion = packetVersion;
	m_bufferSize = sizeof(HeartbeatResp().mmtUp) + sizeof(HeartbeatResp().pmtUp) + sizeof(HeartbeatResp().pollerID);

	if (packetVersion >= PACKET_VERSION_MULTI_GAME_MULTI_DENOM)
		m_bufferSize += sizeof(HeartbeatResp().voucherServiceUp);
	
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerHeartbeatRespMessage::~CPollerHeartbeatRespMessage(void)
{
}

byte CPollerHeartbeatRespMessage::GetPollerID()
{
	HeartbeatResp resp;
	int offset = 0;

	memcpy_s(&resp.pollerID, sizeof(resp.pollerID), m_lpBuffer+offset, sizeof(resp.pollerID));

	return resp.pollerID;
}

bool CPollerHeartbeatRespMessage::IsPMTUp()
{
	int offset = sizeof(HeartbeatResp().pollerID);
	byte pmtUp;
	memcpy_s(&pmtUp, sizeof(pmtUp), m_lpBuffer+offset, sizeof(pmtUp));
	return pmtUp & PMT_UP;
}

bool CPollerHeartbeatRespMessage::IsMMTUp()
{
	HeartbeatResp resp;
	int offset = sizeof(resp.pollerID) + sizeof(resp.pmtUp);

	memcpy_s(&resp.mmtUp, sizeof(resp.mmtUp), m_lpBuffer + offset, sizeof(resp.mmtUp));

	return resp.mmtUp;
}

bool CPollerHeartbeatRespMessage::IsPMTTransactionsUp()
{
	int offset = sizeof(HeartbeatResp().pollerID);
	byte pmtUp;
	memcpy_s(&pmtUp, sizeof(pmtUp), m_lpBuffer+offset, sizeof(pmtUp));
	return IsPMTUp() && (pmtUp & PMT_TRANSACTION_UP) == 0; 
}

bool CPollerHeartbeatRespMessage::IsVoucherServiceUp()
{
	bool retValue = false;
	if (m_packetVersion >= PACKET_VERSION_MULTI_GAME_MULTI_DENOM)
	{
		HeartbeatResp resp;
		int offset = sizeof(resp.pollerID) + sizeof(resp.pmtUp) + sizeof(resp.mmtUp);
		
		memcpy_s(&resp.voucherServiceUp, sizeof(resp.voucherServiceUp), m_lpBuffer + offset, sizeof(resp.voucherServiceUp));
		
		retValue = resp.voucherServiceUp;

	}
	return retValue;
}


