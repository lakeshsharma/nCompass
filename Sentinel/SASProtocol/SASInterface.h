#pragma once
#include "SASSerialWrapper.h"
#include "MeterStepdown.h"
#include "SASConsumer.h"
#include "QueueList.h"
#include "SASPollMessage.h"

//// Declare friend class(es) for unit test access.
//namespace SentinelNativeUnitTests { class SASInterfaceUnitTests; }
//namespace SentinelNativeUnitTests { class SASProtocol2; }
//
class CSASInterface
{
public:
	// SAS version constants.
	static const int VERSION_LENGTH = 3; // 1 digit for major field, 2 digits for minor field.
	static const byte DEFAULT_VERSION[VERSION_LENGTH];

	/// <summary>
	/// Initializes a new instance of the <see cref="CSASInterface"/> class for mocking purposes.
	/// </summary>
	/// <param name="serial">Pointer to CSASSerialWrapperMock object</param>
	/// <param name="sasProtocol">The sas protocol.</param>
	/// <param name="gcfToInternalsQueue">The GCF to internals queue.</param>
	/// <param name="internalsToGCFQueue">The internals to GCF queue.</param>
	CSASInterface(CSASSerialWrapper *serial, CSASConsumer *sasProtocol, CQueueList *gcfToInternalsQueue, CQueueList *internalsToGCFQueue);

	/// <summary>
	/// Destructor. Finalizes an instance of the <see cref="CSASInterface"/> class.
	/// </summary>
	virtual ~CSASInterface();

	/// <summary>
	/// Sends poll to sas.
	/// </summary>
	/// <param name="msg">IN/OUT - Messaage to send. MUST NOT be nullptr.</param>
	/// <param name="retryCount">IN - The retry count.</param>
	virtual void SendToSAS(CSASPollMessage *msg, byte retryCount);

	/// <summary>
	/// Gets the game comm status.
	/// </summary>
	/// <returns>Game comm status</returns>
	bool GameComStatus();

	/// <summary>
	/// Gets the SAS startup queue empty status.
	/// </summary>
	/// <returns>The SAS startup queue empty status.</returns>
	bool GetSasStartupQueueEmpty() const;

	/// <summary>
	/// Determines whether there are currently any game comm errors.
	/// </summary>
	/// <returns>True if current error count is 0.</returns>
	bool HasNoErrors() const;

	/// <summary>
	/// Gets the consecutive EGM communications error count.
	/// </summary>
	/// <returns>The consecutive EGM communications error count.</returns>
	/// <remarks>
	/// This value is also exposed (to CInternalsFramework) via the gcfToInternalsQueue using INT_MSG_GAME_COMM_ERROR messages.
	/// </remarks>
	WORD GetConsecutiveCommErrorCount() const;

	/// <summary>
	/// Sets the game comm status.
	/// </summary>
	/// <param name="status">IN - true if game communications are up, false otherwise.</param>
	/// <returns>A message for internals.</returns>
	CInternalMessage *SetGameComStatus(bool status);

	/// <summary>
	/// Process a read queue (from internals) is empty condition.
	/// </summary>
	/// <returns>A message for internals. MAY be NULL.</returns>
	CInternalMessage *ProcessReadQueueIsEmpty();

	/// <summary>
	/// Sets the finding egm status.
	/// </summary>
	/// <param name="status">if set to <c>true</c> [status].</param>
	void SetFindingEGMStatus(bool status);

	/// <summary>
	/// Sets the polling speed.
	/// </summary>
	/// <param name="pollingSpeed">The polling speed in milliseconds.</param>
	void SetPollingSpeed(short pollingSpeed);

	/// <summary>
	/// Gets the polling speed.
	/// </summary>
	/// <returns>The polling speed in milliseconds.</returns>
	short GetPollingSpeed() const;

	/// <summary>
	/// Sets the af supported flag.
	/// </summary>
	/// <param name="supported">if set to <c>true</c> [supported].</param>
	void SetAfSupported(bool supported);

	/// <summary>
	/// Sets the SAS version and sends a message to Internals.
	/// </summary>
	/// <param name="version">IN - A pointer to the SAS version characters.  Set to nullptr for no change.</param>
	void SetVersion(const char *version);

	/// <summary>
	/// Gets a pointer to the SAS version characters.
	/// </summary>
	/// <returns>A pointer to a buffer of SAS version characters, VERSION_LENGTH characters long.</returns>
	const byte *GetVersion() const;

	/// <summary>
	/// Gets a pointer to the SAS version.
	/// </summary>
	/// <returns>The major character of the SAS version.</returns>
	byte GetMajorVersion() const;

	/// <summary>
	/// Gets a pointer to the SAS version characters.
	/// </summary>
	/// <returns>A pointer to the SAS version characters.</returns>
	std::string GetVersionString() const;

	/// <summary>
	/// Sets the ticket expiration duration.
	/// </summary>
	/// <param name="ticketExpirationDuration">IN - The ticket expiration duration in days.</param>
	void SetTicketExpirationDuration(WORD ticketExpirationDuration);

	/// <summary>
	/// Gets the ticket expiration duration.
	/// </summary>
	/// <returns>The ticket expiration duration in days.</returns>
	WORD GetTicketExpirationDuration() const;

	/// <summary>
	/// Sets whether the EGM supports long poll 7D.
	/// </summary>
	/// <param name="egmSupportsLP7B">IN - Sets whether the EGM supports long poll 7B.</param>
	/// <returns>A message for internals. MAY be NULL.</returns>
	CInternalMessage *SetEgmSupportsLP7B(bool egmSupportsLP7B);

	/// <summary>
	/// Update SAS Read Tiemout.
	/// </summary>
	void UpdateSasReadTimeout();

protected:
	CQueueList *m_gcfToInternalsQueue;
	CQueueList *m_internalsToGCFQueue;
	CSASConsumer *m_sasProtocol;
	CSASSerialWrapper *m_serial;
	bool m_isAfSupported;
	byte m_gameAddress;
	int m_totalRetryCount;
	BYTE GetLastPoll(void) const;
	void SetLastPoll(BYTE pollValue);
	BYTE m_lastPollValue;
	WORD m_consecutiveCommErrorCount;
	bool m_findingEGM;
	bool m_gameComStatus;
	bool m_sasStartupQueueEmpty;
	DWORD m_pbtStartTickCount;
	byte m_version[VERSION_LENGTH];
	WORD m_ticketExpirationDuration;

	// Grant "friend" access for testing.
	//friend class SentinelNativeUnitTests::SASInterfaceUnitTests;
	//friend class SentinelNativeUnitTests::SASProtocol2;
};


