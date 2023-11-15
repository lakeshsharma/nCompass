#pragma once

#include "stdafx.h"
#include "ConfigFile.h"
#include "ProgressiveConfig.h"

class CProgressiveConfigFile : public CConfigFile
{
public:
	/// <summary>
	/// Standard/Default constructor. Initializes a new instance of the <see cref="CThirdPartyUIConfig"/> class.
	/// </summary>
	/// <param name="operatingSystem">An interface to operating system functionality.</param>
	CProgressiveConfigFile(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CThirdPartyUIConfig"/> class.
	/// </summary>
	~CProgressiveConfigFile();

	/// <summary>
	/// Starts initial config file load for this instance.
	/// </summary>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	bool Start();

	/// <summary>
	/// Refreshes the configuration information for this instance.
	/// </summary>
	/// <param name="refresh">IN - This parameter is unused.</param>
	void RefreshConfig(bool refresh);

	/// <summary>
	/// Interprets the name and value.
	/// </summary>
	/// <param name="name">IN - The name.</param>
	/// <param name="value">IN - The value.</param>
	/// <returns>true if the name and value are valid, false</returns>
	bool InterpretNameValue(const std::string& name, const std::string& value);

	/// <summary>
	/// Logs the configuration values.
	/// </summary>
	/// <param name="sourceDesc">IN - Name of the configuration file.</param>
	/// <param name="valid">IN - true if the configuration file contained at least one valid name/value pair, false otherwise.</param>
	void LogConfigValues(LPCTSTR sourceDesc, bool valid);

	/// <summary>
	/// Sets the progressive configuration.
	/// </summary>
	/// <param name="progressiveConfg">IN - The progressive configuration.</param>
	void SetProgressiveConfig(const CProgressiveConfig &progressiveConfig);

	/// <summary>
	/// Gets a copy of the progressive configuration.
	/// </summary>
	/// <returns>A pointer to a new CProgressiveConfig instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CProgressiveConfig* GetProgressiveConfigCopy() const;

private:
	static const LPCTSTR CONFIG_FILENAME; // The configuration filename.

	// Parameter names used in the configuration file.
	static const LPCTSTR HAS_CONFIG_FILE;
	static const LPCTSTR ACTIVE;
	static const LPCTSTR DNS_LOOKUP;
	static const LPCTSTR ENDPOINT;
	static const LPCTSTR PING_INTERVAL;

	void SetDefaults();
	bool WriteConfigFile(const std::shared_ptr<std::ofstream>  &file);

	CProgressiveConfig m_progressiveConfig;
};

