#include "stdafx.h"
#include "MCRAutoTest.h"

#include "DirectoryManager.h"

using namespace std;

const LPCTSTR CMCRAutoTest::CARD_LIST_FILE_NAME(_T("/CardList.txt"));
const int CARD_BUFFER_LEN = 128;
const DWORD CARD_READ_TIMEOUT = 500;
const DWORD CARD_WRITE_TIMEOUT = 500;
const DWORD GPIO_BASE_ADDRESS = 0x40e00000;
const int CARD_OUT_MAX = 120 * 1000;
const int CARD_NUM_LENGTH = 10;

const int AUTO_CARDOUT_TIME = 35 * 1000; // 35000 = 35 seconds to stay carded out.
const int AUTO_CARDEDIN_TIME = 20 * 1000; // 20000 = 20 seconds to stay carded out.
const int INITIAL_FIRE_CARD_TIME = 30 * 1000; // 30000 = 30 seconds before first event.

CMCRAutoTest::CMCRAutoTest(const CConfig &config, DWORD tickCount, IOperatingSystem &operatingSystem) :
	m_operatingSystem(operatingSystem)
{
	m_cardConfig.Setup(config);

	DWORD seed((DWORD)(tickCount + m_operatingSystem.GetCurrentTime()));
	m_operatingSystem.SetRandSeed(seed);

	m_startTickCount = tickCount;
	m_fireCardEventPeriod = INITIAL_FIRE_CARD_TIME;

	m_cardReaderReady = false;
	m_cardIn = false;

	Open();
}

CMCRAutoTest::~CMCRAutoTest(void)
{
	Close();
}

void CMCRAutoTest::Open( void )
{

	std::string cf(CDirectoryManager::Instance().GetWorkingPath(DIRECTORY_SENTINELDATA));
	cf.append(CARD_LIST_FILE_NAME);

	try
	{
        ifstream file(cf);
		if (file.is_open())
		{

			BYTE buffer[CARD_NUM_LENGTH];
            memset(buffer, 0, CARD_NUM_LENGTH);

			while (!file.eof())
			{
                file.read((char*)buffer, CARD_NUM_LENGTH);

                if (file.eof())
                    break;

                std::string cardNumber;
				for (int i = 0; i < CARD_NUM_LENGTH; i++)
				{
					cardNumber += ((char)buffer[i]);
				}

				m_cardArray.push_back(cardNumber);
                memset(buffer, 0, CARD_NUM_LENGTH);
			}

			m_cardReaderReady = true;
		}
	}
	catch(...)
	{
	}
}

void CMCRAutoTest::Close( void )
{
	
}

CCard *CMCRAutoTest::GetCardEvent(bool &bInserted, bool &bRemoved, bool &bTimeout, DWORD tickCount)
{
	CCard *card(NULL);

	if (m_cardArray.size() > 0)
	{
		if (!m_cardIn)
		{
			card = Read(bTimeout, tickCount);
			bInserted = !bTimeout;
		}
		else
		{
			bRemoved = WaitForCardRemoval(bTimeout, tickCount);
		}
	}

	return card;
}

CCard * CMCRAutoTest::Read( bool &bTimeout, DWORD tickCount )
{
	CCard *card = NULL;

	bTimeout = false;

	if (m_cardArray.size() > 0 && tickCount - m_startTickCount >= m_fireCardEventPeriod)
	{
		card = new CCard(CUtilities::GetCurrentTime(), CUtilities::GetUTCTime());

		int index = GetRandomRange(0, m_cardArray.size());

		std::string cardNumber = m_cardArray.at(index);

		m_cs.lock();
		card->SetCardID(cardNumber, &m_cardConfig);
		m_cs.unlock();

		//now set the timeout for the removal of the card
		m_startTickCount = tickCount;
		m_fireCardEventPeriod = AUTO_CARDOUT_TIME;

		m_cardIn = true;
	}	
	// Otherwise, assume it was a timeout.
	else
	{
		bTimeout = true;
	}

	return card;
}

bool CMCRAutoTest::WaitForCardRemoval( bool &bTimeout, DWORD tickCount )
{
	bool bCardRemoved = false;

	bTimeout = false;	

	//if we think that we don't have a card in or the timer expired send true this will keep us in synch just incase
	if ( !m_cardIn || tickCount - m_startTickCount >= m_fireCardEventPeriod)
	{
		bCardRemoved = true;
		
		//now set the timeout for the removal of the card
		m_startTickCount = tickCount;
		m_fireCardEventPeriod = AUTO_CARDEDIN_TIME;
		m_cardIn = false;
	}
	// Otherwise, assume it was a timeout.
	else
	{
		bTimeout = true;
	}
	
	return bCardRemoved;
}

int CMCRAutoTest::GetRandomRange(int rangeMin, int rangeMax)
{
	return m_operatingSystem.GetRandRange(rangeMin, rangeMax);
}

void CMCRAutoTest::RefreshConfig(const CConfig &config)
{
	lock_guard<recursive_mutex> lock(m_cs);
	m_cardConfig.Setup(config);
}
