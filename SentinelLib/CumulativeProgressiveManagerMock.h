#pragma once

#include "gmock/gmock.h"
#include "ProgressiveProtocol/ICumulativeProgressiveManager.h"

using namespace std;//TODO Stop using namespaces in headers.  Or does MOCK_METHOD2 require using namespace std?

class CumulativeProgressiveManagerMock : public ICumulativeProgressiveManager
{
private:
public:

	MOCK_METHOD0(HasCumulativeMeterAlready, bool());
	MOCK_METHOD0(ClearHasCumulativeMeterStatus, void());
	MOCK_METHOD1(GetCumulativeDelta, __int64(__int64));
};

