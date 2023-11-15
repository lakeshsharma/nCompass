#pragma once
#if 000//TODO

#ifdef TEST_SAVED_TRANSACTIONS

/// <summary>
/// Class for saving test transactions for internal testing at Aristocrat.  Not for release code.
/// Also see Tools\NvramParser\NvramParser.vcxproj for human-readable transaction parsing.
/// </summary>
class CSaveTestTransactions
{
public:
	/// <summary>
	/// Save test transactions.
	/// </summary>
	static void SaveTestTransactions();

private:
	static BYTE m_playerCardInMessage[];
	static BYTE m_playerCardOutMessage[];
	static BYTE m_badCardReaderMessage[];
	static BYTE m_badTicketMessage[];

	static void UpdateSrand();
	static UINT GetMaximumNvramWriteSize(UINT offset);
	static void SaveTransaction(const BYTE *msg, WORD msgLength, DWORD transactionIndex, bool isTicketOut = false);
};
#endif // TEST_SAVED_TRANSACTIONS.

#endif//TODO 000
