#include "stdafx.h"
#include "Sentinel.h"

BEGIN_MESSAGE_MAP(CSentinelApp, CWinApp)
END_MESSAGE_MAP()

BOOL CSentinelApp::InitInstance()
{
	return FALSE;
}

CSentinelApp theApp;

// 2014-06-18 Same as Sentinel.cpp implementation.
CSentinelApp::CSentinelApp() :
	CWinApp(),
	m_comInitialized(false)
{
	m_internals = NULL;
	m_egmProtocol = NULL;
	m_systemProtocol = NULL;
	m_speedMediaProtocol = NULL;
	m_progressiveProtocol = NULL;
	m_dlg = NULL;
	m_loggingInitialized = false;

	m_Logger = NULL;
	m_endCalibrationTask = false;
}

/// <summary>
/// Format information for logging without logging it.
/// </summary>
/// <param name="Identifier">IN - logging identifier.</param>
/// <param name="format">IN - format specifier for the information to be logged.  MUST be followed by a correct argument list </param>
void CSentinelApp::LogString(CString Identifier, LPCTSTR format, ...)
{
	if (!Identifier.IsEmpty())
	{
		if (format == nullptr)
		{
			LogStringNoFormat(Identifier, nullptr);
		}
		else
		{
			CString logStr;
			va_list args;
			va_start(args, format);
			logStr.FormatV(format, args);
			va_end(args);
			LogStringNoFormat(Identifier, logStr);
		}
	}
}

void CSentinelApp::LogStringNoFormat(CString Identifier, LPCTSTR logStr)
{
	CString outputString(Identifier);

	if (logStr == nullptr)
	{
		outputString.Append(L": <nullptr>\n");
	}
	else
	{
		outputString.AppendFormat(L": %s\n", logStr);
	}

	OutputDebugString(outputString);
}

void CSentinelApp::LogData(const CString &Identifier, BYTE code, const void *data, long dataLength)
{
}

void CSentinelApp::LogS5DLLStatus(S5_Result status, LPCTSTR format, ...)
{
}

void CSentinelApp::LogS5DLLStatus(S5_Result status, LPCTSTR format, va_list argList)
{
}

int CSentinelApp::Filter(unsigned int, struct _EXCEPTION_POINTERS*, const wchar_t*)
{
	return EXCEPTION_EXECUTE_HANDLER; 
}
