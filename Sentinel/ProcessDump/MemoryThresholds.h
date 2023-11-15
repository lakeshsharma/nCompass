#pragma once

typedef enum {
	MemoryThresholdStatusOkay,
	MemoryThresholdStatusUIRestartAfterDrop,
	MemoryThresholdStatusUIRestartAsap,
	MemoryThresholdStatusRebootAfterDrop,
	MemoryThresholdStatusRebootASAP,
} MemoryThresholdStatusType;

class CMemoryThresholds
{
public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CMemoryThresholds"/> class.
	/// </summary>
	/// <param name="startupTickCount">IN - tick count at time of startup.</param>
	CMemoryThresholds(DWORD startupTickCount);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CMemoryThresholds"/> class.
	/// </summary>
	~CMemoryThresholds(void);

	/// <summary>
	/// Gets the memory threshold status and the available physical memory.
	/// </summary>
	/// <param name="restartUiAfterDrop">IN - Flag to indicate whether UI needs to be restarted after the drop.</param>
	/// <param name="isAbleToRestartUi">IN - Flag to indicate whether UI can be restarted.</param>
	/// <param name="currentTickCount">IN - The current tick count.</param>
	/// <param name="rebootAsapThresholdInMb">IN - The reboot ASAP threshold in MB.</param>
	/// <param name="rebootWarningThresholdMb">IN - The reboot warning threshold in MB.</param>
	/// <param name="restartUIAsapThresholdMb">IN - The restart UI ASAP threshold in MB..</param>
	/// <param name="restartUIAfterDropThresholdMb">IN - The restart UI after the drop threshold in MB.</param>
	/// <param name="availablePhysicalMemory">OUT - The available physical memory.</param>	
	/// <returns>The memory threshold status.</returns>
	/// <remarks>
	/// Indicate if and when to reboot based on memory thresholds.
	/// </remarks>	
	MemoryThresholdStatusType GetMemoryThresholdStatus(
		bool restartUiAfterDrop,
		bool isAbleToRestartUi,
		DWORD rebootAsapThresholdInMb,
		DWORD rebootWarningThresholdMb,
		DWORD restartUIAsapThresholdMb,
		DWORD restartUIAfterDropThresholdMb,
		DWORD currentTickCount, 
		DWORD &availablePhysicalMemory);

private:
	// Disable default constructor, default copy constructor, and default assignment operator.
	CMemoryThresholds(const CMemoryThresholds &rhs);
	CMemoryThresholds& operator=(const CMemoryThresholds &rhs);

	DWORD m_startupTickCount;
	bool m_mininumRebootTimeExpired; // Has the minimum time since startup in order to reboot ASAP expired?
};
