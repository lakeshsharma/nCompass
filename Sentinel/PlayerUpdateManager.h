#pragma once
#include "Player.h"

// Declare friend class(es) for unit test access.
namespace SentinelNativeUnitTests { class PlayerUpdateManager; }

class CPlayerUpdateManager
{

public:
	/// <summary>
	/// Default constructor. Initializes a new instance of the <see cref="CPlayerUpdateManager"/> class.
	/// </summary>
	CPlayerUpdateManager();

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CPlayerUpdateManager"/> class.
	/// </summary>
	~CPlayerUpdateManager();
	
	/// <summary>
	/// Add the update time to the list of update times.
	/// </summary>
	/// <param name="updateTime">A pointer to a time_t object that needs to be added.</param>
	/// <remarks>
	/// This function is a consumer of updateTime. This class is responible for deleting object
	/// </remarks>
	void AddPlayerUpdateTime(time_t *updateTime);

	/// <summary>
	/// Checks to see if an update for this player needs to be sent
	/// </summary>
	/// <param name="lastPlayerUpdateTime">The last time the player update was sent</param>
	/// <returns>true if the state changed player update needs to be sent</returns>
	bool CheckIfPlayerUpdateNeedsToBeSent(time_t lastPlayerUpdateTime);

	/// <summary>
	/// Checks to see if an update for this player needs to be flushed
	/// </summary>
	/// <param name="lastPlayerUpdateTime">The last time the player flush was sent</param>
	/// <returns>true if the state changed player update needs to be sent</returns>
	bool CheckIfPlayerBeFlushed(time_t lastPlayerUpdateTime);

	/// <summary>
	/// Clear player update times list
	/// </summary>
	void ClearPlayerUpdateTimes();

private:
	std::recursive_mutex m_criticalSection;

	std::list<void *> m_playerUpdateTimes;

	// Grant "friend" access for testing.
	friend class SentinelNativeUnitTests::PlayerUpdateManager;
	FRIEND_TEST(PlayerUpdateManager, PlayerUpdateTimeTest);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime0a);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime0b);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime1);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime11);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime2a);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime2b);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime3a);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime3b);
	FRIEND_TEST(PlayerUpdateManager, AddPlayerUpdateTime3c);
	FRIEND_TEST(PlayerUpdateManager, ClearPlayerUpdateTimesTest);
};

