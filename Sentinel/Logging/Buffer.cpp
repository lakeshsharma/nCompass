#include "stdafx.h"
#include "Buffer.h"
#include "../Hardware/NonVolatileMemoryMaps.h"
#include "../Hardware/NVRAMinator.h"

using namespace std;

CBuffer::CBuffer(void)
{
	memset (m_buffer, 0, BUFFER_SIZE);
	m_used = 0;

	m_type = MEMORY_NVRAM;
	m_dwAllowedSize = NVRAM_MAX_DEBUG_SIZE;
	m_dwOffset = NVRAM_DEBUG_DATA_SIZE_OFFSET;
	m_bOffsetIsSet = true;
}

CBuffer::~CBuffer(void)
{
    #ifdef LEAK_CHECK
    ReleaseShadowBuffer();
    #endif
}

int CBuffer::CurrentUsed()
{
	return m_used;
}

void CBuffer::Clear()
{
	m_critsec.lock();
	{
		memset (m_buffer, 0, BUFFER_SIZE);
		m_used = 0;
	}
	m_critsec.unlock();
}

BOOL CBuffer::FlushToFile(std::shared_ptr<std::ofstream> file)
{
	BOOL retVal = TRUE;

	m_critsec.lock();
	{
		if (file.get() != NULL && m_used > 0)
		{
            file->write(reinterpret_cast<LPCSTR>(m_buffer), m_used);
            file->flush();
            retVal = !file->fail();
		}
	}
	m_critsec.unlock();

	return (retVal);
}

void CBuffer::FlushToNVRAM()
{
	m_critsec.lock();
	{
		int written = 0;
		int offset;	

		//if our buffer is bigger than that allocated in NVRAM, move 
		//forward in ours so we get the latest stuff.
		if (m_used < (int)NVRAM_MAX_DEBUG_SIZE)
			offset = 0;
		else
			offset = m_used - NVRAM_MAX_DEBUG_SIZE;	
		
		int total = m_used - offset;

		//set the data size.
		CMemoryStorage::Write(NVRAM_DEBUG_DATA_SIZE_OFFSET, (BYTE *)&total, sizeof( int ));

		while (written < total)
		{
			CMemoryStorage::Write(NVRAM_DEBUG_DATA_OFFSET + written, m_buffer + offset,(m_used - offset) > 255 ? 255 : (m_used - offset));
			written+= 255; //if we're over the used, we exit anyway
			offset+= 255;
		}

		//Set the checkbyte as true.
		CMemoryStorage::Write(NVRAM_DEBUG_CHECKBYTE_OFFSET, (BYTE *)&NVRAM_DEBUG_CHECKBYTE_VALUE, NVRAM_DEBUG_CHECKBYTE_SIZE);
	}
	m_critsec.unlock();
}

void CBuffer::BuildYourself()
{
	BYTE  hasdata;

	m_critsec.lock();
	{
		Read(NVRAM_DEBUG_CHECKBYTE_OFFSET, (BYTE *)&hasdata, sizeof(hasdata));

		if (hasdata == NVRAM_DEBUG_CHECKBYTE_VALUE)
		{
			Read(NVRAM_DEBUG_DATA_SIZE_OFFSET, (BYTE *)&m_used, sizeof( m_used ));
			if (m_used < 0)
			{
				m_used = 0;
			}
			Read(NVRAM_DEBUG_DATA_OFFSET, m_buffer, m_used < BUFFER_SIZE ? m_used : BUFFER_SIZE);
		}
	}
	m_critsec.unlock();
}

void CBuffer::InitializeNVRAM(byte *inBuf, DWORD len)
{
	m_critsec.lock();
	{
		int totalLen = len;
		if ( totalLen > (int)NVRAM_MAX_DEBUG_SIZE )
		{
			totalLen = NVRAM_MAX_DEBUG_SIZE;
		}

		//set the data size.
		CMemoryStorage::Write(NVRAM_DEBUG_DATA_SIZE_OFFSET, (BYTE *)&totalLen, sizeof( int ));

		// Write the initialization string to NVRAM
		int written = 0;
		while (written < totalLen)
		{
			CMemoryStorage::Write(NVRAM_DEBUG_DATA_OFFSET + written, inBuf + written,(totalLen - written) > (NVRAM_MAX_WRITE - 1) ? (NVRAM_MAX_WRITE - 1) : (totalLen - written));
			written+= (NVRAM_MAX_WRITE - 1); //if we're over the used, we exit anyway
		}

		//Set the checkbyte as true.
		CMemoryStorage::Write(NVRAM_DEBUG_CHECKBYTE_OFFSET, (BYTE *)&NVRAM_DEBUG_CHECKBYTE_VALUE, NVRAM_DEBUG_CHECKBYTE_SIZE);
	}
	m_critsec.unlock();
}


//Writes as much data as possible to the buffer
void CBuffer::Write(const void *inBuf, DWORD len, DWORD *written)
{
	m_critsec.lock();
	{
		DWORD toWrite = 0;
		DWORD remaining = BUFFER_SIZE - m_used;

		if (0 <= m_used && m_used < BUFFER_SIZE)
		{
			if (remaining < len)
				toWrite = remaining;
			else
				toWrite = len;

			if (toWrite > 0)
			{
				memcpy_s(m_buffer + m_used, remaining, inBuf, toWrite);
			}
		}

		if (written != NULL)
		{
			*written = toWrite;
		}
		m_used += toWrite;
	}
	m_critsec.unlock();
}

