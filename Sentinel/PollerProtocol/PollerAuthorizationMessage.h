#pragma once
#include "PollerMessage.h"

class CPollerAuthorizationMessage :
	public CPollerMessage
{
public:
// #pragma region constructor
	/// <summary>
	/// Response data constructor.
	/// </summary>
	/// <param name="response">IN - pointer to response data.  Response data MUST be valid.</param>
	CPollerAuthorizationMessage(const byte *response);
// #pragma endregion constructor

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	~CPollerAuthorizationMessage(void);
// #pragma endregion destructor

// #pragma region public functions
	/// <summary>
	/// Returns true if the PersonalBanker transfer is authorized, false otherwise.
	/// </summary>
	/// <returns>true if the PersonalBanker transfer is authorized, false otherwise.</returns>
	bool IsAuthorized(void);

	/// <summary>
	/// Returns the transaction ID for the PersonalBanker transfer.
	/// </summary>
	/// <returns>The transaction ID for the PersonalBanker transfer.</returns>
	std::string GetTransactionID(void);
// #pragma endregion public functions

private:
// #pragma region private variables
	int	 m_pbtIDLength;
	char m_pbtID[MAX_PBID_LENGTH];
	byte m_authorizationGranted;
// #pragma endregion private variables

// #pragma region private functions
	void SetTransactionID(const char* pbID, int length);
// #pragma endregion private functions
};
