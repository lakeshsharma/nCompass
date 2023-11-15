#include "stdafx.h"
#include "MetersLongPoll.h"


CMetersLongPoll::CMetersLongPoll(byte gameAddress) :
CSASPollMessage(gameAddress)
{
	m_meters = nullptr;
}


CMetersLongPoll::~CMetersLongPoll()
{
	// CMetersLongPoll class will not own m_meters.
}

CMeters *CMetersLongPoll::GetMeters()
{
//TODO	ASSERT(m_meters != nullptr);
	return m_meters;
}

