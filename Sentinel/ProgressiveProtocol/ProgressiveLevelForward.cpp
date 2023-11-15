#include "stdafx.h"
#include "ProgressiveLevelInfo.h"
#include "ProgressiveLevelForward.h"

CProgressiveLevelForward::Level::Level(BYTE sasLevel,
	__int64 amount,
	ProgressiveLevelType::Type type,
	DWORD updatedTickCount) :
	amount(amount),
	updatedTickCount(updatedTickCount),
	type(type),
	sasLevel(sasLevel),
	levelId(0),
	eligible(true),
	showOdometer(true),
	isPte(false),
	isPrize(false),
	isPlayXGetY(false),
	isScatter(false),
	m_countdownStartTime(INVALID_VALUE),
	m_countdownEndTime(INVALID_VALUE)
{
}

CProgressiveLevelForward::Level::Level(__int64 levelId,
	LPCSTR levelName,
	bool eligible,
	bool showOdometer,
	bool isPte,
	bool isPrize,
	bool isPlayXGetY,
	bool isScatter,
	LPCSTR prizeDescription,
	int64_t countdownStartTime,
	int64_t countdownEndTime,
	__int64 amount,
	ProgressiveLevelType::Type type,
	DWORD updatedTickCount) :
	amount(amount),
	updatedTickCount(updatedTickCount),
	type(type),
	sasLevel(0),
	levelId(levelId),
	levelName(levelName),
	eligible(eligible),
	showOdometer(showOdometer),
	isPte(isPte),
	isPrize(isPrize),
	isPlayXGetY(isPlayXGetY),
	isScatter(isScatter),
	prizeDescription(prizeDescription),
	m_countdownStartTime(countdownStartTime),
	m_countdownEndTime(countdownEndTime)
{
}

CProgressiveLevelForward::CProgressiveLevelForward(int progressiveGroupId, bool useAftLockOnTransfers) :
	m_progressiveGroupId(progressiveGroupId),
	m_useAftLockOnTransfers(useAftLockOnTransfers)
{
}

CProgressiveLevelForward::~CProgressiveLevelForward()
{
}

CProgressiveLevelForward::CProgressiveLevelForward(const CProgressiveLevelForward &rhs)
{
	*this = rhs;
}

CProgressiveLevelForward& CProgressiveLevelForward::operator=(const CProgressiveLevelForward &rhs)
{
	if (this != &rhs)
	{
		m_levels = rhs.m_levels;
		m_progressiveGroupId = rhs.m_progressiveGroupId;
		m_useAftLockOnTransfers = rhs.m_useAftLockOnTransfers;
	}
	return *this;
}

int CProgressiveLevelForward::GetProgressiveGroupId() const
{
	return m_progressiveGroupId;
}

bool CProgressiveLevelForward::UseAftLockOnTransfers() const
{
	return m_useAftLockOnTransfers;
}

void CProgressiveLevelForward::Clear()
{
	m_levels.clear();
}

void CProgressiveLevelForward::AddLevel(BYTE sasLevel, __int64 amount, ProgressiveLevelType::Type type, DWORD updatedTickCount)
{
	if (sasLevel < MaxSasLevels)
		m_levels.push_back(Level(sasLevel, amount, type, updatedTickCount));
}

void CProgressiveLevelForward::AddLevel(__int64 levelId,
	LPCSTR levelName,
	bool eligible,
	bool showOdometer,
	bool isPte,
	bool isPrize,
	bool isPlayXGetY,
	bool isScatter,
	LPCSTR prizeDescription,
	int64_t countdownStartTime,
	int64_t countdownEndTime,
	__int64 amount,
	ProgressiveLevelType::Type type,
	DWORD updatedTickCount)
{
	m_levels.push_back(Level(levelId, levelName, eligible, showOdometer, isPte, isPrize, isPlayXGetY, isScatter, prizeDescription, countdownStartTime, countdownEndTime, amount, type, updatedTickCount));
}

size_t CProgressiveLevelForward::GetNumLevels() const
{
	return m_levels.size();
}

std::vector<CProgressiveLevelForward::Level>::const_iterator CProgressiveLevelForward::cbegin() const
{ 
	return m_levels.cbegin();
}

std::vector<CProgressiveLevelForward::Level>::const_iterator CProgressiveLevelForward::cend() const
{ 
	return m_levels.cend();
}

std::vector<CProgressiveLevelForward::Level>::const_iterator CProgressiveLevelForward::EraseLevel(
	std::vector<CProgressiveLevelForward::Level>::const_iterator it)
{
	return m_levels.erase(it);
}