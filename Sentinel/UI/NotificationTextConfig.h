#pragma once

class CNotificationTextConfig
{
public:
	typedef enum
	{
		Idle,
		PleaseInsertCard,
		BalanceRetrieval,
		GoodCardRead,
		GoodCardRead_InPlay,
		BadCardRead,

		VirtualSession,
		BannedFromPlay,
		DownloadPending,
		SystemBusy,
		MaxCardInExceeded,

		MobileSessionAccepted,

		Welcome,
		WelcomeWithName,

		DailyEarnedPoints,
		PointsNeededForNextTier,

		Comp,

		Points,
		PointsWithName,
		PointsAsCash,
		PointsAsCashWithName,

		TotalPoints_SessionPoints,
		TotalPoints_SessionPoints_WithName,
		TotalCash_SessionCash,
		TotalCash_SessionCash_WithName,

		Countdown_Session,
		Countdown_Session_WithName,

		Goodluck_Session,
		Goodluck_Session_WithName,
		Goodluck_SessionPoints,
		Goodluck_SessionPoints_WithName,

		NoCountdownAvailable,

		NumberOfTextEnums  // MUST be last item.  Not valid for use as an enumeration.
	} TextType;

	enum TextPriority
	{
		TEXT_PRIORITY_HIGH     = 1,     // higher alerts
		TEXT_PRIORITY_NORMAL   = 2,     // notification text (card in seq)
		TEXT_PRIORITY_LOW      = 3,     // notification conditional & in-play & lower alerts
		TEXT_PRIORITY_LAST     = 10     // last priority, not to be used
	};

	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CNotificationTextConfig"/> class.
	/// </summary>
	/// <remarks>
	/// Initalized to default values.
	/// </remarks>
	CNotificationTextConfig();

	/// <summary>
	/// Copy constructor. Initializes a new instance of the <see cref="CNotificationTextConfig"/> class.
	/// </summary>
	/// <param name="rhs">IN - The source instance.</param>
	CNotificationTextConfig(const CNotificationTextConfig &rhs);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CNotificationTextConfig"/> class.
	/// </summary>
	~CNotificationTextConfig();

	/// <summary>
	/// Gets the text value for the specified enumeration.
	/// </summary>
	/// <param name="index">IN - The text enumeration to get.</param>
	/// <returns>The text value for the enumeration.</returns>
	/// <remarks>
	/// Returns an empty string if the enumeration is out of range.
	/// </remarks>
	std::string GetText(TextType index) const;

	/// <summary>
	/// Gets the priority of the text.
	/// </summary>
	/// <param name="text">IN - input text.</param>
	/// <returns>The priority of the text.</returns>
	TextPriority GetTextPriority(const std::string text) const;

private:
	std::array<std::string, NumberOfTextEnums> m_texts;
	std::map<std::string, TextPriority> m_textPriorityMap;
	void GenerateTextPriorityMap();
};
