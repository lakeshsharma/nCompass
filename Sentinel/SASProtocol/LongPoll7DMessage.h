#pragma once
#include "SASPollMessage.h"

class CLongPoll7DMessage :
	public CSASPollMessage
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll7DMessage"/> class.
	/// </summary>
	/// <param name="gameAddress">The game address.</param>
	CLongPoll7DMessage(byte gameAddress);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll7DMessage"/> class.
	/// </summary>
	~CLongPoll7DMessage(void);

	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Sets the send buffer according to the previously specified instance values.
	/// </summary>
	void Fill();

	/// <summary>
	/// Sets the host ID.
	/// </summary>
	/// <param name="hostId">The host id.</param>
	void SetHostID(WORD hostId);

	/// <summary>
	/// Sets the expiration.
	/// </summary>
	/// <param name="expiration">The expiration.</param>
	void SetExpiration(byte expiration);

	/// <summary>
	/// Sets the location (casino name) and address data.
	/// </summary>
	/// <param name="location">The location (casino name).</param>
	/// <param name="locationLength">Length of the location. Only the first MAXIMUM_LOCATION_LENGTH bytes are used.</param>
	/// <param name="address1">The address1.</param>
	/// <param name="address1Length">Length of the address1. Only the first MAXIMUM_ADDRESS1_LENGTH bytes are used.</param>
	/// <param name="address2">The address2.</param>
	/// <param name="address2Length">Length of the address2. Only the first MAXIMUM_ADDRESS2_LENGTH bytes are used.</param>
	void SetAddressData(const byte *location,
		byte locationLength,
		const byte *address1,
		byte address1Length,
		const byte *address2,
		byte address2Length);
	
private:
	const static BYTE POLL_VALUE = 0x7D;

	WORD m_hostID;
	byte m_expiration;
	byte m_locationLength;
	byte *m_location;
	byte m_address1Length;
	byte *m_address1;
	byte m_address2Length;
	byte *m_address2;

};
