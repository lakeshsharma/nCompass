#pragma once

#include "../Card.h"
#include "../Utilities.h"
#include "ProcessDump/OperatingSystem.h"

// Declare friend class(es) for unit test access.
namespace SentinelUnitTests { class McrAutoTest; }

class CMCRAutoTest
{
	friend class SentinelUnitTests::McrAutoTest; // Grant "friend" access for testing.
public:
	CMCRAutoTest(const CConfig &config, DWORD tickCount,
		IOperatingSystem &operatingSystem = COperatingSystem::m_operatingSystem);

	~CMCRAutoTest(void);

	/// <summary>
	/// Gets the card event.
	/// </summary>
	/// <param name="bInserted">IN/OUT - Set true if a card was inserted.</param>
	/// <param name="bRemoved">IN/OUT - Set true if a card was removed.</param>
	/// <param name="bTimeout">OUT - Set false if a card was inserted or removed, set true otherwise.</param>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <returns>A card instance if bInserted was set true, NULL otherwise.</returns>
	CCard *GetCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, DWORD tickCount);

	/// <summary>
	/// Refreshes the configuration.
	/// </summary>
	/// <param name="config">The configuration.</param>
	void RefreshConfig(const CConfig &config);

private:
	static const LPCTSTR CARD_LIST_FILE_NAME;

	std::recursive_mutex m_cs; // Use for access/updates to m_cardConfig pointers.
	CCardConfig m_cardConfig;
	bool m_cardReaderReady;

	void Open( void );
	void Close( void );
	CCard *Read(bool &bTimeout, DWORD tickCount);
	bool WaitForCardRemoval(bool &bTimeout, DWORD tickCount);
	int GetRandomRange(int rangeMin, int rangeMax);

	DWORD m_startTickCount;
	DWORD m_fireCardEventPeriod; // In tick counts.

	std::vector<std::string> m_cardArray;
	bool m_cardIn;

	IOperatingSystem &m_operatingSystem;
};

