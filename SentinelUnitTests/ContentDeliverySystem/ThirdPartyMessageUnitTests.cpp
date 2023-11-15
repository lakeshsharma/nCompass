#include "stdafx.h"
#include "ContentDeliverySystem/ThirdPartyMessage.h"

// Unit testing includes.
#include "UnitTestingUtils.h"


#include "gtest/gtest.h"


namespace SentinelUnitTests
{
		TEST(ThirdPartyMessage, ThirdPartyMessage_CONTENT_LOADANDPLAY_Test)
		{
			const LPCTSTR blobData("<CONTENT_LOADANDPLAY package_id=\"919\" url=\"1065.swf\" priority=\"1\" command_id=\"command\">Initialization String</CONTENT_LOADANDPLAY>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR initializationString("Initialization String");
			const LPCTSTR filePath("1065.swf");
			const LPCTSTR commandId("command");
			const LPCTSTR packageId("919");
			const PromotionPlayPriority priority(Interruptable);			
			const SMAPINotificationType messageType(ContentLoad);
			const PlayerType playerType(All);
			const int vendorId(969);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetParameters(), initializationString);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Interruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_TRUE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == CThirdPartyMessage::DEFAULT_DISPLAY_TIME);
			ASSERT_TRUE(thirdPartyMessage.GetPackageId() == 919);
		}

		TEST(ThirdPartyMessage, ThirdPartyMessage_CONTENT_LOAD_Test)
		{
			const LPCTSTR blobData("<CONTENT_LOAD package_id=\"919\" url=\"1065.swf\" priority=\"0\" command_id=\"\"></CONTENT_LOAD>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR initializationString("");
			const LPCTSTR filePath("1065.swf");
			const LPCTSTR commandId("");
			const LPCTSTR packageId("919");
			const PromotionPlayPriority priority(Uninterruptable);
			const SMAPINotificationType messageType(ContentLoad);
			const PlayerType playerType(NonCarded);
			const int vendorId(969);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetParameters(), initializationString);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Uninterruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_FALSE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == CThirdPartyMessage::DEFAULT_DISPLAY_TIME);
		}

		TEST(ThirdPartyMessage, ThirdPartyMessage_CONTENT_UNLOAD_Test)
		{
			const LPCTSTR blobData("<CONTENT_UNLOAD package_id=\"919\" command_id=\"\"/>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR initializationString("");
			const LPCTSTR filePath("");
			const LPCTSTR commandId("");
			const LPCTSTR packageId("919");
			const PromotionPlayPriority priority(Uninterruptable);
			const SMAPINotificationType messageType(ContentUnload);
			const PlayerType playerType(NonCarded);
			const int vendorId(969);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetParameters(), initializationString);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Uninterruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_FALSE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == CThirdPartyMessage::DEFAULT_DISPLAY_TIME);
		}

		TEST(ThirdPartyMessage, ThirdPartyMessage_CONTENT_PLAY_Test)
		{
			const LPCTSTR blobData("<CONTENT_PLAY package_id=\"919\" command_id=\"\"/>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR initializationString("");
			const LPCTSTR filePath("");
			const LPCTSTR commandId("");
			const LPCTSTR packageId("919");
			const PromotionPlayPriority priority(Uninterruptable);
			const SMAPINotificationType messageType(ContentPlay);
			const PlayerType playerType(All);
			const int vendorId(969);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetParameters(), initializationString);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Uninterruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_FALSE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == CThirdPartyMessage::DEFAULT_DISPLAY_TIME);
		}
		
		TEST(ThirdPartyMessage, ThirdPartyMessage_POPUP_TEXT_Test)
		{
			const LPCTSTR blobData("<POPUP_TEXT command_id=\"\" display_time=\"9\">Popup Text</POPUP_TEXT>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR popupText("Popup Text");
			const LPCTSTR filePath("");
			const LPCTSTR commandId("");
			const LPCTSTR packageId("");
			
			const PromotionPlayPriority priority(Uninterruptable);
			const SMAPINotificationType messageType(PopUpTextMessage);
			const PlayerType playerType(NonCarded);
			const int vendorId(969);
			const int displayTime(9);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Uninterruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_FALSE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_EQ(thirdPartyMessage.GetParameters(), popupText);
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == displayTime);
		}

		TEST(ThirdPartyMessage, ThirdPartyMessage_SEND_MESSAGE_Test)
		{
			const LPCTSTR blobData("<SEND_MESSAGE command_id=\"\" package_id=\"919\">Hello From PCS</SEND_MESSAGE>");
			const LPCTSTR ocr("4B3D04FD37");
			const LPCTSTR sessionId("urn:uuid:e02e01bc-26c8-4d77-8713-ad793b9162c0");
			const LPCTSTR messageText("Hello From PCS");
			const LPCTSTR filePath("");
			const LPCTSTR commandId("");
			const LPCTSTR packageId("919");

			const PromotionPlayPriority priority(Uninterruptable);
			const SMAPINotificationType messageType(BlobMessage);
			const PlayerType playerType(Carded);
			const int vendorId(969);
			const int displayTime(CThirdPartyMessage::DEFAULT_DISPLAY_TIME);

			CThirdPartyMessage thirdPartyMessage(blobData, ocr, sessionId, messageType, playerType, vendorId);

			ASSERT_EQ(thirdPartyMessage.GetBlobData(), blobData);
			ASSERT_EQ(thirdPartyMessage.GetOCR(), ocr);
			ASSERT_EQ(thirdPartyMessage.GetSessionId(), sessionId);
			ASSERT_EQ(thirdPartyMessage.GetFilePath(), filePath);
			ASSERT_EQ(thirdPartyMessage.GetCommandId(), commandId);
			ASSERT_EQ(thirdPartyMessage.GetPackageIdInString(), packageId);
			ASSERT_TRUE(thirdPartyMessage.GetPriority() == Uninterruptable);
			ASSERT_TRUE(thirdPartyMessage.GetMessageType() == messageType);
			ASSERT_TRUE(thirdPartyMessage.GetPlayerType() == playerType);
			ASSERT_TRUE(thirdPartyMessage.GetVendorId() == vendorId);
			ASSERT_FALSE(thirdPartyMessage.IsBlobLoadAndPlay());
			ASSERT_EQ(thirdPartyMessage.GetParameters(), messageText);
			ASSERT_TRUE(thirdPartyMessage.GetDisplayTime() == displayTime);
		}
}
