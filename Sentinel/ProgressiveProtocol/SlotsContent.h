#pragma once

#include "ProgressiveKeys.h"
#include "MediaPackageManager.h"

class ProgressiveLevelManager;
class CProgressiveLevel;
class CSlotsContentPackage;

const int64_t NOT_A_MINOR(-1);

class CSlotsContentFile
{
	friend ProgressiveLevelManager;
	friend CSlotsContentPackage;
	friend CMediaPackageManager;

public:
	CSlotsContentFile(
		int64_t id,
		int64_t minorId,
		SlotsContentType::Type slotContentType,
		const std::string& orientationContent,
		const std::string& filename,
		const std::string& checksum);

	std::string GetKey() const;
	inline std::string GetFileName() const { return m_filename; };
	inline std::string GetChecksum() const { return m_checksum; };

	static std::string GetKey(const std::string& id, const std::string& minorId, const std::string& slotContentType, const std::string& orientationContent);

protected:
	std::string m_id;
	std::string m_minorId;
	std::string m_slotContentType;
	std::string m_orientationContent;
	std::string m_filename;
	std::string m_checksum;	

	CSlotsContentFile(const CSlotsContentFile& other) = delete;
	void operator=(const CSlotsContentFile& other) = delete;
};


class CSlotsContentPackage
{
	friend ProgressiveLevelManager;
	friend CMediaPackageManager;
protected:
	int64_t m_id;
	int64_t m_minorId;
	SlotsContentType::Type m_slotContentType;

	vector<std::unique_ptr<CSlotsContentFile>> m_contentFiles;

	// None of these parameters are checked since only CSlotsContentPackage class calls it with guaranteed valid data.
public:
	CSlotsContentPackage(const rapidjson::Value& contentPackage, int64_t id, SlotsContentType::Type slotContentType, int64_t minorId = NOT_A_MINOR);

protected:
	CSlotsContentPackage(const CSlotsContentPackage& other) = delete;
	void operator=(const CSlotsContentPackage& other) = delete;

private:
	void AddContentFile(const rapidjson::Value& contentPackage, 
		int64_t id, 
		int64_t minorId,
		SlotsContentType::Type slotContentType,
		const std::string& orientationContent);
};