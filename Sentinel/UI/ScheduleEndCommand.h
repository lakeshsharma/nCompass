#pragma once

#include <string>
#include "GlobalDefs.h"
#include "FullScreenAttractorCommand.h"

class CScheduleEndCommand : public CFullScreenAttractorCommand
{
public:
	CScheduleEndCommand(DWORD displayDuration,
		const std::string& dynamicContentPath,
		const std::string& levelName);
	virtual ~CScheduleEndCommand();

	virtual void AddDynamicTags(JsonWriter& writer) const;

private:
	std::string m_levelName;
};