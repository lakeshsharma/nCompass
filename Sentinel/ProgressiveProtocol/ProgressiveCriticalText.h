#pragma once

#include "InternalMessage.h"
#include "ProgressiveProtocol/ProgressiveConnectionInfo.h"

union u_ProgressiveCriticalText
{
	// first representation (member of union) 
	struct tProgressiveCriticalText
	{
		WORD progressiveEnabled : 1;//Bit0
		WORD progressiveDown : 1;//Bit1
		WORD eligibilityDetermined : 1;//Bit2
		WORD cardedPlayer : 1;//Bit3
		WORD sessionIdMatchStatus : 1;//Bit4, also set true for a session identifier mismatch prior to timeout.
		WORD sasStartupQueueEmpty : 1;//Bit5
	} m_data;

	// second representation (member of union) 
	WORD m_wordValue;

	u_ProgressiveCriticalText(WORD wordValue)
	{
		m_wordValue = wordValue;
	}

	bool IsDown()
	{
		// If the progressive is enabled, either the progressive OR the SAS startup queue empty status is false or a session identifier effectively mismatched will qualify as down.
		return (m_data.progressiveEnabled && (m_data.progressiveDown || !m_data.sasStartupQueueEmpty || !m_data.sessionIdMatchStatus));
	}

	bool IsProcessingDown()
	{
		return !m_data.progressiveEnabled || m_data.progressiveDown || !m_data.sasStartupQueueEmpty;
	}

	WORD GetAlert()
	{
		WORD alert = 0;

		if (m_data.progressiveEnabled)
		{
			if (IsDown())
			{
				alert = Alerts::ALERT_PROGRESSIVE_DOWN;
			}
			else if (m_data.cardedPlayer && !m_data.eligibilityDetermined)
			{
				alert = Alerts::ALERT_PROGRESSIVE_INELIGIBILITY;
			}
		}

		return alert;
	}
};

/// <summary>
/// This class handles the processing of progressive critical text that is to be displayed.
/// </summary>
class CProgressiveCriticalText
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CProgressiveCriticalText"/> class.
	/// </summary>
	CProgressiveCriticalText();

	/// <summary>
	/// Finalizes an instance of the <see cref="CProgressiveCriticalText"/> class.
	/// </summary>
	virtual ~CProgressiveCriticalText();

	/// <summary>
	/// Determines whether the down should be displayed.
	/// </summary>
	/// <returns>true if the down message is to be displayed</returns>
	bool IsDown();

	/// <summary>
	/// Determines whether the ability to process progressive bonus awards is down.
	/// </summary>
	/// <returns>true if the ability to process progressive bonus awards is down, false otherwise.</returns>
	bool IsProcessingDown();

	/// <summary>
	/// Processes the progressive configuration.
	/// </summary>
	/// <param name="progressiveConfig">The progressive configuration.</param>
	/// <returns>true if the down status has changed and messages are to be generated.</returns>
	bool ProcessProgressiveConfig(CProgressiveConfig *progressiveConfig);

	/// <summary>
	/// Processes the specified progressive connection information.
	/// </summary>
	/// <param name="connectionInfo">The connection information.</param>
	/// <returns>true if the down status has changed and messages are to be generated.</returns>
	bool ProcessConnectionInfo(CProgressiveConnectionInfo *connectionInfo);

	/// <summary>
	/// Processes changes to the SAS startup queue empty status.
	/// </summary>
	/// <param name="sasStartupQueueEmpty">IN - The SAS startup queue empty status.</param>
	/// <returns>true if the down status has changed and messages are to be generated.</returns>
	/// <remarks>
	/// Use false for sasStartupQueueEmpty when communications to the EGM are down.
	/// </remarks>
	bool ProcessSasStartupQueueEmpty(bool sasStartupQueueEmpty);

	/// <summary>
	/// Processes updates to the eligibility determined status.
	/// </summary>
	/// <param name="eligibilityDetermined">IN - true if eligibility is determined, false otherwise.</param>
	/// <returns>true if the down status has changed and messages are to be generated.</returns>
	bool ProcessEligibilityDetermined(bool eligibilityDetermined);

	/// <summary>
	/// Processes updates to the session identifier match status.
	/// </summary>
	/// <param name="sessionIdMatchStatus">IN - true if the eligibility session identifier effectively matches, false otherwise.</param>
	/// <returns>true if the down status has changed and messages are to be generated.</returns>
	/// <remarks>
	/// Also use true for sessionIdMatch when session identifier has been mismatched for less than the timeout period.
	/// </remarks>
	bool ProcessSessionIdMatchStatus(bool sessionIdMatchStatus);

	/// <summary>
	/// Returns the alert which needs to be displayed on UI.
	/// </summary>
	/// <returns>The alert if any, otherwise returns 0.</returns>
	WORD GetAlert();

	/// <summary>
	/// Sets player status
	/// </summary>
	/// <param name="cardedPlayer">IN - true if there is a carded player, false otherwise.</param>
	void SetPlayerStatus(bool cardedPlayer);

private:
	u_ProgressiveCriticalText m_progressiveCriticalTextFlags;
};

