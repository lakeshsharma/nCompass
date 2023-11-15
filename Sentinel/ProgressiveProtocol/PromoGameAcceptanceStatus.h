#pragma once
#include "stdafx.h"
#include "ProgressiveKeys.h"
#include "InternalMsgContent.h"

class CPromoGameAcceptanceStatus : public CInternalMsgContent<INT_MSG_CONTENT_PROMOGAME_ACCEPTANCE_STATUS>
{
public:
	/// <summary>
	/// CPromoGameAcceptanceStatus json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The jackpot id.</param>
	CPromoGameAcceptanceStatus(int64_t jackpotId);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CPromoGameAcceptanceStatus"/> class.
	/// </summary>
	virtual ~CPromoGameAcceptanceStatus();

	/// <summary>
	/// Gets the promogame status.
	/// </summary>
	/// <returns>Returns promogame status.</returns>
	std::string GetPromoGameStatus() const;

	/// <summary>
	/// Gets the jackpot Id.
	/// </summary>
	/// <returns>Returns jackpot Id.</returns>
	int64_t GetJackpotId() const;

protected:
	PromoGameStatus::Type m_promoGameStatus;
	int64_t m_jackpotId;
};