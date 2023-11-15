

#include "gmock/gmock.h"
#include "PollerProtocol/PollerProtocol.h"

using namespace std;

CLogs::CLogs(MEMORY_TYPE memoryType, LogType type, const std::string& topic)
: CDataSubscriptionProvider(topic)
{
}

CLogs::CLogs(CLogs &lRHS) : CDataSubscriptionProvider("")
{
	*this = lRHS;
}

CLogs & CLogs::operator=( CLogs &lRHS )
{
	return *this;
}

CLogs::~CLogs()
{
}

std::string CLogs::GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId)
{
	return "";
}

void CLogs::ClearAll()
{
}

bool CLogs::DoesLogExistInNVRAM( void )
{
	return false;
}

void CLogs::SetLogExistanceInNVRAM( void )
{
}

void CLogs::BuildYourself()
{
}

void CLogs::BuildLog()
{
}

void CLogs::Add( CLog *log )
{
}

CLog *CLogs::GetFirst( )
{
	return NULL;
}

CLog *CLogs::GetNext( )
{
	return NULL;
}

CLog *CLogs::GetLastLog( )
{
	return NULL;
}

CLog *CLogs::GetPreviousLog( )
{
	return NULL;
}

CLog *CLogs::GetFirstUnsorted( )
{
	return NULL;
}

CLog *CLogs::GetNextUnsorted( )
{
	return NULL;
}

void CLogs::AddLogToNvram( bool updateNVRamIndex )
{
}

void CLogs::UpdateLastLog( CLog *log )
{
}

void CLogs::SetMaxLogs(int maxLogs)
{
}

int CLogs::GetMaxLogs( void ) const
{
	return 0;
}

int CLogs::GetIndex( void ) const
{
	return 0;
}


