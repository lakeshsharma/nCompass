#pragma once
#include "SASPollMessage.h"
#include "ProgressiveProtocol/ProgressiveLevelForward.h"

namespace SentinelNativeUnitTests { class LongPoll80Message; } // Declare for test access.

/// <summary>
/// Class for creating SAS long poll 80 messages and parsing responses.
/// </summary>
class CLongPoll80Message :
	public CSASPollMessage
{
	friend class SentinelNativeUnitTests::LongPoll80Message; // Grant access for testing.

public:
	static const BYTE POLL_VALUE = 0x80;

	static const int LEVEL_COUNT = 1;
	typedef BYTE GROUP_TYPE;
	typedef BYTE LEVEL_TYPE;

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll80Message"/> class.
	/// </summary>
	/// <param name="gameAddress">IN - The game address.</param>
	/// <param name="progressiveGroup">IN - The progressive group.</param>
	/// <param name="levelRecord">IN - A progressive level record for a single level.</param>
	CLongPoll80Message(byte gameAddress, int progressiveGroup, const CProgressiveLevelForward::Level &levelRecord);

	/// <summary>
	/// Finalizes an instance of the <see cref="CLongPoll80Message"/> class.
	/// </summary>
	virtual ~CLongPoll80Message(void);

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
	static const UINT GROUP_OFFSET = 2;
	static const UINT GROUP_SIZE = sizeof(GROUP_TYPE);

	static const UINT LEVEL_OFFSET = 3;
	static const UINT LEVEL_SIZE = sizeof(LEVEL_TYPE);

	static const UINT AMOUNT_OFFSET = 4;
	static const UINT AMOUNT_SIZE = 5;

	static const UINT POLL_SIZE = 11; // Address, Poll, Group, Level, 5 for amount, 2 for CRC.

	static const UINT RESPONSE_SIZE = 1;
	static const UINT ACK_OFFSET = 0;

	const int m_progressiveGroup;
	const CProgressiveLevelForward::Level &m_levelRecord;
};
