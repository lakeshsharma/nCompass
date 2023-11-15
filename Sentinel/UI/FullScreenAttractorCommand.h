#pragma once

#include <string>
#include "GlobalDefs.h"
#include "CommandMessage.h"

class CFullScreenAttractorCommand : public CCommandMessage
{
public:
	CFullScreenAttractorCommand(DWORD displayDuration, const std::string& assetTag, const std::string& dynamicContentPath);
	virtual ~CFullScreenAttractorCommand();

	virtual void addCustomArgs(JsonWriter &writer) const;
	virtual void AddDynamicTags(JsonWriter& writer) const {};

protected:
	std::string m_assetTag;

private:
	DWORD m_displayDuration;
};