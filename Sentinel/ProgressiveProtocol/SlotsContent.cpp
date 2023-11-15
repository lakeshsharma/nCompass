#include "stdafx.h"
#include <memory.h>
#include "MediaPackageManager.h"
#include "Logging/LogString.h"
#include "RapidJsonHelper.h"
#include "SlotsContent.h"

using namespace std;

CSlotsContentFile::CSlotsContentFile(
	int64_t id,
	int64_t minorId,
	SlotsContentType::Type slotContentType,
	const string& orientationContent,
	const string& filename,
	const string& checksum) :
m_id(to_string(id)),
m_minorId((NOT_A_MINOR == minorId) ? "" : to_string(minorId)),
m_slotContentType(SlotsContentType::emap.At(slotContentType)),
m_orientationContent(orientationContent),
m_filename(filename),
m_checksum(checksum)
{
	LogString(ONELINKCONTENT_LOG, "CSlotsContentFile key=%s m_filename=%s m_checksum=%s", GetKey().c_str(), m_filename.c_str(), m_checksum.c_str());
}

string CSlotsContentFile::GetKey() const
{
	return GetKey(m_id, m_minorId, m_slotContentType, m_orientationContent);
}

string CSlotsContentFile::GetKey(const string& id, const string& minorId, const string& slotContentType, const string& orientationContent)
{
	string key;

	key.append(id);
	key.append("-");

	if (!minorId.empty())
	{
		key.append(minorId);
		key.append("-");
	}

	key.append(slotContentType);
	key.append("-");

	key.append(orientationContent);

	return key;
}


CSlotsContentPackage::CSlotsContentPackage(const rapidjson::Value& contentPackage, int64_t id, SlotsContentType::Type slotContentType, int64_t minorId) :
	m_id(id), m_minorId(minorId), m_slotContentType(slotContentType)
{
	AddContentFile(contentPackage, id, minorId, slotContentType, pKeyLcdContent);
}

void CSlotsContentPackage::AddContentFile(const rapidjson::Value& contentPackage,
	int64_t id,
	int64_t minorId,
	SlotsContentType::Type slotContentType,
	const string& orientationContent)
{
	const rapidjson::Value& contentFileJson = contentPackage[orientationContent.c_str()];
	if (contentFileJson.IsObject())
	{
		m_contentFiles.push_back(unique_ptr<CSlotsContentFile>
			(new CSlotsContentFile(id, minorId, slotContentType, orientationContent, string(JsonGetStringEx(contentFileJson, pKeyFileName, "")), string(JsonGetStringEx(contentFileJson, pKeyMD5, "")))));
	}
}