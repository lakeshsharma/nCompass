#include "stdafx.h"

#include "CheckSum.h"


CCheckSum::CCheckSum(bool simple) 
{ 
	Clear(); 
	m_simple = simple;
}

void CCheckSum::Add(DWORD value)
{
	union { DWORD value; BYTE bytes[4]; } data;
	data.value = value;
	for(UINT i = 0; i < sizeof(data.bytes); i++)
		Add(data.bytes[i]);
}


void CCheckSum::Add(WORD value)
{
	union { DWORD value; BYTE bytes[2]; } data;
	data.value = value;
	for(UINT i = 0; i < sizeof(data.bytes); i++)
		Add(data.bytes[i]);
}


void CCheckSum::Add(BYTE value)
{
	if (m_simple)
	{
		sum += value;
	}
	else
	{
		BYTE cipher = (value ^ (r >> 8));
		r = (cipher + r) * c1 + c2;
		sum += cipher;
	}
}


void CCheckSum::Add(const std::string & s)
{
	for(size_t i = 0; i < s.size(); i++)
		Add((BYTE)s.at(i));
}


void CCheckSum::Add(LPBYTE b, UINT length)
{
	for(UINT i = 0; i < length; i++)
		Add(b[i]);
}
