#pragma once

#include <string>
#include "GlobalDefs.h"
#include "RapidJsonHelper.h"

extern const std::string CMD_MSG;

class CCommandMessage
{
public:
	CCommandMessage(const std::string& cmdName, DWORD displayDuration = 0, const std::string& dynamicContentPath = "");
	
	void AddStringArg(const std::string& argName, const std::string& argValue);

	virtual ~CCommandMessage();

	std::string getJSON() const;

protected:

	// virtual function so that a derived class can add args with any data type it want
	virtual void addCustomArgs(JsonWriter& writer) const {}

	std::string m_cmdName;
	DWORD m_displayDuration;
	std::string m_dynamicContentPath;
	std::map<std::string, std::string> m_cmdStringArgs;
};