#pragma once
#include "PollerMessage.h"

class CPollerGlobalPasswordMessage :
	public CPollerMessage
{
public:
	CPollerGlobalPasswordMessage(void);
public:
	~CPollerGlobalPasswordMessage(void);

	WORD GetPassword( void );
};
