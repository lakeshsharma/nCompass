#include "PackageInfo.h"

CPackageInfo::CPackageInfo() : m_id(0), m_revision(0), m_pendingMediaCount(0)
{
}

CPackageInfo::CPackageInfo(DWORD id, const std::string &name, const std::string &version, DWORD revision, DWORD mediaCount) :
	m_name(name), m_version(version), m_id(id), m_revision(revision), m_pendingMediaCount(mediaCount)
{
}	 

bool CPackageInfo::operator==(CPackageInfo &info)
{
	return (this->m_name == info.m_name
		&& this->m_version == info.m_version
		&& this->m_id == info.m_id
		&& this->m_revision == info.m_revision
		&& this->m_pendingMediaCount == info.m_pendingMediaCount);
}

std::string CPackageInfo::GetName() const
{ 
	return m_name;
}

DWORD CPackageInfo::GetId() const
{
	return m_id;
}

std::string CPackageInfo::GetVersion() const
{
	return m_version;
}

DWORD CPackageInfo::GetRevision() const
{
	return m_revision;
}

DWORD CPackageInfo::GetPendingMediaCount() const
{
	return m_pendingMediaCount;
}

void CPackageInfo::DecrementPendingMediaCount()
{
	m_pendingMediaCount--;
}

