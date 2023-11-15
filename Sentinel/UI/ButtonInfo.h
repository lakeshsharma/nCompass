#pragma once

class CButtonInfo
{
public:
	CButtonInfo(void);
	CButtonInfo(CButtonInfo &bRHS);
	~CButtonInfo(void);
	CButtonInfo &operator=( CButtonInfo &bRHS );

	void SetButtonID(int btnID);
	int	GetButtonID();
	void SetLocation(const RECT& rLocation);
	RECT GetLocation() const;
	std::string GetUpButtonFilename() const;
	std::string GetDownButtonFilename() const;
	void SetImages(const std::string& lpszButtonUp, const std::string& lpszButtonDown);

private:
	int	m_btnID;
	RECT m_rLocation;
	std::string m_strUpButton;
	std::string m_strDownButton;

};
