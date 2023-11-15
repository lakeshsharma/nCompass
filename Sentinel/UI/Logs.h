#pragma once
#include "../Hardware/MemoryStorage.h"
#include "Log.h"
#include "../GlobalDefs.h"
#include "InternalMsgContent.h"
#include "DataSubscriptionProvider.h"

const std::string ATTENDANT_PAID_LOG = "AttendantPaidLog";
const std::string PB_PENDING_LOGS = "PbPendingLogs";
const std::string PB_TRANSACTION_LOGS = "PbTransactionLogs";
const std::string PB_LOGS = "PbLogs";
const std::string REPAIR_LOG = "RepairLog";
const std::string TICKET_LOGS = "TicketLogs";
const std::string TILT_LOGS = "TiltLogs";
const std::string TRANSACTION_LOG = "TransactionLog";

class CLogs : public CDataSubscriptionProvider,
			  public CMemoryStorage, 
			  public CInternalMsgContent<INT_MSG_CONTENT_LOGS>
{
public:
	CLogs(MEMORY_TYPE memoryType, LogType type, const std::string& topic);
	CLogs( CLogs &lRHS );
	virtual ~CLogs();
	CLogs &operator=( CLogs &lRHS );

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	void ClearAll();

	bool DoesLogExistInNVRAM( void );
	void SetLogExistanceInNVRAM( void );
	virtual void BuildYourself();
	void BuildLog();

	void Add( CLog *log );
	CLog *GetFirst( );
	CLog *GetNext( );
	CLog *GetLastLog( );
	CLog *GetPreviousLog( );
	CLog *GetFirstUnsorted( );
	CLog *GetNextUnsorted( );
	void AddLogToNvram( bool updateNVRamIndex = true );
	void UpdateLastLog( CLog *log );

	/// <summary>
	/// Returns the maximum number of log records.
	/// </summary>
	/// <returns>The maximum number of log records.</returns>
	int GetMaxLogs( void ) const;

	/// <summary>
	/// Sets the maximum logs.
	/// </summary>
	/// <param name="maxLogs">The maximum logs.</param>
	void SetMaxLogs(int maxLogs);

	int GetNumItems( void )
	{
		return m_count;
	}

	LogType GetLogType( void )
	{
		return (m_logType);
	}

	void SetLogType( LogType logType )
	{
		m_logType = logType;
	}

	bool IsLogFull( void )
	{
		return (m_isFull);
	}

	/// <summary>
	/// Returns the index.
	/// </summary>
	/// <returns>The index.</returns>
	int GetIndex( void ) const;

	DWORD AllowedSize( void )
	{
		return (m_dwAllowedSize);
	}

	DWORD Offset( void )
	{
		return (m_dwOffset);
	}

	DWORD IndexOffset( void )
	{
		return (m_indexOffset);
	}

	DWORD IsLogFullOffset( void )
	{
		return (m_isLogFullOffset);
	}
protected:	

private:
	std::vector<CLog*> m_logArray;
	int m_maxLogs;
	int m_index;
	int m_retIndex; //used for first, next, last, and previous functions
	int m_count;
	LogType m_logType;
	bool m_isFull;	
	DWORD m_indexOffset;
	DWORD m_isLogFullOffset;
	std::recursive_mutex m_logCriticalSection;

	std::string ResponseForAttendantPaidLog();
	std::string ResponseForRepairLog();	
	std::string ResponseForTransactionLog();
};