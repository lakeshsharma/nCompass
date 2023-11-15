#include "stdafx.h"
#include "Utilities.h"
#include "Logging/LogString.h"


#include "ThirdPartyMessage.h"


CThirdPartyMessage::CThirdPartyMessage(
	const std::string& blobData,
	const std::string& ocr,
	const std::string& sessionId,
	SMAPINotificationType messageType, 
	PlayerType playerType, 
	int vendorId) :
m_blobData(blobData),
m_ocr(ocr),
m_sessionId(sessionId),
m_messageType(messageType),
m_playerType(playerType),
m_vendorId(vendorId)
{
	SetMessageType(messageType);
	LogString(THIRDPARTY, "Thirdparty message created - %s", blobData.c_str());
}

CThirdPartyMessage::~CThirdPartyMessage()
{
}

SMAPINotificationType CThirdPartyMessage::GetMessageType(void) const
{
	return m_messageType;
}

void CThirdPartyMessage::SetMessageType(SMAPINotificationType playerType)
{
	m_messageType = playerType;
}

std::string CThirdPartyMessage::GetOCR() const
{
	return m_ocr;
}

PlayerType CThirdPartyMessage::GetPlayerType(void) const
{
	return m_playerType;
}

int CThirdPartyMessage::GetVendorId(void) const
{
	return m_vendorId;
}

std::string CThirdPartyMessage::GetSessionId(void) const
{
	return m_sessionId;
}

int CThirdPartyMessage::GetPackageId() const
{
	int package_id = DEFAULT_PACKAGE_ID;

    if (m_blobData != "")
	{
		//Get the package_id
        std::size_t findIndex = m_blobData.find(PACKAGE_ID);
		if (findIndex != std::string::npos)
		{
            package_id = strtol(m_blobData.substr(findIndex + PACKAGE_ID.size()).c_str(), NULL, 10);

		}
	}

	return package_id;
}

int CThirdPartyMessage::GetDisplayTime() const
{
	int displayTime = DEFAULT_DISPLAY_TIME;

	if (m_blobData != "")
	{
		//Get the display_time
        std::size_t findIndex = m_blobData.find(DISPLAYTIME);
		if (std::string::npos != findIndex)
		{ 
            displayTime = strtol(m_blobData.substr(findIndex + DISPLAYTIME.size()).c_str(), NULL, 10);
		}
	}

	return displayTime;
}

PromotionPlayPriority CThirdPartyMessage::GetPriority() const
{
	PromotionPlayPriority priority = Uninterruptable;

	if (m_blobData != "")
	{
		std::string priorityString = PRIORITY;
		std::string priorityValueString;

		//extract priority from the original message
        std::size_t startIndex = m_blobData.find(priorityString);
        if (startIndex != std::string::npos) 
        {
            std::size_t endIndex = m_blobData.find("\"", startIndex + priorityString.length());
			if (startIndex < endIndex)
                priorityValueString = m_blobData.substr(startIndex + priorityString.length(), endIndex - startIndex - priorityString.length());
		}

		if (priorityValueString == INTERRUPTIBLE_PRIORITY)
		{
			priority = Interruptable;
		}
	}

	return priority;
}

bool CThirdPartyMessage::IsBlobLoadAndPlay() const
{
	bool isLoadAndPlay = false;

    if (m_blobData != "")
	{
        const std::string commandString = "LOADANDPLAY";
        int findIndex = m_blobData.find(commandString);
		if (findIndex >= 0)
		{
			isLoadAndPlay = true;
		}
	}

	return isLoadAndPlay;
}

std::string CThirdPartyMessage::GetPackageIdInString() const
{
	return ExtractFieldValue(PACKAGE_ID);
}

std::string CThirdPartyMessage::GetCommandId() const
{
	return ExtractFieldValue(COMMAND_ID);
}

std::string CThirdPartyMessage::GetFilePath() const
{
	return ExtractFieldValue(URL);
}

std::string CThirdPartyMessage::GetBlobData() const
{
	return m_blobData;
}

std::string CThirdPartyMessage::GetParameters() const
{
	std::string parameters;

	if (m_blobData != "")
	{
		const std::string LEFT_OF_PARAMETERS("\">");
		const char RIGHT_OF_PARAMETERS1 = '<';
		const char RIGHT_OF_PARAMETERS2 = '/';

		//extract parameters from the original message
        std::size_t startIndex = m_blobData.find(LEFT_OF_PARAMETERS) + LEFT_OF_PARAMETERS.size();
        std::size_t endIndex = m_blobData.find_last_of(RIGHT_OF_PARAMETERS2) - 1;
        if (startIndex != std::string::npos 
            && endIndex != std::string::npos 
            && endIndex > startIndex 
            && m_blobData[endIndex] == RIGHT_OF_PARAMETERS1)
		{
            parameters = m_blobData.substr(startIndex, endIndex - startIndex);
		}
	}

	return parameters;
}

std::string CThirdPartyMessage::GetQueueFlushIDString() const
{
	return (GetSessionId() + GetPackageIdInString());
}


std::string CThirdPartyMessage::ExtractFieldValue(const std::string& fieldName) const
{
	std::string fieldValue;

	if (!m_blobData.empty())
	{
		//extract field value from the original message
		std::size_t findIndex = m_blobData.find(fieldName);
        if (findIndex != std::string::npos) 
        {
            std::size_t startIndex = findIndex + fieldName.size();
            std::size_t endIndex = m_blobData.find("\"", startIndex);
            if (endIndex != std::string::npos && startIndex < endIndex)
			{
                fieldValue = m_blobData.substr(startIndex, endIndex - startIndex);
			}				
		}
	}

	return fieldValue;
}

