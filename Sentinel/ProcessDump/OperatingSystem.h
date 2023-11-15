#pragma once
#include "binsem.h"
#include <stdafx.h>

/// <summary>
/// Interface to operating system functionality.
/// Support running and/or testing with difference environments.
/// </summary>
/// <remarks>
/// Cannot add a GetTickCount() function to this class because of conflicts with existing GetCurrentTime() functions and macros.
/// </remarks>
class IOperatingSystem
//TODO	: public ISentinel5Api
{
public:
	/// <summary>
	/// Creates a file instance
	/// </summary>
	/// <returns>A pointer to a file instance.</returns>
	virtual std::shared_ptr<std::iostream> CreateFile(const std::string &filename, std::ios_base::openmode mode) = 0;

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::istream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::iostream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::ostream> &file);

	/// <summary>
	/// Deletes the file.
	/// </summary>
	/// <param name="filename">IN - The filename.</param>
	/// <param name="pErrorCode">IN/OUT - a pointer to an error code.</param>
	/// <param name="bWaitForDisk">IN - true to wait for the disk to spin up, false to not wait.</param>
	/// <returns>true if succesful, false otherwise.</returns>
	virtual bool DeleteFile(LPCTSTR filename, DWORD *pErrorCode = NULL, bool bWaitForDisk = false) = 0;

	/// <summary>
	/// Gets the current time as CTime.
	/// </summary>
	/// <returns>The current time as CTime.</returns>
	virtual time_t GetCurrentTime() = 0;

//	/// <summary>
//	/// Gets the current UTC time as CTime.
//	/// </summary>
//	/// <returns>The current UTC time as CTime.</returns>
//	virtual CTime GetUTCTime() = 0;

	/// <summary>
	/// Retrieves the number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.
	/// </summary>
	/// <returns>The number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.</returns>
	/// <remarks>
	/// Cannot add a GetTickCount() function to this class because of conflicts with existing GetCurrentTime() functions and macros.
	/// </remarks>
	virtual DWORD GetTickCount32() = 0;

	/// <summary>
	/// Waits until the specified object is in the signaled state or the time-out interval elapses.
	/// </summary>
	/// <param name="hHandle">IN - A handle to the object.</param>
	/// <param name="dwMilliseconds">The time-out interval, in milliseconds.
	/// The function returns if the interval elapses, even if the object's state is nonsignaled.</param>
	/// <returns>true indicates that hHandle was signaled, false indicates that the interval elapsed.</returns>
	virtual bool WaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds) = 0;

	/// <summary>
	/// Sets the seed for GetRand() for the current thread.
	/// </summary>
	/// <remarks>
	/// Uses std::srand().
	/// The srand function sets the starting point for generating a series of pseudorandom integers in the current thread.
	/// To reinitialize the generator, use 1 as the seed argument. Any other value for seed sets the generator to a random starting point.
	/// rand retrieves the pseudorandom numbers that are generated.
	/// Calling rand before any call to srand generates the same sequence as calling srand with seed passed as 1.
	/// </remarks>
	virtual void SetRandSeed(unsigned int seed) = 0;

	/// <summary>
	/// Generates a pseudorandom integer in the range 0 to RAND_MAX (32767).
	/// </summary>
	/// <returns>A pseudorandom integer in the range 0 to RAND_MAX (32767).</returns>
	/// <remarks>
	/// Uses std::rand().
	/// Use the SetRandSeed() function to seed the pseudorandom-number generator before calling GetRand().
	/// </remarks>
	virtual int GetRand() = 0;

	/// <summary>
	/// Gets a random value in the inclusive range using a single call to GetRand().
	/// </summary>
	/// <param name="rangeMin">The range minimum.</param>
	/// <param name="rangeMax">The range maximum.</param>
	/// <returns>A random value in the inclusive range</returns>
	virtual int GetRandRange(int rangeMin, int rangeMax) = 0;
	
// #pragma region S5_UART
	/// <summary>
	/// The SLITE_UART_ClearBuffers function clears the transmit 
	/// and/or receive buffers for specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="mask">[in] bit-mask that specifies which buffer(s) to clear.  Valid values are CLR_RX_BUFFER, CLR_TX_BUFFER</ param>
	/// <returns>Fails if the Sentinel 5 UART Driver cannot be accessed.</returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_ClearBuffers(UINT uart, USHORT mask) = 0;

	/// <summary>
	/// The SLITE_UART_SendBuffer function transmits a block of 
	/// words on the specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer of words to transmit.</param>
	/// <param name="length">[in] number of words to transmit.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_SendBuffer(UINT uart, USHORT *buffer, UINT length) = 0;

	/// <summary>
	/// The SLITE_UART_GetBuffer function receives a block of words
	/// from the specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer to store received words.</param>
	/// <param name="length">[inout] number of words to receive, and the number of words actually received.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_GetBuffer(UINT uart, USHORT *buffer, UINT* length) = 0;
	virtual S7_Result SLITE_UART_SetBaudRate(UINT uart, UINT baud) = 0;
	virtual S7_Result SLITE_UART_SetMode(UINT uart, UINT wordlength, UINT parity, UINT stopbits, UINT rtshandshake) = 0;
	virtual S7_Result SLITE_UART_SetTimeouts(UINT uart, UINT readinterval, UINT writemultipiler, UINT writeconstant) = 0;
	virtual S7_Result SLITE_UART_Init(UINT uart) = 0;
	virtual S7_Result SLITE_SRAM_Write(S7LITE_SRAMACCESS access, void (*fn)(PVOID context), PVOID context) = 0;
	virtual S7_Result SLITE_SRAM_Read(S7LITE_SRAMACCESSBLOCK access, void (*fn)(PVOID context), PVOID context) = 0;
	
// #pragma endregion S5_UART


};

/// <summary>
/// Default operating system/environment.
/// </summary>
class COperatingSystem : public IOperatingSystem
{
public:
	/// <summary>
	/// Default constructor.  Initializes a new instance of the <see cref="COperatingSystem"/> class.
	/// </summary>
	COperatingSystem();

	/// <summary>
	/// Destructor.  Finalizes an instance of the <see cref="COperatingSystem"/> class.
	/// </summary>
	virtual ~COperatingSystem();

	/// <summary>
	/// Creates a file instance
	/// </summary>
	/// <returns>A pointer to a file instance.</returns>
	virtual std::shared_ptr<std::iostream> CreateFile(const std::string &filename, std::ios_base::openmode mode);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::istream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::iostream> &file);

	/// <summary>
	/// Gets the length of the file.
	/// </summary>
	/// <param name="file">IN - The file.</param>
	/// <returns>The length of the file.</returns>
	/// <remarks>
	/// Can return 0 if the file failbit is set.
	/// </remarks>
	static std::ios::pos_type GetLength(const std::shared_ptr<std::ostream> &file);

	/// <summary>
	/// Deletes the file.
	/// </summary>
	/// <param name="filename">IN - The filename.</param>
	/// <param name="pErrorCode">IN/OUT - a pointer to an error code.</param>
	/// <param name="bWaitForDisk">IN - true to wait for the disk to spin up, false to not wait.</param>
	/// <returns>true if successful, false otherwise.</returns>
	virtual bool DeleteFile(LPCTSTR filename, DWORD *pErrorCode = NULL, bool bWaitForDisk = false);

	/// <summary>
	/// Gets the current local time as CTime.
	/// </summary>
	/// <returns>The current local time as CTime.</returns>
	virtual time_t GetCurrentTime();

//	/// <summary>
//	/// Gets the current UTC time as CTime.
//	/// </summary>
//	/// <returns>The current UTC time as CTime.</returns>
//	virtual CTime GetUTCTime();

	/// <summary>
	/// Calls WINAPI GetTickCount().
	/// Retrieves the number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.
	/// </summary>
	/// <returns>The number of milliseconds that have elapsed since the system was started, wrapping after 49.7 days.</returns>
	/// <remarks>
	/// Cannot add a GetTickCount() function to this class because of conflicts with existing GetCurrentTime() functions and macros.
	/// </remarks>
	virtual DWORD GetTickCount32();

	/// <summary>
	/// Waits until the specified object is in the signaled state or the time-out interval elapses.
	/// </summary>
	/// <param name="hHandle">IN - A handle to the object.</param>
	/// <param name="dwMilliseconds">The time-out interval, in milliseconds.
	/// The function returns if the interval elapses, even if the object's state is nonsignaled.</param>
	/// <returns>true indicates that hHandle was signaled, false indicates that the interval elapsed.</returns>
	virtual bool WaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds);

	/// <summary>
	/// Sets the seed for GetRand() for the current thread.
	/// </summary>
	/// <remarks>
	/// Uses std::srand().
	/// The srand function sets the starting point for generating a series of pseudorandom integers in the current thread.
	/// To reinitialize the generator, use 1 as the seed argument. Any other value for seed sets the generator to a random starting point.
	/// rand retrieves the pseudorandom numbers that are generated.
	/// Calling rand before any call to srand generates the same sequence as calling srand with seed passed as 1.
	/// </remarks>
	virtual void SetRandSeed(unsigned int seed);

	/// <summary>
	/// Generates a pseudorandom integer in the range 0 to RAND_MAX (32767).
	/// </summary>
	/// <returns>A pseudorandom integer in the range 0 to RAND_MAX (32767).</returns>
	/// <remarks>
	/// Uses std::rand().
	/// Use the SetRandSeed() function to seed the pseudorandom-number generator before calling GetRand().
	/// </remarks>
	virtual int GetRand();

	/// <summary>
	/// Gets a random value in the inclusive range using a single call to GetRand().
	/// </summary>
	/// <param name="rangeMin">The range minimum.</param>
	/// <param name="rangeMax">The range maximum.</param>
	/// <returns>A random value in the inclusive range</returns>
	/// <remarks>
	/// Uses a single call to GetRand().
	/// Therefore, large ranges may have missing values or poor distribution.
	/// </remarks>
	virtual int GetRandRange(int rangeMin, int rangeMax);

// #pragma region S5_UART
	/// <summary>
	/// The SLITE_UART_ClearBuffers function clears the transmit 
	/// and/or receive buffers for specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="mask">[in] bit-mask that specifies which buffer(s) to clear.  Valid values are CLR_RX_BUFFER, CLR_TX_BUFFER</ param>
	/// <returns>Fails if the Sentinel 5 UART Driver cannot be accessed.</returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_ClearBuffers(UINT uart, USHORT mask);

	/// <summary>
	/// The SLITE_UART_SendBuffer function transmits a block of 
	/// words on the specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer of words to transmit.</param>
	/// <param name="length">[in] number of words to transmit.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_SendBuffer(UINT uart, USHORT *buffer, UINT length);

	/// <summary>
	/// The SLITE_UART_GetBuffer function receives a block of words
	/// from the specified carrier board UART. 
	/// </summary>
	/// <param name="uart">[in] UART number (0-6).</param>
	/// <param name="buffer">[in] a pointer to a buffer to store received words.</param>
	/// <param name="length">[inout] number of words to receive, and the number of words actually received.</param>
	/// <returns></returns>
	/// <remarks>
	/// Documentation was copied from the Braemac Sentinel 5 Carrier Rev 2 Software DD document.
	/// </remarks>
	virtual S7_Result SLITE_UART_GetBuffer(UINT uart, USHORT *buffer, UINT *length);
	virtual S7_Result SLITE_UART_SetBaudRate(UINT uart, UINT baud);
	virtual S7_Result SLITE_UART_SetMode(UINT uart, UINT wordlength, UINT parity, UINT stopbits, UINT rtshandshake);
	virtual S7_Result SLITE_UART_SetTimeouts(UINT uart, UINT readinterval, UINT writemultipiler, UINT writeconstant);
	virtual S7_Result SLITE_UART_Init(UINT uart);
	virtual S7_Result SLITE_SRAM_Write(S7LITE_SRAMACCESS access, void (*fn)(PVOID context), PVOID context);
	virtual S7_Result SLITE_SRAM_Read(S7LITE_SRAMACCESSBLOCK access, void (*fn)(PVOID context), PVOID context);

// #pragma endregion S5_UART

	// Make a default operating system environment readily available.
	static COperatingSystem m_operatingSystem;

private:
	// Disable default copy constructor, and default assignment operator.
	COperatingSystem(const COperatingSystem& rhs);
	COperatingSystem& operator=(const COperatingSystem& rhs);


	std::recursive_mutex  m_writeCriticalSection;

//TODO	LONGLONG m_perfFreq;
};
