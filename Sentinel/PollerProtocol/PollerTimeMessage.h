#pragma once
#include "PollerMessage.h"

typedef struct
{
	SYSTEMTIME sysTime;
	byte useDST;
	WORD timeZoneOffset;
}PollerTime;

class CPollerTimeMessage :
	public CPollerMessage
{
public:
	CPollerTimeMessage(void);
public:
	~CPollerTimeMessage(void);

	SYSTEMTIME * GetTime( void );
};
