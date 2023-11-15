#pragma once
#include "PollerMessage.h"
#include "../PersonalBankerInfo.h"

class CPollerPersonalBankerResponseMessage :
	public CPollerMessage
{
public:
	// #pragma region constructors
	/// <summary>
    /// Response data constructor.This is made to facilitate unit 
    /// testing 
	/// </summary>
	/// <param name="response">IN - pointer to response data. Response data MUST be valid.</param>
    /// <param name="packetVersion">IN - Packet version.</param> 
    /// <param name="Datalen">IN - Data
    ///     length.</param>
	CPollerPersonalBankerResponseMessage(const byte *response, WORD packetVersion, WORD Datalen);

    // #pragma endregion constructors

// #pragma region destructor
	/// <summary>
	/// Destructor.
	/// </summary>
	~CPollerPersonalBankerResponseMessage(void);
// #pragma endregion destructor

// #pragma region public functions
	/// <summary>
	/// Returns a pointer to a copy of the CPersonalBankerInfo for this instance.
	/// </summary>
	/// <returns>A pointer to a copy of the CPersonalBankerInfo for this instance.</returns>
	CPersonalBankerInfo * GetPBInfo();
// #pragma endregion public functions

private:
// #pragma region private variables
	CPersonalBankerInfo m_pbInfo;
// #pragma endregion private variables
};
