#include "stdafx.h"
#include "Calibration.h"
#include "Utilities.h"
#include "DisplayManager.h"
#include "Logging/LogString.h"

using namespace std;

const int MAX_CALIBRATION_CARD_COUNT = 3;
const DWORD CALIBRATION_CARD_TIMEOUT = 10000;
const string LCD_UDEV_RULE_FILE = "/etc/udev/rules.d/10-ar1xxxuart.rules";
const string MW_UDEV_RULE_FILE = "/etc/udev/rules.d/10-pt306-touch.rules";
const string BACKUP_SUFFIX = ".bkp";

static UINT AFX_CDECL CalibrateThread( PVOID threadParam );

Calibration::Calibration():
m_calibrationState(Idle),
m_lastCalibrationCardInTime(0),
m_calibrationCardCount(0),
m_calibrationError(false)
{
}

Calibration::~Calibration()
{
	// This will close thread if still running
	KillCalibrationUtility();
	CheckCalibrationComplete();
}


Calibration::CalibrationState Calibration::StartCalibration()
{
	if (m_calibrationState == Idle)
	{
		LogString(DISPLAY_LOG, "Starting the calibration process. Wait for browser to close");
		m_calibrationState = WaitBrowserTermination;
	}
	
	return m_calibrationState;
}

Calibration::CalibrationState Calibration::EmployeeCardInserted()
{
	if (CheckCalibCardCount())
	{
		m_calibrationState = StartCalibration();
	}
	
	return m_calibrationState;
}

Calibration::CalibrationState Calibration::EmployeeCardRemoved()
{
	// End the calibration thread if running
	KillCalibrationUtility();
	
	// If we are anything but idle, we just want to restart the UI	
	if (m_calibrationState == WaitBrowserTermination)
	{
		LogString(DISPLAY_LOG, "Calibration killed on card removal");
		m_calibrationState = WaitUIRestart;
	}
	
	return m_calibrationState;
}

Calibration::CalibrationState Calibration::BrowserTerminated()
{
	if (m_calibrationState == WaitBrowserTermination)
	{
		m_calibrationState = InCalibration;
		
		LogString(DISPLAY_LOG, "Browser has been terminated. Start the calibration thread.");
		m_calibrateThread = unique_ptr<std::thread>(new std::thread(CalibrateThread, this));
		
		// Wait to for calibration utility to fully come up before we make full service window
		const DWORD FullDisplayDelay = 2000;
		LogString(DISPLAY_LOG, "Calibration Utility started. pausing %d ms", FullDisplayDelay);
		CUtilities::Sleep(FullDisplayDelay);
	}
	
	return m_calibrationState;
}


Calibration::CalibrationState Calibration::CheckCalibrationComplete()
{
	Calibration::CalibrationState returnState =  m_calibrationState;
	
	if (returnState == CalibrationComplete)
	{
		LogString(DISPLAY_LOG, "Calibration utility complete. Waiting for Calibration thread to end");
		m_calibrationState = WaitUIRestart;
		m_calibrateThread->join();
		m_calibrateThread.release();
		//Remove back up rule file if any.
		DeleteBackupRulesFile();
		LogString(DISPLAY_LOG, "Calibration thread has ended");
	}
	
	return returnState;
}


Calibration::CalibrationState Calibration::UIRestarted()
{
	if (m_calibrationState == WaitUIRestart)
	{
		LogString(DISPLAY_LOG, "UI has been restarted after calibration");
		m_calibrationState = Idle;
	}
	
	return m_calibrationState;
}

void Calibration::RunCalibrationUtility()
{
	LogString(DISPLAY_LOG, "Starting calibration utility");
	
	m_calibrationError = false;
	//Back up rule file at start
	BackupRulesFile();

	const string CalibrateCMDLine = "axpt-calibrator";
	if (system(CalibrateCMDLine.c_str()) == 0)
	{
		LogString(DISPLAY_LOG, "UI command successful for %s", CalibrateCMDLine.c_str());
	}
	else
	{
		m_calibrationError = true;
		LogString(ERROR_LOG, "Error running calibration with command %s", CalibrateCMDLine.c_str());
	}
	
	m_calibrationState = CalibrationComplete;
	
	// Wait here for the join to be called on this thread
	LogString(DISPLAY_LOG, "Waiting for calibration thread join");
	const DWORD WaitJoinSleepDuration = 10;
	while (m_calibrationState == CalibrationComplete)
	{
		CUtilities::Sleep(WaitJoinSleepDuration);
	}
	CUtilities::Sleep(WaitJoinSleepDuration);
	
		
	LogString(DISPLAY_LOG, "Exiting calibration thread function");
}

bool Calibration::InCalibrationProcess()
{
	return m_calibrationState != Idle;
}

void Calibration::KillCalibrationUtility()
{
	if (m_calibrationState == InCalibration)
	{
		LogString(DISPLAY_LOG, "Killing calibration utility");
		
		const string KillCalibrateCMDLine = "ps | grep axpt-calibrator | head -n1 | awk '{print $1}' | xargs kill";
		
		if (system(KillCalibrateCMDLine.c_str()) == 0)
		{
			LogString(DISPLAY_LOG, "UI command successful for %s", KillCalibrateCMDLine.c_str());
			//Restore backed up rule file
			RestoreBackedupRulesFile();

			//restart udev
			const string restartudevCMDLine = "/etc/init.d/udev restart";
			if (system(restartudevCMDLine.c_str()))
				LogString(DISPLAY_LOG, "UI command successful for %s", restartudevCMDLine.c_str());
			else
				LogString(ERROR_LOG, "Error restarting udev with command %s", restartudevCMDLine.c_str());
		}
		else
		{
			LogString(ERROR_LOG, "Error killing calibration with command %s", KillCalibrateCMDLine.c_str());
		}
	}
}

bool Calibration::CheckCalibCardCount()
{
	bool startCalibration = false;
	
	DWORD currentTick = CUtilities::GetTickCount();
		
	if(currentTick - m_lastCalibrationCardInTime >= CALIBRATION_CARD_TIMEOUT)
	{
		// Start over again if too much time has elapsed.
		ResetCalibCardCount();
	}
	
	// See if we are to start calibration
	if (++m_calibrationCardCount >= MAX_CALIBRATION_CARD_COUNT) 
	{
		LogString(DISPLAY_LOG, "Calibration card count has occured %d times.", MAX_CALIBRATION_CARD_COUNT);
		startCalibration = true;
	}
	
	return startCalibration;
}

bool Calibration::CalibrationError() const
{
	return m_calibrationError;
}

void Calibration::ResetCalibCardCount()
{
	m_calibrationCardCount = 0;
	m_lastCalibrationCardInTime = CUtilities::GetTickCount();
}

static UINT AFX_CDECL CalibrateThread(PVOID threadParam)
{
	Calibration *calibration = (Calibration *)threadParam;
	
	calibration->RunCalibrationUtility();
	
	return 0;
}

void Calibration::BackupRulesFile()
{
	string BackuprulesfileCMDLine;
	if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
		BackuprulesfileCMDLine = "cp " + MW_UDEV_RULE_FILE + " " + MW_UDEV_RULE_FILE+ BACKUP_SUFFIX;
	else
		BackuprulesfileCMDLine = "cp " + LCD_UDEV_RULE_FILE + " " + LCD_UDEV_RULE_FILE + BACKUP_SUFFIX;

	if (system(BackuprulesfileCMDLine.c_str()) == 0)
	{
		LogString(DISPLAY_LOG, "UI command successful for %s", BackuprulesfileCMDLine.c_str());
	}
	else
	{
		LogString(ERROR_LOG, "Error backing up rules file %s", BackuprulesfileCMDLine.c_str());
	}
}

void Calibration::RestoreBackedupRulesFile()
{
	string RestorebackupbulesFileCMDLine;
	if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
		RestorebackupbulesFileCMDLine = "mv " + MW_UDEV_RULE_FILE + BACKUP_SUFFIX + " " + MW_UDEV_RULE_FILE;
	else
		RestorebackupbulesFileCMDLine = "mv " + LCD_UDEV_RULE_FILE + BACKUP_SUFFIX + " " + LCD_UDEV_RULE_FILE;

	if (system(RestorebackupbulesFileCMDLine.c_str()) == 0)
	{
		LogString(DISPLAY_LOG, "UI command successful for %s", RestorebackupbulesFileCMDLine.c_str());
	}
	else
	{
		LogString(ERROR_LOG, "Error restoring backed up rules file %s", RestorebackupbulesFileCMDLine.c_str());
	}
}

void Calibration::DeleteBackupRulesFile()
{
	string BackedupRulesfile;
	if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
		BackedupRulesfile = MW_UDEV_RULE_FILE + BACKUP_SUFFIX;
	else
		BackedupRulesfile = LCD_UDEV_RULE_FILE + BACKUP_SUFFIX;
	bool couldDelete = CUtilities::DeleteFile(BackedupRulesfile.c_str());
	if (couldDelete)
	{
		LogString(DISPLAY_LOG, "Successfully deleted %s backup rules file.", BackedupRulesfile.c_str());
	}
}




