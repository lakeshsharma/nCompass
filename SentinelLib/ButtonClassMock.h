#pragma once

#include "gmock/gmock.h"
#include "UI\ButtonClass.h"

using namespace std;//TODO Stop using namespaces in headers.  Or does MOCK_METHOD2 require using namespace std?

class CButtonClassMock : public CButtonClass
{
public:
	CButtonClassMock(CButtonInfo *buttonInfo);
	~CButtonClassMock();

	MOCK_METHOD2(InvalidateRect, void(CRect rect, BOOL bErase));
};

