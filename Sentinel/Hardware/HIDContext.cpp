#include "stdafx.h"
#include "HIDContext.h"
#include "../CardReaderLayer/CardReaderLayerAPI.h"
#include "Logging/LogString.h"

using namespace std;

CHIDContext::CHIDContext(void)
{
	m_deviceHandle = nullptr;

	m_featureReportByteLength = 0;
	m_inputReportByteLength = 0;
}

CHIDContext::~CHIDContext(void)
{
	Cleanup();
}

bool CHIDContext::IsReady()
{
	return m_deviceHandle != nullptr;
}

hid_device *CHIDContext::GetDeviceHandle()
{
	return m_deviceHandle;
}

USHORT CHIDContext::GetFeatureReportByteLength()
{
	USHORT length = 0;
	lock_guard<recursive_mutex> cs(m_cs);
	length = m_featureReportByteLength;
	return length;
}

USHORT CHIDContext::GetInputReportByteLength()
{
	USHORT length = 0;
	lock_guard<recursive_mutex> cs(m_cs);
	length = m_inputReportByteLength;
	return length;
}

bool CHIDContext::FindDevice(USHORT vendor, USHORT product)
{
	Cleanup();

	lock_guard<recursive_mutex> cs(m_cs);

	m_deviceHandle = CardReaderLayerFindDevice(vendor, product);

	if (nullptr == m_deviceHandle)
	{
		LogString( ERROR_LOG, "MCR Error: Found looking for device with vendor = %x and product = %x.", vendor, product);
		Cleanup();
	}

	return nullptr != m_deviceHandle;
}

void CHIDContext::Cleanup(void)
{
	lock_guard<recursive_mutex> cs(m_cs);
	if (nullptr != m_deviceHandle)
	{
		CardReaderLayerCloseDevice(m_deviceHandle);
		m_deviceHandle = nullptr;
	}

	m_featureReportByteLength = 0;
	m_inputReportByteLength = 0;
}

