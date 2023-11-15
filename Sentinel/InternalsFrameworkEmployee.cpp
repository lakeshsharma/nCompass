#include "stdafx.h"
#include "InternalsFramework.h"

// Sentinel includes.
#include "Logging/Logger.h"
#include "Logging/LogString.h"

using namespace std;

// Uninitialize employees at shutdown
void CInternalsFramework::UninitializeEmployee( void )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	if ( m_employee1 != NULL )
	{
		delete m_employee1;
		m_employee1 = NULL;
	}

	if ( m_employee2 != NULL )
	{
		delete m_employee2;
		m_employee2 = NULL;
	}
}

// This gets called when an employee card is inserted.
// Returns true if the card was accepted.
bool CInternalsFramework::EmployeeCardInserted( CCard *card, bool goodbyeEnabled )
{
	m_sentinelState.SetEmployeeGoodbyeEnabled(goodbyeEnabled);

	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);
	
	CardType cardType = DetermineCardType( card );

	bool bCardAccepted = true;
	
	// This will close out any open employee sessions just in case.
	// NOTE: this wont close out a virtual session 
	EndActiveEmployeeSession( card->GetCardInTime(), false );

	// See if we currently have employee (will be virtual at this point.)
	if ( m_employee1 != nullptr )
	{
		// If this is the current player, make it active (not virtual) again.
		if ( m_employee1->CompareCard( card ) )
		{
			m_employee1->MakeActive( card );
		}

		// Otherwise make a second employee
		else
		{
			m_employee2 = new CEmployee(card,
				m_config,
				goodbyeEnabled,
				m_playerSession.GetEmployeeOverPlayerIdleCardOut() && IsPlayerVirtualFromEmployeeCardIn(),
				m_playerSession.GetEmployeeIdleCardOutSeconds() * MILLISECONDS_PER_SECOND,
				CUtilities::GetTickCount());
		}
	}

	// There are senarios where we do not accept a currency card or meter card.
	else if ( cardType == CARD_CURRENCY || cardType == CARD_METER )
	{
		//If there is a meter change in progress, disregard currency card or meter card.
		//If we are in an offline state, disregard currency card or meter card.
		//If transaction fill status is not Okay
		if (m_sentinelState.IsMeterChangeInProgress() ||
			m_EGMConfig.GetEGMProtocolState() != protocolStateEnabled ||
			!m_sentinelState.TransactionFillStatusOkay())
		{
			bCardAccepted = false;
		}
	}

	// If we are going to accept this card
	if ( bCardAccepted )
	{
		// Before we display employee screens, check if we will clear a virtual bad card read.
		if (m_playerSession.IsVirtualBadCardRead())
		{
			if (CDisplayManager::Instance().GetDisplayPlatform() == VideoMixerPlatform)
			{
				// There may be a bad read currently displayed on Media Window.
				// Clear the bad read (if any) on Media Window.
				PerformVirtualBadCardReadExpired(false);
			}
		}

		// Make an employee if needed.
		if ( m_employee1 == NULL )
		{
			m_employee1 = new CEmployee(card,
				m_config,
				goodbyeEnabled,
				m_playerSession.GetEmployeeOverPlayerIdleCardOut() && IsPlayerVirtualFromEmployeeCardIn(),
				m_playerSession.GetEmployeeIdleCardOutSeconds() * MILLISECONDS_PER_SECOND,
				CUtilities::GetTickCount());
		}

		// Send card transaction
		SendEmployeeCardMessageToSCF( true );

		// Check to see if we need to auto-open/close the service window or if calibration needs to be started
		bool closeServiceWindow = false; 
		bool startCalibration = false;
		m_displayManager->EmployeeCardInserted(closeServiceWindow, startCalibration);
		
		if (closeServiceWindow)
		{
			SendServiceWindowShowToUI(false);
		}
		SendEmployeeCardInStatusToUI();
		SendEmployeeDisplayStart(false, false, startCalibration);
	}
	
	if(bCardAccepted)
	{
		m_notificationBox.SetEmployeeCardFlag(true);
	}
	
	return bCardAccepted;
}

bool CInternalsFramework::EmployeeCardRemoved( time_t removalTime )
{
	bool employeeCardRemoved = false;
	
	SetPlayerLastPlay(false); // If a player session is in progress, restart the last activity timer for the player.

	{
		lock_guard<recursive_mutex> lock(m_employeeCriticalSection);
		
		LogString("GENERAL", "DEBUG: Employee Card Removed.");

		employeeCardRemoved = EndActiveEmployeeSession(removalTime, m_bCurrentlyPerformingSnapshot);
	}

	if ( employeeCardRemoved )
	{
		bool restartUI = false;
		m_displayManager->EmployeeCardRemoved(restartUI);
		
		if (restartUI)
		{
			SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW, (WORD)m_displayManager->CalibrationError()), QUEUE_PRIORITY_HIGHEST);
		}
		
		m_notificationBox.SetEmployeeCardFlag(false);
	}
	return employeeCardRemoved;
}

bool CInternalsFramework::IsEmployeeVirtual( void )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	bool bIsVirtual = false;

	return bIsVirtual;
}

bool CInternalsFramework::DoesEmployeeExist( void )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	bool bDoesEmployeeExist = (m_employee1 != NULL);

	return bDoesEmployeeExist;
}

void CInternalsFramework::ValidateEnteredEmployeePin( const std::string &pin )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	CEmployee *employee = m_employee1;
	if ( m_employee2 != NULL )
		employee = m_employee2;

	if ( employee != NULL )
	{
		employee->SetEnteredPin( pin );
		employee->SetWaitingForPin( true );	// Set the timer to wait for an employee pin from SCF

		// See if teh employee pin already got here from the SCF.
		ValidateEmployeePin( employee );
	}
}

void CInternalsFramework::ProcessEmployeePin( byte *cardID, int cardIDLen, const std::string &pin )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	CEmployee *employee = m_employee1;
	if ( m_employee2 != NULL )
		employee = m_employee2;

	if ( employee != NULL )
	{
		if ( employee->CompareCard( cardID, cardIDLen ) )
		{
			employee->SetPin( pin );

			if ( employee->IsWaitingForPin() )
			{
				ValidateEmployeePin( employee );
			}
		}
	}
}

void CInternalsFramework::EmployeeTimedEvents( void )
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	CEmployee *employee = m_employee1;
	if ( m_employee2 != NULL )
		employee = m_employee2;

	if ( employee != NULL )
	{
		// If we are waiting for an employee pin, see if we have timed out
		if ( employee->IsWaitingForPin() )
		{
			ValidateEmployeePin( employee );
		}

		if (employee->IsGoodbyeEnabled())
		{
			if (employee->HasIdleTimeoutOccurred(CUtilities::GetTickCount()))
			{
				m_employeeGoodbyeRequested = true;
			}
		}
	}
	
    PerformEGMDisplayCheck();
    PerformExternalFilesCheck();
	PerformVideoModeRevertCheck();
	PerformCalibrationCompleteCheck();
}

void CInternalsFramework::PerformExternalFilesCheck()
{
    bool playUnmountUSBSound = false;

	// See if we need to revert the video mode
	if (m_displayManager->CheckForExternalVideoModeConfig() == VIDEO_CHANGE_RESTARTING_UI)
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
	}

    // Process the external video mixer diagnostics file
    if ( m_displayManager->CheckForExternalVideoMixerDiagnosticsFile() )
    {
        playUnmountUSBSound = true;
    }

    if (playUnmountUSBSound) 
    {
		SendMessageToUI(new CInternalMessage(INT_MSG_PLAY_UNMOUNT_SOUND), QUEUE_PRIORITY_HIGHEST);
    }
}

void CInternalsFramework::PerformEGMDisplayCheck()
{
	// See if we need to check for to see if there is an EGM display.
	m_displayManager->CheckForEGMDisplay();
}

void CInternalsFramework::PerformVideoModeRevertCheck()
{
	// See if we need to revert the video mode
	if (m_displayManager->CheckForVideoModeRevert() == VIDEO_CHANGE_RESTARTING_UI)
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW), QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::StartCalibration()
{
	if (m_displayManager->StartCalibration() == Calibration::WaitBrowserTermination)
	{
		SendMessageToUI( new CInternalMessage(INT_MSG_TERMINATE_BROWSER), QUEUE_PRIORITY_HIGHEST);
		m_protocolManager->ClearInboundQueueStarvation(IProtocolManager::UIProtocol, PERSIST_CONTENT_WINDOW);
	}
}

void CInternalsFramework::PerformCalibrationCompleteCheck()
{
	if (m_displayManager->CheckForCalibrationComplete() == Calibration::CalibrationComplete)
	{
		m_sentinelState.SetUIRestartInProgress(true);
		LogString(DISPLAY_LOG, "Calibration complete. Sending message to restart UI");
		SendMessageToUI(new CInternalMessage(INT_MSG_RESTART_SERVICEWINDOW, (WORD)m_displayManager->CalibrationError()), QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::ProcessEmployeeButtonActivity()
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	CEmployee *employee = m_employee1;
	if (m_employee2 != NULL)
		employee = m_employee2;

	if (employee != NULL)
	{
		employee->UpdateActivityTimestamp(CUtilities::GetTickCount());
	}
}

void CInternalsFramework::ValidateEmployeePin( CEmployee *employee )
{
	// If the pin is cool, tell the UI to display the proper screen
	if ( employee->CompareEnteredPin() )
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, (WORD)true), QUEUE_PRIORITY_HIGHEST);
		employee->SetWaitingForPin( false );
	}

	// The pin is not cool. Tell the UI.
	else if ( employee->HasPinBeenReceived() )
	{
		SendMessageToUI(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, (WORD)false), QUEUE_PRIORITY_HIGHEST);
		employee->SetWaitingForPin( false );
	}

	// The pin has not been received from SCF. See if the timer expired for waiting for the pin.
	else if ( employee->IsDoneWaitingForPin() )
	{
		if ( employee->IsGlobalGlobal() )
		{
			// The pin is not cool. Tell the UI.
			SendMessageToUI(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, (WORD)false), QUEUE_PRIORITY_HIGHEST);
			employee->SetWaitingForPin( false );
		}
		else
		{
			SendMessageToUI(new CInternalMessage(INT_MSG_EMPLOYEE_PIN_RESULT, (WORD)true), QUEUE_PRIORITY_HIGHEST);
			employee->SetWaitingForPin( false );
		}
	}
}

bool CInternalsFramework::EndActiveEmployeeSession( const time_t removalTime, bool bCanMakeVirtual )
{
	bool bEmployeeCardRemoved = false;
	bool bDeleteEmployee = false;

	// See if there is an eligible employee to remove the card for.
	if ( m_employee2 != NULL || (m_employee1 != NULL && !m_employee1->IsVirtual()) )
	{		
		bEmployeeCardRemoved = true;
		
		// Set card removal in employee2 (can't make this guy virtual)
		if ( m_employee2 != NULL )
		{
			m_employee2->CardRemoved( removalTime, false );
		}

		// Set card removal in employee1 and make virtual if necessary.
		else
		{
			m_employee1->CardRemoved( removalTime, bCanMakeVirtual );
		}
	}

	// Find out if either emplyee can be deleted.
	if ( m_employee2 != NULL || (m_employee1 != NULL && !m_employee1->IsVirtual()) )
	{
		bDeleteEmployee = true;
	}

	// Send the card out message to the SCF
	if ( bEmployeeCardRemoved )
	{
		SendEmployeeCardMessageToSCF( false );
	}

	// Delete the approriate employee
	if ( bDeleteEmployee )
	{
		if ( m_employee2 != NULL )
		{
			delete m_employee2;
			m_employee2 = NULL;
		}
		else
		{
			delete m_employee1;
			m_employee1 = NULL;
		}
	}

	if (bEmployeeCardRemoved || bDeleteEmployee)
	{
		SendEmployeeCardInStatusToUI();
	}

	return bEmployeeCardRemoved;
}

// UI_REWRITE_CLEANUP: Validate if we ar missing any use-case of employee screen launch this way, or we want to keep this around for media window use case reference.
void CInternalsFramework::SendEmployeeDisplayStart(bool showVideoModeRevert, bool pinRequestOverride, bool startCalibration)
{
	lock_guard<recursive_mutex> lock(m_employeeCriticalSection);

	CEmployee *employee(m_employee2 != nullptr ? m_employee2 : m_employee1);

	if (employee != nullptr)
	{
		CInternalMessage *msg(new CInternalMessage(INT_MSG_DISPLAY_EMPLOYEE_SCREEN, (WORD)employee->GetEmployeeCardType(), (WORD)startCalibration));
		msg->SetData((DWORD)showVideoModeRevert, (DWORD)(pinRequestOverride ? false : employee->EmployeeRequiresPin()));
		SendMessageToUI(msg, QUEUE_PRIORITY_HIGHEST);
	}
}

void CInternalsFramework::SendEmployeeCardInStatusToUI()
{
	SendMessageToUI(new CInternalMessage(INT_MSG_EMPLOYEE_CARD_IN, (WORD)(m_employee1 != NULL)), QUEUE_PRIORITY_HIGHEST);
	SendSubscriptionUpdateIfNecessary();
}
