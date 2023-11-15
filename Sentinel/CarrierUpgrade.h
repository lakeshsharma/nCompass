#pragma once
#include "FPGAUpgrade.h"
#include "InternalMsgContent.h"

class CCarrierUpgrade :
	public CFPGAUpgrade, public CInternalMsgContent<INT_MSG_CONTENT_CARRIER_UPGRADE>
{
public:
	CCarrierUpgrade( );
	CCarrierUpgrade(const std::string& path, FPGAUpgradeLocationType locationType );
	virtual ~CCarrierUpgrade(void);

	std::string GetCurrentVersionString();
	std::string GetUpgradeVersionString();
	bool IsCurrent();
	bool Upgrade();

protected:
	std::string m_currentVersionNumber;
	std::string m_upgradeVersionNumber;
	void Init(const std::string &path, FPGAUpgradeLocationType locationType );


};
