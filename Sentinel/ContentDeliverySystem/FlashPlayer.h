#pragma once

class CFlashPlayer
{
public:
	/// <summary>
	/// Initializes a new instance of the <see cref="CFlashPlayer"/> class.
	/// </summary>
	/// <param name="windowName">Name of the window.</param>
	CFlashPlayer(const std::string& windowName);

	// Destructor
	/// <summary>
	/// Finalizes an instance of the <see cref="CFlashPlayer"/> class.
	/// </summary>
	~CFlashPlayer();

	/// <summary>
	/// Determines whether flash player running.
	/// </summary>
	/// <returns>Returns true if Flash Player is running, returns false otherwise.</returns>
	bool IsFlashPlayerRunning();

private:
///Functions
	void StartFlashPlayer(const std::string& filename);

	void StopFlashPlayer();

///Attributes
	std::string m_flashPlayerWindowName;
};
