#pragma once

#include "Hardware/MemoryStorage.h"
#include "InternalMessage.h"
#include "TiltsDef.h"
#include "DataSubscriptionProvider.h"

/// <summary>
/// Class CTiltManager.
/// This will be used to generate tilt messages and keep tilt messages from being generated at too fast of a rate.
/// This class will also house the tilt logs.
/// </summary>
class CTiltManager : public CDataSubscriptionProvider
{
public:
	
	/// <summary>
	/// Initializes a new instance of the <see cref="CTiltManager"/> class.
	/// </summary>
	CTiltManager(void);

	/// <summary>
	/// Initializes a new instance of the <see cref="CTiltManager"/> class.
	/// This is for unit testing
	/// </summary>
	/// <param name="memoryType">Type of the memory.Use MEMORY_NONE for unit tests</param>
	/// <param name="tiltTimeoutInSeconds">Timeout for tilt processing.Specifiy for unit tests</param>
	CTiltManager(MEMORY_TYPE memoryType, int tiltTimeoutInSeconds);
	
	/// <summary>
	/// Finalizes an instance of the <see cref="CTiltManager"/> class.
	/// </summary>
	virtual ~CTiltManager();

	/// <summary>
	/// Gets the subscription data response.
	/// </summary>
	/// <param name="whereId">The subscription topic.</param>
	/// <param name="whereId">The where identifier.</param>
	/// <returns>subscription data response</returns>
	virtual std::string GetSubscriptionDataResponse(const std::string& topic, const std::string& whereId);

	/// <summary>
	/// Processes the specifiied tilt code.
	/// This will generate a tilt message to be sent
	/// </summary>
	/// <param name="tilt">The tilt code to process.</param>
	/// <param name="additionaltransactionToSend">
	///		This will be set to a messages type for an additional transaction to be generated.
	///		INT_MSG_NONE will indicate that no transaction is to be generated
	///	</param>
	/// <returns>Returns true if a tilt transaction is to be sent</returns>
	bool ProcessTilt(TiltCode tilt, InternalMsgType &additionaltransactionToSend);

	// Returns a pointer to the tilt code logs
	CLogs *GetTiltLogs();

	/// <summary>
	/// Check if tilt time limit needs to be ignored.
	/// This will generate a tilt message to be sent
	/// </summary>
	/// <param name="tilt">The tilt code to check.</param>
	/// <returns>Returns true if a tilt time limit needs to be ignored, false otherwise</returns>
	bool CheckIfTiltTimeLimitNeedsToBeIgnored(TiltCode tilt) const;

private:

	void Init(MEMORY_TYPE memoryType);

	CLogs *m_tiltLog;

	/// The last 4 tilt codes
	TiltCode m_lastTilt;
	TiltCode m_lastButOneTilt;
	TiltCode m_lastButTwoTilt;
	TiltCode m_lastButThreeTilt;

	// The last 4 tilt code sent times.
	time_t m_lastTiltTime;
	time_t m_lastButOneTiltTime;
	time_t m_lastButTwoTiltTime;
	time_t m_lastButThreeTiltTime;

	int m_tiltTimeoutInSeconds;
};
