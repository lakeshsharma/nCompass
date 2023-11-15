
#include "stdafx.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Utilities.h"
#include "VideoMixerDiagnostics.h"

using ::testing::_;
using ::testing::Return;

// Unit testing includes.
#include "AssertMsg.h"


class VideoMixerDiagnosticsMock : public VideoMixerDiagnostics
{
public:

	MOCK_METHOD0(GetTickCount, DWORD());
    MOCK_METHOD0(FlushFileSystemBuffers, void());
	MOCK_METHOD2(CopyFile, bool(const std::string &, const std::string &));
    MOCK_METHOD1(FileExists, bool( const std::string &));
    MOCK_METHOD0(IsTFTMode, bool());


private:
};

using namespace std;

TEST(VideoMixerDiagnosticsTests, CheckForExternalVideoMixerDiagnosticsFileTest)
{
	VideoMixerDiagnosticsMock vmDiag;

	ON_CALL(vmDiag, GetTickCount()).WillByDefault(Return(1000));
	ASSERT_FALSE(vmDiag.CheckForExternalVideoMixerDiagnosticsFile()) << L"CheckForExternalVideoMixerDiagnosticsFile";

	ON_CALL(vmDiag, GetTickCount()).WillByDefault(Return(11000));
	ON_CALL(vmDiag, FileExists(_)).WillByDefault(Return(false));
	ASSERT_FALSE(vmDiag.CheckForExternalVideoMixerDiagnosticsFile()) << L"CheckForExternalVideoMixerDiagnosticsFile";

	ON_CALL(vmDiag, GetTickCount()).WillByDefault(Return(22000));
	ON_CALL(vmDiag, FileExists(_)).WillByDefault(Return(true));
	ON_CALL(vmDiag, CopyFile(_,_)).WillByDefault(Return(true));
    ON_CALL(vmDiag, IsTFTMode()).WillByDefault(Return(false));
	ASSERT_TRUE(vmDiag.CheckForExternalVideoMixerDiagnosticsFile()) << L"CheckForExternalVideoMixerDiagnosticsFile";

    ON_CALL(vmDiag, GetTickCount()).WillByDefault(Return(22000));
	ON_CALL(vmDiag, FileExists(_)).WillByDefault(Return(true));
	ON_CALL(vmDiag, CopyFile(_,_)).WillByDefault(Return(true));
    ON_CALL(vmDiag, IsTFTMode()).WillByDefault(Return(true));
	ASSERT_FALSE(vmDiag.CheckForExternalVideoMixerDiagnosticsFile()) << L"CheckForExternalVideoMixerDiagnosticsFile";
}
