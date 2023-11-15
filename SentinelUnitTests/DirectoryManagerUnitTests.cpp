#include "stdafx.h"
#include "DirectoryManager.h"

// Unit testing includes.
#include "gtest/gtest.h"
#include "TestUtil.h"

using namespace std;

/*
DIRECTORY_APPLICATIONS,		// The sentinel applications
DIRECTORY_DEFAULTMEDIA,		// The TFT mode and employee screens
DIRECTORY_DEFAULTMEDIATEMP,	// The TFT mode and employee screens temp directory (skinning downloads)
DIRECTORY_MWMEDIA,			// The media window (service window) graphics
DIRECTORY_MWMEDIATEMP,		// The media window (service window) graphics temp directory (skinning downloads)
DIRECTORY_SMCONTENT,		// SpeedMedia content
DIRECTORY_UPGRADE,			// Updgrade
DIRECTORY_SENTINELDATA,		// Config files, misc downloaded files, EEPROM file, and reg filter files
DIRECTORY_LCDMEDIA,			// The lcd window graphics
DIRECTORY_LCDMEDIATEMP,		// The lcd window graphics temp directory (skinning downloads) 
DIRECTORY_COMPACTMEDIA,     // The compact window graphics
DIRECTORY_ERROR_FILE,		// The Error file directory
DIRECTORY_PLCONTENT,		// The playlist media
DIRECTORY_PLCONTENTTEMP,	// The playlist temp download directory
*/


LPCTSTR CompactMedia("/compactmedia");
LPCTSTR LCDMedia("/lcdmedia");
LPCTSTR MWMedia("/mwmedia");
LPCTSTR COMPACT_SHELL_XML("/CompactShell.xml");
LPCTSTR LCD_SHELL_XML("/LCDShell.xml");
LPCTSTR MEDIA_WINDOW_SHELL_XML("/MediaWindowShell.xml");

class DirectoryManagerUnitTests : public ::testing::Test
{
	public:
		void TearDown()
	{
			CDirectoryManager::MockInstanceCleanup();
	}

    void GetShellXmlPath()
    {
     
        string shellXmlFilePathPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_URL_COMPACT);

        shellXmlFilePathPath = shellXmlFilePathPath + CompactMedia;

        shellXmlFilePathPath += COMPACT_SHELL_XML;

        ASSERT_STREQ("https://localhost:8080/default/compactmedia/CompactShell.xml", shellXmlFilePathPath.c_str());

        shellXmlFilePathPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_URL_LCD);

        shellXmlFilePathPath = shellXmlFilePathPath + LCDMedia;

        shellXmlFilePathPath += LCD_SHELL_XML;

        ASSERT_STREQ("https://localhost:8080/default/lcdmedia/LCDShell.xml", shellXmlFilePathPath.c_str());

        shellXmlFilePathPath = CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_URL_MW);

        shellXmlFilePathPath = shellXmlFilePathPath + MWMedia;

        shellXmlFilePathPath += MEDIA_WINDOW_SHELL_XML;

        ASSERT_STREQ("https://localhost:8080/default/mwmedia/MediaWindowShell.xml", shellXmlFilePathPath.c_str());
    }
};

TEST_F(DirectoryManagerUnitTests, DirectoryManagerCustomPath)
{
    // Init CDirectoryManager, prevent full setup
    CDirectoryManager::MockInstance(NULL);

    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_COMPACTMEDIA).c_str(), "/tmp/ncompass/media/custom/compactmedia");
    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_LCDMEDIA).c_str(), "/tmp/ncompass/media/custom/lcdmedia");
    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_LCDMEDIATEMP).c_str(), "/tmp/ncompass/media/custom/templcdmedia");
    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_MWMEDIA).c_str(), "/tmp/ncompass/media/custom/mwmedia");
    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_MWMEDIATEMP).c_str(),  "/tmp/ncompass/media/custom/tempmwmedia");
    ASSERT_STREQ(CDirectoryManager::Instance().GetCustomPath(DIRECTORY_PLCONTENTTEMP).c_str(), "/tmp/ncompass/media/custom/tempplcontent");
}

TEST_F(DirectoryManagerUnitTests, DirectoryManagerWorkingPath)
{
    // Init CDirectoryManager, prevent full setup
    CDirectoryManager::MockInstance(NULL);
    CDirectoryManager::MockSetPath(DIRECTORY_PLCONTENT,"", "/tmp/ncompass/media/custom/plcontent");

    CDirectoryManager::MockSetPath(DIRECTORY_PLCONTENTTEMP,"", "/tmp/ncompass/media/custom/tempplcontent");

    ASSERT_STREQ("/tmp/ncompass/bin"                , CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_APPLICATIONS).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/default/mwmedia"		, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MWMEDIA).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/custom/tempmwmedia"		, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_MWMEDIATEMP).c_str());
    ASSERT_STREQ("/sdboot/update"							, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_UPGRADE).c_str());
    ASSERT_STREQ("/tmp/ncompass/sys/SentinelData"					, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/default/lcdmedia"						, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_LCDMEDIA).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/custom/templcdmedia"					, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_LCDMEDIATEMP).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/default/compactmedia"					, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_COMPACTMEDIA).c_str());
    ASSERT_STREQ("/tmp/ncompass/sys/ErrorFile"			, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ERROR_FILE).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/custom/plcontent"			, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_PLCONTENT).c_str());
    ASSERT_STREQ("/tmp/ncompass/media/custom/tempplcontent"		, CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_PLCONTENTTEMP).c_str());
	ASSERT_STREQ("/tmp/ncompass/etc/certs", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_DEFAULT_CERTIFICATE_STORE).c_str());
	ASSERT_STREQ("/tmp/ncompass/media/custom/certs", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORE).c_str());
	ASSERT_STREQ("/tmp/ncompass/media/custom/certs_temp", CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_ADDITIONAL_CERTIFICATE_STORETEMP).c_str());

}

/// <summary>
/// Code coverage and functional test for:
///     CDirectoryManager::FullSetup()
/// Test duration of a full setup to ensure minimum required duration.
/// </summary>
TEST_F(DirectoryManagerUnitTests, DirectoryManager_FullSetupSleep)
{
    ASSERT_TRUE(CDirectoryManager::WAIT_FOR_DRIVE_TIMEOUT == 0) << _T("WAIT_FOR_DRIVE_TIMEOUT");
    ASSERT_TRUE(CDirectoryManager::WAITFORDRIVEDELAY == 0) << _T("WAITFORDRIVEDELAY");

    // Construct a CDirectoryManager instance and perform a full setup.
    DWORD ticksStart(CUtilities::GetTickCount());
    CDirectoryManager directoryManager;
    directoryManager.FullSetup();
    DWORD ticksEnd(CUtilities::GetTickCount());

    DWORD ticksElapsed = ticksEnd - ticksStart;
    ASSERT_TRUE(ticksElapsed >= CDirectoryManager::WAITFORDRIVEDELAY) << _T("ticksElapsed");
}

TEST_F(DirectoryManagerUnitTests, DirectoryManager_MockSetPath)
{
    // Init CDirectoryManager, prevent full setup
    CDirectoryManager::MockInstance(NULL);
    string contentPath = "/tmp/ncompass/media/custom/plcontent";

    CDirectoryManager::MockSetPath(DIRECTORY_PLCONTENT,"", "/tmp/ncompass/media/custom/plcontent");

    ASSERT_STREQ(contentPath.c_str(), CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_PLCONTENT).c_str());
}

TEST_F(DirectoryManagerUnitTests, DirectoryManager_PollerBadMessageDiagnosticsTextFilePath)
{
    ASSERT_STREQ("/opt/ncompass/sys/ErrorFile/Error.txt", CDirectoryManager::PollerBadMessageDiagnosticsTextFilePath().c_str());
}

TEST_F(DirectoryManagerUnitTests, DirectoryManager_PollerBadMessageDiagnosticsLogFilePath)
{
    ASSERT_EQ("/opt/ncompass/sys/ErrorFile/BADPOLLERDATA.sdf", CDirectoryManager::PollerBadMessageDiagnosticsLogFilePath());
}

TEST_F(DirectoryManagerUnitTests, GetShellXmlPath)
{
    GetShellXmlPath();
}

