#include "stdafx.h"
#include "CardReaderLayerAPI.h"
#include "CardReaderSimulator.h"

const LPCWSTR UIC_DEVICE_PATH = L"UIC";
const LPCWSTR XST_DEVICE_PATH = L"XST";

const USHORT UIC_FEATURE_SIZE = 50;
const USHORT UIC_INPUT_SIZE = 75;
const USHORT XST_FEATURE_SIZE = 100;
const USHORT XST_INPUT_SIZE = 125;

HANDLE CARDREADERLAYER_API CardReaderLayerFindDevice(USHORT vendor, USHORT product, LPWSTR devicePath, ULONG& pathLength, LPDWORD lastError)
{
	HANDLE deviceHandle = INVALID_HANDLE_VALUE;
	DWORD error = ERROR_NOT_SUPPORTED;

	if (CCardReaderSimulator::Instance().GetConnected(vendor))
	{
		error = ERROR_SUCCESS;
		switch (product)
		{
			case USB_PRODUCT_ID_BADLENGTHS:
			case USB_PRODUCT_ID_BADREADHANDLE:
			// Whatever the error we still setup a UIC devcie. We just need to know what error to set.
				switch (product)
				{
					case USB_PRODUCT_ID_BADLENGTHS:
						CCardReaderSimulator::Instance().SetTestError(ERROR_BAD_LENGTH);
						break;
					case USB_PRODUCT_ID_BADREADHANDLE:
						CCardReaderSimulator::Instance().SetTestError(ERROR_READ_FAULT);
						break;
				}
			case USB_PRODUCT_ID_UIC_CRDRDR1:
			case USB_PRODUCT_ID_UIC_CRDRDR2:
			case USB_PRODUCT_ID_UIC_CRDRDR3:
			{
				ULONG length = lstrlen(UIC_DEVICE_PATH);
				if (length > pathLength)
				{
					error = ERROR_INSUFFICIENT_BUFFER;
				}
				else
				{
					for (ULONG n = 0; n < length; n++)
					{
						devicePath[n] = UIC_DEVICE_PATH[n];
					}
					devicePath[length] = 0;
					deviceHandle = CCardReaderSimulator::Instance().CloneUicDeviceHandle();
				}
				pathLength = length;
			}
				break;
			case USB_PRODUCT_ID_XST_CRDRDR1:
			{
				ULONG length = lstrlen(XST_DEVICE_PATH);
				if (length > pathLength)
				{
					error = ERROR_INSUFFICIENT_BUFFER;
				}
				else
				{
					for (ULONG n = 0; n < length; n++)
					{
						devicePath[n] = XST_DEVICE_PATH[n];
					}
					devicePath[length] = 0;
					deviceHandle = CCardReaderSimulator::Instance().CloneXstDeviceHandle();
				}
				pathLength = length;
			}
				break;
			case USB_PRODUCT_ID_LARGEDEVICEPATH:
			{
				ULONG length = MAX_PATH + 1;
				if (length > pathLength)
				{
					error = ERROR_INSUFFICIENT_BUFFER;
				}
				else
				{
					ULONG uicLen = lstrlen(UIC_DEVICE_PATH);
					for (ULONG n = 0; n < uicLen; n++)
					{
						devicePath[n] = UIC_DEVICE_PATH[n];
					}
					deviceHandle = CCardReaderSimulator::Instance().CloneUicDeviceHandle();
				}
				pathLength = length;
			}
				break;
		}
	}

	if (NULL != lastError)
	{
		*lastError = error;
	}
	return deviceHandle;
}

HANDLE CARDREADERLAYER_API CardReaderLayerGetReadHandle(LPCWSTR devicePath, LPDWORD lastError)
{
	HANDLE readHandle = INVALID_HANDLE_VALUE;
	DWORD error = ERROR_NOT_SUPPORTED;

	CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
	if (0 == lstrcmp(UIC_DEVICE_PATH, devicePath))
	{
		if (simulator.GetTestError() == ERROR_READ_FAULT)
		{
			error = ERROR_READ_FAULT;
		}
		else
		{
			readHandle = simulator.CloneUicReadHandle();
			error = ERROR_SUCCESS;
		}
	}
	else if (0 == lstrcmp(XST_DEVICE_PATH, devicePath))
	{
		readHandle = simulator.CloneXstReadHandle();
		error = ERROR_SUCCESS;
	}

	if (NULL != lastError)
	{
		*lastError = error;
	}
	return readHandle;
}

DWORD CARDREADERLAYER_API CardReaderLayerGetReportByteLengths(HANDLE deviceHandle, USHORT& featureReportByteLength, USHORT& inputReportByteLength)
{
	DWORD error = ERROR_NOT_SUPPORTED;

	featureReportByteLength = 0;
	inputReportByteLength = 0;

	if (INVALID_HANDLE_VALUE != deviceHandle)
	{
		CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
		if (simulator.GetUicDeviceHandle() == deviceHandle)
		{
			if (simulator.GetTestError() == ERROR_BAD_LENGTH)
			{
				error = ERROR_BAD_LENGTH;
				featureReportByteLength = 0;
				inputReportByteLength = 0;
			}
			else
			{
				error = ERROR_SUCCESS;
				featureReportByteLength = UIC_FEATURE_SIZE;
				inputReportByteLength = UIC_INPUT_SIZE;
			}
		}
		else if (simulator.GetXstDeviceHandle() == deviceHandle)
		{
			error = ERROR_SUCCESS;
			featureReportByteLength = XST_FEATURE_SIZE;
			inputReportByteLength = XST_INPUT_SIZE;
		}
	}

	return error;
}

DWORD CARDREADERLAYER_API CardReaderLayerSetFeature(HANDLE deviceHandle, PUCHAR reportBuf, USHORT bufferLength)
{
	DWORD error = ERROR_NOT_SUPPORTED;

	CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();
	if (simulator.GetUicDeviceHandle() == deviceHandle || simulator.GetXstDeviceHandle() == deviceHandle)
	{
		if (0 < bufferLength) // bufferLength needs to be at least 1 before we can get the Feature Command
		{
			error = ERROR_SUCCESS;

			DWORD ticks = simulator.GetFeatureErrorTickCount(reportBuf[0]);
			if (0 != ticks)
			{
				if (INFINITE == ticks || GetTickCount() < ticks)
				{
					error = simulator.GetFeatureError(reportBuf[0]);
				}
				else if (INFINITE != ticks)
				{
					// Clear error
					simulator.SetFeatureError(0, ERROR_SUCCESS);
				}
			}

			if (ERROR_SUCCESS == error)
			{
				switch (reportBuf[0])
				{
					case HID_CARDRDR_CMD_ENABLE:
						simulator.SetEnabled(true);
						error = ERROR_SUCCESS;
						break;
					case HID_CARDRDR_CMD_DISABLE:
						simulator.SetEnabled(false);
						error = ERROR_SUCCESS;
						break;
					case HID_CARDRDR_CMD_LIGHT_CONTROL:
						if (HID_CARDRDR_COLOR_ARRAY_SIZE > bufferLength - 1)
						{
							error = ERROR_INSUFFICIENT_BUFFER;
						}
						else
						{
							error = simulator.SetColor(&reportBuf[1], HID_CARDRDR_COLOR_ARRAY_SIZE);
						}
					case HID_CARDRDR_CMD_READ_CARD_DATA:
						simulator.SetReadCardData(true);
						error = ERROR_SUCCESS;
						break;
					case HID_CARDRDR_CMD_CLEAR_BUFFER:
						simulator.ClearCardData();
						error = ERROR_SUCCESS;
						break;
				}
			}
		}
		else
		{
			error = ERROR_INSUFFICIENT_BUFFER;
		}
	}

	return error;
}

DWORD CARDREADERLAYER_API CardReaderLayerGetInputReport(HANDLE readHandle, PUCHAR reportBuf, USHORT bufferLength, LPDWORD bytes, LPOVERLAPPED olap)
{
	DWORD error = ERROR_SUCCESS;

	CCardReaderSimulator& simulator = CCardReaderSimulator::Instance();

	DWORD ticks = simulator.GetReportErrorTickCount();
	if (INFINITE == ticks || GetTickCount() < ticks)
	{
		error = simulator.GetReportError();
	}
	else if (INFINITE != ticks)
	{
		// Clear error
		simulator.SetReportError(ERROR_SUCCESS);
		if (ERROR_IO_PENDING == error)
		{
			SetEvent(olap->hEvent);
		}
	}

	if (ERROR_SUCCESS != error)
	{
		*bytes = 0;
	}
	else if (!simulator.GetConnected(USB_VENDOR_ID_UIC) && !simulator.GetConnected(USB_VENDOR_ID_XST))
	{
		error = ERROR_NOT_CONNECTED;
	}
	else if (simulator.GetUicReadHandle() == readHandle || simulator.GetXstReadHandle() == readHandle)
	{
		if (HID_CARDRDR_DATA_REPORT_SIZE < bufferLength)
		{
			error = ERROR_INSUFFICIENT_BUFFER;
		}
		else if (simulator.GetReadCardData())
		{
			error = simulator.GetCardData(reportBuf, bytes);
			simulator.SetReadCardData(false);
		}
	}

	return error;
}