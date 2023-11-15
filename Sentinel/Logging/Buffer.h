#pragma once

#include "Hardware/MemoryStorage.h"

#define BUFFER_SIZE 4096

class CBuffer: public CMemoryStorage
{
public:
	CBuffer(void);
public:
	~CBuffer(void);

public:
	int CurrentUsed();
	BOOL FlushToFile(std::shared_ptr<std::ofstream> file);
	void FlushToNVRAM();
	void BuildYourself();
	void InitializeNVRAM(byte * inBuf, DWORD len);

	/// <summary>
	/// Writes the specified input data to an internal buffer.
	/// </summary>
	/// <param name="inBuf">IN - The input data bytes.</param>
	/// <param name="len">IN - The length of the input data in bytes.</param>
	/// <param name="written">OUT - The number of bytes written.</param>
	/// <remarks>
	/// Only the data that fits in the remaining internal buffer space will be written.
	/// </remarks>
	void Write(const void *inBuf, DWORD len, DWORD *written);

	void Clear();

private:
	byte m_buffer[BUFFER_SIZE];
	int m_used;

	std::recursive_mutex m_critsec;
};
