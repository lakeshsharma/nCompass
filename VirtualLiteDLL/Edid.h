#pragma once

class CEdid
{
public:
	CEdid();

	bool Updated() const { return m_updated; }
	bool StandardTiming() const;

	S7_Result S7Read(PUCHAR buf, UINT& len);

	bool Update();

	UCHAR& operator[](int n)       { return m_edid[n]; }
	UCHAR  operator[](int n) const { return m_edid[n]; }

protected:
	static const UINT EDID_LENGTH = 128;

	const UCHAR* Data() const { return m_edid; }

	UCHAR m_edid[EDID_LENGTH];
	bool m_updated;

private:
	static const UCHAR EDID_DEFAULT_DATA = 0xee; // So memory not updated is more noticeable.
	static const DWORD STD = 0x26;
	static const DWORD DTD = 0x36;
};
