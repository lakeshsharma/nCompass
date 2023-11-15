#pragma once

#include "ProcessDump/OperatingSystem.h"
#include "ScreenDefs.h"

/// <summary>
/// Data class for counter items
/// </summary>
class CBonusCounterItem
{
public:
	static const DWORD DEFAULT_DISPLAY_TIME = 7000; // 7 seconds.

	/// <summary>
	/// Top Priority Level Value
	/// </summary>
	static const int TOP_PRIORITY_LEVEL_ID = 0;

	/// <summary>
	/// Invalid Level Id, ID when this object is not a ProgressiveCounterItem
	/// </summary>
	static const int INVALID_VALUE = -1;

	/// <summary>
	/// Ctor with parameters
	/// </summary>
	/// <param name="operatingSystem">The operating system.</param>
	/// <param name="bonusCounterType">Type of the bonus counter.</param>
	/// <param name="itemId">IN - id of the default item</param>
	/// <param name="priorityLevel">IN - Prority Level of the item</param>
	/// <param name="displayTime">The display time.</param>
	CBonusCounterItem(IOperatingSystem* operatingSystem, BonusCounterType bonusCounterType, 
		long long itemId, WORD priorityLevel, DWORD displayTime = DEFAULT_DISPLAY_TIME);

	/// <summary>
	/// Default Dtor
	/// </summary>
	virtual ~CBonusCounterItem();

	/// <summary>
	/// The assignment operator for bonus counter.
	/// </summary>
	/// <param name="rhs">The bonus counter object to assign from.</param>
	/// <returns>reference to CBonusCounterItem object</returns>
	virtual CBonusCounterItem & operator = (const CBonusCounterItem &rhs);

	/// <summary>
	/// Equality comparison for use in STL List and sorting
	/// </summary>
	/// <param name="rhs">IN - the other item to compare</param>
	/// <returns>True if this instance is equal and false otherwise</returns>
	virtual bool operator == (const CBonusCounterItem &rhs);

	/// <summary>
	/// Less than comparison for use in STL List for sorting
	/// </summary>
	/// <param name="second">IN - the other item to compare</param>
	/// <returns>True if this instance is less than and false otherwise</returns>
	bool operator < (const CBonusCounterItem &second);

	/// <summary>
	/// Returns the id of the item
	/// </summary>
	/// <returns>the item id of the bonus counter item</returns>
	long long GetID()const;

	/// <summary>
	/// Gets the Priority level of the item
	/// </summary>
	/// <returns>the priority level value</returns>
	WORD GetPriorityLevel() const;

	/// <summary>
	/// Gets the Bonus Counter Type of the item
	/// </summary>
	/// <returns>the Bonus Counter Type</returns>
	BonusCounterType GetBonusCounterType() const;

	/// <summary>
	/// Sets the Priority level of the item
	/// </summary>
	void SetPriorityLevel(WORD priorityLevel);

	/// <summary>
	/// Gets the parameters.
	/// </summary>
	/// <returns>the parameters for the module</returns>
	virtual std::string GetParameters() const;

	/// <summary>
	/// Checks the timed event.
	/// </summary>
	/// <param name="timer">The timer.</param>
	/// <returns>true item is okay to be displayed. false if set to not display or the display time expired</returns>
	virtual bool CheckTimedEvent(time_t timer);

	/// <summary>
	/// Determines if this item needs to be displayed upon add or update
	/// </summary>
	/// <param name="currentlyDisplayedItem">The currently displayed item. May be NULL</param>
	/// <returns>true if item needs to be displayed now, false otherwise</returns>
	virtual bool DisplayOnAddOrUpdate(CBonusCounterItem *currentlyDisplayedItem);

	/// <summary>
	/// Resets the display start time.
	/// </summary>
	void ResetDisplayStartTime();

	/// <summary>
	/// Sets item to display.
	/// </summary>
	/// <param name="display">true if item is to be displayed. false if item is not to be displayed.</param>
	void SetItemToDisplay(bool display);

	/// <summary>
	/// Determines whether [is level counter].
	/// </summary>
	/// <returns>true if bonus counter is a level counter, false otherwise</returns>
	bool IsLevelCounter() const;
protected:
	BonusCounterType m_bonusCounterType;
	long long m_id;
private:
	IOperatingSystem* m_operatingSystem;
	DWORD m_displayTime;
	DWORD m_startTickCount;
	WORD m_priorityLevel;
	bool m_okToDisplay;

	CBonusCounterItem() = delete;
};

