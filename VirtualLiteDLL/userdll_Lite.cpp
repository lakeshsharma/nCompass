#include	"stdafx.h"
#include 	"axhybrid.h"

// Project includes.
#include "binsem.h"
#include	"Hardware/IOCTLOffsets.h"
#include	"CheckSum.h"
#include	"Edid.h"
#include	"S5_DLLNamedPipe.h"

// Third-party includes. As of 2016-11-07, not included via Sentinel/stdafx.h.
#include <fcntl.h>
#include <semaphore.h>

using namespace std;

#define NANOSECONDS_PER_MILLISECOND 1000000
#define NANOSECONDS_PER_SECOND 1000000000
#define USERDLL_API

#if 1//TODO-PORT - Enable the other S5_DLL_LogMsg later in the file and remove this one.
// The Braemac s5dll_api.h declared msg as non-const -- ok for C, but not C++.
S7_Result USERDLL_API S5_DLL_LogMsg(const char *msg, ...)
{
	UNREFERENCED_PARAMETER(msg);
	return S7DLL_STATUS_OK;
}
#endif//TODO-PORT - Enable the other S5_DLL_LogMsg later in the file and remove this one..

const DWORD OpenPipeThreadTimeout = 50;
const DWORD ReadPipeThreadTimeout = 100;

#define USERDLLLITE_API USERDLL_API

#define	CHARSLEFT(s)	(sizeof(s) / sizeof(s[0])) - strlen(s)		// characters left
#define	WCHARSLEFT(s)	(sizeof(s) / sizeof(s[0])) - wcslen(s)		// characters left (wide)
#define	BYTESLEFT(s)	sizeof(s) - (strlen(s) * sizeof(s[0]))		// bytes left
#define	STRINGSIZE(s)	sizeof(s) / sizeof(s[0])					// chars in string buffer

const int NVRAM_WAL_SIZE = 2048;								// 2KB
const int NVRAM_SIZE = (NVRAM_OFFSET_END - NVRAM_WAL_SIZE);	// 2MB - 2KB
const int NVRAM_WAL_OFFSET_START = (NVRAM_OFFSET_END - NVRAM_WAL_SIZE);

void ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess);

#define	FPGABINFILE	"FPGABinFile.bin"
#define	SRAMFILE	"LITESRAM.bin"
#define	USERLITELOG	"UserLiteDLLLog.log"

sem_t *g_hNVRAMMutex = nullptr;
sem_t *g_hCOMMutex = nullptr;
sem_t *g_hSRAMMutex = nullptr;
sem_t *g_hS5LOGMutex = nullptr;
binsem g_hShutdownEvent(0);

LPCTSTR	g_strCOMMutexName = _T("COMMMutex");
LPCTSTR	g_strSRAMMutexName = _T("SRAMMutex");
LPCTSTR	g_strLOGMutexName = _T("S5DLLLogMutex");

// Parameters that can be set or read by Sentinel.exe

BOOLEAN	g_bBatteryStatus;
BOOL	g_bVEBIsPresent;
bool	g_bVEBIsReady;
INT		g_nRTCCalibrationData;
INT		g_nVIHorizontal;
INT		g_nVIVertical;
INT		g_nVOHorizontal;
INT		g_nVOVertical;
UCHAR	g_ucMonConnectMode;
UINT	g_uHSyncPolarity;
UINT	g_uPhase;
USHORT	g_uPWM;					// Backlight PWM
UINT	g_uRtsCtsMode;
UINT	g_uTSBaud;
UINT	g_uTSType;
UINT	g_uTSVendor;
UINT	g_uVIAnalogMode = 0;
UINT	g_uVOIgnoreHotPlug;
UINT	g_uVOMode = 0;
UINT	g_uVSyncPolarity;

SYSTEMTIME	g_stRTCTimeSet;			// time from Sentinel.exe for RTC

// Debugging flags

BOOL	g_bUseLogFile = TRUE;
BOOL	g_bSendToClient = TRUE;
BOOL	g_bUseTimeStamp = TRUE;
BOOL	g_bLogClearRamAccessBlocks;

// UART Config Parameters

// NOTE: Unless explicitly stated otherwise, AX_S5.dll ignores the uart value.
UINT	g_uUART;
BOOL	g_bUARTOpen;
UINT	g_uBaudRate;
UINT	g_uWordLength;
UINT	g_uParity;
UINT	g_uStopBits;
UINT	g_uRTSHandshake;
UINT	g_uDefaultReadInterval;
UINT	g_uDefaultWriteConstant;
UINT	g_uDefaultWriteMultiplier;
BOOL	g_bRTSStatus;
BOOL	g_bEnableTX;

// Watchdog emulation

UINT	g_uWatchDogInterval;	// in ms
ULONG	g_ulWatchDogCount;		// how many kicks since last set
UINT	g_uTimeBetweenKicks;	// how many ms between last two kicks
BOOL	g_bWatchDogEnabled;

clock_t	g_ctTimeLastKick;
clock_t	g_ctTimeThisKick;

bool	g_initializationDone = false;

CHAR	g_strLogMsg[0x4000];

// Formatting utilities for Logging
#if 1//TODO-PORT
static void strcpy_s(char *dest, size_t destSize, const char *src)
{
	strncpy(dest, src, destSize);
	if (destSize > 0)
	{
		dest[destSize - 1] = 0;
	}
}

static size_t strlen_s(const char *str, size_t strsz)
{
	size_t len(0);
	while (len < strsz && 0 != str[len])
	{
		++len;
	}

	return len;
}

static void strcat_s(char *dest, size_t destSize, const char *src)
{
	size_t destLen(strlen_s(dest, destSize));
	size_t remainingLen(destSize - destLen);
	if (remainingLen > 0)
	{
		strncat(dest, src, remainingLen);
	}
}
#endif//TODO-PORT

// MakeHEXDumpBuffer()
//
// Creates a buffer of the style:
// AAAA DD DD DD DD DD DD DD DD <........>
//
// Note: nTextOffset is used to prepend spaces to the output to line up all the columns
const CHAR* MakeHEXDumpBuffer(UCHAR* pIn, UINT uLength, INT nTextOffset = 20, UINT uBytesPerLine = 8)
{
	CHAR	strALine[0x200];				// contains address and data text
	CHAR	strCharSection[0x40];			// printable character section at right
	UINT	uLocalIndex = 0;
	UINT	uWorkIndex;						// for local loops

	static	CHAR	strOutput[0x100000];	// to send to caller

	memset(strOutput, 0, sizeof(strOutput));
	memset(strCharSection, 0, sizeof(strCharSection));
	memset(strALine, 0, sizeof(strALine));

	if (0 == uLength || 0 == pIn)
	{
		return "<No Data>\r\n";
	}

	snprintf(strALine,
		sizeof(strALine) / sizeof(strALine[0]),
		"%04X ",
		uLocalIndex);		// start the very first line

	for (uLocalIndex = 0; uLocalIndex < uLength; uLocalIndex++)
	{
		if (0 != uLocalIndex && 0 == (uLocalIndex) % uBytesPerLine)
		{
			// Starting a new line. Append just processed line to
			// return buffer.

			// Add delimiters to character section line
			{
				CHAR strCharSectionTemp[0x40]; // Temporary working storage for printable character section at right
				strcpy_s(strCharSectionTemp, STRINGSIZE(strCharSectionTemp), strCharSection);
				strcpy_s(strCharSection, STRINGSIZE(strCharSection), " <");
				strcat_s(strCharSection, STRINGSIZE(strCharSection), strCharSectionTemp);
				strcat_s(strCharSection, STRINGSIZE(strCharSection), ">\r\n");
			}

			strcat_s(strALine, STRINGSIZE(strALine), strCharSection);

			// Do a range check
			if (strlen(strOutput) + strlen(strALine) < sizeof(strOutput) / sizeof(strOutput[0]))
			{
				strcat_s(strOutput, STRINGSIZE(strOutput), strALine);

				memset(strALine, 0, sizeof(strALine));
				// Now add any prepend spaces and then the address
				if (0 != nTextOffset)
				{
					for (uWorkIndex = 0; (INT)uWorkIndex < nTextOffset; uWorkIndex++)
					{
						strcat_s(strALine, STRINGSIZE(strALine), " ");
					}
				}							// end if (0 != nTextOffset

				snprintf(strALine + strlen(strALine),
					sizeof(strALine) / sizeof(strALine[0]) - strlen(strALine) - 1,
					"%04X ",
					uLocalIndex);		// Start line with current address (index)

				// Now, initialize the printable character section with periods
				memset(strCharSection, 0, sizeof(strCharSection));
				memset(strCharSection, '.', uBytesPerLine);
				strCharSection[uBytesPerLine] = '\0';
			}					// end if (0 != uLocalIndex...)
		}						// end if (0 != uLocalIndex...)

		// Now filling a new line with data
		snprintf(strALine + strlen(strALine), 4, "%02X ", pIn[uLocalIndex]);

		// Add an extra space every four bytes

		if (0 == (uLocalIndex + 1) % 4)
		{
			strcat_s(strALine, STRINGSIZE(strALine), " ");
		}

		// Add this to the printable character array
		if (isprint(pIn[uLocalIndex]))
		{
			strCharSection[uLocalIndex % uBytesPerLine] = pIn[uLocalIndex];
		}
		else
		{
			strCharSection[uLocalIndex % uBytesPerLine] = '.';
		}
	}							// end for (uLocalIndex = 0; ...)

	// At this point, we must determine if we have a partial line of data
	if (0 != (uLength % uBytesPerLine))
	{
		// First, fill out the data line

		UINT	uSpotsLeft = uBytesPerLine - (uLocalIndex % uBytesPerLine);

		for (uLocalIndex = 0; uLocalIndex < uSpotsLeft; uLocalIndex++)
		{
			strcat_s(strALine, STRINGSIZE(strALine), ".. ");

			// Add an extra space every four columns.
			if (uLocalIndex > 0 && (0 == ((uLocalIndex + 1) % 4)))
			{
				strcat_s(strALine, STRINGSIZE(strALine), " ");
			}
		}

		{
			CHAR strCharSectionTemp[0x40]; // Temporary working storage for printable character section at right
			strcpy_s(strCharSectionTemp, STRINGSIZE(strCharSectionTemp), strCharSection);
			strcpy_s(strCharSection, STRINGSIZE(strCharSection), " <");
			strcat_s(strCharSection, STRINGSIZE(strCharSection), strCharSectionTemp);
			strcat_s(strCharSection, STRINGSIZE(strCharSection), ">");
		}

		strcat_s(strALine, STRINGSIZE(strALine), strCharSection);

		// Do a range check
		if (strlen(strOutput) + strlen(strALine) < sizeof(strOutput) / sizeof(strOutput[0]))
		{
			strcat_s(strOutput, STRINGSIZE(strOutput), strALine);
		}						// end if (0 != uLocalIndex...)
		else
		{
			strcpy_s(strOutput, sizeof(strOutput), "Range error in output string.\r\n");
		}
	}							// end if (0 != (uLocalIndex % uBytesPerLine))
	else
	{
		// At this point, we have all the pieces for the last line,
		// but it is not assembled. We must perform the same logic
		// as above.
		// Add delimiters to character section line

		{
			CHAR strCharSectionTemp[0x40]; // Temporary working storage for printable character section at right
			strcpy_s(strCharSectionTemp, STRINGSIZE(strCharSectionTemp), strCharSection);
			strcpy_s(strCharSection, STRINGSIZE(strCharSection), " <");
			strcat_s(strCharSection, STRINGSIZE(strCharSection), strCharSectionTemp);
			strcat_s(strCharSection, STRINGSIZE(strCharSection), ">");
		}

		strcat_s(strALine, STRINGSIZE(strALine), strCharSection);

		// Do a range check
		if (strlen(strOutput) + strlen(strALine) < sizeof(strOutput) / sizeof(strOutput[0]))
		{
			strcat_s(strOutput, STRINGSIZE(strOutput), strALine);
		}						// end if (strlen(strOutput) + ...)
	}							// end else of if (0 != (uLength % uBytesPerLine)

	return strOutput;			// send back formatted buffer
}								// end MakeHEXDumpBuffer()

// NOTE: Time Between Kicks not currently implemented.

S7_Result USERDLLLITE_API S7LITE_DLL_Init(void)
{
	if (g_initializationDone)
		return S7DLL_STATUS_OK;

	// Threading was handled in AX-S5.dll. We do it here.

	mode_t mutexMode(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	int initialMutexValue(1);
	g_hCOMMutex = sem_open(g_strCOMMutexName, O_CREAT, mutexMode, initialMutexValue);
	g_hSRAMMutex = sem_open(g_strSRAMMutexName, O_CREAT, mutexMode, initialMutexValue);
	g_hS5LOGMutex = sem_open(g_strLOGMutexName, O_CREAT, mutexMode, initialMutexValue);

#ifdef _DEBUG
	g_bUseLogFile = TRUE;
#endif

	g_bUseTimeStamp = TRUE;
	g_bLogClearRamAccessBlocks = FALSE;
	g_bBatteryStatus = TRUE;

	S5_DLL_LogMsg("%s()\r\n", __FUNCTION__);

	S7_Result result = S7DLL_STATUS_OK;

	// Initialize config variables

	g_bVEBIsPresent = FALSE;
	g_bVEBIsReady = FALSE;
	g_uTSVendor = 0;

	g_uWatchDogInterval = 0;	// in ms
	g_ulWatchDogCount = 0;	// how many kicks since last set
	g_uTimeBetweenKicks = 0;	// how many ms between last two kicks
	g_bWatchDogEnabled = FALSE;

	g_ctTimeLastKick = g_ctTimeThisKick = clock();

	// Initialize the SRAM emulation file.
	// Note: If the file exists, then assume it's correct size with
	// correct data.
	// NOTE: In speaking with Tedd and Robert, I'm assuming that references
	// to SRAM and referernces to NVRam are referencing the same thing.

	struct stat fdSRAM;
	if (0 != stat(SRAMFILE, &fdSRAM))
	{
		// At this point, GetFileAttributes() failed. Assume file
		// does not exist.

		// The plan is to create this file once and make it the same size
		// as the SRAM/NVRam. This size is determined by the constant
		// NVRAM_OFFSET_END (nominally 0x02000000 or 2,097,152 bytes)

		FILE* pSRAMFile;
		if (nullptr != (pSRAMFile = fopen(SRAMFILE, "w")))
		{
			int	nCount;

			for (nCount = 0; nCount < NVRAM_OFFSET_END; nCount++)
			{
				if (0 == fwrite("\0", 1, 1, pSRAMFile))
				{
					break;		// if the write fails, just punt
				}
			}					// end for (nCount = 0; ...)
			fclose(pSRAMFile);	// done
		}						// end if (0 == fopen_s(&pSRAMFile,...))
	}							// end if (INVALID_FILE_ATTRIBUTES ...)

	// NOTE: Unless explicitly stated otherwise,
	// AX_S5.dll ignores the g_uUART (uart) value.
	g_uUART = 5;			// default to COM5
	g_bUARTOpen = TRUE;		// emulates AX-S5.dll
	g_bRTSStatus = FALSE;		// RTS inactive
	g_bEnableTX = FALSE;		// TX not enabled

	g_initializationDone = true;	// don't need to do this again.

	return result;
}

S7_Result USERDLLLITE_API S7LITE_DLL_GetDLLVersion(UINT& version)
{
	S5_DLL_LogMsg("%s()\r\n", __FUNCTION__);

	version = 0x0001f7faU;
	return S7DLL_STATUS_OK;
}

S7_Result USERDLLLITE_API S7LITE_DLL_DeInit(void)
{
	S5_DLL_LogMsg("%s()\r\n", __FUNCTION__);

	g_hShutdownEvent.signal(false);

	// Any de-initializaion for this DLL?		// jm 20120807

	if (nullptr != g_hCOMMutex)
	{
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts) != -1)
		{
			ts.tv_nsec += 100 * NANOSECONDS_PER_MILLISECOND;
			ts.tv_sec += ts.tv_nsec / NANOSECONDS_PER_SECOND;
			ts.tv_nsec %= NANOSECONDS_PER_SECOND;
			sem_timedwait(g_hCOMMutex, &ts);
		}

		// We don't care if this timed out or not, we're leaving...
		// Close the mutex

		sem_unlink(g_strCOMMutexName);

		g_hCOMMutex = nullptr;
	}							// end if (I_H_V != g_hCOMMutex)

	if (nullptr != g_hSRAMMutex)
	{
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts) != -1)
		{
			ts.tv_nsec += 100 * NANOSECONDS_PER_MILLISECOND;
			ts.tv_sec += ts.tv_nsec / NANOSECONDS_PER_SECOND;
			ts.tv_nsec %= NANOSECONDS_PER_SECOND;
			sem_timedwait(g_hSRAMMutex, &ts);
		}

		// We don't care if this timed out or not, we're leaving...
		// Close the mutex

		sem_unlink(g_strSRAMMutexName);

		g_hSRAMMutex = nullptr;
	}							// end if (I_H_V != g_hSRAMMutex)

	if (nullptr != g_hS5LOGMutex)
	{
		struct timespec ts;
		if (clock_gettime(CLOCK_REALTIME, &ts) != -1)
		{
			ts.tv_nsec += 100 * NANOSECONDS_PER_MILLISECOND;
			ts.tv_sec += ts.tv_nsec / NANOSECONDS_PER_SECOND;
			ts.tv_nsec %= NANOSECONDS_PER_SECOND;
			sem_timedwait(g_hS5LOGMutex, &ts);
		}

		// We don't care if this timed out or not, we're leaving...
		// Close the mutex

		sem_unlink(g_strLOGMutexName);

		g_hS5LOGMutex = nullptr;
	}							// end if (I_H_V != g_hS5LOGMutex)

	return S7DLL_STATUS_OK;						// jm 20120807
}

S7_Result USERDLLLITE_API S7LITE_SRAM_Read(
	S7LITE_SRAMACCESSBLOCK access,
	void(*fn)(PVOID context),
	PVOID              context
	)
{
	S5_DLL_LogMsg("%s()\r\n", __FUNCTION__);

	S7LITE_SRAMACCESSBLOCK accesslite;

	// Offset and length needs to be converted from word based to byte based
	// to comply with the implementation of the nCompassLite APIs.
	accesslite.offset = access.offset * sizeof(WORD);
	accesslite.buffer = (PUCHAR)access.buffer;
	//change words to byte
	accesslite.length = access.length * 2;

	if ((accesslite.offset + accesslite.length  > NVRAM_OFFSET_END)
		|| (accesslite.offset < NVRAM_OFFSET_START) || accesslite.offset > NVRAM_OFFSET_END)
	{
		return S7DLL_STATUS_ERROR;
	}

	/*
	return S5LITE_SRAM_Read(accesslite,fn,context);
	*/

	// Emulation: NOTE: The AX_S5.dll code ignores fn()
	// At this point, we have a verified accesslite structure.
	// This stub uses a disk file to emulate reading SRAM

	FILE*		pSRAMFile;
	S7_Result	s5Result = S7DLL_STATUS_OK;
	size_t		stReadCount;

	if (0 == accesslite.buffer)
	{
		s5Result = S7DLL_STATUS_ERROR;
	}
	else
	{
		// We could probably enhance this with a timeout
		if (nullptr != g_hSRAMMutex)
		{
			sem_wait(g_hSRAMMutex);
		}

		if (nullptr == (pSRAMFile = fopen(SRAMFILE, "rb")))
		{
			s5Result = S7DLL_STATUS_ERROR;
		}
		else
		{
			// File is open.
			// Now read from the file into the buffer.

			// Position the file
			if (0 == fseek(pSRAMFile, accesslite.offset, SEEK_SET))
			{
				stReadCount = fread(accesslite.buffer,	// destination
					accesslite.length,	// element size
					1,					// how many elements
					pSRAMFile);			// file to read
				//				S5_DLL_LogMsg("%s() "
				//							  "read data = %s\n",
				//							  __FUNCTION__,
				//							  MakeHEXDumpBuffer(accesslite.buffer,
				//											    accesslite.length,
				//											    20,
				//											    8));
			}					// end if (0 == fseek(...))
			fclose(pSRAMFile);
		}						// end else of if (0 != fopen_s())
		if (nullptr != g_hSRAMMutex)
		{
			sem_post(g_hSRAMMutex);
		}
	}							// end else of if (0 == accesslite.buffer)
	return	s5Result;
}

S7_Result USERDLLLITE_API S7LITE_SRAM_Write(
	S7LITE_SRAMACCESS      access,
	void(*fn)(PVOID context),
	PVOID              context
	)
{
	S7_Result	s5Result = S7DLL_STATUS_OK;

	int nDataBlocks = 0;
	bool bOutOfRange = false;

	S7LITE_SRAMACCESS tmp_access;
	ClearRamAccessBlocks(tmp_access);

	for (int i = 0; i < NVRAM_MAX_WRITE_BLOCKS; i++)
	{
		tmp_access.block[nDataBlocks].buffer = (PUCHAR)access.block[i].buffer;
		/*offset and length needs to be converted from word based to byte based
		due to change in implementation of the nCompassLite APIs*/
		tmp_access.block[nDataBlocks].offset = access.block[i].offset * sizeof(WORD);
		tmp_access.block[nDataBlocks].length = access.block[i].length * sizeof(WORD);

		if (access.block[i].buffer != NULL)
		{
			/*offset and length needs to be converted from word based to byte based
			due to change in implementation of the nCompassLite APIs*/
			tmp_access.block[nDataBlocks].offset = access.block[i].offset * sizeof(WORD);
			tmp_access.block[nDataBlocks].length = access.block[i].length * sizeof(WORD);

			//check the boundary conditions for the NVRAM write operation
			if ((tmp_access.block[nDataBlocks].offset + (tmp_access.block[nDataBlocks].length) > NVRAM_SIZE)
				|| (tmp_access.block[nDataBlocks].offset < NVRAM_OFFSET_START) || tmp_access.block[nDataBlocks].length > NVRAM_MAX_WRITE)
			{
				bOutOfRange = true;
			}
			nDataBlocks++;
		}
	}

	// Just write the data to out disk file emulation of SRAM
	// NOTE: This assumes the AX_S5.dll ignores the fn() parameter.

	if (nDataBlocks > 0 && !bOutOfRange)
	{
		// At this point assume all the ranges have been checked and
		// we're good to go. Open the file, write the data, and close the file.

		FILE*		pSRAMFile;
		size_t		stWriteCount;
		int			nBlockCounter;

		// We could probably enhance this with a timeout
		if (nullptr != g_hSRAMMutex)
		{
			sem_wait(g_hSRAMMutex);
		}

		if (nullptr == (pSRAMFile = fopen(SRAMFILE, "a")))
		{
			s5Result = S7DLL_STATUS_ERROR;
		}
		else
		{
			// File is open, write data

			for (nBlockCounter = 0; nBlockCounter < NVRAM_MAX_WRITE_BLOCKS; nBlockCounter++)
			{
				if (0 != access.block[nBlockCounter].buffer)	// make sure have data
				{
					if (0 != fseek(pSRAMFile, access.block[nBlockCounter].offset, SEEK_SET))
					{
						// At this point, a null buffer is not an erorr
						continue;	// an error positioning in file, just punt this one
					}

					stWriteCount = fwrite(access.block[nBlockCounter].buffer,	// data source
						access.block[nBlockCounter].length,	// length data
						1,									// how many blks
						pSRAMFile);
					S5_DLL_LogMsg("%s() offset = <0x%0X>, length = <%d>\r\n",
						__FUNCTION__,
						access.block[nBlockCounter].offset,
						access.block[nBlockCounter].length);
					S5_DLL_LogMsg("%s() "
						"write data = %s\r\n",
						__FUNCTION__,
						MakeHEXDumpBuffer((PUCHAR)access.block[nBlockCounter].buffer,
						access.block[nBlockCounter].length,
						47,
						8));
				}					// end if (0 != ...buffer)
			}
			fclose(pSRAMFile);
		}						// end else of if (0 != fopen_s(...))
		if (nullptr != g_hSRAMMutex)
		{
			sem_post(g_hSRAMMutex);
		}
	}							// end if (nDataBlocks > 0 && !bOutOfRange)

	return s5Result;
}

int32_t AXVM_UpdateFirmware(const char *filename, void (*progress_callback)(int progress))
{
	return 0;
}

S7_Result S7LITE_Firmware_Version(char *version, size_t *versionSize)
{
	return S7DLL_STATUS_OK;
}

S7_Result S7LITE_FirmwareUpgradeByFile(char *filename)
{
	return S7DLL_STATUS_OK;
}

S7_Result S7LITE_MousePointer_Show(void)
{
	return S7DLL_STATUS_OK;
}

S7_Result S7LITE_MousePointer_Hide(void)
{
	return S7DLL_STATUS_OK;
}

//The below functions are specific to WAL implementation
void ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess)
{
	if (TRUE == g_bLogClearRamAccessBlocks)
	{
		S5_DLL_LogMsg("%s()\r\n", __FUNCTION__);
	}

	for (int blockIndex = 0; blockIndex < NVRAM_MAX_WRITE_BLOCKS; blockIndex++)
	{
		ramAccess.block[blockIndex].buffer = NULL;
		ramAccess.block[blockIndex].length = 0;
		ramAccess.block[blockIndex].offset = 0;
	}
}
		
int32_t AXVM_GetFirmwareVersion(struct axvm_version *version)
{
	return VM_Success;
}

int32_t AXVM_OpenDevice(void)
{
	return VM_Success;
}

int32_t AXVM_CloseDevice(void)
{
	return VM_Success;
}

int32_t AXVM_BeginAnimation(void)
{
	return VM_Success;
}

int32_t AXVM_RunAnimation(uint8_t unit,int32_t duration)
{
	return VM_Success;
}

int32_t AXVM_SetAlpha(uint8_t value)
{
	return VM_Success;
}

int32_t AXVM_SetSize(uint8_t display,uint16_t width,uint16_t height)
{
	return VM_Success;
}

int32_t AXVM_SetPosition(uint8_t display,uint16_t x,uint16_t y)
{
	return VM_Success;
}

int32_t AXVM_GetDisplayResolution(uint8_t display,uint16_t *width,uint16_t *height)
{
	return VM_Success;
}

int32_t AXVM_EnableDisplay(uint8_t displays)
{
	return VM_Success;
}

int32_t AXVM_GetVideoFormat(uint8_t display, uint8_t *format)
{
	return VM_Success;
}

int32_t AXVM_GetDetectedVideoFormat(uint8_t display, uint8_t *format)
{
	return VM_Success;
}


int32_t AXVM_GetTSDetected(uint32_t *type, uint32_t *vendor, uint32_t *product)
{
	return VM_Success;
}

int32_t AXVM_GetNumberOfVideoTableEntries(uint32_t *n_modes)
{
	return VM_Success;
}

int32_t AXVM_GetVideoTableEntry(uint32_t mode, struct video_mode *entry)
{
	return VM_Success;
}

int32_t AXVM_GetVIAnalogMode(uint32_t *mode)
{
	return VM_Success;
}

int32_t AXVM_SetVOPos(int32_t horizontal, int32_t vertical)
{
	return VM_Success;
}

int32_t AXVM_GetVOPos(int32_t *horizontal, int32_t *vertical)
{
	return VM_Success;
}

int32_t AXVM_SetVIAnalogPos(int32_t horizontal, int32_t vertical)
{
	return VM_Success;
}

int32_t AXVM_GetVIAnalogPos(int32_t *horizontal, int32_t *vertical)
{
	return VM_Success;
}

int32_t AXVM_GetVOMode(uint32_t *mode)
{
	return VM_Success;
}

int32_t AXVM_SetVOMode(uint32_t mode)
{
	return VM_Success;
}

int32_t AXVM_SetVideoFormat(uint8_t display, uint8_t format)
{
	return VM_Success;
}

int32_t AXVM_SetVIAnalogMode(uint32_t mode)
{
	return VM_Success;
}

int32_t AXVM_GetTSRotation(uint8_t *rotation)
{
	return VM_Success;
}

int32_t AXVM_SetTSRotation(uint8_t rotation)
{
	return VM_Success;
}

int32_t AXVM_GetTSVendor(uint32_t *vendor)
{
	return VM_Success;
}


int32_t AXVM_SetTSVendor(uint32_t vendor)
{
	return VM_Success;
}

int32_t AXVM_GetTSBaudRate(uint32_t *baud_rate)
{
	return VM_Success;
}

int32_t AXVM_SetTSBaudRate(uint32_t baud_rate)
{
	return VM_Success;
}

int32_t AXVM_GetTSSuppotList(uint16_t *size, struct ts_info **ts_list)
{
	return VM_Success;
}

int32_t AXVM_FactoryReset(void)
{
	return VM_Success;
}


int32_t AXVM_DebugDump(const char *filename)
{
	return VM_Success;
}

S7_Result S7LITE_RemoteLCD_SetBacklightPWM(UINT brightness)
{
    (void)brightness;
    return S7DLL_STATUS_OK;
}

S7_Result S7LITE_Battery_GetStatus(BOOLEAN* pStatus)
{
    (void)pStatus;
    return S7DLL_STATUS_OK;
}
S7_Result S7LITE_Rtc_SetTime(SYSTEMTIME systemTime)
{
    (void)systemTime;
    return S7DLL_STATUS_OK;
}

int32_t AXVM_GetTouchFirmwareVersion(struct axvm_touch_version *touch_version)
{
	(void)touch_version;
    return VM_Success;
}

int32_t AXVM_DebugUSBTouchFirmware(struct axvm_touch_debug *touch_debug, int touch_region)
{
	(void)touch_debug;
	(void)touch_region;
    return VM_Success;
}
