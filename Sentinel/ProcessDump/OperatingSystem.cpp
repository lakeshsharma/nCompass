#include "stdafx.h"
#include "OperatingSystem.h"

// Include Sentinel headers.
#include "Logging/LogString.h"
#include "Utilities.h"

// Uncomment out the following to verify the NVRAM writes.
// This is temporary until NVRAM issue can be determined.
// WARNING: Uncommenting the following line can affect performance
//#define VERIFY_NVRAM_WRITES
#ifdef VERIFY_NVRAM_WRITES
#include "Hardware/WatchDog.h"
#endif

using namespace std;

COperatingSystem COperatingSystem::m_operatingSystem;
static const unsigned usec_per_sec = 1000000;
static const unsigned usec_per_msec = 1000;
const int MAXSASSLEEP = 200;

COperatingSystem::COperatingSystem()
{
}

COperatingSystem::~COperatingSystem()
{
}

std::shared_ptr<std::iostream> COperatingSystem::CreateFile(const std::string &filename, std::ios_base::openmode mode)
{
	return shared_ptr<iostream>(new fstream(filename, mode));
}

std::ios::pos_type COperatingSystem::GetLength(const std::shared_ptr<std::istream> &file)
{
	ios::pos_type filePosition(file->tellg());
	file->seekg(0, ios::end);
	ios::pos_type fileLength(file->tellg());
	file->seekg(filePosition, ios::beg);

	if (ios::pos_type(-1) == fileLength)
	{
		fileLength = 0;
	}
	return fileLength;
}

std::ios::pos_type COperatingSystem::GetLength(const std::shared_ptr<std::iostream> &file)
{
	return GetLength(static_cast<const std::shared_ptr<std::istream>>(file));
}

std::ios::pos_type COperatingSystem::GetLength(const std::shared_ptr<std::ostream> &file)
{
	ios::pos_type filePosition(file->tellp());
	file->seekp(0, ios::end);
	ios::pos_type fileLength(file->tellp());
	file->seekp(filePosition, ios::beg);

	if (ios::pos_type(-1) == fileLength)
	{
		fileLength = 0;
	}
	return fileLength;
}

bool COperatingSystem::DeleteFile(LPCTSTR filename, DWORD *pErrorCode, bool bWaitForDisk)
{
	bool success(CUtilities::DeleteFile(filename, pErrorCode, bWaitForDisk));
	return success;
}

time_t COperatingSystem::GetCurrentTime()
{
	return CUtilities::GetCurrentTime();
}

DWORD COperatingSystem::GetTickCount32()
{
	return CUtilities::GetTickCount();
}

bool COperatingSystem::WaitForSingleObject(binsem &hHandle, DWORD dwMilliseconds)
{
	return hHandle.try_wait_for(dwMilliseconds);
}

void COperatingSystem::SetRandSeed(unsigned int seed)
{
	srand(seed);
}

int COperatingSystem::GetRand()
{
	return rand();
}

int COperatingSystem::GetRandRange(int rangeMin, int rangeMax)
{
	//double index = (double)GetRand() / (RAND_MAX + 1) * (rangeMax - rangeMin) + rangeMin;
	// TODO the above generates an overflow error. Not sure what it was trying to do. May need to refactor
	double index = (double)GetRand() / (RAND_MAX) * (rangeMax - rangeMin) + rangeMin;
	return (int)index;
}


S7_Result COperatingSystem::SLITE_UART_ClearBuffers(UINT uart, USHORT mask)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_ClearBuffers(uart, mask);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_SendBuffer(
	UINT uart,
	USHORT *buffer,
	UINT length
	)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_SendBuffer(uart, buffer, length);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_GetBuffer(
	UINT uart,
	USHORT *buffer,
	UINT* length)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_GetBuffer(uart, buffer, length);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_SetBaudRate(UINT uart, UINT baud)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_SetBaudRate(uart, baud);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_SetMode(UINT uart, UINT wordlength, UINT parity, UINT stopbits, UINT rtshandshake)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_SetMode(uart, wordlength, parity, stopbits, rtshandshake);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_SetTimeouts(UINT uart, UINT readinterval, UINT writemultiplier, UINT writeconstant)
{
#ifndef _UNIT_TEST
	return S7LITE_UART_SetTimeouts(uart, readinterval, writemultiplier, writeconstant);
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_UART_Init(UINT uart)
{
    UNREFERENCED_PARAMETER(&uart); 
	// Axiomtek no longer defines this in the library.
	return S7DLL_STATUS_OK;
}

S7_Result COperatingSystem::SLITE_SRAM_Write(S7LITE_SRAMACCESS access, void (*fn)(PVOID context), PVOID context)
{
#ifndef _UNIT_TEST
	S7_Result retVal = S7LITE_SRAM_Write(access, fn, context);

#ifdef VERIFY_NVRAM_WRITES
    // The following will read the NVRAM blocks after they are written 
    //   and verify thast the read blocks match the written blocks.
	if (retVal == S7DLL_STATUS_OK)
	{
        int badWrites = 0;
        int blocksInWrite = 0;

		for (int index = 0; index < 4; index++)
		{
			if (access.block[index].buffer != NULL && access.block[index].length > 0)
			{
                blocksInWrite++;
				byte *shadowBuffer = new byte[1024];
				S7LITE_SRAMACCESSBLOCK block; 
				block.offset = access.block[index].offset;
				block.length = access.block[index].length;
				block.buffer = shadowBuffer;
				if (S7LITE_SRAM_Read(block, NULL, NULL) == S7DLL_STATUS_OK)
				{
					if (memcmp(access.block[index].buffer, block.buffer, access.block[index].length*2) != 0)
					{
                        badWrites++;
                        std::string errStr = FormatString("!!!!!!!!!!!!!!!!!!!!!!!! ERROR write/reading NVRAM check at offset %d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", block.offset);
						cout << errStr;
                        cout << "Before: ";
                        for (int bi = 0; bi < access.block[index].length * 2; bi++) 
                        {
                            BYTE bytee = access.block[index].buffer[bi];
                            string byteStr = FormatString("%02.2x", bytee);
                            cout << byteStr;
                            cout << " ";
                        }
                        cout << "\nAfter: ";
                        for (int bi = 0; bi < block.length * 2; bi++) 
                        {
                            BYTE bytee = block.buffer[bi];
                            string byteStr = FormatString("%02.2x", bytee);
                            cout << byteStr;
                            cout << " ";
                        }
                        cout << "\n";
                        //string byteStr = FormatString("Block offsets: %d, %d, %d, %d\n", access.block[0].offset, access.block[1].offset, access.block[2].offset, access.block[3].offset);
                    }
                    else
                    {
                        //std::string errStr = FormatString("write/reading NVRAM OK at offset %d after %d bad writes\n", block.offset, badWrites);
                        //cout << errStr;
                    }
				}
				else
				{
					cout << "!!!!!!!!!!!!!!!!!!!!!!!! ERROR reading from NVRAM for write check !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
				}
				
				delete[] shadowBuffer;
			}
		}

        if (badWrites > 0) 
        {
            string errStr2 = FormatString("%d out of %d blocks were bad in write\n", badWrites, blocksInWrite);
            cout << errStr2;
            ReportSingleCallstack(errStr2);
        }
    }
	else
	{
		cout << "!!!!!!!!!!!!!!!!!!!!!!!! ERROR writing to NVRAM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	}
#endif	

	return retVal;
	
#else
	return S7DLL_STATUS_OK;
#endif
}

S7_Result COperatingSystem::SLITE_SRAM_Read(S7LITE_SRAMACCESSBLOCK access, void (*fn)(PVOID context), PVOID context)
{
#ifndef _UNIT_TEST
	return S7LITE_SRAM_Read(access, fn, context);
#else
	return S7DLL_STATUS_OK;
#endif
}

