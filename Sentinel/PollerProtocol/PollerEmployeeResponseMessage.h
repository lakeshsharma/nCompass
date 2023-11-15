#pragma once
#include "PollerMessage.h"

typedef struct _employeeResp
{
	byte card_id[CARD_ID_LEN];
	WORD EncryptedPIN;
} EmployeeResponse;

class CPollerEmployeeResponseMessage :
	public CPollerMessage
{
public:
	CPollerEmployeeResponseMessage(const byte *response);
public:
	~CPollerEmployeeResponseMessage(void);
	DWORD GetPIN();
	byte* GetCardID();

private:
	EmployeeResponse m_buffer;
};
