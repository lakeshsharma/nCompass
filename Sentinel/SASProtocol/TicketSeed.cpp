#include "stdafx.h"
#include "TicketSeed.h"

const int NCOMPASSBYTE = 2;
const int UNUSEDBYTE = 3;
const int SEEDRESETINCREMENT = 3;

CTicketSeed::CTicketSeed(DWORD randSeed)
{
	srand(randSeed);
	m_ticketSequenceNumber = 0;
}


CTicketSeed::~CTicketSeed(void)
{
}

void CTicketSeed::IncAndRandSequence(byte *sequence,  bool resetSequence )
{

	if (resetSequence)
	{
		// start with previous sequence # if there was one
		if (m_ticketSequenceNumber != 0)
		{
			memcpy(sequence, &m_ticketSequenceNumber, sizeof(m_ticketSequenceNumber));
		}
		else
		{
			// first time for game and nCompass, use new random number
			WORD tmpRandLSB = rand();
			memcpy(sequence, &tmpRandLSB, sizeof tmpRandLSB);
		}
	}

	// Increment game portion (LSB) of sequence number
	WORD *pGameSeq = (WORD *)sequence;
	WORD gameSeq = *pGameSeq;
	if ( resetSequence )
	{
		// Increment sequence # by 2 ( 2 printed tickets ) plus usual 1 when resetting validation sequence number
		gameSeq += SEEDRESETINCREMENT;
	}
	else
	{
		gameSeq++;
	}
	*pGameSeq = gameSeq;

	// Randomize final byte
	WORD tmpRandMSB = rand();
	memcpy(&sequence[NCOMPASSBYTE], &tmpRandMSB, sizeof( byte ));
	sequence[UNUSEDBYTE] = 0;

	// Keep dereferenced value
	m_ticketSequenceNumber = *(DWORD *)sequence;
}

DWORD CTicketSeed::GetMachineID( long slotmastID, byte casinoNumber)
{
	// calculate machine id ( validation id) and sequence #
	const int SLOTMASTSHIFT = 14;
	const int BEGRANGE = 651; // Beginning of range assigned to Aristocrat
	const int SIZEOFRANGE = 50;
	const int RANGESHIFT = 0x4000;
	const int WORKINGPORTION = RANGESHIFT - 1;
	DWORD machineID;
	DWORD temp;

	temp = (casinoNumber ^ (slotmastID >> SLOTMASTSHIFT)) % SIZEOFRANGE;
	temp += BEGRANGE;
	temp *= RANGESHIFT;
	temp = temp | (WORKINGPORTION & slotmastID);
	machineID = temp;
	return machineID;

}
