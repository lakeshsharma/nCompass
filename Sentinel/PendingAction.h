#pragma once

enum PendingAction
{
	NonePending,
	RestartUI,
	RebootAfterDrop,
	RebootAsap,
	OsUpgrade,
	RemoteReboot,
	RefreshUI
};
