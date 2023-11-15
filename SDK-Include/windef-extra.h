#pragma once

// a few bits missing from "windef.h" that comes from s7lite

inline void memcpy_s(void *__dest, size_t __limit, const void *__src, size_t __n) { memcpy(__dest, __src, __n > __limit ? __limit : __n); }

#ifndef FillMemory
#define FillMemory(Destination, Length, Fill) memset((Destination), (Fill), (Length))
#endif

typedef DWORD COLORREF;

typedef int SOCKET;
#define SOCKET_SUCCESS 0 // Aristocrat defintion, not Windows.
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#define ERROR_SUCCESS 0
#define MAX_PATH PATH_MAX

#define _tcscmp strcmp

typedef union {
	struct {
		DWORD LowPart;
		DWORD HighPart;
	};
	ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

// _countof().
template <typename T, std::size_t N>
constexpr std::size_t _countof(T const (&)[N]) noexcept
{
	return N;
}

struct SIZE
{
    long        cx;
    long        cy;
	
	SIZE()
	{
		cx = 0;
		cy = 0;
	}
	
	SIZE(long cx1, long cy1)
	{
		cx = cx1;
		cy = cy1;
	}

	inline bool operator==(SIZE& rhs)const { return ((this->cx==rhs.cx) && (this->cy==rhs.cy)); }
};

struct POINT
{
    long  x;
    long  y;

	POINT()
	{
		x = 0;
		y = 0;
	}
	
	POINT(long x1, long y1)
	{
		x = x1;
		y = y1;
	}

	inline bool operator==(POINT& rhs)const { return ((this->x==rhs.x) && (this->y==rhs.y)); }
};

struct RECT
{
	long    left;
	long    top;
	long    right;
	long    bottom;

	void SetRectEmpty()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	RECT()
	{
		SetRectEmpty();
	}

	RECT(long l, long t, long r, long b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	
	RECT(POINT p, SIZE s)
	{
		left = p.x;
		top = p.y;
		right = left + s.cx;
		bottom = top + s.cy;
	}
	
	void MoveToXY(long x, long y)
	{
		right = x + right - left;
		bottom = y + bottom - top;
		left = x;
		top = y;
	}
	
	void SetSize(SIZE s)
	{
		right = left + s.cx;
		bottom = top + s.cy;
	}
	
	long Width()
	{
		return right - left;
	}
	
	long Height()
	{
		return bottom - top;
	}
	bool operator==( const RECT &rhs ) const
	{
		bool retVal = true;
		
		if ((left != rhs.left) && (right != rhs.right) && (top != rhs.top) && (bottom != rhs.bottom))
			retVal = false;
	
		return retVal;
	}
	
};
