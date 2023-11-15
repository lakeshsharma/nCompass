#include "stdafx.h"
#include "CardReaderSimulator.h"
#include "CardReaderLayerAPI.h"

const LPCWSTR CCardReaderSimulator::UIC = L"uic";
const LPCWSTR CCardReaderSimulator::XST = L"xst";
const LPCWSTR CCardReaderSimulator::TEST = L"test";

CCardReaderSimulator* CCardReaderSimulator::m_instance = NULL;
::CCriticalSection CCardReaderSimulator::m_cs;

CCardReaderSimulator& CCardReaderSimulator::Instance()
{
	m_cs.Lock();
	if (NULL == m_instance)
	{
		m_instance = new CCardReaderSimulator();
	}
	m_cs.Unlock();
	return *m_instance;
}

void CCardReaderSimulator::CleanupInstance()
{
	m_cs.Lock();
	if (NULL != m_instance)
	{
		m_instance->End();
		delete m_instance;
		m_instance = NULL;
	}
	m_cs.Unlock();
}

CCardReaderSimulator::CCardReaderSimulator(void) : m_testDir(L"")
{
	Clear();
}

CCardReaderSimulator::~CCardReaderSimulator(void)
{
	End();
}

void CCardReaderSimulator::Clear()
{
	m_enabled = false;
	m_readCardData = false;
	memset(m_colorArray, 0, sizeof(m_colorArray));
	m_featureToError = 0;
	m_featureError = ERROR_SUCCESS;
	m_reportError = ERROR_SUCCESS;
	m_testError = ERROR_SUCCESS;
	m_featureErrorTicks = INFINITE;
	m_reportErrorTicks = INFINITE;
	ClearCardData();
}

void CCardReaderSimulator::Start(LPCWSTR testDir)
{
	End();
	m_cs.Lock();
	m_testDir = testDir;
	::CreateDirectory(m_testDir, NULL);
	m_cs.Unlock();
}

void CCardReaderSimulator::End()
{
	m_cs.Lock();
	m_uicDevice.Disconnect();
	m_xstDevice.Disconnect();
	::RemoveDirectory(m_testDir);
	Clear();
	m_cs.Unlock();
}

bool CCardReaderSimulator::GetConnected(USHORT vendor)
{
	bool connected = false;
	m_cs.Lock();
	switch (vendor)
	{
		case USB_VENDOR_ID_UIC:
		case USB_VENDOR_ID_TEST:
			connected = m_uicDevice.IsConnected();
			break;
		case USB_VENDOR_ID_XST:
			connected = m_xstDevice.IsConnected();
			break;
		default:
			connected = false;
	}
	m_cs.Unlock();
	return connected;
}

void CCardReaderSimulator::SetConnected(USHORT vendor, bool flag)
{
	m_cs.Lock();

	if (!flag)
	{
		Clear();
	}

	switch (vendor)
	{
		case USB_VENDOR_ID_UIC:
		case USB_VENDOR_ID_TEST:
			if (flag)
			{
				CString mockFile = m_testDir + L"\\" + UIC;
				m_uicDevice.Connect(mockFile);
			}
			else
			{
				m_uicDevice.Disconnect();
			}
			break;
		case USB_VENDOR_ID_XST:
			if (flag)
			{
				CString mockFile = m_testDir + L"\\" + XST;
				m_xstDevice.Connect(mockFile);
			}
			else
			{
				m_xstDevice.Disconnect();
			}
			break;
		default:
			ASSERT(false);
	}

	m_cs.Unlock();
}

void CCardReaderSimulator::SetEnabled(bool flag)
{
	m_cs.Lock();
	m_enabled = flag;
	m_cs.Unlock();
}

DWORD CCardReaderSimulator::SetColor(const UCHAR* colorArray, unsigned length, unsigned start /*= 0*/)
{
	DWORD error = ERROR_SUCCESS;

	m_cs.Lock();
	if (start + length > HID_CARDRDR_COLOR_ARRAY_SIZE)
	{
		error = ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		for (unsigned n = start; n < length; n++)
		{
			m_colorArray[n] = colorArray[n];
		}
	}
	m_cs.Unlock();

	return error;
}

DWORD CCardReaderSimulator::GetCardData(PUCHAR reportBuf, LPDWORD bytes)
{
	DWORD error = ERROR_SUCCESS;

	m_cs.Lock();
	if (m_dataReports.empty())
	{
		*bytes = 0;
	}
	else
	{
		m_dataReports.front().GetData(reportBuf, bytes);
		m_dataReports.pop();
	}
	m_cs.Unlock();

	return error;
}

void CCardReaderSimulator::ClearCardData()
{
	m_cs.Lock();
	while (!m_dataReports.empty())
	{
		m_dataReports.pop();
	}
	m_cs.Unlock();
}

void CCardReaderSimulator::PushEvtStatusInserted(bool firstTrack)
{
	m_cs.Lock();
	m_dataReports.push(CEvtCardStatusInserted(firstTrack));
	m_cs.Unlock();
}

void CCardReaderSimulator::PushEvtStatusRemoved()
{
	m_cs.Lock();
	m_dataReports.push(CEvtCardStatusRemoved());
	m_cs.Unlock();
}

void CCardReaderSimulator::PushEvtCardData(PUCHAR dataBuf, UCHAR dataLen, bool firstTrack)
{
	m_cs.Lock();
	m_dataReports.push(CEvtCardData(dataBuf, dataLen, firstTrack));
	m_cs.Unlock();
}

void CCardReaderSimulator::PushEvtErrorData(UCHAR dataValue)
{
	m_cs.Lock();
	m_dataReports.push(CEvtErrorData(dataValue));
	m_cs.Unlock();
}

void CCardReaderSimulator::SetFeatureError(UCHAR feature, DWORD error, DWORD tickCount /*= INFINITE*/)
{
	m_cs.Lock();
	m_featureToError = feature;
	m_featureError = error;
	m_featureErrorTicks = tickCount;
	m_cs.Unlock();
}

DWORD CCardReaderSimulator::GetFeatureError(UCHAR feature) const
{
	m_cs.Lock();
	DWORD error = ERROR_SUCCESS;
	if (feature == m_featureToError)
	{
		error = m_featureError;
	}
	m_cs.Unlock();
	return error;
}

DWORD CCardReaderSimulator::GetFeatureErrorTickCount(UCHAR feature) const
{
	m_cs.Lock();
	DWORD ticks = 0;
	if (feature == m_featureToError)
	{
		ticks = m_featureErrorTicks;
	}
	m_cs.Unlock();
	return ticks;
}

void CCardReaderSimulator::SetReportError(DWORD error, DWORD tickCount /*= INFINITE*/)
{
	m_cs.Lock();
	m_reportError = error;
	m_reportErrorTicks = tickCount;
	m_cs.Unlock();
}
