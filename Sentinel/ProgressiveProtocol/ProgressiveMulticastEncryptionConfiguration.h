#pragma once
#include "stdafx.h"
#include "InternalMsgContent.h"
#include "ProgressiveKeys.h"

class CProgressiveMulticastEncryptionConfiguration
{
public:
	/// <summary>
	/// CProgressiveMulticastEncryptionConfiguration json deserialize constructor.
	/// </summary>
	/// <param name="jsonDoc">The json document to use for deserialization.</param>
	CProgressiveMulticastEncryptionConfiguration(const rapidjson::Document& jsonDoc);

	/// <summary>
	/// Gets the current encryption seed
	/// </summary>
	/// <returns>player id</returns>
	std::string GetCurrentEncryptionSeed() const;

	/// <summary>
	/// Gets the previous encryption seed
	/// </summary>
	/// <returns>player id</returns>
	std::string GetPreviousEncryptionSeed() const;

private:
	std::string m_currentEncryptionSeed;
	std::string m_previousEncryptionSeed;
};
