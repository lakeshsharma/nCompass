#include "ScheduleEndCommand.h"

using namespace std;

CScheduleEndCommand::CScheduleEndCommand(DWORD displayDuration,
	const std::string& dynamicContentPath,
	const std::string& levelName) :
	CFullScreenAttractorCommand(displayDuration, "ScheduleEnd", dynamicContentPath), m_levelName(levelName)
{
}


CScheduleEndCommand::~CScheduleEndCommand()
{
}

void CScheduleEndCommand::AddDynamicTags(JsonWriter& writer) const
{
	writer.Key("dynamicTags");
	writer.StartArray();

	writer.StartObject();
	writer.Key("name");		writer.String("LevelName");
	writer.Key("value");	writer.String(m_levelName);
	writer.EndObject();

	writer.EndArray();
}