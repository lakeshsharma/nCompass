#include "stdafx.h"
#include "PokerHand.h"

using namespace std;

PokerHand::PokerHand(const byte* pCards)
	: m_isValid(false)
{
	Set(pCards);
}

bool PokerHand::IsValid() const
{
	return m_isValid;
}

byte PokerHand::operator[](int index) const
{
	return IsValid() && index >= 0 && index < CardsInHand ? m_cards[index] : InvalidCard;
}

void PokerHand::Set(const byte* pCards)
{
	if (pCards != nullptr)
	{
		memcpy(m_cards, pCards, CardsInHand);
		m_isValid = true;
	}
	else
	{
		memset(m_cards, InvalidCard, CardsInHand);
		m_isValid = false;
	}
}

string PokerHand::ToOneLinkString() const
{
	static char suits[] = { 'S', 'C', 'H', 'D', 'W' };
	static char vals[]  = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A', 'W' };

	string rstr;
	rstr.reserve(_countof(m_cards) * 3);
	for (size_t card = 0; card < _countof(m_cards); card++)
	{
		if (card > 0)
			rstr += ' ';

        int val = (m_cards[card] & 0x0f);
		rstr += val < (int)_countof(vals) ? vals[val] : '?';

        val = (m_cards[card] >> 4) & 0x0f;
		rstr += val < (int)_countof(suits) ? suits[val] : '?';

	}
	return rstr;
}
