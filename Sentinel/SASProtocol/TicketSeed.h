#pragma once

class CTicketSeed
{
public:
	CTicketSeed(DWORD randSeed);
	~CTicketSeed(void);

	/// <summary>
	/// Get updated ticket seed sequence number
	/// </summary>
	/// <param name="sequence">Original sequence number to update</param>
	/// <param name="machineID">Machine ID equal 0 will cause game portion of sequence number to be reset to previous value or random value if no previous value exists.</param>
	/// <returns></returns>
	void IncAndRandSequence(byte *sequence, bool resetSequence );

	/// <summary>
	/// Calculates Machine ID based on slotmastID and casinoNumber
	/// </summary>
	/// <param name="slotmastID">Slotmast ID</param>
	/// <param name="casinoNumber">Casino Number</param>
	/// <returns>Returns machine ID.</returns>
	DWORD GetMachineID(long slotmastID, byte casinoNumber);

private:
	byte m_sequence[ sizeof(DWORD) ];
	DWORD m_ticketSequenceNumber;

};
