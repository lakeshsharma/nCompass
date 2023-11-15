#include "stdafx.h"
#include "MockDevice.h"
#if 000
const LPCWSTR CMockDevice::DEVICEEXT = L".fakedevice";
const LPCWSTR CMockDevice::READEXT = L".fakeread";

CMockDevice::CMockDevice(void)
	: m_deviceHandle(INVALID_HANDLE_VALUE), m_readHandle(INVALID_HANDLE_VALUE), m_deviceClone(INVALID_HANDLE_VALUE), m_readClone(INVALID_HANDLE_VALUE)
{
}

CMockDevice::~CMockDevice(void)
{
	Disconnect();
}

void CMockDevice::Connect(LPCWSTR mockFile)
{
	if (!IsConnected())
	{
		m_deviceFile = mockFile;
		m_deviceFile += DEVICEEXT;

		m_deviceHandle = CreateFile(
			m_deviceFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_ALWAYS,
			0,
			NULL);

		m_readFile = mockFile;
		m_readFile += READEXT;

		m_readHandle = CreateFile(
			m_readFile,
			GENERIC_READ,
			FILE_SHARE_READ,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_ALWAYS,
			0,
			NULL);
	}
}

void CMockDevice::Disconnect()
{
	if (INVALID_HANDLE_VALUE != m_deviceHandle)
	{
		if (INVALID_HANDLE_VALUE == m_deviceClone)
		{
			CloseHandle(m_deviceHandle);
		}
		else
		{
			// Someone else has m_deviceHandle and needs to close it
			CloseHandle(m_deviceClone);
		}
		::DeleteFile(m_deviceFile);
		m_deviceHandle = INVALID_HANDLE_VALUE;
		m_deviceClone = INVALID_HANDLE_VALUE;
	}

	if (INVALID_HANDLE_VALUE != m_readHandle)
	{
		if (INVALID_HANDLE_VALUE == m_readClone)
		{
			CloseHandle(m_readHandle);
		}
		else
		{
			// Someone else has m_readHandle and needs to close it
			CloseHandle(m_readClone);
		}
		::DeleteFile(m_readFile);
		m_readHandle = INVALID_HANDLE_VALUE;
		m_readClone = INVALID_HANDLE_VALUE;
	}
}

HANDLE CMockDevice::CloneDeviceHandle()
{
	HANDLE handle = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE == m_deviceClone) // Can only do this once
	{
		handle = m_deviceHandle;
		DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(), &m_deviceClone, 0, FALSE, DUPLICATE_SAME_ACCESS);
	}
	return handle;
}

HANDLE CMockDevice::CloneReadHandle()
{
	HANDLE handle = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE == m_readClone) // Can only do this once
	{
		handle = m_readHandle;
		DuplicateHandle(GetCurrentProcess(), handle, GetCurrentProcess(), &m_readClone, 0, FALSE, DUPLICATE_SAME_ACCESS);
	}
	return handle;
}
#endif
