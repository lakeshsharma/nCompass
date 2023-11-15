#pragma once
#if 000

class CMockDevice
{
	static const LPCWSTR DEVICEEXT;
	static const LPCWSTR READEXT;
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CMockDevice(void);

	/// <summary>
	/// Deconstructor.
	/// </summary>
	~CMockDevice(void);

	/// <summary>
	/// Returns if a mock connection has been established for the device.
	/// </summary>
	/// <returns>'true' if 'connected'.</returns>
	bool IsConnected() const { return INVALID_HANDLE_VALUE != m_deviceHandle; }

	/// <summary>
	/// Returns a mock device handle for the device.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE GetDeviceHandle() const { return m_deviceHandle; }

	/// <summary>
	/// Returns a mock read handle for the device.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE GetReadHandle() const { return  m_readHandle; }

	/// <summary>
	/// Returns the mock handle of the device handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE CloneDeviceHandle();

	/// <summary>
	/// Returns the mock handle of the device handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE CloneReadHandle();

	/// <summary>
	/// Mocks the device connection by creating temorory files to generate mock handles.
	/// </summary>
	void Connect(LPCWSTR mockFile);

	/// <summary>
	/// Mocks the device diconnect by closing and deleting temorory files.
	/// </summary>
	void Disconnect();

private:
	CString m_deviceFile;
	CString m_readFile;
	HANDLE m_deviceHandle;
	HANDLE m_readHandle;
	HANDLE m_deviceClone;
	HANDLE m_readClone;
};
#endif
