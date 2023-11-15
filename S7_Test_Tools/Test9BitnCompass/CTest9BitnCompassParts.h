
#pragma once
//#pragma pack(1)

#include "SASProtocol/SASSerialWrapper.h"
#include "SASProtocol/LongPoll19Message.h"
#include "SASProtocol/SASGeneralPoll.h"
#include "SASProtocol/SASPollMessage.h"
#include "GlobalDefs.h"

class CTest9BitnCompassParts
{
public:
	CTest9BitnCompassParts();
	~CTest9BitnCompassParts();
	S7_Result initialize(int delay);
	S7_Result RunTest(bool runAsEsg, bool runAsSentinel);
	S7_Result finalize();
	void StartSentinel();
	void DoGeneralPoll();
	void StartEGM();
	void DisplayPollCommand(DWORD pollCommand);
	void InitMeterValues();

	BOOL StartTimer();
	void StopTimer();

	CSASSerialWrapper m_serial;
	byte m_gameAddress;
	timer_t m_nTimer;

	bool m_IsStartSentinel;
	bool m_IsStartEGM;
	bool m_IsUARTReady;
	bool m_RunLoop;

	DWORD coinIn, coinOut, totalDrop, jackpot, gamesPlayed;

private:
	short m_pollingSpeed;
	long m_DelayEdit;

	// Linux timer specifics
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    long long freq_nanosecs;
    sigset_t mask;
    struct sigaction sa;
};

