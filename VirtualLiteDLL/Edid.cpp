#include	"stdafx.h"
#include	"Edid.h"

CEdid::CEdid()
	:
	m_updated(false)
{
	memset(m_edid, EDID_DEFAULT_DATA, sizeof(m_edid));
}

bool CEdid::StandardTiming() const
{
	return ((m_edid[DTD+0]&0xFF) == 0 && (m_edid[DTD+1]&0xFF) == 0 && (m_edid[DTD+2]&0xFF) == 0);
}

bool CEdid::Update()
{
	// Make standard timing
	memset(m_edid, 0, sizeof(m_edid));

	const double aspect_ratio[] = { 0.0, 0.75, 0.8, 0.5625 };

	// Make 1280 x 1024 @ 60
	USHORT horizontal = 1280;
	USHORT refreshRate = 60;
	CHAR aspect_i = 2;

	m_edid[STD] = ((horizontal >> 3) - 31) & 0xFF;
	m_edid[STD+1] = ((refreshRate - 60) & 0x3F) + ((aspect_i << 6) & 0xFF);

	m_updated = true;
	return m_updated;
}

S7_Result CEdid::S7Read(PUCHAR buf, UINT& len)
{
	S7_Result result = S7DLL_STATUS_ERROR;

	if (Updated())
	{
		memcpy(buf, m_edid, sizeof(m_edid));
		len = _countof(m_edid);
		result = S7DLL_STATUS_OK;
	}

	return result;
}
