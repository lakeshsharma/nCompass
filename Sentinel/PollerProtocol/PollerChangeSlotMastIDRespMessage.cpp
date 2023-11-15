#include "stdafx.h"
#include "PollerChangeSlotMastIDRespMessage.h"

CPollerChangeSlotMastIDRespMessage::CPollerChangeSlotMastIDRespMessage(void)
{
	m_bufferSize = sizeof(ChangeSlotMastResp().Response) + sizeof(ChangeSlotMastResp().slotMastID);
	m_lpBuffer = new byte[m_bufferSize];
}

CPollerChangeSlotMastIDRespMessage::~CPollerChangeSlotMastIDRespMessage(void)
{
}

SlotmastIDResponse CPollerChangeSlotMastIDRespMessage::SlotMastChangeResponse( void )
{
	ChangeSlotMastResp response;
	memcpy_s(&(response.Response), sizeof(response.Response), m_lpBuffer + sizeof(response.slotMastID), sizeof(response.Response));
	return ((SlotmastIDResponse)response.Response);
}

DWORD CPollerChangeSlotMastIDRespMessage::GetSlotMastID( void )
{
	ChangeSlotMastResp response;
	memcpy_s(&response.slotMastID, sizeof(response.slotMastID), m_lpBuffer, sizeof(response.slotMastID));
	return (response.slotMastID);
}
