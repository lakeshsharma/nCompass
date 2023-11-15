#include "stdafx.h"
#include "ProgressiveCriticalText.h"
#include "Logging/LogString.h"

CProgressiveCriticalText::CProgressiveCriticalText() : m_progressiveCriticalTextFlags(0)
{
	m_progressiveCriticalTextFlags.m_data.progressiveDown = true;
}

CProgressiveCriticalText::~CProgressiveCriticalText()
{
}

bool CProgressiveCriticalText::ProcessProgressiveConfig(CProgressiveConfig *progressiveConfig)
{
	// preserve the down state so we can determine if the state has changed after config is applied
	bool stateChanged(false);
	bool wasDown = IsDown();
	if (progressiveConfig != NULL && progressiveConfig->GetActive())
	{
		m_progressiveCriticalTextFlags.m_data.progressiveEnabled = true;
	}
	else
	{
		m_progressiveCriticalTextFlags.m_data.progressiveEnabled = false;
	}

	stateChanged = wasDown != IsDown();
	if (stateChanged)
	{
		LogString(PROGRESSIVE, _T("%s progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
			__FUNCTION__,
			m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
			m_progressiveCriticalTextFlags.m_data.progressiveDown,
			m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
			m_progressiveCriticalTextFlags.m_data.cardedPlayer,
			m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);
	}


	// Return true if the down state was changed
	return stateChanged;
}

bool CProgressiveCriticalText::ProcessConnectionInfo(CProgressiveConnectionInfo *connectionInfo)
{
	bool stateChanged(false);
	// preserve the down state so we can determine if the state has changed after the new info is applied
	bool wasDown = IsDown();

	if (connectionInfo != NULL && connectionInfo->GetState() != Disabled)
	{
		m_progressiveCriticalTextFlags.m_data.progressiveDown = (connectionInfo->GetState() != Connected);
	}
	else
	{
		m_progressiveCriticalTextFlags.m_data.progressiveDown = true;
	}

	stateChanged = wasDown != IsDown();
	if (stateChanged)
	{
		LogString(PROGRESSIVE, _T("%s progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
			__FUNCTION__,
			m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
			m_progressiveCriticalTextFlags.m_data.progressiveDown,
			m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
			m_progressiveCriticalTextFlags.m_data.cardedPlayer,
			m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);
	}

	// Return true if the down state was changed
	return stateChanged;
}

bool CProgressiveCriticalText::ProcessSasStartupQueueEmpty(bool sasStartupQueueEmpty)
{
	bool stateChanged(false);
	// preserve the down state so we can determine if the state has changed after the system state flags are applied
	bool wasDown = IsDown();

	m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty = sasStartupQueueEmpty;

	stateChanged = wasDown != IsDown();
	if (stateChanged)
	{
		LogString(PROGRESSIVE, _T("%s progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
			__FUNCTION__,
			m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
			m_progressiveCriticalTextFlags.m_data.progressiveDown,
			m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
			m_progressiveCriticalTextFlags.m_data.cardedPlayer,
			m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);
	}

	// Return true if the down state was changed
	return stateChanged;
}

bool CProgressiveCriticalText::ProcessEligibilityDetermined(bool eligibilityDetermined)
{
	bool stateChanged(false);

	// Preserve the m_progressiveCriticalTextFlags.m_data.eligibilityDetermined so we can determine if the state has changed after the system state flags are applied.
	bool origEligibilityDetermined = m_progressiveCriticalTextFlags.m_data.eligibilityDetermined;
	m_progressiveCriticalTextFlags.m_data.eligibilityDetermined = eligibilityDetermined;

	stateChanged = origEligibilityDetermined != m_progressiveCriticalTextFlags.m_data.eligibilityDetermined;
	if (stateChanged)
	{
		LogString(PROGRESSIVE, _T("%s progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
			__FUNCTION__,
			m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
			m_progressiveCriticalTextFlags.m_data.progressiveDown,
			m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
			m_progressiveCriticalTextFlags.m_data.cardedPlayer,
			m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);
	}

	// Return true if the m_progressiveCriticalTextFlags.m_data.eligibilityDetermined was changed.
	return stateChanged;
}

bool CProgressiveCriticalText::ProcessSessionIdMatchStatus(bool sessionIdMatchStatus)
{
	bool stateChanged(false);
	// Preserve the down state so we can determine if the state has changed after the system state flags are applied.
	bool wasDown = IsDown();

	m_progressiveCriticalTextFlags.m_data.sessionIdMatchStatus = sessionIdMatchStatus;

	stateChanged = wasDown != IsDown();
	if (stateChanged)
	{
		LogString(PROGRESSIVE, _T("%s progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
			__FUNCTION__,
			m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
			m_progressiveCriticalTextFlags.m_data.progressiveDown,
			m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
			m_progressiveCriticalTextFlags.m_data.cardedPlayer,
			m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);
	}

	// Return true if the down state was changed.
	return stateChanged;
}

WORD CProgressiveCriticalText::GetAlert()
{
	WORD alert = m_progressiveCriticalTextFlags.GetAlert();
	
	LogString(PROGRESSIVE, _T("%s alert=0x%x progressiveEnabled=%d progressiveDown=%d eligibilityDetermined=%d cardedPlayer=%d sasStartupQueueEmpty=%d"),
		__FUNCTION__,
		alert,
		m_progressiveCriticalTextFlags.m_data.progressiveEnabled,
		m_progressiveCriticalTextFlags.m_data.progressiveDown,
		m_progressiveCriticalTextFlags.m_data.eligibilityDetermined,
		m_progressiveCriticalTextFlags.m_data.cardedPlayer,
		m_progressiveCriticalTextFlags.m_data.sasStartupQueueEmpty);

	return alert;
}

void CProgressiveCriticalText::SetPlayerStatus(bool cardedPlayer)
{
	m_progressiveCriticalTextFlags.m_data.cardedPlayer = cardedPlayer;
}

bool CProgressiveCriticalText::IsDown()
{
	return m_progressiveCriticalTextFlags.IsDown();
}

bool CProgressiveCriticalText::IsProcessingDown()
{
	return m_progressiveCriticalTextFlags.IsProcessingDown();
}
