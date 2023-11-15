#pragma once

#include "InternalMsgContent.h"

class PokerHand : public CInternalMsgContent<INT_MSG_CONTENT_POKER_HAND>
{
public:
	enum { CardsInHand = 5 };
	static const byte InvalidCard = 0xff;

private:
	bool m_isValid;
	byte m_cards[CardsInHand];

public:
	///	<summary>
	/// Constructor.
	///	</summary>
	///	<param name="pCards">An array of CardsInHand (5) cards.</param>
	PokerHand(const byte* pCards = nullptr);

	bool IsValid() const;

	byte operator[](int index) const;

	void Set(const byte* pCards);
	std::string ToOneLinkString() const;
};
