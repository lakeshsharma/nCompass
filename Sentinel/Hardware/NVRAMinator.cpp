#include "stdafx.h"

#include "NVRAMinator.h"
#include "IOCTLOffsets.h"
#include "Utilities.h"
#include "ProcessDump/OperatingSystem.h"
#include "NonVolatileMemoryMaps.h"
#include "WatchDog.h"
#include "DirectoryManager.h"
#include "Diagnostics.h"

#ifdef LOGGING
#include "Logging/LogString.h"
#include "Logging/Logger.h"
#endif

using namespace std;

std::recursive_mutex CNVRAMinator::m_cs;

CNVRAMinator::CNVRAMinator(void)
{
	m_shadowBuffer = NULL;
    m_verifyBlocks = false;
}

CNVRAMinator::~CNVRAMinator(void)
{
    ReleaseShadowBuffer();
}

void CNVRAMinator::ReleaseShadowBuffer()
{
    if ( m_shadowBuffer != nullptr )
    {
        delete [] m_shadowBuffer;
        m_shadowBuffer = nullptr;
    }
}

void CNVRAMinator::SetToVerifyBlocksUponWrite()
{
    m_verifyBlocks = true;
}

bool CNVRAMinator::SetData(DWORD offset1, BYTE * data1, UINT len1,
			 DWORD offset2, BYTE * data2, UINT len2,
			 DWORD offset3, BYTE * data3, UINT len3,
			 DWORD offset4, BYTE * data4, UINT len4)
{
    bool nvramWritten = false;

	// The Hardware guarantees a power-fail-safe write of up to 256 bytes
	// of course more can be written.

	int nDataBlocks = SRAM_ACCESS_BOLCKS;
	bool bOutOfRange = false;
	bool success = true;

	// How much is being written and is it asking to write out of range?
	//---------------------------------------------------
	if ((int)offset4 == NVRAM_NULL_VALUE)
	{
		nDataBlocks = 3;
	}
	else if (offset4 + len4 > NVRAM_OFFSET_END)
	{
		LogString(ERROR_LOG, "Block4 Out of range. offset: %d. length: %d", offset4, len4);
		bOutOfRange = true;
	}

	if ((int)offset3 == NVRAM_NULL_VALUE)
	{
		nDataBlocks = 2;
	}
	else if (offset3 + len3 > NVRAM_OFFSET_END)
	{
		LogString(ERROR_LOG, "Block3 Out of range. offset: %d. length: %d", offset3, len3);
		bOutOfRange = true;
	}

	if ((int)offset2 == NVRAM_NULL_VALUE)
	{
		nDataBlocks = 1;
	}
	else if (offset2 + len2 > NVRAM_OFFSET_END)
	{
		LogString(ERROR_LOG, "Block2 Out of range. offset: %d. length: %d", offset2, len2);
		bOutOfRange = true;
	}

	if ((int)offset1 == NVRAM_NULL_VALUE)
	{
		nDataBlocks = 0;
	}
	else if ((DWORD)(offset1 + len1) > NVRAM_OFFSET_END)
	{
		LogString(ERROR_LOG, "Block1 Out of range. offset: %d. length: %d", offset1, len1);
		bOutOfRange = true;
	}
	//---------------------------------------------------

	// if everything's ok, then write to NVRAM
	if (nDataBlocks > 0 && !bOutOfRange)
	{
		// Lock the critical section
		lock_guard<recursive_mutex> lock(m_cs);

        if ( m_shadowBuffer == NULL )
        {
            InitializeShadowBuffer();
        }

		// pairs of start and end offsets for each block.
		std::vector<DWORD> wordOffsetArray;

		// Write first block to the NVRAM shadow memory buffer.
		memcpy(m_shadowBuffer + offset1, data1, len1 );

		// Add first block to our offset array.
		AddToOffsetArray( wordOffsetArray, offset1, len1 );

		if ( nDataBlocks > 1 )
		{
			// Write first block to the NVRAM shadow memory buffer.
			memcpy(m_shadowBuffer + offset2, data2, len2 );

			// Add second block to our offset array.
			AddToOffsetArray( wordOffsetArray, offset2, len2 );
		}

		if ( nDataBlocks > 2 )
		{
			// Write third block to the NVRAM shadow memory buffer.
			memcpy(m_shadowBuffer + offset3, data3, len3 );

			// Add third block to our offset array.
			AddToOffsetArray( wordOffsetArray, offset3, len3 );
		}

		if ( nDataBlocks > 3 )
		{
			// Write 4th block to the NVRAM shadow memory buffer.
			memcpy(m_shadowBuffer + offset4, data4, len4 );

			// Add 4th block to our offset array.
			AddToOffsetArray( wordOffsetArray, offset4, len4 );
		}

		// Set up the ram access blocks
		S7LITE_SRAMACCESS NVRWrite;
		ClearRamAccessBlocks( NVRWrite ); 
		int combinedWordCount = 0;			// The count of words currently in the ram access blocks
		int blockIndex = 0;					// The current clock of 4 blocks

		// Go through all the start/end pairs of offsets in the word offset array.
		// There should only be a maximum of 256 bytes to be written and should be able to be written in one write to NVRAM. 
		//  But given potential for word boundary overlaps it may be more than 128 words, 
		//  and may require 2 NVRam writes. This is a side effect of the NVRAM API changing from byte-based to word-based.
		for ( size_t woIndex = 0; woIndex < wordOffsetArray.size(); woIndex+=2 )
		{
			// Get the start and end word offsets for this block.
			DWORD startOffset = wordOffsetArray[woIndex];
			DWORD endOffset = wordOffsetArray[woIndex+1];

			// Loop until all words are added to the access blocks or written to NVRAM
			while ( startOffset <= endOffset )
			{
				// Get the number of of words left in this block to be written.
				int wordLen = (endOffset - startOffset) + 1;

				// If the block cannot fit in the access blocks, reduce the size written to the block.
				if ( combinedWordCount + wordLen > NVRAM_MAX_WORD_WRITE )
				{
					wordLen = NVRAM_MAX_WORD_WRITE - combinedWordCount;
				}

				// Set up the current block in the access blocks
				NVRWrite.block[blockIndex].offset = startOffset;
				NVRWrite.block[blockIndex].length = wordLen;
				NVRWrite.block[blockIndex].buffer = (m_shadowBuffer + (startOffset * 2));

				// Increase the word count for the access blocks
				combinedWordCount += wordLen;

				// Move to the next block in the sccess blocks
				blockIndex++;

				// Increment the start offset of the block we are trying to write.
				startOffset += wordLen;

				// If the access blocks are filled up, we need to write.
				if ( combinedWordCount >= NVRAM_MAX_WORD_WRITE || blockIndex > NVRAM_MAX_WRITE_BLOCKS)
				{
                    nvramWritten = true;
					success = WriteToNVRam( NVRWrite );
					ClearRamAccessBlocks( NVRWrite );
					combinedWordCount = 0;
					blockIndex = 0;
				}
			}
		}

		// Wrte the non-full access block.
		if ( combinedWordCount > 0 )
		{
            nvramWritten = true;
			success = WriteToNVRam( NVRWrite );
		}
	}

	if ( bOutOfRange )
	{
#ifdef LOGGING
		LogString(ERROR_LOG, "Error - attempting out of range write to NVRAM" );
#endif
		success = false;
	}
	else if ( nDataBlocks < 0 || !nvramWritten)
	{
        cout << "Error - no data blocks to write to NVRAM\n";
#ifdef LOGGING
		LogString(ERROR_LOG, "Error - no data blocks to write to NVRAM" );
#endif
		success = false;
	}

	return ( success );
}

bool CNVRAMinator::GetData(DWORD offset, BYTE * data, UINT len)
{	
    lock_guard<recursive_mutex> lock(m_cs);

	if ( m_shadowBuffer == NULL )
	{
		InitializeShadowBuffer();
	}

	bool bOutOfRange = false;

	if ((offset + len) > NVRAM_OFFSET_END)
	{
		bOutOfRange = true;
	}

	if (!bOutOfRange)
	{
		// Just copy the values from the shadow buffer.
		memcpy( data, m_shadowBuffer + offset, len );
	}

	return ( !bOutOfRange );
}

void CNVRAMinator::InitializeShadowBuffer()
{
	COperatingSystem os;
	m_shadowBuffer = new byte[NVRAM_OFFSET_END];

	S7LITE_SRAMACCESSBLOCK NVRRead;

	// Lock the critical section
	lock_guard<recursive_mutex> lock(m_cs);

	// Read all of NVRAM into the shdow buffer
	NVRRead.buffer = m_shadowBuffer;
	NVRRead.offset = 0;
	NVRRead.length = NVRAM_OFFSET_END/2;	// The specified length is in WORDS
	S7_Result result = os.SLITE_SRAM_Read( NVRRead, NULL, NULL );

#ifdef LOGGING
	if (CSmartDataLogger::IsActive())
	{
		if (result != S7DLL_STATUS_OK)
		{
			LogString(ERROR_LOG, "Error reading from NVRAM into shadow buffer: Code: %d", CUtilities::GetLastError());
		}
		else
		{
			LogString(NVRAM_LOG, "shadow buffer initialized");
		}
	}
#endif
}

// Helper function to clear ram access blocks.
void CNVRAMinator::ClearRamAccessBlocks( S7LITE_SRAMACCESS &ramAccess )
{
	memset(&ramAccess, 0, sizeof(S7LITE_SRAMACCESS));
}

// Helper function to write blocks to NVRam.
bool CNVRAMinator::WriteToNVRam( S7LITE_SRAMACCESS &NVRWrite )
{
	COperatingSystem os;
	S7_Result result = S7DLL_STATUS_ERROR;

    // This function call is for diagnostics puposes only
    if (m_verifyBlocks) 
    {
        VerifyBlocks(NVRWrite);
    }

	result = os.SLITE_SRAM_Write( NVRWrite, NULL, NULL );

	if ( result != S7DLL_STATUS_OK )
	{

#ifdef LOGGING
		LogString(ERROR_LOG, "Error writing to NVRAM: Code: %d", CUtilities::GetLastError());
#endif
	}

#ifdef LOGGING
	//string logString;
	//string logString2;
	//ostringstream threadId;
	//threadId << CUtilities::GetCurrentThreadId();
	//	logString = FormatString("%s [%s] writing", result == S7DLL_STATUS_OK ? "SUCCESS" : "ERROR  ", static_cast<LPCTSTR>(threadId.str().c_str()));
	//	for (int si = 0; si < NVRAM_MAX_WRITE_BLOCKS; si++ )
	//	{
	//		if ( NVRWrite.block[si].buffer == NULL )
	//		{
	//			break;
	//		}
	//		logString2 = FormatString(": %d words to offset %x[%x]", NVRWrite.block[si].length, NVRWrite.block[si].offset, NVRWrite.block[si].buffer[0]);
	//		logString += logString2;
	//	}
	//	LogStringNoFormat(NVRAM_LOG, logString);

#endif


	return ( result == S7DLL_STATUS_OK );
}


void CNVRAMinator::VerifyBlocks(S7LITE_SRAMACCESS &blocks)
{
    // This is for diagnostics purposes and can be removed later.
    // Display information when the NVRAM sprinkle positions are are written

	DWORD sprinkleOffset[NUM_SPRINKLE_VALUES] = {SPRINKLE1_OFFSET, 
		SPRINKLE2_OFFSET, 
		SPRINKLE3_OFFSET,
		SPRINKLE4_OFFSET,
		SPRINKLE5_OFFSET,
		SPRINKLE6_OFFSET,
		SPRINKLE7_OFFSET,
		SPRINKLE8_OFFSET,
		SPRINKLE9_OFFSET,
		SPRINKLE10_OFFSET,
		SPRINKLE11_OFFSET,
		SPRINKLE12_OFFSET,
		SPRINKLE13_OFFSET,
		SPRINKLE14_OFFSET,
		SPRINKLE15_OFFSET,
		SPRINKLE16_OFFSET };

    bool badSprinkleWrite = false;

    // Go thru all the blocks 
    for (int blockIndex = 0; blockIndex < SRAM_ACCESS_BOLCKS; blockIndex++ ) 
    {
        // If this block is specified
        if (blocks.block[blockIndex].buffer != NULL && blocks.block[blockIndex].length > 0) 
        {
            // Go thru all the sprinkle values
            for (int sprinkleIndex = 0; sprinkleIndex < NUM_SPRINKLE_VALUES; sprinkleIndex++)
            {
                // Translate the word offset start/end to byte offsets
                DWORD offsetStart = blocks.block[blockIndex].offset * 2;
                DWORD offsetEnd = (blocks.block[blockIndex].offset + blocks.block[blockIndex].length) * 2;

                // Determine if this sprinkle offset is within this block
                if ( sprinkleOffset[sprinkleIndex] >= offsetStart && sprinkleOffset[sprinkleIndex] < offsetEnd)
                {
                    // Determine the byte offset off the  
                    DWORD sprinkleBufferOffset = sprinkleOffset[sprinkleIndex] - offsetStart;

                    // Determine the byte pointer in buffer for the sprinkle value
                    BYTE *sprinkleInBuffer = (BYTE *)blocks.block[blockIndex].buffer + sprinkleBufferOffset;

                    // Now see a valid sprinkle value to be written is at teh sprinkle position
                    if (*sprinkleInBuffer != SPRINKLE_VALUE) 
                    {
                        // We wrote a bad spribkle value
                        badSprinkleWrite = true;
                    }
                }
            }
        }
    }

    if (badSprinkleWrite) 
    {
        cout << "!!!!!!!!!!!!!!!!!!!!!!Invalid write to NVRAM Sprinkle position!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        string exceptionType = "Invalid write to NVRAM Sprinkle position";

        ReportSingleCallstack(exceptionType);
    }
}

// Helper function to add the start/end offsets for this block to the array of offset pairs.
void CNVRAMinator::AddToOffsetArray( std::vector<DWORD> &wordOffsetArray, DWORD byteOffset, UINT byteLen )
{
	// Convert the byte-based offset to a word-based offset.
	DWORD startWordOffset = byteOffset / 2;

	// Determine the offset of the last word to be written to NVRAM
	DWORD endWordOffset = (byteOffset + byteLen - 1) / 2;

	// Go through all the existing word-based offsets
	for ( size_t woIndex = 0; woIndex < wordOffsetArray.size(); woIndex+=2 )
	{
		// If the start of the new offset overlaps an existing start/end, move the start offset past the end. 
		if ( startWordOffset >= wordOffsetArray[woIndex] && startWordOffset <= wordOffsetArray[woIndex+1])
		{
			startWordOffset = wordOffsetArray[woIndex+1] + 1;
		}

		// If the end of the new offset overlaps an existing start/end, move the offset before the start. 
		else if ( endWordOffset >= wordOffsetArray[woIndex] && endWordOffset <= wordOffsetArray[woIndex+1] )
		{
			endWordOffset = wordOffsetArray[woIndex] - 1;
		}
	}

	// If the offsets are still valid, add the start/end pair to the array of offsets.
	if ( startWordOffset <= endWordOffset )
	{
		wordOffsetArray.push_back(startWordOffset);
		wordOffsetArray.push_back(endWordOffset);
	}
}
