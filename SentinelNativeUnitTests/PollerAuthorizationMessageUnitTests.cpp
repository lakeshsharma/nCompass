#if 000//TODO-PORT
#include "stdafx.h"

#include "../PollerProtocol/PollerAuthorizationMessage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SentinelNativeUnitTests
{
	TEST_CLASS(PollerAuthorizationMessageUnitTests)
	{
	public:
		TEST_METHOD(PollerAuthorizationMessageIsAuthorizedConstructorTest)
		{
			//TEST
			const byte ID[] = {0x14,0x41,0x54,0x32,0x35,0x35,0x30,0x30,0x31,0x35,0x39,
				               0x30,0x30,0x30,0x31,0x38,0x30,0x30,0x32,0x34,0x38,0x01};
			LPCTSTR transactionID(L"AT255001590001800248");
			CString authorizeCaseToString(L"POLLER_MSG_AUTHORIZATION_RESPONSE");

			//SETUP
			CPollerAuthorizationMessage authorize(ID);

			//TEST
			Assert::IsTrue(authorize.IsAuthorized(), L"m_authorizationGranted was not setup properly");
			Assert::IsTrue(authorize.GetTransactionID() == transactionID, L"m_pbtID was not setup properly");
			Assert::IsTrue(authorizeCaseToString == CPollerAuthorizationMessage::PollerMsgTypeToString(POLLER_MSG_AUTHORIZATION_RESPONSE), 
				L"POLLER_MSG_AUTHORIZATION_RESPONSE CASETOSTRING was not setup properly");
		}

		TEST_METHOD(PollerAuthorizationMessageIsNotAuthorizedConstructorTest)
		{
			//TEST
			const byte ID[] = {0x14,0x41,0x54,0x32,0x35,0x35,0x30,0x30,0x31,0x35,0x39,
				               0x30,0x30,0x30,0x31,0x38,0x30,0x30,0x32,0x34,0x38,0x00};
			LPCTSTR transactionID(L"AT255001590001800248");
			CString authorizeCaseToString(L"POLLER_MSG_AUTHORIZATION_RESPONSE");

			//SETUP
			CPollerAuthorizationMessage authorize(ID);

			//TEST
			Assert::IsFalse(authorize.IsAuthorized(), L"m_authorizationGranted was not setup properly");
			Assert::IsTrue(authorize.GetTransactionID() == transactionID, L"m_pbtID was not setup properly");
			Assert::IsTrue(authorizeCaseToString == CPollerAuthorizationMessage::PollerMsgTypeToString(POLLER_MSG_AUTHORIZATION_RESPONSE), 
				L"POLLER_MSG_AUTHORIZATION_RESPONSE CASETOSTRING was not setup properly");
		}
	};
}
#endif //TODO-PORT