#pragma once

class ExcessiveVoucherOutTests;

class CExcessiveVoucherOut
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CExcessiveVoucherOut"/> class.
	/// </summary>
	/// <param name="type">field type</param>
	CExcessiveVoucherOut();

	/// <summary>
	/// Finalizes an instance of the <see cref="CExcessiveVoucherOut"/> class.
	/// </summary>
	~CExcessiveVoucherOut();

	/// <summary>
	/// Gets the formatted HTML string for the ExcessiveVoucherOut information.
	/// </summary>
	/// <returns>The formatted HTML string.</returns>
	std::string GetHtmlString() const;

	/// <summary>
	/// Sets the Global Excessive Voucher Out Enabled Flag
	/// </summary>
	/// <param name="enabled">enabled flag</param>
	void SetGlobalExcessiveVoucherOutEnabledFlag(bool enabled);

	/// <summary>
	/// Sets the Excessive Voucher Out Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	void SetExcessiveVoucherOutThreshold(DWORD threshold);

	/// <summary>
	/// Sets the Total Bill In Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	void SetTotalBillInThreshold(DWORD threshold);

	/// <summary>
	/// Sets the Coin In Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	void SetCoinInThreshold(DWORD threshold);

	/// <summary>
	/// Sets the Coin Out Threshold
	/// </summary>
	/// <param name="threshold">threshold value</param>
	void SetCoinOutThreshold(DWORD threshold);

	/// <summary>
	/// Adds the billInPennies to the current bill in total
	/// </summary>
	/// <param name="billInPennies">bill in pennies</param>
	void ProcessBillIn(DWORD billInPennies);

	/// <summary>
	/// Adds the coinInPennies to the current coin in total
	/// </summary>
	/// <param name="coinInPennies">coin in pennies</param>
	void ProcessCoinIn(DWORD coinInPennies);

	/// <summary>
	/// Adds the coinOutPennies to the current cout in total
	/// </summary>
	/// <param name="coinOutPennies">coin out pennies</param>
	void ProcessCoinOut(DWORD coinOutPennies);

	/// <summary>
	/// Checks to see if excessive voucher out transaction needs to be sent to FloorLogix
	/// </summary>
	/// <param name="amountInPennies">IN - amount in pennies voucher out</param>
	/// <param name="currentBillInPennies">OUT - current total bill in pennies</param>
	/// <returns>true if transaction needs to be sent to FloorLogix, false otherwise.</returns>
	bool ProcessVoucherOut(DWORD amountInPennies, DWORD& currentBillInPennies);

	/// <summary>
	/// Game credits became zero 
	/// </summary>
	void GameCreditsBecameZero();

private: 
	void ResetTrackingValues();

	bool m_globalExcessiveVoucherOutEnabledFlag;
	DWORD m_excessiveVoucherOutThreshold;
	DWORD m_totalBillInThreshold;
	DWORD m_coinInThreshold;
	DWORD m_coinOutThreshold;

	DWORD m_currentBillInPennies;
	DWORD m_currentCoinInPennies;	
	DWORD m_currentCoinOutPennies;

	//friend class declarations for unit test access.
	//friend class ExcessiveVoucherOutTests;
	FRIEND_TEST(ExcessiveVoucherOutTests, ExcessiveVoucherOutConstructorTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, GameCreditsBecameZeroTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, ProcessBillInTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, ProcessCoinInTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, ProcessCoinOutTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, ProcessVoucherOutTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, HtmlStringTest);
	FRIEND_TEST(ExcessiveVoucherOutTests, ThresholdValuesAndGlobalExcessiveVoucherOutEnabledFlagTest);	
};