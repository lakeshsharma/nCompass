// CheckSum.h : header file
//

#if !defined(AFX_CHECKSUM_H__C5A841DE_1906_4FB1_BE6D_E9A34C870081__INCLUDED_)
#define AFX_CHECKSUM_H__C5A841DE_1906_4FB1_BE6D_E9A34C870081__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCheckSum 
{
public:

    CCheckSum(bool simple = false);
    void Clear() { sum = 0; r = 55665; c1 = 52845; c2 = 22719;}
    void Add(DWORD w);
    void Add(BOOL w) { Add((DWORD)w); }
    void Add(WORD w);
    void Add(const std::string & s);
    void Add(LPBYTE b, UINT length);
    void Add(BYTE b);
    DWORD Get() 
	{ 
		return sum;
	}

protected:
    WORD r;
    WORD c1;
    WORD c2;
    DWORD sum;
	bool m_simple;
};


#endif // !defined(AFX_CHECKSUM_H__C5A841DE_1906_4FB1_BE6D_E9A34C870081__INCLUDED_)
