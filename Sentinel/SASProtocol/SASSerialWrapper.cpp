#include "stdafx.h"
#include "SASSerialWrapper.h"
#include "Config/Config.h"
#include "GlobalDefs.h"
#include "Logging/LogString.h"
#include "Utilities.h"

using namespace std;


const short CSASSerialWrapper::MS200_POLLING_SPEED_MILLISECONDS;
const short CSASSerialWrapper::MS40_POLLING_SPEED_MILLISECONDS;
const short CSASSerialWrapper::DEFAULT_POLLING_SPEED_MILLISECONDS;

CSASSerialWrapper::CSASSerialWrapper(IOperatingSystem &operatingSystem) :
	m_pollingSpeed(DEFAULT_POLLING_SPEED_MILLISECONDS),
	m_lastGameAddress(DEFAULT_LAST_GAME_ADDRESS),
	m_lastTransmitStartTickCount(CUtilities::GetTickCount()),
	m_lastTransmitDurationTickCount(DEFAULT_LAST_TRANSMIT_DURATION_TICK_COUNT),
	m_lastReceivedByteTickCount(m_lastTransmitStartTickCount),
	m_operatingSystem(operatingSystem)
{
}

CSASSerialWrapper::~CSASSerialWrapper()
{
}

void CSASSerialWrapper::OpenSAS(void)
{
	S7_Result result;

	result = m_operatingSystem.SLITE_UART_Init(SASUART);
	LogS5DLLStatus(result, "S5_UART_Init %", SASUART);

	result = m_operatingSystem.SLITE_UART_SetMode(SASUART, SASWORDLENGTH, NO_PARITY, STOP_BIT_1, SERIAL_NO_HANDSHAKE);
	LogS5DLLStatus(result,
		"SASSerialWrapper: Setting UART driver to wordlength = %u, parity = %u, stop bit = %u serial_no_handshare = %u",
		SASWORDLENGTH, NO_PARITY, STOP_BIT_1, SERIAL_NO_HANDSHAKE);

	result = m_operatingSystem.SLITE_UART_SetBaudRate(SASUART, SASBAUDRATE);
	LogS5DLLStatus(result, "SASSerialWrapper: Setting UART driver baudrate = %u", SASBAUDRATE);

	UpdateSasReadTimeout();
}

void CSASSerialWrapper::UpdateSasReadTimeout()
{
	S7_Result result;
	UINT sasReadTimeout(SASREADINTERVAL * CConfig::m_sasReadTimeOutMultiplier);
	result = m_operatingSystem.SLITE_UART_SetTimeouts(SASUART, sasReadTimeout, SASWRITEMULTIPLIER, SASWRITECONSTANT);
	LogS5DLLStatus(result,
		"SASSerialWrapper: Setting UART timeouts sasReadTimeout = %u, writemultiplier = %u, writeconstant = %u",
		sasReadTimeout, SASWRITEMULTIPLIER, SASWRITECONSTANT);
}

void CSASSerialWrapper::SetPollingSpeed(short pollingSpeed)
{
	m_pollingSpeed = pollingSpeed;
}

short CSASSerialWrapper::GetPollingSpeed() const
{
	return (short)m_pollingSpeed;
}

// Estimate when the last byte will be transmitted.
void CSASSerialWrapper::SetLastTransmittedByteTickCount(BYTE gameAddress, UINT bufferLen)
{
	m_lastGameAddress = gameAddress;
	m_lastTransmitStartTickCount = CUtilities::GetTickCount();
	m_lastTransmitDurationTickCount = CalcSasMessagePerfCountDuration(bufferLen);
}

// Estimate when the last byte was received.
void CSASSerialWrapper::SetLastReceivedByteTickCount(bool)
{
	// Don't subtract the read timeout (from a perf count) if less than the requested number of bytes was read,
	// as the accuracy of the driver timeout is unknown.
	m_lastReceivedByteTickCount = CUtilities::GetTickCount();
}

// Delay until the polling period has elapsed since the last byte transmitted or received.
// Note that the time of last byte transmitted could still be in the future.
void CSASSerialWrapper::PerformPollingCycleDelay(BYTE gameAddress)
{
	// Use the full polling delay for a transmission to the same address
	// and half the delay for a transmission to a different address.
	// Global broadcasts count as transmissions to the EGMs address.
	bool sameAddress(
		gameAddress == m_lastGameAddress ||
		gameAddress == GLOBAL_BROADCAST_ADDRESS ||
		m_lastGameAddress == GLOBAL_BROADCAST_ADDRESS);
	DWORD pollingDelay(sameAddress ? m_pollingSpeed : (m_pollingSpeed + 1) / 2);

	// Even though it isn't supposed to, PerfCount clock can wrap.
	DWORD currentTickCount = CUtilities::GetTickCount();
	//double currentPerfCount = m_operatingSystem.GetPerfCount();
	if (currentTickCount < m_lastTransmitStartTickCount && currentTickCount < m_lastReceivedByteTickCount)
	{
		// clock reset, reset counters
		LogString(ERROR_LOG, "System Performance Counter wrapped--last known value %f", max(m_lastTransmitStartTickCount + m_lastTransmitDurationTickCount, m_lastReceivedByteTickCount));
		m_lastTransmitStartTickCount = currentTickCount;
		m_lastReceivedByteTickCount = currentTickCount;
	}
	
	// Calculate the most recent of the last bye transmitted or received.
	DWORD lastTransmitReceivedTickCount(max(m_lastTransmitStartTickCount + m_lastTransmitDurationTickCount, m_lastReceivedByteTickCount));

	// Calculate when to wait until in order to provide the expected polling delay since the last received byte.
	DWORD nextTransmitTickCount(lastTransmitReceivedTickCount + pollingDelay);
	DWORD sleepPeriod = nextTransmitTickCount - CUtilities::GetTickCount();
	if (sleepPeriod > 0 && sleepPeriod <= pollingDelay)
	{
		CUtilities::Sleep(sleepPeriod);
	}
}

S7_Result CSASSerialWrapper::SendBuffer(USHORT *wBuffer, UINT bufferLen)
{
	S7_Result result(S7DLL_STATUS_OK);

	if (wBuffer != nullptr && bufferLen > 0)
	{
		// Perform the delay before clearing the receive buffer, so the receive buffer is less likely to contain unwanted data.
		byte gameAddress((byte)(*wBuffer & GAME_ADDRESS_MASK));
		PerformPollingCycleDelay(gameAddress);

		S7_Result result = m_operatingSystem.SLITE_UART_ClearBuffers(SASUART, CLR_RX_BUFFER);
		if (S7DLL_STATUS_OK != result)
		{
			LogS5DLLStatus(result, "S5_UART_ClearBuffers");
		}

		// write data
		result = m_operatingSystem.SLITE_UART_SendBuffer(SASUART, &wBuffer[0], bufferLen);

		// Estimate when the last byte will be transmitted.  The duration for S5_UART_ClearBuffers() and S5_UART_SendBuffer() calls is uncertain.
		// Set the estimate here, so the estimate should be conservative and provide at least the minimum expected delay between transmissions.
		SetLastTransmittedByteTickCount(gameAddress, bufferLen);
	}

	return result;
}

void CSASSerialWrapper::WriteSAS(const byte *buffer, DWORD bufferLen, bool mark)
{
	if (buffer != NULL && bufferLen > 0)
	{
		unique_ptr<USHORT[]> wBuffer(new USHORT[bufferLen]);

		// Copy first byte of source buffer into target USHORT buffer with parity as specified.
		USHORT parityBit(mark ? SER9BIT_MARK : SER9BIT_NOMARK);
		wBuffer[0] = (USHORT)(buffer[0] | parityBit);

		// Copy remaining source byte buffer into target USHORT buffer (with mark parity turned off).
		for (DWORD index = 1; index < bufferLen; index++)
		{
			wBuffer[index] = (USHORT)(buffer[index] | SER9BIT_NOMARK);
		}

		// write data
		S7_Result result = SendBuffer(&wBuffer[0], bufferLen);

		if (S7DLL_STATUS_OK != result)
			LogS5DLLStatus(result, "SASSerialWrapper: Writing data");
		else
		{
			// Log written data
			LogData(SASDATA, DATA_WRITE | DATA_COM1, buffer, bufferLen);
		}
	}
}

// Read bufferLen characters from Serial Port into buffer
void CSASSerialWrapper::GetBuffer(USHORT *rBuffer, UINT bufferLen, UINT &lengthRead)
{
	lengthRead = bufferLen;
	S7_Result result(m_operatingSystem.SLITE_UART_GetBuffer(SASUART, rBuffer, &lengthRead));
	if (S7DLL_STATUS_ERROR == result)
	{
		LogS5DLLStatus(result, "S5_UART_GetBuffer");
	}

	// Estimate when the last byte was received.
	if (lengthRead > 0)
	{
		SetLastReceivedByteTickCount(lengthRead == bufferLen);
	}
}

// Read bufferLen characters from Serial Port into buffer
void CSASSerialWrapper::ReadSAS(byte *buffer, DWORD bufferLen, DWORD &numberBytesRead)
{
	numberBytesRead = 0; // Default/safety. If anything goes wrong, the caller gets no data.

	if (buffer != NULL && bufferLen > 0)
	{
		unique_ptr<USHORT[]> rBuffer(new USHORT[bufferLen]);
		DWORD startTicks = CUtilities::GetTickCount();
		UINT totalRead = 0;

		// read data
		while (totalRead < bufferLen && (CUtilities::GetTickCount() - startTicks) < MAXREADTIME)
		{
			UINT lengthRead = bufferLen - totalRead;
			GetBuffer(&rBuffer[totalRead], lengthRead, lengthRead);
			totalRead = totalRead + lengthRead;
		}

		if (0 < totalRead)
		{
			// Copy the received USHORT data into the caller's byte-sized buffer.
			// The parity bit on received data is ignored.
			for (DWORD index = 0; index < totalRead; index++)
				buffer[index] = (byte)rBuffer[index];

			numberBytesRead = totalRead;
			LogData(SASDATA, DATA_READ | DATA_COM1, buffer, (WORD)totalRead);
		}
	}
}

double CSASSerialWrapper::CalcSasMessagePerfCountDuration(UINT messageLength)
{
	// For RS-232, baud rate and bits/second are the same.
	// For SAS, the transmit and receive baud rates are the same.
	return (TICK_COUNTS_PER_SECOND * (double)messageLength * SAS_BITS_PER_WORD) / SASBAUDRATE;
}

