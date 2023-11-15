
#include "stdafx.h"
#include "VideoMixerDiagnostics.h"
#include "DirectoryManager.h"
#include "Logging/LogString.h"
#include "axhybrid.h"
#include "DisplayManager.h"
#include "Hardware/WatchDog.h"

using namespace std;

VideoMixerDiagnostics::VideoMixerDiagnostics()
{
    m_initialized = false;
}

VideoMixerDiagnostics::~VideoMixerDiagnostics()
{
}

bool VideoMixerDiagnostics::DumpVideoMixerDiagnostics()
{

    string diagFile = CDirectoryManager::Instance().VideoMixerDiagnosticsInternalFilePath();

    LogString(DISPLAY_LOG, "Dumping VM diagnostics to %s", diagFile.c_str());

    int32_t vmResult = AXVM_DebugDump(diagFile.c_str());

    CWatchDog::Instance().Checkin();

	if (vmResult != VM_Success)
	{
		LogString(ERROR_LOG, "AXVM_DebugDump failure %d", vmResult);
	}

    // Now append the touchscreen versions to this file
    ofstream vmDiagnosticsFile;
    vmDiagnosticsFile.open(diagFile.c_str(), ios_base::app);
    if (vmDiagnosticsFile.is_open())
    {
        struct axvm_touch_version touchVersion;

        vmResult = AXVM_GetTouchFirmwareVersion(&touchVersion);

        CWatchDog::Instance().Checkin();

    	if (vmResult == VM_Success)
        {
            string touchVersionString = FormatString("\nTouchscreen version number: %d.%d.%d - %d\n", touchVersion.touch_version[0], touchVersion.touch_version[1], touchVersion.touch_version[2], touchVersion.touch_version[3]);
            vmDiagnosticsFile << touchVersionString;
        }
        else
    	{
    		LogString(ERROR_LOG, "AXVM_GetTouchFirmwareVersion failure %d", vmResult);
    	}

        struct axvm_touch_debug touchRegionDebug;
        int touchRegionIndex = 0;
        const int StartTouchRegion = 1;
        const int MaxTouchRegions = 256;    //This isures that, in case of a problem, this does not loop forever
        for (int touchRegionIndex = StartTouchRegion; touchRegionIndex < MaxTouchRegions; touchRegionIndex++) 
        {

            vmResult = AXVM_DebugUSBTouchFirmware(&touchRegionDebug, touchRegionIndex);

            CWatchDog::Instance().Checkin();

            if (vmResult == VM_Success)
            {
                string touchRegionDebugString = FormatString("\nTouchscreen region %d - %s\n", touchRegionDebug.touch_region, touchRegionDebug.string_style_version);
                vmDiagnosticsFile << touchRegionDebugString;
            }
            else
            {
                if (vmResult == VM_TouchFWError) 
                {
                    LogString(ERROR_LOG, "AXVM_DebugUSBTouchFirmware failure for touch region %d", touchRegionIndex);
                }
                else
                {
                    vmResult = VM_Success;  // When we reached the end of the regions, a VM_invalid will be returned
                }
                break;  // We will break out of this when there are no more touch firmware regions
            }
        }
        
        vmDiagnosticsFile.close();

        FlushFileSystemBuffers();
    }
    else
    {    
    	LogString(ERROR_LOG, "Could not append the touchscreen version to vm diagnostics file");
    }

    return (vmResult == VM_Success);
}

bool VideoMixerDiagnostics::CheckForExternalVideoMixerDiagnosticsFile()
{
    bool diagnosticsDumpedToExternalFile = false;

    if (!m_initialized) 
    {
        m_externalFileProcessed = false;
        m_externalFileCheckTick = GetTickCount();
        m_initialized = true;
    }

    const DWORD ExternalFileCheckInterval = 10000;

   // Only check for external disagnostics file every 10 seconds, 206/204 dont support videoMixerDiagnostics
    if ( ((GetTickCount() - m_externalFileCheckTick) >= ExternalFileCheckInterval) && (!IsTFTMode()))
    {
        // See if there is an external disagnostics file present
        std::string externalFilePath;
        bool externalFileExists = IsExternalVideoMixerDiagnosticsFilePresent(externalFilePath);

        // We only want to process the external file once per insertion.
        if (externalFileExists && !m_externalFileProcessed) 
        {
            // Dump the video mixer diagnostics to an internal file
            if (DumpVideoMixerDiagnostics())
            {
                // Copy the dumped video mixer diagnostics to the external file.
                LogString(DISPLAY_LOG, "Copying VM diagnostics to %s", externalFilePath.c_str());
                if ( !CopyFile(CDirectoryManager::Instance().VideoMixerDiagnosticsInternalFilePath(), externalFilePath) )
                {
                    LogString(ERROR_LOG, "Could not copy VM diagnostics file to %s", externalFilePath.c_str());
                }
                FlushFileSystemBuffers();
                diagnosticsDumpedToExternalFile = true;
            }
        }

        m_externalFileProcessed = externalFileExists;

        m_externalFileCheckTick = GetTickCount();
    }

    return diagnosticsDumpedToExternalFile;
}

bool VideoMixerDiagnostics::IsExternalVideoMixerDiagnosticsFilePresent(std::string &externalFilePath)
{
    externalFilePath.clear();

    // cycle thru the USB paths
    int usbIndex = 0;
    bool moreUSBPaths = true;
    while (externalFilePath.empty() && moreUSBPaths)
    {
        externalFilePath = CDirectoryManager::Instance().VideoMixerDiagnosticsUSBFilePath(usbIndex);

        if (externalFilePath.empty())
        {               
            moreUSBPaths = false;
        }
        else if (!FileExists(externalFilePath)) 
        {
            externalFilePath.clear();
        }
        usbIndex++;
    }

    return (!externalFilePath.empty());
}

DWORD VideoMixerDiagnostics::GetTickCount()
{
    DWORD tick = CUtilities::GetTickCount();
    return tick;
}

bool VideoMixerDiagnostics::FileExists(const std::string &filename)
{
    return CUtilities::FileExists(filename);
}

void VideoMixerDiagnostics::FlushFileSystemBuffers()
{
    CUtilities::FlushFileSystemBuffers();
}

bool VideoMixerDiagnostics::CopyFile(const std::string &source, const std::string &target)
{
    return CUtilities::CopyFile(source, target);
}

bool VideoMixerDiagnostics::IsTFTMode()
{
    return CDisplayManager::Instance().IsTFTMode();
}
