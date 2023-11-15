#include "stdafx.h"
#include "ProgressiveTransactionResendList.h"
#include "DirectoryManager.h"
#include <glob.h> // glob(), globfree()
#include <sys/stat.h> // mkdir

// Sentinel includes.
#include "Logging/LogString.h"
#include "../Utilities.h"

using namespace std;

const WORD TRANSACTION_LENGTH_LOCATION = 1;
const WORD TRANSACTION_DATA_LOCATION = 3;
const WORD TRANSACTION_ID_LOCATION = 39;

// Size of transaction fields.
const string PROG_RESEND_DIRECTORY = "/progressive-resend/";
const string PROG_RESEND_PATTERN = "*.dat";
const string REMOVE_COMMAND = "rm ";

CProgressiveTransactionResendList::CProgressiveTransactionResendList(bool sdcardValid, MEMORY_TYPE type)
{
	m_type = type;

	m_dwAllowedSize = NVRAM_TRANSACTIONS_SIZE;
	//m_dwOffset = NVRAM_TRANSACTIONS_OFFSET;
	m_bOffsetIsSet = true;

	// reset everything based on current state of SDCARD
	m_sdcardValid = sdcardValid;

	// Make sure SD resend list directoy exists
	CheckResendDirectory();

	if (m_type == MEMORY_SDCARD)
	{
		if (!IntegrityCheck())
		{
			LogString(PROGRESSIVE, "%s %d failed integrity check.", __FUNCTION__, __LINE__);
			// Failed integrity check, attempt to remove resend transactions
			DeleteAllResendListFiles();
		}
		else
		{
			BuildYourself();
		}
	}

	m_currentItemIndex = 0;
}

CProgressiveTransactionResendList::~CProgressiveTransactionResendList(void)
{
	m_resendList.clear();
}

void CProgressiveTransactionResendList::DeleteAllResendListFiles()
{
	string pattern = REMOVE_COMMAND + CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + PROG_RESEND_DIRECTORY + PROG_RESEND_PATTERN;
	system(pattern.c_str());
	LogString(PROGRESSIVE, "%s %d Delete all resend list files. pattern=%s", __FUNCTION__, __LINE__, pattern.c_str());
}

///
/// Refill the list from SDCARD at startup
///
void CProgressiveTransactionResendList::BuildYourself()
{
	// scan through SDCARD resend buffer area to see what outstanding transactions remain and order them according to uniqueId.
    // glob struct resides on the stack
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));
	string pattern = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + PROG_RESEND_DIRECTORY + PROG_RESEND_PATTERN;

    // do the glob operation
    int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if(return_value == 0) 
	{
        // collect all the filenames into a std::list<std::string>
		vector<string> filenames;
		for(size_t i = 0; i < glob_result.gl_pathc; ++i) 
		{
			string filename = glob_result.gl_pathv[i];

			lock_guard<recursive_mutex> lock(m_criticalSection);
			try
			{
				CProgressiveTransactionItem ti(filename);
				ti.SetLastSent(); // Set this to now
				if (ti.IsMessageTypeValid(ti.GetMsgType()))
				{
					// Add to resend List
					ProgessiveInsertReturn success = m_resendList.insert(ProgressiveResendListPair(ti.GetUniqueId(), ti));
				}
			}
			catch(...)
			{ }
		}
	}

    // cleanup
    globfree(&glob_result);

	if (m_resendList.size() > 0)
	{
		LogString( PROGRESSIVE, "Progressive transaction resend list size at startup = %d", m_resendList.size());
	}
}


void CProgressiveTransactionResendList::CheckResendDirectory() 
{
	string directory = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) + PROG_RESEND_DIRECTORY;
	struct stat buffer;
	if (stat(directory.c_str(), &buffer) != 0)
	{
		// create directory
		LogString( PROGRESSIVE, "Progressive resend list directory not found, creating.");
		if (mkdir(directory.c_str(), 0777))
		{
			LogString(ERROR_LOG, "Unable to create Progressive resend list directory.");
		}
	}
	
}
///
///Is the resend list empty?
///
BOOL CProgressiveTransactionResendList::IsEmpty()
{
	SizeReturn ret = m_resendList.size();
	return  ret == 0;
}

///
/// What percentage of the resend list is used?
///
int CProgressiveTransactionResendList::GetPercentageFull()
{
	int percentFull;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		SizeReturn ret = m_resendList.size();
		percentFull = (ret * ONE_HUNDRED_PERCENT) / MAX_NUMBER_OF_TRANSACTIONS ;
	}

	return percentFull;
}

DWORD CProgressiveTransactionResendList::GetHighestUniqueId()
{
	DWORD retval = 0;
	SizeReturn ret = m_resendList.size();

	if (ret > 0)
	{
		ProgressiveResendListMap::iterator iter = m_resendList.end();
		iter--;
		retval = iter->first;
	}

	return retval;
}


///
/// Add a new transacton to the resend list
///
bool CProgressiveTransactionResendList::AddTransaction(ToOneLink::Type msgType, DWORD uniqueId, const std::string &json)
{
	// Add a transaction to the resend list
	WORD totalBytesWritten = 0;
	UINT lengthToWrite;
	bool retVal = false;
	SizeReturn ret = m_resendList.size();

	if(ret < MAX_NUMBER_OF_TRANSACTIONS)
	{
		lock_guard<recursive_mutex> lock(m_criticalSection);
		CProgressiveTransactionItem ti(msgType, uniqueId, json);
		ti.SetLastSent(); // Set this to now as we always add the OneLink queue before adding to the resend list

		// Add to resend List
		ProgessiveInsertReturn success = m_resendList.insert(ProgressiveResendListPair(uniqueId, ti));
		retVal = success.second;
	}

	return retVal;
}

bool CProgressiveTransactionResendList::RemoveTransaction(DWORD uniqueId)
{
	// remove transaction from resend list and clear SDCARD file.
	// return true if found and removed

	bool found = false;
	CProgressiveTransactionItem ti;

	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.count(uniqueId);

	if (ret > 0)
	{
		found = true;
		ti = m_resendList[uniqueId];

		if(m_type == MEMORY_SDCARD)
		{
			// remove from SD card if it has been saved
			ti.RemoveFile();
		}

		m_resendList.erase(uniqueId);
	}

	return ( found );
}

bool CProgressiveTransactionResendList::SetLastSent( DWORD uniqueId )
{
	ProgressiveResendListMap::iterator iterator;
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if(  m_resendList.size() > 0 ){
		iterator = m_resendList.find( uniqueId );

		if( iterator != m_resendList.end() ){
			iterator->second.SetLastSent();
		}
	}

	return iterator != m_resendList.end();
}

ProgressiveTransactionType CProgressiveTransactionResendList::GetTransactionType(DWORD uniqueId)
{
	CProgressiveTransactionItem ti;

	ProgressiveTransactionType type = NONE;
	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.count(uniqueId);

	if (ret > 0)
	{
		ti = m_resendList[uniqueId];
		type = (ProgressiveTransactionType)ti.GetMsgType();
	}

	return type;
}

bool CProgressiveTransactionResendList::GetFirstItem(CProgressiveTransactionItem &ti, DWORD &outUniqueId, DWORD &outLastSent, bool &alreadySuspicious)
{
	bool retVal = false;
	lock_guard<recursive_mutex> lock(m_criticalSection);
	ProgressiveResendListMap::iterator iterator;
	SizeReturn ret = m_resendList.size();

	if (ret > 0)
	{
		// Get first
		iterator = m_resendList.begin();
		ti = iterator->second;
		outUniqueId = ti.GetUniqueId();
		outLastSent = ti.GetLastSent();
		alreadySuspicious = ti.IsPersisted();
		retVal = true;
	}

	return retVal;
}

// CTransactionItem member functions
CProgressiveTransactionItem::CProgressiveTransactionItem()
	:
	m_msgType(ToOneLink::NumberOfToOneLinkMessages),
	m_uniqueId(0),
	m_lastSent(0),
	m_isPersisted(false)
{
}



ToOneLink::Type CProgressiveTransactionItem::GetMsgType()
{
	return m_msgType;
}

CProgressiveTransactionItem::CProgressiveTransactionItem(ToOneLink::Type msgType, DWORD uniqueId, const std::string &json)
{
	m_msgType = msgType;
	m_uniqueId = uniqueId;
	m_json = json;
	m_lastSent = 0;
	m_isPersisted = false;

	// Determine filename
	stringstream ss;
	stringstream unique_idss;
	unique_idss << std::hex << setfill('0') << std::setw(8) << m_uniqueId ; // format as 8 character hex string
	ss << CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA) << PROG_RESEND_DIRECTORY << unique_idss.str() << "-" << static_cast<int>(m_msgType) << ".dat";
	m_filename =  ss.str();
	Persist();
}

CProgressiveTransactionItem::CProgressiveTransactionItem(const std::string &filename)
{
	m_uniqueId = 0;
	m_lastSent = 0;
	m_isPersisted = false;
	m_msgType = ToOneLink::NumberOfToOneLinkMessages;

	// Restore item from SD
	try
	{
		m_filename = filename;
		ifstream ifstrm(filename, std::ios::binary);
		int msgType(ToOneLink::NumberOfToOneLinkMessages);
		if (ifstrm.is_open())
		{
			// read file
			ifstrm >> hex >> m_uniqueId;
			ifstrm.ignore();
			ifstrm >> msgType;
			ifstrm.ignore();
			char nextChar;
			while (!ifstrm.get(nextChar).eof())
			{
				m_json += nextChar;
			}

			//Don't try to cast an invalid message type because 
			// this can result in inconsistent message type
			if (IsMessageTypeValid(msgType))
			{
				m_msgType = static_cast<ToOneLink::Type>(msgType);
				const char* pMessageType = ToOneLink::emap.At(m_msgType, nullptr);
				LogString(PROGRESSIVE, "Restoring progressive resend item type=%s message filename=%s", pMessageType, filename.c_str());
				m_isPersisted = true;
			}
			else
			{
				remove(filename.c_str());
				LogString(ERROR_LOG, "Restoring progressive resend item failed because msgType=%d is invalid file=%s is deleted.", msgType, filename.c_str());
			}
		}
	}
	catch(...)
	{
		LogString(ERROR_LOG, "Exception restoring progressive resend item: file %s",
			filename.c_str());

		// attempt to remove file to prevent this error on every reboot
		remove(filename.c_str());

		// Let CProgressiveTransactionList handle exception
		throw;
	}
}

CProgressiveTransactionItem & CProgressiveTransactionItem::operator=(const CProgressiveTransactionItem &itemRHS )
{
	m_uniqueId = itemRHS.m_uniqueId;
	m_json = itemRHS.m_json;
	m_lastSent = itemRHS.m_lastSent;
	m_isPersisted = itemRHS.m_isPersisted;
	m_msgType = itemRHS.m_msgType;
	m_filename = itemRHS.m_filename;

	return *this;
}

bool CProgressiveTransactionItem::IsMessageTypeValid(int msgType) const
{
	return (msgType >= ToOneLink::CoinIn5 && msgType < ToOneLink::NumberOfToOneLinkMessages);
}

DWORD CProgressiveTransactionItem::GetUniqueId()
{
	return m_uniqueId;
}

void CProgressiveTransactionItem::SetUniqueId(DWORD uniqueId)
{
	m_uniqueId = uniqueId;
}

DWORD CProgressiveTransactionItem::GetLastSent()
{
	return m_lastSent;
}

void CProgressiveTransactionItem::SetLastSent()
{
	m_lastSent = CUtilities::GetTickCount();
}

// We call this when loading the resend list from SDCARD so that items on the resend list are sent immediately
// so that they do not get out of order (if PMT is not up then they are set in the resend list correctly
void CProgressiveTransactionItem::ResetLastSent()
{
	m_lastSent = 0;
}

bool CProgressiveTransactionItem::IsPersisted() const
{
	return m_isPersisted;
}

void CProgressiveTransactionItem::Persist() 
{
	// named uniqueid-msgType.dat
	// file contents:
	// DWORD unique id
	// ToOneLink::Type msgType
	// json string
	try
	{
		// Create file
		stringstream unique_idss;
		unique_idss << std::hex << setfill('0') << std::setw(8) << m_uniqueId ; // format as 8 character hex string
		std::ofstream ostrm(m_filename, std::ios::binary);
		ostrm << unique_idss.str() << "-" << static_cast<int>(m_msgType) << "-" << m_json;
		m_isPersisted = true;
		LogString( PROGRESSIVE, "Created progressive resend item %s", m_filename.c_str());
	}
	catch(const std::exception& e)
	{
		LogString( ERROR_LOG, "Could not create progressive resend item %s", m_filename.c_str());
	}
}

void CProgressiveTransactionItem::RemoveFile() 
{
	if (m_isPersisted)
	{
		if (!remove(m_filename.c_str()))
		{
			// success
			LogString( PROGRESSIVE, "Successfully removed progressive resend item %s", m_filename.c_str());
		}
		else
		{
			// error
			LogString( ERROR_LOG, "Could not remove progressive resend item %s", m_filename.c_str());
		}
		
	}
}


void CProgressiveTransactionItem::setJson(const std::string &json)
{
	m_json = json;
}

std::string CProgressiveTransactionItem::GetJson() 
{
	return m_json;
}
