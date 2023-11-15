#pragma once
#include "PollerMessage.h"
#include "PersonalBankerTransfer.h"

class CPollerPBPinResetResponseMessage :
	public CPollerMessage
{
public:
	/// <summary>
	/// PollerPBPinResetResponseMessage data constructor.
	/// </summary>
	/// <param name="response">IN - pointer to response data.  Response data MUST be valid.</param>
	CPollerPBPinResetResponseMessage(const byte *response);

	/// <summary>
	/// Returns the Pin Reset Result for the PersonalBanker transfer.
	/// </summary>
	/// <returns>The Pin Reset Result for the PersonalBanker transfer.</returns>
	PinResetResult GetPinResetResult(void);

private:
	PinResetResult m_pinResetResult;

};



