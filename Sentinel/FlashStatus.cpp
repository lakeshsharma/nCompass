#include "stdafx.h"
#include "FlashStatus.h"

CFlashStatus::CFlashStatus(FlashType type, byte percentComplete, const std::string &checksum,
	const std::string &timestamp, const std::string &filename, const std::string &url):
	m_flashType(type),
	m_percentComplete(percentComplete),
	m_checksum(checksum),
	m_fileName(filename),
	m_timeStamp(timestamp),
	m_url(url)
{
} 

CFlashStatus::~CFlashStatus(void)
{
}

FlashType CFlashStatus::GetType() const
{
	return m_flashType;
}

std::string CFlashStatus::GetChecksum() const
{
	return m_checksum;
}

byte CFlashStatus::GetPercentComplete() const
{
	return m_percentComplete;
}

void CFlashStatus::SetPercentComplete(byte percent)
{
	m_percentComplete = percent;
}

std::string CFlashStatus::GetFilename() const
{
	return m_fileName;
}

void CFlashStatus::SetFilename(const std::string &filename)
{
	m_fileName = filename;
}

std::string CFlashStatus::GetTimestamp() const
{
	return m_timeStamp;
}

void CFlashStatus::SetTimestamp(const std::string &timestamp)
{
	m_timeStamp = timestamp;
}

std::string CFlashStatus::GetStatus() const
{
	return m_status;
}

void CFlashStatus::SetStatus(const std::string &status)
{
	m_status = status;
}

std::string CFlashStatus::GetUrl() const
{
	return m_url;
}

void CFlashStatus::SetUrl(const std::string &url)
{
	m_url = url;
}
