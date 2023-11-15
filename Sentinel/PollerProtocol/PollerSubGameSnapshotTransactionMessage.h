#pragma once
#include "PollerMessage.h"

class CPollerSubGameSnapshotTransactionMessage :
	public CPollerMessage
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CPollerSubGameSnapshotTransactionMessage"/> class.
	/// </summary>
	/// <param name="meters">The meters.</param>
	/// <param name="transType">IN - Type of the trans.</param>
	/// <param name="transTime">IN - Transaction timestamp.</param>
	/// <param name="uniqueId">IN - The unique identifier.</param>
	/// <param name="gameCommDown">IN - true if game comm. down, false otherwise.</param>
	/// <param name="gameNumber">IN - The game number.</param>
	/// <param name="subgameMeters">The subgame meters.</param>
	/// <param name="type">IN - The snapshot type.</param>
	/// <param name="cardID">IN - The card identifier.</param>
	/// <param name="manufacturerID">IN - The manufacturer identifier.</param>
	/// <param name="paytableID">IN - The paytable identifier.</param>
	/// <param name="additionalID">IN - The additional identifier.</param>
	CPollerSubGameSnapshotTransactionMessage(CMeters *meters,
		byte transType,
		SYSTEMTIME transTime,
		DWORD uniqueId,
		bool gameCommDown,
		WORD gameNumber,
		CMeters * subgameMeters,
		SnapshotType type,
		const byte * cardID,
		const char * manufacturerID,
		const char * paytableID,
		const char * additionalID);

public:
	~CPollerSubGameSnapshotTransactionMessage(void);

	byte * GetReadWriteBuffer( DWORD &bufferSize );
	void SetStatus(DWORD status);
	DWORD GetStatus();
	void SetReadWriteBuffer();

private:
	byte m_DataCount;

	CMeters *m_meters;
	CMeters *m_subgameMeters;
	const byte * m_cardID;
	byte m_meterCount;
	byte m_subgameMetersCount;
	byte m_transCode;
	DWORD m_uniqueId;
	DWORD m_status;
	bool m_bGameCommDown;
	WORD m_gameNumber;
	bool m_rwBufferIsSet;

	char * m_lpPaytableID;
	char * m_lpMfgID;
	char * m_lpAdditionalID;

	SnapshotType m_type;
	SYSTEMTIME m_transTime;
};
