#pragma once
#include "Hardware/MemoryStorage.h"
#include "Card.h"

class CPersonalBankerAutoUpload
{
public:
// #pragma region constructors
	/// <summary>
	/// Default constructor for a CPersonalBankerAutoUpload instance.
	/// </summary>
	CPersonalBankerAutoUpload(void);

	/// <summary>
	/// Copy constructor for a CPersonalBankerAutoUpload instance.
	/// </summary>
	CPersonalBankerAutoUpload(const CPersonalBankerAutoUpload &rhs);
// #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor for a CPersonalBankerAutoUpload instance.
	/// </summary>
	virtual ~CPersonalBankerAutoUpload(void);
// #pragma endregion destructor

// #pragma region operators
	/// <summary>
	/// Assignment operator for a CPersonalBankerAutoUpload instance.
	/// </summary>
	CPersonalBankerAutoUpload& operator=(const CPersonalBankerAutoUpload &rhs);
// #pragma endregion operators

// #pragma region public functions
	/// <summary>
	/// Sets whether there is an automatic PersonalBanker transfer is in process.
	/// </summary>
	/// <param name="autoInProcess">IN - true to indicate there is an automatic PersonalBanker transfer is in process, false otherwise.</param>
	void SetAutoInProcess(bool autoInProcess);

	/// <summary>
	/// Returns true if an automatic PersonalBanker transfer is in process, false otherwise.
	/// </summary>
	/// <returns>true if an automatic PersonalBanker transfer is in process, false otherwise.</returns>
	bool IsAutoInProcess() const;

	/// <summary>
	/// Sets whether there is an automatic PersonalBanker NCEP out transfer pending.
	/// </summary>
	/// <param name="autoNcepOutPending">IN - true to indicate there is an automatic PersonalBanker NCEP out transfer pending, false otherwise.</param>
	void SetAutoUploadPending(bool autoNcepOutPending);

	/// <summary>
	/// Returns true if an automatic PersonalBanker NCEP out transfer is pending.
	/// </summary>
	/// <returns>true if an automatic PersonalBanker NCEP out transfer is pending.</returns>
	bool IsAutoUploadPending() const;

	/// <summary>
	/// Sets the automatic started once value.
	/// </summary>
	/// <param name="autoStartedOnce">IN - the new automatic started once value.</param>
	void SetAutoStartedOnce(bool autoStartedOnce);

	/// <summary>
	/// Returns true if automatic started once is set to true, false otherwise.
	/// </summary>
	/// <returns>true if automatic started once is set to true, false otherwise.</returns>
	bool IsAutoStartedOnce() const;

	/// <summary>
	/// Sets the start upload over value.
	/// </summary>
	/// <param name="startUploadOver">IN - the new start upload over value.</param>
	void SetStartUploadOver(bool startUploadOver);

	/// <summary>
	/// Returns true if start upload over is set to true, false otherwise.
	/// </summary>
	/// <returns>true if start upload over is set to true, false otherwise.</returns>
	bool IsStartUploadOver() const;

	/// <summary>
	/// Creates a new default CCard instance that is used for knowing what player has previously carded out and what player gets the promo credits uploaded.	
	/// </summary>
	/// <returns>Returns a pointer to the new private CCard instance.</returns>
	CCard* CreateCard();

	/// <summary>
	/// if card is not NULL, creates a copy of card that is used for knowing what player has previously carded out and what player gets the promo credits uploaded.
	/// Otherwise, nothing is done.
	/// </summary>
	/// <param name="card">IN - pointer to  of destination buffer.</param>
	/// <returns>Returns a pointer to the CCard instance.</returns>
	CCard* CreateCard(const CCard* card);

	/// <summary>
	/// Returns a pointer to the private CCard instance that is used for knowing what player has previously carded out and what player gets the promo credits uploaded.	
	/// </summary>
	/// <returns>Returns a pointer to the private CCard instance if one exists, NULL otherwise.</returns>
	CCard* GetCard();	


	/// <summary>
	/// Sets the upload timer to expired.
	/// </summary>
	void SetUploadTimerExpired();

	/// <summary>
	/// Sets the upload timer to expire in delaySeconds.
	/// </summary>
	/// <param name="tickCount">IN - the current tick count, generally from ::GetTickCount().</param>
	/// <param name="delayPeriod">IN - number of milliseconds until the upload timer will expire.</param>
	void SetUploadDelay(DWORD tickCount, DWORD delayPeriod);

	/// <summary>
	/// Returns true if the upload timer is expired, false otherwise.
	/// </summary>
	/// <param name="tickCount">IN - The current tick count, generally from ::GetTickCount().</param>
	/// <returns>true if the the upload timer is expired, false otherwise.</returns>
	bool IsUploadTimerExpired(DWORD tickCount) const;

	/// <summary>
	/// Sets the NCEP out amount for the pending automatic PersonalBanker transfer -- in minor units of the currency.
	/// </summary>
	void SetPendingAutoNcepAmount(DWORD amount);

	/// <summary>
	/// Returns the NCEP out amount for the pending automatic PersonalBanker transfer, if there is a pending automatic PersonalBanker transfer  -- in minor units of the currency.
	/// If there is no pending automatic PersonalBanker transfer then the return value is undefined.
	/// </summary>
	/// <returns>The NCEP out amount for the pending automatic PersonalBanker transfer.</returns>
	DWORD GetPendingAutoNcepAmount() const;

	/// <summary>
	/// Returns true if an automatic PersonalBanker upload is in progress, false otherwise.
	/// </summary>
	/// <returns>true if an automatic PersonalBanker upload is in progress, false otherwise.</returns>
	bool IsAutoUploadInProgress() const;

	void SetPendingAutoWatAmount(DWORD amount);
	DWORD GetPendingAutoWatAmount() const;
	void SetPendingAutoCepAmount(DWORD amount);
	DWORD GetPendingAutoCepAmount() const;

	DWORD GetPendingCashToAutoUpload() const;

	/// <summary>
	/// Deletes card info associated with auto upload.
	/// </summary>
	void DeleteCard();
// #pragma endregion public functions

private:
// #pragma region private variables
	bool m_pbtAutoInProcess;		// true when a PBT response from SCF should be treated as a NCEP Out auto upload
	bool m_pbtAutoUploadPending;	// true if internals needs to do an NCEP Out at m_pbtUploadTime.
	bool m_pbtAutoStartedOnce;		// true when there is positive promo balance and auto upload has been tried
	bool m_startUploadOver;
	CCard * m_pbtOrigCard;			// Used for knowing what player has previously carded out and what player gets the promo credits uploaded.
	DWORD m_pbtUploadTimerStart;			// In ticks. When player cards out, delay promo credit upload 3 seconds
	DWORD m_pbtUploadTimerPeriod; // In ticks.
	DWORD m_pendingAutoNCEPAmount;
	DWORD m_pendingAutoWATAmount;
	DWORD m_pendingAutoCEPAmount;
// #pragma endregion private variables

// #pragma region private functions
	void CleanUp();
	void CopyObj(const CPersonalBankerAutoUpload& rhs);
// #pragma endregion private functions
};

