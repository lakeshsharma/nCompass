#if 000//TODO-PORT
#include "stdafx.h"

#include "GlobalDefs.h"
#include "..\\SpeedMedia\\CardInResendItem.h"
#include "..\\SpeedMedia\\CardOutResendItem.h"
#include "..\\SpeedMedia\\CoinInTrigger.h"
#include "..\\SpeedMedia\\InEventTriggerResendItem.h"
#include "..\\SpeedMedia\\InTriggeredEventTriggerResendItem.h"
#include "..\\SpeedMedia\\SplashdownCountdown.h"
#include "..\\SpeedMedia\\CumulativeResendItem.h"
#include "..\\SpeedMedia\\GamePlayedResendItem.h"
#include "..\\SpeedMedia\\SentinelInformationResendItem.h"
#include "..\\SpeedMedia\\SentinelInformationWithOrientationResendItem.h"
#include "..\\SpeedMedia\\SentinelStatusResendItem.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(SpeedMediaResendItemUnitTests)
	{
	private:
		DWORD m_slotMastID;
		BYTE m_cardID[CARD_ID_LEN];
		SYSTEMTIME m_transactionTime;
		__int64 m_minThreshold, m_maxThreshold;
		int m_eventId, m_nonTriggeredEventsCount;
	public:
		SpeedMediaResendItemUnitTests()
		{
			m_slotMastID = 999;
			for(unsigned int cardIdIndex = 0; cardIdIndex < CARD_ID_LEN; ++cardIdIndex)
			{
				m_cardID[cardIdIndex] = cardIdIndex + 1;
			}			
			m_transactionTime.wYear			=	2013;
			m_transactionTime.wMonth		=	1;
			m_transactionTime.wDayOfWeek	=	2;
			m_transactionTime.wDay			=	3;
			m_transactionTime.wHour			=	4;
			m_transactionTime.wMinute		=	5;
			m_transactionTime.wSecond		=	6;
			m_transactionTime.wMilliseconds	=	7;

			m_minThreshold = 50;
			m_maxThreshold = 100;
			m_eventId = 23;
			m_nonTriggeredEventsCount = 5;
		}

		void TestCardInAndOutItemValues(const DWORD& slotMastID,
										const BYTE*  cardID,
									    const SYSTEMTIME& transactionTime) const
		{
			//Tests
			Assert::IsTrue( memcmp( m_cardID, cardID, CARD_ID_LEN ) == 0,
							L"m_cardid was not properly initialized in the constructor" );
			Assert::IsTrue( m_slotMastID == slotMastID,
							L"m_slotmastId was not properly initialized in the constructor" );
			Assert::IsTrue( m_transactionTime.wYear			== transactionTime.wYear &&
							m_transactionTime.wMonth		== transactionTime.wMonth &&
							m_transactionTime.wDayOfWeek	== transactionTime.wDayOfWeek &&
							m_transactionTime.wDay			== transactionTime.wDay &&
							m_transactionTime.wHour			== transactionTime.wHour &&
							m_transactionTime.wMinute		== transactionTime.wMinute &&
							m_transactionTime.wSecond		== transactionTime.wSecond &&
							m_transactionTime.wMilliseconds	== transactionTime.wMilliseconds,
							L"m_transactionTimestamp was not properly initialized in the constructor" );
		}

		void SetupInEventTriggeredResendItem(int*& nonTriggeredEvents,
											 CSpeedMediaEvent*& speedMediaEvent,
											 CSpeedMediaTriggers*& smTriggers,
											 CCoinInTrigger*& trigger)
		{
			//Setup
			smTriggers = new CSpeedMediaTriggers();
			
			trigger = new CCoinInTrigger(m_minThreshold, m_maxThreshold, CoinInTrigger);
			smTriggers->AddTrigger(trigger);
			smTriggers->SetIsCumulative(true);

			speedMediaEvent = new CSpeedMediaEvent();
			speedMediaEvent->SetEventID(m_eventId);
			speedMediaEvent->SetEventStartTrigger(smTriggers);
			
			nonTriggeredEvents = new int[m_nonTriggeredEventsCount];
			for(int nonTriggeredEventIndex = 0; 
				    nonTriggeredEventIndex < m_nonTriggeredEventsCount;
					++nonTriggeredEventIndex)
			{
				nonTriggeredEvents[nonTriggeredEventIndex] = m_nonTriggeredEventsCount - nonTriggeredEventIndex;
			}
		}

		void SetupInTriggeredEventTriggerResendItem(CSplashdownCountdown*& triggeredEvent,
													CSpeedMediaTriggers*& smTriggers,
													CCoinInTrigger*& trigger)
		{
			//Setup
			triggeredEvent	= new CSplashdownCountdown();
			smTriggers		= new CSpeedMediaTriggers();
			trigger			= new CCoinInTrigger(m_minThreshold, m_maxThreshold, CoinInTrigger);
			smTriggers->SetIsCumulative(true);	
			smTriggers->AddTrigger(trigger);
			triggeredEvent->SetCardID(m_cardID, CARD_ID_LEN);
			triggeredEvent->SetInEventTrigger(smTriggers);
			triggeredEvent->SetEventID(m_eventId);
		}

		TEST_METHOD(CardInResendItemConstructorTest)
		{
			bool hasPlayed = false;

			//Setup
			CCardInResendItem* resendItem(new CCardInResendItem(m_slotMastID, m_cardID, hasPlayed, m_transactionTime));

			//Tests
			TestCardInAndOutItemValues(resendItem->GetSlotMastID(), 
									   resendItem->GetCardID(), 
									   resendItem->GetTransactionTime());
			Assert::IsTrue( hasPlayed == resendItem->GetHasPlayed(),
							L"CCardInResendItem - m_hasPlayed was not properly initialized in the constructor" );

			//Cleanup
			delete resendItem;
		}

		TEST_METHOD(CardInResendItemFunctionsTest)
		{
			bool hasPlayed = false;
			SYSTEMTIME transactionTime, returnedTransactionTime;
			transactionTime.wYear			=	2013;
			transactionTime.wMonth			=	9;
			transactionTime.wDayOfWeek		=	7;
			transactionTime.wDay			=	6;
			transactionTime.wHour			=	5;
			transactionTime.wMinute			=	4;
			transactionTime.wSecond			=	3;
			transactionTime.wMilliseconds	=	2;

			//Setup
			CCardInResendItem* resendItem(new CCardInResendItem(m_slotMastID, m_cardID, 
															   hasPlayed, m_transactionTime));

			resendItem->SetTransactionTime(transactionTime);
			returnedTransactionTime = resendItem->GetTransactionTime();

			//Tests
			Assert::IsTrue( transactionTime.wYear			== returnedTransactionTime.wYear &&
							transactionTime.wMonth			== returnedTransactionTime.wMonth &&
							transactionTime.wDayOfWeek		== returnedTransactionTime.wDayOfWeek &&
							transactionTime.wDay			== returnedTransactionTime.wDay &&
							transactionTime.wHour			== returnedTransactionTime.wHour &&
							transactionTime.wMinute			== returnedTransactionTime.wMinute &&
							transactionTime.wSecond			== returnedTransactionTime.wSecond &&
							transactionTime.wMilliseconds	== returnedTransactionTime.wMilliseconds,
							L"SetTransactionTime function failed" );

			//Cleanup
			delete resendItem;
		}

		TEST_METHOD(CardInResendItemConstructorFromBufferTest)
		{
			bool hasPlayed = false;

			//Setup
			CCardInResendItem* resendItem(new CCardInResendItem(m_slotMastID, m_cardID, hasPlayed, 
															    m_transactionTime));
			int lengthOfBuffer(resendItem->GetLength());
			BYTE* buffer(resendItem->GetBuffer(lengthOfBuffer));		
			CCardInResendItem* resendItemFromBuffer(new CCardInResendItem(buffer, lengthOfBuffer));

			//Tests
			TestCardInAndOutItemValues(resendItemFromBuffer->GetSlotMastID(), 
									   resendItemFromBuffer->GetCardID(), 
							           resendItemFromBuffer->GetTransactionTime());
			Assert::IsTrue( hasPlayed == resendItemFromBuffer->GetHasPlayed(),
							L"CCardInResendItem - m_hasPlayed was not properly initialized in the constructor" );

			//Cleanup
			delete resendItem;
			delete resendItemFromBuffer;
		}

		TEST_METHOD(CardOutResendItemConstructorTest)
		{
			//Setup
			CCardOutResendItem* resendItem(new CCardOutResendItem(m_slotMastID, m_cardID, m_transactionTime));

			//Tests
			TestCardInAndOutItemValues(resendItem->GetSlotMastID(), 
									   resendItem->GetCardID(), 
									   resendItem->GetTransactionTime());

			//Cleanup
			delete resendItem;
		}

		TEST_METHOD(CardOutResendItemConstructorFromBufferTest)
		{
			bool hasPlayed = false;

			//Setup
			CCardOutResendItem* resendItem(new CCardOutResendItem(m_slotMastID, m_cardID, m_transactionTime));
			int lengthOfBuffer(resendItem->GetLength());
			BYTE* buffer(resendItem->GetBuffer(lengthOfBuffer));	
			CCardOutResendItem* resendItemFromBuffer(new CCardOutResendItem(buffer, lengthOfBuffer));

			//Tests
			TestCardInAndOutItemValues(resendItemFromBuffer->GetSlotMastID(), 
									   resendItemFromBuffer->GetCardID(), 
							           resendItemFromBuffer->GetTransactionTime());

			//Cleanup
			delete resendItem;
			delete resendItemFromBuffer;
		}

		TEST_METHOD(CardOutResendItemFunctionsTest)
		{
			bool hasPlayed = false;
			SYSTEMTIME transactionTime, returnedTransactionTime;
			transactionTime.wYear			=	2013;
			transactionTime.wMonth			=	9;
			transactionTime.wDayOfWeek		=	7;
			transactionTime.wDay			=	6;
			transactionTime.wHour			=	5;
			transactionTime.wMinute			=	4;
			transactionTime.wSecond			=	3;
			transactionTime.wMilliseconds	=	2;

			//Setup
			CCardOutResendItem* resendItem(new CCardOutResendItem(m_slotMastID, m_cardID, m_transactionTime));

			resendItem->SetTransactionTime(transactionTime);
			returnedTransactionTime = resendItem->GetTransactionTime();

			//Tests
			Assert::IsTrue( transactionTime.wYear			== returnedTransactionTime.wYear &&
							transactionTime.wMonth			== returnedTransactionTime.wMonth &&
							transactionTime.wDayOfWeek		== returnedTransactionTime.wDayOfWeek &&
							transactionTime.wDay			== returnedTransactionTime.wDay &&
							transactionTime.wHour			== returnedTransactionTime.wHour &&
							transactionTime.wMinute			== returnedTransactionTime.wMinute &&
							transactionTime.wSecond			== returnedTransactionTime.wSecond &&
							transactionTime.wMilliseconds	== returnedTransactionTime.wMilliseconds,
							L"SetTransactionTime function failed" );

			//Cleanup
			delete resendItem;
		}

		TEST_METHOD(InEventTriggeredResendItemConstructorTest)
		{
			//Setup
			SpeedMediaTransactionType type(TypeEventStartTrigger);
			int* nonTriggeredEvents(NULL);
			CSpeedMediaEvent* speedMediaEvent(NULL);
			CSpeedMediaTriggers* smTriggers(NULL);
			CCoinInTrigger* trigger(NULL);
			SetupInEventTriggeredResendItem(nonTriggeredEvents, speedMediaEvent, smTriggers, trigger);
			CInEventTriggeredResendItem* resendItem(new CInEventTriggeredResendItem(type,
																					speedMediaEvent,
																					m_slotMastID,
																					nonTriggeredEvents,
																					m_nonTriggeredEventsCount));
			//Tests
			Assert::IsTrue(type == resendItem->GetType(),
						   L"CInEventTriggeredResendItem - m_type was not properly initialized in the constructor");
			Assert::IsTrue(m_eventId == resendItem->GetEvent()->GetEventID(),
						   L"CInEventTriggeredResendItem - m_smEvent was not properly initialized in the constructor");
			Assert::IsTrue(m_slotMastID == resendItem->GetSlotmastID(),
						   L"CInEventTriggeredResendItem - m_slotmastID was not properly initialized in the constructor");
			Assert::IsTrue(m_nonTriggeredEventsCount == resendItem->GetNonTriggeredEventsCount(),
							   L"CInEventTriggeredResendItem - m_nonTriggeredEventCount was not properly initialized in the constructor");
			for(int nonTriggeredEventIndex = 0; 
				    nonTriggeredEventIndex < m_nonTriggeredEventsCount;
					++nonTriggeredEventIndex)
			{
				Assert::IsTrue(nonTriggeredEvents[nonTriggeredEventIndex] == resendItem->GetNonTriggeredEvents()[nonTriggeredEventIndex],
							   L"CInEventTriggeredResendItem - m_nonTriggeredEvents was not properly initialized in the constructor");
			}

			//Cleanup
			delete resendItem;
			delete speedMediaEvent;//smTriggers and trigger objects will be deleted by CSpeedMediaEvent destructor
			delete nonTriggeredEvents;
		}

		TEST_METHOD(InEventTriggeredResendItemConstructorFromBufferTest)
		{
			//Setup
			SpeedMediaTransactionType type(TypeEventStartTrigger);
			int* nonTriggeredEvents(NULL);
			CSpeedMediaEvent* speedMediaEvent(NULL);
			CSpeedMediaTriggers* smTriggers(NULL);
			CCoinInTrigger* trigger(NULL);
			SetupInEventTriggeredResendItem(nonTriggeredEvents, speedMediaEvent, smTriggers, trigger);
			CInEventTriggeredResendItem* resendItem(new CInEventTriggeredResendItem(type,
																					speedMediaEvent,
																					m_slotMastID,
																					nonTriggeredEvents,
																					m_nonTriggeredEventsCount));	
			int lengthOfBuffer(resendItem->GetLength());
			BYTE* buffer(resendItem->GetBuffer(lengthOfBuffer));
			CInEventTriggeredResendItem* resendItemFromBuffer(new CInEventTriggeredResendItem(buffer, lengthOfBuffer));
			
			//Tests
			Assert::IsTrue(resendItem->GetType() == resendItemFromBuffer->GetType(),
						   L"CInEventTriggeredResendItem - m_type was not properly initialized in the constructor");
			Assert::IsTrue(resendItem->GetEvent()->GetEventID() == resendItemFromBuffer->GetEvent()->GetEventID(),
						   L"CInEventTriggeredResendItem - m_smEvent was not properly initialized in the constructor");
			Assert::IsTrue(resendItem->GetSlotmastID() == resendItemFromBuffer->GetSlotmastID(),
						   L"CInEventTriggeredResendItem - m_slotmastID was not properly initialized in the constructor");
			Assert::IsTrue(resendItem->GetNonTriggeredEventsCount() == resendItemFromBuffer->GetNonTriggeredEventsCount(),
							   L"CInEventTriggeredResendItem - m_nonTriggeredEventCount was not properly initialized in the constructor");
			for(int nonTriggeredEventIndex = 0; 
				    nonTriggeredEventIndex < m_nonTriggeredEventsCount;
					++nonTriggeredEventIndex)
			{
				Assert::IsTrue(resendItem->GetNonTriggeredEvents()[nonTriggeredEventIndex] == resendItemFromBuffer->GetNonTriggeredEvents()[nonTriggeredEventIndex],
							   L"CInEventTriggeredResendItem - m_nonTriggeredEvents was not properly initialized in the constructor");
			}

			//Cleanup
			delete resendItem;
			delete resendItemFromBuffer;
			delete speedMediaEvent;//smTriggers and trigger objects will be deleted by CSpeedMediaEvent destructor
			delete nonTriggeredEvents;
		}

		TEST_METHOD(InTriggeredEventTriggerResendItemConstructorTest)
		{
			//Setup
			SpeedMediaTransactionType type(TypeInTriggeredEventTrigger);
			CSplashdownCountdown* triggeredEvent(NULL);
			CSpeedMediaTriggers* smTriggers(NULL);
			CCoinInTrigger* trigger(NULL);
			SetupInTriggeredEventTriggerResendItem(triggeredEvent, smTriggers, trigger);
			CInTriggeredEventTriggerResendItem* resenditem(new CInTriggeredEventTriggerResendItem(type,
																								  triggeredEvent,
																								  m_slotMastID));
			//Tests
			Assert::IsTrue(type == resenditem->GetType(),
						   L"CInTriggeredEventTriggerResendItem - m_type was not properly initialized in the constructor");
			Assert::IsTrue(triggeredEvent->GetEventID() == resenditem->GetTriggeredEvent()->GetEventID(),
						   L"CInTriggeredEventTriggerResendItem - m_smevent was not properly initialized in the constructor");
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotmastID(),
						   L"CInTriggeredEventTriggerResendItem - m_slotmastid was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete triggeredEvent;//this will delete smTriggers and trigger
		}

		TEST_METHOD(InTriggeredEventTriggerResendItemConstructorFromBufferTest)
		{
			//Setup
			SpeedMediaTransactionType type(TypeInTriggeredEventTrigger);
			CSplashdownCountdown* triggeredEvent(NULL);
			CSpeedMediaTriggers* smTriggers(NULL);
			CCoinInTrigger* trigger(NULL);
			SetupInTriggeredEventTriggerResendItem(triggeredEvent, smTriggers, trigger);
			CInTriggeredEventTriggerResendItem* resenditem(new CInTriggeredEventTriggerResendItem(type,
																								  triggeredEvent,
																								  m_slotMastID));
			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CInTriggeredEventTriggerResendItem* resendItemFromBuffer(new CInTriggeredEventTriggerResendItem(buffer, lengthOfBuffer));			


			//Tests
			Assert::IsTrue(resendItemFromBuffer->GetType() == resenditem->GetType(),
						   L"CInTriggeredEventTriggerResendItem - m_type was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetTriggeredEvent()->GetEventID() == resenditem->GetTriggeredEvent()->GetEventID(),
						   L"CInTriggeredEventTriggerResendItem - m_smevent was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetSlotmastID() == resenditem->GetSlotmastID(),
						   L"CInTriggeredEventTriggerResendItem - m_slotmastid was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete triggeredEvent;//this will delete smTriggers and trigger
		}

		TEST_METHOD(CumulativeResendItemConstructorTest)
		{
			//Setup
			__int64 coinInDelta(1), coinOutDelta(2), externalBonusingAttendantPayoutDelta(3), 
				    externalBonusingMachinePayoutDelta(4), jackpotHandpayDelta(5), machinePayProgressivePayoutDelta(6), 
					attendantPayProgressivePayoutDelta(7), jackpotAmount(8), timeDelta(9);
			int jackpotTriggerType(1);
			CSpeedMediaCumulativePackage* pkg(NULL);
			pkg = new CSpeedMediaCumulativePackage(coinInDelta, coinOutDelta, externalBonusingAttendantPayoutDelta, 
												   externalBonusingMachinePayoutDelta, jackpotHandpayDelta, 
												   machinePayProgressivePayoutDelta, attendantPayProgressivePayoutDelta,
												   jackpotAmount, jackpotTriggerType, timeDelta);
			pkg->SetCardID(m_cardID, CARD_ID_LEN);
			CCumulativeResendItem* resenditem(new CCumulativeResendItem(m_slotMastID, pkg));

			//Tests
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotMastID(),
						   L"CCumulativeResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(coinInDelta == resenditem->GetCoinInDelta(),
						   L"CCumulativeResendItem - m_coinInDelta was not properly initialized in the constructor");
			Assert::IsTrue(coinOutDelta == resenditem->GetCoinOutDelta(),
						   L"CCumulativeResendItem - m_coinOutDelta was not properly initialized in the constructor");
			Assert::IsTrue(externalBonusingAttendantPayoutDelta == resenditem->GetExternalBonusingAttendantPayoutDelta(),
						   L"CCumulativeResendItem - m_externalBonusingAttendantPayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(externalBonusingMachinePayoutDelta == resenditem->GetExternalBonusingMachinePayoutDelta(),
						   L"CCumulativeResendItem - m_externalBonusingMachinePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(jackpotHandpayDelta == resenditem->GetJackpotHandpayDelta(),
						   L"CCumulativeResendItem - m_jackpotHandpayDelta was not properly initialized in the constructor");
			Assert::IsTrue(machinePayProgressivePayoutDelta == resenditem->GetMachinePayProgressivePayoutDelta(),
						   L"CCumulativeResendItem - m_machinePayProgressivePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(attendantPayProgressivePayoutDelta == resenditem->GetAttendantPayProgressivePayoutDelta(),
						   L"CCumulativeResendItem - m_attendantPayProgressivePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(jackpotAmount == resenditem->GetJackpotAmount(),
						   L"CCumulativeResendItem - m_jackpotAmount was not properly initialized in the constructor");
			Assert::IsTrue(timeDelta == resenditem->GetTimeDelta(),
						   L"CCumulativeResendItem - m_timeDelta was not properly initialized in the constructor");
			Assert::IsTrue(jackpotTriggerType == resenditem->GetJackpotTriggerType(),
						   L"CCumulativeResendItem - m_jackpotTriggerType was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete pkg;
		}

		TEST_METHOD(CumulativeResendItemConstructorFromBufferTest)
		{
			//Setup
			__int64 coinInDelta(1), coinOutDelta(2), externalBonusingAttendantPayoutDelta(3), 
				    externalBonusingMachinePayoutDelta(4), jackpotHandpayDelta(5), machinePayProgressivePayoutDelta(6), 
					attendantPayProgressivePayoutDelta(7), jackpotAmount(8), timeDelta(9);
			int jackpotTriggerType(1);
			CSpeedMediaCumulativePackage* pkg(NULL);
			pkg = new CSpeedMediaCumulativePackage(coinInDelta, coinOutDelta, externalBonusingAttendantPayoutDelta, 
												   externalBonusingMachinePayoutDelta, jackpotHandpayDelta, 
												   machinePayProgressivePayoutDelta, attendantPayProgressivePayoutDelta,
												   jackpotAmount, jackpotTriggerType, timeDelta);
			pkg->SetCardID(m_cardID, CARD_ID_LEN);
			CCumulativeResendItem* resenditem(new CCumulativeResendItem(m_slotMastID, pkg));

			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CCumulativeResendItem* resendItemFromBuffer(new CCumulativeResendItem(buffer, lengthOfBuffer));			

			//Tests
			Assert::IsTrue(resendItemFromBuffer->GetSlotMastID() == resenditem->GetSlotMastID(),
						   L"CCumulativeResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetCoinInDelta() == resenditem->GetCoinInDelta(),
						   L"CCumulativeResendItem - m_coinInDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetCoinOutDelta() == resenditem->GetCoinOutDelta(),
						   L"CCumulativeResendItem - m_coinOutDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetExternalBonusingAttendantPayoutDelta() == resenditem->GetExternalBonusingAttendantPayoutDelta(),
						   L"CCumulativeResendItem - m_externalBonusingAttendantPayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetExternalBonusingMachinePayoutDelta() == resenditem->GetExternalBonusingMachinePayoutDelta(),
						   L"CCumulativeResendItem - m_externalBonusingMachinePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetJackpotHandpayDelta() == resenditem->GetJackpotHandpayDelta(),
						   L"CCumulativeResendItem - m_jackpotHandpayDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetMachinePayProgressivePayoutDelta() == resenditem->GetMachinePayProgressivePayoutDelta(),
						   L"CCumulativeResendItem - m_machinePayProgressivePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetAttendantPayProgressivePayoutDelta() == resenditem->GetAttendantPayProgressivePayoutDelta(),
						   L"CCumulativeResendItem - m_attendantPayProgressivePayoutDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetJackpotAmount() == resenditem->GetJackpotAmount(),
						   L"CCumulativeResendItem - m_jackpotAmount was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetTimeDelta() == resenditem->GetTimeDelta(),
						   L"CCumulativeResendItem - m_timeDelta was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetJackpotTriggerType() == resenditem->GetJackpotTriggerType(),
						   L"CCumulativeResendItem - m_jackpotTriggerType was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete resendItemFromBuffer;
			delete pkg;
		}

		TEST_METHOD(GamePlayedResendItemConstructorTest)
		{
			//Setup
			CGamePlayedResendItem* resenditem(new CGamePlayedResendItem(m_slotMastID, m_cardID));

			//Tests
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotMastID(),
						   L"CGamePlayedResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue( memcmp( m_cardID, resenditem->GetCardID(), CARD_ID_LEN ) == 0,
							L"CGamePlayedResendItem - m_cardid was not properly initialized in the constructor" );

			//Cleanup
			delete resenditem;
		}

		TEST_METHOD(GamePlayedResendItemConstructorFromBufferTest)
		{
			//Setup
			CGamePlayedResendItem* resenditem(new CGamePlayedResendItem(m_slotMastID, m_cardID));

			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CGamePlayedResendItem* resendItemFromBuffer(new CGamePlayedResendItem(buffer, lengthOfBuffer));	

			//Tests
			Assert::IsTrue(resenditem->GetSlotMastID() == resendItemFromBuffer->GetSlotMastID(),
						   L"CGamePlayedResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue( memcmp( resenditem->GetCardID(), resendItemFromBuffer->GetCardID(), CARD_ID_LEN ) == 0,
							L"CGamePlayedResendItem - m_cardid was not properly initialized in the constructor" );

			//Cleanup
			delete resenditem;
			delete resendItemFromBuffer;
		}	

		TEST_METHOD(SentinelInformationResendItemConstructorTest)
		{
			//Setup
			CString OSVersion("1.0"), EXEVersion("2.0"),  SkinName("SkinName");
			BYTE displayOrientationID(3);			
			CSentinelInformationResendItem* resenditem(new CSentinelInformationResendItem(m_slotMastID, OSVersion, 
															EXEVersion, SkinName, displayOrientationID));

			//Tests
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotMastID(),
						   L"CSentinelInformationResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(OSVersion == resenditem->m_osVersion,
						   L"CSentinelInformationResendItem - m_osVersion was not properly initialized in the constructor");
			Assert::IsTrue(EXEVersion == resenditem->m_exeVersion,
						   L"CSentinelInformationResendItem - m_exeVersion was not properly initialized in the constructor");
			Assert::IsTrue(SkinName == resenditem->m_skinName,
						   L"CSentinelInformationResendItem - m_skinName was not properly initialized in the constructor");
			Assert::IsTrue(displayOrientationID == resenditem->GetDisplayOrientationID(),
						   L"CSentinelInformationResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
		}

		TEST_METHOD(SentinelInformationResendItemConstructorFromBufferTest)
		{
			//Setup
			CString OSVersion("1.0"), EXEVersion("2.0"),  SkinName("SkinName");
			BYTE displayOrientationID(3);			
			CSentinelInformationResendItem* resenditem(new CSentinelInformationResendItem(m_slotMastID, OSVersion, 
															EXEVersion, SkinName, displayOrientationID));

			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CSentinelInformationResendItem* resendItemFromBuffer(new CSentinelInformationResendItem(buffer, lengthOfBuffer));	

			//Tests
			Assert::IsTrue(resendItemFromBuffer->GetSlotMastID() == resenditem->GetSlotMastID(),
						   L"CSentinelInformationResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_osVersion == resenditem->m_osVersion,
						   L"CSentinelInformationResendItem - m_osVersion was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_exeVersion == resenditem->m_exeVersion,
						   L"CSentinelInformationResendItem - m_exeVersion was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_skinName == resenditem->m_skinName,
						   L"CSentinelInformationResendItem - m_skinName was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetDisplayOrientationID() == resenditem->GetDisplayOrientationID(),
						   L"CSentinelInformationResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete resendItemFromBuffer;
		}

		TEST_METHOD(SentinelInformationWithOrientationResendItemConstructorTest)
		{
			//Setup
			CString OSVersion("1.0"), EXEVersion("2.0"),  SkinName("SkinName");
			BYTE displayOrientationID(3);			
			CSentinelInformationWithOrientationResendItem* resenditem(new CSentinelInformationWithOrientationResendItem(m_slotMastID, OSVersion, 
															EXEVersion, SkinName, displayOrientationID));

			//Tests
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotMastID(),
						   L"CSentinelInformationWithOrientationResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(OSVersion == resenditem->m_osVersion,
						   L"CSentinelInformationWithOrientationResendItem - m_osVersion was not properly initialized in the constructor");
			Assert::IsTrue(EXEVersion == resenditem->m_exeVersion,
						   L"CSentinelInformationWithOrientationResendItem - m_exeVersion was not properly initialized in the constructor");
			Assert::IsTrue(SkinName == resenditem->m_skinName,
						   L"CSentinelInformationWithOrientationResendItem - m_skinName was not properly initialized in the constructor");
			Assert::IsTrue(displayOrientationID == resenditem->GetDisplayOrientationID(),
						   L"CSentinelInformationWithOrientationResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
		}

		TEST_METHOD(SentinelInformationWithOrientationResendItemConstructorFromBufferTest)
		{
			//Setup
			CString OSVersion("1.0"), EXEVersion("2.0"),  SkinName("SkinName");
			BYTE displayOrientationID(3);			
			CSentinelInformationWithOrientationResendItem* resenditem(new CSentinelInformationWithOrientationResendItem(m_slotMastID, OSVersion, 
															EXEVersion, SkinName, displayOrientationID));

			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CSentinelInformationWithOrientationResendItem* resendItemFromBuffer(new CSentinelInformationWithOrientationResendItem(buffer, lengthOfBuffer));	

			//Tests
			Assert::IsTrue(resendItemFromBuffer->GetSlotMastID() == resenditem->GetSlotMastID(),
						   L"CSentinelInformationWithOrientationResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_osVersion == resenditem->m_osVersion,
						   L"CSentinelInformationWithOrientationResendItem - m_osVersion was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_exeVersion == resenditem->m_exeVersion,
						   L"CSentinelInformationWithOrientationResendItem - m_exeVersion was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_skinName == resenditem->m_skinName,
						   L"CSentinelInformationWithOrientationResendItem - m_skinName was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->GetDisplayOrientationID() == resenditem->GetDisplayOrientationID(),
						   L"CSentinelInformationWithOrientationResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
			delete resendItemFromBuffer;
		}

		TEST_METHOD(SentinelStatusResendItemConstructorTest)
		{
			//Setup
			CString Status("Status");		
			CSentinelStatusResendItem* resenditem(new CSentinelStatusResendItem(m_slotMastID, Status));

			//Tests
			Assert::IsTrue(m_slotMastID == resenditem->GetSlotMastID(),
						   L"CSentinelStatusResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(Status == resenditem->m_status,
						   L"CSentinelStatusResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
		}

		TEST_METHOD(SentinelStatusResendItemConstructorFromBufferTest)
		{
			//Setup
			CString Status("Status");		
			CSentinelStatusResendItem* resenditem(new CSentinelStatusResendItem(m_slotMastID, Status));

			int lengthOfBuffer(0);
			BYTE* buffer(resenditem->GetBuffer(lengthOfBuffer));
			CSentinelStatusResendItem* resendItemFromBuffer(new CSentinelStatusResendItem(buffer, lengthOfBuffer));	

			//Tests
			Assert::IsTrue(resendItemFromBuffer->GetSlotMastID() == resenditem->GetSlotMastID(),
						   L"CSentinelStatusResendItem - m_slotmastid was not properly initialized in the constructor");
			Assert::IsTrue(resendItemFromBuffer->m_status == resenditem->m_status,
						   L"CSentinelStatusResendItem - m_osVersion was not properly initialized in the constructor");

			//Cleanup
			delete resenditem;
		}

		TEST_METHOD(CardInResendItemOffsetTest)
		{
			const int OFFSET = 68;
			//Setup
			CCardInResendItem resendItem(m_slotMastID, m_cardID, false, m_transactionTime);

			resendItem.SetOffset(OFFSET);

			//Test
			Assert::IsTrue(OFFSET == resendItem.GetOffset(), L"CCardInResendItem - Offset was not properly initialized in the constructor");
		}

		TEST_METHOD(CardInResendItemSetItemNumberTest)
		{
			//Setup
			DWORD itemNumber = 10;
			CCardInResendItem resendItem(m_slotMastID, m_cardID, false, m_transactionTime);
			resendItem.SetItemNumber(itemNumber);

			//Test
			Assert::IsTrue(itemNumber == resendItem.GetItemNumber(), L"m_itemNumber was not properly set");
		}

		TEST_METHOD(CardInResendItemSetOffsetTest)
		{
			//Setup
			DWORD offset = 11;
			CCardInResendItem resendItem(m_slotMastID, m_cardID, false, m_transactionTime);
			resendItem.SetOffset(offset);

			//Test
			Assert::IsTrue(offset == resendItem.GetOffset(), L"m_offset was not properly set");
		}

		TEST_METHOD(CardInResendItemIsCardTransactionTest)
		{
			//Setup
			CCardInResendItem cardInResendItem(m_slotMastID, m_cardID, false, m_transactionTime);

			//Test
			Assert::IsTrue(cardInResendItem.IsCardTransaction(), L"IsCardTransaction function failed");
		}

		TEST_METHOD(CardOutResendItemIsCardTransactionTest)
		{
			//Setup
			CCardOutResendItem cardOutResendItem(m_slotMastID, m_cardID, m_transactionTime);

			//Test
			Assert::IsTrue(cardOutResendItem.IsCardTransaction(), L"IsCardTransaction function failed");
		}
	};
}
#endif //TODO-PORT