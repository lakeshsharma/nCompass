
#include "BatteryTestParts.h"

using namespace std;


S7_Result BatteryTestParts::initialize()
{
	char eMessage[57];
	S7_Result result = -1;

	if ((result = S7LITE_DLL_Init()) != 0)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_Init returned [%d]\n", result);
		throw std::runtime_error(eMessage);
	}
	return result;
}

std::string BatteryTestParts::RunTest()
{
	char eMessage[57], tempBuffer[80];
	BOOLEAN Status = false;
	S7_Result bt_result;
	USHORT voltage;
	std::string returnValue = "ABORTED";

	bt_result = S7LITE_Battery_GetStatus(&Status);
	bt_result = S7LITE_Battery_GetVoltage(&voltage);
	if (bt_result != 0) 
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_Battery_GetStatus returned [%d]\n", bt_result);
		throw std::runtime_error(eMessage);
	}
	if (Status)
	{
		snprintf(tempBuffer, sizeof(tempBuffer), "OK (%d)", voltage);
	} else {
		snprintf(tempBuffer, sizeof(tempBuffer), "LOW (%d)", voltage);
	}

	returnValue = string(tempBuffer);
	return returnValue;
}

S7_Result BatteryTestParts::finalize()
{
	char eMessage[57];
	S7_Result result = -1;
	if ((result = S7LITE_DLL_Init()) != 0)
	{
		snprintf(eMessage, sizeof(eMessage), "S7LITE_DLL_DeInit returned [%d]\n", result);
		throw std::runtime_error(eMessage);
	}
	return result;
}


