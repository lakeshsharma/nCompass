#pragma once

extern "C" {
	#include <s7lite.h>
}

#include "stdafx.h"

class BatteryTestParts
{
public:
	S7_Result initialize();
	std::string  RunTest();
	S7_Result finalize();
private:
	S7_Result results;
};


