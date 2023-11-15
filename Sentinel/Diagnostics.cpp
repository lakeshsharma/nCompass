#include "stdafx.h"
#include <cxxabi.h>
#include <dlfcn.h>
#include "Diagnostics.h"
#include "ProcessDump/WebDiagnosticsStatusInfo.h"

// Sentinel includes.
#include "DirectoryManager.h"
#include "Logging/LogString.h"
#include "resource.h"
#include "Utilities.h"

using namespace std;

CDiagnostics* CDiagnostics::m_instance = NULL;
std::recursive_mutex CDiagnostics::m_cs;
std::recursive_mutex CDiagnostics::m_csFileAccess;
volatile std::atomic_long CDiagnostics::m_fileUpdateCount(0);

const string EXCEPTION_OCCURED("\
************************************************************************************************************************************************\n\
*************************************************************EXCEPTION OCCURRED************************************************************\n\
************************************************************************************************************************************************\n");

const string END_OF_EXCEPTION("\
***********************************************************************************************************************************************\n\
**************************************************************END OF EXCEPTION**************************************************************\n\
***********************************************************************************************************************************************\n");

CDiagnostics& CDiagnostics::Instance()
{
	lock_guard<recursive_mutex> lock(m_cs);
	if (NULL == m_instance)
	{
		m_instance = new CDiagnostics();
	}
	return *m_instance;
}

void CDiagnostics::CleanupInstance()
{
	lock_guard<recursive_mutex> lock(m_cs);
	if (NULL != m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

CDiagnostics::CDiagnostics() : m_config()
{
	m_ticketPbtMaxAmount = new CTicketPBTMaxAmount(CUtilities::GetTickCount());
	m_ticketPbtMaxAmount->Start();
}

CDiagnostics::~CDiagnostics()
{
	if (m_ticketPbtMaxAmount != NULL)
	{
		delete m_ticketPbtMaxAmount;
	}
}

/// <summary>
/// Opens a log file for binary writes.
/// If the file already exceeds the maximum size then the file is truncated.
/// </summary>
/// <param name="stream">IN/OUT - The stream.</param>
/// <param name="path">IN - The path.</param>
void CDiagnostics::OpenLogFile(FILE *&stream, const string& path)
{
	// Allow any number of readers, but only one writer at a time.
    stream = fopen(path.c_str(), "ab+");

	if (stream != nullptr)
	{
        long int len = ftell(stream);
        if (len > MAX_LOG_FILE_SIZE)
		{
			fclose(stream);
            stream = fopen(path.c_str(), "wb");
		}
	}
}

void CDiagnostics::CloseLogFile(FILE *stream)
{
	fflush(stream);
	flock(fileno(stream), LOCK_UN);
	fclose(stream);
}

TiltCode CDiagnostics::GetRebootTiltCode() const
{
	return m_config.GetRebootTiltCode();
}

void CDiagnostics::PersistReboot(TiltCode code, const std::string &identifier, DiagnosticsPersistFlag flag)
{
	lock_guard<recursive_mutex> lock(m_cs);
	if (tltUnknown == GetRebootTiltCode() || tltRebootUnknown == GetRebootTiltCode())
	{
		m_config.SetRebootTiltCode(code, flag);
	}
	CWatchDog::Instance().SetShutdownInProcess();
	ReportRebootToFile(CDirectoryManager::RebootDiagnosticsFilePath(), code, identifier);
}

void CDiagnostics::ClearReboot()
{
	lock_guard<recursive_mutex> lock(m_cs);
	m_config.ClearReboot();
}

size_t CDiagnostics::ReportThreadCallstackToBuffer(const std::string& name, char* buffer, size_t bufferSize)
{
	size_t remaining = bufferSize;

	memset(buffer, 0, bufferSize);

	int count = sprintf(buffer, "%s:", name.c_str());
	if (0 < count)
	{
		char* bufPtr = buffer;
        std::string stack_trace = CWatchDog::Instance().GetExceptionStackTrace(CUtilities::GetCurrentThreadId());
		count = sprintf(bufPtr, "%s", stack_trace.c_str());
		if (0 < count)
		{
			remaining -= count;
		}
	}

	return remaining;
}

void CDiagnostics::AppendThreadCallstackToStringInner(std::string &exceptionString, bool crashed, const std::string &name, const std::string& exceptionType, const std::string& callstack)
{
	string timeStamp(CUtilities::GetTimeStamp());
	exceptionString.append(FormatString("\r\n@@%s - %s", timeStamp.c_str(), name.c_str()));

	if (!exceptionType.empty())
	{
		exceptionString.append(FormatString("\r\n%s", exceptionType.c_str()));
	}

	if (!callstack.empty())
	{
		exceptionString.append(FormatString("\r\nCallstack:\r\n%s", callstack.c_str()));
	}

	if (crashed)
	{
#ifndef _UNIT_TEST
		LogString(ERROR_LOG, "----- Exception Thrown -----");
		LogString(ERROR_LOG, "%s - %s", timeStamp.c_str(), name.c_str());
		LogString(ERROR_LOG, "ERROR: %s", exceptionType.c_str());
		LogString(ERROR_LOG, "Callstack:\n %s", callstack.c_str());
		LogString(ERROR_LOG, "----------------------------");
#endif
	}
}

int  CDiagnostics::ReportThreadCallstackToFile(const std::string &path, const std::string &thread, const std::string& exceptionType, const std::string& callstack)
{
	int bytesWritten(0);

	string exceptionString;
	AppendThreadCallstackToStringInner(exceptionString, true, thread, exceptionType, callstack);

	WriteStringToFile(path, exceptionString);

	return bytesWritten;
}

void CDiagnostics::WriteStringToFile(const std::string &path, const std::string &stringToWrite)
{
	lock_guard<recursive_mutex> lock(m_csFileAccess);

	fstream stream;
	string debugData(stringToWrite);
	do
	{
		stream.open(path, std::fstream::in | std::fstream::out | std::fstream::app);
		if (!stream.is_open())
		{
			CUtilities::Sleep(SLEEP_PER_RETRYLOOP);
			continue;
		}
		else
		{
			break;
		}
#ifndef _UNIT_TEST
	} while (true);
#else
	} while (false);
#endif

	if (stream.is_open())
	{
		stream.seekg(0, ios::end);
		long length(stream.tellg());
		long newTotalLengthAfterWrite(length + stringToWrite.length());
		if (newTotalLengthAfterWrite >= MAX_ERROR_FILE_SIZE)
		{
			stream.close();

			std::ifstream t(path);
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

			std::size_t found(str.find(EXCEPTION_OCCURED));
			if (found != std::string::npos)
			{
				str.erase(0, found + EXCEPTION_OCCURED.length());
				found = str.find(END_OF_EXCEPTION);
				if (found != std::string::npos)
				{
					str.erase(0, found + END_OF_EXCEPTION.length());
					str.append(stringToWrite);
					debugData = str;
				}
			}			

			stream.open(path, std::fstream::out);
		}
		stream << debugData;
		stream.close();
		CUtilities::FlushVolume();

		m_fileUpdateCount++;		
	}
}

void CDiagnostics::WriteExceptionToFile(const std::string &path, const std::string &name, const std::string& exceptionType, const std::string& callstack, bool threadTimeOut)
{
	string exceptionString;

	AppendExceptionHeaderToString(exceptionString);

	if (threadTimeOut)
	{
		AppendThreadsHeaderToString(exceptionString);
	}

	AppendThreadCallstackToString(exceptionString, name, exceptionType, callstack);

	CWatchDog::Instance().AppendAllThreadCallStacks(exceptionString, CUtilities::GetCurrentThreadId(), !threadTimeOut);

	if (threadTimeOut)
	{
		AppendThreadsFooterToString(exceptionString);
	}

	AppendExceptionFooterToString(exceptionString);

	std::cout << "exception string length=" << exceptionString.length() << std::endl;

	std::cout << "WriteExceptionToFile\n" << exceptionString << std::endl;

	WriteStringToFile(path, exceptionString);
}

void CDiagnostics::AppendThreadCallstackToString(std::string &exceptionString, const std::string &name, const std::string& exceptionType, const std::string& callstack)
{
	AppendThreadCallstackToStringInner(exceptionString, true, name, exceptionType, callstack);
}

void CDiagnostics::AppendExceptionHeaderToString(std::string &exceptionString)
{
	exceptionString.append("\n");
	exceptionString.append(EXCEPTION_OCCURED);
	exceptionString.append("Sentinel " + CUtilities::GetEXEVersion() + "\n");
}

void CDiagnostics::AppendExceptionFooterToString(std::string &exceptionString)
{
	exceptionString.append("\n");
	exceptionString.append(END_OF_EXCEPTION);
}

void CDiagnostics::AppendSecondaryThreadsHeaderToString(std::string &exceptionString)
{
	exceptionString.append("*********************************************ADDITIONAL THREADS*********************************************\n");
}

void CDiagnostics::AppendSecondaryThreadsFooterToString(std::string &exceptionString)
{
	exceptionString.append("*********************************************END OF ADDITIONAL THREADS*********************************************\n");
}

void CDiagnostics::AppendThreadsHeaderToString(std::string &exceptionString)
{
	exceptionString.append("*********************************************THREAD TIMEOUT - RUNNING THREADS*********************************************\n");
}

void CDiagnostics::AppendThreadsFooterToString(std::string &exceptionString)
{
	exceptionString.append("*********************************************END OF THREADS*********************************************\n");
}

int CDiagnostics::ReportRebootToFileInner(const std::string& path, TiltCode code)
{
	int bytesWritten = 0;
	try
	{
		string rebootTiltCodeStr(CWebDiagnosticsStatusInfo::TiltCodeToString(code));
		string temp(FormatString("\n@@%s - Reboot Tilt Code: %d - %s\n", 
			CUtilities::GetTimeStamp().c_str(), code, rebootTiltCodeStr.c_str()));
		WriteStringToFile(path, temp);
		bytesWritten = temp.length();
	}
    catch (...)
    {
        // No place to write this error to. We have to catch this.
    }


	return bytesWritten;
}

int CDiagnostics::ReportErrorToFileInner(const std::string& path, const std::string& message)
{
	int bytesWritten = 0;
	try
	{
		string temp(FormatString("\n@@%s - %s", CUtilities::GetTimeStamp().c_str(), message.c_str()));
		WriteStringToFile(path, temp);
		bytesWritten = temp.length();
	}
    catch (...) 
	{

	}

	return bytesWritten;
}

int CDiagnostics::ReportRebootToFile(const std::string& path, TiltCode code, const std::string &identifier)
{
	// First write code to log, so we can tell if problem happened later when writting 'error.txt'.
#ifndef _UNIT_TEST
	string logStr;
	if (tltRebootUnknown == code)
	{
		logStr = FormatString("Reboot occurred with tilt code = %d.", code);
	}
	else
	{
		logStr = FormatString("Reboot is about to occur with tilt code = %d.", code);
	}

	LogString(identifier, "%s", logStr.c_str());
	if (identifier != GENERAL)
	{
		// Log reboot info to the "GENERAL" log, if we didn't just do that.
		LogString(GENERAL, "%s", logStr.c_str());
	}
#endif

	return ReportRebootToFileInner(path, code);
}

int CDiagnostics::ReportErrorToFile(const std::string& path, const std::string &errorMessage, const std::string &identifier)
{
	// First write code to log, so we can tell if problem happened later when writing 'error.txt'.
	string logStr;
	logStr = FormatString("General Error: %s", errorMessage.c_str());

#ifndef _UNIT_TEST
	LogString(identifier, "%s", logStr.c_str());
#endif

	int bytesWritten = ReportErrorToFileInner(path, logStr);

	return bytesWritten;
}


int CDiagnostics::ReportNVRAMSprinklesToFile(const std::string& path, std::vector<BYTE> &badSprinkleValues)
{
	// First write values to the log, so we can tell if problem happened later when writting 'error.txt'.
    string fileStr = "Invalid NVRAM sprinkle values detected at start-up:\r\n";

#ifndef _UNIT_TEST
    string logStr = "Invalid sprinkle values read in NVRAM at start-up:";

	for (vector<BYTE>::iterator sprinkleIter = badSprinkleValues.begin(); sprinkleIter != badSprinkleValues.end(); ++sprinkleIter)
	{
        logStr += FormatString(" [%d]", *sprinkleIter);
        fileStr += FormatString("%d\t", *sprinkleIter);
    }

	fileStr.append("\r\n");
	LogString(ERROR_LOG, "%s", logStr.c_str());
#endif

    int bytesWritten = 0;

    try
    {
		string temp(FormatString("\r\n@@%s - %s", CUtilities::GetTimeStamp().c_str(), fileStr.c_str()));
		WriteStringToFile(path, temp);
		bytesWritten = temp.length();
    }
    catch (...) 
    {
    }

	return bytesWritten;
}

int CDiagnostics::ReportPBTTicketDiagnosticToFile(const string& path, const string& errorMessage, 
	const string& objectString, TiltCode tilt)
{
	int bytesWritten = 0;

	string temp(FormatString("\r\n@@%s - Tilt Code %d: %s\r\n%s\r\n",
		CUtilities::GetTimeStamp().c_str(),
		tilt,
		errorMessage.c_str(),
		objectString.c_str()));
	WriteStringToFile(path, temp);
	bytesWritten = temp.length();

	return bytesWritten;
}

int CDiagnostics::ReportBadPollerProtocolSendPacketToTextFile(
        const std::string& path,
		const std::string& timeStamp,
		TiltCode tiltCode,
	    const std::string& reason,
		const BYTE *message,
		WORD messageLength)
{
	if (nullptr == message)
	{
		messageLength = 0;
	}

	// Log to a debug stick, if present.
	string dataString(CUtilities::ToHexString(message, (DWORD)messageLength, MAXIMUM_BAD_MESSAGE_LENGTH));
	LogString(ERROR_LOG, _T("Bad Poller Protocol packet created/sent. tiltCode=%u reason=\"%s\" data(%u) : %s"),
		(unsigned)tiltCode,
		reason.c_str(),
		(unsigned)messageLength,
		dataString.c_str());

	// Log to the diagnostics disk, if present.
	int bytesWritten = 0;
	{
		string temp(FormatString(CUtilities::LoadResourceString(IDS_FMT_BAD_POLLER_PACKET_SEND).c_str(),
			timeStamp.c_str(),
			(unsigned)tiltCode,
			reason.c_str()));
		WriteStringToFile(path, temp);
		bytesWritten = temp.length();
	}

	return bytesWritten;
}

int CDiagnostics::ReportBadPollerProtocolSendPacketToLogFile(
    const string& path,
	const BYTE *message,
	WORD messageLength)
{
	if (nullptr == message)
	{
		messageLength = 0;
	}

	// Log to a debug stick, if present.
    LogData(BADPOLLERDATA_LOG, DATA_WRITE | DATA_ETHERNET, message, messageLength);

	// Log to the diagnostics disk, if present.
	int bytesWritten = 0;
	{
		FILE *stream(nullptr);
		OpenLogFile(
            stream,
            path);

		if (stream != nullptr)
		{
			BYTE code(DATA_WRITE | DATA_ETHERNET);
			bytesWritten += fwrite(&code, sizeof(BYTE), sizeof(BYTE), stream); // Write out the data code.
			bytesWritten += fwrite(&messageLength, sizeof(BYTE), sizeof(messageLength), stream); // Write out the data length.
			bytesWritten += fwrite(message, sizeof(BYTE), messageLength, stream); // And write out the data.
			CloseLogFile(stream);
		}
	}

	return bytesWritten;
}

int CDiagnostics::ReportBadPollerProtocolSendPacket(TiltCode tiltCode,
	const std::string& reason, 
	const BYTE *message,
	WORD messageLength)
{
	string textPath(CDirectoryManager::PollerBadMessageDiagnosticsTextFilePath());
	int bytesWritten = ReportBadPollerProtocolSendPacketToTextFile(textPath, CUtilities::GetTimeStamp(), tiltCode, reason, message, messageLength);

	string logPath(CDirectoryManager::PollerBadMessageDiagnosticsLogFilePath());
	ReportBadPollerProtocolSendPacketToLogFile(logPath, message, messageLength);

	return bytesWritten;
}

DWORD CDiagnostics::GetFileUpdateCount()
{
	return m_fileUpdateCount;
}

BOOL CDiagnostics::ReadEndOfFile(const std::shared_ptr<std::istream> &file, char *buffer, UINT bufferSize, UINT &bytesRead, ULONGLONG &fileSize)
{
	BOOL retval(FALSE);
	bytesRead = 0;
	fileSize = 0;

	fileSize = COperatingSystem::GetLength(file);
	if (fileSize > bufferSize)
	{
		ios::pos_type filePosition((ios::pos_type)(fileSize - bufferSize));
		file->seekg(filePosition, ios::beg);
	}

	bytesRead = file->read(buffer, bufferSize).gcount();

	retval = fileSize == 0 || bytesRead != 0;

	return retval;
}

long CDiagnostics::GetMaxPBTAmount()
{
	return m_ticketPbtMaxAmount->GetMaxPBTAmount();
}

long CDiagnostics::GetMaxTicketAmount()
{
	return m_ticketPbtMaxAmount->GetMaxTicketAmount();
}

#ifdef LEAK_CHECK
static volatile int malloc_hook_active = 0;
int live_pointers;
static int live_pointers_peak = 0;
static unsigned long int live_memory = 0;
static pthread_mutex_t cs_mutex =  PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
int malloc_seq;

typedef struct _heapEntry
{
	struct _heapEntry *next;
	void* ptr;
	size_t size;
	int seq;
	int csLength;
	void* cs;
} heapEntry;

typedef struct _heapHash 
{
	int size;
	heapEntry **table; 
} heapHash;

static heapHash* hash_table;

typedef struct _stackHeapEntry
{
	struct _stackHeapEntry *next;
	size_t totalSize;
	int instanceCount;
	int firstSeq;
	int lastSeq;
	int csLength;
	void* cs;
} stackHeapEntry;

typedef struct _stackHeapHash 
{
	int size;
	stackHeapEntry **table; 
} stackHeapHash;

static stackHeapHash* stack_hash_table;

heapHash* create_hash_table(int size)
{
	heapHash* ht;

	if ((ht = (heapHash*)malloc(sizeof(heapHash))) == nullptr) 
	{
		return nullptr;
	}

	if ((ht->table = (heapEntry**)malloc(sizeof(heapEntry *) * size)) == nullptr) 
	{
        free(ht);
		return nullptr;
	}

	for(int i=0; i<size; i++) 
		ht->table[i] = nullptr;

	ht->size = size;

	return ht;
}

unsigned int ptr_hash(void* ptr)
{
	return (((unsigned int)ptr) >> 2) & 0xFFFF;
}

heapEntry* look_up_ptr(heapHash* hash, void* ptr)
{
	heapEntry* list;
    unsigned int h = ptr_hash(ptr);

	for(list = hash->table[h]; list != nullptr; list = list->next) 
	{
		if (list->ptr == ptr) 
			return list;
	}

	return nullptr;
}

heapEntry* look_up_ptr_unlink_if_found(heapHash* hash, void* ptr)
{
	heapEntry *list, *prev;
    unsigned int h = ptr_hash(ptr);

	prev = nullptr;
	list = hash->table[h];
	while (list != nullptr)
	{
		if (list->ptr == ptr) 
		{
			if (prev != nullptr)
			{
				prev->next = list->next;
			}
			else
			{
				hash->table[h] = list->next;
			}

			return list;
		}

		prev = list;
		list = list->next;
	}

	return nullptr;
}

bool add_ptr(heapHash* hash, void* ptr, size_t size)
{
	void* callstack[128];
	const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
	int nFrames = backtrace2(callstack, nMaxFrames, 2);

	heapEntry* new_entry;
	unsigned int h = ptr_hash(ptr);

	if ((new_entry = (heapEntry*)malloc(sizeof(heapEntry) + sizeof(void*)*nFrames)) == nullptr) 
		return false;

	new_entry->ptr = ptr;
	new_entry->size = size;
    new_entry->seq = malloc_seq++;
	new_entry->next = hash->table[h];
	hash->table[h] = new_entry;
	new_entry->csLength = nFrames;

	void** cse = &new_entry->cs;
	for (int i = 0; i < nFrames; i++)
	{
		cse[i] = callstack[i];
	}

	return true;
}

void emit_backtrace_here()
{
	char buf[1024];
	void* callstack[32];
	const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
	int nFrames = backtrace2(callstack, nMaxFrames, 2);

	char **symbols = backtrace_symbols(callstack, nFrames);

	for (int i = 0; i < nFrames; i++) 
	{
		Dl_info info;
		if (dladdr(callstack[i], &info) && info.dli_sname) 
		{
			char *demangled = NULL;
			int status = -1;
			if (info.dli_sname[0] == '_')
				demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s + 0x%zx\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i],
					 status == 0 ? demangled :
					 info.dli_sname == 0 ? symbols[i] : info.dli_sname,
					 (char *)callstack[i] - (char *)info.dli_saddr);
			free(demangled);
		} 
		else 
		{
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
		}
		cout << buf;
	}
	free(symbols);
}

void DumpStackFor(heapEntry* entry)
{
	char buf[1024];
	void* callstack[32];
	void **cse = &entry->cs;

	for (int i = 0; i < entry->csLength; i++)
	{
		callstack[i] = cse[i];
	}

	char **symbols = backtrace_symbols(callstack, entry->csLength);

	for (int i = 0; i < entry->csLength; i++) 
	{
		Dl_info info;
		if (dladdr(callstack[i], &info) && info.dli_sname) 
		{
			char *demangled = NULL;
			int status = -1;
			if (info.dli_sname[0] == '_')
				demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s + 0x%zx\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i],
					 status == 0 ? demangled :
					 info.dli_sname == 0 ? symbols[i] : info.dli_sname,
					 (char *)callstack[i] - (char *)info.dli_saddr);
			free(demangled);
		} 
		else 
		{
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
		}
		cout << buf;
	}
	free(symbols);
}

void enumerate_ptr_hash(heapHash *hash)
{
	int i;
	heapEntry *list;

	if (hash==nullptr) 
		return;

	for(i=0; i<hash->size; i++) 
	{
		list = hash->table[i];

		while(list!=nullptr) 
		{
			cout << list->ptr << " " << list->size << "\n";
			DumpStackFor(list);

			list = list->next;
		}
	}
	cout << "\n";
}

void* malloc_hook (size_t size);
void free_hook(void *ptr);
void* realloc_hook(void *__ptr, size_t __size);

stackHeapHash* create_stack_hash_table(int size)
{
	stackHeapHash* ht;

	if ((ht = (stackHeapHash*)malloc(sizeof(stackHeapHash))) == nullptr) 
	{
		return nullptr;
	}

	if ((ht->table = (stackHeapEntry**)malloc(sizeof(stackHeapEntry *) * size)) == nullptr) 
	{
        free(ht);
		return nullptr;
	}

	for(int i=0; i<size; i++) 
		ht->table[i] = nullptr;

	ht->size = size;

	return ht;
}

unsigned int stack_hash(void** stack, int stack_len, int heap_size)
{
	unsigned int retVal = 0;

	if (stack_len > 0)
	{
		retVal = (unsigned int)stack[0];
		for (int i = 1; i < stack_len; i++)
		{
			retVal ^= (unsigned int)stack[i];
		}

		retVal %= heap_size;
	}
	
	return retVal;
}

stackHeapEntry* look_up_stack(stackHeapHash* hash, void* stack[], int stack_len)
{
	stackHeapEntry* retVal = nullptr;

	stackHeapEntry* list;
	unsigned int h = stack_hash(stack, stack_len, hash->size);

	for(list = hash->table[h]; list != nullptr; list = list->next) 
	{
		bool found = false;

		if (list->csLength == stack_len)
		{
			found = true;

			void** lcs = &list->cs;

			for (int i = 0; i < stack_len; i++)
			{
				if (stack[i] != lcs[i])
				{
					found = false;
					break;
				}
			}
		}

		if (found)
		{
			retVal = list;
			break;
		}
	}

	return retVal;
}

bool add_stack(stackHeapHash* hash, heapEntry* heap_entry)
{
	stackHeapEntry* new_stack_entry;
	unsigned int h = stack_hash(&heap_entry->cs, heap_entry->csLength, hash->size);

	if ((new_stack_entry = (stackHeapEntry *)malloc(sizeof(stackHeapEntry) + sizeof(void *) * heap_entry->csLength)) == nullptr)
		return false;

	new_stack_entry->instanceCount = 1;
	new_stack_entry->totalSize = heap_entry->size;
    new_stack_entry->firstSeq = heap_entry->seq;
    new_stack_entry->lastSeq = -1;
	new_stack_entry->next = hash->table[h];
	hash->table[h] = new_stack_entry;
	new_stack_entry->csLength = heap_entry->csLength;

	void** css = &heap_entry->cs;
	void** cst = &new_stack_entry->cs;
	for (int i = 0; i < heap_entry->csLength; i++)
	{
		cst[i] = css[i];
	}

	return true;
}

void consolidate_by_backtrace(stackHeapHash* stack_hash, heapHash* heap_hash)
{
	int i;
	heapEntry *list;

	if (heap_hash==nullptr) 
		return;

	for(i=0; i < heap_hash->size; i++) 
	{
		list = heap_hash->table[i];

		while(list!=nullptr) 
		{
			stackHeapEntry *s = look_up_stack(stack_hash, &list->cs, list->csLength);

			if (s != nullptr)
			{
				s->instanceCount++;
				s->totalSize += list->size;
				int sq = list->seq;
				if (sq < s->firstSeq)
				{
					if (s->lastSeq == -1)
					{
						s->lastSeq = s->firstSeq;
					}
					s->firstSeq = sq;
				}
				else if (s->lastSeq == -1 || sq > s->lastSeq)
				{
					s->lastSeq = sq;
				}				
			}
			else
			{
				add_stack(stack_hash, list);
			}
			
			list = list->next;
		}
	}
}

void LockHeapAndDisableTracking()
{
    pthread_mutex_lock( &cs_mutex );
    malloc_hook_active--;
}

void UnlockHeapAndReenableTracking()
{
    malloc_hook_active++;
    pthread_mutex_unlock( &cs_mutex );
}

void* malloc (size_t size)
{
	void* retVal;
	pthread_mutex_lock( &cs_mutex );	
    if (malloc_hook_active > 0)
	{
		retVal = malloc_hook(size);
	}
	else
	{
	  static void* (*const orgmalloc)(size_t size) = (void* (*)(size_t size))dlsym(RTLD_NEXT, "malloc");
	  retVal = orgmalloc(size);
	}
	pthread_mutex_unlock( &cs_mutex );
	return retVal;
}

void free(void *ptr)
{
	pthread_mutex_lock( &cs_mutex );	
	if (malloc_hook_active > 0)
	{
	  free_hook(ptr); 
	}
	else
	{
		static void (*const orgfree)(void* ptr) = (void (*)(void *ptr))dlsym(RTLD_NEXT, "free");
		orgfree(ptr);
	}
	pthread_mutex_unlock( &cs_mutex );
}

void* realloc(void *ptr, size_t size)
{
	void* retVal;
	pthread_mutex_lock( &cs_mutex );	
	if (malloc_hook_active > 0)
	{
		retVal = realloc_hook(ptr, size);
	}
	else
	{
		static void* (*const orgrealloc)(void* ptr, size_t size) = (void* (*)(void* ptr, size_t size))dlsym(RTLD_NEXT, "realloc");
		retVal = orgrealloc(ptr, size);
	}
	pthread_mutex_unlock( &cs_mutex );
	return retVal;
}

void* malloc_hook (size_t size)
{
	void *result;

	// deactivate hooks for leak code's internal memory management
	malloc_hook_active--;

	result = malloc(size);

	if (result != nullptr)
	{
		heapEntry *look = look_up_ptr(hash_table, result);
		if (look != nullptr)
		{
			//cout << "internal error. ptr " << result << " returned from malloc already in hash\n";
			//emit_backtrace_here();
		}
		else
		{
			add_ptr(hash_table, result, size);
		}

		if (++live_pointers > live_pointers_peak)
		{
			live_pointers_peak = live_pointers;
		}
		live_memory += size;
	}

	// reactivate hooks
	malloc_hook_active++;

	return result;
}

void free_hook (void* ptr)
{
	// deactivate hooks for leak code's internal memory management
	malloc_hook_active--;

	if (ptr != nullptr)
	{
		heapEntry *look = look_up_ptr_unlink_if_found(hash_table, ptr);
		if (look == nullptr)
		{
			//cout << "\nfree: ptr " << ptr << " was not found in hash\n";
			//emit_backtrace_here();
		}
		else
		{
			live_pointers--;
			live_memory -= look->size; 
			free(look);
		}

		free(ptr);
	}
	// reactivate hooks
	malloc_hook_active++;  
}

void* realloc_hook(void* ptr, size_t size)
{
	void *result;

	// deactivate hooks for leak code's internal memory management
	malloc_hook_active--;

	if (ptr != nullptr)
	{
		heapEntry *look = look_up_ptr_unlink_if_found(hash_table, ptr);
		if (look == nullptr)
		{
			//cout << "realloc: ptr " << ptr << " was not found in hash\n";
			//emit_backtrace_here();
		}
		else
		{
			live_pointers--;
			live_memory -= look->size; 
			free(look);
		}
	}

	result = realloc(ptr, size);

	if (result != nullptr)
	{
		add_ptr(hash_table, result, size);
		live_pointers++;
		live_memory += size; 
	}

	// reactivate hooks
	malloc_hook_active++;

	return result;
}

void dump_stack_for_heap_entry(stackHeapEntry* entry)
{
	char buf[1024];
	void* callstack[32];
	void **cse = &entry->cs;

	for (int i = 0; i < entry->csLength; i++)
	{
		callstack[i] = cse[i];
	}

	char **symbols = backtrace_symbols(callstack, entry->csLength);

	for (int i = 0; i < entry->csLength; i++) 
	{
		Dl_info info;
		if (dladdr(callstack[i], &info) && info.dli_sname) 
		{
			char *demangled = NULL;
			int status = -1;
			if (info.dli_sname[0] == '_')
				demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s + 0x%zx\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i],
					 status == 0 ? demangled :
					 info.dli_sname == 0 ? symbols[i] : info.dli_sname,
					 (char *)callstack[i] - (char *)info.dli_saddr);
			free(demangled);
		} 
		else 
		{
			snprintf(buf, sizeof(buf), "%-3d [%*p] %s\n",
					 i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
		}
		cout << buf;
	}
	free(symbols);
}

int enumerate_stack_hash(stackHeapHash *hash, int instanceThreshold)
{
	int retVal = 0;

	int i;
	stackHeapEntry *list;

	if (hash != nullptr) 
	{
		for(i=0; i<hash->size; i++) 
		{
			list = hash->table[i];

			while(list!=nullptr) 
			{
				if (list->instanceCount >= instanceThreshold)
				{
					cout << "\ntotal size: " << list->totalSize << " instance count:" << list->instanceCount << " seq: " << list->firstSeq << "/" << list->lastSeq << "\n";
					dump_stack_for_heap_entry(list);
					retVal++;
				}

    			list = list->next;
    		}
		}
    	cout << "\n";
	}

	return retVal;
}

void InitMemoryLeakChecking()
{
	cout << "########### LEAK CHECKING ENABLED\n";
	malloc_seq = 0;
	live_pointers = 0;
	hash_table = create_hash_table(0x10000);

	malloc_hook_active = 1;
}

void ShowMemoryLeakInfo()
{
	malloc_hook_active = 0;
	stack_hash_table = create_stack_hash_table(101);
	//enumerate_ptr_hash(hash_table);
	consolidate_by_backtrace(stack_hash_table, hash_table);
	int instanceThreshold = 1;
	int numLeakLocations = enumerate_stack_hash(stack_hash_table, instanceThreshold);
	cout << "### REPORTING THRESHOLD: " << instanceThreshold << " INSTANCE(S). LIVE POINTERS: " << live_pointers << " PEAK: " << live_pointers_peak << " LIVE MEMORY: " << live_memory << " UNIQUE LOCATIONS: " << numLeakLocations << "\n";
}

#endif


