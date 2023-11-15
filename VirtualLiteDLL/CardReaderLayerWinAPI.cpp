#include "stdafx.h"
#include "CardReaderLayerAPI.h"
#include "HidCommon.h"

using namespace std;

bool CardReaderLayerSetFeature(hid_device *deviceHandle, PUCHAR reportBuf, USHORT bufferLength)
{
	return false;
}

bool CardReaderLayerGetInputReport(hid_device *readHandle, PUCHAR reportBuf, USHORT bufferLength, int &bytes)
{
	return false;
}

hid_device *CardReaderLayerFindDevice(USHORT vendor, USHORT product)
{
	return nullptr;
}

void CardReaderLayerCloseDevice(hid_device *device)
{
}
