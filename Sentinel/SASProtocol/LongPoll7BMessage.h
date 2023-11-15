#pragma once
#include "SASPollMessage.h"

#define X7B_RESPONSE_SIZE 15
#define X7B_DEFSIZE 8
#define X7B_COMMANDSIZE 13


class CLongPoll7BMessage :
	public CSASPollMessage
{
public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CLongPoll7BMessage"/> class.
	/// </summary>
	/// <param name="gameAddress">The game address.</param>
	CLongPoll7BMessage(byte gameAddress);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CLongPoll7BMessage"/> class.
	/// </summary>
	~CLongPoll7BMessage(void);

	/// <summary>
	/// Returns the poll value.
	/// </summary>
	/// <returns>The poll value.</returns>
	virtual BYTE GetPollValue();

	/// <summary>
	/// Sets the response received event.
	/// </summary>
	/// <remarks>
	/// Received cashable expiration is copied as is, without any BCD conversion.
	/// Received restricted expiration is copied as is, without any BCD conversion.
	/// </remarks>
	void ResponseReceived(void);

	/// <summary>
	/// Sets the send buffer according to the previously specified instance values.
	/// </summary>
	void Fill();

	// set/get ticketing information

	/// <summary>
	/// Sets the printer cashout.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetPrinterCashout(bool value);

	/// <summary>
	/// Sets the printer handpay receipt.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetPrinterHandpayReceipt(bool value);

	/// <summary>
	/// Sets the validate handpays.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetValidateHandpays(bool value);

	/// <summary>
	/// Sets the print restricted tickets.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetPrintRestrictedTickets(bool value);

	/// <summary>
	/// Sets the foreign restricted amounts.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetForeignRestrictedAmounts(bool value);

	/// <summary>
	/// Sets the ticket redemption.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// </remarks>
	void SetTicketRedemption(bool value);

	/// <summary>
	/// Sets the secure enhanced config.
	/// </summary>
	/// <param name="value">The value.  See remarks.</param>
	/// <remarks>
	/// Once the value is set true, attempts to set false are ignored.
	/// 2013-11-04 Configures a reserved bit instead of the bit indicated in SAS documentation.
	/// </remarks>
	void SetSecureEnhancedConfig(bool value);

	/// <summary>
	/// Sets the cashable expiration.
	/// </summary>
	/// <param name="value">The value.  Valid range is 0..9999.</param>
	/// <remarks>
	/// If the value is not in the valid range, the value 0 is sent to the EGM.
	/// </remarks>
	void SetCashableExpiration(WORD value);

	/// <summary>
	/// Sets the restricted expiration.
	/// </summary>
	/// <param name="value">The value.  Valid range is 0..9999.</param>
	/// <remarks>
	/// If the value is not in the valid range, the value 0 is sent to the EGM.
	/// </remarks>
	void SetRestrictedExpiration(WORD value);

	/// <summary>
	/// Gets the printer cashout.
	/// </summary>
	/// <returns>The printer cashout.</returns>
	bool GetPrinterCashout() const;

	/// <summary>
	/// Gets the printer handpay receipt.
	/// </summary>
	/// <returns>The printer handpay receipt.</returns>
	bool GetPrinterHandpayReceipt() const;

	/// <summary>
	/// Gets the validate handpays.
	/// </summary>
	/// <returns>The validate handpays.</returns>
	bool GetValidateHandpays() const;

	/// <summary>
	/// Gets the print restricted tickets.
	/// </summary>
	/// <returns>The print restricted tickets.</returns>
	bool GetPrintRestrictedTickets() const;

	/// <summary>
	/// Gets the foreign restricted amounts.
	/// </summary>
	/// <returns>The foreign restricted amounts.</returns>
	bool GetForeignRestrictedAmounts() const;

	/// <summary>
	/// Gets the ticket redemption.
	/// </summary>
	/// <returns>The ticket redemption.</returns>
	bool GetTicketRedemption() const;

	/// <summary>
	/// Gets the secure enhanced config.
	/// </summary>
	/// <returns>The secure enhanced config.</returns>
	/// <remarks>
	/// 2013-11-04 Reports a reserved bit instead of the bit indicated in SAS documentation.
	/// </remarks>
	bool GetSecureEnhancedConfig() const;

	/// <summary>
	/// Gets the cashable expiration.
	/// </summary>
	/// <returns>The cashable expiration.</returns>
	/// <remarks>
	/// The returned value is either the value provided to the SetCashableExpiration() method
	/// or the value provided by the EGM copied as is, without any BCD conversion.
	/// I.e., if the value is provided by the EGM then the returned value is probably formatted as BCD.
	/// </remarks>
	WORD GetCashableExpiration() const;

	/// <summary>
	/// Gets the restricted expiration.
	/// </summary>
	/// <returns>The restricted expiration.</returns>
	/// <remarks>
	/// The returned value is either the value provided to the SetRestrictedExpiration() method
	/// or the value provided by the EGM copied as is, without any BCD conversion.
	/// I.e., if the value is provided by the EGM then the returned value is probably formatted as BCD.
	/// </remarks>
	WORD GetRestrictedExpiration() const;

	// values returned in response

	/// <summary>
	/// Gets the asset number.
	/// </summary>
	/// <returns>The asset number.</returns>
	DWORD GetAssetNumber() const;
	
private:
	static const BYTE POLL_VALUE = 0x7B;

	// Message offsets and sizes indicated by SAS documentation.
	static const size_t CONTROL_MASK_OFFSET = OPTIONAL_LENGTH_OFFSET + OPTIONAL_LENGTH_SIZE;
	static const size_t CONTROL_MASK_SIZE = 2;
	static const size_t STATUS_OFFSET = CONTROL_MASK_OFFSET + CONTROL_MASK_SIZE;
	static const size_t STATUS_SIZE = 2;
	static const size_t CASHABLE_EXPIRATION_OFFSET = STATUS_OFFSET + STATUS_SIZE;
	static const size_t CASHABLE_EXPIRATION_SIZE = 2;
	static const size_t RESTRICTED_EXPIRATION_OFFSET = CASHABLE_EXPIRATION_OFFSET + CASHABLE_EXPIRATION_SIZE;
	static const size_t RESTRICTED_EXPIRATION_SIZE = 2;

	byte m_controlMask[CONTROL_MASK_SIZE];
	byte m_status[STATUS_SIZE];
	WORD m_cashableExpiration;
	WORD m_restrictedExpiration;

	// values returned in response
	DWORD m_assetNumber;
};
