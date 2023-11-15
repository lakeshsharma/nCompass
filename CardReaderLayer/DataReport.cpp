#include "stdafx.h"
#include "DataReport.h"

CDataReport::CDataReport(UCHAR event, ULONG length) : m_length(length)
{
	memset(m_data, 0, sizeof(m_data));
	m_data[HID_CARDRDR_DATA_REPORT_OFFSET_EVENT] = event;
}

CDataReport::CDataReport(const CDataReport& dataReport) : m_length(dataReport.m_length)
{
	memcpy(m_data, dataReport.m_data, dataReport.m_length);
}

CDataReport::~CDataReport(void)
{
}

void CDataReport::GetData(PUCHAR reportBuf, DWORD *bytes) const
{
	if (NULL != reportBuf) // reportBuf == NULL -> just return bytes
	{
		for (unsigned n = 0; n < m_length; n++)
		{
			reportBuf[n] = m_data[n];
		}
	}
	*bytes = m_length;
}

CEvtCardStatus::CEvtCardStatus(UCHAR dataValue, ULONG length) : CDataReport(HID_CARDRDR_EVT_CARD_STATUS, length)
{
	m_data[HID_CARDRDR_DATA_REPORT_OFFSET_VALUE] = dataValue;
}

CEvtCardStatusInserted::CEvtCardStatusInserted(bool firstTrack)
	: CEvtCardStatus(HID_CARDRDR_DATA_REPORT_VALUE_INSERTED, HID_CARDRDR_EVT_LENGTH_CARD_STATUS_INSERTED)
{
	if (firstTrack)
	{
		m_data[HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] = HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK1_AVAILABLE_FLAG;
	}
	else
	{
		m_data[HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK_OFFSET] = HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK2_AVAILABLE_FLAG;
	}
}

CEvtCardStatusRemoved::CEvtCardStatusRemoved()
	: CEvtCardStatus(HID_CARDRDR_DATA_REPORT_VALUE_REMOVED, HID_CARDRDR_EVT_LENGTH_CARD_STATUS_REMOVED)
{
}

CEvtErrorData::CEvtErrorData(UCHAR dataValue) : CDataReport(HID_CARDRDR_EVT_ERROR_DATA, HID_CARDRDR_EVT_LENGTH_ERROR_DATA)
{
	m_data[HID_CARDRDR_DATA_REPORT_OFFSET_VALUE] = dataValue;
}

CEvtCardData::CEvtCardData(PUCHAR dataBuf, UCHAR dataLen, bool firstTrack)
	: CDataReport(HID_CARDRDR_EVT_CARD_DATA, HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + dataLen)
{
	m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX1] = HID_CARDRDR_DATA_REPORT_VALUE_NONE; // Not used
	m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX2] = HID_CARDRDR_DATA_REPORT_VALUE_NONE; // Not used
	m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_LENGTH] = dataLen;

	if (firstTrack)
	{
		m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_TRACK] = HID_CARDRDR_DATA_REPORT_CARD_DATA_VALUE_TRACK1_USED;
	}
	else
	{
		m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_TRACK] = HID_CARDRDR_DATA_REPORT_CARD_DATA_VALUE_TRACK2_USED;
	}

	// Allow for invalid data
	if (NULL != dataBuf)
	{
		for (DWORD n = 0; n < dataLen; n++)
		{
			m_data[HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA + n] = dataBuf[n];
		}
	}
}
