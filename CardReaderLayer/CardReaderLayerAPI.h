#pragma once
#include <hidapi/hidapi.h>

#ifdef CARDREADERLAYER_EXPORTS
  // if we're building the DLL
  #define CARDREADERLAYER_API __declspec(dllexport)
#else
  #ifdef _LIB
    // if we're building the static library
    #define CARDREADERLAYER_API extern
  #else
    #ifndef CARDREADERLAYERLIB
      // if we're building an application
      // to link against the DLL
      #define CARDREADERLAYER_API __declspec(dllimport)
    #else
      // if we're building an application
      // to link against the static library
      #define CARDREADERLAYER_API extern
    #endif
  #endif
#endif

#define USB_VENDOR_ID_UIC          0x6352
#define USB_PRODUCT_ID_UIC_CRDRDR1 0x240A
#define USB_PRODUCT_ID_UIC_CRDRDR2 0x240B
#define USB_PRODUCT_ID_UIC_CRDRDR3 0x240C
#define USB_PRODUCT_ID_UIC_RFID    0x242A
#define USB_VENDOR_ID_XST          0x2724
#define USB_PRODUCT_ID_XST_CRDRDR1 0x6001

#define HID_CARDRDR_CMD_ENABLE  0x02
#define HID_CARDRDR_CMD_DISABLE	0x03
#define HID_CARDRDR_CMD_READ_CARD_DATA 0x5B
#define HID_CARDRDR_CMD_LIGHT_CONTROL 0x5F
#define HID_CARDRDR_CMD_CLEAR_BUFFER 0x60

#define HID_CARDRDR_COLOR_ARRAY_SIZE 13
#define HID_CARDRDR_DATA_REPORT_SIZE 1024

#define HID_CARDRDR_EVT_CARD_STATUS	0x64
#define HID_CARDRDR_EVT_CARD_DATA 0x65
#define HID_CARDRDR_EVT_ERROR_DATA 0x66

#define HID_CARDRDR_DATA_REPORT_OFFSET_EVENT 0
#define HID_CARDRDR_DATA_REPORT_OFFSET_VALUE 1

#define HID_CARDRDR_DATA_REPORT_VALUE_INSERTED 1
#define HID_CARDRDR_DATA_REPORT_VALUE_REMOVED 2
#define HID_CARDRDR_DATA_REPORT_VALUE_PARTIALlY_INSERTED 4
#define HID_CARDRDR_DATA_REPORT_VALUE_NONE 0

#define HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK_OFFSET 2
#define HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK1_AVAILABLE_FLAG 2
#define HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK2_AVAILABLE_FLAG 4
#define HID_CARDRDR_DATA_REPORT_CARD_STATUS_RFID_AVAILABLE_FLAG 16

#define HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX1 1
#define HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_INDEX2 2
#define HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_LENGTH 3
#define HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_TRACK 4
#define HID_CARDRDR_DATA_REPORT_CARD_DATA_OFFSET_DATA 5

#define HID_CARDRDR_DATA_REPORT_CARD_DATA_VALUE_TRACK1_USED 1
#define HID_CARDRDR_DATA_REPORT_CARD_DATA_VALUE_TRACK2_USED 2

#define HID_CARDRDR_EVT_LENGTH_CARD_STATUS_INSERTED (HID_CARDRDR_DATA_REPORT_CARD_STATUS_TRACK_OFFSET + 1)
#define HID_CARDRDR_EVT_LENGTH_CARD_STATUS_REMOVED (HID_CARDRDR_DATA_REPORT_OFFSET_VALUE + 1)
#define HID_CARDRDR_EVT_LENGTH_ERROR_DATA (HID_CARDRDR_DATA_REPORT_OFFSET_VALUE + 1)

#ifdef __cplusplus
extern "C"
{
#endif

/// <summary>
/// Find the HID device with the given 'vendor' and 'product' id.
/// </summary>
/// <param name="vendor">IN - 'vendor' id of device desired.</param>
/// <param name="product">IN - 'product' id of device desired.</param>
/// <param name="devicePath">OUT - path of device found.</param>
/// <param name="pathLength">
/// INT/OUT - length of the buffer passed as 'devicePath'. If the length is too small, an INVALID_HANDLE_VALUE is returned,
/// lastError is set to ERROR_INSUFFICIENT_BUFFER, and pathLength will be set to the length needed.
/// Otherwise, pathLength may be set to the length of the device path.
/// </param>
/// <param name="lastError">OUT - last error generated if unsuccessful due to error. Can be NULL.</param>
/// <returns>Device handle. INVALID_HANDLE_VALUE is returned if unsuccessful.</returns>
/// <remarks>
/// The caller is responsible for the closing the returned HANDLE, if is is not INVALID_HANDLE_VALUE.
/// </remarks>
hid_device *CardReaderLayerFindDevice(USHORT vendor, USHORT product); //, std::string devicePath, ULONG& pathLength, DWORD *lastError);

void CardReaderLayerCloseDevice(hid_device *device);

///// <summary>
///// Generated a 'read' handle for the given device.
///// </summary>
///// <param name="devicePath">IN - path of device provided by it's 'detail data'.</param>
///// <param name="lastError">OUT - last error generated if unsuccessful due to error. Can be NULL.</param>
///// <returns>Device handle. INVALID_HANDLE_VALUE is returned if unsuccessful.</returns>
///// <remarks>
///// The caller is responsible for the closing the returned HANDLE, if is is not INVALID_HANDLE_VALUE.
///// </remarks>
//hid_device *CardReaderLayerGetReadHandle(std::string devicePath, DWORD *lastError);
//
///// <summary>
///// Provides the byte lengths of 'report buffers' for the given device.
///// </summary>
///// <param name="deviceHandle">IN - device handle, provided by 'CardReaderLayerFindDevice'.</param>
///// <param name="featureReportByteLength">OUT - byte length of the 'feature report' buffer. Set to 0 if unsuccessful.</param>
///// <param name="inputReportByteLength">OUT - byte length of the 'input report' buffer. Set to 0 if unsuccessful.</param>
///// <returns>'ERROR_SUCCESS' if successful. Last error otherwise.</returns>
//DWORD CardReaderLayerGetReportByteLengths(hid_device *deviceHandle, USHORT& featureReportByteLength, USHORT& inputReportByteLength);

/// <summary>
/// Passes a 'SetFeature' command to the card reader.
/// </summary>
/// <param name="deviceHandle">IN - device handle, provided by 'CardReaderLayerFindDevice'.</param>
/// <param name="reportBuf">IN - 'feature report' buffer containing the feature command and any other relevant data.</param>
/// <param name="bufferLength">IN - byte size of the 'feature report' buffer passed, provided by 'CardReaderLayerGetReportByteLengths'.</param>
/// <returns>'ERROR_SUCCESS' if successful. Last error otherwise.</returns>
bool CardReaderLayerSetFeature(hid_device *deviceHandle, PUCHAR reportBuf, USHORT bufferLength);

/// <summary>
/// Gets the 'input report' generated by the last 'SetFeature' command passed to the card reader.
/// </summary>
/// <param name="deviceHandle">IN - device handle, provided by 'CardReaderLayerFindDevice'.</param>
/// <param name="reportBuf">OUT - 'input report' buffer to receive the results of the last 'feature' command passed to the card reader.</param>
/// <param name="bufferLength">IN - byte size of the 'input report' buffer passed, provided by 'CardReaderLayerGetReportByteLengths'.</param>
/// <returns>'ERROR_SUCCESS' if successful. Last error otherwise.</returns>
bool CardReaderLayerGetInputReport(hid_device *readHandle, PUCHAR reportBuf, USHORT bufferLength, int &bytes);

#ifdef __cplusplus
}
#endif

