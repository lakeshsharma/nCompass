#pragma once

#include "SASPollMessage.h"

class CLongPoll54Message : public CSASPollMessage
{
private:
	static const UINT GAME_ADDRESS_LOCATION = 0;
	static const UINT POLL_VALUE_LOCATION = 1;
	static const UINT POLL_VALUE = 0x54;
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll54Message"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	CLongPoll54Message(byte gameAddress);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll54Message"/> class.
	/// </summary>
	~CLongPoll54Message(void);

	char version[3];
	// Gaming machine serial number field is 0 to 40 bytes of ASCII.
	// If used, need to the determine the length provided by the EGM.

	virtual BYTE GetPollValue();
	virtual bool SwapPollValue();

	/// <summary>
	/// Processes the response received.
	/// </summary>
	void ResponseReceived(void);

	void Fill();

	/// <summary>
	/// Gets the serial number.
	/// </summary>
	/// <returns></returns>
	std::string GetSerialNumber() const;

private:
	std::string m_serialNumber;
};
