#pragma once
#include "Buffer.h"

class CBufferHandler
{
	static const int NUMBER_OF_BUFFERS = 2;

public:
	CBufferHandler();
	CBufferHandler(const std::string& filename);
	~CBufferHandler(void);

	BOOL Flush(BOOL FlushToNVRAM);

	//This only gets on shutdown
	void GetFromNVRAM();

	void InitializeNVRAM(byte * inBuf, DWORD len);

	/// <summary>
	/// Writes the specified input data bytes to an internal buffer, optionally flushing the buffer.
	/// </summary>
	/// <param name="inputBuffer">IN - The input data bytes.</param>
	/// <param name="len">IN - The length of the data in bytes.</param>
	/// <returns>TRUE</returns>
	/// <remarks>
	/// Nothing is done if len <= 0.
	/// </remarks>
	BOOL Write(const void *inputBuffer, int len);

	void SetFileHandle(const std::string& fileName);
	int GetBufferFileSize();
	bool FileExists();
    
private:

	std::shared_ptr<std::ofstream> m_outFile;
	int  m_currentBuffer;
	int  m_fileSize;
	BOOL m_roll;
	std::string m_filename;
    CBuffer m_buffer[NUMBER_OF_BUFFERS];

	std::recursive_mutex m_criticalSection;
};
