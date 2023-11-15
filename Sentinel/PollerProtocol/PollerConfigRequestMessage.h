#pragma once
#include "PollerMessage.h"

class CPollerConfigRequestMessage :
	public CPollerMessage
{
public:
	/// <summary>
	/// Constructs a poller config request message
	/// </summary>
	/// <returns></returns>
	CPollerConfigRequestMessage(void);
public:
	~CPollerConfigRequestMessage(void);

	/// <summary>
	/// Initializes read/write buffer
	/// </summary>
	/// <param name="bufferSize"></param>
	/// <returns>Poller message memory buffer</returns>
	byte * GetReadWriteBuffer( long &bufferSize );

};
