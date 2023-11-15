
#include "TestMasterParts.h"
#include "TestMaster.h"
#include "TestMasterUtilities.h"
#include "Utilities.h"
#include "phidget21.h"
#include <curses.h>
#include <term.h>
#include <sys/times.h>

using namespace std;

//#define BRAEMAC_BLUE_BOX // 2015-10-16 Define this to enable temporary (?) fixes for the Braemac blue boxes.

#define WORDLENGTH_9BIT		9
#define CONVERT_TO_MILLISECONDS 100000

USHORT wSendBuffer[BUFFER_SIZE];
USHORT wGetBuffer[BUFFER_SIZE + 100];

volatile BOOL start_test = FALSE;
QWORD testCnt = 0;

// CAboutDlg dialog used for App About

static const char alphanum[] =
"0123456789"
"!@#$%^&*()+=-_"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

//static const LPCTSTR DEFAULT_LOG_FILE_NAME(_T("E:\\ErrorLog.txt"));


// Define filename so it can be uses as both ASCII and TCHAR.
#define TIME_FILE_NAME "/opt/ncompass/mediadisk/Time.txt"
#define TOTALS_FILE_NAME "/opt/ncompass/mediadisk/testMaster_Totals.txt"

#ifdef _DEBUG
static const bool AutoStart(false); // Set false when starting manually.
#else
static const bool AutoStart(true); // Set true to start the whole thing automatically (after a startup drive delay).
#endif
static const int WAIT_FOR_DRIVE_TIMEOUT = 30000;	// 30 seconds.
static const int WAITFORDRIVEDELAY = 50000; // milliseconds
static string p_descriptions[TOTALTESTS];
static int s_TestAll = 1;
static int s_RunTest[TOTALTESTS];
static bool m_phidgetHardware;
static bool m_manualInput;

int iStrlen = sizeof(alphanum) - 1;

struct COMM_PARAM {
	UINT portno;
	WORD baudrate;
	UINT datalength;
	BYTE parity;
	BYTE stopbits;
	BYTE RTShandshake;
};

TestMasterParts::TestMasterParts()
{
	p_descriptions[battery] = "Battery";
	p_descriptions[dll] = "DLL";
	p_descriptions[backlight] = "Backlight";
	p_descriptions[timer] = "Timer";
	p_descriptions[sram] = "SRAM";
	p_descriptions[uart] = "UART";
	p_descriptions[watchdog] = "Watchdog";
}

TestMasterParts::~TestMasterParts()
{
}

S7_Result TestMasterParts::initialize()
{
	char eMessage[57];

	S7_Result result = -1;

	result = S7LITE_DLL_Init();
	//prtf((char *)"NOTE: value of S7LITE_DLL_Init return = %d\n", (char *)"f", result);
	if (result != S7DLL_STATUS_OK)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_Init returned [%d]", result);
		prtf((char *)"ERROR: %s\n", (char *)"f", eMessage);
	}

	return result;
}


S7_Result TestMasterParts::RunTest(int TestAll, int testFlags[])
{
	char eMessage[57];
	char answer[5];
	long x_start;
	long result_times[TOTALTESTS];
	S7_Result result[TOTALTESTS];
	S7_Result result_sums = 0;

	s_TestAll = TestAll;
	for (int i = 0; i < TOTALTESTS; i++) 
	{
		result[i] = 0;
		result_times[i] = 0;
		s_RunTest[i] = testFlags[i];
		m_resultMessage[i] = string("");
	}

	++m_runNumber;
	prtf((char *)"Start of Test Loop %d\n", (char *)"f", m_runNumber);

	if ((TestAll || testFlags[(int)e_Test::battery]) && (m_nextTest == (int)e_Test::battery)) // run battery test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::battery] = S7_Test_Battery();
		result_times[(int)e_Test::battery] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::dll;  // next test
		UpdateTotalsFile((int)e_Test::battery, result[(int)e_Test::battery], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::dll]) && (m_nextTest == (int)e_Test::dll)) // run DLL test.
	{
		m_nextTest++;
		x_start =  GetLinuxTicks();
		result[(int)e_Test::dll] = S7_Test_DLL();
		result_times[(int)e_Test::dll] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::backlight;  // next test
		UpdateTotalsFile((int)e_Test::dll, result[(int)e_Test::dll], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::backlight]) && (m_nextTest == (int)e_Test::backlight)) // run Backlight test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::backlight] = S7_Test_Backlight();
		m_nextTest = (int)e_Test::sram;  // next test
		UpdateTotalsFile((int)e_Test::backlight, result[(int)e_Test::backlight], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::sram]) && (m_nextTest == (int)e_Test::sram)) // run SRAM test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::sram] = S7_Test_SRAM();
		result_times[(int)e_Test::sram] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::uart;  // next test
		UpdateTotalsFile((int)e_Test::sram, result[(int)e_Test::sram], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::uart]) && (m_nextTest == (int)e_Test::uart)) // run UART test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::uart] = S7_Test_UART();
		result_times[(int)e_Test::uart] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::timer;  // next test
		UpdateTotalsFile((int)e_Test::uart, result[(int)e_Test::uart], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::timer]) && (m_nextTest == (int)e_Test::timer)) // run RTC Timer test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::timer] = S7_Test_RTCTimer();
		result_times[(int)e_Test::timer] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::watchdog;  // next test
		UpdateTotalsFile((int)e_Test::timer, result[(int)e_Test::timer], m_TotalErrors, m_totalRunErrors);
	}
	if ((TestAll || testFlags[(int)e_Test::watchdog]) && (m_nextTest == (int)e_Test::watchdog)) // run WatchDog test.
	{
		x_start =  GetLinuxTicks();
		result[(int)e_Test::watchdog] = S7_Test_Watchdog();
		result_times[(int)e_Test::watchdog] =  abs((GetLinuxTicks() - x_start) / CONVERT_TO_MILLISECONDS);
		m_nextTest = (int)e_Test::battery;  // next test
		UpdateTotalsFile((int)e_Test::watchdog, result[(int)e_Test::watchdog], m_TotalErrors, m_totalRunErrors);
	}

	m_nextTest = 0;

	for (int i=0; i < TOTALTESTS; i++) 
	{
		result_sums += result[i];
	}

	UpdateScreen(result_sums, result, result_times);
	return result_sums;
}

S7_Result TestMasterParts::finalize()
{
	char eMessage[57];
	S7_Result result = -1;
	if ((result = S7LITE_DLL_DeInit()) != 0)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_DeInit returned [%d]", result);
		prtf((char *)"ERROR: %s\n", (char *)"f", eMessage);
	}
	return result;
}

int CCONV TestMasterParts::AttachHandler(CPhidgetHandle IFK, void *userptr)
{
	UNREFERENCED_PARAMETER(userptr);

	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	return 0;
}

int CCONV TestMasterParts::DetachHandler(CPhidgetHandle IFK, void *userptr)
{
	UNREFERENCED_PARAMETER(userptr);

	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	return 0;
}

int CCONV TestMasterParts::ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
	UNREFERENCED_PARAMETER(IFK);
	UNREFERENCED_PARAMETER(userptr);

	prtf((char *)"%s\n", (char *)"f", "ERROR: Phidget Error handled. %d - %s\n", ErrorCode, unknown);
	return 0;
}


void TestMasterParts::UpdateScreen(int run_total, S7_Result results_errors[], long result_times[])
{
	char x_buffer[1024];
	char *p_str;
	char space[10] = {"         "};
	char *sp = space;

	// clear the screen
	tgetent(x_buffer, getenv("TERM"));
	p_str = tgetstr((char *)"cl", NULL);
	fputs(p_str, stdout);

	// display the results to the log and screen
	snprintf(x_buffer, sizeof(x_buffer), (char *)"TestMaster Summary for Loop %ld.", m_runNumber);
	prtf((char *)"%s\n", (char *)"b", x_buffer);
	prtf((char *)"%-10.10s%2.2s%8.8s%2.2s%8.8s%2.2s%8.8s%1.1s%s\n", (char *)"b", (char *)"Test", sp, (char *)"Loop", sp, 
		 (char *)"Session", sp, (char *)"Total", sp, (char *)"Result Message");
	for (int i = 0; i < TOTALTESTS; i++) 
	{
		if ((s_TestAll == 1) || (s_RunTest[i] == 1))
		{
			char *sptr = (m_resultMessage[i].length() > 0) ? (char *)m_resultMessage[i].c_str() : (char *)"\t";
			prtf((char *)"%-10.10s%2.2s%8ld%2.2s%8ld%2.2s%8ld%1.1s%s\n", (char *)"b", p_descriptions[i].c_str(), sp,  
				 results_errors[i], sp, m_sessionErrors[i], sp, m_TotalErrors[i], sp, sptr);
		}
	}
	prtf((char *)"%-10.10s%2.2s%8ld%2.2s%8ld%2.2s%8ld\n", (char *)"b", (char *)"TOTALS:", sp,  
		 run_total, sp,m_totalSessionErrors, sp,  m_totalRunErrors);

	prtf((char *)"End of Test Loop %ld\n", (char *)"f", m_runNumber);
	prtf((char *)"\n", (char *)"f");

	// pause for one second
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}


void TestMasterParts::update_srand()
{
	unsigned int part1;
	unsigned int part2;
	unsigned int seed;
    struct tms timeTms;
	times(&timeTms);
	part1 = (unsigned int)timeTms.tms_stime;
	part2 = (unsigned int)clock();
	seed = (unsigned int)((part1 >> 16 && 0xffff) + (part1 && 0xffff)
		+ (part2 >> 15 && 0xffff) + (part2 && 0xffff)
		+ (unsigned int)clock() + (unsigned int)time(NULL));
	srand(seed);

	// Consume some values to perform additional mixing.
	rand();
	rand();
	rand();
}


void TestMasterParts::GetRandomString(char* buffer, int buffersiize)
{
	int iBufferLen = 0;
	int i = 0;
	update_srand();
	iBufferLen = buffersiize - 1;

	for (; i<iBufferLen; i++)
	{
		buffer[i] = alphanum[rand() % iStrlen];
	}
	buffer[i] = '\0';
}

void TestMasterParts::Set9BitSerial(bool value)
{
	m_nineBitLoopbackEnabled = value;
}


/// <summary>
/// Sets a string from a buffer that might contain trailing NUL characters.
/// </summary>
/// <param name="string">OUT - The string.</param>
/// <param name="str">IN - The string buffer.</param>
/// <param name="length">IN - The length of the string buffer.</param>
void TestMasterParts::Setstring(string &xstring, LPCTSTR str, UINT length)
{
	if (NULL == str)
	{
		xstring.clear();
	}
	else
	{
		while (length > 0 && 0 == ((LPCTSTR)str)[length - 1])
		{
			--length;
		}
	}

	xstring = string((LPCTSTR)str);
}

void TestMasterParts::DetectPhidgetHardware()
{
	int result;
	bool x_attachmentError;

	prtf((char *)"NOTE: Attempting to detect Phidget Hardware!!\n",  (char *)"b");

	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit = 0;

	//create the InterfaceKit object
	result = CPhidgetInterfaceKit_create(&ifKit);
	//prtf((char *)"NOTE: value of CPhidgetInterfaceKit_create return = %d\n", (char *)"f", result);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	result = CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	//prtf((char *)"NOTE: value of CPhidget_set_OnAttach_Handler return = %d\n", (char *)"f", result);
	result = CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	//prtf((char *)"NOTE: value of CPhidget_set_OnDetach_Handler return = %d\n", (char *)"f", result);
	result = CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);
	//prtf((char *)"NOTE: value of CPhidget_set_OnError_Handler return = %d\n", (char *)"f", result);

	//open the interfacekit for device connections
	result = CPhidget_open((CPhidgetHandle)ifKit, -1);
	//prtf((char *)"NOTE: value of CPhidget_open return = %d\n", (char *)"f", result);

	//get the program to wait for an interface kit device to be attached
	result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 20000);
	//prtf((char *)"NOTE: value of CPhidget_waitForAttachment return = %d\n", (char *)"f", result);
	x_attachmentError = EPHIDGET_OK != result;
	if (x_attachmentError)
	{
		m_phidgetHardware = false;
		IDC_RTC_PowerCycle = false;
		//prtf((char *)"NOTE: Test running without Phidget Hardware!!\n",  (char *)"b");
	} else {
		m_phidgetHardware = true;
		IDC_RTC_PowerCycle = true;
		//prtf((char *)"NOTE: Test running with Phidget Hardware!!\n",  (char *)"b");
	}
	result = CPhidget_close((CPhidgetHandle)ifKit);
	//prtf((char *)"NOTE: value of CPhidget_close return = %d\n", (char *)"f", result);
	CPhidget_delete((CPhidgetHandle)ifKit);
	//prtf((char *)"NOTE: value of CPhidget_delete return = %d\n", (char *)"f", result);
}

void TestMasterParts::AddErrorString(const string &name)
{
	char x_buffer[1024];
	sprintf(x_buffer, "%s", name.c_str());
	prtf((char *)"%s\n", (char *)"f", x_buffer);
}

void TestMasterParts::AddLogString(const string &name)
{
	char x_buffer[1024];
	sprintf(x_buffer, "%s",  name.c_str());
	prtf((char *)"%s\n", (char *)"b", x_buffer);
}

void TestMasterParts::UpdateResult(e_Test TestId, UINT ID, LPCTSTR result)
{
	 m_resultMessage[(int)TestId] = string(result);
}

void TestMasterParts::InitTestVars()
{
	FILE *f_totals;
	int x = 0;
	char *ptr;
	char x_buffer[256];

	try
	{
		m_timerStarted = false;
		m_runNumber = 0;
		m_totalSessionErrors = 0;
		m_totalSessionTicks = 0;
		m_totalRunErrors = 0;
		m_nextTest = 0;
		for (int i = 0; i < TOTALTESTS; i++) 
		{
			m_sessionErrors[i] = 0;
			m_sessionTicks[i] = 0;
			m_TotalErrors[i] = 0;
		}

		m_timerStartTickCountMinimum = 0;
		m_timerStartTickCountMaximum = 0;
		m_nineBitLoopbackEnabled = true;

		// Get the running total
		f_totals = fopen(TOTALS_FILE_NAME, "r");
		if (f_totals != NULL) 
		{
			ptr = fgets(x_buffer, sizeof(x_buffer), f_totals);
			//prtf((char *)"Reading the Initial Buffers [%s]\n", (char *)"f", ptr);
			if ((ptr != NULL) && (strlen(ptr) > 0))
			{
				// process the totals columns
				x = 0;
				ptr = strtok(x_buffer, ":");
				m_runNumber = atol(ptr);
				ptr = strtok(NULL, ":");
				m_nextTest = atoi(ptr);
				ptr = strtok(NULL, ":");
				m_totalRunErrors = atol(ptr);
				ptr = strtok(NULL, ":");
				m_TotalErrors[x++] = atol(ptr);
				while (((ptr = strtok(NULL, ":")) != NULL) && (x < TOTALTESTS)) 
				{	
					m_TotalErrors[x++] = atol(ptr);
				}
			}
			fclose(f_totals);
		}
		else
		{
			m_runNumber = 0;
			for (int i = 0; i < TOTALTESTS; i++) 
			{
				m_TotalErrors[i] = 0;		
			}
			m_totalRunErrors = 0;
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: InitTestVars unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"f", x_buffer);
	}
}



void TestMasterParts::UpdateTotalsFile(int lastTest, long errors, long totalErrors[], long totalRunErrors)
{
	std::FILE *f_totals;
	string s_buffer ("");
	char x_buffer[256];

	try
	{
		m_sessionErrors[lastTest] += errors;
		m_TotalErrors[lastTest] += errors;
		m_totalSessionErrors += errors;
		m_totalRunErrors += errors;

		// Get the running total
		f_totals = fopen((char *)TOTALS_FILE_NAME, (char *)"w");
		snprintf(x_buffer, sizeof(x_buffer), (char *)"%ld:", m_runNumber);
		s_buffer.append(string(x_buffer));
		snprintf(x_buffer, sizeof(x_buffer), (char *)"%d:", lastTest);
		s_buffer.append(string(x_buffer));
		snprintf(x_buffer, sizeof(x_buffer), (char *)"%ld:", m_totalRunErrors);
		s_buffer.append(string(x_buffer));
		if (f_totals) 
		{
			for (int i = 0;  i < TOTALTESTS; i++) 
			{
				snprintf(x_buffer, sizeof(x_buffer), (char *)"%ld", m_TotalErrors[i]);
				s_buffer.append(string(x_buffer));
				if (i < (TOTALTESTS - 1)) 
				{
					s_buffer.append(":");
				}
			}
			s_buffer.append("\n");
			fprintf(f_totals, (char *)s_buffer.c_str());
			fflush(f_totals);
			fclose(f_totals);
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: UpdateTotalsFile unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"f", x_buffer);
	}
}

// Note that calling S7_DLL_Init() resets the milliseconds on the RTC.
S7_Result TestMasterParts::S7_Test_DLL(void)
{
	S7_Result errorCount(0);
	S7_Result result_init(S7DLL_STATUS_OK), result_version;
	BYTE version = 0;
	string result_status;

	if (!m_timerStarted)
	{
		result_init = S7LITE_DLL_Init();
	}
	result_version = S7LITE_DLL_GetDLLVersion(&version);

	if (result_init == S7DLL_STATUS_ERROR)
	{
		AddErrorString("ERROR: S7LITE_DLL_Init");
		UpdateResult(timer, 0, _T("S7LITE_DLL_Init ERROR"));
		++errorCount;
	}
	if (result_version != S7DLL_STATUS_OK)
	{
		AddErrorString("ERROR: S7LITE_DLL_GetDLLVersion : DLL Version Not found");
		UpdateResult(dll, result_version, _T("S7LITE_DLL_GetDLLVersion ERROR"));
		++errorCount;
	}

	if (errorCount == 0)
		UpdateResult(dll, result_version, _T("DLL OK"));

	return errorCount;
}

// Note that calling S7_DLL_Init() resets the milliseconds on the RTC.
// ---------
// Periodically, return with m_timerStarted == false so additional tests (like S7_Test_DLL) can execute.
S7_Result TestMasterParts::S7_Test_RTCTimer(void)
{
	DWORD errorCount(0);
	S7_Result result_GetTime, result_SetTime, result_tempTime;
	SYSTEMTIME temp_Time1, temp_Time2;
	FILETIME filetime1, filetime2;
	//clockid_t 
	//struct timespec 

	/* Test Scenario 1:
	Testing the RTC timer by setting timer value to local PC time and waiting for 2.1 seconds (2100ms)
	before retrieving the timer value and making sure that the time difference is in the range of 2 to 2.2 seconds*/
	const LONGLONG RTCTimer_millisecondsDiffMinimum = -100; // Compared to GetTickCount().
	const LONGLONG RTCTimer_millisecondsDiffMaximum = 100; // Compared to GetTickCount().
	const DWORD RTCTimer_testDelayMinimum = 2100; // Test for RTC within min/max over at least this period.

	UpdateResult(timer, 0, _T("RTC Timer ERROR"));
	if (!m_timerStarted)
	{
		//test the settime function by setting the random datetime
		GetLocalTime(&temp_Time1);
		//temp_Time1 will be used to set the timer using S7_Rtc_SetTime()
		//S7_Rtc_GetTime() funcion always returns 0 for wMilliseconds
		//So, Set wMilliseconds portion of temp_Timer1 to 0 that way we dont have to worry about
		//milliseconds portion in the calculations
		temp_Time1.wMilliseconds = 0;
		m_timerStartSystemTime = temp_Time1;
		m_timerStartTickCountMinimum = GetTickCount();
		result_SetTime = S7LITE_Rtc_SetTime(temp_Time1);
		m_timerStartTickCountMaximum = GetTickCount();
		if (result_SetTime == S7DLL_STATUS_OK)
		{
			m_timerStarted = true;
		}
		else
		{
			AddErrorString("ERROR: S7LITE_Rtc_SetTime");
			UpdateResult(timer, 0, _T("Set Time ERROR"));
			return ++errorCount;
		}

		DWORD endTickCountMinimum(GetTickCount());
		result_GetTime = S7LITE_Rtc_GetTime(&temp_Time2);
		DWORD endTickCountMaximum(GetTickCount());
		if (result_GetTime != S7DLL_STATUS_OK)
		{
			AddErrorString("ERROR: S7LITE_Rtc_GetTime 0");
			UpdateResult(timer, 0, _T("Get Time ERROR"));
			return ++errorCount;
		}
		else if (m_timerStarted)
		{
			SystemTimeToFileTime(&m_timerStartSystemTime, &filetime1);
			SystemTimeToFileTime(&temp_Time2, &filetime2);

			ULARGE_INTEGER nfiletime1, nfiletime2;
			nfiletime1.HighPart = filetime1.dwHighDateTime;
			nfiletime1.LowPart = filetime1.dwLowDateTime;
			nfiletime2.HighPart = filetime2.dwHighDateTime;
			nfiletime2.LowPart = filetime2.dwLowDateTime;

			LONGLONG rtcTimerFiletimeDiff = nfiletime2.QuadPart - nfiletime1.QuadPart;
			// rtcTimerMillisecondsDiff is the difference in between filtime1 and filetime2 in milliseconds.
			LONGLONG rtcTimerMillisecondsDiff = rtcTimerFiletimeDiff / 10000;

			DWORD tickCountDiffMinimum = endTickCountMinimum - m_timerStartTickCountMaximum; // Measured.
			DWORD tickCountDiffMaximum = endTickCountMaximum - m_timerStartTickCountMinimum; // Measured.
			const LONGLONG allowedMillisecondsDiffMinimum = 0;
			LONGLONG allowedMillisecondsDiffMaximum = tickCountDiffMaximum + RTCTimer_millisecondsDiffMaximum;

			if (rtcTimerMillisecondsDiff < allowedMillisecondsDiffMinimum ||
				rtcTimerMillisecondsDiff > allowedMillisecondsDiffMaximum)
			{
				char x_buffer[256];
				m_timerStarted = false; // Don't do the N-second tests if this test failed.
				snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_Rtc_SetTime: inconsistency rtc0=%lldms after %u..%u ticks",
					(long long)rtcTimerMillisecondsDiff,
					(unsigned)tickCountDiffMinimum,
					(unsigned)tickCountDiffMaximum);
				AddErrorString(string(x_buffer));
				UpdateResult(timer, 0, _T("Set Time ERROR"));
				return ++errorCount;
			}
		}
	}

	if (m_timerStarted)
	{
		// Do an N-second test.
		DWORD endTickCountMinimum(GetTickCount());
		result_tempTime = S7LITE_Rtc_GetTime(&temp_Time2);
		DWORD endTickCountMaximum(GetTickCount());
		if (result_tempTime != S7DLL_STATUS_OK)
		{
			AddErrorString("ERROR: S7LITE_Rtc_GetTime: Error getting random set time");
			UpdateResult(timer, 0, _T("Get Time ERROR"));
			return ++errorCount;
		}
		SystemTimeToFileTime(&m_timerStartSystemTime, &filetime1);
		SystemTimeToFileTime(&temp_Time2, &filetime2);

		ULARGE_INTEGER nfiletime1, nfiletime2;
		nfiletime1.HighPart = filetime1.dwHighDateTime;
		nfiletime1.LowPart = filetime1.dwLowDateTime;
		nfiletime2.HighPart = filetime2.dwHighDateTime;
		nfiletime2.LowPart = filetime2.dwLowDateTime;

		LONGLONG rtcTimerFiletimeDiff = nfiletime2.QuadPart - nfiletime1.QuadPart;
		// milliseconds is the difference in between filtime1 and filetime2 in milliseconds.
		LONGLONG rtcTimerMillisecondsDiff = rtcTimerFiletimeDiff / 10000;

		DWORD tickCountDiffMinimum = endTickCountMinimum - m_timerStartTickCountMaximum; // Measured.
		DWORD tickCountDiffMaximum = endTickCountMaximum - m_timerStartTickCountMinimum; // Measured.
		LONGLONG allowedMillisecondsDiffMinimum = tickCountDiffMinimum + RTCTimer_millisecondsDiffMinimum;
		allowedMillisecondsDiffMinimum = allowedMillisecondsDiffMinimum / 1000 * 1000; // Round down to full seconds.
		LONGLONG allowedMillisecondsDiffMaximum = tickCountDiffMaximum + RTCTimer_millisecondsDiffMaximum;

		if (rtcTimerMillisecondsDiff < allowedMillisecondsDiffMinimum ||
			rtcTimerMillisecondsDiff > allowedMillisecondsDiffMaximum)
		{
			m_timerStarted = false;
			char x_buffer[256];
			snprintf(x_buffer, sizeof(x_buffer), "ERROR: S7LITE_Rtc_SetTime: inconsistency rtcN=%lldms after %u..%u ticks",
				(long long)rtcTimerMillisecondsDiff,
				(unsigned)tickCountDiffMinimum,
				(unsigned)tickCountDiffMaximum);
			AddErrorString(string(x_buffer));
			UpdateResult(timer, 0, _T("Set Time ERROR"));
			return ++errorCount;
		}
	}

	DWORD elapsedTicks = GetTickCount() - m_timerStartTickCountMaximum;
	if (elapsedTicks >= RTCTimer_testDelayMinimum)
	{
		// Start another test.
		m_timerStarted = false;
	}

	/* Test Scenario 2:
	Testing the RTC timer by comparing the timer values before and after
	powercycling and making sure that the time difference is within specifications */
	UINT iRTC_PowerCycleTest = IDC_RTC_PowerCycle;
	if (iRTC_PowerCycleTest == true)
	{
		//test the settime function by setting the random datetime
		GetLocalTime(&temp_Time1);
		char write_time[50];
		//Note that the '/0' at the end of the string is to indicate the freshness of data
		// /0 at the end of the string means its fresh and /1 means its stale. We make the
		//data stale once we read the data (which is done in the OninitDialog() function)
		sprintf(write_time, "%04u/%02u/%01u/%02u/%02u/%02u/%02u/0",
			temp_Time1.wYear, temp_Time1.wMonth, temp_Time1.wDayOfWeek, temp_Time1.wDay, temp_Time1.wHour, temp_Time1.wMinute, temp_Time1.wSecond);

		if (!WriteDataToFile((char *)TIME_FILE_NAME, write_time))
		{
			char x_buffer[256];
			snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: Unable to Save to file %s", static_cast<LPCTSTR>(_T(TIME_FILE_NAME)));
			AddErrorString(string(x_buffer));
			++errorCount;
		}


		result_SetTime = S7LITE_Rtc_SetTime(temp_Time1);
		if (result_SetTime != S7DLL_STATUS_OK)
		{
			AddErrorString("ERROR: S7LITE_Rtc_SetTime PC");
			UpdateResult(timer, 0, _T("Set Time ERROR"));
			return ++errorCount;
		}

		char read_time[50];
		if (!ReadDataFromFile((char *)TIME_FILE_NAME, read_time))
		{
			char x_buffer[256];
			snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: Reading from file %s", static_cast<LPCTSTR>(TIME_FILE_NAME));
			AddErrorString(string(x_buffer));
			++errorCount;
		}
		else
		{
			if (strcmp(write_time, read_time) == 0)
			{
				usleep(30000);
				if (m_phidgetHardware) 
				{
					//We are sleeping for 30s, just to make sure that the file is written to the disk
					//and is still not in the buffer when power cycling happens
					SystemRestart();
				}
				else
				{
					UpdateResult(timer, 0, _T("Restart Skipped"));
				}
			}
		}
	}

	if (errorCount == 0)
		UpdateResult(timer, 0, _T("RTC Timer OK"));

	return errorCount;
}

S7_Result TestMasterParts::S7_Test_Backlight(void)
{
#ifdef BRAEMAC_BLUE_BOX // 2015-10-16 - On Braemac Blue Boxes, these LCD tests run slowly and fail.
	return 0;
#endif
	DWORD errorCount(0);
	S7_Result result;
	char buffer[100];
	USHORT Pwm = 7;
	for (int i = 0; i<8 && 0 == errorCount; i++)
	{
		update_srand();
		Pwm = rand() % 6 + 1;
		result = S7LITE_RemoteLCD_SetBacklightPWM(Pwm);
		if (result != S7DLL_STATUS_OK)
		{
			sprintf(buffer, "Unable to set backLight with PWM=%d prevPWM=%d diffPWM=%d result=%d lcdTest=%d",
				(int)Pwm, (int)m_prevPWM, (int)(Pwm - m_prevPWM), (int)result, (int)i);
			string errorString = string("ERROR: S7LITE_RemoteLCD_SetBacklight: ");
			errorString.append((string)buffer);
			AddErrorString(errorString);
			UpdateResult(backlight, result, _T("Set ERROR"));
			m_prevPWM = Pwm;
			++errorCount;
		}
		m_prevPWM = Pwm;
	}

	// Try to set maximum even if an error occurred.
	Pwm = 7;
	result = S7LITE_RemoteLCD_SetBacklightPWM(Pwm);
	if (result == S7DLL_STATUS_OK)
	{
		m_prevPWM = Pwm;
	}
	else
	{
		sprintf(buffer, "Unable to set Maximum PWM=%d prevPWM=%d diffPWM=%d result=%d",
			(int)Pwm, (int)m_prevPWM, (int)(Pwm - m_prevPWM), (int)result);
		string errorString = string("ERROR: S7LITE_RemoteLCD_SetBacklight: ");
		errorString.append((string)buffer);
		AddErrorString(errorString);
		UpdateResult(backlight, result, _T("Set Maximum ERROR"));
		m_prevPWM = Pwm;
		++errorCount;
	}

	if (errorCount == 0)
		UpdateResult(backlight, result, _T("Backlight OK"));
	return errorCount;
}

S7_Result TestMasterParts::S7_Test_Battery(void)
{
	DWORD errorCount(0);
	int		BatteryStatus = 1;
	S7_Result result;
	result = S7LITE_Battery_GetStatus(&BatteryStatus);
	if (result == S7DLL_STATUS_OK)
	{
		switch (BatteryStatus)
		{
			case batteryStatusOkay:
			{
				UpdateResult(battery, BatteryStatus, _T("Battery OK"));
			}
				break;
			case batteryStatusLow:
			{
				UpdateResult(battery, BatteryStatus, _T("Battery LOW"));
			}
				break;
			case batteryStatusNoBattery:
			{
				UpdateResult(battery, BatteryStatus, _T("No Battery"));
			}
				break;
			case batteryStatusUnknown:
			{
				UpdateResult(battery, BatteryStatus, _T("Battery UNKNOWN"));
			}
				break;
			default:
			{
				UpdateResult(battery, BatteryStatus, _T("Battery INVALID"));
				++errorCount;
			}
		}
	}
	else
	{
		UpdateResult(battery, BatteryStatus, _T("Battery ERROR"));
		AddErrorString("ERROR: S7LITE_Battery_GetStatus");
		++errorCount;
	}

	return errorCount;
}

S7_Result TestMasterParts::S7_Test_Watchdog(void)
{
	DWORD errorCount(0);
	S7_Result result;

	result = S7LITE_Watchdog_Enable();
	//prtf((char *)"NOTE: value of S7LITE_Watchdog_Enable return = %d\n", (char *)"f", result);
	if (result != S7DLL_STATUS_OK)
	{
		AddErrorString("ERROR: S7LITE_Watchdog_Enable");
		++errorCount;
	}
	result = S7LITE_Watchdog_SetTimeout(WATCHDOG_TIMEOUT);
	//prtf((char *)"NOTE: value of S7LITE_Watchdog_SetTimeout return = %d\n", (char *)"f", result);
	if (result != S7DLL_STATUS_OK)
	{
		AddErrorString("ERROR: S7LITE_Watchdog_SetTimeout");
		++errorCount;
	}
	result = S7LITE_Watchdog_Kick();
	//prtf((char *)"NOTE: value of S7LITE_Watchdog_Kick return = %d\n", (char *)"f", result);
	if (result != S7DLL_STATUS_OK)
	{
		AddErrorString("ERROR: S7LITE_Watchdog_Kick");
		++errorCount;
	}

	if (errorCount == 0)
		UpdateResult(watchdog, result, _T("Watchdog Timer OK"));
	else
		UpdateResult(watchdog, result, _T("Watchdog Timer ERROR"));
	return errorCount;
}

bool TestMasterParts::IsAxiomtekUart() const
{
	bool isAxiomtekUart(false);

	if (m_uartVersion <= m_maximumAxiomtekUartVersion)
	{
		isAxiomtekUart = true;
	}

	return isAxiomtekUart;
}

S7_Result TestMasterParts::S7_Test_UART(void)
{
	S7_Result result;
	char buffer[1024];
	USHORT	wData = 0;
	DWORD errorCount(0);
	ULONG tempVersion = 0;
	COMM_PARAM commp = { UART_PORT1, 19200, WORDLENGTH_9BIT, NO_PARITY, STOP_BIT_1, SERIAL_NO_HANDSHAKE };

	m_uartVersion = tempVersion;

	// Set mode before baud rate.  This is for compatibility with multiple applications and platforms.
	result = S7LITE_UART_SetMode(commp.portno, commp.datalength, commp.parity, commp.stopbits, commp.RTShandshake);
	if (result != S7DLL_STATUS_OK)
	{
		sprintf(buffer, " Unable to set Mode with Port = %d,datalength= %d,"
			"parity= %d,stopbit = %d,handshake bit = %d", commp.portno,
			commp.datalength, commp.parity, commp.stopbits, commp.RTShandshake);
		string errorString = string("ERROR: S7LITE_UART_SetMode: ");
		errorString.append((string)buffer);
		AddErrorString(errorString);
		UpdateResult(uart, result, _T("Set Parameter ERROR"));
		++errorCount;
	}

	result = S7LITE_UART_SetBaudRate(commp.portno, commp.baudrate);
	if (result != S7DLL_STATUS_OK)
	{
		sprintf(buffer, " Error setting the baudrate= %d", commp.baudrate);
		string errorString = string("ERROR: S7LITE_UART_SetBaudRate: ");
		errorString.append((string)buffer);
		AddErrorString(errorString);
		UpdateResult(uart, result, _T("Set Parameter ERROR"));
		++errorCount;
	}

	result = S7LITE_UART_SetTimeouts(commp.portno, READINTERVAL, WRITEMULTIPLIER, WRITECONSTANT);
	if (result != S7DLL_STATUS_OK)
	{
		sprintf(buffer, " Unable to set Timeout with Port = %d,readinterval= %d,"
			"writemultiplier= %d,writeconstant = %d", commp.portno,
			READINTERVAL, WRITEMULTIPLIER, WRITECONSTANT);
		string errorString = string("ERROR: S7LITE_UART_SetTimeouts: ");
		errorString.append((string)buffer);
		AddErrorString(errorString);
		UpdateResult(uart, result, _T("Set Parameter ERROR"));
		++errorCount;
	}

	result = S7LITE_UART_SetClrRTS(commp.portno, TRUE);
	// 2015-10-29 The Braemac S6 blue boxes return S7DLL_STATUS_WARNING to indicate that the call had no effect.
	if (result != S7DLL_STATUS_OK && result != S7DLL_STATUS_WARNING)
	{
		char x_buffer[256];
		snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_UART_SetClrRTS result=%d", (int)result);
		AddErrorString(string(x_buffer));
		UpdateResult(uart, result, _T("Set Parameter ERROR"));
		++errorCount;
	}

	// Only clear buffers once for each test iteration.
	USHORT mask(CLR_RX_BUFFER | CLR_TX_BUFFER);
	switch ((unsigned)testCnt % 3)
	{
	case 0:
		mask = CLR_RX_BUFFER;
		break;
	case 1:
		mask = CLR_TX_BUFFER;
		break;
	default:
		break;
	}

	result = S7LITE_UART_ClearBuffers(commp.portno, mask);
	if (result != S7DLL_STATUS_OK)
	{
		sprintf(buffer, " Unable to clear Buffer with Port=%d mask=%d result=%d"
			, commp.portno, mask, result);
		string errorString = string("S7LITE_UART_ClearBuffers: ");
		errorString.append((string)buffer);
		AddErrorString(errorString);
		UpdateResult(uart, result, _T("Clear Buffer ERROR"));
		++errorCount;
	}

	//check if the 9-bit loopback is required
	USHORT wSendWord[1];
	wSendWord[0] = 0x0141;
	if (m_nineBitLoopbackEnabled)
	{
		result = S7LITE_UART_SendWord(commp.portno, wSendWord[0]);
		if (result != S7DLL_STATUS_OK)
		{
			AddErrorString("ERROR: S7LITE_UART_SendWord");
			UpdateResult(uart, result, _T("Send Word ERROR"));
			++errorCount;
		}
		if (IDC_LOOPBACK == true)
		{
			// For this test, expect Axiomtek UART to return no data.
			// To improve testing speed, only ask for expected data. Asking for an extra byte is a major performance hit.
			if (!IsAxiomtekUart())
			{
				// Older Axiomtek drivers return success when no data is received.
				result = S7LITE_UART_GetWord(commp.portno, &wData);
				{
					switch (result)
					{
					case S7DLL_STATUS_ERROR:
						AddErrorString("ERROR: S7LITE_UART_GetWord");
						UpdateResult(uart, result, _T("Get Word ERROR"));
						++errorCount;
						break;
					case S7DLL_STATUS_WARNING:
						// Expected result for axiomtek hardware/drivers.
						break;
					case S7DLL_STATUS_OK:
						if ((wData != wSendWord[0]))
						{
							AddErrorString("ERROR: S7LITE_UART_GetWord: Wrong word received");
							UpdateResult(uart, result, _T("Get Word ERROR"));
							++errorCount;
						}
						break;
					default:
						AddErrorString("ERROR: S7LITE_UART_GetWord: Unknown result");
						UpdateResult(uart, result, _T("Get Word ERROR"));
						++errorCount;
						break;
					}
				}
			}
		}

		if (IsAxiomtekUart())
		{
			// Test sending 2 bytes, 1st byte with parity bit set.
			const UINT wSendBufferLen(2);
			wSendBuffer[0] = 0x0132;
			wSendBuffer[1] = 0x00ff;
			result = S7LITE_UART_SendBuffer(commp.portno, wSendBuffer, wSendBufferLen);
			if (result != S7DLL_STATUS_OK)
			{
				AddErrorString("ERROR: S7LITE_UART_SendBuffer");
				UpdateResult(uart, result, _T("Send Buffer ERROR"));
				++errorCount;
			}

			if (IDC_LOOPBACK == true)
			{
				result = S7LITE_UART_GetWord(commp.portno, &wData);
				switch (result)
				{
				case S7DLL_STATUS_ERROR:
					AddErrorString("ERROR: S7LITE_UART_GetWord2");
					UpdateResult(uart, result, _T("Get Word ERROR"));
					++errorCount;
					break;
				case S7DLL_STATUS_WARNING:
					AddErrorString("WARNING! S7LITE_UART_GetWord2:No character received");
					UpdateResult(uart, result, _T("Get Word WARNING"));
					++errorCount;
					break;
				case S7DLL_STATUS_OK:
					// Expect only second word received for Axiomtek hardware/drivers.
					if (wData != (wSendBuffer[1] ^ 0x2a00))
					{
						AddErrorString("ERROR: S7LITE_UART_GetWord2: Wrong word received");
						UpdateResult(uart, result, _T("Get Word ERROR"));
						++errorCount;
					}
					break;
				default:
					AddErrorString("ERROR: S7LITE_UART_GetWord2: Unknown result");
					UpdateResult(uart, result, _T("Get Word ERROR"));
					++errorCount;
					break;
				}
			}
		}

		// Test sending a buffer with all words with parity bit set.
		{
			//create buffer data
			wSendBuffer[0] = 0x0132;
			wSendBuffer[1] = 0x01ff;
			wSendBuffer[2] = 0x0132;
			wSendBuffer[3] = 0x01f2;
			wSendBuffer[4] = 0x0132;

			result = S7LITE_UART_SendBuffer(commp.portno, wSendBuffer, BUFFER_SIZE);
			if (result != S7DLL_STATUS_OK)
			{
				AddErrorString("ERROR: S7LITE_UART_SendBuffer");
				UpdateResult(uart, result, _T("Send Buffer ERROR"));
				++errorCount;
			}

			// To improve testing speed, only ask for expected data. Asking for extra byte(s) is a major performance hit.
			if (!IsAxiomtekUart())
			{
				if (IDC_LOOPBACK == true)
				{
					memset(wGetBuffer, 0, sizeof(wGetBuffer));
					const UINT expectedLen(BUFFER_SIZE); // Ask only for the amount of data expected.
					UINT iLen(expectedLen); // Ask only for the amount of data expected.
					result = S7LITE_UART_GetBuffer(commp.portno, wGetBuffer, &iLen);
					{
						switch (result)
						{
						case S7DLL_STATUS_WARNING:
						{
							// Expect no data received for axiomtek hardware/drivers.
							if (!IsAxiomtekUart())
							{
								char x_buffer[256];
								snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7_UART_GetBuffer: %d/%d words received", (int)iLen, (int)expectedLen);
								AddErrorString(string(x_buffer));
								UpdateResult(uart, result, _T("Get Buffer ERROR"));
								++errorCount;
							}
							else if (iLen != 0) // Expect no data received for axiomtek hardware/drivers.
							{
								AddErrorString("ERROR: S7LITE_UART_GetBuffer: WARNING: Too much data received");
								UpdateResult(uart, result, _T("Get Buffer ERROR"));
								++errorCount;
							}
						}
						break;
						case S7DLL_STATUS_ERROR:
							AddErrorString("ERROR:S7LITE_UART_GetBuffer");
							UpdateResult(uart, result, _T("Get Buffer ERROR"));
							++errorCount;
							break;
						case S7DLL_STATUS_OK:
							if (!IsAxiomtekUart())
							{
								bool done(false);
								for (UINT index(0); index < _countof(wSendBuffer) && !done; ++index)
								{
									if (wGetBuffer[index] != wSendBuffer[index])
									{
										AddErrorString("ERROR:S7LITE_UART_GetBuffer:Data inconsistency");
										UpdateResult(uart, result, _T("Get Buffer ERROR"));
										++errorCount;
										done = true;
									}
								}
							}
							else
							{
								AddErrorString("ERROR: S7LITE_UART_GetBuffer: OK: Too much data received");
								UpdateResult(uart, result, _T("Get Buffer ERROR"));
								++errorCount;
							}
							break;
						default:
							AddErrorString("ERROR: S7LITE_UART_GetBuffer: Unknown result");
							UpdateResult(uart, result, _T("Get Buffer ERROR"));
							++errorCount;
							break;
						}
					}
				}
			}
		}

		// Test sending only some words with parity bit set.
		{
			//create buffer data
			wSendBuffer[0] = 0x011a;
			wSendBuffer[1] = 0x002b;
			wSendBuffer[2] = 0x013c;
			wSendBuffer[3] = 0x004d;
			wSendBuffer[4] = 0x015e;

			result = S7LITE_UART_SendBuffer(commp.portno, wSendBuffer, BUFFER_SIZE);
			if (result != S7DLL_STATUS_OK)
			{
				AddErrorString("ERROR: S7LITE_UART_SendBuffer2");
				UpdateResult(uart, result, _T("Send Buffer ERROR"));
				++errorCount;
			}

			if (IDC_LOOPBACK == true)
			{
				memset(wGetBuffer, (int)testCnt, sizeof(wGetBuffer));
				UINT expectedLen(2);
				if (!IsAxiomtekUart())
				{
					expectedLen = BUFFER_SIZE;
				}
				UINT iLen(expectedLen); // Ask only for the amount of data expected.
				result = S7LITE_UART_GetBuffer(commp.portno, wGetBuffer, &iLen);
				{
					switch (result)
					{
					case S7DLL_STATUS_WARNING:
					{
						char x_buffer[256];
						snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_UART_GetBuffer2: %d/%d words received", (int)iLen, (int)expectedLen);
						AddErrorString(string(x_buffer));
						UpdateResult(uart, result, _T("Get Buffer ERROR"));
						++errorCount;
					}
					break;
					case S7DLL_STATUS_ERROR:
						AddErrorString("ERROR:S7LITE_UART_GetBuffer2");
						UpdateResult(uart, result, _T("Get Buffer ERROR"));
						++errorCount;
						break;
					case S7DLL_STATUS_OK:
						if (IsAxiomtekUart())
						{
							if (wGetBuffer[0] != (wSendBuffer[1] ^ 0x2a00) || wGetBuffer[1] != (wSendBuffer[3] ^ 0x2a00))
							{
								AddErrorString("ERROR:S7LITE_UART_GetBuffer2:Data inconsistency");
								UpdateResult(uart, result, _T("Get Buffer ERROR"));
								++errorCount;
							}
						}
						else
						{
							bool done(false);
							for (UINT index(0); index < _countof(wSendBuffer) && !done; ++index)
							{
								if (wGetBuffer[index] != wSendBuffer[index])
								{
									char x_buffer[256];
									snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_UART_GetBuffer2[%u] %4.4x<>%4.4x Data inconsistency",
										(unsigned)index, (unsigned)wGetBuffer[index], (unsigned)wSendBuffer[index]);
									AddErrorString(string(x_buffer));
									UpdateResult(uart, result, _T("Get Buffer ERROR"));
									++errorCount;
									done = true;
								}
							}
						}
						break;
					default:
						AddErrorString("ERROR: S7LITE_UART_GetBuffer2: Unknown result");
						UpdateResult(uart, result, _T("Get Buffer ERROR"));
						++errorCount;
						break;
					}
				}
			}
		}

		// Buffer Test #3 -- sending many words, no words with parity bit set.
		if (0 != (testCnt & 1)) // Save time by running this test every other pass.
		{
			WORD wTxBuffer[150]; // 2015-02-12 Typical maximum length response to a nCompass 6F poll is about 151 (words).
			for (UINT index(0); index < _countof(wTxBuffer); ++index)
			{
				wTxBuffer[index] = (WORD)((index + (UINT)testCnt) & 0xff);
			}

			result = S7LITE_UART_SendBuffer(commp.portno, wTxBuffer, _countof(wTxBuffer));
			if (result != S7DLL_STATUS_OK)
			{
				AddErrorString("ERROR: S7LITE_UART_SendBuffer3");
				UpdateResult(uart, result, _T("Send Buffer ERROR"));
				++errorCount;
			}

			if (IDC_LOOPBACK == true)
			{
				WORD wRxBuffer[_countof(wTxBuffer) + 1];
				memset(wRxBuffer, (int)testCnt / 2, sizeof(wRxBuffer));

				const UINT expectedLen(_countof(wTxBuffer));
				UINT iLen(expectedLen); // Ask only for the amount of data expected.
				result = S7LITE_UART_GetBuffer(commp.portno, wRxBuffer, &iLen);
				{
					switch (result)
					{
					case S7DLL_STATUS_WARNING:
					{
						char x_buffer[256];
						snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_UART_GetBuffer3: %d/%d words received", (int)iLen, (int)expectedLen);
						AddErrorString(string(x_buffer));
						UpdateResult(uart, result, _T("Get Buffer ERROR"));
						++errorCount;
					}
					break;
					case S7DLL_STATUS_ERROR:
						AddErrorString("ERROR:S7LITE_UART_GetBuffer3");
						UpdateResult(uart, result, _T("Get Buffer ERROR"));
						++errorCount;
						break;
					case S7DLL_STATUS_OK:
						{
							bool done(false);
							for (UINT index(0); index < _countof(wTxBuffer) && !done; ++index)
							{
								WORD wTx(wTxBuffer[index]);
								if (IsAxiomtekUart())
								{
									wTx ^= 0x2a00;
								}

								if (wRxBuffer[index] != wTx)
								{
										char x_buffer[256];
										snprintf(x_buffer, sizeof(x_buffer), (char *)"ERROR: S7LITE_UART_GetBuffer3[%u] %4.4x<>%4.4x Data inconsistency",
										(unsigned)index, (unsigned)wRxBuffer[index], (unsigned)wTx);
									AddErrorString(string(x_buffer));
									UpdateResult(uart, result, _T("Get Buffer ERROR"));
									++errorCount;
									done = true;
								}
							}
						}
						break;
					default:
						AddErrorString("ERROR: S7LITE_UART_GetBuffer3: Unknown result");
						UpdateResult(uart, result, _T("UART Unknown ERROR"));
						++errorCount;
						break;
					}
				}
			}
		}

	}
	if (errorCount == 0)
		UpdateResult(uart, result, _T("UART OK"));

	return errorCount;
}

void TestMasterParts::ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess)
{
	for (int blockIndex = 0; blockIndex < NVRAM_MAX_WRITE_BLOCKS; blockIndex++)
	{
		ramAccess.block[blockIndex].buffer = NULL;
		ramAccess.block[blockIndex].length = 0;
		ramAccess.block[blockIndex].offset = 0;
	}
}

void TestMasterParts::SystemRestart()
{
	try
	{
		if (m_phidgetHardware) {
			//Declare Interface Kit Handle
			CPhidgetInterfaceKitHandle ifKit = 0;

			//create the InterfaceKit Object
			CPhidgetInterfaceKit_create(&ifKit);

			//open the interfacekit for device connections
			CPhidget_open((CPhidgetHandle)ifKit, -1);

			const char *err;
			int Result;
			//get the program to wait for an interface kit device to be attached
			Result = (CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000));
			if (Result != 0)
			{
				CPhidget_getErrorDescription(Result, &err);
				string temp;
				temp = "ERROR: Waiting time for Interface Kit Device has exceeded";
				AddErrorString(temp);
			}

			//Switching off the board (Interface Kit Handler, index, outputstate)
			//index = 1 means we are using the second relay on the board. and the
			//connection is NC (Normally Closed) state so outputstate=1 means 
			//disconnecting the board from power supply
			for (int i = 0; i<4; i++)
				CPhidgetInterfaceKit_setOutputState(ifKit, i, 1);

			CPhidget_close((CPhidgetHandle)ifKit);
			CPhidget_delete((CPhidgetHandle)ifKit);
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: SystemRestart: unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"f", x_buffer);
	}
}


// Return the index of the first character that is different between the two strings.
// If the strings are the same, returns the number of characters in the strings.
// If either string is a nullptr then -1 is returned.
int TestMasterParts::StringDiffIndex(LPCSTR string1, LPCSTR string2)
{
	int index(-1);

	if (string1 != nullptr && string2 != nullptr)
	{
		CHAR c1;
		CHAR c2;
		do
		{
			++index;
			c1 = *string1++;
			c2 = *string2++;
		} while (c1 == c2 && c1 != (CHAR)0 && c2 != (CHAR)0);
	}

	return index;
}

S7_Result TestMasterParts::S7_Test_SRAM()
{
	char buffer[1024];
	DWORD errorCount(0);
	S7_Result result_write, result_read;
	S7LITE_SRAMACCESS WriteBlock, ReadBlock;
	ZeroMemory(&WriteBlock, sizeof(WriteBlock));
	ZeroMemory(&ReadBlock, sizeof(ReadBlock));
	BYTE WriteBuffer[NVRAM_MAX_WRITE_BLOCKS][NVRAM_MAX_WRITE];
	BYTE ReadBuffer[NVRAM_MAX_WRITE_BLOCKS][NVRAM_MAX_WRITE];

	USHORT nBlocks = 0;

	typedef enum {
		ttOld = 0,
		ttShort = 1,
		ttLong = 2,
		ttSeq = 3,
	} SramTestType;

	// Default to the (old) original classic test with regards to block length.
	int iCountNum(112);
	int progressBarUpdateMask = 0x0f;
	string BLOCK_NAME(_T(" OLD")); // Include a leading space for convenience.

	const unsigned testType((unsigned)testCnt % 4);
	switch (testType)
	{
	case ttShort:
		iCountNum = 200;
		progressBarUpdateMask = 0x1f;
		BLOCK_NAME = _T(" SHORT"); // Short 1-4 blocks, 2-8 bytes/block, write/read calls.
		break;
	case ttLong:
		iCountNum = 48;
		progressBarUpdateMask = 0x07;
		BLOCK_NAME = _T(" LONG"); // Longer than normal 1-4 blocks, 208-256 bytes/block, write/read calls.
		break;
	case ttSeq:
		iCountNum = 44;
		progressBarUpdateMask = 0x07;
		BLOCK_NAME = _T(" SEQ"); // Sequence of (2-4) 1-block, 198-256 bytes/block write calls, then read all.
		break;
	case ttOld:
	default:
		iCountNum = 112;
		progressBarUpdateMask = 0x0f;
		BLOCK_NAME = _T(" OLD");	// OLD = 1-4 blocks, 2-(256/NumOfBlocks) bytes/block, write/read calls.
		break;
	}

	for (int iCount = 0; iCount < iCountNum; iCount++)
	{
		update_srand();
		if (testType != ttSeq)
		{
			nBlocks = rand() % 4 + 1;

			ClearRamAccessBlocks(WriteBlock);

			for (int iBlock = 0; iBlock < nBlocks; iBlock++)
			{
				UINT wordLength;
				switch (testType)
				{
				case ttShort:
					wordLength = 1 + rand() % 4;
					break;
				case ttLong:
					wordLength = NVRAM_MAX_WORD_WRITE - rand() % 50;
					if (!IsAxiomtekUart())
					{
						// For Braemac, don't exceed NVRAM_MAX_WORD_WRITE for the total length for all blocks.
						wordLength /= nBlocks;
					}
					break;
				case ttOld:
				default:
					wordLength = 1 + rand() % (NVRAM_MAX_WORD_WRITE / nBlocks);
					break;
				}
				//generate 4 random strings
				GetRandomString((char*)WriteBuffer[iBlock], wordLength * sizeof(WORD));
				WriteBlock.block[iBlock].buffer = (PUCHAR)WriteBuffer[iBlock];
				WriteBlock.block[iBlock].length = wordLength;
				WriteBlock.block[iBlock].offset = ((iBlock + iCount) * 0x1000) / 2;
			}

			result_write = S7LITE_SRAM_Write(WriteBlock, NULL, NULL);
			if (result_write == S7DLL_STATUS_ERROR)
			{
				string errorString("ERROR:S7LITE_SRAM_Write" + BLOCK_NAME);
				for (int errBlock = 0; errBlock < nBlocks; errBlock++)
				{
					char x_buffer[256];
					snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
					errorString.append(string(x_buffer));
				}
				AddErrorString(errorString);
				++errorCount;
				break;
			}
			else if (result_write == S7DLL_STATUS_WARNING)
			{
				string errorString("WARNING! S7LITE_SRAM_Write:Busy with anoher Write" + BLOCK_NAME);
				for (int errBlock = 0; errBlock < nBlocks; errBlock++)
				{
					char x_buffer[256];
					snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
					errorString.append(string(x_buffer));
				}
				AddErrorString(errorString);
				++errorCount;
				break;
			}
			//read the blocks of memory written above

			ClearRamAccessBlocks(ReadBlock);

			for (int iBlock = 0; iBlock < nBlocks; iBlock++)
			{
				ReadBlock.block[iBlock].buffer = (PUCHAR)ReadBuffer[iBlock];
				ReadBlock.block[iBlock].length = WriteBlock.block[iBlock].length;
				ReadBlock.block[iBlock].offset = WriteBlock.block[iBlock].offset;

				result_read = S7LITE_SRAM_Read(ReadBlock.block[iBlock], NULL, NULL);
				if (result_read == S7DLL_STATUS_ERROR)
				{
					sprintf(buffer, " can't read block no.= %d in loop = %d", iBlock, iCount);
					string errorString = string("ERROR:S7LITE_SRAM_Read ");
					errorString.append((string)buffer);
					AddErrorString(errorString + BLOCK_NAME);
					++errorCount;
				}
				else if (result_read == S7DLL_STATUS_WARNING)
				{
					++errorCount;
					string errorString(" WARNING!S7LITE_SRAM_Read:Busy with anoher Read" + BLOCK_NAME);
					for (int errBlock = 0; errBlock < nBlocks; errBlock++)
					{
						char x_buffer[256];
						snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
						errorString.append(string(x_buffer));
					}
					AddErrorString(errorString);
				}
			}
			//test the read result
			for (int iBlock = 0; iBlock < nBlocks; iBlock++)
			{
				if (strcmp((char*)ReadBuffer[iBlock], (char*)WriteBuffer[iBlock]) != S7DLL_STATUS_OK)
				{
					string errorString = string("ERROR:S7LITE_SRAM_Write: Read write data inconsistency ");
					errorString += BLOCK_NAME;
					for (int errBlock = 0; errBlock < nBlocks; errBlock++)
					{
						if (errBlock == iBlock)
						{
							char x_buffer[256];
							snprintf(x_buffer, sizeof(x_buffer), (char *)" [%d/%u]",
								(int)StringDiffIndex((LPCSTR)ReadBuffer[iBlock], (LPCSTR)WriteBuffer[iBlock]),
								(unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
							errorString.append(string(x_buffer));
						}
						else
						{
							char x_buffer[256];
							snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
							errorString.append(string(x_buffer));
						}
					}
					AddErrorString(errorString);
					++errorCount;
				}
			}
		}
		else // Sequence of long writes, then read.
		{
			ClearRamAccessBlocks(WriteBlock);
			ClearRamAccessBlocks(ReadBlock);

			const int seqNumMin(2);
			const int seqNumMax(4);
			UINT seqWordLengths[seqNumMax];
			UINT totalSeqWordLength(0);
			BYTE seqWriteBuffer[NVRAM_MAX_WRITE * seqNumMax];
			BYTE seqReadBuffer[NVRAM_MAX_WRITE * seqNumMax];
			memset(seqReadBuffer, (int)testCnt, sizeof(seqReadBuffer));

			// Axiomtek internal limits used with SRAM writes.
			#define qMAX_USB_TX_XP			120				//USB EndP buffer is 128 byte, define the maximun Tx data length
			#define qMAX_USB_TX_W7			60				//USB EndP buffer is 64 byte, define the maximun Tx data length
			// Axiomtek firmware versions: XP=2.xx, W7=3.xx.

			int seqNum = rand() % (seqNumMax - seqNumMin + 1) + seqNumMin;
			for (int seqIndex = 0; seqIndex < seqNum; ++seqIndex)
			{
				seqWordLengths[seqIndex] = NVRAM_MAX_WORD_WRITE - rand() % qMAX_USB_TX_W7;
				totalSeqWordLength += seqWordLengths[seqIndex];
			}
			GetRandomString((char*)seqWriteBuffer, totalSeqWordLength * sizeof(WORD));

			// Write a sequence of long blocks.
			UINT seqWordOffset(0);
			for (int seqIndex = 0; seqIndex < seqNum; ++seqIndex)
			{
				WriteBlock.block[0].buffer = (PUCHAR)(seqWriteBuffer + seqWordOffset * sizeof(WORD));
				WriteBlock.block[0].length = seqWordLengths[seqIndex];
				WriteBlock.block[0].offset = (iCount * 0x1000) / 2 + seqWordOffset;
				seqWordOffset += seqWordLengths[seqIndex];

				result_write = S7LITE_SRAM_Write(WriteBlock, NULL, NULL);
				if (result_write == S7DLL_STATUS_ERROR)
				{
					string errorString("ERROR:S7LITE_SRAM_Write" + BLOCK_NAME);
					for (int errBlock = 0; errBlock < nBlocks; errBlock++)
					{
						char x_buffer[256];
						snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
						errorString.append(string(x_buffer));
					}
					AddErrorString(errorString);
					++errorCount;
					break;
				}
				else if (result_write == S7DLL_STATUS_WARNING)
				{
					string errorString("WARNING! S7LITE_SRAM_Write:Busy with anoher Write" + BLOCK_NAME);
					for (int errBlock = 0; errBlock < nBlocks; errBlock++)
					{
						char x_buffer[256];
						snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
						errorString.append(string(x_buffer));
					}
					AddErrorString(errorString);
					++errorCount;
					break;
				}
			}

			// Read the blocks of memory written above, using a single read.
			ReadBlock.block[0].buffer = (PUCHAR)seqReadBuffer;;
			ReadBlock.block[0].length = totalSeqWordLength;
			ReadBlock.block[0].offset = (iCount * 0x1000) / 2;

			result_read = S7LITE_SRAM_Read(ReadBlock.block[0], NULL, NULL);
			if (result_read == S7DLL_STATUS_ERROR)
			{
				sprintf(buffer, " can't read seq in loop = %d", iCount);
				string errorString = string("ERROR:S7LITE_SRAM_Read ");
				errorString.append((string)buffer);
				AddErrorString(errorString + BLOCK_NAME);
				++errorCount;
			}
			else if (result_read == S7DLL_STATUS_WARNING)
			{
				++errorCount;
				string errorString(" WARNING!S7LITE_SRAM_Read:Busy with anoher Read" + BLOCK_NAME);
				for (int errBlock = 0; errBlock < nBlocks; errBlock++)
				{
					char x_buffer[256];
					snprintf(x_buffer, sizeof(x_buffer), (char *)" [%u]", (unsigned)(WriteBlock.block[errBlock].length * sizeof(WORD)));
					errorString.append(string(x_buffer));
				}
				AddErrorString(errorString);
			}

			//test the read result
			if (strcmp((char*)seqReadBuffer, (char*)seqWriteBuffer) != 0)
			{
				string errorString = string("ERROR:S7LITE_SRAM_Write: Read write data inconsistency ");
				errorString += BLOCK_NAME;
				char x_buffer[256];
				snprintf(x_buffer, sizeof(x_buffer), (char *)" [%d/", (int)StringDiffIndex((LPCSTR)seqReadBuffer, (LPCSTR)seqWriteBuffer));
				errorString.append(string(x_buffer));
				for (int errBlock = 0; errBlock < seqNum; errBlock++)
				{
					if (errBlock != 0)
					{
						errorString += _T('+');
					}
					snprintf(x_buffer, sizeof(x_buffer), (char *)"%u",(unsigned)(seqWordLengths[errBlock] * sizeof(WORD)));
					errorString.append(string(x_buffer));
				}
				errorString += _T(']');
				AddErrorString(errorString);
				++errorCount;
			}
		}
	}

	if (errorCount == 0)
		UpdateResult(sram, result_read, _T("SRAM OK"));
	else 
		UpdateResult(sram, result_read, _T("SRAM ERROR"));

	return errorCount;
}


void TestMasterParts::RTC_PowerCycleTest()
{
	S7_Result result = S7LITE_DLL_Init();
	if (result == S7DLL_STATUS_ERROR)
		AddErrorString("ERROR: Unable to Initialize");


	//RTC Timer Powercycle test
	SYSTEMTIME SystemTime, SysTimeBeforePowerCycle;
	S7_Result result_GetTime;
	result_GetTime = S7LITE_Rtc_GetTime(&SystemTime);
	if (result_GetTime != S7DLL_STATUS_OK)
		AddErrorString("ERROR: S7LITE_Rtc_GetTime");

	char time[50];
	if (ReadDataFromFile((char *)TIME_FILE_NAME, time))
	{
		char StaleData;
		FILETIME filetime1, filetime2;
		sscanf(time, "%4u/%2u/%1u/%2u/%2u/%2u/%2u/%c",
			(unsigned int*)&SysTimeBeforePowerCycle.wYear, (unsigned int*)&SysTimeBeforePowerCycle.wMonth, 
			(unsigned int*)&SysTimeBeforePowerCycle.wDayOfWeek, (unsigned int*)&SysTimeBeforePowerCycle.wDay,
			(unsigned int*)&SysTimeBeforePowerCycle.wHour, (unsigned int*)&SysTimeBeforePowerCycle.wMinute, 
			(unsigned int*)&SysTimeBeforePowerCycle.wSecond, &StaleData);

#ifdef _DEBUG
		//Only for debugging Purposes
		char x_buffer[256];
		snprintf(x_buffer, sizeof(x_buffer), (char *)"Able to Open the %s file", (char *)TIME_FILE_NAME);

		char displaytime[250];
		sprintf(displaytime, " System time Before Powercycle: %4u/%2u/%1u/%2u/%2u/%2u/%2u\n System time after power cycle: %4u/%2u/%1u/%2u/%2u/%2u/%2u",
			SysTimeBeforePowerCycle.wYear, SysTimeBeforePowerCycle.wMonth, SysTimeBeforePowerCycle.wDayOfWeek, SysTimeBeforePowerCycle.wDay,
			SysTimeBeforePowerCycle.wHour, SysTimeBeforePowerCycle.wMinute, SysTimeBeforePowerCycle.wSecond,
			SystemTime.wYear, SystemTime.wMonth, SystemTime.wDayOfWeek, SystemTime.wDay,
			SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
		string display;
		display = displaytime;
		AddErrorString(display);
#endif

		SystemTimeToFileTime(&SysTimeBeforePowerCycle, &filetime1);
		SystemTimeToFileTime(&SystemTime, &filetime2);


		//Check if the data is stale
		if (StaleData == '0')
		{
			//Make the data stale, by making the last character in time to 1
			time[22] = '1';
			WriteDataToFile((char *)TIME_FILE_NAME, time);

			ULARGE_INTEGER nfiletime1, nfiletime2;

			nfiletime1.HighPart = filetime1.dwHighDateTime;
			nfiletime1.LowPart = filetime1.dwLowDateTime;

			nfiletime2.HighPart = filetime2.dwHighDateTime;
			nfiletime2.LowPart = filetime2.dwLowDateTime;

			unsigned long nDiff = nfiletime2.QuadPart - nfiletime1.QuadPart;
			//fReturn is the difference in between filtime1 and filetime2 in milliseconds
			float fReturn = nDiff / 10000.0f;

			//The Reboot time is between 25s to 55s,
			//We added Sleep(30s) to make sure the time.txt file is stored on hard drive
			//So, we expect time difference to be between
			//(25+30)k and (55+30)k milliseconds = 55s to 85s
			const float RTCTimeDelayMin = 55000.0f;
			const float RTCTimeDelayMax = 85000.0f;
			if ((fReturn < RTCTimeDelayMin) || (fReturn > RTCTimeDelayMax))
			{
				AddErrorString("ERROR: RTC timer does not meet the specifications for power cycle test");
			}
			else
			{
				AddErrorString("PASSED: RTC timer meets the specifications for power cycle test");
			}
		}
	}
	else
	{
#ifdef _DEBUG
		char x_buffer[256];
		snprintf(x_buffer, sizeof(x_buffer), (char *)"Unable to open file %s", (char *)(TIME_FILE_NAME));
#endif
	}
}

bool TestMasterParts::WriteDataToFile(char* filename, char *text)
{
	m_IniFile = fopen(filename, "rw");
	if (!m_IniFile)
	{
		return FALSE;
	}
	else
	{
		fprintf(m_IniFile, "%s/n", text );
		fflush(m_IniFile);
		fclose(m_IniFile);
		return TRUE;
	}
}

bool TestMasterParts::ReadDataFromFile(char* filename, char *text)
{
	ifstream ifile(filename);
	if (ifile)
	{
		ifile >> text;
		ifile.close();
		return TRUE;
	}
	else
	{
		ifile.close();
		return FALSE;
	}
}

