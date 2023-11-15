#pragma once
#include "Hardware/MemoryStorage.h"
#include "Card.h"
#include "Config/Config.h"
#include "InternalMsgContent.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class EmployeeUnitTests; }

class CEmployee :
	public CInternalMsgContent<INT_MSG_CONTENT_EMPLOYEE>
{
	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::EmployeeUnitTests;

public:
	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CEmployee"/> class.
	/// </summary>
	/// </summary>
	/// <param name="card">The card.  The caller MUST NOT access after calling.</param>
	/// <param name="config">IN - The configuration.</param>
	/// <param name="goodbyeEnabled">IN - true if goodbye functionality should be enabled, false otherwise.</param>
	/// <param name="idleTimeout">IN - true if the idle timeout is enabled, false otherwise.</param>
	/// <param name="idleTimeoutDuration">IN - Duration of the idle timeout in milliseconds.</param>
	/// <param name="activityTimestamp">IN - The activity timestamp. Tick counts in milliseconds.</param>
	CEmployee(CCard *card,
		const CConfig &config,
		bool goodbyeEnabled,
		bool idleTimeout,
		DWORD idleTimeoutDuration,
		DWORD activityTimestamp);

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CEmployee"/> class.
	/// </summary>
	/// <param name="empRHS">IN - The source instance.</param>
	/// <remarks>
	/// The new instance is not a copy of the source instance.
	/// </remarks>
	CEmployee( const CEmployee &empRHS );

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CEmployee"/> class.
	/// </summary>
	~CEmployee(void);

	/// <summary>
	/// Gets whether a goodbye button should be enabled for this employee session.
	/// </summary>
	/// <returns>true if a goodbye button should be enabled, false otherwise.</returns>
	bool IsGoodbyeEnabled() const;

	/// <summary>
	/// Update the activity timestamp.
	/// </summary>
	/// <param name="activityTimestamp">IN - The activity timestamp. Tick counts in milliseconds.</param>
	void UpdateActivityTimestamp(DWORD activityTimestamp);

	/// <summary>
	/// Determines whether the idle timeout is currently timed out.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count.</param>
	/// <returns>true if the idle timeout is currently timed out, false otherwise.</returns>
	/// <remarks>
	/// Uses the idle timeout configuration (and player state) from when the instance was created using the standard constructor.
	/// The current nCompass configuration (and player state) may be different.
	/// </remarks>
	bool HasIdleTimeoutOccurred(DWORD tickCount) const;

	/// <summary>
	/// Assigment operator.
	/// </summary>
	/// <param name="empRHS">IN - The source instance.</param>
	/// <returns>A reference to the target of the assignment.</returns>
	CEmployee &operator=( const CEmployee &empRHS );

	void CardRemoved( const time_t &removalTime, bool bMakeVirtual );
	bool MakeVirtual( const time_t &cardOutTime );
	void MakeActive( CCard *newCard );

	/// <summary>
	/// Gets whether the employee session is virtual.
	/// </summary>
	/// <returns>true if the employee session is virtual, false otherwise.</returns>
	bool IsVirtual( void ) const;

	bool CompareCard( CCard *card )
	{
		return m_card->Compare( card );
	}

	bool CompareCard( byte *cardID, int cardIDLen )
	{
		return m_card->Compare( cardID, cardIDLen );
	}

	/// <summary>
	/// Gets the employee card.
	/// </summary>
	/// <returns>A pointer to a const card.  MAY be NULL.</returns>
	const CCard *GetCard( void ) const;

	bool IsGlobalGlobal()
	{
		return m_bIsGlobalGlobal;
	}

	bool EmployeeRequiresPin( void )
	{
		// If we have a global global card and the global global password is empty, we don't need a pin.
		return ( (!m_bIsGlobalGlobal && m_bEmployeePinRequired) || (m_bIsGlobalGlobal && !m_globalGlobalPassword.empty()) );
	}

	const std::string &GetPin( void )
	{
		return m_pin;
	}
	void SetPin( const std::string &pin )
	{
		m_pin = pin;
	}
	bool HasPinBeenReceived( void )
	{
		return !m_pin.empty();
	}

	void SetEnteredPin( const std::string &pin )
	{
		m_enteredPin = pin;
	}

	bool ComparePin( const std::string &pin );

	bool CompareEnteredPin( void )
	{
		return ComparePin( m_enteredPin );
	}

	CardType GetEmployeeCardType( void );

	bool IsWaitingForPin( void )
	{
		return m_bWaitingForPin;
	}
	void SetWaitingForPin( bool bWaitingForPin );
	bool IsDoneWaitingForPin( void );

private:
	bool m_bIsVirtual;
	bool m_isGoodbyeEnabled;
	bool m_idleTimeout;
	DWORD m_idleTimeoutDuration; // In milliseconds.
	DWORD m_activityTimestamp; // Tick count in milliseconds.

	CCard *m_card;
	std::string m_name;

	bool m_bEmployeePinRequired;
	bool m_bIsGlobalGlobal;
	std::string m_pin;
	std::string m_enteredPin;
	std::string m_globalGlobalPassword;

	bool m_bWaitingForPin;
	DWORD m_pinRequestTickCount;
};
