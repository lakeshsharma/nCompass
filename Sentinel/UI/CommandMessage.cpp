#include "CommandMessage.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"

const std::string CMD_MSG = "CommandMessage";

CCommandMessage::CCommandMessage(const std::string& cmdName, DWORD displayDuration, const std::string& dynamicContentPath) :
	m_cmdName(cmdName), m_displayDuration(displayDuration), m_dynamicContentPath(dynamicContentPath)
{
}

CCommandMessage::~CCommandMessage()
{
}

void CCommandMessage::AddStringArg(const std::string& argName, const std::string& argValue)
{
	m_cmdStringArgs.insert({ argName, argValue });
}

std::string CCommandMessage::getJSON() const
{
	JsonWriteStreamString writeStream;
	JsonWriter writer(writeStream);

	writer.StartObject();
	writer.Key("type");				writer.String(CMD_MSG);
	writer.Key("commandName");		writer.String(m_cmdName);
	writer.Key("commandArgs");
	{
		writer.StartObject();
		writer.Key("displayDuration");		writer.Int64(m_displayDuration);

		for (const auto& keyValue : m_cmdStringArgs)
		{
			writer.Key(keyValue.first.c_str());
			writer.String(keyValue.second.c_str());
		}

		if (!m_dynamicContentPath.empty())
		{
			writer.Key("dynamicContentPath");	writer.String(m_dynamicContentPath);
		}

		addCustomArgs(writer);

		writer.EndObject();
	}
	writer.EndObject();

	return writeStream.m_str;
}
