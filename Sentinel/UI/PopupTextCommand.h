#pragma once

#include <string>
#include "GlobalDefs.h"
#include "CommandMessage.h"

extern const std::string TEXT_TAG_TYPE_GENERIC;
extern const std::string TEXT_TAG_TYPE_CASHOUT_ERROR_FLAGS;
extern const std::string TEXT_TAG_TYPE_AUTOUPLOAD_ERROR_FLAGS;
extern const std::string TEXT_TAG_TYPE_PB_TRANSFER_ERROR;

/* NOTE: This enum must always be in sync with a similar enum in Angular UI code */
enum PopupTextType {
	Popup_Info,
	Popup_ConfirmYesNo,
	Popup_ConfirmOkOnly,
	Popup_InProgress,
	Popup_Success,
	Popup_Failure,
};

class CPopupTextCommand : public CCommandMessage
{
public:
	CPopupTextCommand(const std::string& textTagType, std::string textTag, PopupTextType popupType, DWORD displayDuration);
	virtual ~CPopupTextCommand();

	virtual void addCustomArgs(JsonWriter& writer) const;

private:
	std::string m_textTagType;
	std::string m_textTag;
	PopupTextType m_popupType;
};