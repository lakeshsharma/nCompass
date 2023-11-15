#pragma once

class Calibration
{
	
public:
	
	typedef enum
	{
		Idle,
		WaitBrowserTermination,
		InCalibration,
		CalibrationComplete,
		WaitUIRestart,
	} CalibrationState;

	
	Calibration();
	~Calibration();
	
	/// <summary> 
	/// Sets the state to start the calibration processes if appropriate
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState StartCalibration();
	
	/// <summary> 
	/// Indicates that an employee card was inserted. May start the calibraton 
	/// process 
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState EmployeeCardInserted();
	
	/// <summary> 
	/// Indicates that an employee card was inserted. May kill the calibration 
	/// process 
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState EmployeeCardRemoved();
	
	/// <summary> 
	/// Indicates that the browser was terminated 
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState BrowserTerminated();
	
	/// <summary> 
	/// Checks to see if the calibration was complete 
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState CheckCalibrationComplete();
	
	/// <summary> 
	/// Indicates that the UI was restarted 
	/// </summary>
	/// <returns>The new calibration state
	/// </returns> 
	CalibrationState UIRestarted();
	
	/// <summary> 
	/// Thread function to run the calibration utility
	/// </summary>
	void RunCalibrationUtility();
	
	/// <summary> 
	/// are we currently in the calibration process 
	/// </summary>
	/// <returns>true if in the calibration process
	/// </returns> 
	bool InCalibrationProcess();
	
	/// <summary> 
	/// Check if calibration needs to be performed
	/// </summary>
	/// <returns>true if in the calibration is needed, false otherwise.
	/// </returns> 
	bool CheckCalibCardCount();

	/// <summary> 
	/// Checks if there was a calibratione error. 
	/// </summary>
	/// <returns> 
	/// true if there was calibration error, false otherwise. 
	/// </returns> 
	bool CalibrationError() const;

private:
	
	CalibrationState m_calibrationState;
	
	DWORD m_lastCalibrationCardInTime;
	int m_calibrationCardCount;
	
	std::unique_ptr<std::thread> m_calibrateThread;

	bool m_calibrationError;
	
	void KillCalibrationUtility();
	
	void ResetCalibCardCount();

	void BackupRulesFile();
	
	void RestoreBackedupRulesFile();

	void DeleteBackupRulesFile();
};
