#pragma once

#include "../Hardware/MemoryStorage.h"
#include "ProgressiveKeys.h"

typedef enum
{
	NONE = 0,
    COININ5,
    GAMEEND,
    PROGRESSIVELEVELHIT,
	PROGRESSIVE_TRANSACTION_CODE_MAXIMUM = PROGRESSIVELEVELHIT, // Set this to the highest valid enumeration, or more.
} ProgressiveTransactionType;

class CProgressiveTransactionItem
{
public:
	CProgressiveTransactionItem();
	CProgressiveTransactionItem(ToOneLink::Type msgType, DWORD uniqueId, const std::string &json);
	CProgressiveTransactionItem(const std::string &filename);
	CProgressiveTransactionItem & operator=(const CProgressiveTransactionItem &itemRHS);

	bool IsMessageTypeValid(int msgType) const;

private:
	ToOneLink::Type m_msgType;
	DWORD m_uniqueId;
    std::string m_json;
	DWORD m_lastSent;
	bool m_isPersisted;
	std::string m_filename;

public:
	ToOneLink::Type GetMsgType();
	DWORD GetUniqueId();
	void SetUniqueId(DWORD uniqueId);
	DWORD GetLastSent();
	void SetLastSent();
	void ResetLastSent();
	bool IsPersisted() const;
	void Persist();
	void RemoveFile();
	void setJson(const std::string &json);
	std::string GetJson();
};

// key for CMap will be UniqueId of the transaction
typedef std::map<DWORD, CProgressiveTransactionItem> ProgressiveResendListMap;
typedef std::pair<DWORD, CProgressiveTransactionItem> ProgressiveResendListPair;
typedef std::pair<ProgressiveResendListMap::iterator, bool> ProgessiveInsertReturn;
typedef std::map<int, int>::size_type SizeReturn;

class CProgressiveTransactionResendList : public CMemoryStorage
{
public:
	CProgressiveTransactionResendList(bool fullReset, MEMORY_TYPE type = MEMORY_SDCARD);
	~CProgressiveTransactionResendList(void);

	static void DeleteAllResendListFiles();

	bool AddTransaction(ToOneLink::Type msgType, DWORD uniqueId, const std::string& json);
	bool GetFirstItem(CProgressiveTransactionItem &ti, DWORD &outUniqueId, DWORD &outLastSent, bool &alreadySuspicious);

	bool RemoveTransaction(DWORD uniqueId);
	bool SetLastSent(DWORD uniqueId);
	ProgressiveTransactionType GetTransactionType(DWORD uniqueId);
	BOOL IsEmpty();
	int GetPercentageFull();
	DWORD GetHighestUniqueId();

	void Lock()
	{
		m_criticalSection.lock();
	}
	void Unlock()
	{
		m_criticalSection.unlock();
	}

private:
	ProgressiveResendListMap m_resendList;
	bool m_sdcardValid;
	int m_currentItemIndex;
	std::recursive_mutex m_criticalSection;

	void BuildYourself();
	void CheckResendDirectory();
};

