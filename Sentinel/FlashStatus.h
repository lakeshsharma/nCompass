#pragma once
#include "InternalMsgContent.h"

enum FlashType
{
	FLASH_NONE = 0,
	FLASH_NK,
	FLASH_SYSTEM = 4,
	FLASH_DEFAULTMEDIA,
	FLASH_CONFIG,
	FLASH_CUSTOM_CONTENT,
	FLASH_CAMPAIGN_MEDIA,
	FLASH_SKIN
};

class CFlashStatus :
	public CInternalMsgContent<INT_MSG_CONTENT_FLASHSTATUS>
{
public:
	CFlashStatus(FlashType type, byte percentComplete, const std::string &checksum,
		const std::string &timestamp, const std::string &filename, const std::string &url);
	~CFlashStatus(void);

	FlashType GetType() const;
	std::string GetChecksum() const;

	byte GetPercentComplete() const;
	void SetPercentComplete(byte percent);

	std::string GetFilename() const;
	void SetFilename(const std::string &filename);

	std::string GetTimestamp() const;
	void SetTimestamp(const std::string &timestamp);

	std::string GetStatus() const;
	void SetStatus(const std::string &error);

	std::string GetUrl() const;
	void SetUrl(const std::string &url);

private:
	FlashType m_flashType;
	byte m_percentComplete;
	std::string m_checksum;
	std::string m_fileName;
	std::string m_timeStamp;
	std::string m_status;
	std::string m_url;
};
