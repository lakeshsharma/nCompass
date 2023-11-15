
#include "../stdafx.h"
#include "../Test9BitnCompass.h"
#include "SASSerialWrapper.h"


#pragma pack(1)

//Sentinel 5
#define SASUART 1

//Sentinel Lite
//#define SASUART 0

#define SASWORDLENGTH 9
#define SASBAUDRATE 19200
#define SASREADINTERVAL 24
#define SASWRITEMULTIPLIER 2
#define SASWRITECONSTANT 10
#define SER9BIT_NOMARK 0x00
#define SER9BIT_MARK 0xff


enum COMM_DATA_TYPE
{
	DATA_WRITE = 0x1,
	DATA_READ = 0x2,
	DATA_COM1 = 0x4,
	DATA_COM2 = 0x8,
	DATA_ETHERNET = 0x10,
	DATA_NDI = 0x20
};

CSASSerialWrapper::CSASSerialWrapper(void)
{
	m_LastWrittenBufer = NULL;
	m_LastReadBufer = NULL;
}

CSASSerialWrapper::~CSASSerialWrapper(void)
{
	CloseSAS();
}

S7_Result CSASSerialWrapper::OpenSAS(void)
{
	S7_Result result;

	// Not is the this version of Library
	//result = S7LITE_UART_Init(SASUART);
	//LogStatus(result, L"SASSerialWrapper: Initializing UART driver");


	result = S7LITE_UART_SetMode(SASUART, SASWORDLENGTH, NO_PARITY, STOP_BIT_1, SERIAL_NO_HANDSHAKE);
	LogS7DLLStatus(result,
		(char *)"SASSerialWrapper: Setting UART driver to wordlength = %d, parity = %d, stop bit = %d, Handshake = %d",
		(int)SASWORDLENGTH, (int)NO_PARITY, (int)STOP_BIT_1, (int)SERIAL_NO_HANDSHAKE);


	result = S7LITE_UART_SetBaudRate(SASUART, SASBAUDRATE);
	LogS7DLLStatus(result, (char *)"SASSerialWrapper: Setting UART driver baudrate = %d", SASBAUDRATE);

	result = S7LITE_UART_SetTimeouts(SASUART, SASREADINTERVAL, SASWRITEMULTIPLIER, SASWRITECONSTANT);
	LogS7DLLStatus(result,
		(char *)"SASSerialWrapper: Setting UART timeouts readinterval = %d, writemultiplier = %d, writeconstant = %d",
		SASREADINTERVAL, SASWRITEMULTIPLIER, SASWRITECONSTANT);

	//result = S7LITE_UART_TxLedControl(SASUART, LED_AUTO);
	//LogStatus(result, L"SASSerialWrapper: Turning off UART transmission led");

	//result = S7LITE_UART_RxLedControl(SASUART, LED_AUTO);
	//LogStatus(result, L"SASSerialWrapper: Turning off UART read led");

	//result = S7LITE_UART_EnableTx(SASUART, TRUE);
	//LogStatus(result, L"SASSerialWrapper: Enabling UART transmission");

	return result;
}

// Closes SAS Serial Port
void CSASSerialWrapper::CloseSAS(void)
{
	//S7_Result result = S7LITE_UART_EnableTx(SASUART, FALSE);
	//LogStatus(result, L"SASSerialWrapper: Closing UART transmission");
}


S7_Result CSASSerialWrapper::Write(byte * buffer, DWORD bufferLen, bool mark, DWORD *numberBytesWritten)
{
	Ser9BitData *wBuffer = new Ser9BitData[bufferLen];

	// copy buffer into wBuffer
	for (DWORD i = 0; i<bufferLen; i++)
	{
		wBuffer[i].sp = buffer[i];
		wBuffer[i].cp[1] = SER9BIT_NOMARK;
	}

	// set mark bit for first byte
	if (mark)
		wBuffer[0].cp[1] = SER9BIT_MARK;

	if (NULL != numberBytesWritten)
		*numberBytesWritten = 0;

	S7_Result result;
	result = S7LITE_UART_ClearBuffers(SASUART, CLR_RX_BUFFER);
	LogS7DLLStatus(result, (char *)"SASSerialWrapper: Clearing read buffers");

	// write data
	if (1 == bufferLen)
	{
		result = S7LITE_UART_SendWord(SASUART, wBuffer[0].sp);
	}
	else
	{
		result = S7LITE_UART_SendBuffer(SASUART, (USHORT *)wBuffer, bufferLen);
	}

	delete[] wBuffer;

	if (S7DLL_STATUS_OK != result)
		LogS7DLLStatus(result, (char *)"SASSerialWrapper: Writing data");
	else
	{
		if (NULL != numberBytesWritten)
		{
			*numberBytesWritten = bufferLen;
#ifdef _DUMP
			DumpBuffer('W', 1, (byte *)buffer, bufferLen, 40);
#endif
			// Log written data
			if (m_LastWrittenBufer != NULL)
			{
				if (memcmp(m_LastWrittenBufer, buffer, bufferLen) != 0)
				{
					memcpy(m_LastWrittenBufer, buffer, bufferLen);
					LogData((char *)"SASDATA", (BYTE)(DATA_WRITE | DATA_COM1), (void *)buffer, (long)bufferLen);
				}
			}
			else
			{
				m_LastWrittenBufer = new byte[bufferLen];
				memcpy(m_LastWrittenBufer, buffer, bufferLen);
				LogData((char *)"SASDATA", (BYTE)(DATA_WRITE | DATA_COM1), (void *)buffer, (long)bufferLen);
			}
		}
	}

	return result;
}

// Read bufferLen characters from Serial Port into buffer
S7_Result CSASSerialWrapper::Read(byte * buffer, DWORD bufferLen, DWORD * numberBytesRead)
{
	const DWORD MAXREADTIME = 40; // wait for 40 milliseconds

	Ser9BitData *rBuffer = new Ser9BitData[bufferLen];
	DWORD startTicks = GetTickCount();
	UINT lengthRead = bufferLen;
	UINT totalRead = 0;

	S7_Result result = S7DLL_STATUS_OK;

	if (NULL != numberBytesRead)
		*numberBytesRead = 0;

	// read data
	while (totalRead < bufferLen && GetTickCount() < startTicks + MAXREADTIME)
	{
#ifdef _DEBUG
		if (totalRead > 0)
			prtf((char *)"Short packet trying to get rest, missing %d of %d\n", (char *)"b", bufferLen - totalRead, bufferLen);
#endif
		lengthRead = bufferLen - totalRead;

		if (1 < lengthRead)
		{
			result = S7LITE_UART_GetBuffer(SASUART, (USHORT *)(rBuffer + totalRead), &lengthRead);
		}
		else
		{
			result = S7LITE_UART_GetWord(SASUART, &(rBuffer[totalRead].sp));
			if (S7DLL_STATUS_OK != result)
				lengthRead = 0;
			else
				lengthRead = 1;
		}
		totalRead = totalRead + lengthRead;
	}
	lengthRead = totalRead;

	// put data into buffer
	for (DWORD i = 0; i < lengthRead; i++)
	{
		buffer[i] = (byte)rBuffer[i].sp;
	}

	if (0 < lengthRead)
	{
		if (NULL != numberBytesRead)
		{
			*numberBytesRead = lengthRead;
#ifdef _DUMP
			DumpBuffer('R', 1, (byte *)buffer, lengthRead, 40);
#endif
		}

		// Log read data
		if (m_LastReadBufer != NULL)
		{
			if (memcmp(m_LastReadBufer, buffer, lengthRead) != 0)
			{
				memcpy(m_LastReadBufer, buffer, lengthRead);
				LogData((char *)"SASDATA", (BYTE)(DATA_READ | DATA_COM1), (void *)buffer, (long)lengthRead);
			}
		}
		else
		{
			m_LastReadBufer = new byte[lengthRead];
			memcpy(m_LastReadBufer, buffer, lengthRead);
			LogData((char *)"SASDATA", (BYTE)(DATA_READ | DATA_COM1), (void *)buffer, (long)lengthRead);
		}
	}
	delete[] rBuffer;

	return result;
}
