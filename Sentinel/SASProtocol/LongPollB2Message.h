#pragma once
#include "SASPollMessage.h"

#define XB2_RESPONSE_EXTRADATASIZE 5
#define XB2_LENGTHBYTE 2
#define XB2_LENGTHSIZE 1

class CLongPollB2Message : public CSASPollMessage
{
public:
	CLongPollB2Message(byte gameAddress);
public:
	~CLongPollB2Message(void);

	byte m_numberDenoms;
	
	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();
	void ResponseReceived(void);
	void Fill();
	void SetEnabledDenomList(byte *enabledDenomList, byte enabledDenomList_LenMax);

private:
	void AdjustForDuplicates();

private:
	byte *m_enabledDenomList;
	byte m_enabledDenomList_LenMax; // Same type as m_numberDenoms.




	
};
