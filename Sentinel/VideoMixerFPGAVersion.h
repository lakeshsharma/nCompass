#pragma once

class CVideoMixerFPGAVersion
{
public:
	CVideoMixerFPGAVersion(void);
	~CVideoMixerFPGAVersion(void);

	std::string GetVersionString( );
	std::string GetUpgradeVersionString();
	void SetVersion( const std::string& major, const std::string& minor, const std::string& build );
	void SetVersion( int16_t major, int16_t minor, int16_t build );
	
private:
	std::string m_major;
	std::string m_minor;
	std::string m_build;


};
