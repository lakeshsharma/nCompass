#pragma once

#include "QueueList.h"
#include "InternalMessage.h"
#include "MessagePersist.h"
#include "QueuingConfigFile.h"

typedef enum
{
	IncrementUpdateValue,
	Force100PercentValue,
	ForceZeroValue,
	DeleteBonusCounter,
} CasinoChallengeIncrementUpdateType;

typedef enum
{
	NoFlushType,
	PBFlushType,
	PromotionFlushType,
	JackpotFlushType,
	PteFlushType,
	EmployeeFlushType,
} MessageFlushType;

// Data class encapsulation for Prioritize/Persist data based on MessageType
struct MessagePriorityInfo
{
	// the priority of associated CMessagePersist type for UI
	int persistPriority;

	// the priority of the associated CInternalMessage
	int messagePriority;

	// the Message Persist type
	MessagePersistType persistType;

	// the queue starvation value - default to -1
	int queueStarve;

	// is interactive and needs to have primary/secondary starvation priorities
	bool interactive;

	// time to live
	DWORD ttl;
};

// 3 Seconds
const DWORD INITIALINTERACTIVESTARVETIME = 3000;
const DWORD INTERACTIVECONTENTTOTALSTARVETIME = 300000;
const DWORD POPUPDISPLAYTIME = 4000;
const DWORD POPUPDISPLAYUNLIMITED = MAXDWORD;
const DWORD POPUPDISPLAYTIME_ONESECOND = 1000;
const DWORD USERSCREENTIME = 300000;
const DWORD PBTSCREENS = 1000;
const DWORD PLAYERINITIATED = 2000;
const DWORD INTERACTIVECONTENT = 2500;
const DWORD WINNERMESSAGE = 3000;
const DWORD WINNERTTL = 5000;
const DWORD SMPROMO = 5000;
const DWORD CONGRATS = 7000;
const DWORD CONGRATSTTL = 15000;
const DWORD WELCOME = 8000;
const DWORD WINNERANTICIPATION = 9000;
const DWORD WINNERANTICIPATIONTTL = 15000;
const DWORD ANNOUNCEMENTS = 9000;
const DWORD ANNOUNEMENTSTTL = 60000;
const DWORD BIRTHDAY = 11000;
const DWORD ANNIVERSARY = 12000;
const DWORD DISPLAY_BANNED_PLAYER_FOR_RFID_TIMEOUT = 7500;

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class MessagePrioritizerUnitTests; }

// executing class to prioritze InternalMessages for UI based on message type
class CMessagePrioritizer
{
public:
	/// <summary>
	/// Default Ctor
	/// </summary>
	CMessagePrioritizer();

	/// <summary>
	/// Default Dtor
	/// </summary>
	~CMessagePrioritizer();

	/// <summary>
	/// Use the CInternalMessage to determine the Priority and return the MessagePersist
	/// </summary>
	/// <remarks>
	/// The class use a keyed collection on MessageType.  The collection is ONLY for those items that 
	/// are not default.  The returned Persist and Priority will be default values if the MessageType
	/// is not in the Keyed collection.
	/// </remarks>
	/// <param name="messageIn">the CInternalMessage pointer</param>
	/// <param name="priority">the message prioirity</param>
	/// <param name="flushID">the ID to be used in message flushing</param>
	/// <returns>pointer to new CMessagePersist object</returns>
	CMessagePersist *PrioritizeMessage(CInternalMessage *messageIn, int &priority, __int64 flushID = 0);

	CMessagePersist *PrioritizeMessage(CInternalMessage *messageIn, int &priority, const std::string &flushID);

	/// <summary>
	/// Use the CInternalMessage to determine the Priority and return the MessagePersist
	/// </summary>
	/// <remarks>
	/// The class use a keyed collection on MessageType.  The collection is ONLY for those items that 
	/// are not default.  The returned Persist and Priority will be default values if the MessageType
	/// is not in the Keyed collection.
	/// </remarks>
	/// <param name="messageIn">the CInternalMessage pointer</param>
	/// <param name="screenType">the screen type</param>
	/// <param name="priority">the message prioirity</param>
	/// <param name="flushID">the ID to be used in message flushing</param>
	/// <returns>pointer to new CMessagePersist object</returns>
	CMessagePersist *PrioritizeAnnouncementMessage(CInternalMessage *messageIn, const std::string& screenType, int &priority, __int64 flushID);

	/// <summary>
	/// Use the CInternalMessage to determine the Priority and return the MessagePersist
	/// </summary>
	/// <remarks>
	/// The class use a keyed collection on MessageType.  The collection is ONLY for those items that 
	/// are not default.  The returned Persist the default value if the MessageType
	/// is not in the Keyed collection.
	/// </remarks>
	/// <param name="messageIn">The message in.</param>
	/// <returns></returns>
	CMessagePersist *PrioritizeMessage(CInternalMessage *messageIn);


	/// <summary>
	/// Prioritize the winner picked message.
	/// </summary>
	/// <param name="msg">The MSG.</param>
	/// <param name="priority">The priority.</param>
	/// <param name="winner">The winner.</param>
	/// <returns>New message persist</returns>
	CMessagePersist *PrioritizeWinnerPickedMessage(CInternalMessage *msg, int& priority, CProgressiveWinnerPicked *winner);

	/// <summary>
	/// Loads config file and builds the MessagePriorityMap
	/// </summary>
	void BuildMessagePriorityMap();

	/// <summary>
	/// Gets the equal priority interrupt value.
	/// </summary>
	/// <returns>true if equal priorities should interrupt.</returns>
	bool GetEqualPriorityInterrupt();

	/// <summary>
	/// Gets the flush type of the provided message type.
	/// </summary>
	/// <param name="msgType">The message type</param>
	/// <returns>the flush type</returns>
	MessageFlushType GetMessageFlushType(InternalMsgType msgType);

	/// <summary>
	/// Determines if the provided message type is for a flusher message.
	/// </summary>
	/// <param name="msgType">The message type</param>
	/// <param name="screeenType">The screen type screen. NULL if N/A</param>
	/// <returns>true if the provided message is for a flusher message, otherwise false</returns>
	bool IsFlusherMessage(InternalMsgType msgType, const std::string& screeenType = "");

	/// <summary>
	/// Gets the queuing configuration.
	/// </summary>
	/// <returns></returns>
	CQueuingConfig& GetQueuingConfig();

private:
	/// <summary>
	/// The TypeDef for a keyed collection of MessagePriorityInfo objects
	/// </summary>
	typedef std::map<InternalMsgType, MessagePriorityInfo> MessagePriorityMap;

	/// <summary>
	/// The keyed collection of MessagePriorityInfo objects
	/// </summary>
	MessagePriorityMap m_messagePriorityMap;

	/// <summary>
	/// Called by the BuildMessagePriorityMap this builds a single MessagePriorityInfo object
	/// </summary>
	void BuildPriorityInfo(InternalMsgType msgType,
		int persistPriority = QUEUE_PRIORITY_NORMAL,
		int messagePriority = QUEUE_PRIORITY_NORMAL,
		MessagePersistType persistType = PERSIST_NONE,
		DWORD ttl = 0,
		bool interactive = false,
		int queueStarve = -1);

	/// <summary>
	/// Find a MessagePriorityInfo for the MessageType or stay with defaults
	/// </summary>
	bool FindMessagePriority(InternalMsgType msgType, MessagePriorityInfo &priorityInfo);

	// Message flushing functions
	bool IsPBTMessage(InternalMsgType msgType);
	bool IsPromotionMessage(InternalMsgType msgType);
	bool IsJackpotMessage(InternalMsgType msgType);
	bool IsPteFlushtype(InternalMsgType msgType);
	bool IsEmployeeFlushtype(InternalMsgType msgType);

	CQueuingConfig * LoadConfigFile();

	bool m_nRichAwardInProgress;
	CQueuingConfig m_queuingConfig;
	std::recursive_mutex m_criticalSection;
	DWORD m_smpromo;
	DWORD m_smpromottl;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::MessagePrioritizerUnitTests;
	FRIEND_TEST(MessagePrioritizerUnitTests, GetMessageFlushTypeUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, IsPBTMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, IsPromotionMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, IsJackpotMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, IsCDMFlushtypeUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, IsPteFlushtypeUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, FindMessagePriorityUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, SetupnRichIncrementUpdateValueMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, SetupnRichIncrementForce100PercentValueMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, SetupnRichIncrementForceZeroValueMessageUnitTest);
	FRIEND_TEST(MessagePrioritizerUnitTests, SetupnRichIncrementDeleteBonusCounterMessageUnitTest);
};

