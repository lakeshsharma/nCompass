#pragma once
#include <hidapi/hidapi.h>

class CHIDContext
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	CHIDContext(void);

	/// <summary>
	/// Deconstructor.
	/// </summary>
	virtual ~CHIDContext(void);

	/// <summary>
	/// Returns whether or not a device has been found and all members set.
	/// </summary>
	/// <returns>'true' if context is ready.</returns>
	bool IsReady();

	/// <summary>
	/// Returns the device handle.
	/// </summary>
	/// <returns>device handle.</returns>
	hid_device *GetDeviceHandle();

	/// <summary>
	/// Returns bytes needed to report buffer used when getting or setting a 'feature'.
	/// </summary>
	/// <returns>size of report buffer.</returns>
	USHORT GetFeatureReportByteLength();

	/// <summary>
	/// Returns bytes needed to report buffer used when getting the 'input report' of a 'feature'
	/// </summary>
	/// <returns>size of report buffer.</returns>
	USHORT GetInputReportByteLength();

	/// <summary>
	/// Tries to find a HID device with the given 'vendor' and 'product' id attributes and then acquire handles for communications.
	/// </summary>
	/// <returns>'true' if successful.</returns>
	bool FindDevice(USHORT vendor, USHORT product);

	/// <summary>
	/// Cleans up handles and memory used by this class
	/// </summary>
	/// <remarks>
	/// Includes cleaning up handles and memory used by the HID context, so effectively sets 'IsReady' to 'false'
	/// </remarks>
	void Cleanup(void);

private:
	// Disable copy constructor (at least for now)
    
	CHIDContext(const CHIDContext& context) 
    {
        UNREFERENCED_PARAMETER(&context); 
    };

	hid_device *m_deviceHandle;

	USHORT m_featureReportByteLength;
	USHORT m_inputReportByteLength;

	std::recursive_mutex m_cs;
};


