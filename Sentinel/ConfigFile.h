#pragma once
#include "ProcessDump/OperatingSystem.h"

class CConfigFile
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigFile"/> class.
	/// </summary>
	/// <param name="startupTickCount">IN - The startup tick count.</param>
	/// <param name="operatingSystem">IN/OUT - The operating system.</param>
	CConfigFile(DWORD startupTickCount,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	/// <summary>
	/// Starts initial config file load for this instance.
	/// </summary>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	virtual bool Start() = 0;

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CConfigFile"/> class.
	/// </summary>
	virtual ~CConfigFile();

	/// <summary>
	/// Loads the configuration file.
	/// </summary>
	/// <param name="configFileName">Name of the configuration file.</param>
	/// <param name="tryUSB">if true, try the USB drive.</param>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	/// <remarks>
    /// If a configuration file is not found in the default download 
    /// directory then try the OS directory. Will try USB dribe if 
    /// tryUSB is specified 
	/// </remarks>
	virtual bool LoadConfigFile(std::string configFileName, bool tryUSB = false);

	/// <summary>
	/// Gets the time that the configuration file was last loaded.
	/// </summary>
	/// <returns>The time that the configuration file was last loaded.</returns>
	time_t GetLastConfigFileLoaded() const;

	/// <summary>
	/// Reads the configuration values.
	/// </summary>
	/// <param name="filePath">The filename including path.</param>
	/// <returns>0 if file not found, 1 if file found but not valid,
	/// 2 if file found and valid.
	/// processed.</returns> 
	virtual int ReadConfigValues(std::string filePath);

	/// <summary>
	/// Interprets the name and value.
	/// </summary>
	/// <param name="name">The name.</param>
	/// <param name="value">The value.</param>
	/// <returns>True if name matches expected configuration option and value successfully parsed.</returns>
	virtual bool InterpretNameValue(const std::string& name, const std::string& value) = 0;

	/// <summary>
	/// Logs the configuration values.
	/// </summary>
	/// <param name="sourceDesc">IN - Name of the configuration file.</param>
	/// <param name="valid">IN - true if the configuration file contained at least one valid name/value pair, false otherwise.</param>
	virtual void LogConfigValues(LPCTSTR sourceDesc, bool valid) = 0;

	/// <summary>
	/// Set "variable" with a succesfully parsed "value".
	/// </summary>
	/// <param name="value">IN - The string value to parse.</param>
	/// <param name="name">IN/OUT - The variable that is updated if "value" is successfully parsed.</param>
	/// <returns>true if "value" was parsed and "variable" set successfully, false otherwise.</returns>
	/// <remarks>
	/// "variable" is unchanged if the return value is false.
	/// </remarks>
	static bool InterpretBool(const std::string &value, bool &variable);

	/// <summary>
	/// Set "variable" with a succesfully parsed "value".
	/// </summary>
	/// <param name="value">IN - The string value to parse.</param>
	/// <param name="name">IN/OUT - The variable that is updated if "value" is successfully parsed.</param>
	/// <returns>true if "value" was parsed and "variable" set successfully, false otherwise.</returns>
	/// <remarks>
	/// "variable" is unchanged if the return value is false.
	/// Negative values are converted to unsigned values as is normal for C libraries.
	/// Only base 10 values are supported.
	/// </remarks>
	static bool InterpretWORD(const std::string &value, WORD &variable);

	/// <summary>
	/// Set "variable" with a succesfully parsed "value".
	/// </summary>
	/// <param name="value">IN - The string value to parse.</param>
	/// <param name="name">IN/OUT - The variable that is updated if "value" is successfully parsed.</param>
	/// <returns>true if "value" was parsed and "variable" set successfully, false otherwise.</returns>
	/// <remarks>
	/// "variable" is unchanged if the return value is false.
	/// Negative values are converted to unsigned values as is normal for C libraries.
	/// Only base 10 values are supported.
	/// </remarks>
	static bool InterpretDWORD(const std::string &value, DWORD &variable);

	/// <summary>
	/// Set "variable" with a succesfully parsed "value".
	/// </summary>
	/// <param name="value">IN - The string value to parse.</param>
	/// <param name="name">IN/OUT - The variable that is updated if "value" is successfully parsed.</param>
	/// <returns>true if "value" was parsed and "variable" set successfully, false otherwise.</returns>
	/// <remarks>
	/// "variable" is unchanged if the return value is false.
	/// Negative values are converted to unsigned values as is normal for C libraries.
	/// Only base 10 values are supported.
	/// </remarks>
	static bool InterpretInt(const std::string &value, int &variable);

	/// <summary>
	/// Saves the configuration file in the default download directory.
	/// </summary>
	/// <param name="configFileName">IN - Name of the configuration file.</param>
	/// <remarks>
	/// Derived classes must provide their own implementations of WriteConfigFile(CStdioFile* file), if configuration files are saved.
	/// </remarks>
	virtual void SaveConfigFile(const std::string &configFileName);

	/// <summary>
	/// Writes a configuration file.
	/// </summary>
	/// <param name="filePath">IN - Full path of the configuration file.</param>
	/// <returns>true if successful, false otherwise.</returns>
	/// <remarks>
	/// Derived classes must provide their own implementations of WriteConfigFile(CStdioFile* file), if configuration files are saved.
	/// </remarks>
	virtual bool WriteConfigFile(const std::string &filePath);

	/// <summary>
	/// Writes a configuration file.
	/// </summary>
	/// <param name="file">IN - The configuration file.</param>
	/// <returns>true if successful, false otherwise.</returns>
	/// <remarks>
	/// Derived classes must provide their own implementations of this method, if configuration files are saved.
	/// </remarks>
	virtual bool WriteConfigFile(const std::shared_ptr<std::ofstream> &file);

	/// <summary>
	/// Writes a configuration item to a configuration file.
	/// </summary>
	/// <param name="file">IN - The configuration file to write to.</param>
	/// <param name="name">IN - The name of the configuration item.</param>
	/// <param name="value">IN - The value of the configuration item.</param>
	static void WriteConfigItem(const std::shared_ptr<std::ofstream> &file, LPCTSTR name, LPCTSTR value);

	/// <summary>
	/// Writes a configuration item to a configuration file.
	/// </summary>
	/// <param name="file">IN - The configuration file to write to.</param>
	/// <param name="name">IN - The name of the configuration item.</param>
	/// <param name="value">IN - The value of the configuration item.</param>
	static void WriteConfigItem(const std::shared_ptr<std::ofstream> &file, LPCTSTR name, bool value);

	/// <summary>
	/// Writes a configuration item to a configuration file.
	/// </summary>
	/// <param name="file">IN - The configuration file to write to.</param>
	/// <param name="name">IN - The name of the configuration item.</param>
	/// <param name="value">IN - The value of the configuration item.</param>
	static void WriteConfigItem(const std::shared_ptr<std::ofstream> &file, LPCTSTR name, DWORD value);

	/// <summary>
	/// Writes a configuration item to a configuration file.
	/// </summary>
	/// <param name="file">IN - The configuration file to write to.</param>
	/// <param name="name">IN - The name of the configuration item.</param>
	/// <param name="value">IN - The value of the configuration item.</param>
	static void WriteConfigItem(const std::shared_ptr<std::ofstream> &file, LPCTSTR name, int value);

protected:
	// Values used with configuration files.
	static const LPCTSTR FALSE_STRING;
	static const LPCTSTR TRUE_STRING;
	static const LPCTSTR EQUALS_STRING;
	static const LPCTSTR NEW_LINE_STRING;
	static const int RADIX = 10; // Only support base 10 for numeric values.

	// Disable default constructor.
	CConfigFile();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CConfigFile"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CConfigFile(const CConfigFile &rhs);

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="rhs">IN - The source instance</param>
	/// <returns>A reference to the new instance.</returns>
	CConfigFile& operator=(const CConfigFile &rhs);

	virtual void SetDefaults() = 0;
	time_t m_lastConfigFileLoaded;
	DWORD m_startupTickCount;

	IOperatingSystem &m_operatingSystem;
	static std::recursive_mutex m_criticalSection;
};

