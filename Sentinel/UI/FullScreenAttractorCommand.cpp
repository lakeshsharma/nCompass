#include "FullScreenAttractorCommand.h"

using namespace std;

CFullScreenAttractorCommand::CFullScreenAttractorCommand(DWORD displayDuration, const string& assetTag, const string& dynamicContentPath) :
	CCommandMessage("ShowFullScreenAttractor", displayDuration, dynamicContentPath), m_assetTag(assetTag)
{
}

CFullScreenAttractorCommand::~CFullScreenAttractorCommand()
{
}

void CFullScreenAttractorCommand::addCustomArgs(JsonWriter &writer) const
{
	writer.Key("assetTag");		writer.String(m_assetTag);
	AddDynamicTags(writer);
}
