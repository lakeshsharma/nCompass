#pragma once

#include "stdafx.h"


using namespace std;

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define CTL_CODE( DeviceType, Function, Method, Access ) ( (DWORD)( \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
) )

extern void displayUsage(string errorMessage);
extern void dispayVersion();
extern void OnTimer(int sig, siginfo_t *si, void *uc);
extern void prtf(char *fmt, char *chk, ...);
extern void LogString(string Identifier, LPCTSTR strToLog, ... );
extern void LogData(char *Identifier, BYTE code, void *data, long dataLength);
extern void LogS7DLLStatus(int status, LPCTSTR strToLog, ... );
extern DWORD GetTickCount();
extern bool doesFileExist(const char *fileName);
extern 	void DumpBuffer(char note, byte lines, byte *buffer, int length, int max_length);
