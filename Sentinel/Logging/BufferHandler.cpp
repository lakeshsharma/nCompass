#include "stdafx.h"

#include "BufferHandler.h"

using namespace std;

CBufferHandler::CBufferHandler()
	:   m_currentBuffer(0)
	,   m_fileSize(0)
{
}

CBufferHandler::CBufferHandler(const std::string& filename)
	:   m_outFile(new ofstream(filename, ofstream::binary | ofstream::trunc))
	,   m_currentBuffer(0)
	,   m_fileSize(0)
	,   m_filename(filename)
{
}

CBufferHandler::~CBufferHandler(void)
{
	if (m_outFile)
	{
		Flush(false);
		m_outFile->close();
		m_outFile.reset();
	}
}

BOOL CBufferHandler::Flush(BOOL FlushToNVRAM)
{
	BOOL retVal = FALSE;

    lock_guard<recursive_mutex> lock(m_criticalSection);
	{
        retVal = TRUE;

        //NVRAM?
		if ( FlushToNVRAM )
		{
			(m_buffer[m_currentBuffer]).FlushToNVRAM();
		}

		//FILE?
		if (retVal && m_outFile)
		{
			if ( !(m_buffer[m_currentBuffer]).FlushToFile(m_outFile) )
			{
                m_outFile->close();
                m_outFile.reset();
				retVal = FALSE;
				m_fileSize = 0;
			}
			else
			{
                m_fileSize += m_buffer[m_currentBuffer].CurrentUsed();
			}
		}

        m_buffer[m_currentBuffer].Clear();
	}

	return (retVal);
}

void CBufferHandler::GetFromNVRAM()
{
    (m_buffer[m_currentBuffer]).BuildYourself();
}

void CBufferHandler::InitializeNVRAM(byte *inBuf, DWORD len)
{
    (m_buffer[m_currentBuffer]).InitializeNVRAM(inBuf, len);
}

void CBufferHandler::SetFileHandle(const string& fileName)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);
	if (m_outFile.get() != nullptr)
	{
		Flush(FALSE);
		m_outFile->close();
		m_fileSize = 0;
		m_outFile.reset(new ofstream(fileName, ofstream::binary | ofstream::trunc));
	}
}

int  CBufferHandler::GetBufferFileSize()
{
	int retVal;
	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		retVal = m_fileSize;
	}

	return retVal;
}


BOOL CBufferHandler::Write(const void *inputBuffer, int len)
{
	DWORD written = 0;
	DWORD totalWritten = 0;
	const BYTE *inBuf(reinterpret_cast<const BYTE *>(inputBuffer));

	BOOL retVal = TRUE;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	if (len > 0)
	{
		while (totalWritten < (DWORD)len && retVal)
		{
			//write the data
			(m_buffer[m_currentBuffer]).Write(inBuf + totalWritten, len - totalWritten, &written);

			//if the buffer is full, 
			if (written == 0)
			{
				//flush it to its medium
				//TBD thread this out or there's no point in having multiple buffers!!
				retVal = Flush(false);

				//get the next buffer
				m_currentBuffer++;

				//check for wrapping
				if( m_currentBuffer == NUMBER_OF_BUFFERS )
					m_currentBuffer = 0;
			}

			totalWritten+= written;
			written = 0;
		}
	}

	return (retVal);
}

bool CBufferHandler::FileExists()
{
	struct stat fd;
	bool retVal = false;

    if (stat(m_filename.c_str(), &fd) == 0)
		retVal = true;
	
	return retVal;
}

