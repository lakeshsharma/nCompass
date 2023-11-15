#pragma once
#include "SASPollMessage.h"

class CLongPoll4CMessage :
	public CSASPollMessage
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll4CMessage"/> class.
	/// </summary>
	/// <param name="gameAddress">The game address.</param>
	CLongPoll4CMessage(byte gameAddress);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll4CMessage"/> class.
	/// </summary>
	~CLongPoll4CMessage(void);

	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Sets the response received event and updates member variables.
	/// </summary>
	void ResponseReceived(void);

	/// <summary>
	/// Sets the send buffer according to the previously specified instance values.
	/// </summary>
	void Fill();

	/// <summary>
	/// Sets the machine validation ID.
	/// </summary>
	/// <param name="validationID">The validation ID.  Only the lower 24 bits are used.</param>
	void SetMachineValidationID( DWORD validationID );

	/// <summary>
	/// Sets the validation sequence number.
	/// </summary>
	/// <param name="sequenceNumber">The sequence number.  Only the lower 24 bits are used.</param>
	void SetValidationSequenceNumber( DWORD sequenceNumber );

	/// <summary>
	/// Gets the machine validation ID.
	/// </summary>
	/// <returns>The machine validation ID.</returns>
	DWORD GetMachineValidationID( void ) const;

	/// <summary>
	/// Gets the validation sequence number.
	/// </summary>
	/// <returns>The validation sequence number.</returns>
	DWORD GetValidationSequenceNumber( void ) const;

private:
	static const BYTE POLL_VALUE = 0x4C;
	static const size_t RESPONSE_BUFFER_SIZE = 10;
	static const size_t SEND_BUFFER_SIZE = 10;

	static const size_t MACHINE_VALIDATION_ID_OFFSET = POLL_VALUE_OFFSET + POLL_VALUE_SIZE;
	static const size_t MACHINE_VALIDATION_ID_SIZE = 3;
	static const size_t VALIDATION_SEQUENCE_NUMBER_OFFSET = MACHINE_VALIDATION_ID_OFFSET + MACHINE_VALIDATION_ID_SIZE;
	static const size_t VALIDATION_SEQUENCE_NUMBER_SIZE = 3;
	static const size_t CRC_OFFSET = VALIDATION_SEQUENCE_NUMBER_OFFSET + VALIDATION_SEQUENCE_NUMBER_SIZE;

	DWORD m_machineValidationID;
	DWORD m_validationSequenceNumber;
};
