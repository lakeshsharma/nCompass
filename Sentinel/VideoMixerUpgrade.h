#pragma once
#include "FPGAUpgrade.h"
#include "VideoMixerFPGAVersion.h"
#include "InternalMsgContent.h"

class CVideoMixerUpgrade :
	public CFPGAUpgrade, public CInternalMsgContent<INT_MSG_CONTENT_VIDEOMIXER_UPGRADE>
{
public:
	CVideoMixerUpgrade(  );
	CVideoMixerUpgrade(const std::string& path, FPGAUpgradeLocationType locationType);
	virtual ~CVideoMixerUpgrade(void);

	std::string GetCurrentVersionString();
	std::string GetUpgradeVersionString();
	bool IsCurrent();
	bool Upgrade();

private:
	CVideoMixerFPGAVersion m_currentVersionNumber;
	CVideoMixerFPGAVersion m_upgradeVersionNumber;
	void Init(const std::string& path, FPGAUpgradeLocationType locationType);
	std::recursive_mutex m_cs;	

};
