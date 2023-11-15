#pragma once

#include "InternalMsgContent.h"
#include "GlobalDefs.h"

typedef enum
{
	Carded = 1,
	NonCarded = 2,
	All = 3,
} PlayerType;

const std::string CONTENT_LOAD_STATUS(_T("<CONTENT_LOAD_STATUS"));
const std::string CONTENT_UNLOAD_STATUS(_T("<CONTENT_UNLOAD_STATUS"));
const std::string CONTENT_PLAY_STATUS(_T("<CONTENT_PLAY_STATUS"));
const std::string POPUP_TEXT_STATUS(_T("<POPUP_TEXT_STATUS"));
const std::string SEND_MESSAGE_STATUS(_T("<SEND_MESSAGE_STATUS"));
const std::string SEND_MESSAGE_END(_T("</SEND_MESSAGE>"));
const std::string SEND_MESSAGE(_T("<SEND_MESSAGE"));
const std::string POPUP_TEXT_END(_T("</POPUP_TEXT>"));
const std::string END_PLAY(_T("<END_PLAY"));
const std::string BEGINNING_MARK_OF_MESSAGE(_T("\">"));
const std::string CARD_NUMBER(_T("card_number=\""));
const std::string URL(_T("url=\""));
const std::string PACKAGE_ID(_T("package_id=\""));
const std::string COMMAND_ID(_T("command_id=\""));
const std::string PRIORITY(_T("priority=\""));
const std::string DISPLAYTIME(_T("display_time=\""));
const std::string INTERRUPTIBLE_PRIORITY(_T("1"));

class CThirdPartyMessage :
	public CInternalMsgContent<INT_MSG_CONTENT_THIRDPARTYMESSAGE>
{
public:
	static const int DEFAULT_PACKAGE_ID = 0;
	static const int DEFAULT_DISPLAY_TIME = 2;

	/// <summary>
	/// Constructor. Initializes a new instance of the <see cref="CThirdPartyMessage"/> class.
	/// </summary>
	/// <param name="blobData">The BLOB data.</param>
	/// <param name="ocr">The ocr.</param>
	/// <param name="sessionId">The session identifier.</param>
	/// <param name="messageType">message type </param>
	/// <param name="playerType">player type.</param>
	/// <param name="vendorId">The vendor identifier.</param>
	CThirdPartyMessage(
		const std::string& blobData,
		const std::string& ocr,
		const std::string& sessionId,
		SMAPINotificationType messageType,
		PlayerType playerType,
		int vendorId);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CThirdPartyMessage"/> class.
	/// </summary>
	~CThirdPartyMessage();

	/// <summary>
	/// Gets the type of the message.
	/// </summary>
	/// <returns>type of message</returns>
	/// <summary>
	SMAPINotificationType GetMessageType()const;

	/// Sets the type of the message.
	/// </summary>
	/// <param name="playerType">Type of the player.</param>
	void SetMessageType(SMAPINotificationType playerType);

	/// <summary>
	/// Gets the OCR.
	/// </summary>
	/// <returns>The OCR.</returns>
	std::string GetOCR() const;

	/// <summary>
	/// Gets the player type.
	/// </summary>
	/// <returns>The player type.</returns>
	PlayerType GetPlayerType(void) const;

	/// <summary>
	/// Gets the vendor identifier.
	/// </summary>
	/// <returns>The vendor identifier.</returns>
	int GetVendorId(void) const;

	/// <summary>
	/// Gets the session identifier.
	/// </summary>
	/// <returns>The session identifier.</returns>
	std::string GetSessionId(void) const;

	/// <summary>
	/// Gets the package identifier.
	/// </summary>
	/// <returns>Package id</returns>
	int GetPackageId() const;

	/// <summary>
	/// Gets the display time.
	/// </summary>
	/// <returns>display time</returns>
	int GetDisplayTime() const;

	/// <summary>
	/// Gets the priority.
	/// </summary>
	/// <returns>PromotionPlayPriority</returns>
	PromotionPlayPriority GetPriority() const;

	/// <summary>
	/// Determines whether [is BLOB load and play].
	/// </summary>
	/// <returns>true if blob is load and play, returns false otherwise.</returns>
	bool IsBlobLoadAndPlay() const;

	/// <summary>
	/// Gets the package identifier in string.
	/// </summary>
	/// <returns>package identifier in string</returns>
	std::string GetPackageIdInString() const;

	/// <summary>
	/// Gets the command identifier.
	/// </summary>
	/// <returns>CommandId</returns>
	std::string GetCommandId() const;

	/// <summary>
	/// Gets the file path.
	/// </summary>
	/// <returns>file path</returns>
	std::string GetFilePath() const;

	/// <summary>
	/// Gets the BLOB data.
	/// </summary>
	/// <returns>BLOB data</returns>
	std::string GetBlobData() const;

	/// <summary>
	/// Gets the parameters.
	/// </summary>
	/// <returns>parameters</returns>
	std::string GetParameters() const;

	/// <summary>
	/// Gets the ID string used for queue flushing
	/// </summary>
	/// <returns>the flush ID string</returns>
	std::string GetQueueFlushIDString() const;

private:
	std::string m_blobData;
	std::string	m_ocr;
	std::string m_sessionId;
	SMAPINotificationType m_messageType;
	PlayerType m_playerType;
	int m_vendorId;

	std::string ExtractFieldValue(const std::string& fieldName) const;
	CThirdPartyMessage();
	CThirdPartyMessage(const CThirdPartyMessage& other);
	void operator=(const CThirdPartyMessage& other);
};
