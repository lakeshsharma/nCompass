#pragma once
#include "Hardware/MemoryStorage.h"
#include "BonusEvent.h"
#include "InternalMsgContent.h"

class CBonusEvents :
	public CMemoryStorage, public CInternalMsgContent<INT_MSG_CONTENT_BONUSEVENTS>
{
public:
	/// <summary>
	/// Standard constructor.
	/// </summary>
	/// <param name="memoryType">IN - Type of memory used for persistent storage.</param>
	/// <param name="memoryOffset">IN - The memory offset used for persistent storage.</param>
	CBonusEvents(MEMORY_TYPE memoryType = MEMORY_NONE, DWORD memoryOffset = 0);

	/// <summary>
	/// "Copy" constructor.
	/// </summary>
	/// <param name="eventsRHS">IN - The source for the copy construction.</param>
	/// <param name="memoryType">IN - Type of memory used for persistent storage.</param>
	/// <param name="memoryOffset">IN - The memory offset used for persistent storage.</param>
	/// <remarks>
	/// NOTE: Values related to memory type are not copied.
	/// </remarks>
	CBonusEvents(const CBonusEvents &eventsRHS, MEMORY_TYPE memoryType = MEMORY_NONE, DWORD memoryOffset = 0);

	/// <summary>
	/// Destructor.
	/// </summary>
	~CBonusEvents(void);

	/// <summary>
	/// Assignment operator.
	/// </summary>
	/// <param name="eventsRHS">IN - The source for the assignment.</param>
	/// <returns>A reference to the target of the assigment.</returns>
	CBonusEvents &operator=( const CBonusEvents &eventsRHS );  

	/// <summary>
	/// Merges the events.
	/// </summary>
	/// <param name="events">The events.</param>
	void MergeEvents( const CBonusEvents *events );

	static bool DoBonusEventsExistInNVRAM( void );
	virtual void BuildYourself();

	void AddEvent( CBonusEvent *bonusEvent );
	void ClearEvents( void );

	bool AwardBonusPoints( DWORD newPoints );

	bool AwardPartialBonusPoints( DWORD countdownDecrement, DWORD divisor, DWORD multiplier );

	/// <summary>
	/// Get the bonus points awarded during this session.
	/// </summary>
	/// <returns>The bonus points awarded during this session.</returns>
	DWORD GetBonusPoints( void );

	/// <summary>
	/// Returns the number of bonus events.
	/// </summary>
	/// <returns>The number of bonus events</returns>
	int GetBonusEventsCount( void ) const
	{
		return m_bonusEvents.size();
	}

	/// <summary>
	/// Returns a pointer to the CBonusEvent instance corresponding to eventIndex, so the caller can modify the bonus event.
	/// </summary>
	/// <param name="eventIndex">IN - Index of the event.</param>
	/// <returns>A pointer to the CBonusEvent instance corresponding to eventIndex.</returns>
	CBonusEvent *GetBonusEvent( int eventIndex );

	/// <summary>
	/// Returns a pointer to the CBonusEvent corresponding to eventIndex.
	/// </summary>
	/// <param name="eventIndex">IN - Index of the event.</param>
	/// <returns>A pointer to the CBonusEvent corresponding to eventIndex.</returns>
	const CBonusEvent *GetBonusEvent( int eventIndex ) const;

	CBonusEvent *GetOutstandingBonusEvent( bool bIsCardOutTime );
	void SessionReset( void );

	void ClearBonusPoints( void );

	void SetCardNumber( const byte *cardNumber, int cardNumberLen );
	const byte *GetCardNumber( int &cardNumberLen ) const;

	/// <summary>
	/// Reset current award for holding bonus events.
	/// </summary>
	/// <param>void.</param>
	/// <returns>void</returns>
	void ResetCurrentAwardedPoints(void); 
	
	/// <summary>
	/// Gets the current bonus points for all holding bonus events.
	/// </summary>
	/// <param>void.</param>
	/// <returns>Total current bonus points for all bonus event.</returns>
	DWORD GetBonusPointsCurrent(void) const; // gets the current bonus points for all holding bonus events.

private:
	byte *m_cardNumber;
	int m_cardNumberLen;
	int m_oneTimeBonusToSendCount;	// This provides a quick way to tell if we have one time bonus events to send

	void StoreOverheadDataToNVRAM( void );
	void StoreEventToNVRAM( int eventIndex );

protected:
	std::vector<CBonusEvent *> m_bonusEvents;
};
