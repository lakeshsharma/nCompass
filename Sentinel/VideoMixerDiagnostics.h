
#pragma once


class VideoMixerDiagnostics
{

public:
    VideoMixerDiagnostics();

    ~VideoMixerDiagnostics();

    /// <summary> Dumps the video mixer diagnostics to a file on the
    /// data drive. 
	/// </summary>
    /// <returns> True if successful. 
	/// </returns> 
    bool DumpVideoMixerDiagnostics();

    /// <summary> Checks if there is an external video mixer 
    /// diagnostics file. If present dumps disgnostics to the file. 
	/// </summary>
    /// <returns> True if external diagnostics file processed. 
	/// </returns> 
    bool CheckForExternalVideoMixerDiagnosticsFile();

protected:
    // Mockable functions
    virtual DWORD GetTickCount();
    virtual bool FileExists(const std::string &filename);
    virtual void FlushFileSystemBuffers();
    virtual bool CopyFile(const std::string &source, const std::string &target);
    virtual bool IsTFTMode();

private:
    bool m_initialized;
    DWORD m_externalFileCheckTick;
    bool m_externalFileProcessed;

    bool IsExternalVideoMixerDiagnosticsFilePresent(std::string &externalFilePath);
};
