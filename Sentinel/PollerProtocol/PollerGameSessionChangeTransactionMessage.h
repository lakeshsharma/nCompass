#pragma once
#include "PollerMessage.h"
#include "../Player.h"
#include "../Meters.h"
#include "../InternalMessage.h"

class CPollerGameSessionChangeTransactionMessage : public CPollerMessage
{
  public:
  	
    CPollerGameSessionChangeTransactionMessage(CMeters *meters, CPlayer *player, const byte *cardID, time_t *transTime, 
		DWORD uniqueId, DWORD psid, DWORD playTxSeqNum, int denom, int gameNumber, bool mgmdAvailable);
	
	/// <summary>
	/// Get the data from message buffer.
	/// </summary>
	/// <param name="bufferSize">OUT - The size of the buffer actually used.</param>
	/// <returns>Pointer the the message buffer.</returns>
    byte *GetReadWriteBuffer(DWORD &bufferSize);

  	/// <summary>
	/// Set the data into message buffer.
	/// </summary>
	/// <returns>None.</returns>
    void SetReadWriteBuffer();

	/// <summary>
	/// Sets the status in the message buffer.
	/// </summary>
 	/// <param name="status">IN - Status to be set.</param>
	/// <returns>None.</returns>
    void SetStatus(DWORD status);

	/// <summary>
	/// Gets the status from the message buffer.
	/// </summary>
	/// <returns>status from the message buffer.</returns>
    DWORD GetStatus();

	~CPollerGameSessionChangeTransactionMessage(void);

  private:
    CMeters *m_meters;
    CPlayer *m_player;
    const byte *m_cardID;

	time_t m_transTime;
    DWORD m_uniqueId, m_psid, m_playTxSeqNum;
    WORD m_denom, m_gameNumber;
    bool m_mgmdAvailable;

    byte m_meterCount;
    byte m_transCode;
    DWORD m_status;
	
	byte m_dataCount;

	static const byte FIELD_COUNT = 11;
};
