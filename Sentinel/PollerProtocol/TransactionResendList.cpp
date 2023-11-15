#include "stdafx.h"
#include "TransactionResendList.h"

// Sentinel includes.
#include "Logging/LogString.h"
#include "PollerTicketResponseMessage.h" //for the offsets within the ticket message
#include "../Utilities.h"

using namespace std;

const byte NVRAM_TRANSACTION_BLOCK_USED = 0xff;
const byte NVRAM_TRANSACTION_BLOCK_UNUSED = 0;
const WORD TRANSACTION_LENGTH_LOCATION = 1;
const WORD TRANSACTION_DATA_LOCATION = 3;
const WORD TRANSACTION_ID_LOCATION = 39;

// Size of transaction fields.
const DWORD TRANSACTION_BLOCK_USED_SIZE = sizeof (BYTE);
const DWORD TRANSACTION_DATA_LENGTH_SIZE = sizeof (WORD);
const DWORD TRANSACTION_IS_TICKET_OUT_SIZE = sizeof (BYTE);

const DWORD TRANSACTION_DATA_LENGTH_MAXIMUM =
	NVRAM_TRANSACTION_SIZE -
	(TRANSACTION_BLOCK_USED_SIZE + TRANSACTION_DATA_LENGTH_SIZE + TRANSACTION_IS_TICKET_OUT_SIZE);
const DWORD BAD_TRANSACTION_LOG_LENGTH = 256;

CTransactionResendList::CTransactionResendList(bool nvramValid, MEMORY_TYPE type)
{
	m_type = type;

	m_dwAllowedSize = NVRAM_TRANSACTIONS_SIZE;
	m_dwOffset = NVRAM_TRANSACTIONS_OFFSET;
	m_bOffsetIsSet = true;

	// reset everything based on current state of NVRAM
	m_nvramValid = nvramValid;

	if (m_type == MEMORY_NVRAM)
	{
		BuildYourself();
	}

	m_currentItemIndex = 0;
}

CTransactionResendList::~CTransactionResendList(void)
{
	CTransactionItem ti;
	ResendListMap::iterator resendIterator;

	for (resendIterator = m_resendList.begin(); resendIterator != m_resendList.end(); resendIterator++)
	{
		ti = resendIterator->second;
		delete [] ti.GetTransPtr();
	}

	m_resendList.clear();
}

///
/// Refill the list from NVRAM at startup
///
void CTransactionResendList::BuildYourself()
{
	// scan through nvram resend buffer area to see what outstanding transactions remain and order them according to uniqueId.
	// also build free Nv Ram List

	DWORD uniqueId;
	WORD transLength;
	byte blockStatus;
	int itemCount = 0;

	m_initialTicketOutCount = 0;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	DWORD offset = NVRAM_TRANSACTIONS_OFFSET;
	for (int i = 0; i < (int)MAX_NUMBER_OF_TRANSACTIONS; i++, offset += NVRAM_TRANSACTION_SIZE)
	{
		if (!m_nvramValid) 
		{
			// clear item out, add to free list
			Write(offset, (BYTE *)&NVRAM_TRANSACTION_BLOCK_UNUSED, sizeof(byte));
			m_freeNvList.push_back(offset);
		}
		else
		{
			// Get first block status and transaction length
			Read(offset, &blockStatus, sizeof(blockStatus));
			if (blockStatus)
			{
				// memory spot used, add to resend list
				// read transaction length
				Read(offset + TRANSACTION_LENGTH_LOCATION, (BYTE *)&transLength, sizeof(transLength));
				if (transLength > TRANSACTION_DATA_LENGTH_MAXIMUM)
				{
					// Log the first part of the transaction buffer.
					// For the transaction data format, see CPollerSlotmastTransactionMessage::SetReadWriteBuffer().
					BYTE data[BAD_TRANSACTION_LOG_LENGTH];
					Read(offset, data, sizeof data);
					string dataStr = CUtilities::ToHexString(data, sizeof data);
					LogString(ERROR_LOG,
						_T("NVRAM transLength=%u > MAX=%u i=%d offset=%u Data=%s"),
						(unsigned)transLength,
						(unsigned)TRANSACTION_DATA_LENGTH_MAXIMUM,
						(int)i,
						(unsigned)offset,
						static_cast<LPCTSTR>(dataStr.c_str()));

					// Make the transLength usable.
					transLength = TRANSACTION_DATA_LENGTH_MAXIMUM;
				}

				CTransactionItem tranItem;
				tranItem.SetTransPtr( new byte[transLength]);
				tranItem.SetTransLength(transLength);
				tranItem.SetNvramOffset(offset);

				// read transaction into tranItem
				Read(offset + TRANSACTION_DATA_LOCATION, (BYTE *)tranItem.GetTransPtr(),transLength);

				bool tempIsTicketOut;
				Read(offset + TRANSACTION_DATA_LOCATION + transLength, (byte*)&tempIsTicketOut, sizeof(tempIsTicketOut));
				tranItem.SetAsTicketOut(tempIsTicketOut);
				if(tempIsTicketOut)
				{
					m_initialTicketOutCount++;
				}

				// copy UniqueId into tranItem
				memcpy(&uniqueId,(BYTE *)tranItem.GetTransPtr() + TRANSACTION_ID_LOCATION ,sizeof(DWORD));
				tranItem.SetUniqueId(uniqueId);

				tranItem.ResetLastSent(); // Resend on startup otherwise we get out of order

				tranItem.SetMsgType(*(tranItem.GetTransPtr() + SIZE_OF_MESSAGE_HEADER));

				// add item to resend list
				InsertReturn success = m_resendList.insert(ResendListPair(uniqueId, tranItem));
				if (!success.second)
				{
					CTransactionItem temp = m_resendList[uniqueId];
					delete temp.GetTransPtr();
					m_resendList[uniqueId] = tranItem;
				}
				itemCount++;
			}
			else
			{
				// memory spot unused, add to free list
				m_freeNvList.push_back(offset);

			}
		}

	}

	if (itemCount > 0)
	{
		LogString( GENERAL, "Poller transaction resend list size at startup = %d", itemCount );
	}

	m_nvramValid = true;
}

///
///How many tickets are in the resend list?
///
int CTransactionResendList::GetInitialTicketOutCount()
{
	return m_initialTicketOutCount;
}

///
///Is the resend list empty?
///
BOOL CTransactionResendList::IsEmpty()
{
	SizeReturn ret = m_resendList.size();
	return  ret == 0;
}

///
/// What percentage of the resend list is used?
///
int CTransactionResendList::GetPercentageFull()
{
	int percentFull;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	{
		SizeReturn ret = m_resendList.size();
		percentFull = (ret * ONE_HUNDRED_PERCENT) / MAX_NUMBER_OF_TRANSACTIONS ;
	}

	return percentFull;
}

DWORD CTransactionResendList::GetHighestUniqueId()
{
	DWORD retval = 0;
	SizeReturn ret = m_resendList.size();

	if (ret > 0)
	{
		ResendListMap::iterator iter = m_resendList.end();
		iter--;
		retval = iter->first;
	}

	return retval;
}


///
/// Add a new transacton to the resend list
///
bool CTransactionResendList::AddTransaction(byte msgType, DWORD uniqueId, byte *transPtr, WORD transLength, bool storeInNVRAM,  bool isTicketOut)
{
	// Add a transaction to the resend list
	WORD totalBytesWritten = 0;
	UINT lengthToWrite;

	bool retVal = false;
	SizeReturn ret = m_resendList.size();

	if((transLength <= TRANSACTION_DATA_LENGTH_MAXIMUM) && (ret < MAX_NUMBER_OF_TRANSACTIONS))
	{
		retVal = true;
		lock_guard<recursive_mutex> lock(m_criticalSection);

		// Add to NVRAM, first get free block from free list
		// keep copy of transaction buffer
		byte *buffer = new byte[transLength];
		memcpy(buffer,transPtr, transLength);

		if ((m_type == MEMORY_NVRAM) && storeInNVRAM)
		{
			if (!m_freeNvList.empty())
			{

				DWORD offset = m_freeNvList.front();
				m_freeNvList.pop_front();
				while (totalBytesWritten < transLength)
				{
					// write up to 256 bytes
					lengthToWrite = transLength - totalBytesWritten > 256 ? 256 : transLength - totalBytesWritten;
					Write(offset + TRANSACTION_DATA_LOCATION + totalBytesWritten, buffer + totalBytesWritten, lengthToWrite);
					totalBytesWritten += lengthToWrite;
				}
				// This is appended to show if it is a ticket out.
				Write(offset + TRANSACTION_DATA_LOCATION + transLength, (byte *)&isTicketOut, sizeof(isTicketOut));
				// And complete the write by setting the block as used.
				Write(offset, (byte *)&NVRAM_TRANSACTION_BLOCK_USED, sizeof(byte),offset + TRANSACTION_LENGTH_LOCATION, (BYTE *)&transLength, sizeof(WORD));

				CTransactionItem ti(msgType, uniqueId, buffer, offset);
				ti.SetLastSent(); // Set this to now as we always add the poller queue before adding to the resend list
				ti.SetTransLength(transLength);
				ti.SetAsTicketOut(isTicketOut);

				// Add to resend List
				InsertReturn success = m_resendList.insert(ResendListPair(uniqueId, ti));
				if (!success.second)
				{ 
					CTransactionItem temp = m_resendList[uniqueId];
					delete temp.GetTransPtr();
					m_resendList[uniqueId] = ti;
				}
			}
			else
			{
				LogString(ERROR_LOG, "Transaction Resend List Full.");
			}
		}
		else
		{
			//put it in the resend list, but not in NVRAM
			CTransactionItem ti(msgType, uniqueId, buffer/*, resendTime, timeToExpire*/, 0);
			ti.SetTransLength(transLength);
			ti.SetAsTicketOut(isTicketOut);
			ti.SetLastSent();

			// Add to resend List
			InsertReturn success = m_resendList.insert(ResendListPair(uniqueId, ti));
			if (!success.second)
			{ 
				CTransactionItem temp = m_resendList[uniqueId];
				delete temp.GetTransPtr();
				m_resendList[uniqueId] = ti;
			}
		}
	}

	return retVal;
}

bool CTransactionResendList::RemoveTransaction(DWORD uniqueId, bool isStoredInNVRAM)
{
	// remove transaction from resend list and clear nvram block.
	// return true if found and removed

	bool found = false;
	CTransactionItem ti;

	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.count(uniqueId);

	if (ret > 0)
	{
		found = true;
		ti = m_resendList[uniqueId];

		if((m_type == MEMORY_NVRAM) && isStoredInNVRAM)
		{
			// update nvram
			BYTE value = NVRAM_TRANSACTION_BLOCK_UNUSED;
			Write(ti.GetNvramOffset(),&value, sizeof(byte));
			// add nvram chunk back into free list
			m_freeNvList.push_back(ti.GetNvramOffset());
		}

		// delete transptr
		delete [] ti.GetTransPtr();
		m_resendList.erase(uniqueId);
	}

	return ( found );
}

void CTransactionResendList::SetSuspicious(DWORD uniqueId)
{
	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.count(uniqueId);
	if (ret > 0)
	{
		m_resendList[uniqueId].SetSuspicious();
	}
}

bool CTransactionResendList::SetLastSent( DWORD uniqueId )
{
	ResendListMap::iterator iterator;
	lock_guard<recursive_mutex> lock(m_criticalSection);

	if(  m_resendList.size() > 0 ){
		iterator = m_resendList.find( uniqueId );

		if( iterator != m_resendList.end() ){
			iterator->second.SetLastSent();
		}
	}

	return iterator != m_resendList.end();
}

bool CTransactionResendList::RemoveChangeSlotMastIDTransaction(DWORD SlotMastID)
{
	bool foundSlotMastTransaction = false;
	CTransactionItem ti;

	{
		lock_guard<recursive_mutex> lock(m_criticalSection);

		// Iterate through list.
		byte * transBuffer;
		DWORD transSlotmastID;

		ResendListMap::iterator resendIterator;

		for (resendIterator = m_resendList.begin(); resendIterator != m_resendList.end() && !foundSlotMastTransaction; resendIterator++)
		{
			ti = resendIterator->second;

			transBuffer = ti.GetTransPtr();
			if ((ti.GetMsgType() == CHANGE_SLOTMAST_ID_CODE))
			{
				int offset = ti.GetTransLength() - sizeof(DWORD);
				memcpy_s(&transSlotmastID, sizeof(transSlotmastID), transBuffer + offset, sizeof(DWORD));
				if (SlotMastID == transSlotmastID)
				{
					foundSlotMastTransaction = true;
				}
			}
		}
	}

	if(foundSlotMastTransaction)
	{
		RemoveTransaction(ti.GetUniqueId(), true);
	}

	return (foundSlotMastTransaction);
}

TransactionType CTransactionResendList::GetTransactionType(DWORD uniqueId)
{
	CTransactionItem ti;

	TransactionType type = NONE_SPECIFIED;
	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.count(uniqueId);

	if (ret > 0)
	{
		ti = m_resendList[uniqueId];
		type = (TransactionType)ti.GetMsgType();
	}

	return type;
}

bool CTransactionResendList::GetTicketTransaction(CTicket * ticket, bool &isTicketOut, DWORD &dwUniqueID)
{
	//This will attenpt to remove a ticket from the resend list
	//if it is successful, then it returns true, and the isticketOut reference
	//variable is set to true if it was a printed ticket.
	bool foundMatchingTicket = false;
	CTransactionItem ti;

	m_criticalSection.lock();

	// Iterate through list.
	ResendListMap::iterator resendIterator;

	for (resendIterator = m_resendList.begin(); resendIterator != m_resendList.end() && !foundMatchingTicket; resendIterator++)
	{
		ti = resendIterator->second;

		if ((ti.GetMsgType() == TICKET_CODE) && (ti == *ticket))
		{
			foundMatchingTicket = true;
		}
	}

	m_criticalSection.unlock();

	if(foundMatchingTicket)
	{
		isTicketOut = ti.IsTicketOut();
		dwUniqueID = (ti.GetUniqueId());
	}

	return (foundMatchingTicket);
}

bool CTransactionResendList::GetNULLValidationNumberTicket(DWORD &dwUniqueID, bool &isTicketOut)
{
	bool foundMatchingTicket = false;
	CTransactionItem ti;

	m_criticalSection.lock();

	// Iterate through list.
	ResendListMap::iterator resendIterator;

	for (resendIterator = m_resendList.begin(); resendIterator != m_resendList.end() && !foundMatchingTicket; resendIterator++)
	{
		ti = resendIterator->second;

		if (ti.IsVALNumNULL())
		{
			foundMatchingTicket = true;
		}
	}

	m_criticalSection.unlock();

	if(foundMatchingTicket)
	{
		isTicketOut = ti.IsTicketOut();
		dwUniqueID = (ti.GetUniqueId());
	}

	return foundMatchingTicket;
}
byte * CTransactionResendList::GetFirstItem(WORD &transLength, DWORD &outUniqueId, DWORD &outLastSent, bool &alreadySuspicious)
{
	byte *retVal = NULL;

	CTransactionItem ti;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	SizeReturn ret = m_resendList.size();
	ResendListMap::iterator iterator;

	if (ret > 0)
	{
		// Get first
		iterator = m_resendList.begin();
		ti = iterator->second;
		transLength = ti.GetTransLength();
		retVal = ti.GetTransPtr();
		outUniqueId = ti.GetUniqueId();
		outLastSent = ti.GetLastSent();
		alreadySuspicious = ti.IsSuspicious();
	}
	else
	{
		transLength = 0;
	}

	return retVal;
}
byte * CTransactionResendList::GetNextItem(WORD &transLength, DWORD &currentUniqueId, DWORD &currentLastSent, bool &alreadySuspicious)
{
	// find next item to resend, return NULL if nothing exists

	byte *retVal = NULL;
	transLength = 0;

	CTransactionItem ti;
	ResendListMap::iterator iterator;

	lock_guard<recursive_mutex> lock(m_criticalSection);

	SizeReturn ret = m_resendList.size();
	if (ret > 0)
	{
		//get the current
		iterator = m_resendList.find(currentUniqueId);
		//check that it's not beyond the end
		if (iterator != m_resendList.end())
		{
			//increment it
			iterator++;
			//check again that we're not beyond the end
			if (iterator != m_resendList.end())
			{
				ti = iterator->second;
				transLength = ti.GetTransLength();
				retVal = ti.GetTransPtr();
				currentUniqueId = ti.GetUniqueId();
				currentLastSent = ti.GetLastSent();
				alreadySuspicious = ti.IsSuspicious();
			}
		}
	}

	return retVal;
}

WORD CTransactionResendList::GetPBSequenceNum(DWORD uniqueId)
{
	CTransactionItem ti;
	WORD retVal = 0;

	lock_guard<recursive_mutex> lock(m_criticalSection);
	SizeReturn ret = m_resendList.count(uniqueId);

	if (ret > 0)
	{
		ti = m_resendList[uniqueId];
		//the sequence number is the last two bytes of the PB transaction
		memcpy(&retVal, ti.GetTransPtr() + ti.GetTransLength() - sizeof(WORD), sizeof(WORD));
	}

	return retVal;

}

// CTransactionItem member functions
CTransactionItem::CTransactionItem()
	:
	m_msgType(0),
	m_uniqueId(0),
	m_transPtr(nullptr),
	m_transLength(0),
	m_lastSent(0),
	m_nvramOffset(0),
	m_isTicketOut(false),
	m_isSuspicious(false)
{
}

byte CTransactionItem::GetMsgType()
{
	return m_msgType;
}

CTransactionItem::CTransactionItem(byte msgType, DWORD uniqueId, byte * transPtr, DWORD nvOffset)
	:
	m_transLength(0),
	m_lastSent(0),
	m_nvramOffset(0),
	m_isTicketOut(false),
	m_isSuspicious(false)
{
	m_msgType = msgType;
	m_uniqueId = uniqueId;
	m_transPtr = transPtr;
	m_nvramOffset = nvOffset;
}

void CTransactionItem::SetMsgType(byte msgType)
{
	m_msgType = msgType;
}

DWORD CTransactionItem::GetUniqueId()
{
	return m_uniqueId;
}

void CTransactionItem::SetUniqueId(DWORD uniqueId)
{
	m_uniqueId = uniqueId;
}

DWORD CTransactionItem::GetLastSent()
{
	return m_lastSent;
}

void CTransactionItem::SetLastSent()
{
	m_lastSent = CUtilities::GetTickCount();
}

// We call this when loading the resend list from nvram so that items on the resend list are sent immediately
// so that they do not get out of order (if PMT is not up then they are set in the resend list correctly
void CTransactionItem::ResetLastSent()
{
	m_lastSent = 0;
}

byte * CTransactionItem::GetTransPtr()
{
	return m_transPtr;
}

void CTransactionItem::SetTransPtr(byte * transPtr)
{
	m_transPtr = transPtr;
}

WORD CTransactionItem::GetTransLength()
{
	return m_transLength;
}

void CTransactionItem::SetTransLength(WORD transLength)
{
	m_transLength = transLength;
}

DWORD CTransactionItem::GetNvramOffset()
{
	return m_nvramOffset;
}

void CTransactionItem::SetNvramOffset(DWORD nvramOffset)
{
	m_nvramOffset = nvramOffset;
}

void CTransactionItem::SetAsTicketOut(bool isTicketOut)
{
	m_isTicketOut = isTicketOut;
}

bool CTransactionItem::IsTicketOut()
{
	return m_isTicketOut;
}

bool CTransactionItem::operator ==(CTicket &rhs) 
{
	//this is to find a ticket from within the transaction resend list.

	if (this->GetMsgType() != TICKET_CODE)
		return false;

	bool retVal = false;
	byte * buffer;
	byte * validationLength;
	byte valLength;
	byte * validationNumber;
	const byte * rhsValidationNumber;
	byte length;
	SYSTEMTIME * gameDateTime;
	SYSTEMTIME sysTime;
	PollerMeter tempMeter;
	UNREFERENCED_PARAMETER(&tempMeter); // Variable is not used, just its member sizes.

	if( (this->GetMsgType() == TICKET_CODE) )
	{
		buffer = this->GetTransPtr();
		byte *meterCount = buffer + SIZE_OF_MESSAGE_HEADER + NONVARIABLE_TRANSACTION_SIZE;

		validationLength = meterCount +									//this is an address
			(*meterCount * (sizeof(tempMeter.Number) + sizeof(tempMeter.Value))) + //the meters
			sizeof(byte) +									//the data count
			sizeof(byte) + sizeof(byte) + sizeof(int) +	// Ticket Number
			sizeof(byte) + sizeof(byte);

		//the pointers above are not aligned, so we need to copy the data.
		memcpy(&valLength, validationLength, sizeof(valLength));

		validationNumber = validationLength + sizeof(byte);

		gameDateTime = (SYSTEMTIME*)(validationNumber + valLength + 
			sizeof(byte) + sizeof(byte) + sizeof(int) +			//machine number
			sizeof(byte) + sizeof(byte) + sizeof(byte) +		//expiration type
			sizeof(byte) + sizeof(byte) + sizeof(byte) +		//expiration duration
			sizeof(byte) + sizeof(byte) + sizeof(WORD) +		//ticket sequence number
			sizeof(byte) + sizeof(byte) + sizeof(byte) +		//action code
			sizeof(byte) + sizeof(byte) + sizeof(byte) +		//ticket type
			sizeof(byte) + sizeof(byte) + sizeof(__int64) +		//amount in pennies
			sizeof(byte) + sizeof(byte));

		//the pointers above are not aligned, so we need to copy the data.
		memcpy(&sysTime, gameDateTime, sizeof(SYSTEMTIME));

		if( rhs.GameDateTime() == CUtilities::GetTimeFromUTCSystemTime(sysTime))
		{
			rhsValidationNumber = rhs.ValidationNumber(length);

			for(int i = 0; i < valLength; i++)
			{
				if(validationNumber[i] != rhsValidationNumber[i])
				{
					retVal = false;
					break;
				}
				retVal = true;
			}
		}
	}
	return retVal;
}

bool CTransactionItem::IsVALNumNULL() 
{
	//this is to find a ticket from within the transaction resend list.

	if (this->GetMsgType() != TICKET_CODE)
		return false;

	bool retVal = false;
	byte * buffer;
	byte * validationLength;
	byte valLength;
	byte * validationNumber;
	PollerMeter tempMeter;
	UNREFERENCED_PARAMETER(&tempMeter); // Variable is not used, just its member sizes.

	if( (this->GetMsgType() == TICKET_CODE) )
	{
		buffer = this->GetTransPtr();
		byte *meterCount = buffer + SIZE_OF_MESSAGE_HEADER + NONVARIABLE_TRANSACTION_SIZE;

		validationLength = meterCount +									//this is an address
			(*meterCount * (sizeof(tempMeter.Number) + sizeof(tempMeter.Value))) + //the meters
			sizeof(byte) +									//the data count
			sizeof(byte) + sizeof(byte) + sizeof(int) +	// Ticket Number
			sizeof(byte) + sizeof(byte);

		//the pointers above are not aligned, so we need to copy the data.
		memcpy(&valLength, validationLength, sizeof(valLength));

		validationNumber = validationLength + sizeof(byte);

		if(valLength == 0)
			retVal = true;
		else if(memcmp( validationNumber, L"D0D0D0D0D0D0D0D0D0", valLength ))
			retVal = true;
	}
	return retVal;
}

void CTransactionItem::SetSuspicious()
{
	m_isSuspicious = true;
}

bool CTransactionItem::IsSuspicious() const
{
	return m_isSuspicious;
}

bool  CTransactionItem::IsMessagePMTDependent(BYTE* resendItem)
{
	WORD packetVersion;
	byte transactionCode;
	bool bPmtDependent(true);

	//Find the transaction code and packet version  in message
	memcpy(&transactionCode, resendItem + SIZE_OF_MESSAGE_HEADER, sizeof(byte));
	memcpy(&packetVersion, resendItem + PACKET_VERSION_OFFSET, sizeof(WORD));

	if ((transactionCode == TICKET_CODE) && (packetVersion >= PACKET_VERSION_VOUCHER_SEPARATION))
	{
		bPmtDependent = false;
	}

	return(bPmtDependent);
}
