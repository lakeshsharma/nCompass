#pragma once

#include "../Hardware/MemoryStorage.h"
#include "PollerMessage.h"

class CTransactionItem
{
public:
	CTransactionItem();
	CTransactionItem(byte msgType, DWORD uniqueId, byte * transPtr, DWORD nvBuffer);

private:
	byte m_msgType;
	DWORD m_uniqueId;
	byte * m_transPtr;
	WORD m_transLength;
	DWORD m_lastSent;
	DWORD m_nvramOffset;
	bool m_isTicketOut;
	bool m_isSuspicious;

public:
	byte GetMsgType();
	void SetMsgType(byte msgType);
	DWORD GetUniqueId();
	void SetUniqueId(DWORD uniqueId);
	byte * GetTransPtr();
	void SetTransPtr(byte * transPtr);
	WORD GetTransLength(); 
	void SetTransLength(WORD transLength);
	DWORD GetLastSent();
	void SetLastSent();
	void ResetLastSent();
	DWORD GetNvramOffset();
	void SetNvramOffset(DWORD nvramOffset);
	void SetAsTicketOut(bool isTicketOut);
	bool IsTicketOut();
	bool operator==( CTicket & );
	bool IsVALNumNULL();

	/// <summary>
	/// Sets the suspicious flag true, indicating that the content of the transaction item has already been logged as suspicious.
	/// </summary>
	void SetSuspicious();

	/// <summary>
	/// Returns true if the transaction item has already been logged as suspicioius, false otherwise.
	/// </summary>
	/// <returns>true if the transaction item has already been logged as suspicioius, false otherwise.</returns>
	bool IsSuspicious() const;

    /// <summary>
    /// Returns true if the transaction
    /// item is  PMT service dependent, false otherwise. 
	/// </summary>
    /// <returns>true if the transaction item 
    /// PMT service dependent.</returns> 
    static bool IsMessagePMTDependent(BYTE* resendItem);

};

// key for CMap will be UniqueId of the transaction
typedef std::map<DWORD, CTransactionItem> ResendListMap;
typedef std::pair<DWORD, CTransactionItem> ResendListPair;
typedef std::pair<ResendListMap::iterator, bool> InsertReturn;
typedef std::map<int, int>::size_type SizeReturn;
typedef std::list<DWORD> FreeNvRamList;

class CTransactionResendList : public CMemoryStorage
{
public:
	CTransactionResendList(bool fullReset, MEMORY_TYPE type = MEMORY_NVRAM);
public:
	~CTransactionResendList(void);

	bool AddTransaction(byte msgType, DWORD uniqueId, byte *transPtr, WORD transLength, bool storeInNVRAM, bool isTicketOut);
	byte *GetFirstItem(WORD &transLength, DWORD &outUniqueId, DWORD &outLastSent, bool &alreadySuspicious);
	byte *GetNextItem(WORD &transLength, DWORD &currentUniqueId, DWORD &currentLastSent, bool &alreadySuspicious);

	bool RemoveTransaction(DWORD uniqueId, bool isStoredInNVRAM);
	bool SetLastSent(DWORD uniqueId);
	TransactionType GetTransactionType(DWORD uniqueId);
	bool RemoveChangeSlotMastIDTransaction(DWORD SlotMastID);
	bool GetTicketTransaction(CTicket * ticket, bool &isTicketOut, DWORD &dwUniqueID);
	bool GetNULLValidationNumberTicket(DWORD &dwUniqueID, bool &isTicketOut);
	WORD GetPBSequenceNum(DWORD uniqueID);
	int GetInitialTicketOutCount();
	BOOL IsEmpty();
	int GetPercentageFull();
	DWORD GetHighestUniqueId();

	void SetSuspicious(DWORD uniqueId);

	void Lock()
	{
		m_criticalSection.lock();
	}
	void Unlock()
	{
		m_criticalSection.unlock();
	}

private:
	ResendListMap m_resendList;
	FreeNvRamList m_freeNvList;
	bool m_nvramValid;
	int m_currentItemIndex;
	std::recursive_mutex m_criticalSection;
	int m_initialTicketOutCount;


	void BuildYourself();
};

