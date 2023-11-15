#pragma once

class CAudioInfo
{
public:
	CAudioInfo(void);
	CAudioInfo(CAudioInfo &iRHS);
public:
	CAudioInfo &operator=( CAudioInfo &iRHS );
	~CAudioInfo(void);

	std::string GetAudioFilename() const;	
	void SetAudioFilename(const std::string& lpszAudio);

private:
	std::string m_AudioName;
};
