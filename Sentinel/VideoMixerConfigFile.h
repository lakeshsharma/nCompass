#pragma once
#include "ConfigFile.h"
#include "VideoModeConfig.h"

class VideoMixerConfigFile : CConfigFile
{

public:
	/// <summary>
    /// Standard constructor. Initializes a new instance of the <see 
    /// cref="VideoMixerConfigFile"/> class. 
	/// </summary>
	/// <param name="startupTickcount">IN - The startup tickcount.</param>
	VideoMixerConfigFile(DWORD startupTickcount);

	/// <summary>
    /// Destructor. Finalizes an instance of the <see 
    /// cref="VideoMixerConfigFile"/> class. 
	/// </summary>
	~VideoMixerConfigFile();

    /// <summary> Checks if the config file exists. 
    /// </summary>
    /// <returns>true if the config file exists, false 
    /// otherwise.</returns> 
    static bool CheckExists();

    /// <summary> Gets the video input configuration specified in 
    /// the file.
	/// </summary>
    /// <param name="inputFormatType">OUT-Input format type 
    /// string.</param> 
    /// <param name="isInputAutoConfig">OUT-set true if auto input
    /// is specified</param>
    /// <param name="inputMode">OUT-mode number specified</param> 
    /// <returns>True if the file contains input 
    /// parameters. If false all parameters returned are 
    /// N/A</returns> 
    bool GetVideoInput(std::string &inputFormatType, bool &isInputAutoConfig, int &inputMode);

    /// <summary> Gets the video output configuration specified in 
    /// the file. 
	/// </summary>
    /// <param name="isOutputAutoConfig">OUT-set true if auto output
    /// is specified</param>
    /// <param name="outputMode">OUT-mode number specified</param> 
    /// <returns>True if the file contains output parameters. If 
    /// false all parameters returned are 
    /// N/A</returns> 
    bool GetVideoOutput(bool &isOutputAutoConfig, int &outputMode);

    /// <summary> Gets the touchscreen protocol specified in the 
    /// file.
    /// </summary>
    /// <param name="touchProtocolName">OUT-the specified 
    /// touch screen protocol name</param> 
    /// <returns>True if the file contains touch 
    /// parameters. If false all parameters returned are 
    /// N/A</returns> 
    bool GetTouchProtocol(std::string &touchProtocolName);

	/// <summary>
	/// Starts initial config file load for this instance.
	/// </summary>
	/// <returns>true if any name/value pairs were processed from the configuration file, false otherwise.</returns>
	virtual bool Start();

	/// <summary>
	/// Interprets the name and value.
	/// </summary>
	/// <param name="name">The name.</param>
	/// <param name="value">The value.</param>
	/// <returns>True if name matches expected configuration option and value successfully parsed.</returns>
	virtual bool InterpretNameValue(const std::string& name, const std::string& value);


private:

    bool m_inputModeSpecified;
    std::string m_inputFormatType;
    bool m_inputAutoConfig;
    int m_inputMode;

    bool m_outputModeSpecified;
    bool m_outputAutoConfig;
    int m_outputMode;

    std::string m_touchProtocolName;

	void SetDefaults();
	void LogConfigValues(LPCTSTR sourceDesc, bool valid);
};
