#include "stdafx.h"
#include "CardReaderLayerAPI.h"
#include "HidCommon.h"

using namespace std;

bool CardReaderLayerSetFeature(hid_device *deviceHandle, PUCHAR reportBuf, USHORT bufferLength)
{
	int result = -1;

	if (deviceHandle != nullptr)
	{
		result = hid_send_feature_report(deviceHandle, reportBuf, bufferLength);
	}

	return result != -1;
}

bool CardReaderLayerGetInputReport(hid_device *readHandle, PUCHAR reportBuf, USHORT bufferLength, int &bytes)
{
	int bytesRead = -1;
	
	try
	{
		if (readHandle != nullptr)
		{
			bytesRead = hid_read(readHandle, reportBuf, bufferLength);
		}
	}
	catch (...)
	{
		bytesRead = 0;
	}
	
	bytes = bytesRead;

	return bytesRead != 1;
}

hid_device *CardReaderLayerFindDevice(USHORT vendor, USHORT product)
{
	hid_device *deviceHandle = hid_open(vendor, product, nullptr);
	if (deviceHandle != nullptr)
	{
		hid_set_nonblocking(deviceHandle, 1);
	}
	return deviceHandle;
}

void CardReaderLayerCloseDevice(hid_device *device)
{
	if (device != nullptr)
	{
		hid_close(device);
	}
}
