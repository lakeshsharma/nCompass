#include "stdafx.h"
#include "PollerProtocol/MobileConnectProtocol/MobilePBTSessionInfo.h"

// Unit testing includes.
#include "UnitTestingOperatingSystem.h"

#include "gtest/gtest.h"


namespace SentinelNativeUnitTests
{
    class MobilePBTSessionInfo : public ::testing::Test {
		protected:
			MobilePBTSessionInfo(){}

			virtual void SetUp()
			{
			}

			virtual void TearDown() 
			{
			}

        void MobilePBTSessionInfo_Default()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.m_mobileTransferType == PersonalBankerTransferType::Undeclared) << "m_mobileTransferType";
			ASSERT_TRUE(obj.m_mobileAmountInPennies == 0) << "m_mobileAmountInPennies";
			ASSERT_FALSE(obj.m_mobileFundsTransferBalanceRequest) << "m_mobileFundsTransferBalanceRequest";
			ASSERT_FALSE(obj.m_mobileFundTransferRequest) << "m_mobileFundTransferRequest";
			ASSERT_FALSE(obj.m_mobileDenomMismatch) << "m_mobileDenomMismatch";
			ASSERT_FALSE(obj.m_mobileSASError) << "m_mobileSASError";
			ASSERT_FALSE(obj.m_mobilePBSession) << "m_mobilePBSession";
			ASSERT_TRUE(obj.m_cashable == 0) << "m_cashable";
			ASSERT_TRUE(obj.m_cashPromo == 0) << "m_cashPromo";
			ASSERT_TRUE(obj.m_nonCashablePromo == 0) << "m_nonCashablePromo";
			ASSERT_TRUE(obj.m_isWallet_2_0_CompatiblePDS) << "m_isWallet_2_0_CompatiblePDS";
		}

		void MobilePBTSessionInfo_GetMobileTransferType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileTransferType = PersonalBankerTransferType::PointsFromGame;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetMobileTransferType() == PersonalBankerTransferType::PointsFromGame) << "GetMobileTransferType()";
		}

		void MobilePBTSessionInfo_SetMobileTransferType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileTransferType(PersonalBankerTransferType::PointsFromGame);
			ASSERT_TRUE(obj.m_mobileTransferType == PersonalBankerTransferType::PointsFromGame) << "SetMobileTransferType()";
		}

		void MobilePBTSessionInfo_GetMobileAmountInPennies()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileAmountInPennies = 50;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetMobileAmountInPennies() == 50) << "GetMobileAmountInPennies()";
		}

		void MobilePBTSessionInfo_SetMobileAmountInPennies()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileAmountInPennies(100);
			ASSERT_TRUE(obj.m_mobileAmountInPennies == 100) << "SetMobileAmountInPennies()";
		}

		void MobilePBTSessionInfo_IsMobileFundsTransferBalanceRequest()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileFundsTransferBalanceRequest = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsMobileFundsTransferBalanceRequest()) << "IsMobileFundsTransferBalanceRequest()";
		}

		void MobilePBTSessionInfo_SetMobileFundsTransferBalanceRequest()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileFundsTransferBalanceRequest(true);
			ASSERT_TRUE(obj.m_mobileFundsTransferBalanceRequest) << "SetMobileFundsTransferBalanceRequest()";
		}

		void MobilePBTSessionInfo_IsMobileFundTransferRequest()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileFundTransferRequest = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsMobileFundTransferRequest()) << "IsMobileFundTransferRequest()";
		}

		void MobilePBTSessionInfo_SetMobileFundTransferRequest()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileFundTransferRequest(true);
			ASSERT_TRUE(obj.m_mobileFundTransferRequest) << "SetMobileFundTransferRequest()";
		}

		void MobilePBTSessionInfo_IsMobilePBSession()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobilePBSession = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsMobilePBSession()) << "IsMobilePBSession()";
		}

		void MobilePBTSessionInfo_SetMobilePBSession()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobilePBSession(true);
			ASSERT_TRUE(obj.m_mobilePBSession) << "SetMobilePBSession()";
		}

		void MobilePBTSessionInfo_IsMobileDenomMismatch()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileDenomMismatch = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsMobileDenomMismatch()) << "IsMobileDenomMismatch()";
		}

		void MobilePBTSessionInfo_SetMobileDenomMismatch()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileDenomMismatch(true);
			ASSERT_TRUE(obj.m_mobileDenomMismatch) << "SetMobileDenomMismatch()";
		}

		void MobilePBTSessionInfo_IsMobileSASError()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileSASError = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsMobileSASError()) << "IsMobileSASError()";
		}

		void MobilePBTSessionInfo_SetMobileSASError()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileSASError(true);
			ASSERT_TRUE(obj.m_mobileSASError) << "SetMobileSASError()";
		}

		void MobilePBTSessionInfo_GetMobilePin()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobilePin = "1234";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetMobilePin() == "1234") << "GetMobilePin()";
		}

		void MobilePBTSessionInfo_SetMobilePin()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobilePin("1234");
			ASSERT_TRUE(obj.m_mobilePin == "1234") << "SetMobilePin()";
		}

		void MobilePBTSessionInfo_GetMobileFundType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileFundType = "type1";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetMobileFundType() == "type1") << "GetMobileFundType()";
		}

		void MobilePBTSessionInfo_SetMobileFundType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileFundType("type1");
			ASSERT_TRUE(obj.m_mobileFundType == "type1") << "SetMobileFundType()";
		}

		void MobilePBTSessionInfo_GetMobileTransferDirection()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_mobileTransferDirection = "ToGame";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetMobileTransferDirection() == "ToGame") << "GetMobileTransferDirection()";
		}

		void MobilePBTSessionInfo_SetMobileTransferDirection()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetMobileTransferDirection("ToGame");
			ASSERT_TRUE(obj.m_mobileTransferDirection == "ToGame") << "SetMobileTransferDirection()";
		}

		void MobilePBTSessionInfo_GetError()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_errorMessage = "Error1";

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetError() == "Error1") << "GetError()";
		}

		void MobilePBTSessionInfo_SetError()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetError("Error1");
			ASSERT_TRUE(obj.m_errorMessage == "Error1") << "SetError()";
		}

		void MobilePBTSessionInfo_ClearError()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_errorMessage = "Error1";

			// Perform operation(s) and test(s).
			obj.ClearError();
			ASSERT_TRUE(obj.m_errorMessage.empty()) << "ClearError()";
		}

		void MobilePBTSessionInfo_SetBalances()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetBalances(100, 200, 300);
			ASSERT_TRUE(obj.m_cashable == 100) << "m_cashable";
			ASSERT_TRUE(obj.m_cashPromo == 200) << "m_cashPromo";
			ASSERT_TRUE(obj.m_nonCashablePromo == 300) << "m_nonCashablePromo";
		}

		void MobilePBTSessionInfo_ClearBalances()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_cashable = 100;
			obj.m_cashPromo = 200;
			obj.m_nonCashablePromo = 300;

			// Perform operation(s) and test(s).
			obj.ClearBalances();
			ASSERT_TRUE(obj.m_cashable == 0) << "m_cashable";
			ASSERT_TRUE(obj.m_cashPromo == 0) << "m_cashPromo";
			ASSERT_TRUE(obj.m_nonCashablePromo == 0) << "m_nonCashablePromo";
		}

		void MobilePBTSessionInfo_GetCashableBalance()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_cashable = 10;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetCashableBalance() == 10) << "GetCashableBalance()";
		}

		void MobilePBTSessionInfo_GetCashPromoBalance()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_cashPromo = 10;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetCashPromoBalance() == 10) << "GetCashPromoBalance()";
		}

		void MobilePBTSessionInfo_GetNonCashablePromoBalance()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_nonCashablePromo = 10;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.GetNonCashablePromoBalance() == 10) << "GetNonCashablePromoBalance()";
		}

		void MobilePBTSessionInfo_IsWallet_2_0_CompatiblePDS()
		{
			// Setup.
			CMobilePBTSessionInfo obj;
			obj.m_isWallet_2_0_CompatiblePDS = true;

			// Perform operation(s) and test(s).
			ASSERT_TRUE(obj.IsWallet_2_0_CompatiblePDS()) << "IsWallet_2_0_CompatiblePDS() - true";

			// Setup.
			obj.m_isWallet_2_0_CompatiblePDS = false;

			// Perform operation(s) and test(s).
			ASSERT_FALSE(obj.IsWallet_2_0_CompatiblePDS()) << "IsWallet_2_0_CompatiblePDS() - false";
		}

		void MobilePBTSessionInfo_SetWallet_2_0_CompatiblePDS()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			obj.SetWallet_2_0_CompatiblePDS(true);
			ASSERT_TRUE(obj.m_isWallet_2_0_CompatiblePDS) << "SetWallet_2_0_CompatiblePDS(true)";

			// Perform operation(s) and test(s).
			obj.SetWallet_2_0_CompatiblePDS(false);
			ASSERT_FALSE(obj.m_isWallet_2_0_CompatiblePDS) << "SetWallet_2_0_CompatiblePDS(false)";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_InvalidTransferDirection()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("Cash", "togame");
			ASSERT_TRUE(transferType == Undeclared) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_InvalidFundType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("cash", "ToGame");
			ASSERT_TRUE(transferType == Undeclared) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_Cash()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("Cash", "ToGame");
			ASSERT_TRUE(transferType == CashToGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_CashablePromo()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("CashablePromo", "ToGame");
			ASSERT_TRUE(transferType == PointsToGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_NonCashablePromo()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("NonCashablePromo", "ToGame");
			ASSERT_TRUE(transferType == PromoDollarsToGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_InvalidFundType()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("cash", "FromGame");
			ASSERT_TRUE(transferType == Undeclared) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_Cash()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("Cash", "FromGame");
			ASSERT_TRUE(transferType == CashFromGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_CashablePromo()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("CashablePromo", "FromGame");
			ASSERT_TRUE(transferType == PointsFromGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_NonCashablePromo()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("NonCashablePromo", "FromGame");
			ASSERT_TRUE(transferType == PromoDollarsFromGame) << "ConvertToPBTransferType()";
		}

		void MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_All()
		{
			// Setup.
			CMobilePBTSessionInfo obj;

			// Perform operation(s) and test(s).
			PersonalBankerTransferType transferType = obj.ConvertToPBTransferType("All", "FromGame");
			ASSERT_TRUE(transferType == AllFromGame) << "ConvertToPBTransferType()";
		}
	};

// #pragma region CMobilePBTSessionInfo
		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::CMobilePBTSessionInfo()
		/// Test for initial values of object created via default constructor.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_Default)
		{
			MobilePBTSessionInfo_Default();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetMobileTransferType()
		/// Test for GetMobileTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetMobileTransferType)
		{
			MobilePBTSessionInfo_GetMobileTransferType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileTransferType()
		/// Test for SetMobileTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileTransferType)
		{
			MobilePBTSessionInfo_SetMobileTransferType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetMobileAmountInPennies()
		/// Test for GetMobileAmountInPennies() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetMobileAmountInPennies)
		{
			MobilePBTSessionInfo_GetMobileAmountInPennies();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileAmountInPennies()
		/// Test for SetMobileAmountInPennies() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileAmountInPennies)
		{
			MobilePBTSessionInfo_SetMobileAmountInPennies();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsMobileFundsTransferBalanceRequest()
		/// Test for IsMobileFundsTransferBalanceRequest() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsMobileFundsTransferBalanceRequest)
		{
			MobilePBTSessionInfo_IsMobileFundsTransferBalanceRequest();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileFundsTransferBalanceRequest()
		/// Test for SetMobileFundsTransferBalanceRequest() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileFundsTransferBalanceRequest)
		{
			MobilePBTSessionInfo_SetMobileFundsTransferBalanceRequest();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsMobileFundTransferRequest()
		/// Test for IsMobileFundTransferRequest() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsMobileFundTransferRequest)
		{
			MobilePBTSessionInfo_IsMobileFundTransferRequest();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileFundTransferRequest()
		/// Test for SetMobileFundTransferRequest() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileFundTransferRequest)
		{
			MobilePBTSessionInfo_SetMobileFundTransferRequest();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsMobilePBSession()
		/// Test for IsMobilePBSession() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsMobilePBSession)
		{
			MobilePBTSessionInfo_IsMobilePBSession();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobilePBSession()
		/// Test for SetMobilePBSession() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobilePBSession)
		{
			MobilePBTSessionInfo_SetMobilePBSession();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsMobileDenomMismatch()
		/// Test for IsMobileDenomMismatch() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsMobileDenomMismatch)
		{
			MobilePBTSessionInfo_IsMobileDenomMismatch();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileDenomMismatch()
		/// Test for SetMobileDenomMismatch() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileDenomMismatch)
		{
			MobilePBTSessionInfo_SetMobileDenomMismatch();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsMobileSASError()
		/// Test for IsMobileSASError() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsMobileSASError)
		{
			MobilePBTSessionInfo_IsMobileSASError();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileSASError()
		/// Test for SetMobileSASError() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileSASError)
		{
			MobilePBTSessionInfo_SetMobileSASError();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetMobilePin()
		/// Test for GetMobilePin() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetMobilePin)
		{
			MobilePBTSessionInfo_GetMobilePin();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobilePin()
		/// Test for SetMobilePin() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobilePin)
		{
			MobilePBTSessionInfo_SetMobilePin();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetMobileFundType()
		/// Test for GetMobileFundType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetMobileFundType)
		{
			MobilePBTSessionInfo_GetMobileFundType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileFundType()
		/// Test for SetMobileFundType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileFundType)
		{
			MobilePBTSessionInfo_SetMobileFundType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetMobileTransferDirection()
		/// Test for GetMobileTransferDirection() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetMobileTransferDirection)
		{
			MobilePBTSessionInfo_GetMobileTransferDirection();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetMobileTransferDirection()
		/// Test for SetMobileTransferDirection() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetMobileTransferDirection)
		{
			MobilePBTSessionInfo_SetMobileTransferDirection();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetError()
		/// Test for GetError() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetError)
		{
			MobilePBTSessionInfo_GetError();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetError()
		/// Test for SetError() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetError)
		{
			MobilePBTSessionInfo_SetError();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ClearError()
		/// Test for ClearError() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ClearError)
		{
			MobilePBTSessionInfo_ClearError();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetBalances()
		/// Test for SetBalances() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetBalances)
		{
			MobilePBTSessionInfo_SetBalances();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ClearBalances()
		/// Test for ClearBalances() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ClearBalances)
		{
			MobilePBTSessionInfo_ClearBalances();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetCashableBalance()
		/// Test for GetCashableBalance() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetCashableBalance)
		{
			MobilePBTSessionInfo_GetCashableBalance();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetCashPromoBalance()
		/// Test for GetCashPromoBalance() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetCashPromoBalance)
		{
			MobilePBTSessionInfo_GetCashPromoBalance();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::GetNonCashablePromoBalance()
		/// Test for GetNonCashablePromoBalance() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_GetNonCashablePromoBalance)
		{
			MobilePBTSessionInfo_GetNonCashablePromoBalance();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::IsWallet_2_0_CompatiblePDS()
		/// Test for IsWallet_2_0_CompatiblePDS() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_IsWallet_2_0_CompatiblePDS)
		{
			MobilePBTSessionInfo_IsWallet_2_0_CompatiblePDS();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::SetWallet_2_0_CompatiblePDS()
		/// Test for SetWallet_2_0_CompatiblePDS() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_SetWallet_2_0_CompatiblePDS)
		{
			MobilePBTSessionInfo_SetWallet_2_0_CompatiblePDS();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_InvalidTransferDirection)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_InvalidTransferDirection();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_ToGame_InvalidFundType)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_InvalidFundType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_ToGame_Cash)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_Cash();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_ToGame_CashablePromo)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_CashablePromo();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_ToGame_NonCashablePromo)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_ToGame_NonCashablePromo();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_FromGame_InvalidFundType)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_InvalidFundType();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_FromGame_Cash)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_Cash();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_FromGame_CashablePromo)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_CashablePromo();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_FromGame_NonCashablePromo)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_NonCashablePromo();
		}

		/// <summary>
		/// Functional test for:
		///     CMobilePBTSessionInfo::ConvertToPBTransferType()
		/// Test for ConvertToPBTransferType() method.
		/// </summary>
		TEST_F(MobilePBTSessionInfo, CMobilePBTSessionInfo_ConvertToPBTransferType_FromGame_All)
		{
			MobilePBTSessionInfo_ConvertToPBTransferType_FromGame_All();
		}

// #pragma endregion CMobilePBTSessionInfo
}
