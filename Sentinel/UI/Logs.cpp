#include "stdafx.h"

#include "Logs.h"
#include "../Sentinel.h"
#include "TicketLog.h"
#include "TransactionLog.h"
#include "MediaWindowLog.h"
#include "PersonalBankerLog.h"
#include "RepairLog.h"
#include "TiltLog.h"
#include "AttendantPaidPayoutLog.h"
#include "Logging/LogString.h"

using namespace std;

CLogs::CLogs(MEMORY_TYPE memoryType, LogType type, const string& topic) : CDataSubscriptionProvider(topic)
{	
	m_type = memoryType;
	m_logType = type;
	m_maxLogs = 0;
	m_count = 0;
	m_index = 0;
	m_retIndex = 0;	
	
	//set up items for nvram
	if ( memoryType == MEMORY_NVRAM )
	{
		switch (type)
		{
			case LOG_TICKET:
				m_dwAllowedSize = NVRAM_TICKET_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_TICKET_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_TICKET_LOGS;
				m_indexOffset = NVRAM_TICKET_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_TICKET_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_TRANS:
				m_dwAllowedSize = NVRAM_TRANSACTION_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_TRANSACTION_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_TRANSACTION_LOGS;
				m_indexOffset = NVRAM_TRANSACTION_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_TRANSACTION_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_PBT_LOG:
				m_dwAllowedSize = NVRAM_PBT_LOG_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_PBT_LOG_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_PBT_LOG_LOGS;
				m_indexOffset = NVRAM_PBT_LOG_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_PBT_LOG_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_PBT_EVENTS:
				m_dwAllowedSize = NVRAM_PBT_LOG_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_PBT_EVENTS_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_PBT_EVENTS_LOGS;
				m_indexOffset = NVRAM_PBT_EVENTS_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_PBT_EVENTS_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_FAILURE:
				m_dwAllowedSize = NVRAM_REPAIR_CODES_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_REPAIR_CODES_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_REPAIR_CODES_LOGS;
				m_indexOffset = NVRAM_REPAIR_CODES_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_REPAIR_CODES_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_TILT:
				m_dwAllowedSize = NVRAM_COMPOSITE_TILTS_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_COMPOSITE_TILTS_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_COMPOSITE_TILTS_LOGS;
				m_indexOffset = NVRAM_COMPOSITE_TILTS_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_COMPOSITE_TILTS_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_ATT_PAID_PAYOUT:
				m_dwAllowedSize = NVRAM_ATT_PAID_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_ATT_PAID_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_ATT_PAID_LOGS;
				m_indexOffset = NVRAM_ATT_PAID_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_ATT_PAID_LOGS_IS_LOG_FULL_OFFSET;
				break;
			case LOG_MEDIA_WINDOW:
				m_dwAllowedSize = NVRAM_MEDIA_WINDOW_CODES_LOG_DATA_SIZE;
				m_dwOffset = NVRAM_MEDIA_WINDOW_CODES_LOGS_DATA_OFFSET;
				m_maxLogs = NVRAM_MAX_MEDIA_WINDOW_CODES_LOGS;
				m_indexOffset = NVRAM_MEDIA_WINDOW_CODES_LOGS_INDEX_OFFSET;
				m_isLogFullOffset = NVRAM_MEDIA_WINDOW_CODES_LOGS_IS_LOG_FULL_OFFSET;
				break;
			default:
				break;
		}
		m_bOffsetIsSet = true;

		m_logArray.resize( m_maxLogs );
		m_isFull = false;

		BuildYourself();
	}	
}

CLogs::CLogs(CLogs &lRHS) : CDataSubscriptionProvider("")
{
	*this = lRHS;
}

CLogs & CLogs::operator=( CLogs &lRHS )
{
	ClearAll();

	SetLogType( lRHS.GetLogType() );
	SetMaxLogs( lRHS.GetMaxLogs() );
	m_index = lRHS.GetIndex();
	m_retIndex = 0;
	m_count = 0;
	m_isFull = lRHS.IsLogFull();

	m_dwAllowedSize = lRHS.AllowedSize();
	m_dwOffset = lRHS.Offset();
	m_indexOffset = lRHS.IndexOffset();
	m_isLogFullOffset = lRHS.IsLogFullOffset();

	m_logArray.resize( m_maxLogs );

	if( lRHS.GetNumItems() > 0 )
	{
		switch( m_logType )
		{
			case LOG_TICKET:
				{
					//Load our logArray with first log
					m_logArray.at(m_count++) = new CTicketLog(*(CTicketLog *)lRHS.GetFirstUnsorted());					

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						m_logArray.at(m_count) = new CTicketLog(*(CTicketLog *)lRHS.GetNextUnsorted());
					}				
				}
				break;
			case LOG_TRANS:
				{
					//Load our logArray with first log
					m_logArray.at(m_count++) = new CTransactionLog(*(CTransactionLog *)lRHS.GetFirstUnsorted());

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						m_logArray.at(m_count) = new CTransactionLog(*(CTransactionLog *)lRHS.GetNextUnsorted());
					}
				}
				break;
			case LOG_PBT_LOG:
			case LOG_PBT_EVENTS:
				{
					//Load our logArray with first log
					m_logArray.at(m_count++) = new CPersonalBankerLog(*(CPersonalBankerLog *)lRHS.GetFirstUnsorted());

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						m_logArray.at(m_count) = new CPersonalBankerLog(*(CPersonalBankerLog *)lRHS.GetNextUnsorted());
					}
				}
				break;
			case LOG_FAILURE:
				{
					//Load our logArray with first log
					CRepairLog *repairLog = new CRepairLog( *(CRepairLog *)lRHS.GetFirstUnsorted() );
					m_logArray.at(m_count++) = repairLog;

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						repairLog = new CRepairLog( *(CRepairLog *)lRHS.GetNextUnsorted() );
						m_logArray.at(m_count) = repairLog;
					}
				}
				break;
			case LOG_TILT:
				{
					//Load our logArray with first log
					CTiltLog *tiltLog = new CTiltLog( *(CTiltLog *)lRHS.GetFirstUnsorted() );
					m_logArray.at(m_count++) = tiltLog;

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						tiltLog = new CTiltLog( *(CTiltLog *)lRHS.GetNextUnsorted() );
						m_logArray.at(m_count) = tiltLog;
					}
				}
				break;
			case LOG_ATT_PAID_PAYOUT:
				{
					//Load our logArray with first log
					m_logArray.at(m_count++) = new CAttendantPaidPayoutLog(*(CAttendantPaidPayoutLog *)lRHS.GetFirstUnsorted());

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						m_logArray.at(m_count) = new CAttendantPaidPayoutLog(*(CAttendantPaidPayoutLog *)lRHS.GetNextUnsorted());
					}
				}
				break;
			case LOG_MEDIA_WINDOW:
				{
					//Load our logArray with first log
					m_logArray.at(m_count++) = new CMediaWindowLog(*(CMediaWindowLog *)lRHS.GetFirstUnsorted());

					for(; m_count < lRHS.GetNumItems(); m_count++)
					{
						//Load our logArray with next log
						m_logArray.at(m_count) = new CMediaWindowLog(*(CMediaWindowLog *)lRHS.GetNextUnsorted());
					}
				}
				break;
			default:
				break;
		}
	}
	return *this;
}

CLogs::~CLogs(void)
{
	ClearAll();
}

string CLogs::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	UNREFERENCED_PARAMETER(&whereId); 

	string dataResponse;

	if (ATTENDANT_PAID_LOG == topic)
	{
		dataResponse = ResponseForAttendantPaidLog();
	}
	else if (REPAIR_LOG == topic)
	{
		dataResponse = ResponseForRepairLog();
	}
	else if (TRANSACTION_LOG == topic)
	{
		dataResponse = ResponseForTransactionLog();
	}

	return dataResponse;
}

void CLogs::SetMaxLogs(int maxLogs)
{
	m_maxLogs = maxLogs;
	m_logArray.resize(m_maxLogs);
	m_isFull = (m_count >= m_maxLogs);
}

void CLogs::ClearAll()
{
	lock_guard<recursive_mutex> lock(m_logCriticalSection);
	try
	{
		if( !m_logArray.empty() )
		{
			for(int index = 0; index < ((int)m_logArray.size()); index++)
			{
				CLog *log = (CLog *)m_logArray.at( index );
				
				if( log != NULL )
				{
					delete log;
				}
			}

			m_logArray.clear();
		}

		m_count = 0;
	}
	catch(...)
	{
	}
}
void CLogs::BuildYourself()
{
	lock_guard<recursive_mutex> lock(m_logCriticalSection);
	try
	{
	if ( (m_type == MEMORY_NVRAM) )
	{
		if( DoesLogExistInNVRAM() )
		{
			BuildLog();
		}
		else
		{
			SetLogExistanceInNVRAM( );
		}				
	}
	}
	catch(...)
	{
	}
}

void CLogs::BuildLog()
{
	byte nvramBuffer[NVRAM_MAX_READ];
	byte *bufferPos = nvramBuffer;

	//read is log full from nvram
	if ( !Read( m_isLogFullOffset, (byte *)&m_isFull, sizeof(byte) ) )
	LogString(ERROR_LOG, "Could not read is log full from NVRAM" );
	
	//read index from nvram
	if ( !Read( m_indexOffset, (byte *)&m_index, sizeof(m_index) ) )
	LogString(ERROR_LOG, "Could not read index from NVRAM" );

	//if log is full read full log and parse
	if( m_isFull )
	{
		for( int index = 0; index < m_maxLogs; index++ )
		{
			if ( !Read( m_dwOffset + (m_dwAllowedSize * index), nvramBuffer, m_dwAllowedSize ) )
				LogString(ERROR_LOG, "Could not read log data from NVRAM" );

			bufferPos = nvramBuffer;
			switch ( m_logType )
			{
				case LOG_TICKET:
					{
						m_logArray.at(index) = new CTicketLog(bufferPos);
	  				}
					break;
				case LOG_TRANS:
					{
						m_logArray.at(index) = new CTransactionLog(bufferPos);
					}
					break;
				case LOG_PBT_LOG:
				case LOG_PBT_EVENTS:
					{
						m_logArray.at(index) = new CPersonalBankerLog( bufferPos);
					}
					break;
				case LOG_FAILURE:
					{
						m_logArray.at(index) = new CRepairLog(bufferPos);
					}
					break;
				case LOG_TILT:
					{
						m_logArray.at(index) = new CTiltLog( bufferPos);
					}
					break;
				case LOG_ATT_PAID_PAYOUT:
					{
						m_logArray.at(index) = new CAttendantPaidPayoutLog(bufferPos);
					}
					break;
				case LOG_MEDIA_WINDOW:
					{
						m_logArray.at(index) = new CMediaWindowLog(bufferPos);
					}
					break;
				default:
					break;
			}

		}
		m_count = m_maxLogs;
	}
	//else read from start to index
	else
	{
		for( int index = 0; index <= m_index; index++ )
		{
			if ( !Read( m_dwOffset + (m_dwAllowedSize * index), nvramBuffer, m_dwAllowedSize ) )
				LogString(ERROR_LOG, "Could not read log data from NVRAM" );

			bufferPos = nvramBuffer;
			switch ( m_logType )
			{
				case LOG_TICKET:
					{
						m_logArray.at(index) = new CTicketLog(bufferPos);
					}
					break;
				case LOG_TRANS:
					{
						m_logArray.at(index) = new CTransactionLog(bufferPos);
					}
					break;
				case LOG_PBT_LOG:
				case LOG_PBT_EVENTS:
					{
						m_logArray.at(index) = new CPersonalBankerLog(bufferPos);
					}
					break;
				case LOG_FAILURE:
					{
						m_logArray.at(index) = new CRepairLog(bufferPos);
					}
					break;
				case LOG_TILT:
					{
						CTiltLog *Log = new CTiltLog( bufferPos);
						m_logArray.at(index) = Log;
					}
					break;
				case LOG_ATT_PAID_PAYOUT:
					{
						m_logArray.at(index) = new CAttendantPaidPayoutLog(bufferPos);
					}
					break;
				case LOG_MEDIA_WINDOW:
					{
						m_logArray.at(index) = new CMediaWindowLog(bufferPos);
					}
					break;
				default:
					break;
			}
			m_count++;
		}
	}
	m_index++;
}


bool CLogs::DoesLogExistInNVRAM(  )
{
	bool bExists = false;
	DWORD bOffset = 0;
	byte bCheckValue = 0;
		
	switch ( m_logType )
	{
		case LOG_TICKET:
			bOffset = NVRAM_TICKET_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_TICKET_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_TRANS:
			bOffset = NVRAM_TRANSACTION_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_TRANSACTION_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_PBT_LOG:
			bOffset = NVRAM_PBT_LOG_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_PBT_LOG_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_PBT_EVENTS:
			bOffset = NVRAM_PBT_EVENTS_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_PBT_EVENTS_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_FAILURE:
			bOffset = NVRAM_REPAIR_CODES_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_REPAIR_CODES_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_TILT:
			bOffset = NVRAM_COMPOSITE_TILTS_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_COMPOSITE_TILTS_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_ATT_PAID_PAYOUT:
			bOffset = NVRAM_ATT_PAID_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_ATT_PAID_LOGS_CHECKBYTE_VALUE;
			break;
		case LOG_MEDIA_WINDOW:
			bOffset = NVRAM_MEDIA_WINDOW_CODES_LOGS_CHECKBYTE_OFFSET;
			bCheckValue = NVRAM_MEDIA_WINDOW_CODES_LOGS_CHECKBYTE_VALUE;
			break;
		default:
			break;
	}
	
	bExists = CMemoryStorage::IntegrityCheck() && IsActiveLocation( bOffset, bCheckValue );

	if (!bExists)
	{
		SetInactiveLocation( bOffset, ~bCheckValue );
	}
	return bExists;
}

void CLogs::SetLogExistanceInNVRAM( void )
{
	DWORD bOffset = 0;
	byte logCheckByteValue;

	if ( m_type == MEMORY_NVRAM )
	{
		switch ( m_logType )
		{
			case LOG_TICKET:
				logCheckByteValue = m_count > 0 ? NVRAM_TICKET_LOGS_CHECKBYTE_VALUE : ~NVRAM_TICKET_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_TICKET_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_TRANS:
				logCheckByteValue = m_count > 0 ? NVRAM_TRANSACTION_LOGS_CHECKBYTE_VALUE : ~NVRAM_TRANSACTION_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_TRANSACTION_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_PBT_LOG:
				logCheckByteValue = m_count > 0 ? NVRAM_PBT_LOG_LOGS_CHECKBYTE_VALUE : ~NVRAM_PBT_LOG_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_PBT_LOG_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_PBT_EVENTS:
				logCheckByteValue = m_count > 0 ? NVRAM_PBT_EVENTS_LOGS_CHECKBYTE_VALUE : ~NVRAM_PBT_EVENTS_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_PBT_EVENTS_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_FAILURE:
				logCheckByteValue = m_count > 0 ? NVRAM_REPAIR_CODES_LOGS_CHECKBYTE_VALUE : ~NVRAM_REPAIR_CODES_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_REPAIR_CODES_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_TILT:
				logCheckByteValue = m_count > 0 ? NVRAM_COMPOSITE_TILTS_LOGS_CHECKBYTE_VALUE : ~NVRAM_COMPOSITE_TILTS_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_COMPOSITE_TILTS_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_ATT_PAID_PAYOUT:
				logCheckByteValue = m_count > 0 ? NVRAM_ATT_PAID_LOGS_CHECKBYTE_VALUE : ~NVRAM_ATT_PAID_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_ATT_PAID_LOGS_CHECKBYTE_OFFSET;
				break;
			case LOG_MEDIA_WINDOW:
				logCheckByteValue = m_count > 0 ? NVRAM_MEDIA_WINDOW_CODES_LOGS_CHECKBYTE_VALUE : ~NVRAM_MEDIA_WINDOW_CODES_LOGS_CHECKBYTE_VALUE;
				bOffset = NVRAM_MEDIA_WINDOW_CODES_LOGS_CHECKBYTE_OFFSET;
				break;
			default:
				break;
		}

		if ( !Write( bOffset, &logCheckByteValue, sizeof( logCheckByteValue ) ) )
			LogString(ERROR_LOG, "Could not write Log check byte to NVRAM" );
	}
}

void CLogs::Add( CLog *log )
{
	lock_guard<recursive_mutex> lock(m_logCriticalSection);
	try {
		//Add new log to our array at next index
		if( m_maxLogs != 0 )
		{
			if( m_index == m_maxLogs )//set index to top of array
			{
				m_index = 0;
				m_isFull = true;
			}

			if( m_isFull )
			{
				CLog *templog = (CLog *)m_logArray.at( m_index );

				if( templog != NULL )
					delete templog;
			}
			m_logArray.at(m_index) = log;
			
			//Send log to Nvram
			AddLogToNvram();

			m_index++;
		

			if( m_count < m_maxLogs )
			{
				m_count++;
			}

			SetLogExistanceInNVRAM( );
		}
	}
	catch(...)
	{
	}
}


CLog *CLogs::GetFirstUnsorted(  )
{
	CLog* log(nullptr);
	m_retIndex = 0;

	if( m_count > 0 )
		log = m_logArray.at( m_retIndex++ );

	return log;
}

CLog *CLogs::GetNextUnsorted( )
{
	CLog* log(nullptr);

	if( m_retIndex < m_maxLogs )
		log = m_logArray.at( m_retIndex++ );

	return log;
}

CLog *CLogs::GetFirst(  )
{
	CLog *log(nullptr);

	m_retIndex = m_index - 1;

	if (m_retIndex < 0)
	{
		m_retIndex = m_count - 1;
	}

	if (m_count > 0)
		log = m_logArray.at( m_retIndex-- );

	return log;
}

CLog *CLogs::GetNext( )
{
	CLog *log(nullptr);

	if( m_retIndex < 0 )
	{
		m_retIndex = m_count - 1;
	}

	log = m_logArray.at( m_retIndex-- );

	return log;
}

CLog *CLogs::GetLastLog( )
{
	CLog *log;

	//There is nothing in the log if this is true
	if( m_index == 0 && !m_isFull ) 
	{
		log = nullptr;
	}

	else
	{
		//set ret index to m_index minus one to get last log
		m_retIndex = m_index - 1;

		if( m_retIndex < 0 )
			m_retIndex = m_maxLogs - 1;

		log = (CLog *)m_logArray.at( m_retIndex );		
	}

	return log;
}

CLog *CLogs::GetPreviousLog( )
{
	CLog *log;

	//There is nothing in the log if this is true
	if( m_index == 0 && !m_isFull ) 
	{
		log = nullptr;
	}

	else
	{
		//decrament ret index to get next previous
		m_retIndex--;

		if( m_retIndex < 0 )
			m_retIndex = m_maxLogs - 1;

		log = (CLog *)m_logArray.at( m_retIndex );		
	}
	return log;
}


void CLogs::UpdateLastLog( CLog *log )
{
	lock_guard<recursive_mutex> lock(m_logCriticalSection);
	try
	{
		//use tempIndex to reuse later
		int tempIndex = m_index;

		//set m_index to retIndex to update last returned log
		m_index = m_retIndex;

		m_logArray.at(m_index) = log;

		//Send log to Nvram
		AddLogToNvram(/* log,*/ false );

		//reset m_index to previous value so we stay lined up
		m_index = tempIndex;
	}
	catch(...)
	{
	}
}

void CLogs::AddLogToNvram(bool updateNVRamIndex )
{
	if (m_type == MEMORY_NVRAM)
	{
		switch (m_logType)
		{
		case LOG_TICKET:
		{
			//Get our ticket at current index
			CTicketLog *ticketLog = (CTicketLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_TICKET_LOG_DATA_SIZE];

			int bytesInBuffer = ticketLog->GetNvramBuffer( nvramBuffer, m_dwAllowedSize );

			//Set log to nvram
			if ( !Write( m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write ticket log to NVRAM" );

			//Set whether log is full or not to nvram
			if ( !Write( m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE) ) )
				LogString(ERROR_LOG, "Could not write is log full to NVRAM" );

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if ( !Write( m_indexOffset, (BYTE *)&m_index, sizeof(m_index) ) )
					LogString(ERROR_LOG, "Could not write log index to NVRAM" );
			}
		}
		break;
		case LOG_TRANS:
		{
			//Get our transaction at current index
			CTransactionLog *transLog = (CTransactionLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_TRANSACTION_LOG_DATA_SIZE];

			int bytesInBuffer = transLog->GetNvramBuffer(nvramBuffer, m_dwAllowedSize);

            if (bytesInBuffer > (int)m_dwAllowedSize) 
                LogString(ERROR_LOG, "Error writing transaction to NVRAM: Transaction too long (%d) to fit in buffer (%d)", bytesInBuffer, m_dwAllowedSize);
            
			//Set log to nvram
			if (!Write(m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer))
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM");

			//Set whether log is full or not to nvram
			if (!Write(m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE)))
				LogString(ERROR_LOG, "Could not write is log full to NVRAM");

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if (!Write(m_indexOffset, (BYTE *)&m_index, sizeof(m_index)))
					LogString(ERROR_LOG, "Could not write log index to NVRAM");
			}
		}
		break;
		case LOG_PBT_LOG:
		case LOG_PBT_EVENTS:
		{
			//Get our PersonalBanker log at current index
			CPersonalBankerLog *PBLog = (CPersonalBankerLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_PBT_LOG_LOG_DATA_SIZE];

			int bytesInBuffer = PBLog->GetNvramBuffer(nvramBuffer, m_dwAllowedSize);

			//Set log to nvram
			if (!Write(m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer))
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM");

			//Set whether log is full or not to nvram
			if (!Write(m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE)))
				LogString(ERROR_LOG, "Could not write is log full to NVRAM");

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if (!Write(m_indexOffset, (BYTE *)&m_index, sizeof(m_index)))
					LogString(ERROR_LOG, "Could not write log index to NVRAM");
			}
		}
		break;
		case LOG_FAILURE:
		{
			//Get our repair log at current index
			CRepairLog *repairLog = (CRepairLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_REPAIR_CODES_LOG_DATA_SIZE];

			int bytesInBuffer = repairLog->GetNvramBuffer( nvramBuffer, m_dwAllowedSize );

			//Set log to nvram
			if ( !Write( m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM" );

			//Set whether log is full or not to nvram
			if ( !Write( m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE) ) )
				LogString(ERROR_LOG, "Could not write is log full to NVRAM" );

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if ( !Write( m_indexOffset, (BYTE *)&m_index, sizeof(m_index) ) )
					LogString(ERROR_LOG, "Could not write log index to NVRAM" );
			}	
		}
		break;
		case LOG_TILT:
		{
			//Get our tilt log at current index
			CTiltLog *tiltLog = (CTiltLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_COMPOSITE_TILTS_LOG_DATA_SIZE];

			int bytesInBuffer = tiltLog->GetNvramBuffer(nvramBuffer, m_dwAllowedSize);

			//Set log to nvram
			if (!Write(m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer))
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM");

			//Set whether log is full or not to nvram
			if (!Write(m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE)))
				LogString(ERROR_LOG, "Could not write is log full to NVRAM");

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if (!Write(m_indexOffset, (BYTE *)&m_index, sizeof(m_index)))
					LogString(ERROR_LOG, "Could not write log index to NVRAM");
			}
		}
		break;
		case LOG_ATT_PAID_PAYOUT:
		{
			//Get our tilt log at current index
			CAttendantPaidPayoutLog *Log = (CAttendantPaidPayoutLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_ATT_PAID_LOG_DATA_SIZE];

			int bytesInBuffer = Log->GetNvramBuffer( nvramBuffer, m_dwAllowedSize );

			//Set log to nvram
			if ( !Write( m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM" );

			//Set whether log is full or not to nvram
			if ( !Write( m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE) ) )
				LogString(ERROR_LOG, "Could not write is log full to NVRAM" );

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if ( !Write( m_indexOffset, (BYTE *)&m_index, sizeof(m_index) ) )
					LogString(ERROR_LOG, "Could not write log index to NVRAM" );					
			}
		}
		break;
		case LOG_MEDIA_WINDOW:
		{
			//Get our Media Window log at current index
			CMediaWindowLog *mediaWindowLog = (CMediaWindowLog *)m_logArray[m_index];

			byte nvramBuffer[NVRAM_MEDIA_WINDOW_CODES_LOG_DATA_SIZE];

			int bytesInBuffer = mediaWindowLog->GetNvramBuffer( nvramBuffer, m_dwAllowedSize );

			//Set log to nvram
			if ( !Write( m_dwOffset + (m_dwAllowedSize * m_index), nvramBuffer, bytesInBuffer ) )
				LogString(ERROR_LOG, "Could not write transaction log to NVRAM" );

			//Set whether log is full or not to nvram
			if ( !Write( m_isLogFullOffset, (BYTE *)&m_isFull, sizeof(BYTE) ) )
				LogString(ERROR_LOG, "Could not write is log full to NVRAM" );

			//Set index to nvram
			if (updateNVRamIndex)
			{
				if ( !Write( m_indexOffset, (BYTE *)&m_index, sizeof(m_index) ) )
					LogString(ERROR_LOG, "Could not write log index to NVRAM" );
			}				
		}
		break;
		default:
			break;
		}
	}
}

int CLogs::GetMaxLogs( void ) const
{
	return m_maxLogs;
}

int CLogs::GetIndex( void ) const
{
	return (m_index);
}

string CLogs::ResponseForAttendantPaidLog()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(TRANSACTION_LOG.c_str());
	{
		writer.StartArray();
		{
			int numLogs(GetNumItems());
			string amount, type;
			CAttendantPaidPayoutLog* log((CAttendantPaidPayoutLog*)GetFirst());
			for (int index = 0; index < numLogs; index++)
			{
				if (log)
				{
					writer.StartObject();
					{
						if (log->GetAmount() > 0)
						{
							amount = CUtilities::GetFormattedDollarsPenniesString((DWORD)log->GetAmount() /
								ONE_DOLLAR_IN_CENTS, (DWORD)log->GetAmount() % ONE_DOLLAR_IN_CENTS);
						}
						else
						{
							amount = "N/A";
						}

						switch (log->GetJackpotType())
						{
						case JTRANS_COMPOSITE:
							type = TXT_MECHANIC_HANDPAY;
							break;
						case JTRANS_PROGRESSIVE_HOPPER_PAID:
							type = TXT_MECHANIC_CC;
							break;
						case JTRANS_PROGRESSIVE_SCANNER_ONLY:
							type = TXT_MECHANIC_TOP_AWARD;
							break;
						default:
							type.clear();
							break;
						}

						writer.Key("TimeStamp"); writer.String(FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str()));
						writer.Key("Amount");	 writer.String(amount);
						writer.Key("Type");		 writer.String(type);
					}
					writer.EndObject();

					log = (CAttendantPaidPayoutLog *)GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

string CLogs::ResponseForRepairLog()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(REPAIR_LOG.c_str());
	{
		writer.StartArray();
		{
			int numLogs(GetNumItems());
			CRepairLog* log((CRepairLog*)GetFirst());
			for (int index = 0; index < numLogs; index++)
			{
				if (log)
				{
					writer.StartObject();
					{
						writer.Key("TimeStamp"); writer.String(FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str()));
						writer.Key("Code");		 writer.String(FormatString("%u", log->GetRepairCode()));
					}
					writer.EndObject();
					log = (CRepairLog *)GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}

string CLogs::ResponseForTransactionLog()
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key(TRANSACTION_LOG.c_str());
	{
		writer.StartArray();
		{
			int numLogs(GetNumItems());
			string amount;
			CTransactionLog* log((CTransactionLog *)GetFirst());
			for (int index = 0; index < numLogs; index++)
			{
				if (log != nullptr)
				{
					writer.StartObject();
					{
						if (log->GetAmount() > 0)
						{
							amount = CUtilities::GetFormattedDollarsPenniesString((DWORD)log->GetAmount() /
								ONE_DOLLAR_IN_CENTS, (DWORD)log->GetAmount() % ONE_DOLLAR_IN_CENTS);
						}
						else
						{
							amount = "N/A";
						}
						writer.Key("TimeStamp"); writer.String(FormatString("(%d) %s", index + 1, log->GetDateTimeAsString().c_str()));
						writer.Key("Amount");	 writer.String(amount);
						writer.Key("Type");		 writer.String(FormatString("%d", log->GetTransactionType()));
					}
					writer.EndObject();
					log = (CTransactionLog *)GetNext();
				}
				else
				{
					break;
				}
			}
		}
		writer.EndArray();
	}
	writer.EndObject();

	return writeStream.m_str;
}