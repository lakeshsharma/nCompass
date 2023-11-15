#pragma once
#include "stdafx.h"

using namespace std;

extern bool IDC_LOOPBACK;
extern UINT IDC_RTC_PowerCycle;
extern FILE *m_IniFile;
extern UINT IDC_Battery_STATUS;
extern UINT IDC_ERROR_COUNT;

void displayUsage(string errorMessage);
void dispayVersion();
void prtf(char *fmt, char *chk, ...);
std::string timeString();

