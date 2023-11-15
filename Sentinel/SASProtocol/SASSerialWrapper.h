#pragma once
#include "ProcessDump/OperatingSystem.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class SASSerialWrapper; }

class CSASSerialWrapper
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::SASSerialWrapper;

public:
	static const short MS200_POLLING_SPEED_MILLISECONDS = 200;
	static const short MS40_POLLING_SPEED_MILLISECONDS = 40;
	static const short DEFAULT_POLLING_SPEED_MILLISECONDS = MS40_POLLING_SPEED_MILLISECONDS;

	/// <summary>
	/// Standard/Default constructor. Initializes a new instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	/// <param name="operatingSystem">An interface to operating system functionality.</param>
	/// <remarks>
	/// The construction time is treated as the time of the last transmitted byte to game address 1.
	/// The construction time is treated as the time of the last received byte from game address 1.
	/// </remarks>
	CSASSerialWrapper(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CConfigExtFile"/> class.
	/// </summary>
	virtual ~CSASSerialWrapper(void);

	/// <summary>
	/// Opens, initializes, and enables the SAS serial port.
	/// </summary>
	virtual void OpenSAS(void);

	/// <summary>
	/// Update SAS Read Tiemout.
	/// </summary>
	void UpdateSasReadTimeout();

	/// <summary>
	/// Sets the polling speed.
	/// </summary>
	/// <param name="pollingSpeed">The polling speed in milliseconds.</param>
	/// <remarks>
	/// The polling speed is the minimum period prior to sending a command since the last transmitted byte to the same address
	/// and the minimum period prior to sending a command since the last received byte from the same address.
	/// Each call to WriteSAS is treated as if the buffer contains a single command.
	/// </remarks>
	void SetPollingSpeed(short pollingSpeed);

	/// <summary>
	/// Gets the polling speed.
	/// </summary>
	/// <returns>The polling speed in milliseconds.</returns>
	/// <remarks>
	/// See SetPollingSpeed for details.
	/// </remarks>
	short GetPollingSpeed() const;

	/// <summary>
	/// Writes the specified buffer to the SAS serial port.
	/// </summary>
	/// <param name="buffer">IN - The buffer.</param>
	/// <param name="bufferLen">IN - Length of the buffer.</param>
	/// <param name="mark">IN - If true then use mark parity for the first word, else use space parity.</param>
	/// <remarks>
	/// Enforces the followng timing:
	/// 1. Does not write to the same game address as last the last write until:
	///     a) the minimum polling period has elapsed since the last byte written AND
	///     b) the minimum polling period has elapsed since the last byte read.
	/// 2. Does not write to a different game address as the last write until:
	///     a) one-half of the minimum polling period has elapsed since the last byte written AND
	///     b) one-half of the minimum polling period has elapsed since the last byte read.
	/// 3. The lower 7-bits of the first byte of the buffer is treated as the game address.
	/// NOTE: If the host did not recieve the EGM's response correctly,
	/// ... the host does not need to wait the polling rate time period to re-issue the poll.
	/// </remarks>
	virtual void WriteSAS(const byte *buffer, DWORD bufferLen, bool mark);

	/// <summary>
	/// Tries to read bufferLen characters from the SAS serial port into the specified buffer.
	/// </summary>
	/// <param name="buffer">OUT - The buffer.</param>
	/// <param name="bufferLen">IN - Length of the buffer.</param>
	/// <param name="numberBytesRead">OUT - The number bytes read.</param>
	/// <remarks>
	/// The parity bit is stripped off before storing the received data.
	/// </remarks>
	virtual void ReadSAS(byte *buffer, DWORD bufferLen, DWORD &numberBytesRead);

private:
	static const UINT SASUART = 1;
	static const UINT SASWORDLENGTH = 9;
	static const UINT SAS_BITS_PER_WORD = SASWORDLENGTH + 2; // Includes the start bit, 8 data bits, a parity bit, and one stop bit.
	static const UINT SASBAUDRATE = 19200;
	static const UINT SASREADINTERVAL = 100;
	static const UINT SASWRITEMULTIPLIER = 2;
	static const UINT SASWRITECONSTANT = 10;
	static const USHORT SER9BIT_NOMARK = 0x0000;
	static const USHORT SER9BIT_MARK = 0xff00;

	static const DWORD MAXREADTIME = 200; // wait for 200 milliseconds

	static const DWORD GLOBAL_BROADCAST_ADDRESS = 0;
	static const DWORD DEFAULT_LAST_GAME_ADDRESS = 1;
	static const DWORD DEFAULT_LAST_TRANSMIT_DURATION_TICK_COUNT = 0;
	static const byte GAME_ADDRESS_MASK = 0x7f; // The lower 7-bits of the first byte of the buffer is treated as the game address.
	static const UINT MAXIMUM_MESSAGE_LENGTH = 260; // 5 for message header/trailer overhead, plus 255 for the maximum extra length (in an 8-bit length field).

	DWORD m_pollingSpeed;
	byte m_lastGameAddress;
	// Instead of setting the m_lastTransmittedBytePerfCount sometime in the future,
	// ... save the start time of the transmission, plus the duration of the transmission.
//	double m_lastTransmitStartPerfCount;
//	double m_lastTransmitDurationPerfCount;
//	double m_lastReceivedBytePerfCount;
	double m_lastTransmitStartTickCount;
	double m_lastTransmitDurationTickCount;
	double m_lastReceivedByteTickCount;

	IOperatingSystem &m_operatingSystem;

	void SetLastTransmittedByteTickCount(byte gameAddress, UINT bufferLen);
	void SetLastReceivedByteTickCount(bool);
	void PerformPollingCycleDelay(byte gameAddress);
	S7_Result SendBuffer(USHORT *wBuffer, UINT bufferLen); // wBuffer MUST NOT be nullptr.
	void GetBuffer(USHORT *buffer, UINT bufferLen, UINT &numberBytesRead);

	/// <summary>
	/// Calculates the duration in perf counts to transmit a SAS message of messageLength.
	/// </summary>
	/// <param name="transmitLength">IN - Length of the message.</param>
	/// <returns>The duration in tick counts to transmit a message of transmitLength.</returns>
	static double CalcSasMessagePerfCountDuration(UINT messageLength);
};

