#pragma once

enum class CurrentActivity : uint
{
	None,
	Upgrading,
	Rebooting,
	RestartingUi,
	Downloading
};