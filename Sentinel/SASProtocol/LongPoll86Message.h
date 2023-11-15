#pragma once
#include "SASPollMessage.h"
#include "ProgressiveProtocol/ProgressiveLevelForward.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class LongPoll86Message; }

/// <summary>
/// Class for creating SAS long poll 86 messages and parsing responses.
/// This message was not present in SAS 4.00, but was present in SAS 4.03.
/// </summary>
class CLongPoll86Message :
	public CSASPollMessage
{
	// Grant "friend" access for testing.
	friend class SentinelUnitTests::LongPoll86Message;

public:
	static const BYTE POLL_VALUE = 0x86;

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll86Message"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	/// <param name="levelInfo">IN/OUT - The progressive level information.</param>
	CLongPoll86Message(byte gameAddress, const CProgressiveLevelForward &levelInfo);

	/// <summary>
	/// Finalizes an instance of the <see cref="CLongPoll86Message"/> class.
	/// </summary>
	virtual ~CLongPoll86Message(void);

	/// <summary>
	/// Gets the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Indicates if Fill(DWORD tickCount) needs to be called (immediately) before sending the message.
	/// Indicates if the message contains time-sensitive information that needs to be discarded if stale or expired.
	/// </summary>
	/// <returns>true if Fill(DWORD tickCount) needs to be called (immediately) before sending the message, false otherwise.</returns>
	/// <remarks>
	/// After calling Fill(DWORD tickCount), if there is nothing to send then GetSendBufferSize() will return 0.
	/// </remarks>
	virtual bool FillWithTickCountBeforeSending();

	/// <summary>
	/// Fills the message after checking if stale or expired time-sensitive information needs to be discarded.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <remarks>
	/// After calling this method, if there is nothing to send then GetSendBufferSize() will return 0.
	/// </remarks>
	virtual void Fill(DWORD tickCount);

	/// <summary>
	/// Does nothing. This class uses Fill(DWORD tickCount) instead.
	/// </summary>
	virtual void Fill();

	/// <summary>
	/// Parses the response and signals the response event.
	/// </summary>
	void ResponseReceived();

private:
	typedef BYTE GROUP_TYPE;
	static const UINT GROUP_ID_OFFSET = 3;

	typedef BYTE LEVEL_TYPE;
	static const int MAX_LEVEL_COUNT = 32;

	static const UINT MIN_POLL_SIZE = 6;
	static const UINT LEVEL_SIZE = sizeof(LEVEL_TYPE);
	static const UINT AMOUNT_SIZE = 5;
	static const UINT MAX_POLL_SIZE = MIN_POLL_SIZE + MAX_LEVEL_COUNT * (LEVEL_SIZE + AMOUNT_SIZE);
	static const UINT NON_DATA_LENGTH = 5;

	static const UINT RESPONSE_SIZE = 1;
	static const UINT ACK_OFFSET = 0;

	const CProgressiveLevelForward m_levelInfo;
};
