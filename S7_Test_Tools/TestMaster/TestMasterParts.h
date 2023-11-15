#pragma once

#include "stdafx.h"
#include "Utilities.h"
#include "IOCTLOffsets.h"
#include "phidget21.h"


using namespace std;

#define WATCHDOG_TIMEOUT	120
#ifdef BRAEMAC_LITE
#define	UART_PORT0		1
#else
#define	UART_PORT0		0
#endif

#define	UART_PORT1		1
#define	UART_PORT2		2
#define	UART_PORT3		3
#define	UART_PORT4		4
#define	UART_PORT5		5
#define	UART_PORT6		6

#define READINTERVAL	24
#define WRITEMULTIPLIER	2
#define WRITECONSTANT	10
#define BUFFER_SIZE		5

#define WMU_UPDATE_RESULT	(WM_APP + 1)
#define WMU_ADD_STRING		(WM_APP + 2)

#define TOTALTESTS			7

#pragma pack(1)

/* Memory size values.  If any of these change on a future product,
* we'll add functions to get the values at run-time.
*
* 1.  Size of EEPROM, in bytes.
* 2.  Amount of EEPROM that is read-only.  Writes to addresses below this value will fail.
* 3.  Amount of battery-backed SRAM, in bytes.  Note that SRAM must be read/written in WORDS, not bytes.
* 4.  Amount of battery-backed SRAM, in WORDS.
* 5.  Number of S7_SRAMACCESSBLOCK write blocks per S5_SRAMACCESS structure.
*/
#define EEPROM_MEMORY_SIZE_BYTES    1024                          // 1
#define EEPROM_READ_ONLY            384                           // 2
#define SRAM_MEMORY_SIZE_BYTES      (2*1024*1024UL)               // 3
#define SRAM_MEMORY_SIZE_WORDS      (SRAM_MEMORY_SIZE_BYTES / 2)  // 4
#define SRAM_MAX_WRITE_BLOCKS       4                             // 5

typedef struct
{
	PWORD   buffer;       // Pointer to buffer of WORDs to receive/supply data for SRAM read/write.
	UINT    offset;       // Offset, in WORD units, into SRAM where read/write will start.
	UINT    length;       // Number of words to read or write.
} S7_SRAMACCESSBLOCK;

typedef struct
{
	S7_SRAMACCESSBLOCK  block[SRAM_MAX_WRITE_BLOCKS];   // Multiple blocks can be written in one function call.
} S7_SRAMACCESS;


//static DWORD WINAPI S5_Start_Test( PVOID threadParam );

/*
typedef enum                 
{                            
    batteryStatusUnknown = 0,
    batteryStatusOkay,       
    batteryStatusLow,        
    batteryStatusNoBattery,  
}BatteryStatus;              
*/

typedef enum 
{
	battery = 0,
	dll,
	backlight,
	sram,
	uart,
	timer,
	watchdog,
} e_Test;


class TestMasterParts
{
public:
	TestMasterParts();
	~TestMasterParts();
	S7_Result initialize();
	S7_Result RunTest(int AllTests, int testFlags[]);
	S7_Result finalize();
	S7_Result S7_Test_DLL(void);
	S7_Result S7_Test_RTCTimer(void);
	S7_Result S7_Test_Backlight(void);
	S7_Result S7_Test_Battery(void);
	S7_Result S7_Test_Watchdog(void);
	S7_Result S7_Test_UART(void);
	S7_Result S7_Test_SRAM(void);
	void RTC_PowerCycleTest();
	void ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess );
	void AddErrorString(const string &name);
	void AddLogString(const string &name);
	void WriteLogString(const string &name);
	void UpdateResult(e_Test testNum, UINT ID, LPCTSTR result);
	void UpdateScreen(int run_total, S7_Result results_errors[], long result_times[]);
	void SystemRestart();
	void Set9BitSerial(bool value);
	void DetectPhidgetHardware();
	void InitTestVars();

private:
	static int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);

	void update_srand();
	void GetRandomString(char* buffer, int buffersiize);

	static int StringDiffIndex(LPCSTR string1, LPCSTR string2);
	void Setstring(string &string, LPCTSTR str, UINT length);
	bool WriteDataToFile(char* filename, char *text);
	bool ReadDataFromFile(char* filename, char *text);
	bool IsAxiomtekUart() const;
	void UpdateTotalsFile(int lastTest, long errors, long totalErrors[], long totalRunErrors);

	string m_productName;
	string m_productVersion;
	bool m_nineBitLoopbackEnabled;
	byte *m_shadowBuffer;
	
	long m_sessionErrors[TOTALTESTS];
	long m_totalSessionErrors;
	long m_sessionTicks[TOTALTESTS];
	long m_totalSessionTicks;
	long m_TotalErrors[TOTALTESTS];
	long m_totalRunErrors;
	int  m_nextTest;
	string m_resultMessage[TOTALTESTS];

	long m_runNumber;

	bool m_timerStarted;
	SYSTEMTIME m_timerStartSystemTime;
	DWORD m_timerStartTickCountMinimum;
	DWORD m_timerStartTickCountMaximum;

	const static ULONG m_maximumAxiomtekUartVersion = 0x13ffff; // This value or less indicates an Axiomtek UART.
	ULONG m_uartVersion;
	int m_prevPWM;
};
