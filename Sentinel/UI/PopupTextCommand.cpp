#include "PopupTextCommand.h"
#include "Logging/Logger.h"
#include "Logging/LogString.h"

const std::string TEXT_TAG_TYPE_GENERIC = "Generic";
const std::string TEXT_TAG_TYPE_CASHOUT_ERROR_FLAGS = "CashoutErrorFlags";
const std::string TEXT_TAG_TYPE_AUTOUPLOAD_ERROR_FLAGS = "AutoUploadErrorFlags";
const std::string TEXT_TAG_TYPE_PB_TRANSFER_ERROR = "PBTransferError";

CPopupTextCommand::CPopupTextCommand(const std::string& textTagType, std::string textTag, PopupTextType popupType, DWORD displayDuration) :
	CCommandMessage("ShowPopupText", displayDuration),
	m_textTagType(textTagType), m_textTag(textTag), m_popupType(popupType)
{
}

CPopupTextCommand::~CPopupTextCommand()
{
}

void CPopupTextCommand::addCustomArgs(JsonWriter &writer) const
{
	writer.Key("textTagType");			writer.String(m_textTagType);
	writer.Key("textTag");				writer.String(m_textTag);
	writer.Key("popupType");			writer.Int64(m_popupType);
}