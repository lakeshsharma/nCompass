#pragma

#include "afxmt.h"
#include <queue>
#include "CardReaderLayerAPI.h"
#include "MockDevice.h"
#include "DataReport.h"

#define USB_INVALID_VENDOR_ID  0xFFFF
#define USB_INVALID_PRODUCT_ID 0xFFFF

#define USB_VENDOR_ID_TEST             0xFFF0
#define USB_PRODUCT_ID_LARGEDEVICEPATH 0xFFF1
#define USB_PRODUCT_ID_BADLENGTHS      0xFFF2
#define USB_PRODUCT_ID_BADREADHANDLE   0xFFF3

// Singleton
class CCardReaderSimulator
{
	static const LPCWSTR UIC;
	static const LPCWSTR XST;
	static const LPCWSTR TEST;
public:
	/// <summary>
	/// Call to access and create if needed singleton.
	/// </summary>
	static CCardReaderSimulator& Instance();

	/// <summary>
	/// Delete singleton and make sure 'simulation' has ended.
	/// </summary>
	static void CleanupInstance();

	bool IsEnabled() const { return m_enabled; }

	/// <summary>
	/// Returns a mock device handle for the UIC device.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE GetUicDeviceHandle() const { return m_uicDevice.GetDeviceHandle(); }

	/// <summary>
	/// Returns a mock read handle for the UIC device.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE GetUicReadHandle() const { return m_uicDevice.GetReadHandle(); }

	/// <summary>
	/// Returns a mock device handle for the XST device.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE GetXstDeviceHandle() const { return m_xstDevice.GetDeviceHandle(); }

	/// <summary>
	/// Returns a mock read handle for the XST device.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE GetXstReadHandle() const { return m_xstDevice.GetReadHandle(); }

	/// <summary>
	/// Returns the mock handle of the UIC device handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE CloneUicDeviceHandle() { return m_uicDevice.CloneDeviceHandle(); }

	/// <summary>
	/// Returns the mock handle of the UIC read handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE CloneUicReadHandle() { return m_uicDevice.CloneReadHandle(); }

	/// <summary>
	/// Returns the mock handle of the XST device handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Device handle.</returns>
	HANDLE CloneXstDeviceHandle() { return m_xstDevice.CloneDeviceHandle(); }

	/// <summary>
	/// Returns the mock handle of the XST read handle and generates a second so the consumer and the object
	/// can do a CloseHandle.
	/// </summary>
	/// <returns>Read handle.</returns>
	HANDLE CloneXstReadHandle() { return m_xstDevice.CloneReadHandle(); }

	///	<summary>
	///	Returns the color at an offset.
	///	</summary>
	///	<param name="n">The offset.</param>
	///	<returns>The color at an offset.</returns>
	///	<remarks>
	/// This does no range checking.
	///	</remarks>
	UCHAR GetColorItem(unsigned n) const { return m_colorArray[n]; }

	///	<summary>
	///	Returns a pointer to the color array.
	///	</summary>
	///	<returns>A pointer to the color array.</returns>
	const UCHAR* GetColors() const { return m_colorArray; }

	/// <summary>
	/// Starts a card reader "simulation" including creating a temporary directory to hold files for mocked handles.
	/// </summary>
	/// <param name="testDir">IN - test directory to create and use.</param>
	void Start(LPCWSTR testDir);

	/// <summary>
	/// Returns if a mock connection has been established for the given device.
	/// </summary>
	/// <param name="vendor">IN - vendor id of the device being mocked.</param>
	/// <returns>'true' if 'connected'.</returns>
	bool GetConnected(USHORT vendor);

	/// <summary>
	/// Opens and closes a mock connection for given device.
	/// </summary>
	/// <param name="vendor">IN - vendor id of the device being mocked.</param>
	/// <param name="flag">IN - whether to open or close the connection.</param>
	void SetConnected(USHORT vendor, bool flag);

	/// <summary>
	/// Pushes a mock Card Inserted Event onto the queue.
	/// </summary>
	/// <param name="trackOne">IN - is the first or second track being used.</param>
	void PushEvtStatusInserted(bool trackOne);

	/// <summary>
	/// Pushes a mock Card Removed Event onto the queue.
	/// </summary>
	void PushEvtStatusRemoved();

	/// <summary>
	/// Pushes a mock Card Data Event onto the queue.
	/// </summary>
	/// <param name="dataBuf">IN - card data being pushed.</param>
	/// <param name="dataLen">IN - length of data.</param>
	/// <param name="firstTrack">IN - is the first or second track being used.</param>
	void PushEvtCardData(PUCHAR dataBuf, UCHAR dataLen, bool firstTrack);

	/// <summary>
	/// Pushes a mock error onto the queue.
	/// </summary>
	/// <param name="dataValue">IN - error pushed.</param>
	void PushEvtErrorData(UCHAR dataValue);

	/// <summary>
	/// Ends a card reader "simulation" including removing a temporary directory created by 'Start'.
	/// </summary>
	void End();

	/// <summary>
	/// Set an error to be return by the next given feature request for testing.
	/// </summary>
	/// <param name="feature">IN - feature to error.</param>
	/// <param name="error">IN - error to be returned.</param>
	/// <param name="tickCount">IN - How long too return the error.</param>
	void SetFeatureError(UCHAR feature, DWORD error, DWORD tickCount = INFINITE);

	/// <summary>
	/// Set an error to be return by the next request of report datafor testing.
	/// </summary>
	/// <param name="error">IN - error to be returned.</param>
	/// <param name="tickCount">IN - How long too return the error.</param>
	void SetReportError(DWORD error, DWORD tickCount = INFINITE);

protected:

	friend HANDLE CARDREADERLAYER_API CardReaderLayerFindDevice(USHORT vendor, USHORT product, LPWSTR devicePath, ULONG& pathLength, LPDWORD lastError);
	friend HANDLE CARDREADERLAYER_API CardReaderLayerGetReadHandle(LPCWSTR devicePath, LPDWORD lastError);
	friend DWORD CARDREADERLAYER_API CardReaderLayerGetReportByteLengths(HANDLE deviceHandle, USHORT& featureReportByteLength, USHORT& inputReportByteLength);
	friend DWORD CARDREADERLAYER_API CardReaderLayerSetFeature(HANDLE deviceHandle, PUCHAR reportBuf, USHORT bufferLength);
	friend DWORD CARDREADERLAYER_API CardReaderLayerGetInputReport(HANDLE readHandle, PUCHAR reportBuf, USHORT bufferLength, LPDWORD bytes, LPOVERLAPPED olap);

	DWORD GetTestError() const { return m_testError; }
	void SetTestError(DWORD error) { m_testError = error; }

	DWORD GetFeatureError(UCHAR feature) const;
	DWORD GetFeatureErrorTickCount(UCHAR feature) const;
	DWORD GetReportError() const { return m_reportError; }
	DWORD GetReportErrorTickCount() const { return m_reportErrorTicks; }

	void SetEnabled(bool flag);

	bool GetReadCardData() const { return m_readCardData; }
	void SetReadCardData(bool flag) { m_readCardData = flag; }

	DWORD SetColor(const UCHAR* colorArray, unsigned length, unsigned start = 0);

	DWORD GetCardData(PUCHAR reportBuf, LPDWORD bytes);
	void ClearCardData();

private:
	static CCardReaderSimulator* m_instance;
	static ::CCriticalSection m_cs;

	CCardReaderSimulator(void);
	virtual ~CCardReaderSimulator(void);

	void Clear();

	CString m_testDir;
	bool m_enabled;
	bool m_readCardData;
	UCHAR m_colorArray[HID_CARDRDR_COLOR_ARRAY_SIZE];

	std::queue<CDataReport> m_dataReports;

	CMockDevice m_uicDevice;
	CMockDevice m_xstDevice;

	DWORD m_testError;

	UCHAR m_featureToError;
	DWORD m_featureError;
	DWORD m_featureErrorTicks;

	DWORD m_reportError;
	DWORD m_reportErrorTicks;
};

