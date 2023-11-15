#pragma once

#include "InternalMsgContent.h"

#define INVALID_VIDEO_MODE_INDEX -1

typedef enum
{
	VIDEO_MODE_SIGNAL_AUTO,
	VIDEO_MODE_SIGNAL_ANALOG,
	VIDEO_MODE_SIGNAL_DVI,
	VIDEO_MODE_SIGNAL_DP,
} VideoModeSignalType;

typedef enum
{
	VIDEO_CHANGE_NONE,
	VIDEO_CHANGE_INPUT_ENTERED,
	VIDEO_CHANGE_OUTPUT_ENTERED,
	VIDEO_CHANGE_OFFSETS_ENTERED,
	VIDEO_CHANGE_RESTARTING_UI,
	VIDEO_CHANGE_REVERT_TIMER,
} VideoConfigChangeStatus;

class CVideoModeConfig:
	public CInternalMsgContent<INT_MSG_CONTENT_VIDEO_MODE_CONFIG>
{
public:
	// Constructor
	CVideoModeConfig(void);
	
	/// <summary>
	/// Constructor for input mode configuration from UI
	/// </summary>
	/// <param name="signalType">The type of input signal to be set.</param>
	/// <param name="inputModeIsAuto">true if input mode is to be auto 
	/// config</param> 
	/// <param name="inputMode">The input mode number if not auto config</param> 
	CVideoModeConfig(const std::string& signalTypeString, bool inputModeIsAuto, int inputMode);
	
	/// <summary>
	/// Constructor for output mode configuration from UI
	/// </summary>
	/// <param name="outputModeIsAuto">true if output mode is to be auto 
	/// config</param> 
	/// <param name="outputMode">The output mode number if not auto config</param> 
	CVideoModeConfig(bool outputModeIsAuto, int outputMode);
	
	/// <summary>
	/// Constructor for offsets configuration from UI
	/// </summary>
	/// <param name="egmHorizontalOffset">The egm horizontal offset</param> 
	/// <param name="egmVerticalOffset">The egm vertical offset</param> 
	/// <param name="monitorHorizontalOffset">The monitor horizontal offset</param> 
	/// <param name="monitorVerticalOffset">The monitor vertical offset</param> 
	CVideoModeConfig(int egmHorizontalOffset, int egmVerticalOffset, int monitorHorizontalOffset, int monitorVerticalOffset);
	
	/// <summary>
	/// Constructor for the revert configuration
	/// </summary>
	/// <param name="signalTypeString">The type of input signal to be set.</param>
	/// <param name="inputModeIsAuto">true if input mode is to be auto 
	/// config</param> 
	/// <param name="inputMode">The input mode number if not auto config</param>
	/// <param name="outputModeIsAuto">true if output mode is to be auto 
	/// config</param> 
	/// <param name="outputMode">The output mode number if not auto config</param> 
	CVideoModeConfig(VideoModeSignalType signalType, bool inputModeIsAuto, int selectedInputModeIndex, bool outputModeIsAuto, int selectedOutputModeIndex);
	
	/// <summary> 
	/// Copy Constructor
	/// </summary>
	CVideoModeConfig(const CVideoModeConfig &rhs);

	// Destructor
	~CVideoModeConfig(void);

	// CVideoModeConfig class assignment operator
	CVideoModeConfig& operator=(const CVideoModeConfig &rhs);
	
	/// <summary>
	/// Is this configuration object for input video configuration
	/// </summary>
	/// <returns>true if the configuration is for input only</returns>
	bool IsInputConfig();
	
	/// <summary>
	/// Is this configuration object for output video configuration
	/// </summary>
	/// <returns>true if the configuration is for output only</returns>
	bool IsOutputConfig();
	
	/// <summary>
	/// Is this configuration object for offsets video configuration
	/// </summary>
	/// <returns>true if the configuration is for offsets only</returns>
	bool IsOffsetConfig();
	
	/// <summary>
	/// Gets the input signal type
	/// </summary>
	/// <returns>the input sigal type</returns>
	VideoModeSignalType GetInputSignalType();
	
	/// <summary>
	/// Is set for auto input mode configuration
	/// </summary>
	/// <returns>true if auto input mode</returns>
	bool IsAutoInputMode();
	
	/// <summary>
	/// Gets the input mode number
	/// </summary>
	/// <returns>returns input mode number set. Value only applicable if 
	/// not auto input mode</returns> 
	int GetInputMode();
	
	/// <summary>
	/// Is set for auto output mode configuration
	/// </summary>
	/// <returns>true if auto output mode</returns>
	bool IsAutoOutputMode();
	
	/// <summary>
	/// Gets the output mode number
	/// </summary>
	/// <returns>returns output mode number set. Value only applicable if 
	/// not auto output mode</returns> 
	int GetOutputMode();
	
	/// <summary>
	/// Gets the set video offsets 
	/// </summary>
	/// <param name="egmHorizontalOffset">output for the egm horizontal 
	/// offset</param> 
	/// <param name="egmVerticalOffset">output for the egm vertical offset</param> 
	/// <param name="monitorHorizontalOffset">output for the monitor horizontal 
	/// offset</param> 
	/// <param name="monitorVerticalOffset">output for the monitor vertical 
	/// offset</param> 
	void GetOffsets(int &egmHorizontalOffset, int &egmVerticalOffset, int &monitorHorizontalOffset, int &monitorVerticalOffset);
	
	/// <summary>
	/// Indicates that the UI was successfully restarted. Might start the revert 
	/// timer 
	/// </summary>
	/// <returns>returns the status after UI restart</returns> 
	VideoConfigChangeStatus UIRestarted();
	
	/// <summary>
	/// Determines if the revert video mode timer has expired 
	/// </summary>
	/// <returns>returns true if we are waiting to revert video mode and 
	/// the revert timer expired</returns> 
	bool HasRevertTimeExpired();

	/// <summary>
	/// Converts the provided string to a VideoModeSignalType
	/// </summary>
    /// <param name="signalTypeString">signal type string to 
    /// convert</param> 
	/// <returns>returns the signal type</returns> 
    static VideoModeSignalType ConvertStringToSignalType(const std::string& signalTypeString);

	static const std::string AutoTypeString;
	static const std::string AnalogTypeString;
	static const std::string DVITypeString;
	static const std::string DisplayPortTypeString;
	static const std::string NotConnectedTypeString;
	static const std::string ErrorTypeString;
	static const std::string UnknownTypeString;

private:
	VideoModeSignalType m_signalType;

	bool m_inputModeIsAuto;
	int m_selectedInputModeIndex;
	
	bool m_outputModeIsAuto;
	int m_selectedOutputModeIndex;
	
	int m_egmHorizontalOffset;
	int m_egmVerticalOffset;
	int m_monitorHorizontalOffset;
	int m_monitorVerticalOffset;
	
	VideoConfigChangeStatus m_changeStatus;
	
	DWORD m_revertTick;
	
	void SetDefaults();
};
