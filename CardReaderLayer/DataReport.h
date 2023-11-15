#pragma once

#include "CardReaderLayerAPI.h"

class CDataReport
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CDataReport(UCHAR event, ULONG length);

	/// <summary>
	/// Copy constructor.
	/// </summary>
	/// <param name="dataReport">IN - object to be copied.</param>
	CDataReport(const CDataReport& dataReport);

	/// <summary>
	/// Deconstructor.
	/// </summary>
	virtual ~CDataReport(void);

	/// <summary>
	/// Gets the 'data report' being stored.
	/// </summary>
	/// <param name="reportBuf">OUT - 'data report' buffer.</param>
	/// <param name="bytes">OUT - bytes read.</param>
	void GetData(PUCHAR reportBuf, DWORD *bytes) const;

protected:
	UCHAR m_data[HID_CARDRDR_DATA_REPORT_SIZE];

private:
	ULONG m_length;
};

class CEvtCardStatus : public CDataReport
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CEvtCardStatus(UCHAR dataValue, ULONG length);
};

class CEvtCardStatusInserted : public CEvtCardStatus
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CEvtCardStatusInserted(bool firstTrack);
};

class CEvtCardStatusRemoved : public CEvtCardStatus
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CEvtCardStatusRemoved();
};

class CEvtErrorData : public CDataReport
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CEvtErrorData(UCHAR dataValue);
};

class CEvtCardData : public CDataReport
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CEvtCardData(PUCHAR dataBuf, UCHAR dataLen, bool firstTrack);
};
