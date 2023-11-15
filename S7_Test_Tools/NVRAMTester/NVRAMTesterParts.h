#pragma once

#include "stdafx.h"
#include "TimerThread.h"

#include <phidget21.h>

using namespace std;

/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7

#if(WINVER >= 0x0400)
#define IDCLOSE         8
#define IDHELP          9
#endif /* WINVER >= 0x0400 */


#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define NVRAM_OFFSET_START	0
// For DEVELOPER-ONLY testing you might find it helpful to …
// Change S7_Test_Tools\NVRAMTester\NVRAMTesterParts.h FROM:
#define NVRAM_OFFSET_END   2097152                  // 2MB
//#define NVRAM_OFFSET_END   (2097152 / 256)                       // 2MB //TODO!!! Back out this change!!!                    

#define NVRAM_MAX_READ			256
#define NVRAM_MAX_READ_BLOCKS	1

#define NVRAM_MAX_WRITE			256	// In bytes.
#define NVRAM_MAX_WORD_WRITE	(NVRAM_MAX_WRITE/2)
#define NVRAM_MAX_WRITE_BLOCKS	4

#define NVRAM_SIZE				(NVRAM_OFFSET_END-NVRAM_WAL_SIZE)		// 2MB
#define NVRAM_WAL_SIZE			2048	//2KB
#define NVRAM_WAL_OFFSET_START	(NVRAM_OFFSET_END-NVRAM_WAL_SIZE)

class NVRAMTesterParts
{
public:
	NVRAMTesterParts();
	~NVRAMTesterParts();
	S7_Result initialize();
	bool RunTest(bool manual);
	S7_Result finalize();
	void DetectPhidgetHardware();

private:
	bool S7_Test_NVRAM();
	bool S7_Test_NVRAM_Reading();
	bool S7_Test_NVRAM_Resetting(bool memoryBlockTestPass);
	bool S7_Test_NVRAM_Writing(bool memoryBlockTestPass);
	void S7_Test_NVRAM_Clear(bool memoryBlockTestPass);
	uint S7_Test_NVRAM(LPVOID p);
	void S7_TestCanceled();
	void GetTestNumber();
	void SetTestNumber();

	// NVRAMSpecific Routines
	void Prepare_Test_Buffer(PUCHAR Buffer, int Bufferlen);
	static void update_srand();
	void GetRandomString(char* buffer, int buffersiize);
	void SetCString(std::string &xstring, LPCTSTR str, UINT length);
	void ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess);

	// Posix Timers
	//static int signalInit();
	//static int SetSigTimer(long p_timeVal);
	//static void handler(int sig, siginfo_t *si, void *uc);
	static void SystemRestart();

	static int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr);
	static int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown);

	S7_Result getResults();
};


