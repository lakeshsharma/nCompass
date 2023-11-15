
#include "NVRAMTesterParts.h"
#include "NVRAMTester.h"
#include <sys/times.h>

using namespace std;


// Program file defines
#define MEMORY_TEST_BYTE	10
#define TEST_BYTE			128
#define FIRST_BLOCK_SIZE	8
#define TIMER_ID			1
#define MIN_TIME			3000       	//3 Sec
#define MAX_TIME			60000		//60 seconds.

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define TOTALSFILE "/opt/ncompass/mediadisk/nvram_totals.txt"

// Program file globals and constants
const int WAIT_FOR_DRIVE_TIMEOUT = 30000;	// 30000 milliseconds = 30 seconds.
const int WAITFORDRIVEDELAY = 50000; // 50000 milliseconds = 50 seconds.

bool stop_test = FALSE;

typedef struct {
	int blockLen;
	char repeatVal;
}BLOCK_PARAM;

const char alphanum[] =
"0123456789"
"!@#$%^&*()+=-_"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int iStrlen = sizeof(alphanum) - 1;

// Member variables
S7_Result results;
std::string m_productName;
std::string m_productVersion;
static std::fstream m_RAMfile;
static bool m_attachmentError;
static int m_timerCount;
static int m_timerTicks; // Timer interval period in milliseconds.
void * m_nTimer;
static volatile bool m_poweringDown;
static volatile bool m_resettingMemory;
static std::recursive_mutex m_criticalSection;
static TimerThread *pt;
static int LoopCount;
static bool m_phidgetHardware;
static bool m_manualInput;
static timer_t timerid;

// Class method code
S7_Result NVRAMTesterParts::getResults() 
{ 
	return results; 
};

NVRAMTesterParts::NVRAMTesterParts()
{
	pt = new TimerThread();
	LoopCount = 0;
}

NVRAMTesterParts::~NVRAMTesterParts()
{
	delete pt;
}

void NVRAMTesterParts::GetTestNumber()
{
	char x_buffer[1024];
	char *x_ptr = x_buffer;
	int  currentLoop = 0;
	FILE *totalsFile;

	try
	{
		totalsFile = fopen(TOTALSFILE, "r");
		if (totalsFile != NULL) 
		{
			x_ptr = fgets(x_buffer, sizeof(x_buffer), totalsFile);
			if (x_ptr != NULL) 
				currentLoop = atoi(x_ptr);
			else 
				currentLoop = 0;
		}
		else
		{
			currentLoop = 0;
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR GetTestNumber: unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
	LoopCount = currentLoop;
}

void NVRAMTesterParts::SetTestNumber()
{
	char x_buffer[1024];
	char *x_ptr = x_buffer;
	int  currentLoop = 0;
	FILE *totalsFile;

	try
	{
		totalsFile = fopen(TOTALSFILE, "w");
		fprintf(totalsFile, "%d\n", LoopCount);
		fflush(totalsFile);
		fclose(totalsFile);
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR SetTestNumber: unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
}


S7_Result NVRAMTesterParts::initialize()
{
	char eMessage[57];
	void (*func)(void);
	func = &NVRAMTesterParts::SystemRestart;
	S7_Result result = -1;
	// verify if there is phidget hardware connected.
	m_RAMfile.open("/opt/ncompass/mediadisk/nvram_run.txt", fstream::out | fstream::app);
	GetTestNumber();
			
	DetectPhidgetHardware();
	if ((result = S7LITE_DLL_Init()) != 0)
	{
		prtf((char *)"%s\n", (char *)"\r", "ERROR: Unexpected exception encountered - S7LITE_DLL_Init returned [%d]\n", result);
	}
	snprintf(eMessage, sizeof(eMessage), "Start!! - %d                  ", ++LoopCount);
	prtf((char *)"%s\n",  (char *)"\r", (char *)eMessage);
	SetTestNumber();

	if (m_phidgetHardware) 
	{
		pt->setThread(func);
	}
	return result;
}


bool NVRAMTesterParts::RunTest(bool manual)
{
	char eMessage[57];
	char answer[5];
	bool result = false;
	m_manualInput = manual;

	result = S7_Test_NVRAM();
	if (!result) 
	{
		prtf((char *)"%s\n", (char *)"\r", "ERROR: Unexpected exception encountered - S7_Test_NVRAM returned [%d]\n", result);
	}
	return result;
}

S7_Result NVRAMTesterParts::finalize()
{
	char eMessage[57];
	S7_Result result = -1;
	if ((result = S7LITE_DLL_DeInit()) != 0)
	{
		prtf((char *)"%s\n", (char *)"\r", "ERROR: Unexpected exception encountered - S7LITE_DLL_DeInit returned [%d]\n", result);
	}
	m_RAMfile.close();

	return result;
}

bool NVRAMTesterParts::S7_Test_NVRAM()
{
	S7_Result result = S7DLL_STATUS_OK;
	stop_test = FALSE;

	m_resettingMemory = false;
	m_poweringDown = false;
	m_attachmentError = false;
	m_timerCount = 0;
	m_timerTicks = 0;

	bool memoryBlockTestPass(S7_Test_NVRAM_Reading());

	S7_Test_NVRAM_Clear(memoryBlockTestPass);
	memoryBlockTestPass = S7_Test_NVRAM_Resetting(memoryBlockTestPass);

	if (m_phidgetHardware) 
	{
		update_srand();
		m_timerTicks = rand() % MAX_TIME + MIN_TIME;
		prtf((char *)"%s%d\n", (char *)"\r", (char *)"Timer setting value = ", m_timerTicks);
		pt->SetTimer(m_timerTicks);
		//++m_timerCount;
	}
	memoryBlockTestPass = S7_Test_NVRAM_Writing(memoryBlockTestPass);

	if (m_phidgetHardware) 
	{
		while (memoryBlockTestPass)
		{
			memoryBlockTestPass = S7_Test_NVRAM_Resetting(memoryBlockTestPass);
			memoryBlockTestPass = S7_Test_NVRAM_Writing(memoryBlockTestPass);
		}
	}

	// If no error then display that the test is done.
	if (memoryBlockTestPass)
	{
		prtf((char *)"%s\n", (char *)"\r", (char *)"Finished and Passed!!                    ");
	} else {
		prtf((char *)"%s\n", (char *)"\r", (char *)"Finished and Failed!!                    ");
	}

	S7_TestCanceled();
	return true;
}


void NVRAMTesterParts::S7_TestCanceled()
{
	if (m_phidgetHardware) 
	{
		pt->StopTimer();
	}
	stop_test = TRUE;
}

void NVRAMTesterParts::DetectPhidgetHardware()
{
	int result;

	prtf((char *)"NOTE: Attempting to detect Phidget Hardware!!\n",  (char *)"\r");

	//Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle ifKit = 0;

	//create the InterfaceKit object
	CPhidgetInterfaceKit_create(&ifKit);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle)ifKit, -1);

	//get the program to wait for an interface kit device to be attached
	result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 20000);
	m_attachmentError = EPHIDGET_OK != result;
	if (m_attachmentError)
	{
		m_phidgetHardware = false;
		prtf((char *)"NOTE: Test running without Phidget Hardware!!\n",  (char *)"\r");
	} else {
		CPhidget_close((CPhidgetHandle)ifKit);
		CPhidget_delete((CPhidgetHandle)ifKit);
		m_phidgetHardware = true;
		prtf((char *)"NOTE: Test running with Phidget Hardware!!\n",  (char *)"\r");
	}
}


/// <summary>
/// Perform the reading portion of the NVRAM test.
/// </summary>
/// <returns>false if an error has occurred, true otherwise.</returns>
bool NVRAMTesterParts::S7_Test_NVRAM_Reading()
{
	// 2016-02-03 Reading takes about 5+ seconds on a Braemac blue box.
	prtf((char *)"%s\n",  (char *)"\r", (char *)"Reading!!                    ");

	//check for the memory block written
	S7_Result result = S7DLL_STATUS_OK;
	bool memoryBlockTestPass = TRUE;
	S7LITE_SRAMACCESSBLOCK NVRAMRead;

	byte *testBuffer = new byte[1024 + 1];
	std::unique_ptr<byte> testBufferPtr(testBuffer);
	memset(testBuffer, 0, 1024 + 1);

	byte *tempBuffer = new byte[NVRAM_WAL_OFFSET_START];
	std::unique_ptr<byte> tempBufferPtr(tempBuffer);
	PUCHAR  bufferPointer;
	bufferPointer = tempBuffer;
	NVRAMRead.buffer = (PUCHAR)testBuffer;
	NVRAMRead.offset = 0;

	memset(tempBuffer, 0, NVRAM_WAL_OFFSET_START);
	memset(testBuffer, 0, 1024);

	if ((1024) % 2 == 0)
		NVRAMRead.length = (1024) / 2;
	else
		NVRAMRead.length = ((1024) / 2) + 1;

	//read in 1kb chuncks
	for (int i = 0; i<NVRAM_WAL_OFFSET_START/1024; i++)
	{
		result = S7LITE_SRAM_Read(NVRAMRead, NULL, NULL);
		memcpy(tempBuffer, testBuffer, 1024);
		tempBuffer += 1024;
		NVRAMRead.offset += 1024 / sizeof(WORD);
		memset(testBuffer, 0, 1024);
	}

	if (result != S7DLL_STATUS_OK)
	{
		m_RAMfile << "ERROR: read error!!" << endl;
		prtf((char *)"%s\n", (char *)"\r", "ERROR: read error!!\n");
		return false;
	}

	//reset pointer location
	tempBuffer = bufferPointer;
	prtf((char *)"%s\n",  (char *)"\r", (char *)"Reading!!                    ");

	if (!memcmp(tempBuffer, "MemoryTest", MEMORY_TEST_BYTE) && tempBuffer[MEMORY_TEST_BYTE] == 0)
	{
		prtf((char *)"%s\n", (char *)"\r", (char *)"Checking!!                   ");
		//perform memory blocks test

		//increment the buffer to point to the first block
		tempBuffer = tempBuffer + MEMORY_TEST_BYTE;
		int nBlocks = 0;
		bool breakCheck = FALSE;
		int endOffset = NVRAM_WAL_OFFSET_START / 2;
		int startOffset = MEMORY_TEST_BYTE;
		byte *firstBlock = new byte[FIRST_BLOCK_SIZE];
		std::unique_ptr<byte> firstBlockPtr(firstBlock);

		memcpy(firstBlock, tempBuffer, FIRST_BLOCK_SIZE);

		while (firstBlock[0] == 0 && !breakCheck && !stop_test && ((endOffset - startOffset) > (NVRAM_MAX_WRITE * NVRAM_MAX_WRITE_BLOCKS)))
		{
			BLOCK_PARAM blockParam[3] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
			nBlocks = firstBlock[1];

			if (nBlocks == 1)
			{
				//check only the first block
				int val = 1;
				for (int i = 1; i<FIRST_BLOCK_SIZE; i++)
				{
					if (memcmp(&val, &tempBuffer[i], sizeof(byte)))
					{
						m_RAMfile << "ERROR: memory block error!!" << endl;
						prtf((char *)"%s\n", (char *)"\r", "ERROR: memory block error!!\n");
						breakCheck = TRUE;
						memoryBlockTestPass = FALSE;
						break;
					}
				}
				tempBuffer += FIRST_BLOCK_SIZE;
			}
			else if (nBlocks>1 && nBlocks <= NVRAM_MAX_WRITE_BLOCKS)
			{
				tempBuffer += FIRST_BLOCK_SIZE;
				for (int iBlock = 0; iBlock<nBlocks - 1 && !breakCheck; iBlock++)
				{
					blockParam[iBlock].blockLen = firstBlock[2 * (iBlock + 1)];
					blockParam[iBlock].repeatVal = firstBlock[2 * (iBlock + 2) - 1];
					//check the value of the memory block written
					for (int i = 0; i< blockParam[iBlock].blockLen; i++)
					{
						if (memcmp(&blockParam[iBlock].repeatVal, &tempBuffer[i], sizeof(byte)))
						{
							breakCheck = TRUE;
							memoryBlockTestPass = FALSE;
							break;
						}
					}
					tempBuffer += blockParam[iBlock].blockLen;
					startOffset += blockParam[iBlock].blockLen;
				}
			}
			else
			{
				breakCheck = TRUE;
				memoryBlockTestPass = FALSE;
				break;
			}
			memcpy(firstBlock, tempBuffer, FIRST_BLOCK_SIZE);
		}
	}
	prtf((char *)"%s\n", (char *)"\r", (char *)"Reading - Done!!             ");

	return memoryBlockTestPass;
}


/// <summary>
/// Perform the resetting portion of the NVRAM test, if memoryBlockTestPass is true.
/// </summary>
/// <param name="memoryBlockTestPass">false if an error has occurred, true otherwise.</param>
/// <returns>false if an error has occurred, true otherwise.</returns>
bool NVRAMTesterParts::S7_Test_NVRAM_Resetting(bool memoryBlockTestPass)
{
	try
	{
		//Prepare the memory
		if (memoryBlockTestPass)
		{
			prtf((char *)"%s\n", (char *)"\r", (char *)"Resetting!                 ");
			int startOffset = 0;
			S7LITE_SRAMACCESS access;
			//initialize the access block
			ClearRamAccessBlocks(access);

			//prtf((char *)"%s\n", (char *)"\r", (char *)"Note: S7_Test_NVRAM_Resetting: calling mutex lock.");
			if (m_criticalSection.try_lock())
			{
				if (m_poweringDown)
				{
					//prtf((char *)"%s\n", (char *)"\r", (char *)"Note: S7_Test_NVRAM_Resetting: calling mutex unlock.");
					prtf((char *)"%s\n", (char *)"\r", "Powering down!!");
					while (m_poweringDown) 
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME));
						m_criticalSection.unlock();
					}
				}
				else
				{
					m_resettingMemory = true;
				}
				m_criticalSection.unlock();
			}

			// 2016-02-03 Reseting takes about 11+ seconds on a Braemac blue box.
			while (startOffset<(NVRAM_WAL_OFFSET_START) / 2 && !stop_test)
			{
				// Set up the current block in the access blocks
				access.block[0].offset = startOffset;
				//if bytes to be written > remaining memory adjust the length
				if (((NVRAM_WAL_OFFSET_START) / 2 - startOffset) < (TEST_BYTE / 2))
					access.block[0].length = (NVRAM_WAL_OFFSET_START) / 2 - startOffset;
				else
					access.block[0].length = TEST_BYTE / sizeof(WORD);

				char *memoryBuff = new char[TEST_BYTE];
				std::unique_ptr<char> memoryBuffPtr(memoryBuff);

				memset(memoryBuff, 0, TEST_BYTE);
				access.block[0].buffer = (PUCHAR)memoryBuff;

				Prepare_Test_Buffer(access.block[0].buffer, access.block[0].length * sizeof(WORD));
				S7_Result result = S7LITE_SRAM_Write(access, NULL, NULL);
				if (result != S7DLL_STATUS_OK)
				{
					m_RAMfile << "ERROR: S7LITE_SRAM_Write error!!" << endl;
					prtf((char *)"%s\n", (char *)"\r", "ERROR: S7LITE_SRAM_Write error!!\n");
					return false;
				}

				// Increment the start offset of the block we are trying to write.
				startOffset += access.block[0].length;;
			}
			m_resettingMemory = false;
		}
	} 
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
	prtf((char *)"Resetting - Done!!\n", (char *)"\r" );
	return memoryBlockTestPass;
}


/// <summary>
/// Perform the writing portion of the NVRAM test, if memoryBlockTestPass is true.
/// </summary>
/// <param name="memoryBlockTestPass">false if an error has occurred, true otherwise.</param>
/// <returns>false if an error has occurred, true otherwise.</returns>
bool NVRAMTesterParts::S7_Test_NVRAM_Writing(bool memoryBlockTestPass)
{
	try
	{
		if (memoryBlockTestPass)
		{
			//Write to the memory till the start of WAL address
			while (!stop_test)
			{
				int startOffset = MEMORY_TEST_BYTE;
				int endOffset = (NVRAM_WAL_OFFSET_START) / 2;

				// 2016-02-03 Writing takes about 13+ seconds on a Braemac blue box.
				prtf((char *)"%s\n", (char *)"\r", (char *)"Writing!!                    ");


				while (((endOffset - startOffset) > FIRST_BLOCK_SIZE) && !stop_test)
				{
					int bufferlen = FIRST_BLOCK_SIZE, nBlocks = 0, blockLen = 0;
					//write into the NVRAM
					update_srand();
					nBlocks = rand() % 4 + 1;//WRITE_BLOCKS;
					if (nBlocks > NVRAM_MAX_WRITE_BLOCKS)
						nBlocks = NVRAM_MAX_WRITE_BLOCKS;

					byte **WriteBuffer = new byte*[nBlocks];
					std::unique_ptr<byte *> WriteBufferPtr(WriteBuffer);
					std::unique_ptr<byte> WriteBuffers[4];
					for (int maxBlock = 0; maxBlock < nBlocks; maxBlock++)
					{
						WriteBuffer[maxBlock] = new byte[NVRAM_MAX_WRITE];
						WriteBuffers[maxBlock].reset(WriteBuffer[maxBlock]);
					}

					for (int iBlock = 0; iBlock<nBlocks - 1; iBlock++)
					{
						blockLen = rand() % (NVRAM_MAX_WRITE - FIRST_BLOCK_SIZE) / (nBlocks - 1);

						if (blockLen % 2 != 0 || blockLen == 1)
							blockLen++;

						bufferlen += blockLen;

						if (bufferlen>endOffset - startOffset)
						{
							// Completed writing portion of test (without the power down timer expiring).
							// Return true to continue testing.
							prtf((char *)"Writing - Done!!- [%d]\n", (char *)"\r", memoryBlockTestPass);
							return memoryBlockTestPass;
						}
						//generate random strings
						GetRandomString((char*)WriteBuffer[iBlock], blockLen);
					}

					byte *NVRAMBuffer = new byte[bufferlen];
					std::unique_ptr<byte> NVRAMBufferPtr(NVRAMBuffer);
					byte *originalBuffer = NVRAMBuffer;
					//start-write the first block of 8 bytes

					//set the first byte NULL
					memset(NVRAMBuffer, 0, sizeof(byte));
					//fill the rest of the buffer with all ONEs
					memset(NVRAMBuffer + 1, 1, FIRST_BLOCK_SIZE - 1);
					//number of blocks
					memset(NVRAMBuffer + sizeof(byte), nBlocks, sizeof(byte));

					NVRAMBuffer += 2 * sizeof(byte);
					for (int iBlock = 0; iBlock<nBlocks - 1; iBlock++)
					{
						//length of second block
						memset(NVRAMBuffer, (int)strlen((char*)WriteBuffer[iBlock]), sizeof(byte));
						memset(NVRAMBuffer + sizeof(byte), *(WriteBuffer[iBlock]), sizeof(byte));
						NVRAMBuffer += 2 * sizeof(byte);
					}
					//increment the buffer to point to the second block to be written
					NVRAMBuffer = originalBuffer + FIRST_BLOCK_SIZE;
					//End-write the first block of 8 bytes

					//fill the rest of the data
					for (int iBlock = 0; iBlock<nBlocks - 1; iBlock++)
					{
						memset(NVRAMBuffer, *(WriteBuffer[iBlock]), (int)strlen((char*)WriteBuffer[iBlock]));
						NVRAMBuffer += (int)strlen((char*)WriteBuffer[iBlock]);
					}
					//Now write the data to NVRAM
					S7LITE_SRAMACCESS WriteBlock;
					//initialize the access block
					//ClearRamAccessBlocks(WriteBlock);
					for (int blockIndex = 0; blockIndex < NVRAM_MAX_WRITE_BLOCKS; blockIndex++)
					{
						WriteBlock.block[blockIndex].buffer = NULL;
						WriteBlock.block[blockIndex].length = 0;
						WriteBlock.block[blockIndex].offset = 0;
					}

					int blockendOffset = startOffset + bufferlen;
					bool endWrite = FALSE;
					while (startOffset<blockendOffset && !endWrite)
					{
						// Get the number of of bytes in this block to be written.
						//
						if ((endOffset - startOffset) > FIRST_BLOCK_SIZE)
							blockLen = blockendOffset - startOffset;
						else
						{
							blockLen = endOffset - startOffset;
							endWrite = TRUE;
						}
						// If the block cannot fit in the access blocks, reduce the size written to the block.
						if (blockLen > NVRAM_MAX_WRITE)
						{
							blockLen = NVRAM_MAX_WRITE;
						}
						// Set up the current block in the access blocks
						WriteBlock.block[0].offset = startOffset / 2;
						if (blockLen % 2 == 0)
							WriteBlock.block[0].length = blockLen / 2;
						else
							WriteBlock.block[0].length = (blockLen / 2) + 1;

						WriteBlock.block[0].buffer = (PUCHAR)originalBuffer;
						if (m_RAMfile.is_open())
						{
							m_RAMfile << "In SRAM Write function at address " << startOffset << std::endl;
							//prtf((char *)"%s%d\n", (char *)"\r", "In SRAM Write function at address ", startOffset);
						}
						//write using WAL functionality,use S5_SRAM_Write in final version
						S7_Result result = S7LITE_SRAM_Write(WriteBlock, NULL, NULL);

						if (m_RAMfile.is_open()) 
						{
							m_RAMfile << "SRAM Write function complete" << std::endl;
							//prtf((char *)"%s\n", (char *)"\r", "SRAM Write function complete");
						}
						if (result != S7DLL_STATUS_OK)
						{
							m_RAMfile << "ERROR: Write error WAL!!" << endl;
							prtf((char *)"%s\n", (char *)"\r", "ERROR: Write error WAL!!\n");
							return false;
						}

						// Increment the start offset of the block we are trying to write.
						startOffset += blockLen;
						// Increment the start offset of the shadow buffer 
						originalBuffer += blockLen;
					}
				}
				break;
			}
		}
	}
	catch (exception &ex) 
	{
		char x_buffer[1024];
		snprintf(x_buffer, sizeof(x_buffer), "ERROR: unexpected exception: %s\n", ex.what());
		prtf((char *)"%s\n", (char *)"\r", x_buffer);
	}
	prtf((char *)"Writing Done!!- [%d]\n", (char *)"\r", memoryBlockTestPass);
	return memoryBlockTestPass;
}


/// <summary>
/// Perform the clearing portion of the NVRAM test, if memoryBlockTestPass is false.
/// </summary>
/// <param name="memoryBlockTestPass">false if an error has occurred, true otherwise.</param>
void NVRAMTesterParts::S7_Test_NVRAM_Clear(bool memoryBlockTestPass)
{
	char buffer[256];
	char *cptr=buffer;
	prtf((char *)"%s\n", (char *)"\r", (char *)"Clearing!!                   ");

	if (!memoryBlockTestPass)
	{
		m_RAMfile << "Partially written blocks found!\n" << endl;
		//display error
		strcpy(buffer,"y");
		prtf((char *)"%s\n", (char *)"\r", "ERROR: Partially written blocks found!\n");
		if (m_manualInput) 
		{
			printf((char *)"NVRAM needs to be cleared.\nDo you want to clear the NVRAM? (Y/n)");
			cptr = fgets(buffer, sizeof(buffer), stdin);

			if ((strlen(buffer) != 0) && (strcmp(buffer, "quit") == 0)) 
			{
				exit (0);
			}
		}
		
		if ((strlen(buffer) == 0) || (buffer[0] == 'Y') || (buffer[0] == 'y'))
		{
			byte *buffer = new byte[MEMORY_TEST_BYTE];
			std::unique_ptr<byte> bufferPtr(buffer);
			memset(buffer, 0, MEMORY_TEST_BYTE);
			S7LITE_SRAMACCESS WriteBlock;
			//initialize the access block
			ClearRamAccessBlocks(WriteBlock);

			WriteBlock.block[0].offset = 0;
			WriteBlock.block[0].length = MEMORY_TEST_BYTE / 2;
			WriteBlock.block[0].buffer = (PUCHAR)buffer;

			S7_Result result = S7LITE_SRAM_Write(WriteBlock, NULL, NULL);
			if (result != S7DLL_STATUS_OK)
			{
				m_RAMfile << "ERROR: Write error Clear NVRAM!!" << endl;
				prtf((char *)"%s\n", (char *)"\r", "ERROR: Write error Clear NVRAM!!\n");
				return;
			}
		}
	}
	prtf((char *)"%s\n", (char *)"\r", (char *)"Clearing - Done!!            ");

}

UINT NVRAMTesterParts::S7_Test_NVRAM(LPVOID p)
{
	NVRAMTesterParts *testNVRAM = (NVRAMTesterParts *)p;
	testNVRAM->S7_Test_NVRAM();
	return 0;
}


void NVRAMTesterParts::GetRandomString(char* buffer, int buffersiize)
{
	int index = 0;
	int i = 0;
	update_srand();
	index = rand() % iStrlen;

	for (; i<buffersiize; i++)
	{
		buffer[i] = alphanum[index];
	}
	buffer[i] = '\0';
}


void NVRAMTesterParts::Prepare_Test_Buffer(PUCHAR Buffer, int Bufferlen)
{
	int length = MEMORY_TEST_BYTE;
	for (; Bufferlen>0; )
	{
		if (Bufferlen<MEMORY_TEST_BYTE)
		{
			length = Bufferlen;
		}

		memset(Buffer, 0, Bufferlen);
		strncpy((char*)Buffer, "MemoryTest", Bufferlen);

		Buffer += length;
		Bufferlen -= length;
	}
}

void NVRAMTesterParts::update_srand()
{
	struct tms timeTms;
	unsigned int part1;
	unsigned int part2;
	unsigned int seed;
	times(&timeTms);
	part1 = (unsigned int)timeTms.tms_stime;
	part2 = (unsigned int)clock();
	seed = (unsigned int)((part1 >> 16 && 0xffff) + (part1 && 0xffff)
		+ (part2 >> 15 && 0xffff) + (part2 && 0xffff)
		+ (unsigned int)clock() + (unsigned int)time(NULL));
	srand(seed);

	// Consume some values to perform additional mixing.
	rand();
	rand();
	rand();
}

/// <summary>
/// Sets a CString from a buffer that might contain trailing NUL characters.
/// </summary>
/// <param name="xstring">OUT - The CString.</param>
/// <param name="str">IN - The string buffer.</param>
/// <param name="length">IN - The length of the string buffer.</param>
void NVRAMTesterParts::SetCString(std::string &xstring, LPCTSTR str, UINT length)
{
	
	if (NULL == str)
	{
		xstring.clear();
	}
	else
	{
		while (length > 0 && 0 == ((LPCTSTR)str)[length - 1])
		{
			--length;
		}
	}
	string temp(str, length);
	xstring =temp;
}


void NVRAMTesterParts::ClearRamAccessBlocks(S7LITE_SRAMACCESS &ramAccess)
{
	for (int blockIndex = 0; blockIndex < NVRAM_MAX_WRITE_BLOCKS; blockIndex++)
	{
		ramAccess.block[blockIndex].buffer = NULL;
		ramAccess.block[blockIndex].length = 0;
		ramAccess.block[blockIndex].offset = 0;
	}
}

int CCONV NVRAMTesterParts::AttachHandler(CPhidgetHandle IFK, void *userptr)
{
	UNREFERENCED_PARAMETER(userptr);

	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	return 0;
}

int CCONV NVRAMTesterParts::DetachHandler(CPhidgetHandle IFK, void *userptr)
{
	UNREFERENCED_PARAMETER(userptr);

	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);
	return 0;
}

int CCONV NVRAMTesterParts::ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
	UNREFERENCED_PARAMETER(IFK);
	UNREFERENCED_PARAMETER(userptr);

	prtf((char *)"%s\n", (char *)"\r", "ERROR: Phidget Error handled. %d - %s\n", ErrorCode, unknown);
	return 0;
}

void NVRAMTesterParts::SystemRestart()
{
	int result;
	const char *err;

	if (m_phidgetHardware) 
	{
		try {
			prtf((char *)"%s\n", (char *)"\r", (char *)"Restarting                    ");

			//Declare an InterfaceKit handle
			CPhidgetInterfaceKitHandle ifKit = 0;

			//create the InterfaceKit object
			CPhidgetInterfaceKit_create(&ifKit);

			//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
			CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
			CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
			CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);

			//open the interfacekit for device connections
			CPhidget_open((CPhidgetHandle)ifKit, -1);

			//get the program to wait for an interface kit device to be attached
			result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 20000);
			m_attachmentError = EPHIDGET_OK != result;
			if (m_attachmentError)
			{
				CPhidget_getErrorDescription(result, &err);
				m_RAMfile << "ERROR: Problem waiting for attachment\nMake sure that the Phidget Hardware is connected!" << endl;
				prtf((char *)"%s\n", (char *)"\r", "ERROR: Problem waiting for attachment\nMake sure that the Phidget Hardware is connected!\n");
			}
			else
			{
				//prtf((char *)"%s\n", (char *)"\r", (char *)"Note: SystemRestart: calling mutex lock.");
				if (m_criticalSection.try_lock())
				{
					if (!m_resettingMemory)
					{
						m_poweringDown = true;
						//prtf((char *)"%s\n", (char *)"\r", (char *)"Note: SystemRestart: calling mutex unlock.");
						prtf((char *)"%s\n", (char *)"\r", (char *)"NOTE: Phidget Power Cycle Called.");
						for (int i = 0; i < 4; i++)
						{
							result = CPhidgetInterfaceKit_setOutputState(ifKit, i,  PTRUE);
							if (result != 0) 
							{
								CPhidget_getErrorDescription(result, &err);
								m_RAMfile << "ERROR: CPhidget_getErrorDescription returned an error " << err << endl;
								prtf((char *)"%s\n", (char *)"\r", "ERROR: CPhidget_getErrorDescription returned an error :%s\n", err);
							}
						}
					}
					m_criticalSection.unlock();
				}
				CPhidget_close((CPhidgetHandle)ifKit);
				CPhidget_delete((CPhidgetHandle)ifKit);
			}

			// Set the timer to fire again with a different timeout.
			update_srand();
			m_timerTicks = rand() % MAX_TIME + MIN_TIME;
			prtf((char *)"%s%d\n", (char *)"\r", (char *)"Timer setting value = ", m_timerTicks);
			pt->SetTimer(m_timerTicks);
		}
		catch (exception &ex) 
		{
			char x_buffer[1024];
			snprintf(x_buffer, sizeof(x_buffer), "ERROR: unexpected exception: %s\n", ex.what());
			prtf((char *)"%s\n", (char *)"\r", x_buffer);
		}
	}
}


