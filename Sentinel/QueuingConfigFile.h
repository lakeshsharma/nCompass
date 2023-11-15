#pragma once
#include "ConfigFile.h"
#include "QueuingConfig.h"

class CQueuingConfigFile :
	public CConfigFile
{
public:
	static const LPCTSTR CONFIG_FILENAME; // The configuration filename.
	CQueuingConfigFile(IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);
	virtual ~CQueuingConfigFile();
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
	/// Gets a copy of the queuing configuration.
	/// </summary>
	/// <returns>A pointer to a new CQueuingConfig instance.  MUST NOT return NULL.</returns>
	/// <remarks>
	/// The caller is responsible for deleting the returned instance.
	/// </remarks>
	CQueuingConfig* GetQueuingConfigCopy() const;

private:
	static const LPCTSTR HAS_CONFIG_FILE;

	void SetDefaults();
	CQueuingConfig m_queuingConfig;
};
