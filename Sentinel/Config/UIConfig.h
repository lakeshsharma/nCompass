#pragma once
#include "Config.h"
#include "InternalMsgContent.h"

class CUIConfig : public CInternalMsgContent < INT_MSG_CONTENT_UI_CONFIG >
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CUIConfig"/> class.
	/// </summary>
	/// <remarks>
	/// Initalized to default values.
	/// </remarks>
	CUIConfig();

	/// <summary>
	/// Standard constructor. Initializes a new instance of the <see cref="CUIConfig"/> class.
	/// </summary>
	/// <param name="config">IN - a CConfig instance.</param>
	/// <param name="mobileEnabled">IN - mobile enabled flag.</param>
	/// <remarks>
	/// Initial values are set using values from the CConfig instance.
	/// </remarks>
	CUIConfig(const CConfig& config, bool mobileEnabled);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CUIConfig"/> class.
	/// </summary>
	~CUIConfig();

	/// <summary>
	/// Gets the duration time in seconds for showing player promotion messages in the notification box.
	/// </summary>
	/// <returns>The duration time in seconds for showing player promotion messages in the notification box.</returns>
	WORD GetNotificationForShowPromotionTimeInSeconds() const;

	/// <summary>
	/// Gets the duration time in milliseconds (tick counts) for showing player promotion messages in the notification box.
	/// </summary>
	/// <returns>The duration time in milliseconds (tick counts) for showing player promotion messages in the notification box.</returns>
	DWORD GetNotificationForShowPromotionTimeInMilliseconds() const;

	/// <summary>
	/// Gets the duration time in seconds for showing player tracking messages in the notification box.
	/// </summary>
	/// <returns>The duration time in seconds for showing player tracking messages in the notification box.</returns>
	WORD GetNotificationForPlayerTrackingTimeInSeconds() const;

	/// <summary>
	/// Gets the duration time in milliseconds (tick counts) for showing player tracking messages in the notification box.
	/// </summary>
	/// <returns>The duration time in milliseconds (tick counts) for showing player tracking messages in the notification box.</returns>
	DWORD GetNotificationForPlayerTrackingTimeInMilliseconds() const;

	/// <summary>
	/// Gets the number of buttons on resort menu.
	/// </summary>
	/// <returns>The number of buttons on resort menu.</returns>
	WORD GetResortMenuButtonCount() const;

	/// <summary>
	/// Gets whether Custom button 1 is enabled.
	/// </summary>
	/// <returns>true if Custom button 1 is enabled, false otherwise.</returns>
	bool GetCustomButton1Enabled() const;

	/// <summary>
	/// Gets whether Custom button 2 is enabled.
	/// </summary>
	/// <returns>true if Custom button 2 is enabled, false otherwise.</returns>
	bool GetCustomButton2Enabled() const;

	/// <summary>
	/// Gets the number of buttons on custom menu 1.
	/// </summary>
	/// <returns>The number of buttons on custom menu 1.</returns>
	WORD GetCustomMenuButton1Count() const;

	/// <summary>
	/// Gets the number of buttons on custom menu 2.
	/// </summary>
	/// <returns>The number of buttons on custom menu 2.</returns>
	WORD GetCustomMenuButton2Count() const;

	/// <summary>
	/// Gets the mobile enabled flag.
	/// </summary>
	/// <returns>mobile enabled flag value.</returns>
	bool IsMobileEnabled() const;
	
	/// <summary>
	/// Gets the bonus counter Notification priority
	/// </summary>
	/// <returns>bonus counter Notification priority</returns>
	WORD GetBonusCounterNotificationPriority() const;

	/// <summary>
	/// Gets the bonus counter nRich Level priority
	/// </summary>
	/// <returns>bonus counter nRich Level priority</returns>
	WORD GetBonusCounternRichLevelPriority() const;

	/// <summary>
	/// Gets the bonus counter Splashdown priority
	/// </summary>
	/// <returns>bonus counter Splashdown priority</returns>
	WORD GetBonusCounterSplashdownPriority() const;

	/// <summary>
	/// Gets the bonus counter Progressive Odometer priority
	/// </summary>
	/// <returns>bonus counter Progressive Odometer priority</returns>
	WORD GetBonusCounterProgressiveOdometerPriority() const;

	/// <summary>
	/// Gets the bonus counter PTE priority
	/// </summary>
	/// <returns>bonus counter PTE priority</returns>
	WORD GetBonusCounterPtePriority() const;

	/// <summary>
	/// Gets the bonus counter Scatter Countdown Timer priority
	/// </summary>
	/// <returns>bonus counter Scatter Countdown Timer priority</returns>
	WORD GetBonusCounterScatterCountdownTimerPriority() const;

	/// <summary>
	/// Gets the bonus counter prize odometer priority
	/// </summary>
	/// <returns>bonus counter prize odometer priority</returns>
	WORD GetBonusCounterPrizePriority() const;

	/// <summary>
	/// Gets the bonus counter playxgety odometer priority
	/// </summary>
	/// <returns>bonus counter playxgety odometer priority</returns>
	WORD GetBonusCounterPlayXGetYPriority() const;

	/// <summary>
	/// Sets the number of buttons on resort menu.
	/// </summary>
	/// <param name="buttonCount">IN - button count.</param>
	void SetResortMenuButtonCount(WORD buttonCount);

	/// <summary>
	/// Sets Custom 1 button enabled flag.
	/// </summary>
	/// <param name="enable">IN - enable button flag.</param>
	void SetCustomButton1Enabled(bool enable);

	/// <summary>
	/// Sets Custom 2 button enabled flag.
	/// </summary>
	/// <param name="enable">IN - enable button flag.</param>
	void SetCustomButton2Enabled(bool enable);

	/// <summary>
	/// Sets the number of buttons on custom menu 1.
	/// </summary>
	/// <param name="buttonCount">IN - button count.</param>
	void SetCustomMenuButton1Count(WORD buttonCount);

	/// <summary>
	/// Sets the number of buttons on custom menu 2.
	/// </summary>
	/// <param name="buttonCount">IN - button count.</param>
	void SetCustomMenuButton2Count(WORD buttonCount);

private:
	WORD m_notificationForShowPromotionTime;
	WORD m_notificationForPlayerTrackingTime;
	WORD m_resortMenuButtonCount;
	bool m_customButton1Enabled;
	bool m_customButton2Enabled;
	WORD m_customMenuButton1Count;
	WORD m_customMenuButton2Count;
	bool m_mobileEnabled;
	WORD m_bonusCounterNotificationPriority;
	WORD m_bonusCounternRichLevelPriority;
	WORD m_bonusCounterSplashdownPriority;
	WORD m_bonusCounterProgressiveOdometerPriority;
	WORD m_bonusCounterPtePriority;
	WORD m_bonusCounterScatterCountdownTimerPriority;
	WORD m_bonusCounterPrizePriority;
	WORD m_bonusCounterPlayXGetYPriority;
};
