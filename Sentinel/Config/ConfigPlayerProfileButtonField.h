#pragma once
#include "ConfigField.h"

class CConfigPlayerProfileButtonField : public CConfigField
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CConfigPlayerProfileButtonField"/> class.
	/// </summary>
	CConfigPlayerProfileButtonField(void);

	/// <summary>
	/// Finalizes an instance of the <see cref="CConfigPlayerProfileButtonField"/> class.
	/// </summary>
	~CConfigPlayerProfileButtonField(void);

	/// <summary>
	/// Sets the display player profile button.
	/// </summary>
	/// <param name="displayProfileButton">if set to <c>true</c> [display profile button].</param>
	void SetDisplayPlayerProfileButton(bool displayProfileButton);

	/// <summary>
	/// Displays the player profile button.
	/// </summary>
	/// <returns>true to display player profile button, false otherwise.</returns>
	bool DisplayPlayerProfileButton() const;

private:
	bool m_displayPlayerProfileButton;
};


