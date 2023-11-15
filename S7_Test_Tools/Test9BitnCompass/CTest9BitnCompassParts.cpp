
#include "stdafx.h"
#include "Test9BitnCompass.h"
#include "CTest9BitnCompassParts.h"

const int EVENT_EGM_POLL_TIMEOUT = 40; //miliseconds
const int EVENT_WAIT_TIMEOUT = 5000; //miliseconds

CTest9BitnCompassParts::CTest9BitnCompassParts()
{
	// start polling at machine address 0x01.
	m_gameAddress = 1;
	m_pollingSpeed = 40;
	m_IsStartSentinel = false;
	m_IsStartEGM = false;
	m_IsUARTReady = false;
	m_RunLoop = false;
}

CTest9BitnCompassParts::~CTest9BitnCompassParts()
{

}

S7_Result CTest9BitnCompassParts::initialize(int delay)
{
	char eMessage[57];

	S7_Result result = -1;
	m_DelayEdit = delay;
	m_nTimer = 0;

	// Initialize the DLL
	result = S7LITE_DLL_Init();
	if (result != S7DLL_STATUS_OK)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_Init returned [%d]", result);
		prtf((char *)"ERROR: %s\n", (char *)"b", eMessage);
	}

	return result;
}


S7_Result CTest9BitnCompassParts::RunTest(bool runAsEsg, bool runAsSentinel)
{
	S7_Result result = 1;
	if(!m_IsUARTReady)
	{
		// Initialize the Uart
		prtf((char *)"Initializing UART\n", (char *)"b");
        m_serial.OpenSAS();
		result = m_serial.OpenSAS();
		if(result == S7DLL_STATUS_ERROR)
		{
			m_serial.CloseSAS();
			prtf((char *)"ERROR: Open SAS Error\n", (char *)"b");
			return result;
		}
		
		if (result == S7DLL_STATUS_OK)
		{
			m_IsUARTReady = true;
		}
		else
		{
			return result;
		}
	}

	if (runAsEsg) 
	{
		m_IsStartSentinel = false;
		m_IsStartEGM = true;
		m_RunLoop = true;
		//Initialize Meter values for EGM
		InitMeterValues();
		StartEGM();
	} 
	else if (runAsSentinel) 
	{
		m_IsStartSentinel = true;
		m_IsStartEGM = false;
		m_RunLoop = true;
		DoGeneralPoll();
		StartSentinel();
	}

	while (m_RunLoop)
	{
		StartTimer();
		sleep(1);
	}

	return result;
}

S7_Result CTest9BitnCompassParts::finalize()
{
	char eMessage[57];
	S7_Result result = -1;
	if ((result = S7LITE_DLL_DeInit()) != 0)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_DeInit returned [%d]", result);
		prtf((char *)"ERROR: %s\n", (char *)"b", eMessage);
	}
	return result;
}

//Initialize Meter values
void CTest9BitnCompassParts::InitMeterValues()
{
	coinIn = 1000;
	coinOut = 2000;
	totalDrop = 3000;
	jackpot = 4000;
	gamesPlayed = 5000;
}


void CTest9BitnCompassParts::StartSentinel()
{
	DWORD numberBytesWritten, numberBytesRead = 0;
	S7_Result result;

	if(!m_IsStartSentinel)
		return;

	CMeters metersForThisPoll(MEMORY_NONE);
	
	//Set the meters for Long poll 19
	metersForThisPoll.SetMeter(gCI);
	metersForThisPoll.SetMeter(gCO);
	metersForThisPoll.SetMeter(mTotalDrop);
	metersForThisPoll.SetMeter(gJP);
	metersForThisPoll.SetMeter(gGS);

	//Long poll 19
	CLongPoll19Message *lp19;

	lp19 = new CLongPoll19Message( m_gameAddress, &metersForThisPoll );
	lp19->Fill();

	//Write Long poll 19
	result = m_serial.Write(lp19->GetSendBuffer(),lp19->GetSendBufferSize(),true,&numberBytesWritten);

	usleep(m_DelayEdit);
	if(result == S7DLL_STATUS_OK)
	{
		//Read Long poll 19
		result = m_serial.Read(lp19->GetResponseBuffer(), lp19->GetResponseBufferSize(), &numberBytesRead);

		if (!lp19->VerifyResponseChecksum())
		{
			prtf((char *)"ERROR: SAS Checksum error: Long Poll 19\n", (char *)"b");
			//return;	
		}

		if(result == S7DLL_STATUS_OK)
		{
			lp19->ResponseReceived();
			
			char ch[100];
			string str;

			string sCoinIn;
			sCoinIn = std::to_string(lp19->m_coinIn);
			str = "Coin In: " + sCoinIn + ", ";

			string sCoinOut;
			sCoinOut = std::to_string(lp19->m_coinOut);
			str = str + "Coin Out: " + sCoinOut + ", ";

			string sTotalDrop;
			sTotalDrop = std::to_string(lp19->m_totalDrop);
			str = str + "Total Drop: " + sTotalDrop + ", ";

			string sJackpot;
			sJackpot = std::to_string(lp19->m_jackpot);
			str = str + "Jackpot: " + sJackpot + ", ";

			string sGamesPlayed;
			sGamesPlayed = std::to_string(lp19->m_gamesPlayed);
			str = str + "Games Played: " + sGamesPlayed;

			prtf((char *)"%s\n", (char *)"b", str.c_str());
		}
		else if(result == S7DLL_STATUS_WARNING)
		{
			prtf((char *)"WARNING: Read warning\n", (char *)"b");
		}
		else
		{
			if (numberBytesRead > 0) 
			{
				prtf((char *)"ERROR: Read Error\n", (char *)"b");
			}
		}
	}
	else if(result == S7DLL_STATUS_WARNING)
	{
		prtf((char *)"WARNING: Write warning\n", (char *)"b");
	}
	else
	{
		prtf((char *)"ERROR: Write Error\n", (char *)"b");
	}

	delete lp19;
}

void CTest9BitnCompassParts::DoGeneralPoll()
{
	DWORD numberBytesWritten;
	S7_Result result;

	//Do general poll at 0 (80)
	CSASGeneralPoll *gp_0 = new CSASGeneralPoll(0x00);
	gp_0->Fill();
	result = m_serial.Write(gp_0->GetSendBuffer(),gp_0->GetSendBufferSize(),true,&numberBytesWritten);
	delete gp_0;
}

void CTest9BitnCompassParts::StartEGM()
{
	if(!m_IsStartEGM)
		return;
	
	byte * readBuffer = (BYTE *)new byte[2];
	DWORD numberBytesRead = 0, pollCommand = 0;

	byte * responseBuffer = (BYTE *)new byte[24];
	DWORD numberBytesResponse = 0;

	S7_Result result = m_serial.Read(readBuffer, 1, &numberBytesRead);
	if(result != S7DLL_STATUS_ERROR)
	{
		pollCommand = CSASPollMessage::Bcd2Dword(readBuffer, 1);

		switch (pollCommand)
		{
		case 80:
			DisplayPollCommand(pollCommand);
			break;
		case 1:
			DisplayPollCommand(pollCommand);
			break;
		case 19:
			DisplayPollCommand(pollCommand);
			
			//fill response buffer
			if(gamesPlayed >= ULONG_MAX)
				InitMeterValues();

			coinIn++; coinOut++; totalDrop++; jackpot++; gamesPlayed++;

			responseBuffer[0] = m_gameAddress;
			responseBuffer[1] = 0x19;
			CSASPollMessage::Dword2Bcd(coinIn,&responseBuffer[2], 4);
			CSASPollMessage::Dword2Bcd(coinOut,&responseBuffer[6], 4);
			CSASPollMessage::Dword2Bcd(totalDrop,&responseBuffer[10], 4);
			CSASPollMessage::Dword2Bcd(jackpot,&responseBuffer[14], 4);
			CSASPollMessage::Dword2Bcd(gamesPlayed,&responseBuffer[18], 4);
			ushort commandCRC = CSASPollMessage::CRC(responseBuffer, 22, 0);
			byte *crc = (byte *)&responseBuffer[22];
			memcpy(crc,&commandCRC,2);

			m_serial.Write(responseBuffer, 24, true, &numberBytesResponse);
			break;
		}
	}
	else
	{
		if (numberBytesRead > 0) 
		{
			prtf((char *)"ERROR: Read Error, bytes read %d\n", (char *)"b", numberBytesRead);
		}
	}
}


// Start the sentinel operation.
BOOL CTest9BitnCompassParts::StartTimer()
{
	// Create a timer so sentinel will send general and long poll command periodically.
	if (m_nTimer) 
	{
		timer_delete(m_nTimer);
		m_nTimer = 0;
	}

	// Setup the timer
	sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = OnTimer;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) 
	{
		prtf((char *)"ERROR: Timer init failed critia error.\n", (char *)"b");
		exception *ex = new exception();
		throw (ex); 
	}

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &m_nTimer;
	if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
	{
		prtf((char *)"ERROR: Unable to create the new timer errno = %d\n", (char *)"b", errno);
		exception *ex = new exception();
		throw (ex); 
	}

	if(m_IsStartSentinel)
	{
		// Start timer
		its.it_value.tv_sec = EVENT_WAIT_TIMEOUT/1000;
		its.it_value.tv_nsec = 0;
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;
	}
	else
	{
		// Start timer
		its.it_value.tv_sec = 0;
		its.it_value.tv_nsec = EVENT_EGM_POLL_TIMEOUT * 100000;
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;
	}

	if (timer_settime(timerid, 0, &its, NULL) == -1)
	{
		prtf((char *)"ERROR: Unable to start the new timer errno = %d\n", (char *)"b", errno);
		exception *ex = new exception();
		throw (ex); 
	}

	return TRUE;
}

// Stop the timers.
void CTest9BitnCompassParts::StopTimer()
{
    timer_delete(m_nTimer);
	m_nTimer = 0;
}


