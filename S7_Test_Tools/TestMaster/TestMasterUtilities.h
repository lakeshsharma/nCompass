#pragma once

#include "stdafx.h"
using namespace std;

typedef struct {
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME;

DWORD GetTickCount();
void GetLocalTime(SYSTEMTIME *outTime);
void SystemTimeToFileTime(const SYSTEMTIME *x_systemTime, FILETIME *x_fileTime);
long GetLinuxTicks();
