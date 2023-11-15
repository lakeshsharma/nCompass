#pragma once
#include "MetersLongPoll.h"
#include "../Meters.h"

const int X20_MAX_METERS = 1;
const int LP20 = 0x20;

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class LongPoll20Message; }

class CLongPoll20Message :
	public CMetersLongPoll
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll20Message"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	/// <param name="meters">IN/OUT - The meters instance. If not nullptr, should contain a meter for mBD.</param>
	/// <param name="denomMult">IN - The denom mult. Not used if meters==nullptr.</param>
	/// <remarks>
	/// The caller is responsible for the lifetime of "meters".
	/// The lifetime of "meters" should exceed the lifetime of this instance.
	/// </remarks>
	CLongPoll20Message(byte gameAddress, CMeters *meters = nullptr, WORD denomMult = INVALID_DENOM);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll20Message"/> class.
	/// </summary>
	~CLongPoll20Message(void);

	/// <summary>
	/// Gets the total bills, normally set by ResponseReceived().
	/// </summary>
	/// <returns>The total bills.</returns>
	__int64 GetTotalBills();

	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Processes a response for this message.
	/// </summary>
	void ResponseReceived(void);

	/// <summary>
	/// Fills the send buffer with the contents for this message.
	/// </summary>
	void Fill();

private:
	static const BYTE POLL_VALUE = 0x20;
	static const UINT POLL_SIZE = 2; // Address, Poll.

	static const UINT RESPONSE_SIZE = 8;

	static const size_t METER_VALUE_OFFSET = POLL_VALUE_OFFSET + POLL_VALUE_SIZE;
	static const size_t METER_VALUE_SIZE = 4;

	__int64 m_totalBills;
	CMeters *m_meters;
	WORD m_denomMult;

	// Grant "friend" access for testing.
	friend class SentinelUnitTests::LongPoll20Message;
};

