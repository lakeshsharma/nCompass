#pragma	data_seg ("SHARED_USERDLL")

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
UINT	g_uVIAnalogMode;
UINT	g_uVIFormat;
UINT	g_uVOFormat;
UINT	g_uVOIgnoreHotPlug;
UINT	g_uVOMode;
UINT	g_uVSyncPolarity;

SYSTEMTIME	g_stRTCTimeSet;		// time from Sentinel.exe for RTC

// Debugging flags

BOOL	g_bUseLogFile;
BOOL	g_bDisplayStatusDlg;	// Status Dlg (UI) not defined at this time
BOOL	g_bUseTimeStamp;
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

eLEDCONTROL	g_eLRXLed;
eLEDCONTROL	g_eLTXLed;

// Watchdog emulation

UINT	g_uWatchDogInterval;	// in ms
ULONG	g_ulWatchDogCount;		// how many kicks since last set
UINT	g_uTimeBetweenKicks;	// how many ms between last two kicks
BOOL	g_bWatchDogEnabled;

clock_t	g_ctTimeLastKick;
clock_t	g_ctTimeThisKick;

BOOL	g_bInitializationDone = FALSE;

CHAR	g_strLogMsg[0x4000];

#pragma	data_seg()				// end shared data

#pragma comment(linker, "/section:SHARED_USERDLL,RWS")

