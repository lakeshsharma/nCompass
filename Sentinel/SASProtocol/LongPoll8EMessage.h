#pragma once

#include "SASPollMessage.h"
#include "PokerHand.h"

/// <summary>
/// Class for creating SAS long poll 8E messages and parsing responses.
/// </summary>
class CLongPoll8EMessage :
	public CSASPollMessage
{
public:
	typedef enum
	{
		// Values defined by SAS.
		DealtHand = 0,
		FinalHand = 1
	} PokerHandType;

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll8EMessage"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	/// <param name="handType">IN - The type of hand required.</param>
	CLongPoll8EMessage(byte gameAddress, PokerHandType pokerHandType);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll8EMessage"/> class.
	/// </summary>
	~CLongPoll8EMessage();
	
	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Processes the response received.
	/// </summary>
	/// <remarks>
	/// This method should only be called once per instance.
	/// </remarks>
	void ResponseReceived(void);

	/// <summary>
	/// Fills the transmit buffer.
	/// </summary>
	void Fill();

	/// <summary>
	/// Gets the poker hand.
	/// </summary>
	/// <returns>The poker hand.</returns>
	/// <remarks>
	/// The poker hand is invalid if the hand does not match the hand type required.
	/// </remarks>
	PokerHand GetPokerHand(); 

private:
	static const BYTE COMMAND;
	static const UINT SENDBUFFERLENGTH;
	static const UINT RECBUFFERLENGTH;
	static const UINT HAND_TYPE_LOCATION;
	static const UINT CARDLOC;

	PokerHandType m_pokerHandType;
	PokerHand m_pokerHand;

	friend class LongPoll8EMessageTests; // Grant access for testing.
	FRIEND_TEST(LongPoll8EMessageTests, ResponseReceived_DealtHandFalse);
};
