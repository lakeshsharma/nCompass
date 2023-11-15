#include "stdafx.h"
#include "ProgressiveMulticastEncryptionConfiguration.h"
#include "RapidJsonHelper.h"

using namespace std;
using namespace rapidjson;

CProgressiveMulticastEncryptionConfiguration::CProgressiveMulticastEncryptionConfiguration(const Document& jsonDoc)
	: m_currentEncryptionSeed(JsonGetStringEx(jsonDoc, pKeyMulticastCurrentSeed)),
	  m_previousEncryptionSeed(JsonGetStringEx(jsonDoc, pKeyMulticastPreviousSeed))
{
}

string CProgressiveMulticastEncryptionConfiguration::GetCurrentEncryptionSeed() const
{
	return m_currentEncryptionSeed;
}

string CProgressiveMulticastEncryptionConfiguration::GetPreviousEncryptionSeed() const
{
	return m_previousEncryptionSeed;
}