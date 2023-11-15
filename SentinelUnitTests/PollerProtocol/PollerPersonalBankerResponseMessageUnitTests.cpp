#include "stdafx.h"
#include "PollerProtocol/PollerPersonalBankerResponseMessage.h"

// Unit testing includes.
#include "gtest/gtest.h"

using namespace std;

namespace SentinelNativeUnitTests
{
	class PollerPersonalBankerResponseMessage : public ::testing::Test
	{
	public:
	};

		/// <summary>
		/// Test the implementation of:
		///     CPollerPersonalBankerResponseMessage::CPollerPersonalBankerResponseMessage(const byte *response, ...)
		///     CPollerPersonalBankerResponseMessage::GetPBInfo()
		/// </summary>
		/// <remarks>
		/// Full code path coverage test.
		/// Full functional test.
		/// Picked response values to test edge conditions.
		/// </remarks>
		TEST(PollerPersonalBankerResponseMessage, PollerPersonalBankerResponseMessageTest)
		{
			typedef struct
			{
				// Inputs.
				const byte *m_response;
				const int m_bPromoValues;

				// Expected results.
				const byte m_cardNumberExpected[CARD_ID_LEN];
				const PBError m_pbErrorExpected;
				const DWORD m_cashBalanceExpected;
				const DWORD m_pbFlagsExpected;
				const DWORD m_promo2BalanceExpected;
				const DWORD m_pointsAvailableExpected;
				const DWORD m_pointsPerDollarExpected;
				const DWORD m_sequenceNumberExpected;
				const DWORD m_minimumPointsExpected;
				const DWORD m_maximumPointsExpected;
				const DWORD m_minimumCashDownExpected;
				const DWORD m_maximumCashDownExpected;
				const DWORD m_minimumCashUpExpected;
				const DWORD m_maximumCashUpExpected;
				const DWORD m_minimumPromoDownExpected;
				const DWORD m_maximumPromoDownExpected;
				const int m_pbIDLengthExpected;
				const char m_pbIDExpected[MAX_PBID_LENGTH];
				const DWORD m_minimumPromoUpExpected;
				const DWORD m_maximumPromoUpExpected;
				bool m_externalCreditProviderEnabled;
				DWORD m_creditFee;
				bool m_creditFeeFixed;
				DWORD m_extCreditTransferType;
                WORD m_bufferLen;
			} TestDataType;

			const byte response0[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // Card number.
				1, // Error code.
				2222 % 256, 2222 / 256, 0, 0, // CashBalance -- more than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				3, 0, 0, 0, // PBFlags.
				4444 % 256, 4444 / 256, 0, 0, // Promo2Balance -- more than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				55, 0, 0, 0, // PointsAvailable -- more than MaximumPoints.
				6, 0, 0, 0, // PointsPerDollar.
				7, 0, 0, 0, // SequenceNumber.
				8, 0, 0, 0, // MinimumPoints.
				9, 0, 0, 0, // MaximumPoints.
				10, 0, 0, 0, // MinimumCashDown.
				11, 0, 0, 0, // MaximumCashDown.
				12, 0, 0, 0, // MinimumCashUp.
				13, 0, 0, 0, // MaximumCashUp.
				14, 0, 0, 0, // MinimumPromoDown.
				15, 0, 0, 0, // MaximumPromoDown.
				16, // PBIDLength.
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', // PBID.
			};

			const byte response1[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // Card number.
				1, // Error code.
				222, 0, 0, 0, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				3, 0, 0, 0, // PBFlags.
				444 % 256, 444 / 256, 0, 0, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				5, 0, 0, 0, // PointsAvailable -- less than MaximumPoints.
				6, 0, 0, 0, // PointsPerDollar.
				7, 0, 0, 0, // SequenceNumber.
				8, 0, 0, 0, // MinimumPoints.
				9, 0, 0, 0, // MaximumPoints.
				10, 0, 0, 0, // MinimumCashDown.
				11, 0, 0, 0, // MaximumCashDown.
				12, 0, 0, 0, // MinimumCashUp.
				13, 0, 0, 0, // MaximumCashUp.
				14, 0, 0, 0, // MinimumPromoDown.
				15, 0, 0, 0, // MaximumPromoDown.
				16, // PBIDLength.
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', // PBID.
			};

			const byte response2[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // Card number.
				1, // Error code.
				2222 % 256, 2222 / 256, 0, 0, // CashBalance -- more than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				3, 0, 0, 0, // PBFlags.
				4444 % 256, 4444 / 256, 0, 0, // Promo2Balance -- more than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				55, 0, 0, 0, // PointsAvailable -- more than MaximumPoints.
				6, 0, 0, 0, // PointsPerDollar.
				7, 0, 0, 0, // SequenceNumber.
				8, 0, 0, 0, // MinimumPoints.
				9, 0, 0, 0, // MaximumPoints.
				10, 0, 0, 0, // MinimumCashDown.
				11, 0, 0, 0, // MaximumCashDown.
				12, 0, 0, 0, // MinimumCashUp.
				13, 0, 0, 0, // MaximumCashUp.
				14, 0, 0, 0, // MinimumPromoDown.
				15, 0, 0, 0, // MaximumPromoDown.
				16, // PBIDLength.
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', // PBID.
				18, 0, 0, 0, // MinimumPromoUp.
				19, 0, 0, 0, // MaximumPromoUp.
			};

			const byte response3[] = {
				0x12, 0x34, 0x56, 0x78, 0x90, // Card number.
				1, // Error code.
				222, 0, 0, 0, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				3, 0, 0, 0, // PBFlags.
				444 % 256, 444 / 256, 0, 0, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				5, 0, 0, 0, // PointsAvailable -- less than MaximumPoints.
				6, 0, 0, 0, // PointsPerDollar.
				7, 0, 0, 0, // SequenceNumber.
				8, 0, 0, 0, // MinimumPoints.
				9, 0, 0, 0, // MaximumPoints.
				10, 0, 0, 0, // MinimumCashDown.
				11, 0, 0, 0, // MaximumCashDown.
				12, 0, 0, 0, // MinimumCashUp.
				13, 0, 0, 0, // MaximumCashUp.
				14, 0, 0, 0, // MinimumPromoDown.
				15, 0, 0, 0, // MaximumPromoDown.
				66, // PBIDLength.
				'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', // PBID.
				18, 0, 0, 0, // MinimumPromoUp.
				19, 0, 0, 0, // MaximumPromoUp.
			};

			const byte response4[] = {
				0x00, 0x00, 0x00, 0x00, 0x00, // Card number.
				0, // Error code.
				0, 0, 0, 0, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PBFlags.
				0, 0, 0, 0, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PointsAvailable -- less than MaximumPoints.
				0, 0, 0, 0, // PointsPerDollar.
				0, 0, 0, 0, // SequenceNumber.
				0, 0, 0, 0, // MinimumPoints.
				0, 0, 0, 0, // MaximumPoints.
				0, 0, 0, 0, // MinimumCashDown.
				0, 0, 0, 0, // MaximumCashDown.
				0, 0, 0, 0, // MinimumCashUp.
				0, 0, 0, 0, // MaximumCashUp.
				0, 0, 0, 0, // MinimumPromoDown.
				0, 0, 0, 0, // MaximumPromoDown.
				0, // PBIDLength.
				// PBID with length of 0.
			};

			const byte response5[] = {
				0x00, 0x00, 0x00, 0x00, 0x00, // Card number.
				0, // Error code.
				0, 0, 0, 0, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PBFlags.
				0, 0, 0, 0, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PointsAvailable -- less than MaximumPoints.
				0, 0, 0, 0, // PointsPerDollar.
				0, 0, 0, 0, // SequenceNumber.
				0, 0, 0, 0, // MinimumPoints.
				0, 0, 0, 0, // MaximumPoints.
				0, 0, 0, 0, // MinimumCashDown.
				0, 0, 0, 0, // MaximumCashDown.
				0, 0, 0, 0, // MinimumCashUp.
				0, 0, 0, 0, // MaximumCashUp.
				0, 0, 0, 0, // MinimumPromoDown.
				0, 0, 0, 0, // MaximumPromoDown.
				0, // PBIDLength.
				// PBID with length of 0.
				0, 0, 0, 0, // MinimumPromoUp.
				0, 0, 0, 0, // MaximumPromoUp.
			};

			const byte response6[] = {
				0x00, 0x00, 0x00, 0x00, 0x00, // Card number.
				0, // Error code.
				0, 0, 0, 0, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PBFlags.
				0, 0, 0, 0, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0, 0, 0, 0, // PointsAvailable -- less than MaximumPoints.
				0, 0, 0, 0, // PointsPerDollar.
				0, 0, 0, 0, // SequenceNumber.
				0, 0, 0, 0, // MinimumPoints.
				0, 0, 0, 0, // MaximumPoints.
				0, 0, 0, 0, // MinimumCashDown.
				0, 0, 0, 0, // MaximumCashDown.
				0, 0, 0, 0, // MinimumCashUp.
				0, 0, 0, 0, // MaximumCashUp.
				0, 0, 0, 0, // MinimumPromoDown.
				0, 0, 0, 0, // MaximumPromoDown.
				0, // PBIDLength.
				// PBID with length of 0.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
			};

			const byte response7[] = {
				0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
				0xff, // Error code.
				0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PBFlags.
				0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
				0xff, 0xff, 0xff, 0xff, // SequenceNumber.
				0xff, 0xff, 0xff, 0xff, // MinimumPoints.
				0xff, 0xff, 0xff, 0xff, // MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
				0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
				0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
				0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
				0xff, // PBIDLength.
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
			};

			const byte response8[] = {
				0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
				0xff, // Error code.
				0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PBFlags.
				0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
				0xff, 0xff, 0xff, 0xff, // SequenceNumber.
				0xff, 0xff, 0xff, 0xff, // MinimumPoints.
				0xff, 0xff, 0xff, 0xff, // MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
				0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
				0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
				0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
				0xff, // PBIDLength.
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
			};

			const byte response9[] = {
				0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
				0xff, // Error code.
				0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PBFlags.
				0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
				0xff, 0xff, 0xff, 0xff, // SequenceNumber.
				0xff, 0xff, 0xff, 0xff, // MinimumPoints.
				0xff, 0xff, 0xff, 0xff, // MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
				0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
				0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
				0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
				0xff, // PBIDLength.
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
				0x00, //m_externalCreditProviderEnabled
				0x12, 0x34, 0x56, 0x78, //m_creditFee
				0x01, //m_creditFeeFixed
			};

			const byte response10[] = {
				0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
				0xff, // Error code.
				0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PBFlags.
				0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
				0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
				0xff, 0xff, 0xff, 0xff, // SequenceNumber.
				0xff, 0xff, 0xff, 0xff, // MinimumPoints.
				0xff, 0xff, 0xff, 0xff, // MaximumPoints.
				0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
				0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
				0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
				0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
				0xff, // PBIDLength.
				0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
				0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
				0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
				0x01, //m_externalCreditProviderEnabled
				0x87, 0x65, 0x43, 0x21, //m_creditFee
				0x00, //m_creditFeeFixed
			};

            const byte response11[] = {
                0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
                0xff, // Error code.
                0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PBFlags.
                0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
                0xff, 0xff, 0xff, 0xff, // SequenceNumber.
                0xff, 0xff, 0xff, 0xff, // MinimumPoints.
                0xff, 0xff, 0xff, 0xff, // MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
                0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
                0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
                0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
                0xff, // PBIDLength.
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
                0x01, //m_externalCreditProviderEnabled
                0x87, 0x65, 0x43, 0x21, //m_creditFee
                0x00, //m_creditFeeFixed
                0x00,//M_externalCreditToGameType
            };

            const byte response12[] = {
                0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
                0xff, // Error code.
                0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PBFlags.
                0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
                0xff, 0xff, 0xff, 0xff, // SequenceNumber.
                0xff, 0xff, 0xff, 0xff, // MinimumPoints.
                0xff, 0xff, 0xff, 0xff, // MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
                0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
                0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
                0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
                0xff, // PBIDLength.
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
                0x01, //m_externalCreditProviderEnabled
                0x87, 0x65, 0x43, 0x21, //m_creditFee
                0x00, //m_creditFeeFixed
                0x01,//M_externalCreditToGameType
            };

            const byte response13[] = {
                0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
                0xff, // Error code.
                0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PBFlags.
                0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
                0xff, 0xff, 0xff, 0xff, // SequenceNumber.
                0xff, 0xff, 0xff, 0xff, // MinimumPoints.
                0xff, 0xff, 0xff, 0xff, // MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
                0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
                0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
                0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
                0xff, // PBIDLength.
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
                0x01, //m_externalCreditProviderEnabled
                0x87, 0x65, 0x43, 0x21, //m_creditFee
                0x00, //m_creditFeeFixed
                0x02,//M_externalCreditToGameType
            };

                const byte response14[] = {
                0xff, 0xff, 0xff, 0xff, 0xff, // Card number.
                0xff, // Error code.
                0xff, 0xff, 0xff, 0xff, // CashBalance -- less than MaximumCashDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PBFlags.
                0xff, 0xff, 0xff, 0xff, // Promo2Balance -- less than MaximumPromoDown * ONE_DOLLAR_IN_CENTS.
                0xff, 0xff, 0xff, 0xff, // PointsAvailable -- less than MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // PointsPerDollar.
                0xff, 0xff, 0xff, 0xff, // SequenceNumber.
                0xff, 0xff, 0xff, 0xff, // MinimumPoints.
                0xff, 0xff, 0xff, 0xff, // MaximumPoints.
                0xff, 0xff, 0xff, 0xff, // MinimumCashDown.
                0xff, 0xff, 0xff, 0xff, // MaximumCashDown.
                0xff, 0xff, 0xff, 0xff, // MinimumCashUp.
                0xff, 0xff, 0xff, 0xff, // MaximumCashUp.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoDown.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoDown.
                0xff, // PBIDLength.
                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // PBID with length of 20.
                0xff, 0xff, 0xff, 0xff, // MinimumPromoUp.
                0xff, 0xff, 0xff, 0xff, // MaximumPromoUp.
                0x01, //m_externalCreditProviderEnabled
                0x87, 0x65, 0x43, 0x21, //m_creditFee
                0x00, //m_creditFeeFixed
                0x03,//M_externalCreditToGameType
            };

			const TestDataType TEST_DATA[] =
			{
				{ response0, 0,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 }, (PBError)1, 2222, 3, 4444, 55, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
					{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 0, 0, 0, 0 },
					0, 0,
					0, 0, 0,3, sizeof(response0)/sizeof(response0[0])
				},
				{ response1, 0,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 }, (PBError)1, 222, 3, 444, 5, 6, 7, 8, 5, 10, 2, 12, 13, 14, 4, 16,
					{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 0, 0, 0, 0 },
					0, 0,
					0, 0, 0,3,sizeof(response1)/sizeof(response1[0])
				},
				{ response2, PACKET_VERSION_NO_POINTS_FOR_PROMO,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 }, (PBError)1, 2222, 3, 4444, 55, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
					{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 0, 0, 0, 0 } ,
					18, 19,
					0, 0, 0,3,sizeof(response2)/sizeof(response2[0])
				},
				{ response3, PACKET_VERSION_NO_POINTS_FOR_PROMO,
					{ 0x12, 0x34, 0x56, 0x78, 0x90 }, (PBError)1, 222, 3, 444, 5, 6, 7, 8, 5, 10, 2, 12, 13, 14, 4, MAX_PBID_LENGTH,
					{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' },
					18, 19,
					0, 0, 0,3,sizeof(response3)/sizeof(response3[0])
				},
				{ response4, 0,
					{ 0x00, 0x00, 0x00, 0x00, 0x00 }, (PBError)0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					0, 0,
					0, 0, 0,3,sizeof(response4)/sizeof(response4[0])
				},
				{ response5, PACKET_VERSION_NO_POINTS_FOR_PROMO,
					{ 0x00, 0x00, 0x00, 0x00, 0x00 }, (PBError)0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					0, 0,
					0, 0, 0,3,sizeof(response5)/sizeof(response5[0])
				},
				{ response6, PACKET_VERSION_NO_POINTS_FOR_PROMO,
					{ 0x00, 0x00, 0x00, 0x00, 0x00 }, (PBError)0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
					0xffffffff, 0xffffffff,
					0, 0, 0,3,sizeof(response6)/sizeof(response6[0])
				},
				{ response7, 0,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0, 0,
					0, 0, 0,3,sizeof(response7)/sizeof(response7[0])
				},
				{ response8, PACKET_VERSION_NO_POINTS_FOR_PROMO,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0, 0, 0,3,sizeof(response8)/sizeof(response8[0])
				},

				{ response9, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x00, 0x78563412, 0x01,3,sizeof(response9)/sizeof(response9[0])
				},

				{ response10, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x01, 0x21436587, 0x00,3,sizeof(response10)/sizeof(response10[0])
				},

                { response11, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x01, 0x21436587, 0x00,0,sizeof(response11)/sizeof(response11[0])
				},

                {
                    response12, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x01, 0x21436587, 0x00,1,sizeof(response12)/sizeof(response12[0])
				},

                {
                   response13, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x01, 0x21436587, 0x00,2,sizeof(response13)/sizeof(response13[0])
				},

               {
                   response14, PACKET_VERSION_CREDIT_TRANSFER,
					{ 0xff, 0xff, 0xff, 0xff, 0xff }, (PBError)0xff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0xffffffff,
					0, 0xffffffff, 0xffffffff / 100, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff / 100, MAX_PBID_LENGTH,
					{ (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
						(char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff },
					0xffffffff, 0xffffffff,
					0x01, 0x21436587, 0x00, 3,sizeof(response14)/sizeof(response14[0])
				},
			};

			for (int testIndex = 0; testIndex < _countof(TEST_DATA); ++testIndex)
			{
				// Setup.

				// Perform operation(s) to be tested.
				CPollerPersonalBankerResponseMessage msg(TEST_DATA[testIndex].m_response,
					TEST_DATA[testIndex].m_bPromoValues, TEST_DATA[testIndex].m_bufferLen);

				// Test #1.
				const CPersonalBankerInfo * const pbInfo = msg.GetPBInfo();

				{
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbInfo=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)pbInfo,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE((void *)nullptr, (void *)pbInfo) << failedMsg;
				}

				// Test #2.
				const byte * const cardNumber = pbInfo->GetCardNumber();

				{

					string failedMsg;
					failedMsg = FormatString(_T("FAILED cardNumber=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)cardNumber,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE((void *)nullptr, (void *)cardNumber) << failedMsg;
				}

				// Test #3.

				{
					bool areEqual = 0 == memcmp(cardNumber,
						TEST_DATA[testIndex].m_cardNumberExpected,
						sizeof(TEST_DATA[testIndex].m_cardNumberExpected));
					string failedMsg;
					failedMsg = FormatString(_T("FAILED typeResult=%2.2x,%2.2x,%2.2x,%2.2x,%2.2x expected=%2.2x,%2.2x,%2.2x,%2.2x,%2.2x in %s"),
						(int)cardNumber[0],
						(int)cardNumber[1],
						(int)cardNumber[2],
						(int)cardNumber[3],
						(int)cardNumber[4],
						(int)TEST_DATA[testIndex].m_cardNumberExpected[0],
						(int)TEST_DATA[testIndex].m_cardNumberExpected[1],
						(int)TEST_DATA[testIndex].m_cardNumberExpected[2],
						(int)TEST_DATA[testIndex].m_cardNumberExpected[3],
						(int)TEST_DATA[testIndex].m_cardNumberExpected[4],
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #4.
				{
					const PBError pbError = pbInfo->GetErrorCode();

					bool areEqual = pbError == TEST_DATA[testIndex].m_pbErrorExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbError=%d expected=%d testIndex=%d in %s"),
						(int)pbError,
						(int)TEST_DATA[testIndex].m_pbErrorExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #5.
				{
					const DWORD cashBalance = pbInfo->GetCashBalance();
	
					bool areEqual = cashBalance == TEST_DATA[testIndex].m_cashBalanceExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED cashBalance=%u expected=%u testIndex=%d in %s"),
						(unsigned)cashBalance,
						(unsigned)TEST_DATA[testIndex].m_cashBalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #6.
				{
					const DWORD pbFlags = pbInfo->GetPBFlags();

					bool areEqual = pbFlags == TEST_DATA[testIndex].m_pbFlagsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbFlags=%u expected=%u testIndex=%d in %s"),
						(unsigned)pbFlags,
						(unsigned)TEST_DATA[testIndex].m_pbFlagsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #7.
				{
					const DWORD promo2Balance = pbInfo->GetPromo2Balance();

					bool areEqual = promo2Balance == TEST_DATA[testIndex].m_promo2BalanceExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED promo2Balance=%u expected=%u testIndex=%d in %s"),
						(unsigned)promo2Balance,
						(unsigned)TEST_DATA[testIndex].m_promo2BalanceExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #8.
				{
					const DWORD pointsAvailable = pbInfo->GetPointsAvailable();

					bool areEqual = pointsAvailable == TEST_DATA[testIndex].m_pointsAvailableExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsAvailable=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsAvailable,
						(unsigned)TEST_DATA[testIndex].m_pointsAvailableExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #9.
				{
					const DWORD pointsPerDollar = pbInfo->GetPointsPerDollar();

					bool areEqual = pointsPerDollar == TEST_DATA[testIndex].m_pointsPerDollarExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pointsPerDollar=%u expected=%u testIndex=%d in %s"),
						(unsigned)pointsPerDollar,
						(unsigned)TEST_DATA[testIndex].m_pointsPerDollarExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #10.
				{
					const DWORD sequenceNumber = pbInfo->GetSequenceNumber();

					bool areEqual = sequenceNumber == TEST_DATA[testIndex].m_sequenceNumberExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED sequenceNumber=%u expected=%u testIndex=%d in %s"),
						(unsigned)sequenceNumber,
						(unsigned)TEST_DATA[testIndex].m_sequenceNumberExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #11.
				{
					const DWORD minimumPoints = pbInfo->GetMinimumPoints();

					bool areEqual = minimumPoints == TEST_DATA[testIndex].m_minimumPointsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPoints,
						(unsigned)TEST_DATA[testIndex].m_minimumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #12.
				{
					const DWORD maximumPoints = pbInfo->GetMaximumPoints();

					bool areEqual = maximumPoints == TEST_DATA[testIndex].m_maximumPointsExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPoints=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPoints,
						(unsigned)TEST_DATA[testIndex].m_maximumPointsExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #13.
				{
					const DWORD minimumCashDown = pbInfo->GetMinimumCashDown();

					bool areEqual = minimumCashDown == TEST_DATA[testIndex].m_minimumCashDownExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashDown,
						(unsigned)TEST_DATA[testIndex].m_minimumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #14.
				{
					const DWORD maximumCashDown = pbInfo->GetMaximumCashDown();

					bool areEqual = maximumCashDown == TEST_DATA[testIndex].m_maximumCashDownExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashDown,
						(unsigned)TEST_DATA[testIndex].m_maximumCashDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #15.
				{
					const DWORD minimumCashUp = pbInfo->GetMinimumCashUp();

					bool areEqual = minimumCashUp == TEST_DATA[testIndex].m_minimumCashUpExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumCashUp,
						(unsigned)TEST_DATA[testIndex].m_minimumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #16.
				{
					const DWORD maximumCashUp = pbInfo->GetMaximumCashUp();

					bool areEqual = maximumCashUp == TEST_DATA[testIndex].m_maximumCashUpExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumCashUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumCashUp,
						(unsigned)TEST_DATA[testIndex].m_maximumCashUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #17.
				{
					const DWORD minimumPromoDown = pbInfo->GetMinimumPromoDown();

					bool areEqual = minimumPromoDown == TEST_DATA[testIndex].m_minimumPromoDownExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #18.
				{
					const DWORD maximumPromoDown = pbInfo->GetMaximumPromoDown();

					bool areEqual = maximumPromoDown == TEST_DATA[testIndex].m_maximumPromoDownExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoDown=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoDown,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoDownExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #19.
				{
					const int pbIDLength = pbInfo->GetPBIDLength();

					bool areEqual = pbIDLength == TEST_DATA[testIndex].m_pbIDLengthExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbIDLength=%d expected=%d testIndex=%d in %s"),
						(int)pbIDLength,
						(int)TEST_DATA[testIndex].m_pbIDLengthExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #20.
				const char * const pbID = pbInfo->GetPersonalBankerID();

				{

					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID=%p expected=NOT NULL testIndex=%d in %s"),
						(const void *)pbID,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_NE((void *)nullptr, (void *)pbID) << failedMsg;
				}

				// Test #21.

				{
					bool areEqual = 0 == memcmp(pbID,
						TEST_DATA[testIndex].m_pbIDExpected,
						TEST_DATA[testIndex].m_pbIDLengthExpected);

					string pbIDString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(pbID), TEST_DATA[testIndex].m_pbIDLengthExpected);
					string pbIDExpectedString = CUtilities::ToHexString(reinterpret_cast<const BYTE *>(TEST_DATA[testIndex].m_pbIDExpected), TEST_DATA[testIndex].m_pbIDLengthExpected);
					string failedMsg;
					failedMsg = FormatString(_T("FAILED pbID={%s} expected={%s} in %s"),
						static_cast<LPCTSTR>(pbIDString.c_str()),
						static_cast<LPCTSTR>(pbIDExpectedString.c_str()),
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #22.
				{
					const DWORD minimumPromoUp = pbInfo->GetMinimumPromoUp();

					bool areEqual = minimumPromoUp == TEST_DATA[testIndex].m_minimumPromoUpExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED minimumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)minimumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_minimumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #23.
				{
					const DWORD maximumPromoUp = pbInfo->GetMaximumPromoUp();

					bool areEqual = maximumPromoUp == TEST_DATA[testIndex].m_maximumPromoUpExpected;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED maximumPromoUp=%u expected=%u testIndex=%d in %s"),
						(unsigned)maximumPromoUp,
						(unsigned)TEST_DATA[testIndex].m_maximumPromoUpExpected,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

				// Test #24.
				{//Credit Tests
					const bool externalCreditProviderEnabled = pbInfo->ExternalCreditProviderEnabled();

					bool areEqual = externalCreditProviderEnabled == TEST_DATA[testIndex].m_externalCreditProviderEnabled;
					string failedMsg;
					failedMsg = FormatString(_T("FAILED externalCreditProviderEnabled=%u expected=%u testIndex=%d in %s"),
						(unsigned)externalCreditProviderEnabled,
						(unsigned)TEST_DATA[testIndex].m_externalCreditProviderEnabled,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;


					DWORD creditFee = pbInfo->CreditFee();
					areEqual = creditFee == TEST_DATA[testIndex].m_creditFee;
					failedMsg = FormatString(_T("FAILED creditFee=%u expected=%u testIndex=%d in %s"),
						(unsigned)creditFee,
						(unsigned)TEST_DATA[testIndex].m_creditFee,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;

					const bool creditFeeFixed = pbInfo->CreditFeeFixed();
					areEqual = creditFeeFixed == TEST_DATA[testIndex].m_creditFeeFixed;
					failedMsg = FormatString(_T("FAILED creditFeeFixed=%u expected=%u testIndex=%d in %s"),
						(unsigned)creditFeeFixed,
						(unsigned)TEST_DATA[testIndex].m_creditFeeFixed,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;
				}

                //Test case #25 for External Credit to game test
                {
                    ExtCreditTransferType ExternalCreditTransferType = pbInfo->ExternalCreditTransferType();
					bool areEqual = ExternalCreditTransferType == TEST_DATA[testIndex].m_extCreditTransferType;
                    string failedMsg;
					failedMsg = FormatString(_T("FAILED ExternalCreditTransferType=%u expected=%u testIndex=%d in %s"),
						(unsigned)ExternalCreditTransferType,
						(unsigned)TEST_DATA[testIndex].m_extCreditTransferType,
						(int)testIndex,
						static_cast<LPCTSTR>(__TFUNCTION__));
					ASSERT_TRUE(areEqual) << failedMsg;

                }
				// Cleanup.
			}
		}
}
